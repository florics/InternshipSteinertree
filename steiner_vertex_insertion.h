//
// Created by Lukas on 07.07.2022.
//

#ifndef PRAKTIKUMSTEINERBAUM_STEINER_VERTEX_INSERTION_H
#define PRAKTIKUMSTEINERBAUM_STEINER_VERTEX_INSERTION_H

#include "graph.h"
#include "Insertion_Tree_Data_Structure.h"

namespace SteinerVertexInsertion {

    // führt Steiner-Vertex-Elimination aus, bis ein lokales Minimum erreicht wird
    // die Verbesserungen werden direkt an dem eingegebenen Graphen ausgeführt
    void find_local_minimum(Subgraph& input_subgraph);

    // prüft Steiner-Vertex-Elimination für alle Knoten, die nicht in dem Lösungsgraphen liegen
    // echte Verbesserungen werden ausgeführt
    // Ausgabe: Wert der Verbesserung
    Graph::PathLength evaluate_neighborhood(const Subgraph& input_subgraph,
                                            Insertion_Tree_Data_Structure& tree_data_structure,
                                            std::vector<Graph::NodeId>& added_nodes);

    //berechnet den MST des Graphen, der durch Hinzufügen des Eingabeknotens zur Baum-Datenstruktur entsteht
    // falls diese Veränderung des Lösungsgraphen eine echte Verbesserung (des Gewichts) darstellt,
    // so wird die Baum-Datenstruktur entsprechend aktualisiert, sonst bleibt sie gleich
    // Ausgabe: Wert der Verbesserung
    Graph::PathLength process_node(Graph::NodeId node_to_insert, const Graph& original_graph,
                                   Insertion_Tree_Data_Structure& tree_data_structure);

}

#endif //PRAKTIKUMSTEINERBAUM_STEINER_VERTEX_INSERTION_H
