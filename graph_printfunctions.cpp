//
// Created by Lukas on 14.06.2022.
//

#include "iostream"

#include "graph_printfunctions.h"

#include "graph_aux_functions.h"


void GraphAuxPrint::print_graph(const Graph& input_graph) {

    std::cout << "SECTION Graph \n";
    std::cout << "Nodes " << input_graph.num_nodes() << "\n";
    std::cout << "Edges " << input_graph.num_edges() << "\n";
    for(const auto& var_edge : input_graph.edges()){
        std::cout << "E ";
        GraphAuxPrint::print_nodename(var_edge.node_a());
        std::cout << " ";
        GraphAuxPrint::print_nodename(var_edge.node_b());
        std::cout << " ";
        GraphAuxPrint::print_edgeweight(var_edge.weight() );
        std::cout << "\n";
    }
    std::cout << "\n";

    std::cout<< "SECTION Terminals \n";
    const std::vector<Graph::NodeId>& vect_term = input_graph.get_terminals();
    std::cout << "Terminals " << vect_term.size() << "\n";
    for (unsigned int id : vect_term){
        std::cout << "T ";
        GraphAuxPrint::print_nodename(id);
        std::cout << "\n";
    }
    std::cout << "\n";

}

void GraphAuxPrint::print_subgraph(const Subgraph& input_subgraph) {

    const Graph& this_graph = input_subgraph.this_graph();

    std::cout << "SECTION Graph \n";
    std::cout << "Nodes " << this_graph.num_nodes() << "\n";
    std::cout << "Edges " << this_graph.num_edges() << "\n";
    for(const auto& var_edge : this_graph.edges()){
        std::cout << "E ";
        GraphAuxPrint::print_nodename(var_edge.node_a(), input_subgraph);
        std::cout << " ";
        GraphAuxPrint::print_nodename(var_edge.node_b(), input_subgraph);
        std::cout << " ";
        GraphAuxPrint::print_edgeweight(var_edge.weight() );
        std::cout << "\n";
    }
    std::cout << "\n";

    std::cout<< "SECTION Terminals \n";
    const std::vector<Graph::NodeId>& vect_term = this_graph.get_terminals();
    std::cout << "Terminals " << vect_term.size() << "\n";
    for (unsigned int id : vect_term){
        std::cout << "T ";
        GraphAuxPrint::print_nodename(id, input_subgraph);
        std::cout << "\n";
    }
    std::cout << "\n";

}

void GraphAuxPrint::print_nodename(Graph::NodeId id){
    if(id == Graph::invalid_node_id){
        std::cout << " 'invalid_node' ";
    }else{
        std::cout << id+1;
    }
}

void GraphAuxPrint::print_nodename(Graph::NodeId id, const Subgraph &input_subgraph) {
    if(id == Graph::invalid_node_id){
        std::cout << " 'invalid_node' ";
    } else if ( id >= input_subgraph.this_graph().num_nodes()) {
        throw std::runtime_error("(GraphAuxPrint::print_nodename) Eingabeknoten liegt nicht im Eingabegraphen.");
    } else {
        std::cout << input_subgraph.original_nodeids()[id] + 1;
    }
}

void GraphAuxPrint::print_pathlength(Graph::PathLength l){
    if(l != Graph::infinite_length){
        std::cout << l;
    }else{
        std::cout << " 'infinite_length' ";
    }
}

void GraphAuxPrint::print_edgeweight(Graph::EdgeWeight w){
    if(w != Graph::infinite_weight){
        std::cout << w;
    }else{
        std::cout << " 'infinite_weight' ";
    }
}

void GraphAuxPrint::print_length_of_all_edges(const Graph& input_graph) {
    const Graph::PathLength output = GraphAux::length_of_all_edges(input_graph);
    std::cout << "Das Gewicht des Graphen ist ";
    GraphAuxPrint::print_pathlength( output );
    std::cout << "\n";
}

