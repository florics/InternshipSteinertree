//
// Created by Lukas on 10.07.2022.
//

#ifndef PRAKTIKUMSTEINERBAUM_MEHLHORN_WITH_LOCAL_SEARCH_H
#define PRAKTIKUMSTEINERBAUM_MEHLHORN_WITH_LOCAL_SEARCH_H

#include "graph.h"

namespace Mehlhorn_with_LocalSearch {

    //berechnet einen Steinerbaum mit Mehlhorns Algorithmus und verbessert diesen mit lokaler Suche
    // die berechnete Lösung wird auf der Konsole ausgegeben, sowie der Wert der Lösung
    void complete_algorithm(const Graph& input_graph);

}

#endif //PRAKTIKUMSTEINERBAUM_MEHLHORN_WITH_LOCAL_SEARCH_H
