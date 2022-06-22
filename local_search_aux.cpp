//
// Created by Lukas on 22.06.2022.
//

#include "local_search_aux.h"

#include "iostream"

void LocalSearch::initialise_heaps_w_bound_edges( const Voronoi_diagram& input_vd,
                                                  LocalSearch::BoundEdgeHeaps& input_heaps ) {
    for( auto curr_edge : input_vd.original_graph().edges() ) {

        if( input_vd.check_if_bound_edge(curr_edge) ) {

            Graph::NodeId curr_node_a = curr_edge.node_a();
            Graph::NodeId curr_node_b = curr_edge.node_b();
            Graph::PathLength length_bound_path = input_vd.dist_to_base()[curr_node_a] + input_vd.dist_to_base()[curr_node_b] + curr_edge.weight();

            input_heaps[curr_edge.node_a()].push( {length_bound_path, curr_edge.edge_id()} );
            input_heaps[curr_edge.node_b()].push( {length_bound_path, curr_edge.edge_id()} );
        }
    }
}

Graph::NodeId LocalSearch::find_crucial_parent (const Graph& input_graph,
                                   Graph::NodeId start_node,
                                   std::vector<Graph::NodeId>& key_path_node_ids,
                                   Graph::PathLength& key_path_length) {
    if( input_graph.dir_type() == Graph::undirected) {
        throw std::runtime_error("(LocalSearch::find_crucial_parent) Graph ist ungerichtet.");
    }
    if( start_node == Graph::invalid_node_id) {
        throw std::runtime_error("(LocalSearch::find_crucial_parent) Eingabeknoten ist ungueltig.");
    }
    if( start_node > input_graph.num_nodes() ) {
        throw std::runtime_error("(LocalSearch::find_crucial_parent) Eingabeknoten liegt nicht im Eingabegraphen.");
    }

    Graph::NodeId curr_node_id = start_node;
    while( true ) {
        //finde eingehende Kante des aktuellen Knoten
        std::vector<Graph::EdgeId> curr_in_edge_id = input_graph.get_ingoing_edge_ids(curr_node_id);
        if( curr_in_edge_id.size() != 1) {
            std::cout << "aktueller Knoten hat NodeId: " << curr_node_id << "\n";
            throw std::runtime_error("(LocalSearch::find_crucial_parent) Eingabegraph hat Knoten mit Eingangsgrad ungleich 1");
        }

        //füge das Gewicht der Kante zur Länge des Key-Paths hinzu
        Graph::Edge curr_in_edge = input_graph.get_edge( curr_in_edge_id.front() );
        key_path_length += curr_in_edge.weight();

        Graph::Node curr_in_neighbor = input_graph.get_node( input_graph.get_tail(curr_in_edge) );

        curr_node_id = curr_in_neighbor.node_id();

        if( LocalSearch::check_if_crucial(curr_in_neighbor) ){
            //In dem Fall ist der aktuelle Knoten der andere Endpunkt des Key-Paths
            break;
        }

        key_path_node_ids.push_back( curr_node_id );
    }

    return curr_node_id;
}

bool LocalSearch::check_if_crucial (const Graph::Node& input_node) {
    if( input_node.terminal_state() == Graph::terminal || input_node.incident_edge_ids().size() > 2){
        return true;
    } else {
        return false;
    }
}