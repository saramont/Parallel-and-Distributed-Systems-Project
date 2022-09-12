#include <iostream>
#include <unordered_set>
#include <vector>
#include <atomic>
#include <stdio.h>
#include <ff/ff.hpp>
#include <ff/pipeline.hpp>
#include <ff/farm.hpp>

#include "utimer.cpp"
#include "ntimer.cpp"
#include "Graph.cpp"
#include "delay.cpp"

using namespace ff;
using namespace std;

atomic<int> total(0);

struct Emitter: ff_node_t<vector<int>, int> { 

    vector<int> cur_layer;
    vector<int> next_layer;
    int finished=0;
    int nw;
    int val;

    Graph* gp;

    Emitter(int value, int start_node, int n, Graph * graph) {
        cur_layer.push_back(start_node);
        gp = graph;
        nw = n;
        val = value;
    }

    int* svc(vector<int>* task) {

        if (task == nullptr) {
            for(int node_id : cur_layer) {
                ff_send_out(new int(node_id));
            }
            return GO_ON;
        }

	//{
	//ntimer time_process_result("time to process result ");
        vector<int> &t = *task;
	//{
	//ntimer time_process_result("time process result");  
        for (int e : t) {
            if (gp->get_already_seen(e) == 0) {
                next_layer.push_back(e);
                gp->set_already_seen(e);
            }
        }
	//} //ntimer
        delete task;
        finished++;
	//} //ntimer
        if (finished == cur_layer.size()) {
            // passaggio a layer successivo
            if (next_layer.size() == 0) return EOS;
            swap(cur_layer, next_layer);
            next_layer.clear();
            for(int node_id : cur_layer) {
                ff_send_out(new int(node_id));
            }
            finished = 0;
        }
        return GO_ON;  
               
    }

    void svc_end() {}
};

struct Worker: ff_node_t<int, vector<int>> {

    Graph* gp;
    vector<int> my_newnodes;
    int val;

    Worker(Graph *graph, int value) {
        gp = graph;
        val = value;
    }
    
    vector<int>* svc(int * task) { 
	//{
	//ntimer analyze_time("time node analysis");
        my_newnodes.clear();
        int &u = *task;
        if (gp->get_node_value(u) == val) total++; 
       	// delay
       	//delay(chrono::microseconds(30));
       	
        vector<int> adjs = gp->get_adj_list(u);
        for (int i=0; i<adjs.size(); i++) {
            int v = adjs[i];
            if (gp->get_node_color(v) == 0) {  
                gp->set_node_color(v, 1);
                my_newnodes.push_back(v);
            }
        }
        gp->set_node_color(u, 2);

        ff_send_out(new vector<int>(my_newnodes)); 
        delete task;
	//}//ntimer
        return GO_ON; 
    }
};

int main(int argc, char *argv[]) {   
 
    if (argc != 5) {
        std::cerr << "use: " << argv[0]  << " filename nw value start_node\n";
        return 0;
    }

    string filename = argv[1];
    const size_t nworkers = stol(argv[2]);
    int val = atoi(argv[3]);
    int start_node = atoi(argv[4]);

    Graph g = Graph(filename);
    if (g.get_tot_nodes() == -1) {
        cout << "error reading the graph" << endl;
        return 0;
    }

    Emitter e(val, start_node, nworkers, &g);

    vector<unique_ptr<ff_node> > W;
    for(size_t i=0;i<nworkers;++i) W.push_back(make_unique<Worker>(&g, val));

    ff_Farm<> farm(std::move(W), e);  
    farm.remove_collector(); 
    farm.wrap_around();   
    {
        utimer tff("");
        if (farm.run_and_wait_end()<0) {
            error("running farm");
            return -1;
        }
    }
    cout << "TOT " << total << endl;

    return 0;
}
