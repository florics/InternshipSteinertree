//
// Created by Lukas on 22.06.2022.
//

#include "key_path_exchange.h"

#include "iostream"

std::vector<Graph::NodeId> KeyPathExch::get_crucialnodes_in_postorder(const Graph& input_graph, Graph::NodeId root_id) {
    // Checks weglassen ?
    if( root_id == Graph::invalid_node_id) {
        throw std::runtime_error("(KeyPathExch::get_crucialnodes_in_postorder) Eingabeknoten ungueltig");
    }
    if( root_id > input_graph.num_nodes()) {
        throw std::runtime_error("(KeyPathExch::get_crucialnodes_in_postorder) Eingabeknoten nicht im Graphen");
    }

    //speichert die crucial nodes in Reihenfolge der Graphendurchmusterung
    std::vector<Graph::NodeId> crucial_nodes_in_preorder;
    crucial_nodes_in_preorder.push_back(root_id);

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

                if( LocalSearch::check_if_crucial( input_graph.get_node(curr_neighbor_id)) ) {
                    crucial_nodes_in_preorder.push_back( curr_neighbor_id );
                }
            }
        }
    }

    // gehe crucial_nodes_in_preorder rückwärts durch und konstruiere so die Ausgabe
    std::vector<Graph::NodeId> output;
    for( int i = crucial_nodes_in_preorder.size()-1; i >= 0; i--) {
        output.push_back( crucial_nodes_in_preorder[i] );
    }

    return output;
}
