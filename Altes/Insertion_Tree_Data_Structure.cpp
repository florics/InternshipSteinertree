//
// Created by Lukas on 07.07.2022.
//

#include "Insertion_Tree_Data_Structure.h"

#include "iostream"
#include "limits"

#include "graph_aux_functions.h"
#include "general_aux_functions.h"

const Insertion_Tree_Data_Structure::NodeId Insertion_Tree_Data_Structure::invalid_node_id =
        std::numeric_limits<unsigned int>::max();


Insertion_Tree_Data_Structure::Insertion_Tree_Data_Structure(const Subgraph &input_subgraph, Graph::NodeId root):
    //beim Erstellen der Datenstruktur entsprechen die tree_nodeids genau den subgraph_nodeids
    _tree_nodeids_of_original_nodes(input_subgraph.subgraph_nodeids_of_nodes_in_originalgraph())
{
    // Graph, den die Datenstruktur repräsentieren soll, sowie seine Knotenanzahl
    const Graph& graph_to_represent = input_subgraph.this_graph();
    unsigned int num_nodes = graph_to_represent.num_nodes();

    // prüfe, ob Kantenanzahl wie bei einem Baum (siehe auch Ende der Funktion)
    if( graph_to_represent.num_edges() != num_nodes-1) {
        throw std::runtime_error("(Insertion_Tree_Data_Structure::Insertion_Tree_Data_Structure) "
                                 "Eingabegraph ist kein Baum.");
    }

    if( root == Graph::invalid_node_id) {
        throw std::runtime_error("(Insertion_Tree_Data_Structure::Insertion_Tree_Data_Structure) "
                                 "Eingabeknoten ungueltig.");
    }
    if( root >= num_nodes) {
        throw std::runtime_error("(Insertion_Tree_Data_Structure::Insertion_Tree_Data_Structure) "
                                 "Eingabeknoten liegt nicht im Graphen.");
    }

    // Werte werden später überschrieben (außer für die Wurzel)
    _parents.assign(num_nodes, Insertion_Tree_Data_Structure::invalid_node_id);
    _weights_of_ingoing_edges.assign(num_nodes, - Graph::infinite_weight);
    _original_edge_ids_of_ingoing_edges.assign(num_nodes, Graph::invalid_edge_id);

    std::vector<Graph::NodeId> next_nodes(1, root);

    //Graphendurchmusterung von der Wurzel aus:

    while( not next_nodes.empty() ) {
        Graph::NodeId curr_node = next_nodes.back();
        next_nodes.pop_back();

        for( auto curr_inc_edge_id: graph_to_represent.incident_edge_ids(curr_node)) {
            const Graph::Edge& curr_inc_edge = graph_to_represent.get_edge(curr_inc_edge_id);
            Graph::NodeId curr_neighbor = curr_inc_edge.get_other_node(curr_node);

            if(_parents[curr_neighbor] == Insertion_Tree_Data_Structure::invalid_node_id && curr_neighbor != root) {

                next_nodes.push_back(curr_neighbor);

                //Ausfüllen der Attribute der Datenstruktur für diesen Nachbarn
                _parents[curr_neighbor] = curr_node;
                _weights_of_ingoing_edges[curr_neighbor] = curr_inc_edge.weight();
                _original_edge_ids_of_ingoing_edges[curr_neighbor] = input_subgraph.original_edgeids()[curr_inc_edge_id];
            }
        }
    }

    //prüfe, ob alle Knoten erreicht wurden
    // zusammen mit dem Check am Anfang des Konstruktors, wissen wir nun, ob der Eingabegraph ein Baum ist
    for(unsigned int i=0; i<num_nodes; i++) {
        if(_parents[i] == Insertion_Tree_Data_Structure::invalid_node_id && i != root) {
            throw std::runtime_error("(Insertion_Tree_Data_Structure::Insertion_Tree_Data_Structure) "
                                     "Nicht alle Knoten des Eingabegraphen von der Eingabewurzel aus erreicht.");
        }
    }

}


