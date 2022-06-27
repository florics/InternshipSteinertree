//
// Created by Lukas on 11.06.2022.
//

#ifndef PRAKTIKUMSTEINERBAUM_MEHLHORNS_ALGO_H
#define PRAKTIKUMSTEINERBAUM_MEHLHORNS_ALGO_H

#include "vector"

#include "graph.h"
#include "voronoi_diagram.h"
#include "Subgraph.h"

Subgraph mehlhorns_algo(Graph& input_graph);

//berechnet die boundary edges (1. Eintrag des pair), die auf kürz. Weg zwischen den entsprechenden Basen liegen sowie die Länge dieses Weges (2. Eintrag des pair)
//genauer: die Kanten (v,w), die _dist_to_base[v] + (v,w).weight() + _dist_to_base[w] minimieren
//Eintrag (i,j) der ausgegebenen "Matrix" entspricht den Basen i+1 und j (wobei i>=j) (wenn man die Basen durchnummeriert, wie zB in compute_base_ids)
// ? Laufzeit/Speicher? ->Kanten direkt in den Graphen ('aux_graph') speichern (Problem, da Kanten doppelt hinzugefügt werden und sich das schwer vermeiden lässt)
std::vector<
        std::vector<
                std::pair< Graph::EdgeId, Graph::PathLength >
        >
> compute_min_bound_edges_of_vd(const Voronoi_diagram& input_vd);

//erstellt den Hilfsgraphen (auxiliary graph) für Mehlhorns ALgo. (in dem Paper als G'_1 notiert)
Graph construct_aux_graph_of_vd(const Voronoi_diagram& input_vd,
                                const std::vector<std::vector<std::pair<Graph::EdgeId, Graph::PathLength>>>& min_bound_edges);

//gibt den dem eingebenen Graphen (in Mehlhorns Algo. der MST des Hilfsgraphen(auxiliary graph)) entsprechenden Subgraphen des zugrundeliegenden Graphen aus
//Eingabe: min_bound_edges ist in dem Format wie die Ausgabe der Funktion compute_min_bound_edges_of_vd
//auslagern?
Subgraph turn_into_subgraph_of_vds_original_graph(const Voronoi_diagram& input_vd,
                                                const Graph& input_graph,
                                                const std::vector<std::vector<std::pair<Graph::EdgeId, Graph::PathLength>>>& min_bound_edges);

//debug
//ruft min_bound_edges auf und gibt die ausgegebene Matrix auf Konsole aus (debug)
void print_min_bound_edges_of_vd(const Voronoi_diagram& input_vd);

#endif //PRAKTIKUMSTEINERBAUM_MEHLHORNS_ALGO_H
