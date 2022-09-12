#include <iostream>
#include <vector>
#include <queue>
#include <thread>
#include <set>
#include <atomic>
#include <mutex>
#include <condition_variable>
#include <stdio.h>

#include "utimer.cpp"
#include "ntimer.cpp"
#include "Graph.cpp"
#include "delay.cpp"

using namespace std;

//#define SEQ 0

mutex m;
mutex mf;
mutex md;
condition_variable restart;
bool end_iter = false;
int count_ready = 0;

//---------------------------------
//vector<int> stats;
//atomic<int> total(0);
//---------------------------------


void mex_print(int tid, string s) {
    lock_guard<mutex> lock(md);
    cerr << "TH " << tid << ": " << s << endl;
}

void thread_function (int n, int tid, int val, vector<vector<int>> &cur_layer, vector<vector<int>> &next_layer, int &cur_layer_tot, Graph &g, vector<int> &final_result) {

    int range_start;
    int range_end;
    int tot_mynodes;
    bool have_work;

    int tot=0;
    int start_vect;
    int start_pos;

    vector<int> my_result; // per ora è un set, magari poi cambia...

    while (!end_iter) { 
      //  {
      //  ntimer compute_range_time("compute range (" +to_string(tid) + ")");
        // 1. calcolo dei range
        if (cur_layer_tot < n) { // ho più thread che nodi nel layer da processare
            if (cur_layer_tot > tid) {
                range_start = tid;
                range_end = tid+1;
                tot_mynodes = range_end-range_start;
                have_work = true;
            }
            else {
                have_work = false;
            }
        }
        else { // ho più nodi da processare che thread 
            have_work = true;
            int m = cur_layer_tot / n;
            range_start = tid * m;
            range_end = (tid+1) * m;
            if ( (tid == n-1) && (range_end > cur_layer_tot || range_end < cur_layer_tot) )  // ultimo thread
                range_end = cur_layer_tot;

            tot_mynodes = range_end-range_start;
        }
      //  }// ntimer compute range

        if (have_work) {
            // 2. calcolo del punto da cui iniziare a leggere cur_layer
          //  {
          //  ntimer compute_start("compute start (" +to_string(tid) + ")");
            tot = 0;
            for (int i=0; i<n; i++) {
                tot = tot + cur_layer[i].size();
                if (tot > range_start) {
                    start_vect = i;
                    start_pos = range_start - tot + cur_layer[i].size();
                    break;
                }
            }
          //  } // fine blocco ntimer compute_start
         //   {
         //   ntimer time_explore ("explore (" +to_string(tid) + ")");
            // 3. lettura e analisi della parte di nodi assegnata al thread
            int remaining = tot_mynodes;
            int vect = start_vect;
            int pos = start_pos;

            while (remaining) {
                while (pos == cur_layer[vect].size()) {
                    vect++;
                    pos = 0;
                }
                int u = cur_layer[vect][pos];               
                if (g.get_node_value(u) == val) my_result.push_back(u); //total++;
                vector<int> adjs = g.get_adj_list(u);
                for (int i=0; i<adjs.size(); i++) {
                    int v = adjs[i];
                    if (g.get_node_color(v) == 0) {  // più di un thread potrebbe trovare vero questo in contemporanea
                        g.set_node_color(v, 1);
                        next_layer[tid].push_back(v);
                    }
                }
                g.set_node_color(u, 2);

                pos++;
                remaining--;
            }
       //     }//ntimer
        }

        // 4. fine analisi del range del thread, 
        // deve segnalare che ha finito,
        // deve aspettare che gli altri finiscono e poi ricominciare con il prossimo layer,
        // l'ultimo thread esegue il setup della prossima iterazione
        {
            unique_lock<mutex> lk(m);
            count_ready++; 
            if (count_ready == n) {
                // STATISTICS --------------------------------------
                /*
                cerr << "END ITER" << endl;
                for (int el : stats) el = 0;
                for (int i=0; i<n; i++)
                    for (int el : next_layer[i])
                        stats[el]++; 
                for (int i=0; i<stats.size(); i++)
                    if (stats[i] >= 2) cerr << i << ": " << stats[i] << endl;
                    */
                //--------------------------------------------------
               // {
               // ntimer time_sync("time sync (" +to_string(tid) + ")");
                //setup prossima iterazione
                swap(cur_layer, next_layer);
                end_iter = true;
                cur_layer_tot = 0;
                for (int i=0; i<n; i++) {
                    next_layer[i].clear();
                    cur_layer_tot = cur_layer_tot + cur_layer[i].size();
                    if (cur_layer[i].size() != 0) end_iter = false;
                }
                count_ready = 0;
                restart.notify_all();
             //   }//ntimer
            }
            else {
                // aspetta
                restart.wait(lk);
            }
        }
    } // end-while iniziale

    // 5. aggiornare il risultato finale
    mf.lock();
    //{
    //ntimer time_final_res("update final res (" +to_string(tid) + ")");
    for (int n : my_result) {
        if (g.get_already_seen(n) == 0) {
            g.set_already_seen(n);
            final_result.push_back(n);
        }
    }
    //}//ntimer
    mf.unlock();

    return;
}


