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

    //gibt Graph auf Konsole aus, Knoten werden über _node_id ausgegeben
    void print_graph_by_id(const Graph &input_graph);

    //gibt Inzidenz-Vektor auf Konsole aus, Knoten werden als _node_name ausgegeben
    void print_incidence_edges(const Graph &input_graph, Graph::NodeId id);

    //gibt Inzidenz-Vektor auf Konsole aus, Knoten werden als _node_id ausgegeben
    void print_incident_edges_using_ids(const Graph &input_graph, Graph::NodeId id);

    //gibt alle Inzidenz-Vektoren auf Konsole aus, Knoten werden als _node_id ausgegeben
    void print_all_incidence_vect_using_ids(const Graph &input_graph);

    void print_outgoing_neighbors(const Graph &input_graph, Graph::NodeId input_node_id);

    void print_ingoing_neighbors(const Graph &input_graph, Graph::NodeId input_node_id);

    //Hilfsfunktion zur Ausgabe von Knoten auf Konsole
    // gibt id+1 oder 'invalid_node' aus
    void print_nodename(Graph::NodeId id);
    // gibt die original_node_id des Eingabeknotens +1 aus
    void print_nodename(Graph::NodeId id, const Subgraph& input_subgraph);

    // Hilfsfunktion zur Ausgabe von Kanten als Knotenpaar auf Konsole (Paar von NodeName's)
    void print_edge_as_pair(const Graph &input_graph, Graph::EdgeId id);

    // Hilfsfunktion zur Ausgabe von NodeIds auf Konsole (gibt Wert von id aus oder 'invalid_node')
    void print_nodeid(Graph::NodeId id);

    // Hilfsfunktion zur Ausgabe von EdgeIds auf Konsole (gibt Wert von id aus oder 'invalid_edge')
    void print_edgeid(Graph::EdgeId id);

    // Hilfsfunktion zur Ausgabe von Weg-Längen auf Konsole (gibt Wert von l aus oder 'infinite_length')
    void print_pathlength(Graph::PathLength l);

    // Hilfsfunktion zur Ausgabe von Kanten-Längen auf Konsole (gibt Wert von w aus oder 'infinite_weight')
    void print_edgeweight(Graph::EdgeWeight w);

    //gibt Gewicht des gesamten Graphen auf Konsole aus
    void print_length_of_all_edges(const Graph &input_graph);

    // löschen?
    //void print_thisgraph_of_subgraph(const Subgraph& input_subgraph);

    void print_edge_sequence(const Graph& input_graph, const EdgeSequence &input_path);

    void print_edge_vect(const Graph& input_graph, const std::vector<Graph::EdgeId> &input_edges);
}
#endif //PRAKTIKUMSTEINERBAUM_GRAPH_PRINTFUNCTIONS_H
