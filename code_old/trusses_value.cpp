#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <vector>
#include <queue>
#include <time.h>
using namespace std;

// split string
void split(string &src, char spt, vector<int> &dest){
    stringstream sstrm(src);
    string tmp;
    dest.clear();
    while(getline(sstrm, tmp, spt)){
        dest.push_back(stoi(tmp));
    }
}

// read graph from .txt file, which contains edges
void read_graph(string name, vector<vector<int>> &edges, unordered_set<int> &graph,
                unordered_map<int, unordered_set<int>> &node_neis, unordered_map<int, unordered_set<int>> &node_edges){
    
    // read file
    ifstream reader;
    reader.open(name, ios::in);
    if(reader.fail()){
        cout << "Can't read the graph file..." << endl;
        return;
    }
    
    // edges in graph
    int count = 0;
    string tmp_str;
    while(getline(reader, tmp_str)){
        vector<int> tmp_edge;
        split(tmp_str, ' ', tmp_edge);
        edges.push_back(tmp_edge);
        count++;
    }

    // close file reader
    reader.close();
    
    // can't figure out why, there are more elements in the vector ???
    edges.resize(count);

    // set of edge index in graph
    for(int i = 0; i < edges.size(); i++){
        graph.insert(i);
    }

    // map of node to its neighbours and to its edges
    for(int i = 0; i < edges.size(); i++){
        int a = edges[i][0];
        int b = edges[i][1];

        auto itr1 = node_neis.find(a);
        if(itr1 == node_neis.end()){
            unordered_set<int> tmp_set;
            tmp_set.insert(b);
            node_neis.insert({a, tmp_set});
        }else{
            itr1->second.insert(b);
        }

        auto itr2 = node_neis.find(b);
        if(itr2 == node_neis.end()){
            unordered_set<int> tmp_set;
            tmp_set.insert(a);
            node_neis.insert({b, tmp_set});
        }else{
            itr2->second.insert(a);
        }

        auto itr3 = node_edges.find(a);
        if(itr3 == node_edges.end()){
            unordered_set<int> tmp_set;
            tmp_set.insert(i);
            node_edges.insert({a, tmp_set});
        }else{
            itr3->second.insert(i);
        }

        auto itr4 = node_edges.find(b);
        if(itr4 == node_edges.end()){
            unordered_set<int> tmp_set;
            tmp_set.insert(i);
            node_edges.insert({b, tmp_set});
        }else{
            itr4->second.insert(i);
        }
    }

}

// write the index of edge, and its truss value
void write_graph(string name, unordered_map<int, int> &truss_value){
    int count[truss_value.size()];
    for(auto itr = truss_value.begin(); itr != truss_value.end(); itr++){
        count[itr->first] = itr->second;
    }
    
    // make output file, and write
    ofstream outfile;
    outfile.open(name);
    
    for(int i = 0; i < truss_value.size(); i++){
        outfile << to_string(i) + " " + to_string(count[i]) + "\n";
    }

    outfile.close();
}

// remove:
//    1. edge index from graph
//    2. edge index from node a's edges
//    3. edge index from node b's edges
//    4. neighbor b from node a's neighbors
//    5. neighbor a from node b's neighbors
void remove(int i, int a, int b, unordered_map<int, unordered_set<int>> &node_edges, unordered_map<int, unordered_set<int>> &node_neis, unordered_set<int> &graph){
    graph.erase(i);

    // erase edge
    unordered_set<int> *set1 = &node_edges.find(a)->second;
    (*set1).erase(i);
    unordered_set<int> *set2 = &node_edges.find(b)->second;
    (*set2).erase(i);

    // erase nei
    unordered_set<int> *set3 = &node_neis.find(a)->second;
    (*set3).erase(b);
    unordered_set<int> *set4 = &node_neis.find(b)->second;
    (*set4).erase(a);
}