int main (int argc, char * argv[]) {

#if SEQ 
// bfs sequenziale

    if (argc != 4) {
        cout << "usage: " << argv[0] << " filename value start_node" << endl; 
        return 0;
    }

    string filename = argv[1];
    Graph g = Graph(filename);
    if (g.get_tot_nodes() == -1) {
        cout << "error reading the graph" << endl;
        return 0;
    }

    int found = 0;
    int val = atoi(argv[2]);
    int start_node = atoi(argv[3]);
    queue<int> q;

    g.set_node_color(0, 1);
    q.push(start_node);

    {
        utimer tseq("");
        while (q.size() != 0) {
            int u = q.front();
            q.pop();
            //{
               // ntimer time_node_visit("time node visit");
                if (g.get_node_value(u) == val) found++;

                vector<int> adjs = g.get_adj_list(u);
                for (int i=0; i<adjs.size(); i++) {
                    int v = adjs[i];
                    if (g.get_node_color(v) == 0) {
                        g.set_node_color(v, 1);
                        q.push(v);
                    }
                }
                g.set_node_color(u, 2);
           // }//ntimer
        }
    }
    //cout << endl;
    cout << "TOT " << found << endl;

#else 
// bfs parallela
    if (argc != 5) {
        cout << "usage: " << argv[0] << " filename nw value start_node" << endl; 
        return 0;
    }
    string filename = argv[1];
    int val = atoi(argv[3]);
    int start_node = atoi(argv[4]); 
    int n = atoi(argv[2]); // numero di thread

    Graph g = Graph(filename);
    if (g.get_tot_nodes() == -1) {
        cout << "error reading the graph" << endl;
        return 0;
    }

    // STATISTICS -------------------------------------------------------------------
    //stats.resize(g.get_tot_nodes(), 0);
    //-------------------------------------------------------------------------------

    vector<vector<int>> cur_layer(n);
    vector<vector<int>> next_layer(n);
    for (int i=0; i<n; i++) {
        vector<int> v;
        cur_layer[i] = v;
        next_layer[i] = v;
    }

    cur_layer[0].push_back(start_node);
    int cur_layer_tot = 1; 

    vector<int> final_result;
    vector<thread*> tids(n);
    {
        utimer tpar("");
        for (int i=0; i<n; i++) tids[i] = new thread(thread_function, n, i, val, ref(cur_layer), ref(next_layer), ref(cur_layer_tot), ref(g), ref(final_result));
        for(int i=0; i<n; i++) tids[i]->join(); 
    }
    cout << "TOT " << final_result.size() << endl;

#endif

    return 0;
}
