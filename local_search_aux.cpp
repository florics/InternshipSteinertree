//
// Created by Lukas on 22.06.2022.
//

#include "local_search_aux.h"

#include "iostream"

#include "vor_diag_aux_functions.h"
#include "EdgeSequence.h"


std::vector<Graph::NodeId> LocalSearchAux::get_crucialnodes_in_postorder(const Graph& input_graph, Graph::NodeId root_id) {
    // Checks weglassen ?
    if( root_id == Graph::invalid_node_id) {
        throw std::runtime_error("(KeyPathExch::get_crucialnodes_in_postorder) Eingabeknoten ungueltig");
    }
    if( root_id > input_graph.num_nodes()) {
        throw std::runtime_error("(KeyPathExch::get_crucialnodes_in_postorder) Eingabeknoten nicht im Graphen");
    }

    //speichert die crucial nodes in Reihenfolge der Graphendurchmusterung
    std::vector<Graph::NodeId> output;
    //? output.push_back(root_id);

    // es folgt eine Graphendurchmusterung

    std::vector<Graph::NodeId> next_nodes (1, root_id);
    //speichert die bereits erreichten Knoten (die, die bereits zu next_nodes hinzugfügt wurden)
    //Laufzeit O(num_nodes()) Problem?
    std::vector<bool> reached_nodes (input_graph.num_nodes(), false);
    reached_nodes[root_id] = true;

    while( not next_nodes.empty()) {
        Graph::NodeId curr_node_id = next_nodes.back();
        next_nodes.pop_back();

        for( auto curr_edge_id : input_graph.incident_edge_ids(curr_node_id) ) {
            Graph::NodeId curr_neighbor_id = input_graph.get_edge(curr_edge_id).get_other_node(curr_node_id);

            if( not reached_nodes[curr_neighbor_id] ) {
                reached_nodes[curr_neighbor_id] = true;
                next_nodes.push_back(curr_neighbor_id);

                if( LocalSearchAux::check_if_crucial( input_graph.get_node(curr_neighbor_id)) ) {
                    output.push_back( curr_neighbor_id );
                }
            }
        }
    }

    //kehre die Reihenfolge um
    std::reverse(output.begin(), output.end());

    return output;
}



EdgeSequence LocalSearchAux::find_ingoing_keypath (const Graph& input_graph,
                                                   Graph::NodeId start_node,
                                                   std::vector<Graph::NodeId>& internal_node_ids) {
    if( input_graph.dir_type() == Graph::undirected) {
        throw std::runtime_error("(LocalSearchAux::find_crucial_parent) Graph ist ungerichtet.");
    }
    if( start_node == Graph::invalid_node_id) {
        throw std::runtime_error("(LocalSearchAux::find_crucial_parent) Eingabeknoten ist ungueltig.");
    }
    if( start_node > input_graph.num_nodes() ) {
        throw std::runtime_error("(LocalSearchAux::find_crucial_parent) Eingabeknoten liegt nicht im Eingabegraphen.");
    }
    if( not LocalSearchAux::check_if_crucial(input_graph.get_node(start_node)) ) {
        throw std::runtime_error("(LocalSearchAux::find_crucial_parent) Eingabeknoten kein crucial node.");
    }

    internal_node_ids.clear();
    Graph::PathLength key_path_length = 0;
    std::vector<Graph::EdgeId> key_path_edges;

    Graph::NodeId curr_node_id = start_node;
    //Schleife endet, wenn crucial parent gefunden
    while( true ) {
        //finde eingehende Kante des aktuellen Knoten
        std::vector<Graph::EdgeId> curr_in_edge_id = input_graph.get_ingoing_edge_ids(curr_node_id);

        if( curr_in_edge_id.size() > 1) {
            std::cout << "aktueller Knoten hat NodeId: " << curr_node_id << "\n";
            throw std::runtime_error("(LocalSearchAux::find_crucial_parent) Eingabegraph hat Knoten mit Eingangsgrad > 1");
        }

        if( curr_in_edge_id.size() == 0) {
            if( not input_graph.get_node(curr_node_id).check_terminal()) {
                std::cout << "aktueller Knoten hat NodeId: " << curr_node_id << "\n";
                throw std::runtime_error("(LocalSearchAux::find_crucial_parent) Eingabegraph hat Knoten mit Eingangsgrad > 1");
            } else {
                //in diesem Fall ist der aktuelle Knoten die Wurzel
                break;
            }
        }

        //füge das Gewicht der Kante zur Länge des Key-Paths hinzu
        Graph::Edge curr_in_edge = input_graph.get_edge( curr_in_edge_id.front() );
        key_path_length += curr_in_edge.weight();
        //füge die Kante zur Kantenmenge des Key-Path hinzu
        key_path_edges.push_back(curr_in_edge_id.front());

        Graph::Node curr_in_neighbor = input_graph.get_node( input_graph.get_tail(curr_in_edge_id.front()) );

        curr_node_id = curr_in_neighbor.node_id();

        if( LocalSearchAux::check_if_crucial(curr_in_neighbor) ){
            if( curr_node_id == start_node) {
                throw std::runtime_error("(LocalSearchAux::find_crucial_parent) Eingabegraph hat Kreis");
            }
            //In dem Fall ist der aktuelle Knoten der andere Endpunkt des Key-Paths
            break;
        }

        internal_node_ids.push_back( curr_node_id );
    }

    EdgeSequence output(key_path_edges, curr_node_id, start_node, key_path_length);

    return output;
}

bool LocalSearchAux::check_if_crucial (const Graph::Node& input_node) {
    if( input_node.terminal_state() == Graph::terminal || input_node.incident_edge_ids().size() > 2){
        return true;
    } else {
        return false;
    }
}

std::vector<EdgeSequence> LocalSearchAux::get_new_bound_paths(Voronoi_diagram input_vd, const std::vector<Graph::NodeId>& bases_to_delete) {
    std::vector<EdgeSequence> new_bound_paths;

    if( bases_to_delete.empty() ) {
        return new_bound_paths;
    }

    //enthält alle Knoten, die vor repair zu einer Vor-Region einer Basis gehören, die danach entfernt wird
    std::vector<Graph::NodeId> vor_regions_of_keypath = input_vd.compute_some_vor_regions(bases_to_delete);

    input_vd.repair(bases_to_delete);

    //berechne die neuen boundary edges
    new_bound_paths = VorDiagAux::get_bound_paths_inc_to_nodeset(input_vd, vor_regions_of_keypath);

    return new_bound_paths;
}