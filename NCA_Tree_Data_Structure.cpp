//
// Created by Lukas on 02.07.2022.
//

#include "NCA_Tree_Data_Structure.h"

#include "limits"
#include "iostream"
#include "graph_aux_functions.h"


const NCA_Tree_Data_Structure::Depth NCA_Tree_Data_Structure::infinite_depth = std::numeric_limits<unsigned int>::max();


NCA_Tree_Data_Structure::NCA_Tree_Data_Structure(const Graph &input_graph, Graph::NodeId root) {
    GraphAux::check_node_id(root, input_graph.num_nodes(), "NCA_Tree_Data_Structure::NCA_Tree_Data_Structure");

    _parents.assign(input_graph.num_nodes(), Graph::invalid_node_id);

    // _depths wird insbesondere verwendet, um zu prüfen, ob Knoten bereits betrachtet wurden (während der Graphendurchmusterung)
    //? könnte man auch weglassen (diesen Check)
    _depths.assign(input_graph.num_nodes(), NCA_Tree_Data_Structure::infinite_depth);

    // Initialisierung für die Wurzel
    _depths[root] = 0;

    std::vector<Graph::NodeId> next_nodes(1, root);

    //Graphendurchmusterung
    while( not next_nodes.empty() ) {
        Graph::NodeId curr_node = next_nodes.back();
        next_nodes.pop_back();

        for( auto curr_out_neighbor: input_graph.get_outgoing_neighbors(curr_node)) {

            if(_depths[curr_out_neighbor] < NCA_Tree_Data_Structure::infinite_depth) {
                //? könnte man auch weglassen (diesen Check)
                throw std::runtime_error("(NCA_Tree_Data_Structure::NCA_Tree_Data_Structure) Eingabegraph enthält Kreis.");
            }

            _parents[curr_out_neighbor] = curr_node;
            _depths[curr_out_neighbor] = _depths[curr_node] +1;

            next_nodes.push_back(curr_out_neighbor);

        }
    }

    //Check
    for(unsigned int i=0; i<input_graph.num_nodes(); i++) {
        if(_depths[i] == NCA_Tree_Data_Structure::infinite_depth) {
            throw std::runtime_error("(NCA_Tree_Data_Structure::NCA_Tree_Data_Structure) Nicht alle Knoten des Eingabegraphen von der Eingabewurzel aus erreicht.");
        }
    }

}

Graph::NodeId NCA_Tree_Data_Structure::get_parent(Graph::NodeId input_node) const {
    GraphAux::check_node_id(input_node, _parents.size(), "NCA_Tree_Data_Structure::get_parent");
    return _parents[input_node];
}

NCA_Tree_Data_Structure::Depth NCA_Tree_Data_Structure::get_depth(Graph::NodeId input_node) const {
    GraphAux::check_node_id(input_node, _parents.size(), "NCA_Tree_Data_Structure::get_depth");
    return _depths[input_node];
}

Graph::NodeId NCA_Tree_Data_Structure::nearest_common_ancestor(Graph::NodeId input_node_x, Graph::NodeId input_node_y) const {
    GraphAux::check_node_id(input_node_x, _parents.size(), "NCA_Tree_Data_Structure::nearest_common_ancestor");
    GraphAux::check_node_id(input_node_y, _parents.size(), "NCA_Tree_Data_Structure::nearest_common_ancestor");

    Graph::NodeId next_parent_of_x = input_node_x;
    Graph::NodeId next_parent_of_y = input_node_y;

    // gehe solange zu dem Vorgänger des tieferen Knotens, bis beide Knoten die gleiche Tiefe haben
    while(get_depth(next_parent_of_y) < get_depth(next_parent_of_x) ) {
        next_parent_of_x = get_parent(next_parent_of_x);
    }
    while( get_depth(next_parent_of_x) < get_depth(next_parent_of_y) ) {
        next_parent_of_y = get_parent(next_parent_of_y);
    }

    // in dem Fall ist der eine Eingabeknoten ein Vorgänger des anderen
    if(next_parent_of_x == next_parent_of_y) {
        return Graph::invalid_node_id;
    }

    // gehe solange rückwärts im Baum, bis die Knoten gleich sind
    while( next_parent_of_x != next_parent_of_y ) {
        next_parent_of_x = get_parent(next_parent_of_x);
        next_parent_of_y = get_parent(next_parent_of_y);
    }

    return next_parent_of_x;
}

