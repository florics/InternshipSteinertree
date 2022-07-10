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
