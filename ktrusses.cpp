#include <iostream>
#include <string.h>
#include <unordered_map>
#include <unordered_set>
#include <vector>
#include <queue>
using namespace std;

// manually add a graph, will be replaced with real data
void get_graph(vector<vector<int>> &edges, unordered_set<int> &graph,
                unordered_map<int, unordered_set<int>> &node_neis, unordered_map<int, unordered_set<int>> &node_edges){
    
    // edges in graph
    int edges_org[5][2] = {{1,2}, {1,5}, {2,3}, {2,5}, {3,4}};
    for(int i = 0; i < sizeof(edges_org); i++){
        vector<int> tmp_edge;
        tmp_edge.push_back(edges_org[i][0]);
        tmp_edge.push_back(edges_org[i][1]);
        edges.push_back(tmp_edge);
    }
    // can't figure out why, there are more elements in the vector ???
    edges.resize(5);

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

// simple print of the graph
void print_graph(unordered_set<int> &graph, vector<vector<int>> edges){
    for(int edge_id : graph){
        cout << "Edge" << edge_id << " : " << edges[edge_id][0] << " , " << edges[edge_id][1] << endl;
    }
    cout << "\n" << endl;
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
void remove_edges(int k){
    // edges in graph
    vector<vector<int>> edges;
    // edge index in graph
    unordered_set<int> graph;
    // node & neighbours
    unordered_map<int, unordered_set<int>> node_neis;
    // node & edges
    unordered_map<int, unordered_set<int>> node_edges;
    // queue for edges to be removed
    queue<int> remove_queue;
    // support count for each edge
    unordered_map<int, int> count;

    // get graph data
    get_graph(edges, graph, node_neis, node_edges);

    // print original graph
    cout << "Original graph: " << endl;
    print_graph(graph, edges);
    
    // update supports count for each edge
    for(int i = 0; i < edges.size(); i++){
        count.insert({i, 0});
    }
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

        if(count.find(i)->second < k - 2){
            remove_queue.push(i);
        }
    }

    // iteratively remove all edges with support less than k - 2
    while(!remove_queue.empty()){
        // remove currend edge
        int cur_edge = remove_queue.front();
        remove_queue.pop();
        int a = edges[cur_edge][0];
        int b = edges[cur_edge][1];
        remove(cur_edge, a, b, node_edges, node_neis, graph);
        
        // put others into the remove queue, if possible
        unordered_set<int> nei_a = node_neis.find(a)->second;
        unordered_set<int> nei_b = node_neis.find(b)->second;
        for(int v : nei_a){
            if(nei_b.find(v) != nei_b.end()){
                unordered_set<int> edge_v = node_edges.find(v)->second;
                for(int edge : edge_v){
                    if(edges[edge][0] == a || edges[edge][0] == b || edges[edge][1] == a || edges[edge][1] == b){
                        int ct = --(count.find(edge)->second);
                        if(ct < k - 2){
                            remove_queue.push(edge);
                        }
                    }
                }
            }
        }
    }
    
    cout << "Reduced k-trusses graph: " << endl;
    print_graph(graph, edges);

}

int main(){
    remove_edges(3);
    return 0;
}
