//
// Created by Lukas on 01.07.2022.
//

#include "Supergraph.h"

#include "iostream" //später löschen?


Supergraph::Supergraph(unsigned int num_supernodes):
        _this_graph(num_supernodes)
{
    /*
    Graph var_graph( num_supernodes );
    _this_graph = var_graph;
    */

    std::vector< std::vector<Graph::EdgeWeight> > var_weight_of_superedges;
    for(unsigned int i=1; i<num_supernodes+1; i++) {
        std::vector<Graph::EdgeWeight> var_weight_vector (i, Graph::infinite_weight);
        var_weight_of_superedges.push_back(var_weight_vector);
    }
    _weight_of_superedges = var_weight_of_superedges;
}

Graph& Supergraph::this_graph() {
    return _this_graph;
}

const Graph& Supergraph::get_this_graph() const{
    return _this_graph;
}

std::vector<Graph::EdgeId>& Supergraph::original_edge_ids() {
    return _original_edge_ids;
}

const std::vector<Graph::EdgeId>& Supergraph::get_original_edge_ids()  const{
    return _original_edge_ids;
}


std::vector<std::vector<Graph::EdgeWeight> >& Supergraph::weight_of_superedges()  {
    return _weight_of_superedges;
}

void Supergraph::add_edge(Graph::NodeId node_a, Graph::NodeId node_b, Graph::EdgeWeight weight, Graph::EdgeId original_id) {
    if(node_a == node_b) {
        throw std::runtime_error("(Supergraph::add_edge) Eingabeknoten sind gleich.");
    }

    std::vector<Graph::NodeId> endnodes_sorted = {node_a, node_b};
    std::sort(endnodes_sorted.begin(), endnodes_sorted.end());

    //debug (löschen!!)
    if( endnodes_sorted[0] > endnodes_sorted[1] ) {
        throw std::runtime_error("sort tut nicht das, was es soll");
    }

    //prüfe, ob bereits eine Kante zwischen den beiden Endknoten existiert, die günstiger ist
    // (in dem Fall fügen wir die Kante nicht hinzu)
    if( weight < _weight_of_superedges[endnodes_sorted[1]][endnodes_sorted[0]] ){
        _this_graph.add_edge(node_a, node_b, weight);
        _original_edge_ids.push_back(original_id);
        _weight_of_superedges[endnodes_sorted[1]][endnodes_sorted[0]] = weight;
    }
}


std::vector<Supergraph_KVE::BoundEdgeType>& Supergraph_KVE::superedges_corresponding_to_new_bound_edges() {
    return _superedges_corresponding_to_new_bound_edges;
}

const std::vector<Supergraph_KVE::BoundEdgeType> &Supergraph_KVE::get_superedges_corresponding_to_new_bound_edges() const {
    return _superedges_corresponding_to_new_bound_edges;
}


Supergraph_KVE::Supergraph_KVE(unsigned int num_supernodes):
    Supergraph(num_supernodes)
    {}

void Supergraph_KVE::add_edge(Graph::NodeId node_a, Graph::NodeId node_b, Graph::EdgeWeight weight,
                              Graph::EdgeId original_id, BoundEdgeType b_e_type) {

    Supergraph::add_edge(node_a, node_b, weight, original_id);

    //prüfe, ob wir die Kante tatsächlich hinzugefügt haben
    //? if( get_original_edge_ids().back() == original_id ){
    if( get_original_edge_ids().size() == _superedges_corresponding_to_new_bound_edges.size()+1 ){
        _superedges_corresponding_to_new_bound_edges.push_back(b_e_type);
    }
}
