#include <iostream>
#include <vector>
#include<mutex>

using namespace std;

class Graph
{
    // i vettori hanno dimensione n = numero di nodi
    // values: per ogni nodo il suo valore
    // colors: per ogni nodo il suo colore corrente (che si aggiorna man mano che la visita procede)
    // adj_list: per ogni nodo un vettore che contiene gli id dei nodi a lui adiacenti 
    // (l'id di un nodo è la posizione a lui corrispondente nei 3 vettori, quindi gli id vanno da 0 a n-1)
private:
    int node_num;
    vector<int> values;
    vector<int> colors;
    vector<vector<int>> adj_lists;

    vector<int> already_seen;
public:
    Graph();
    Graph(string);
    ~Graph();
    int get_tot_nodes();
    void set_node_color(int, int);
    int get_node_color(int);
    vector<int> get_adj_list(int);
    int get_node_value(int);
    int get_already_seen(int);
    void set_already_seen(int);

    void print_node_num();
    void print_values();
    void print_colors();
    void print_adj_lists();
};

Graph::Graph() {
    node_num = 0;
    values.clear();
    colors.clear();
    adj_lists.clear();
    already_seen.clear();
}

Graph::Graph(string filename) {
    FILE * pFile;
    pFile = fopen(filename.c_str(), "r");
    if (pFile==NULL)
    {
        node_num = -1;
        return;
    }
    fread(&node_num, sizeof(int), 1, pFile);
    colors.resize(node_num, 0);
    values.resize(node_num, 0);
    already_seen.resize(node_num, 0);
    vector<int> v(0);
    adj_lists.resize(node_num, v);

    // leggi i valori da assegnare ad ogni nodo
    fread(&values[0], sizeof(int), node_num, pFile);

    int node_id, size_adj;
    while (fread(&node_id, sizeof(int), 1, pFile) > 0) {
        fread(&size_adj, sizeof(int), 1, pFile);
        vector<int> list;
        list.resize(size_adj);
        fread(&list[0], sizeof(int), size_adj, pFile);
        // aggiorna strutture dati
        adj_lists[node_id] = list;
    }
    fclose(pFile);
}

Graph::~Graph()
{
    values.clear();
    colors.clear();
    adj_lists.clear();
    already_seen.clear();
}

int Graph::get_tot_nodes() {
    return node_num;
}

void Graph::set_node_color(int node_id, int color) {
    colors[node_id] = color;
}

int Graph::get_node_color (int node_id) {
    return colors[node_id];
}

vector<int> Graph::get_adj_list(int node_id) {
    return adj_lists[node_id];
}

int Graph::get_node_value(int node_id) {
    return values[node_id];
}

int Graph::get_already_seen(int node_id) {
    return already_seen[node_id];
}

void Graph::set_already_seen(int node_id) {
    already_seen[node_id] = 1;
}

// DEBUG -------------------------------------------------------------------------------
// -------------------------------------------------------------------------------------

void Graph::print_node_num() {
    cout << "node number: " << node_num << endl;
}

void Graph::print_values() {
    for (int i=0; i<values.size(); i++) {
        cout << i << ": " << values[i] << endl;
    }
}

void Graph::print_colors() {
    for (int i=0; i<colors.size(); i++) {
        cout << i << ": " << colors[i] << endl;
    }
}

void Graph::print_adj_lists() {
    for (int i=0; i<adj_lists.size(); i++) {
        cout << i << ": ";
        for (int v : adj_lists[i])
            cout << v << " ";
        cout << endl;
    }
}

// -------------------------------------------------------------------------------------
// --------------------------------------------------------------------------------