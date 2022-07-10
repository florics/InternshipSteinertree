//
// Created by Lukas on 14.06.2022.
//

#include "graph_aux_functions.h"

#include "iostream"
#include "algorithm"

namespace GraphAux {

    //Ausgabe: Steinerknoten in einer post-order bzgl. einer Graphendurchmusterung-Arboreszenz mit dem Eingabeknoten
    // als Wurzel (diese muss Terminal sein)
    //Verwendung ausschließlich in der Funktion compute_steiner_branches
    std::vector<Graph::NodeId> get_steinernodes_in_postorder(const Graph& input_graph, Graph::NodeId terminal_root);

    // Ausgabe: für jeden Knoten gibt es einen Eintrag, der 'true' ist gdw. der Knoten ein Steinerknoten ist,
    // der ein Blatt ist bzw. ein Blatt wird nach Entfernen von anderen 'Steiner-Blättern'
    //Verwendung ausschließlich in der Funktion remove_steinerbranches
    std::vector<bool> compute_steiner_branches(const Graph& input_graph);
}


bool GraphAux::check_if_graph_has_nonnegative_weights(const Graph& input_graph) {

    for(const auto& var_edge : input_graph.edges()){
        if(var_edge.weight() < 0){
            return false;
        }
    }
    return true;
}

bool GraphAux::check_if_graph_has_finite_weights(const Graph& input_graph) {
    for(const auto& var_edge : input_graph.edges()){
        if( var_edge.weight() == Graph::infinite_weight
            || var_edge.weight() == -Graph::infinite_weight ){
            return false;
        }
    }
    return true;
}

Graph::PathLength GraphAux::length_of_all_edges(const Graph& input_graph) {
    Graph::PathLength output = 0;
    for(const auto& curr_edge : input_graph.edges() ) {
        output += curr_edge.weight();
    }
    return output;
}


bool GraphAux::check_if_graph_is_connected(const Graph &input_graph) {

    // wir sagen, dass der leere Graph zusammenhängend ist
    if( input_graph.num_nodes() == 0) {
        return true;
    }

    //Graphendurchmusterung

    std::vector<Graph::NodeId> next_nodes;
    std::vector<bool> reached(input_graph.num_nodes(), false);

    //starte bei dem Knoten mit NodeId 0
    next_nodes.push_back(0);
    reached[0] = true;

    while( not next_nodes.empty() ) {
        Graph::NodeId curr_node = next_nodes.back();
        next_nodes.pop_back();

        for(auto curr_neighbor: input_graph.adjacency_vect(curr_node)) {
            if( not reached[curr_neighbor]) {
                next_nodes.push_back(curr_neighbor);
                reached[curr_neighbor] = true;
            }
        }
    }

    for(unsigned int i=0; i<input_graph.num_nodes(); i++) {
        if( not reached[i]) {
            return false;
        }
    }

    return true;
}

