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
    std::vector<Graph::NodeId> &accessSubgraphNodeidsOfNodesInOriginalgraph();

    const std::vector<Graph::NodeId> &getOriginalNodeids() const;
    std::vector<Graph::NodeId> &accessOriginalNodeids();

    const std::vector<Graph::EdgeId> &getOriginalEdgeids() const;
    std::vector<Graph::EdgeId> &accessOriginalEdgeids();

    //verändert den Subgraphen so, dass er dem Graphen entspricht der alle eingegebenen Kanten enthält und keine isolierten Knoten
    void reset_with_set_of_edges(std::vector<Graph::EdgeId>& new_original_edgeids);

private:

    //setzt alle Attribute des Subgraphen auf die der Eingabe (bis auf original_graph)
    void assign(Graph& new_this_graph, std::vector<Graph::NodeId>& new_subgraph_nodeids_of_nodes_in_originalgraph,
                std::vector<Graph::NodeId>& new_original_nodeids,  std::vector<Graph::EdgeId>& new_original_edgeids);

    //zugrundeliegender Graph, Objekt repräsentiert einen Sugraphen bzgl. dieses Graphen
    const Graph& _original_graph;

    //der eigentliche Subgraph (den das Objekt repräsentiert)
    Graph _this_graph;

    //speichert für jeden Knoten des zugrundeliegenden Graphs dessen NodeId im Subgraph (ungütlige ID, falls der Knoten nicht im Subgraph liegt)
    std::vector<Graph::NodeId> _subgraph_nodeids_of_nodes_in_originalgraph;

    //speichert für jeden Knoten des Subgraphen dessen NodeId im zugrundeliegenden Graphen
    std::vector<Graph::NodeId> _original_nodeids;

    //speichert für jede Kante des Subgraphen deren EdgeId im zugrundeliegenden Graphen
    std::vector<Graph::EdgeId> _original_edgeids;

};


#endif //PRAKTIKUMSTEINERBAUM_SUBGRAPH_H
