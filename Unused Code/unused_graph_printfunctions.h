//
// Created by Lukas on 10.07.2022.
//

#ifndef PRAKTIKUMSTEINERBAUM_UNUSED_GRAPH_PRINTFUNCTIONS_H
#define PRAKTIKUMSTEINERBAUM_UNUSED_GRAPH_PRINTFUNCTIONS_H

#include "../graph.h"
#include "../EdgeSequence.h"

namespace GraphAuxPrint {

    // Hilfsfunktion zur Ausgabe von NodeIds auf Konsole (gibt Wert von id aus oder 'invalid_node')
    void print_nodeid(Graph::NodeId id);

    // Hilfsfunktion zur Ausgabe von EdgeIds auf Konsole (gibt Wert von id aus oder 'invalid_edge')
    void print_edgeid(Graph::EdgeId id);

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

    void print_edge_sequence(const Graph& input_graph, const EdgeSequence &input_path);

    void print_edge_vect(const Graph& input_graph, const std::vector<Graph::EdgeId> &input_edges);

    // Hilfsfunktion zur Ausgabe von Kanten als Knotenpaar auf Konsole (Paar von NodeName's)
    void print_edge_as_pair(const Graph &input_graph, Graph::EdgeId id);
}

#endif //PRAKTIKUMSTEINERBAUM_UNUSED_GRAPH_PRINTFUNCTIONS_H