//gibt alle Einträge von _original_edge_ids_of_ingoing_edges aus, bis auf den ungültigen (der zu der Wurzel gehört)
std::vector<Graph::EdgeId> Insertion_Tree_Data_Structure::get_all_edges_as_original_edge_ids() const {
    std::vector<Graph::EdgeId> output = _original_edge_ids_of_ingoing_edges;
    //? wenn Wurzel bekannt, geht das schneller
    for(unsigned int i = 0; i < output.size(); i++) {
        if(output[i] == Graph::invalid_edge_id) {
            output.erase(output.begin()+i);
        }
    }
    return output;
}

void Insertion_Tree_Data_Structure::add_node(Graph::NodeId node_to_add, Graph::NodeId parent,
                                             Graph::EdgeWeight weight, Graph::EdgeId orig_edge_id) {

    if( parent == Graph::invalid_node_id || node_to_add == Graph::invalid_node_id ){
        throw std::runtime_error("(Insertion_Tree_Data_Structure::add_node) Ein Eingabeknoten ist ungueltig");
    }
    if( node_to_add >= _tree_nodeids_of_original_nodes.size()
        || parent >= _tree_nodeids_of_original_nodes.size() ){
        throw std::runtime_error("(Insertion_Tree_Data_Structure::add_node) "
                                 "Ein Eingabeknoten liegt nicht im zugrundeliegenden Graphen der Datenstruktur");
    }

    if( weight < 0) {
        throw std::runtime_error("(Insertion_Tree_Data_Structure::add_node) Eingabe-Kantengewicht negativ.");
    }

    if( weight == Graph::infinite_weight ) {
        throw std::runtime_error("(Insertion_Tree_Data_Structure::add_node) Eingabe-Kantengewicht unendlich.");
    }

    //setze die tree_nodeid des hinzuzufügenden Knotens
    _tree_nodeids_of_original_nodes[node_to_add] = _parents.size();

    Insertion_Tree_Data_Structure::NodeId parent_tree_id = _tree_nodeids_of_original_nodes[parent];

    _parents.push_back(parent_tree_id);
    _weights_of_ingoing_edges.push_back(weight);
    _original_edge_ids_of_ingoing_edges.push_back(orig_edge_id);
}

void Insertion_Tree_Data_Structure::remove_last_node(Graph::NodeId node_to_remove) {

    if( node_to_remove == Graph::invalid_node_id ) {
        throw std::runtime_error("(Insertion_Tree_Data_Structure::remove_last_node) Eingabeknoten ist ungueltig");
    }
    if( node_to_remove >= _tree_nodeids_of_original_nodes.size() ){
        throw std::runtime_error("(Insertion_Tree_Data_Structure::remove_last_node) "
                                 "Eingabeknoten liegt nicht im zugrundeliegenden Graphen der Datenstruktur");
    }

    if( _tree_nodeids_of_original_nodes[node_to_remove] != _parents.size()-1) {
        throw std::runtime_error("Eingabeknoten ist nicht der letzte Knoten der Datenstruktur.");
    }

    //aktualisiere die tree_nodeid des zu entfernenden Knotens (dafür brauchen wir die NodeId aus dem Graphen)
    _tree_nodeids_of_original_nodes[node_to_remove] = Insertion_Tree_Data_Structure::invalid_node_id;

    //entferne Knoten aus den anderen Vektoren
    _parents.pop_back();
    _weights_of_ingoing_edges.pop_back();
    _original_edge_ids_of_ingoing_edges.pop_back();
}

Insertion_Tree_Data_Structure::TreeEdge
Insertion_Tree_Data_Structure::get_ingoing_tree_edge(Insertion_Tree_Data_Structure::NodeId input_node) const {

    if(input_node == Insertion_Tree_Data_Structure::invalid_node_id) {
        throw std::runtime_error("(Insertion_Tree_Data_Structure::get_ingoing_tree_edge)"
                                 "Eingabeknoten ungueltig");
    }
    if(input_node >= _parents.size() ) {
        throw std::runtime_error("(Insertion_Tree_Data_Structure::get_ingoing_tree_edge)"
                                 "Eingabeknoten nicht in Datenstruktur");
    }

    return { input_node, _parents[input_node],
            _weights_of_ingoing_edges[input_node],
            _original_edge_ids_of_ingoing_edges[input_node] };
}


