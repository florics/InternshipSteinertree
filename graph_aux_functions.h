//
// Created by Lukas on 14.06.2022.
//

#ifndef PRAKTIKUMSTEINERBAUM_GRAPH_AUX_FUNCTIONS_H
#define PRAKTIKUMSTEINERBAUM_GRAPH_AUX_FUNCTIONS_H

#include "graph.h"
#include "Subgraph.h"

namespace GraphAux{

    //gibt 1 aus gdw. nicht-negative Kantengewichte vorliegen
    bool check_if_graph_has_nonnegative_weights(const Graph& input_graph) ;
    //gibt 1 aus gdw. alle Kanten Gewicht (im Betrag) < infinite_weight haben
    bool check_if_graph_has_finite_weights(const Graph& input_graph);


    // gibt 1 aus gdw. der Graph zusammenhängend ist
    bool check_if_graph_is_connected(const Graph& input_graph);

    // gibt 'true' aus gdw. der Eingabegraph 'Steiner-Blätter' enthält
    bool check_for_steinerleafs(const Graph& input_graph);
    //enfernt Steinerknoten aus dem Graphen, sodass ein Graph ohne 'Steiner-Blätter' entsteht
    void remove_steinerbranches(Subgraph& input_subgraph);

    //gibt das Gewicht des gesamten Graphen aus
    Graph::PathLength length_of_all_edges(const Graph& input_graph);

}

#endif //PRAKTIKUMSTEINERBAUM_GRAPH_AUX_FUNCTIONS_H