Weighted_Tree_Data_Structure::Weighted_Tree_Data_Structure(const Subgraph &input_subgraph, Graph::NodeId root):
        NCA_Tree_Data_Structure(input_subgraph.this_graph(), root)
    {
        _weights_of_ingoing_edges.assign(input_subgraph.this_graph().num_nodes(), Graph::infinite_weight);
        _original_edge_ids_of_ingoing_edges.assign(input_subgraph.this_graph().num_nodes(), Graph::invalid_node_id);

        std::vector<Graph::NodeId> next_nodes(1, root);

        //Graphendurchmusterung
        while( not next_nodes.empty() ) {
            Graph::NodeId curr_node = next_nodes.back();
            next_nodes.pop_back();

            for( auto curr_out_edge_id: input_subgraph.this_graph().get_outgoing_edge_ids(curr_node)) {
                Graph::Edge curr_out_edge = input_subgraph.this_graph().get_edge(curr_out_edge_id);
                Graph::NodeId curr_out_neighbor = curr_out_edge.node_b();

                _weights_of_ingoing_edges[curr_out_neighbor] = curr_out_edge.weight();

                _original_edge_ids_of_ingoing_edges[curr_out_neighbor] = input_subgraph.original_edgeids()[curr_out_edge_id];

                next_nodes.push_back(curr_out_neighbor);

                //debug
                if( curr_node == curr_out_neighbor ) {
                    throw std::runtime_error("(Weighted_Tree_Data_Structure::Weighted_Tree_Data_Structure) Richtung der Kante iwie falsch.");
                }
            }
        }
    }

Graph::EdgeWeight Weighted_Tree_Data_Structure::get_weight_of_ingoing_edge(Graph::NodeId input_node) const {
    return _weights_of_ingoing_edges[input_node];
}

Graph::EdgeWeight Weighted_Tree_Data_Structure::get_original_edge_id_of_ingoing_edge(Graph::NodeId input_node) const {
    return _original_edge_ids_of_ingoing_edges[input_node];
}

const std::vector<Graph::EdgeId> &Weighted_Tree_Data_Structure::original_edge_ids_of_ingoing_edges() const {
    return _original_edge_ids_of_ingoing_edges;
}

