//
// Created by Lukas on 14.06.2022.
//

#ifndef PRAKTIKUMSTEINERBAUM_GRAPH_PRINTFUNCTIONS_H
#define PRAKTIKUMSTEINERBAUM_GRAPH_PRINTFUNCTIONS_H

#include "graph.h"

//gibt Graph auf Konsole aus, Knoten werden über _node_name ausgegeben
void print(const Graph& input_graph);
//gibt Graph auf Konsole aus, Knoten werden über _node_id ausgegeben
void print_by_id(const Graph& input_graph);
//gibt Inzidenz-Vektor auf Konsole aus, Knoten werden als _node_name ausgegeben
void print_incidence_vect(const Graph& input_graph, Graph::NodeId id);
//gibt Inzidenz-Vektor auf Konsole aus, Knoten werden als _node_id ausgegeben
void print_incidence_vect_by_id(const Graph& input_graph, Graph::NodeId id);
//gibt alle Inzidenz-Vektor auf Konsole aus, Knoten werden als _node_id ausgegeben
void print_all_incidence_vect_by_id(const Graph& input_graph);
//Hilfsfunktion zur Ausgabe von Knoten auf Konsole (gibt _nodename oder 'invalid_node' aus) (löschen?)
void print_nodename(const Graph& input_graph, Graph::NodeId id);
// Hilfsfunktion zur Ausgabe von Kanten als Knotenpaar auf Konsole (Paar von NodeName's)
void print_edge_as_pair(const Graph& input_graph, Graph::EdgeId id);

// Hilfsfunktion zur Ausgabe von NodeIds auf Konsole (gibt Wert von id aus oder 'invalid_node')
void print_nodeid(Graph::NodeId id);
// Hilfsfunktion zur Ausgabe von EdgeIds auf Konsole (gibt Wert von id aus oder 'invalid_edge')
void print_edgeid(Graph::EdgeId id);
// Hilfsfunktion zur Ausgabe von Weg-Längen auf Konsole (gibt Wert von l aus oder 'infinite_length')
void print_pathlength(Graph::PathLength l);
// Hilfsfunktion zur Ausgabe von Kanten-Längen auf Konsole (gibt Wert von w aus oder 'infinite_weight')
void print_edgeweight(Graph::EdgeWeight w);

#endif //PRAKTIKUMSTEINERBAUM_GRAPH_PRINTFUNCTIONS_H
