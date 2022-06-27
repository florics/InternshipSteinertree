//
// Created by Lukas on 14.06.2022.
//

#ifndef PRAKTIKUMSTEINERBAUM_GRAPH_ALGORITHMS_H
#define PRAKTIKUMSTEINERBAUM_GRAPH_ALGORITHMS_H

#include "graph.h"
#include "Subgraph.h"

namespace GraphAlgorithms {

    //berechnet min. Spannbaum der Zsmhangskomp. des Startknoten
    Graph mst_prim(const Graph &input_graph, Graph::NodeId start_node);

    Subgraph mst_prim_for_subgraphs(const Subgraph &input_subgraph, Graph::NodeId start_node);

    //Test-Funktion für die Union_Find_Structure
    //gibt Spannwald des eingegebenen Graphen aus (mit beliebigem Gewicht)
    Graph compute_spann_forest(const Graph &input_graph);
}
#endif //PRAKTIKUMSTEINERBAUM_GRAPH_ALGORITHMS_H