Insertion_Tree_Data_Structure::Depth Insertion_Tree_Data_Structure::compute_depth(Insertion_Tree_Data_Structure::NodeId input_node) const {
    if( input_node == Insertion_Tree_Data_Structure::invalid_node_id) {
        throw std::runtime_error("(Insertion_Tree_Data_Structure::compute_depth) Eingabeknoten ungueltig.");
    }
    if( input_node >= _parents.size()) {
        throw std::runtime_error("(Insertion_Tree_Data_Structure::compute_depth) "
                                 "Eingabeknoten nicht in Datenstruktur.");
    }

    Insertion_Tree_Data_Structure::NodeId next_node = _parents[input_node];

    Insertion_Tree_Data_Structure::Depth output_depth = 0;

    while( next_node != Insertion_Tree_Data_Structure::invalid_node_id) {
        next_node = _parents[next_node];
        output_depth++;
    }

    return output_depth;
}

Graph::PathLength
Insertion_Tree_Data_Structure::try_to_insert_edge(const Graph::Edge& edge_to_insert,
                                                  std::vector<Insertion_Tree_Data_Structure::TreeEdge>&  removed_edges,
                                                  std::vector<Insertion_Tree_Data_Structure::TreePath>&  reversed_paths) {

    if ( edge_to_insert.node_a() == Graph::invalid_node_id
         || edge_to_insert.node_b() == Graph::invalid_node_id) {
        throw std::runtime_error("(Insertion_Tree_Data_Structure::set_edge) Knoten der Eingabekante ungueltig");
    }
    if ( edge_to_insert.node_a() >= _tree_nodeids_of_original_nodes.size()
         || edge_to_insert.node_b() >= _tree_nodeids_of_original_nodes.size()) {
        throw std::runtime_error("(Insertion_Tree_Data_Structure::set_edge) "
                                 "Knoten der Eingabekante nicht in dem zugrundeliegenden Graphen");
    }


    //finde die tree-NodeIds der Endpunkte der Eingabekante
    const Insertion_Tree_Data_Structure::NodeId input_node_a_tree_id =
            _tree_nodeids_of_original_nodes[edge_to_insert.node_a()];
    const Insertion_Tree_Data_Structure::NodeId input_node_b_tree_id =
            _tree_nodeids_of_original_nodes[edge_to_insert.node_b()];
    const Graph::EdgeWeight input_weight = edge_to_insert.weight();

    // Variablen, für die Knoten auf dem Weg vom Eingabeknoten zum NCA
    Insertion_Tree_Data_Structure::NodeId next_parent_a = input_node_a_tree_id;
    Insertion_Tree_Data_Structure::NodeId next_parent_b = input_node_b_tree_id;

    //Variablen, für die Tiefe des aktuellen Knotens auf dem Weg vom Eingabeknoten zum NCA
    Insertion_Tree_Data_Structure::Depth depth_a = compute_depth(input_node_a_tree_id);
    Insertion_Tree_Data_Structure::Depth depth_b = compute_depth(input_node_b_tree_id);

    // Variablen für die teuerste Kante auf dem Weg vom Eingabeknoten zum NCA
    Insertion_Tree_Data_Structure::NodeId candidate_a = input_node_a_tree_id;
    Insertion_Tree_Data_Structure::NodeId candidate_b = input_node_b_tree_id;

    // gehe solange zu dem Vorgänger des tieferen Knotens, bis beide Knoten die gleiche Tiefe haben
    // finde dabei die teuerste Kante von den auf diesem Weg betrachteten
    while( depth_b < depth_a ) {
        if(_weights_of_ingoing_edges[candidate_a] < _weights_of_ingoing_edges[next_parent_a] ) {
            candidate_a = next_parent_a;
        }
        next_parent_a = _parents[next_parent_a];
        depth_a--;
    }
    while( depth_a < depth_b ) {
        if(_weights_of_ingoing_edges[candidate_b] < _weights_of_ingoing_edges[next_parent_b] ) {
            candidate_b = next_parent_b;
        }
        next_parent_b = _parents[next_parent_b];
        depth_b--;
    }

    // gehe solange rückwärts im Baum, bis die Knoten gleich sind
    // finde dabei die jeweils teuerste Kante auf den beiden Wegen
    while( next_parent_a != next_parent_b) {

        if(_weights_of_ingoing_edges[candidate_a] < _weights_of_ingoing_edges[next_parent_a] ) {
            candidate_a = next_parent_a;
        }
        if(_weights_of_ingoing_edges[candidate_b] < _weights_of_ingoing_edges[next_parent_b] ) {
            candidate_b = next_parent_b;
        }

        next_parent_a = _parents[next_parent_a];
        next_parent_b = _parents[next_parent_b];
    }

    // die teuerste gefundene Kante
    Insertion_Tree_Data_Structure::TreeEdge most_costly_edge = {Insertion_Tree_Data_Structure::invalid_node_id,
                                                                Insertion_Tree_Data_Structure::invalid_node_id,
                                                                - Graph::infinite_weight,
                                                                Graph::invalid_edge_id};

    Insertion_Tree_Data_Structure::NodeId endnode_path_to_reverse = Insertion_Tree_Data_Structure::invalid_node_id;

    Insertion_Tree_Data_Structure::TreeEdge tree_edge_to_insert = {Insertion_Tree_Data_Structure::invalid_node_id,
                                                                  Insertion_Tree_Data_Structure::invalid_node_id,
                                                                  - Graph::infinite_weight,
                                                                  Graph::invalid_edge_id};


    //finde die teurere der beiden Kanten
    // danach wollen wir die Richtung des Pfads von der teuersten Kante zu den Eingabeknoten umkehren, entsprechend
    // wählen wir als Endknoten für diesen Weg den Eingabeknoten, dessen Kandidaten-Kante teurer war,
    // entsprechend richten wir auch die einzusetzende Kante von dem Endknoten, dessen Kandidaten-Kante billiger war,
    // zu dem anderen Endknoten (dessen Kandidaten-Kante teurer war)
    if( _weights_of_ingoing_edges[candidate_b] < _weights_of_ingoing_edges[candidate_a] ) {

        most_costly_edge = get_ingoing_tree_edge(candidate_a);

        endnode_path_to_reverse = input_node_a_tree_id;

        tree_edge_to_insert = {input_node_a_tree_id, input_node_b_tree_id, input_weight, edge_to_insert.edge_id()};

    } else {

        most_costly_edge = get_ingoing_tree_edge(candidate_b);

        endnode_path_to_reverse = input_node_b_tree_id;

        tree_edge_to_insert = {input_node_b_tree_id, input_node_a_tree_id, input_weight, edge_to_insert.edge_id()};
    }

    //falls die teuerste gefundene Kante günstiger ist als die Eingabekante, tauschen wir die Kanten nicht aus
    if (most_costly_edge.weight < input_weight) {
        return 0;
    }

    Insertion_Tree_Data_Structure::TreePath path_to_reverse = {most_costly_edge.child, endnode_path_to_reverse};

    reverse_path(path_to_reverse);

    set_edge(tree_edge_to_insert);

    //merke die entfernte Kante sowie den Pfad, der umgerichtet wurde

    removed_edges.push_back(most_costly_edge);
    //bemerke, dass der Pfad nun umgekehrt ist
    Insertion_Tree_Data_Structure::TreePath path_to_re_reverse = {endnode_path_to_reverse, most_costly_edge.child};
    reversed_paths.push_back(path_to_re_reverse);

    return most_costly_edge.weight - input_weight;
}

