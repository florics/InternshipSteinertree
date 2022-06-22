//
// Created by Lukas on 14.06.2022.
//

#include "iostream"

#include "graph_printfunctions.h"

#include "graph_aux_functions.h"

//todo Funktionennamen aktualisieren?

void print_graph(const Graph& input_graph) {

    std::cout << "SECTION Graph \n";
    std::cout << "Nodes " << input_graph.num_nodes() << "\n";
    std::cout << "Edges " << input_graph.num_edges() << "\n";
    for(auto var_edge : input_graph.edges()){
        std::cout << "E ";
        print_nodename(input_graph, var_edge.node_a());
        std::cout << " ";
        print_nodename(input_graph, var_edge.node_b());
        std::cout << " ";
        print_edgeweight(var_edge.weight() );
        std::cout << "\n";
    }
    std::cout << "\n";

    std::cout<< "SECTION Terminals \n";
    std::vector<Graph::NodeId> vect_term = input_graph.get_vect_term();
    std::cout << "Terminals " << vect_term.size() << "\n";
    for (unsigned int id : vect_term){
        std::cout << "T ";
        print_nodename(input_graph, id);
        std::cout << "\n";
    }
    std::cout << "\n";

}

void print_graph_by_id(const Graph& input_graph){
    std::cout << "print_by_id \n";

    std::cout << "SECTION Graph \n";
    std::cout << "Nodes " << input_graph.num_nodes() << "\n";
    std::cout << "Edges " << input_graph.num_edges() << "\n";
    for (auto var_edge : input_graph.edges()){
        std::cout << "E ";
        print_nodeid( var_edge.node_a() );
        std::cout << " ";
        print_nodeid( var_edge.node_b() );
        std::cout << " ";
        print_edgeweight( var_edge.weight() );
        std::cout << "\n";
    }
    std::cout << "\n";

    std::cout<< "SECTION Terminals \n";
    std::vector<Graph::NodeId> vect_term = input_graph.get_vect_term();
    std::cout << "Terminals " << vect_term.size() << "\n";
    for (unsigned int id : vect_term){
        std::cout << "T ";
        print_nodeid(id);
        std::cout << "\n";
    }
    std::cout << "\n";

}

void print_nodename(const Graph& input_graph, Graph::NodeId id){
    if(id == Graph::invalid_node_id){
        std::cout << " 'invalid_node' ";
    }else if(input_graph.num_nodes() <= id) {
        throw std::runtime_error("(Graph::print_nodename) Knoten mit NodeId id liegt nicht im Graph");
    }else{
        std::cout << input_graph.get_node(id).node_name();
    }
}

void print_edge_as_pair(const Graph & input_graph, Graph::EdgeId id) {
    if(id < input_graph.num_edges()) {
        std::cout << "(";
        print_nodename( input_graph, input_graph.get_edge(id).node_a() ),
                std::cout << ",";
        print_nodename( input_graph, input_graph.get_edge(id).node_b() ),
                std::cout << ")";
    } else if(id == Graph::invalid_edge_id) {
        std::cout << " 'invalid_edge' ";
    } else {
        throw std::runtime_error("(Graph::print_edge_as_pair) Kante mit EdgeId id nicht im Graph");
    }
}

//? nicht getestet
void print_incidence_edges(const Graph& input_graph, Graph::NodeId id) {
    const Graph::Node& v = input_graph.get_node(id);
    std::cout << "Knoten ";
    print_nodename(input_graph, id);
    std::cout << " ist zu " << v.num_neighbors() << " Kanten inzident: \n";
    for(auto curr_edgeid : v.incident_edge_ids() ){
        Graph::Edge curr_edge = input_graph.get_edge(curr_edgeid);
        std::cout << "E ";
        print_nodename(input_graph, curr_edge.node_a() );
        std::cout << " ";
        print_nodename(input_graph, curr_edge.node_b() );
        std::cout << " ";
        print_edgeweight(curr_edge.weight() );
        std::cout << "\n";
    }
}

//? nicht getestet
void print_incident_edges_using_ids(const Graph& input_graph, Graph::NodeId id) {
    const Graph::Node& v = input_graph.get_node(id);
    std::cout << "Knoten mit NodeId";
    print_nodeid(id);
    std::cout << " ist zu " << v.num_neighbors() << " Kanten inzident: (hier werden die _node_id's verwendet) \n";
    for(auto curr_edge_id : v.incident_edge_ids() ){
        Graph::Edge curr_edge = input_graph.get_edge(curr_edge_id);
        std::cout << "E ";
        print_nodeid( curr_edge.node_a() );
        std::cout << " ";
        print_nodeid( curr_edge.node_b() );
        std::cout << " ";
        print_edgeweight( curr_edge.weight() );
        std::cout << "\n";
    }
}

void print_all_incidence_vect_using_ids(const Graph& input_graph) {
    for(Graph::NodeId i=0; i<input_graph.num_nodes(); i++){
        print_incident_edges_using_ids(input_graph, i);
    }
}

void print_ingoing_neighbors(const Graph& input_graph, Graph::NodeId input_node_id) {
    std::cout << "Die eingehenden Nachbarn von Knoten ";
    print_nodeid(input_node_id);
    std::cout << " sind: \n";
    for( auto curr_neighbor_id : input_graph.get_ingoing_neighbor_ids(input_node_id)) {
        print_nodename(input_graph, curr_neighbor_id );
        std::cout << "\n";
    }
}

void print_outgoing_neighbors(const Graph& input_graph, Graph::NodeId input_node_id) {
    std::cout << "Die ausgehenden Nachbarn von Knoten ";
    print_nodeid(input_node_id);
    std::cout << " sind: \n";
    for( auto curr_neighbor_id : input_graph.get_outgoing_neighbor_ids(input_node_id)) {
        print_nodename(input_graph, curr_neighbor_id );
        std::cout << "\n";
    }
}

void print_nodeid(Graph::NodeId id){
    if(id != Graph::invalid_node_id){
        std::cout << id;
    }else{
        std::cout << " 'invalid_node' ";
    }
}

void print_edgeid(Graph::EdgeId id){
    if(id != Graph::invalid_edge_id){
        std::cout << id;
    }else{
        std::cout << " 'invalid_edge' ";
    }
}

void print_pathlength(Graph::PathLength l){
    if(l != Graph::infinite_length){
        std::cout << l;
    }else{
        std::cout << " 'infinite_length' ";
    }
}

void print_edgeweight(Graph::EdgeWeight w){
    if(w != Graph::infinite_weight){
        std::cout << w;
    }else{
        std::cout << " 'infinite_weight' ";
    }
}

void print_length_of_all_edges(const Graph& input_graph) {
    Graph::PathLength output = length_of_all_edges(input_graph);
    std::cout << "Das Gewicht des Graphen ist ";
    print_pathlength( output );
    std::cout << "\n";
}