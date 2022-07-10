//
// Created by Lukas on 10.07.2022.
//

#include "unused_graph_printfunctions.h"

#include "iostream"

#include "../graph_printfunctions.h"


void GraphAuxPrint::print_nodeid(Graph::NodeId id){
    if(id != Graph::invalid_node_id){
        std::cout << id;
    }else{
        std::cout << " 'invalid_node' ";
    }
}

void GraphAuxPrint::print_edgeid(Graph::EdgeId id){
    if(id != Graph::invalid_edge_id){
        std::cout << id;
    }else{
        std::cout << " 'invalid_edge' ";
    }
}


void GraphAuxPrint::print_graph_by_id(const Graph& input_graph){
    std::cout << "print_by_id \n";

    std::cout << "SECTION Graph \n";
    std::cout << "Nodes " << input_graph.num_nodes() << "\n";
    std::cout << "Edges " << input_graph.num_edges() << "\n";
    for(const auto& var_edge : input_graph.edges()){
        std::cout << "E ";
        GraphAuxPrint::print_nodeid( var_edge.node_a() );
        std::cout << " ";
        GraphAuxPrint::print_nodeid( var_edge.node_b() );
        std::cout << " ";
        GraphAuxPrint::print_edgeweight( var_edge.weight() );
        std::cout << "\n";
    }
    std::cout << "\n";

    std::cout<< "SECTION Terminals \n";
    std::vector<Graph::NodeId> vect_term = input_graph.get_terminals();
    std::cout << "Terminals " << vect_term.size() << "\n";
    for (unsigned int id : vect_term){
        std::cout << "T ";
        GraphAuxPrint::print_nodeid(id);
        std::cout << "\n";
    }
    std::cout << "\n";

}

void GraphAuxPrint::print_incidence_edges(const Graph& input_graph, Graph::NodeId id) {
    const Graph::Node& v = input_graph.get_node(id);
    std::cout << "Knoten ";
    GraphAuxPrint::print_nodename(id);
    std::cout << " ist zu " << v.num_neighbors() << " Kanten inzident: \n";
    for(auto curr_edgeid : v.incident_edge_ids() ){
        Graph::Edge curr_edge = input_graph.get_edge(curr_edgeid);
        std::cout << "E ";
        GraphAuxPrint::print_nodename(curr_edge.node_a() );
        std::cout << " ";
        GraphAuxPrint::print_nodename(curr_edge.node_b() );
        std::cout << " ";
        GraphAuxPrint::print_edgeweight(curr_edge.weight() );
        std::cout << "\n";
    }
}

void GraphAuxPrint::print_incident_edges_using_ids(const Graph& input_graph, Graph::NodeId id) {
    const Graph::Node& v = input_graph.get_node(id);
    std::cout << "Knoten mit NodeId";
    GraphAuxPrint::print_nodeid(id);
    std::cout << " ist zu " << v.num_neighbors() << " Kanten inzident: (hier werden die _node_id's verwendet) \n";
    for(auto curr_edge_id : v.incident_edge_ids() ){
        Graph::Edge curr_edge = input_graph.get_edge(curr_edge_id);
        std::cout << "E ";
        GraphAuxPrint::print_nodeid( curr_edge.node_a() );
        std::cout << " ";
        GraphAuxPrint::print_nodeid( curr_edge.node_b() );
        std::cout << " ";
        GraphAuxPrint::print_edgeweight( curr_edge.weight() );
        std::cout << "\n";
    }
}

void GraphAuxPrint::print_all_incidence_vect_using_ids(const Graph& input_graph) {
    for(Graph::NodeId i=0; i<input_graph.num_nodes(); i++){
        GraphAuxPrint::print_incident_edges_using_ids(input_graph, i);
    }
}

void GraphAuxPrint::print_ingoing_neighbors(const Graph& input_graph, Graph::NodeId input_node_id) {
    std::cout << "Die eingehenden Nachbarn von Knoten ";
    GraphAuxPrint::print_nodeid(input_node_id);
    std::cout << " sind: \n";
    for( auto curr_neighbor_id : input_graph.get_ingoing_edge_ids(input_node_id)) {
        GraphAuxPrint::print_nodename(curr_neighbor_id );
        std::cout << "\n";
    }
}

void GraphAuxPrint::print_outgoing_neighbors(const Graph& input_graph, Graph::NodeId input_node_id) {
    std::cout << "Die ausgehenden Nachbarn von Knoten ";
    GraphAuxPrint::print_nodeid(input_node_id);
    std::cout << " sind: \n";
    for( auto curr_neighbor_id : input_graph.get_outgoing_edge_ids(input_node_id)) {
        GraphAuxPrint::print_nodename(curr_neighbor_id );
        std::cout << "\n";
    }
}

void GraphAuxPrint::print_edge_sequence(const Graph& input_graph, const EdgeSequence &input_path) {
    std::vector<Graph::EdgeId> var_edges = input_path.edge_ids();
    std::cout << "EdgeSequence von ";
    GraphAuxPrint::print_nodename( input_path.endnode_a());
    std::cout << " nach ";
    GraphAuxPrint::print_nodename( input_path.endnode_b());
    std::cout << " : \n";
    for(auto curr_edge: var_edges) {
        GraphAuxPrint::print_edge_as_pair(input_graph, curr_edge);
        std::cout << "\n";
    }
}

void GraphAuxPrint::print_edge_vect(const Graph &input_graph, const std::vector<Graph::EdgeId> &input_edges) {
    std::cout << "Aufruf von (GraphAuxPrint::print_edge_vect): \n";
    for(auto curr_edge: input_edges) {
        GraphAuxPrint::print_edge_as_pair(input_graph, curr_edge);
        std::cout << "  ";
        GraphAuxPrint::print_edgeweight(input_graph.get_edge(curr_edge).weight());
        std::cout << "\n";
    }
}

void GraphAuxPrint::print_edge_as_pair(const Graph & input_graph, Graph::EdgeId id) {
    if(id < input_graph.num_edges()) {
        std::cout << "(";
        GraphAuxPrint::print_nodename( input_graph.get_edge(id).node_a() ),
                std::cout << ",";
        GraphAuxPrint::print_nodename( input_graph.get_edge(id).node_b() ),
                std::cout << ")";
    } else if(id == Graph::invalid_edge_id) {
        std::cout << " 'invalid_edge' ";
    } else {
        throw std::runtime_error("(Graph::print_edge_as_pair) Kante mit EdgeId id nicht im Graph");
    }
}
