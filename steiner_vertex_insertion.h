//
// Created by Lukas on 07.07.2022.
//

#ifndef PRAKTIKUMSTEINERBAUM_STEINER_VERTEX_INSERTION_H
#define PRAKTIKUMSTEINERBAUM_STEINER_VERTEX_INSERTION_H

#include "graph.h"
#include "Insertion_Tree_Data_Structure.h"

namespace SteinerVertexInsertion {

    //wendet Steiner-Vertex-Insertion einmal auf jeden Steinerknoten an
    //die Verbesserungen werden direkt an dem eingegebenen Graphen ausgeführt (am Ende der Funktion),
    // danach werden evtl. enstandene Steinerblätter ( bzw. -Äste) entfernt
    //Ausgabe: falls keine echten Verbesserungen gefunden wurden, wird 'false' ausgegeben, sonst 'true'
    bool run_one_pass_and_perform_changements_on_graph(Subgraph& input_subgraph);

    //führt Steiner-Vertex-Elimination aus, bis ein lokales Minimum erreicht wird
    //die Verbesserungen werden direkt an dem eingegebenen Graphen ausgeführt (am Ende der Funktion),
    // danach werden evtl. enstandene Steinerblätter ( bzw. -Äste) entfernt
    //in dieser Funktion wird der Vorteil der Baum-Datenstruktur ausgenutzt, dass die Veränderungen sofort in der
    // Datenstruktur ausgeführt werden können, so dass nicht nach jedem pass der (Sub-)Graph verändert werden muss
    void find_local_minimum(Subgraph& input_subgraph);

    //prüft Steiner-Vertex-Elimination für alle Knoten, die nicht in dem Lösungsgraphen liegen
    //echte Verbesserungen werden innerhalb der Baum-Datenstruktur ausgeführt
    //Ausgabe: Wert der Verbesserung
    Graph::PathLength evaluate_neighborhood(const Subgraph& input_subgraph,
                                            Insertion_Tree_Data_Structure& tree_data_structure,
                                            std::vector<Graph::NodeId>& added_nodes);

    //berechnet den MST des Graphen, der durch Hinzufügen des Eingabeknotens zur Baum-Datenstruktur entsteht
    //falls diese Veränderung des Lösungsgraphen eine echte Verbesserung (des Gewichts) darstellt,
    // so wird die Baum-Datenstruktur entsprechend aktualisiert, sonst bleibt sie gleich
    //Ausgabe: Wert der Verbesserung
    Graph::PathLength process_node(Graph::NodeId node_to_insert, const Graph& original_graph,
                                   Insertion_Tree_Data_Structure& tree_data_structure);

}

#endif //PRAKTIKUMSTEINERBAUM_STEINER_VERTEX_INSERTION_H