void Insertion_Tree_Data_Structure::set_edge(const Insertion_Tree_Data_Structure::TreeEdge& edge_to_set) {

    if ( edge_to_set.child == Insertion_Tree_Data_Structure::invalid_node_id
            || edge_to_set.parent == Insertion_Tree_Data_Structure::invalid_node_id) {
        throw std::runtime_error("(Insertion_Tree_Data_Structure::set_edge) Knoten der Eingabekante ungueltig");
    }
    if ( edge_to_set.child >= _parents.size()
         || edge_to_set.parent >= _parents.size()) {
        throw std::runtime_error("(Insertion_Tree_Data_Structure::set_edge) "
                                 "Knoten der Eingabekante nicht in der Datenstruktur");
    }

    _parents[edge_to_set.child] = edge_to_set.parent;
    _weights_of_ingoing_edges[edge_to_set.child] = edge_to_set.weight;
    _original_edge_ids_of_ingoing_edges[edge_to_set.child] = edge_to_set.orig_edge_id;
}


void Insertion_Tree_Data_Structure::reverse_path(Insertion_Tree_Data_Structure::TreePath path_to_reverse) {

    if ( path_to_reverse.startnode == Insertion_Tree_Data_Structure::invalid_node_id
         || path_to_reverse.endnode == Insertion_Tree_Data_Structure::invalid_node_id) {
        throw std::runtime_error("(Insertion_Tree_Data_Structure::set_edge) Knoten des Eingabepfads ungueltig");
    }
    if ( path_to_reverse.startnode >= _parents.size()
         || path_to_reverse.endnode >= _parents.size()) {
        throw std::runtime_error("(Insertion_Tree_Data_Structure::set_edge) "
                                 "Knoten des Eingabepfads nicht in der Datenstruktur");
    }

    Insertion_Tree_Data_Structure::TreeEdge curr_edge = get_ingoing_tree_edge(path_to_reverse.endnode);

    while(curr_edge.child != path_to_reverse.startnode) {

        if(curr_edge.parent == Insertion_Tree_Data_Structure::invalid_node_id) {
            throw std::runtime_error("(Insertion_Tree_Data_Structure::reverse_path) Eingabepfad existiert nicht, "
                                     "Wurzel wurde vor dem Endknoten erreicht");
        }

        Insertion_Tree_Data_Structure::TreeEdge parent_edge = get_ingoing_tree_edge(curr_edge.parent);

        // kehre die Richtung der (ehemals) eingehenden Kante des aktuellen Knotens um
        // dabei wird der (ehemalige) Vorgänger des aktuellen Knotens gelöscht,
        // wir haben ihn aber mit curr_old_parent zwischengespeichert
        _parents[curr_edge.parent] = curr_edge.child;
        _weights_of_ingoing_edges[curr_edge.parent] = curr_edge.weight;
        _original_edge_ids_of_ingoing_edges[curr_edge.parent] = curr_edge.orig_edge_id;

        // aktualisiere die Variable curr_edge ('gehe einen Schritt weiter in Richtung Wurzel')
        curr_edge = parent_edge;
    }

}

bool Insertion_Tree_Data_Structure::check_if_orig_node_in_tree_data_str(Graph::NodeId input_node) const {

    if(input_node == Graph::invalid_node_id) {
        throw std::runtime_error("(Insertion_Tree_Data_Structure::check_if_orig_node_in_tree_data_str) "
                                 "Eingabeknoten ungueltig");
    }
    if(input_node >= _tree_nodeids_of_original_nodes.size() ) {
        throw std::runtime_error("(Insertion_Tree_Data_Structure::check_if_orig_node_in_tree_data_str) "
                                 "Eingabeknoten nicht im zugrundeliegenden Graphen");
    }

    if( _tree_nodeids_of_original_nodes[input_node] == Insertion_Tree_Data_Structure::invalid_node_id) {
        return false;
    } else {
        return true;
    }
}

