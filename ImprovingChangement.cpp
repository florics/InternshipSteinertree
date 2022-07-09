//
// Created by Lukas on 25.06.2022.
//

#include "ImprovingChangement.h"

#include "iostream"
#include "graph_printfunctions.h"

ImprovingChangement::ImprovingChangement(std::vector<Graph::EdgeId> edgesToRemove,
                                         const std::vector<Graph::EdgeId> edgesToInsert,
                                         Graph::PathLength improvementValue) :
                                         _edges_to_remove(edgesToRemove),
                                         _edges_to_insert(edgesToInsert),
                                         _improvement_value(improvementValue) {}

const std::vector<Graph::EdgeId> &ImprovingChangement::getEdgesToRemove() const {
    return _edges_to_remove;
}

const std::vector<Graph::EdgeId> &ImprovingChangement::getEdgesToInsert() const {
    return _edges_to_insert;
}

const Graph::PathLength ImprovingChangement::getImprovementValue() const {
    return _improvement_value;
}

void ImprovingChangement::print(const Subgraph& input_subgraph) {
    const Graph& original_graph = input_subgraph.original_graph();
    const std::vector<Graph::EdgeId>& original_edgeids = input_subgraph.original_edgeids();
    std::cout << "(ImprovingChangement::print) \n ";

    std::cout << "Die edges to remove: \n";
    for(auto curr_edge: _edges_to_remove) {
        GraphAuxPrint::print_edge_as_pair(original_graph, original_edgeids[curr_edge]);
        std::cout << "\n";
    }

    std::cout << "Die edges to insert: \n";
    for(auto curr_edge: _edges_to_insert) {
        GraphAuxPrint::print_edge_as_pair(original_graph, curr_edge);
        std::cout << "\n";
    }
}