std::vector<Graph::NodeId>
GraphAux::get_steinernodes_in_postorder(const Graph &input_graph, Graph::NodeId terminal_root) {
    if( terminal_root == Graph::invalid_node_id) {
        throw std::runtime_error("(GraphAux::get_steinernodes_in_postorder) Eingabeknoten ungueltig");
    }
    if( terminal_root > input_graph.num_nodes()) {
        throw std::runtime_error("(GraphAux::get_steinernodes_in_postorder) Eingabeknoten nicht im Graphen");
    }

    if( not input_graph.check_if_terminal(terminal_root)) {
        throw std::runtime_error("(GraphAux::get_steinernodes_in_postorder) Wurzel ist kein Terminal");
    }

    //speichert die Steinerknoten zunächst in der Reihenfolge der Graphendurchmusterung
    std::vector<Graph::NodeId> output;
    output.reserve(input_graph.num_nodes());

    // es folgt eine Graphendurchmusterung

    std::vector<Graph::NodeId> next_nodes (1, terminal_root);
    //speichert die bereits erreichten Knoten (die, die bereits zu next_nodes hinzugfügt wurden)
    //Laufzeit O(num_nodes()) Problem?
    std::vector<bool> reached_nodes (input_graph.num_nodes(), false);
    reached_nodes[terminal_root] = true;

    while( not next_nodes.empty()) {
        Graph::NodeId curr_node_id = next_nodes.back();
        next_nodes.pop_back();

        for( auto curr_edge_id : input_graph.incident_edge_ids(curr_node_id) ) {
            Graph::NodeId curr_neighbor_id = input_graph.get_edge(curr_edge_id).get_other_node(curr_node_id);

            if( not reached_nodes[curr_neighbor_id] ) {
                reached_nodes[curr_neighbor_id] = true;
                next_nodes.push_back(curr_neighbor_id);

                if( not input_graph.check_if_terminal(curr_neighbor_id) ) {
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

std::vector<bool> GraphAux::compute_steiner_branches(const Graph &input_graph) {
    std::vector<bool> nodes_in_st_branch(input_graph.num_nodes(), false);

    std::vector<Graph::NodeId> steinernodes_in_postorder =
            GraphAux::get_steinernodes_in_postorder(input_graph, input_graph.get_terminals()[0]);

    //berechne die Anzahl der Nachbarn für alle Steinerknoten

    using NumNeighbors = unsigned int;
    //? static const NumNeighbors invalid_num = std::numeric_limits<double>::max();

    std::vector<NumNeighbors> num_neighbors(input_graph.num_nodes(), 0);

    for(auto curr_node_id: steinernodes_in_postorder) {
        const Graph::Node& curr_node = input_graph.get_node(curr_node_id);
        num_neighbors[ curr_node_id ] = curr_node.num_neighbors();
    }


    for(auto curr_node_id: steinernodes_in_postorder) {

        if( num_neighbors[curr_node_id] <= 1) {
            nodes_in_st_branch[curr_node_id] = true;

            //aktualisiere die Anzahl an Nachbarn für alle Nachbarn des aktuellen Knotens
            for(auto curr_neighbor_id: input_graph.adjacency_vect(curr_node_id) ) {
                if( (not nodes_in_st_branch[curr_neighbor_id]) && (not input_graph.check_if_terminal(curr_neighbor_id)) ) {
                    num_neighbors[curr_neighbor_id] --;
                }
            }
        }
    }

    return nodes_in_st_branch;
}

void GraphAux::remove_steinerbranches(Subgraph &input_subgraph) {

    const Graph& this_graph = input_subgraph.this_graph();
    const std::vector<Graph::EdgeId>& old_original_edge_ids = input_subgraph.original_edgeids();

    // berechne alle Knoten, die entfernt werden sollen
    std::vector<bool> nodes_in_st_branches = GraphAux::compute_steiner_branches(input_subgraph.this_graph());

    //falls es keine solchen Knoten gibt, können wir abbrechen
    if ( std::all_of(nodes_in_st_branches.begin(), nodes_in_st_branches.end(), [](bool b){return not b;}) ) {
        return;
    }

    // hier sollen alle Kanten eingefügt werden, die nicht mit einem Steinerpunkt inzident sind, der entfernt werden soll
    // Kanten werden über die EdgeId's des zugrundeliegenden Graphen gespeichert
    std::vector<Graph::EdgeId> new_original_edge_ids;
    new_original_edge_ids.reserve(this_graph.num_edges());

    for( const auto& curr_edge: this_graph.edges() ) {

        if( (not nodes_in_st_branches[curr_edge.node_a()]) && (not nodes_in_st_branches[curr_edge.node_b()]) ) {

            Graph::EdgeId curr_edge_original_id = old_original_edge_ids[curr_edge.edge_id()];
            new_original_edge_ids.push_back(curr_edge_original_id);
        }
    }

    new_original_edge_ids.shrink_to_fit();

    input_subgraph.reset(new_original_edge_ids);
}

bool GraphAux::check_for_steinerleafs(const Graph &input_graph) {

    for( const auto& curr_node: input_graph.nodes()) {
        if( (not curr_node.check_if_terminal()) && curr_node.num_neighbors() == 1 ) {
            return true;
        }
    }

    return false;
}


