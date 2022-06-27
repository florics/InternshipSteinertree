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
