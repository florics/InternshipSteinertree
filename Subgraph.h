//
// Created by Lukas on 26.06.2022.
//

#ifndef PRAKTIKUMSTEINERBAUM_SUBGRAPH_H
#define PRAKTIKUMSTEINERBAUM_SUBGRAPH_H

#include "graph.h"

class Subgraph {

public:
    Subgraph(const Graph &originalGraph, const Graph &thisGraph,
             const std::vector<Graph::NodeId> &subgraphNodeidsOfNodesInOriginalgraph,
             const std::vector<Graph::NodeId> &originalNodeids, const std::vector<Graph::EdgeId> &originalEdgeids);

    const Graph &getOriginalGraph() const;

    const Graph &getThisGraph() const;

    Graph &accessThisGraph();

    const std::vector<Graph::NodeId> &getSubgraphNodeidsOfNodesInOriginalgraph() const;

    const std::vector<Graph::EdgeId> &getOriginalEdgeids() const;

private:

    //zugrundeliegender Graph, Objekt repräsentiert einen Sugraphen bzgl. dieses Graphen
    const Graph& _original_graph;

    //der eigentliche Subgraph (den das Objekt repräsentiert)
    Graph this_graph;

    //speichert für jeden Knoten des zugrundeliegenden Graphs dessen NodeId im Subgraph (ungütlige ID, falls der Knoten nicht im Subgraph liegt)
    std::vector<Graph::NodeId> subgraph_nodeids_of_nodes_in_originalgraph;

    //speichert für jeden Knoten des Subgraphen dessen NodeId im zugrundeliegenden Graphen
    std::vector<Graph::NodeId> original_nodeids;
public:
    const std::vector<Graph::NodeId> &getOriginalNodeids() const;

private:

    //speichert für jede Kante des Subgraphen deren EdgeId im zugrundeliegenden Graphen
    std::vector<Graph::EdgeId> original_edgeids;

};


#endif //PRAKTIKUMSTEINERBAUM_SUBGRAPH_H
