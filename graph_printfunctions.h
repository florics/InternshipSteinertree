//
// Created by Lukas on 14.06.2022.
//

#ifndef PRAKTIKUMSTEINERBAUM_GRAPH_PRINTFUNCTIONS_H
#define PRAKTIKUMSTEINERBAUM_GRAPH_PRINTFUNCTIONS_H

#include "graph.h"
#include "Subgraph.h"
#include "EdgeSequence.h"

namespace GraphAuxPrint {

    //gibt Graph auf Konsole aus, Knoten werden mit _node_id+1 ausgegeben
    void print_graph(const Graph &input_graph);

    //gibt den this_graph der Eingabe aus Konsole aus,
    // Knoten werden mit der NodeId aus dem zugrundeliegenden Graphen +1 ausgegeben
    void print_subgraph(const Subgraph& input_graph);

    //Hilfsfunktion zur Ausgabe von Knoten auf Konsole
    // gibt id+1 oder 'invalid_node' aus
    void print_nodename(Graph::NodeId id);
    // gibt die original_node_id des Eingabeknotens +1 aus
    void print_nodename(Graph::NodeId id, const Subgraph& input_subgraph);

    // Hilfsfunktion zur Ausgabe von Weg-Längen auf Konsole (gibt Wert von l aus oder 'infinite_length')
    void print_pathlength(Graph::PathLength l);

    // Hilfsfunktion zur Ausgabe von Kanten-Längen auf Konsole (gibt Wert von w aus oder 'infinite_weight')
    void print_edgeweight(Graph::EdgeWeight w);

    //gibt Gewicht des gesamten Graphen auf Konsole aus
    void print_length_of_all_edges(const Graph &input_graph);

}
#endif //PRAKTIKUMSTEINERBAUM_GRAPH_PRINTFUNCTIONS_H
