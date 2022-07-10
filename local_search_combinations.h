//
// Created by Lukas on 10.07.2022.
//

#ifndef PRAKTIKUMSTEINERBAUM_LOCAL_SEARCH_COMBINATIONS_H
#define PRAKTIKUMSTEINERBAUM_LOCAL_SEARCH_COMBINATIONS_H

#include "vector"

#include "ImprovingChangement.h"
#include "Subgraph.h"

//Hier finden sich die Kombinationen der lokalen Suchen, die so auch im Paper in Kapitel 5 beschrieben werden

namespace LocalSearchCombi {

    //wendet abwechselnd Steiner-Vertex-Insertion und die Kombination aus KVE und KPE
    // (evaluate_neighborhood_with_kve_kpe_in_sequence_for_each_node) auf den ganzen Graphen an
    void find_local_minimum_with_svi_and_kvekpe_in_sequence_each_pass(Subgraph& input_subgraph);

    // führt für jeden Knoten zunächst Key-Vertex-Elimination aus und danach Key-Path-Exchange (falls KVE keine
    // Verbesserung brachte)
    // Ausgabe: Verbesserungen, die simultan ausgeführt werden können (so dass die Lösung ein Baum bleibt)
    std::vector<ImprovingChangement> evaluate_neighborhood_with_kve_kpe_in_sequence_for_each_node(Subgraph& input_subgraph);

}

#endif //PRAKTIKUMSTEINERBAUM_LOCAL_SEARCH_COMBINATIONS_H
