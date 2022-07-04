//
// Created by Lukas on 02.07.2022.
//

#include "Tree_Data_Structure.h"

#include "limits"
#include "iostream"


const Tree_Data_Structure::Depth Tree_Data_Structure::infinite_depth = std::numeric_limits<unsigned int>::max();


Tree_Data_Structure::Tree_Data_Structure(const Graph &input_graph, Graph::NodeId root) {

    _parents.assign(input_graph.num_nodes(), Graph::invalid_node_id);

    // _depths wird insbesondere verwendet, um zu prüfen, ob Knoten bereits betrachtet wurden (während der Graphendurchmusterung)
    //? könnte man auch weglassen (diesen Check)
    _depths.assign(input_graph.num_nodes(), Tree_Data_Structure::infinite_depth);

    // Initialisierung für die Wurzel
    _depths[root] = 0;

    std::vector<Graph::NodeId> next_nodes(1, root);

    //Graphendurchmusterung
    while( not next_nodes.empty() ) {
        Graph::NodeId curr_node = next_nodes.back();
        next_nodes.pop_back();

        for( auto curr_out_neighbor: input_graph.get_outgoing_neighbors(curr_node)) {

            if(_depths[curr_out_neighbor] < Tree_Data_Structure::infinite_depth) {
                //? könnte man auch weglassen (diesen Check)
                throw std::runtime_error("(Tree_Data_Structure::Tree_Data_Structure) Eingabegraph enthält Kreis.");
            }

            _parents[curr_out_neighbor] = curr_node;
            _depths[curr_out_neighbor] = _depths[curr_node] +1;

            next_nodes.push_back(curr_out_neighbor);

        }
    }

    //Check
    for(unsigned int i=0; i<input_graph.num_nodes(); i++) {
        if( _depths[i] == Tree_Data_Structure::infinite_depth) {
            throw std::runtime_error("(Tree_Data_Structure::Tree_Data_Structure) Nicht alle Knoten des Eingabegraphen von der Eingabewurzel aus erreicht.");
        }
    }

}

const std::vector<Graph::NodeId> &Tree_Data_Structure::parents() const {
    return _parents;
}

const std::vector<Tree_Data_Structure::Depth> &Tree_Data_Structure::depths() const {
    return _depths;
}

Graph::NodeId Tree_Data_Structure::nearest_common_ancestor(Graph::NodeId input_node_x, Graph::NodeId input_node_y) const {
    if(input_node_x == Graph::invalid_node_id || input_node_y == Graph::invalid_node_id) {
        throw std::runtime_error("(Tree_Data_Structure::nearest_common_ancestor) Eingabeknoten ungueltig");
    }
    if(input_node_x >= _parents.size() || input_node_y >= _parents.size()) {
        throw std::runtime_error("(Tree_Data_Structure::nearest_common_ancestor) Eingabeknoten nicht in der Datenstruktur.");
    }

    Graph::NodeId next_parent_of_x = input_node_x;
    Graph::NodeId next_parent_of_y = input_node_y;

    // gehe solange zu dem Vorgänger des tieferen Knotens, bis beide Knoten die gleiche Tiefe haben
    while( _depths[next_parent_of_y] < _depths[next_parent_of_x] ) {
        next_parent_of_x = _parents[next_parent_of_x];
    }
    while( _depths[next_parent_of_x] < _depths[next_parent_of_y] ) {
        next_parent_of_y = _parents[next_parent_of_y];
    }

    // in dem Fall ist der eine Eingabeknoten ein Vorgänger des anderen
    if(next_parent_of_x == next_parent_of_y) {
        return Graph::invalid_node_id;
    }

    // gehe solange rückwärts im Baum, bis die Knoten gleich sind
    while( next_parent_of_x != next_parent_of_y ) {
        next_parent_of_x = _parents[next_parent_of_x];
        next_parent_of_y = _parents[next_parent_of_y];
    }

    return next_parent_of_x;
}

Weighted_Tree_Data_Structure::Weighted_Tree_Data_Structure(const Graph &input_graph, Graph::NodeId root):
    Tree_Data_Structure(input_graph, root)
    {
        _weight_of_ingoing_edge.assign(input_graph.num_nodes(), Graph::infinite_weight);

        std::vector<Graph::NodeId> next_nodes(1, root);

        //Graphendurchmusterung
        while( not next_nodes.empty() ) {
            Graph::NodeId curr_node = next_nodes.back();
            next_nodes.pop_back();

            for( auto curr_out_edge_id: input_graph.get_outgoing_edge_ids(curr_node)) {
                Graph::Edge curr_out_edge = input_graph.get_edge(curr_out_edge_id);
                Graph::NodeId curr_out_neighbor = curr_out_edge.node_b();

                _weight_of_ingoing_edge[curr_out_neighbor] = curr_out_edge.weight();

                next_nodes.push_back(curr_out_neighbor);

                //debug
                if( curr_node == curr_out_neighbor ) {
                    throw std::runtime_error("(Weighted_Tree_Data_Structure::Weighted_Tree_Data_Structure) Richtung der Kante iwie falsch.");
                }
            }
        }
    }

const std::vector<Graph::EdgeWeight> &Weighted_Tree_Data_Structure::weight_of_ingoing_edge() const {
    return _weight_of_ingoing_edge;
}