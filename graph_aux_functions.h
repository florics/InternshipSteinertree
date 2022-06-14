//
// Created by Lukas on 14.06.2022.
//

#ifndef PRAKTIKUMSTEINERBAUM_GRAPH_AUX_FUNCTIONS_H
#define PRAKTIKUMSTEINERBAUM_GRAPH_AUX_FUNCTIONS_H

#include "graph.h"

// Hilfsfunktion zur Ausgabe von NodeIds auf Konsole (gibt Wert von id aus oder 'invalid_node')
void print_nodeid(Graph::NodeId id);
// Hilfsfunktion zur Ausgabe von EdgeIds auf Konsole (gibt Wert von id aus oder 'invalid_edge')
void print_edgeid(Graph::EdgeId id);
// Hilfsfunktion zur Ausgabe von Weg-Längen auf Konsole (gibt Wert von l aus oder 'infinite_length')
void print_pathlength(Graph::PathLength l);
// Hilfsfunktion zur Ausgabe von Kanten-Längen auf Konsole (gibt Wert von w aus oder 'infinite_weight')
void print_edgeweight(Graph::EdgeWeight w);

//gibt 1 aus gdw. die Kanten der Eingabe-Graphen bis auf Permutation der EdgeIds einander entsprechen (Terminale werden nicht betrachtet)
//Eingabe: endliche Kantengewichte, einfache Graphen, NodeIds müssen "gleich" sein (d. h. wir prüfen nicht, ob Graphen ggf. nach Umnummerierung der Knoten gleich sind)
bool check_if_isomorph(const Graph& graph1, const Graph& graph2);

//gibt Kopie des Graphen aus, aber ohne Kanten
Graph copygraph_wo_edges(const Graph& input_graph);
//gibt Kopie des Graphen aus, in der alle Knoten ohne Nachbarn entfernt wurden
Graph copygraph_wo_iso_nodes(const Graph& input_graph);
//gibt Kopie des Graphen aus, in der alle Nicht-Terminale mit Knotengrad 1 entfernt wurden
Graph copygraph_wo_steinerleafs(const Graph& input_graph);

#endif //PRAKTIKUMSTEINERBAUM_GRAPH_AUX_FUNCTIONS_H
