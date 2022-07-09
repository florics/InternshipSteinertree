//
// Created by Lukas on 22.06.2022.
//

#include "local_search_aux.h"

#include "iostream"
#include "stack"
#include "algorithm"

#include "vor_diag_aux_functions.h"
#include "EdgeSequence.h"
#include "general_aux_functions.h"
#include "graph_aux_functions.h"


std::vector<Graph::NodeId> LocalSearchAux::get_crucialnodes_in_postorder(const Graph& input_graph, Graph::NodeId root_id) {
    // Checks weglassen ?
    if( root_id == Graph::invalid_node_id) {
        throw std::runtime_error("(LocalSearchAux::get_crucialnodes_in_postorder) Eingabeknoten ungueltig");
    }
    if( root_id > input_graph.num_nodes()) {
        throw std::runtime_error("(LocalSearchAux::get_crucialnodes_in_postorder) Eingabeknoten nicht im Graphen");
    }

    if( not input_graph.check_if_terminal(root_id)) {
        throw std::runtime_error("(LocalSearchAux::get_crucialnodes_in_postorder) Wurzel ist kein Terminal");
    }

    //speichert die crucial nodes in Reihenfolge der Graphendurchmusterung
    std::vector<Graph::NodeId> output;
    output.reserve(input_graph.num_nodes());
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

    output.shrink_to_fit();

    //kehre die Reihenfolge um
    std::reverse(output.begin(), output.end());

    return output;
}


EdgeSequence LocalSearchAux::find_ingoing_keypath (const Graph& input_graph,
                                                   Graph::NodeId start_node,
                                                   std::vector<Graph::NodeId>& internal_node_ids) {
    if( input_graph.dir_type() == Graph::undirected) {
        throw std::runtime_error("(LocalSearchAux::find_ingoing_keypath) Graph ist ungerichtet.");
    }
    if( start_node == Graph::invalid_node_id) {
        throw std::runtime_error("(LocalSearchAux::find_ingoing_keypath) Eingabeknoten ist ungueltig.");
    }
    if( start_node > input_graph.num_nodes() ) {
        throw std::runtime_error("(LocalSearchAux::find_ingoing_keypath) Eingabeknoten liegt nicht im Eingabegraphen.");
    }
    if( not LocalSearchAux::check_if_crucial(input_graph.get_node(start_node)) ) {
        throw std::runtime_error("(LocalSearchAux::find_ingoing_keypath) Eingabeknoten kein crucial node.");
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
            throw std::runtime_error("(LocalSearchAux::find_ingoing_keypath) Eingabegraph hat Knoten mit Eingangsgrad > 1");
        }

        if( curr_in_edge_id.size() == 0) {
            if( not input_graph.check_if_terminal(curr_node_id) ) {
                std::cout << "aktueller Knoten hat NodeId: " << curr_node_id << "\n";
                throw std::runtime_error("(LocalSearchAux::find_ingoing_keypath) Eingabegraph hat Knoten mit Eingangsgrad 0, der kein Terminal ist.");
            } else {
                //in diesem Fall ist der aktuelle Knoten die Wurzel
                break;
            }
        }

        //füge das Gewicht der Kante zur Länge des Key-Paths hinzu
        const Graph::Edge& curr_in_edge = input_graph.get_edge( curr_in_edge_id.front() );
        key_path_length += curr_in_edge.weight();
        //füge die Kante zur Kantenmenge des Key-Path hinzu
        key_path_edges.push_back(curr_in_edge_id.front());

        const Graph::Node& curr_in_neighbor = input_graph.get_node( input_graph.get_tail(curr_in_edge_id.front()) );

        curr_node_id = curr_in_neighbor.node_id();

        if( LocalSearchAux::check_if_crucial(curr_in_neighbor) ){
            if( curr_node_id == start_node) {
                throw std::runtime_error("(LocalSearchAux::find_ingoing_keypath) Eingabegraph hat Kreis");
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

/*
 * löschen?
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
*/

void LocalSearchAux::perform_improving_changements(Subgraph &input_subgraph, std::vector<ImprovingChangement> changements) {

    std::vector<Graph::EdgeId>& original_edge_ids = input_subgraph.original_edgeids();
    const Graph& original_graph = input_subgraph.original_graph();

    //Aktualisieren von original_edge_ids

    //entferne alle zu entfernenden Kanten aus original_edge_ids

    for(auto curr_change: changements) {
        for (auto curr_edge_to_remove: curr_change.getEdgesToRemove()) {
            original_edge_ids[curr_edge_to_remove] = Graph::invalid_edge_id;
        }
    }

    for(unsigned int i = 0; i < original_edge_ids.size(); ) {
        if( original_edge_ids[i] == Graph::invalid_edge_id) {
            original_edge_ids.erase(original_edge_ids.begin() + i);
        } else {
            i++;
        }
    }

    //füge alle hinzuzufügende Kanten zu original_edge_ids hinzu

    //speichert die bereits hinzugefügten Kanten, um Dopplungen zu vermeiden
    //? alternativ kann man jedes Mal checken, ob die entsprechenden Knoten schon inzident sind (Laufzeit)
    std::vector<bool> added_edges(original_graph.num_edges(), false);
    for(auto curr_or_edge_id: original_edge_ids) {
        added_edges[curr_or_edge_id] = true;
    }

    for(auto curr_change: changements) {
        for( auto curr_edge_to_insert: curr_change.getEdgesToInsert()){
            if( not added_edges[curr_edge_to_insert]) {
                original_edge_ids.push_back(curr_edge_to_insert);
                added_edges[curr_edge_to_insert] = true;
            }
        }
    }

    input_subgraph.reset(original_edge_ids);
}

void LocalSearchAux::update_pinned_for_bound_egde(const Voronoi_diagram &vor_diag, const std::vector<Graph::NodeId>& solution_nodeids_of_original_nodes,
                                                  std::vector<bool>& pinned, Graph::EdgeId bound_edge_id) {
    std::pair<Graph::NodeId, Graph::NodeId> endpoints_in_solution_graph = VorDiagAux::get_bases_of_edge(vor_diag, bound_edge_id);
    pinned[ solution_nodeids_of_original_nodes[ endpoints_in_solution_graph.first ] ] = true;
    pinned[ solution_nodeids_of_original_nodes[ endpoints_in_solution_graph.second ] ] = true;
}

void LocalSearchAux::update_forbidden(const Graph &solution_graph, std::vector<bool> &forbidden,
                                      Graph::NodeId node_to_mark) {
    std::stack<Graph::NodeId> next_nodes;
    next_nodes.push(node_to_mark);

    while( not next_nodes.empty() ) {
        Graph::NodeId curr_node_id = next_nodes.top();
        next_nodes.pop();

        for( auto curr_neighbor: solution_graph.get_outgoing_neighbors(curr_node_id)) {
            if( not forbidden[curr_neighbor]) {
                next_nodes.push(curr_neighbor);
                forbidden[curr_neighbor] = true;
            }
        }
    }
}

std::vector<Horizontal_Edges_Lists::ListId>
LocalSearchAux::compute_list_ids_for_horizon_edges_lists(unsigned int num_nodes,
                                         const std::vector<Graph::NodeId>& nodes_to_process) {

    std::vector<Horizontal_Edges_Lists::ListId> output(num_nodes, Horizontal_Edges_Lists::no_list_available);

    for( unsigned int i=0; i<nodes_to_process.size(); i++) {
        output[ nodes_to_process[i] ] = i;
    }

    return output;
}
