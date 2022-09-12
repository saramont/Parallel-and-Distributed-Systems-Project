#include <iostream>
#include <unordered_map>
#include <vector>
#include <utility>
#include <stdio.h>
#include <fstream>

using namespace std;

#define MIN_CHILDREN 15
#define MAX_CHILDREN 15
#define MIN_RANKS 4    
#define MAX_RANKS 4
#define PERCENT 15    // Chance of having an Edge.  


int main (int argc, char * argv[]) {

    int i, j, k = 0;
    int prev_rank_nodes = 1;
    int nodes = 1;
    unordered_map<int, vector<int>> adj_lists; // key = node_id, value = adj list
    srand (time (NULL));

    int ranks = MIN_RANKS + (rand () % (MAX_RANKS - MIN_RANKS + 1));

    //ofstream dotfile;
    //dotfile.open("digraph.dot", ios::out);
    //dotfile << "digraph {" << endl;
    //printf ("digraph {\n");
    for (i = 0; i < ranks; i++)
    {
        cout << "rank " << i << endl;
        int new_rank_nodes = 0;
        for (j = 0; j < prev_rank_nodes; j++) 
        {
            int father = j + nodes - prev_rank_nodes;
            int new_nodes = MIN_CHILDREN + (rand () % (MAX_CHILDREN - MIN_CHILDREN + 1)); // per ogni nodo del rank precedente genero i suoi figli            
            for (k = 0; k < new_nodes; k++) // archi tra j e i suoi figli
            {
                int child = k + nodes + new_rank_nodes;
                //dotfile << " " << father << " -> " << child << ";" << endl;
                //printf ("  %d -> %d;\n", father, child); // An Edge.
                if (adj_lists.count(father)) // node j already has edges to other nodes
                    adj_lists[father].push_back(child);
                else { // this is the first edge for node j
                    vector<int> v (1, child);
                    pair<int, vector<int>> p (father, v);
                    adj_lists.insert(p);
                }

                for (int t = 0; t < nodes; t++) 
                {
                    int x = (rand () % 100);
                    if ( x < PERCENT && t != father) {   
                        // insert edge in map
                        //dotfile << " " << t << " -> " << child << ";" << endl;
                        //printf ("  %d -> %d;\n", t, child); // An Edge.  
                        if (adj_lists.count(t)) // node j already has edges to other nodes
                            adj_lists[t].push_back(child);
                        else { // this is the first edge for node j
                            vector<int> v (1, child);
                            pair<int, vector<int>> p (t, v);
                            adj_lists.insert(p);
                        }
                    }                 
                }
            }
            new_rank_nodes += new_nodes;
        }
        prev_rank_nodes = new_rank_nodes;
        nodes += new_rank_nodes;
    }
    //printf ("}\n");
    //dotfile << "}" << endl;
    //dotfile.close();

    cout << "tot nodes: " << nodes << endl;
/*
    cout << "printing map:" << endl;
    for (pair<int, vector<int>> p : adj_lists) {
        vector<int> list = get<1>(p);
        cout << get<0>(p) << ": ";
        for (int n : list) {
            cout << n << " ";
        }
        cout << endl;
    }
*/

    // write file for bfs program
    int num_nodes = nodes;
    FILE * pFile;
    pFile = fopen ("digraph","w");
    if (pFile==NULL)
    {
        cout << "error opening file" << endl;
        return 0;
    }
    fwrite(&num_nodes, sizeof(int), 1, pFile);

    vector<int> values;
    values.resize(num_nodes);
    //cout << "values: ";
    for (int i=0; i<num_nodes; i++) {
        values[i] = rand() % 5; // nodes have values in [0,5)
        //cout << values[i] << " ";
    }
    //cout << endl;
    fwrite(&values[0], sizeof(int), num_nodes, pFile);

    for (pair<int, vector<int>> p : adj_lists) {
        int node_id = get<0>(p);
        fwrite(&node_id, sizeof(int), 1, pFile);
        vector<int> list = get<1>(p);
        int num_adj = list.size();
        fwrite(&num_adj, sizeof(int), 1, pFile);
        fwrite(&list[0], sizeof(int), num_adj, pFile);
    }
    fclose(pFile);

    return 0;
}

/*
file structure:
num_nodes [lista di num_nodes valori, uno per ogni nodo] node_id size_adj adj_list node_id size_adj adj_list ... node_id size_adj adj_list
*/