// k-trusses
//    remove edges in graph to generate a k-thrusses
void get_truss_value(vector<vector<int>> &edges, unordered_set<int> &graph, unordered_map<int, unordered_set<int>> &node_neis,
        unordered_map<int, unordered_set<int>> &node_edges, unordered_map<int, int> &truss_value){
    // queue for edges to be removed
    queue<int> remove_queue;
    // support count for each edge
    unordered_map<int, int> count;

    // initialize supports count == 0 for each edge
    for(int i = 0; i < edges.size(); i++){
        count.insert({i, 0});
    }
    
    // count truss value
    for(int i = 0; i < edges.size(); i++){
        int a = edges[i][0];
        int b = edges[i][1];
        unordered_set<int> nei_a = node_neis.find(a)->second;
        unordered_set<int> nei_b = node_neis.find(b)->second;

        for(int nei : nei_a){
            if(nei_b.find(nei) != nei_b.end()){
                count.find(i)->second++;
            }
        }
    }
    
    // get the truss value for each edge
    //      1> set k = 0;
    //      2> if remove_queue is empty, refill it
    //      3> remove edge has value < k+1
    //          the removed edge is putted in truss_value with k
    //      4> k++
    //      5> continue, until graph is empty
    int k = -1;
    while(!graph.empty()){
        // update k, which is the truss value for edges removed in this loop
        k++;
        // cout << "current k value: \t" << k << endl;

        // deduplication in each loop
        unordered_set<int> dedup;

        // scan the graph, add edges to be removed
        for(int idx : graph){
            if(count.find(idx)->second < k+1){
                remove_queue.push(idx);
                dedup.insert(idx);
            }
        }

        // if have edges to be remvoed, remove them, and put truss value
        // iteratively remove all edges with support less than k+1
        while(!remove_queue.empty()){
            // remove currend edge
            int cur_edge = remove_queue.front();
            remove_queue.pop();
            int a = edges[cur_edge][0];
            int b = edges[cur_edge][1];
            remove(cur_edge, a, b, node_edges, node_neis, graph);
            truss_value.insert({cur_edge, k});
            
            // put others into the remove queue, if possible
            unordered_set<int> nei_a = node_neis.find(a)->second;
            unordered_set<int> nei_b = node_neis.find(b)->second;
            for(int v : nei_a){
                if(nei_b.find(v) != nei_b.end()){
                    unordered_set<int> edge_v = node_edges.find(v)->second;
                    for(int edge : edge_v){
                        if(dedup.find(edge) != dedup.end()){
                            continue;
                        }
                        if(edges[edge][0] == a || edges[edge][0] == b || edges[edge][1] == a || edges[edge][1] == b){
                            int ct = --(count.find(edge)->second);
                            if(ct < k+1){
                                remove_queue.push(edge);
                                dedup.insert(edge);
                            }
                        }
                    }
                }
            }
        }
    }

}

int main(int argc, char* argv[]){

    // timing
    clock_t start, end;
    start = clock();

    // check input
    if(argc != 2){
        cout << "input file error..." << endl;
    }
    
    // file names
    string name_in = argv[1];
    string name_out = name_in.substr(0, name_in.size() - 4) + "_k_truss.txt";
    
    // prepare graph related data
    // edges in graph
    vector<vector<int>> edges;
    // edge index in graph
    unordered_set<int> graph;
    // node & neighbours
    unordered_map<int, unordered_set<int>> node_neis;
    // node & edges
    unordered_map<int, unordered_set<int>> node_edges;
    // truss count for each edge, empty in initial
    unordered_map<int, int> truss_value;

    cout << "in processing ......" << endl;

    // read data from file
    read_graph(name_in, edges, graph, node_neis, node_edges);

    // main process
    get_truss_value(edges, graph, node_neis, node_edges, truss_value);

    // output k-truss value in file
    write_graph(name_out, truss_value);
    
    // timing
    end = clock();
    int minute = (end - start)/(60*CLOCKS_PER_SEC);
    int second = (end - start - minute*60*CLOCKS_PER_SEC)/(CLOCKS_PER_SEC);

    // hints
    cout << "k truss value for each edge have been written in file:\t" + name_out << endl;
    cout << "time used:\t" << minute << "(min)" << second << "(s)" << endl;

    return 0;
}