Graph::PathLength
Weighted_Tree_Data_Structure::try_to_insert_edge(Graph::NodeId input_node_a,
                                                 Graph::NodeId input_node_b,
                                                 Graph::EdgeWeight input_weight,
                                                 Graph::EdgeId input_orig_edge_id) {

    GraphAux::check_node_id(input_node_a, _parents.size(), "Weighted_Tree_Data_Structure::try_to_insert_edge");
    GraphAux::check_node_id(input_node_b, _parents.size(), "Weighted_Tree_Data_Structure::try_to_insert_edge");
    GraphAux::check_if_weight_nonnegative(input_weight, "Weighted_Tree_Data_Structure::try_to_insert_edge");
    GraphAux::check_if_weight_finite(input_weight, "Weighted_Tree_Data_Structure::try_to_insert_edge");

    // Variablen, für die Knoten auf dem Weg vom Eingabeknoten zum NCA
    Graph::NodeId next_parent_a = input_node_a;
    Graph::NodeId next_parent_b = input_node_b;

    // Variablen für die teuerste Kante auf dem Weg vom Eingabeknoten zum NCA
    std::pair<Graph::EdgeWeight, Graph::NodeId> candidate_a = {0, Graph::invalid_node_id};
    std::pair<Graph::EdgeWeight, Graph::NodeId> candidate_b = {0, Graph::invalid_node_id};

    // gehe solange zu dem Vorgänger des tieferen Knotens, bis beide Knoten die gleiche Tiefe haben
    // finde dabei die teuerste Kante von den auf diesem Weg betrachteten
    while( get_depth(next_parent_b) < get_depth(next_parent_a) ) {
        if(candidate_a.first < _weights_of_ingoing_edges[next_parent_a] ) {
            candidate_a = {_weights_of_ingoing_edges[next_parent_a], next_parent_a};
        }
        next_parent_a = get_parent(next_parent_a);
    }
    while( get_depth(next_parent_a) < get_depth(next_parent_b) ) {
        if(candidate_b.first < _weights_of_ingoing_edges[next_parent_b] ) {
            candidate_b = {_weights_of_ingoing_edges[next_parent_b], next_parent_b};
        }
        next_parent_b = get_parent(next_parent_b);
    }

    // gehe solange rückwärts im Baum, bis die Knoten gleich sind
    // finde dabei die jeweils teuerste Kante auf den beiden Wegen
    while( next_parent_a != next_parent_b) {

        if(candidate_a.first < _weights_of_ingoing_edges[next_parent_a] ) {
            candidate_a = {_weights_of_ingoing_edges[next_parent_a], next_parent_a};
        }
        if(candidate_b.first < _weights_of_ingoing_edges[next_parent_b] ) {
            candidate_b = {_weights_of_ingoing_edges[next_parent_b], next_parent_b};
        }

        next_parent_a = get_parent(next_parent_a);
        next_parent_b = get_parent(next_parent_b);
    }

    // das Gewicht sowie der Endknoten der teuersten gefundenen Kante
    std::pair<Graph::EdgeWeight, Graph::NodeId> most_costly_edge;

    // Variablen, die einen aktuellen Knoten sowie seinen Vorgänger speichern
    /*Graph::NodeId curr_node = Graph::invalid_node_id;
    Graph::NodeId curr_old_parent = Graph::invalid_node_id;
    Graph::NodeId curr_old_grand_parent = Graph::invalid_node_id;*/
    Weighted_Tree_Data_Structure::VarTreeNode invalid_var_tree_node = {Graph::invalid_node_id,
                                                                       Graph::invalid_node_id,
                                                                       NCA_Tree_Data_Structure::infinite_depth,
                                                                       Graph::infinite_weight,
                                                                       Graph::invalid_edge_id};
    Weighted_Tree_Data_Structure::VarTreeNode curr_node = invalid_var_tree_node;
    Weighted_Tree_Data_Structure::VarTreeNode curr_old_parent = invalid_var_tree_node;

    //Weighted_Tree_Data_Structure::TreeEdge curr_old_grand_parent = parents()[input_node_a];


    //finde die teurere der beiden Kanten
    // danach wollen wir von den Eingabeknoten aus zu der teuersten Kante gehen
    // entsprechend wählen wir den aktuellen Knoten als Startknoten für diesen Weg
    // als den Eingabeknoten, dessen Kandidaten-Kante billiger war
    if( candidate_b.first < candidate_a.first) {

        most_costly_edge = candidate_a;

        curr_node = {input_node_b, Graph::invalid_node_id, get_depth(input_node_b),
                     input_weight, input_orig_edge_id};
        curr_old_parent = get_var_tree_node(input_node_a);
        //curr_old_grand_parent = parents()[input_node_a];
    } else {

        most_costly_edge = candidate_b;

        curr_node = {input_node_a, Graph::invalid_node_id, get_depth(input_node_a),
                     input_weight, input_orig_edge_id};
        curr_old_parent = get_var_tree_node(input_node_b);
        //curr_old_grand_parent = parents()[input_node_b];
    }

    //falls die teuerste gefundene Kante günstiger ist als die Eingabekante, tauschen wir die Kanten nicht aus
    if (most_costly_edge.first < input_weight) {
        return 0;
    }

    // durchlaufe den Weg von dem ausgewählten Eingabeknoten zu dem Knoten, dessen eingehende Kante die teuerste
    // gefundene Kante ist, drehe dabei die Richtung aller Kanten um (und richte die einzufügende Kante entsprechend)

    while( curr_node.node_id != candidate_a.second) {

        // kehre die Richtung der (ehemals) eingehenden Kante des aktuellen Knotens um
        // dabei wird der (ehemalige) Vorgänger des aktuellen Knotens gelöscht,
        // wir haben ihn aber mit curr_old_parent zwischengespeichert
        _parents[curr_old_parent.node_id] = curr_node.node_id;
        _depths[curr_old_parent.node_id] = curr_node.depth +1;
        _weights_of_ingoing_edges[curr_old_parent.node_id] = curr_node.weight;
        _original_edge_ids_of_ingoing_edges[curr_old_parent.node_id] = curr_node.orig_edge_id;


        // aktualisiere die Variablen ('gehe einen Schritt weiter in Richtung Wurzel')
        curr_node = curr_old_parent;
        curr_old_parent = get_var_tree_node(curr_node.parent);
        //curr_old_grand_parent = parents()[curr_old_grand_parent];
    }

    return most_costly_edge.first - input_weight;
}

void Weighted_Tree_Data_Structure::add_node(Graph::NodeId parent, Graph::EdgeWeight weight, Graph::EdgeId orig_edge_id) {
    GraphAux::check_node_id(parent, _parents.size(), "Weighted_Tree_Data_Structure::add_node");

    GraphAux::check_if_weight_nonnegative(weight, "Weighted_Tree_Data_Structure::add_node");
    GraphAux::check_if_weight_finite(weight, "Weighted_Tree_Data_Structure::add_node");

    _parents.push_back(parent);
    _depths.push_back(get_depth(parent) +1);
    _weights_of_ingoing_edges.push_back(weight);
    _original_edge_ids_of_ingoing_edges.push_back(orig_edge_id);
}

void Weighted_Tree_Data_Structure::remove_last_node() {
    _parents.pop_back();
    _depths.pop_back();
    _weights_of_ingoing_edges.pop_back();
    _original_edge_ids_of_ingoing_edges.pop_back();
}

Weighted_Tree_Data_Structure::VarTreeNode Weighted_Tree_Data_Structure::get_var_tree_node(Graph::NodeId input_node) {
    return {input_node, get_parent(input_node), get_depth(input_node),
            get_weight_of_ingoing_edge(input_node), original_edge_ids_of_ingoing_edges()[input_node]};
}