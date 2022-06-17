//
// Created by Lukas on 14.06.2022.
//

#ifndef PRAKTIKUMSTEINERBAUM_GRAPH_AUX_FUNCTIONS_H
#define PRAKTIKUMSTEINERBAUM_GRAPH_AUX_FUNCTIONS_H

#include "graph.h"

//gibt 1 aus gdw. die Kanten der Eingabe-Graphen bis auf Permutation der EdgeIds einander entsprechen (Terminale werden nicht betrachtet)
//Eingabe: endliche Kantengewichte, einfache Graphen, NodeIds müssen "gleich" sein (d. h. wir prüfen nicht, ob Graphen ggf. nach Umnummerierung der Knoten gleich sind)
bool check_if_isomorph(const Graph& graph1, const Graph& graph2);

//gibt 1 aus gdw. nicht-negative Kantengewichte vorliegen
bool edgeweight_nonnegative(const Graph& input_graph) ;
//gibt 1 aus gdw. alle Kanten Gewicht (im Betrag) < infinite_weight haben
bool edgeweight_finite(const Graph& input_graph);
//gibt 1 aus gdw. Graph einfach
bool check_if_simple(const Graph& input_graph);

//gibt Kopie des Graphen aus, aber ohne Kanten
Graph copygraph_wo_edges(const Graph& input_graph);
//gibt Kopie des Graphen aus, in der alle Knoten ohne Nachbarn entfernt wurden
Graph copygraph_wo_iso_nodes(const Graph& input_graph);
//gibt Kopie des Graphen aus, in der alle Nicht-Terminale mit Knotengrad 1 entfernt wurden
Graph copygraph_wo_steinerleafs(const Graph& input_graph);

//gibt das Gewicht des gesamten Graphen aus
Graph::PathLength length_of_all_edges(const Graph& input_graph);

//gibt Gewicht des gesamten Graphen auf Konsole aus
void print_length_of_all_edges(const Graph& input_graph);

#endif //PRAKTIKUMSTEINERBAUM_GRAPH_AUX_FUNCTIONS_H
