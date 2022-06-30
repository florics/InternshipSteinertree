//
// Created by Lukas on 26.06.2022.
//

#include "Subgraph.h"

Subgraph::Subgraph(const Graph &originalGraph, const Graph &thisGraph,
                   const std::vector<Graph::NodeId> &subgraphNodeidsOfNodesInOriginalgraph,
                   const std::vector<Graph::NodeId> &originalNodeids, const std::vector<Graph::EdgeId> &originalEdgeids):
                   _original_graph(originalGraph),
                   this_graph(thisGraph),
                   subgraph_nodeids_of_nodes_in_originalgraph(subgraphNodeidsOfNodesInOriginalgraph),
                   original_nodeids(originalNodeids), original_edgeids(originalEdgeids) {}

const Graph &Subgraph::getOriginalGraph() const {
    return _original_graph;
}

//? const machen? (die Funktion, nicht die Ausgabe)
const Graph &Subgraph::getThisGraph() const {
    return this_graph;
}

const std::vector<Graph::NodeId> &Subgraph::getSubgraphNodeidsOfNodesInOriginalgraph() const {
    return subgraph_nodeids_of_nodes_in_originalgraph;
}

const std::vector<Graph::EdgeId> &Subgraph::getOriginalEdgeids() const {
    return original_edgeids;
}

const std::vector<Graph::NodeId> &Subgraph::getOriginalNodeids() const {
    return original_nodeids;
}

Graph &Subgraph::accessThisGraph() {
    return this_graph;
}

std::vector<Graph::NodeId> &Subgraph::accessSubgraphNodeidsOfNodesInOriginalgraph() {
    return subgraph_nodeids_of_nodes_in_originalgraph;
}

std::vector<Graph::NodeId> &Subgraph::accessOriginalNodeids() {
    return original_nodeids;
}

std::vector<Graph::EdgeId> &Subgraph::accessOriginalEdgeids() {
    return original_edgeids;
}

void Subgraph::assign(Subgraph &subgraph_to_assign) {
    //todo: Checks einbauen, dass original-Graphen gleich sind

    //? hier Problem, wenn Dinge im Graph const
    this_graph = subgraph_to_assign.this_graph;
    subgraph_nodeids_of_nodes_in_originalgraph = subgraph_to_assign.getSubgraphNodeidsOfNodesInOriginalgraph();
    original_nodeids = subgraph_to_assign.getOriginalNodeids();
    original_edgeids = subgraph_to_assign.getOriginalEdgeids();
}
