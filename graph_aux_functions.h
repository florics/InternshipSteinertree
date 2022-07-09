//
// Created by Lukas on 14.06.2022.
//

#ifndef PRAKTIKUMSTEINERBAUM_GRAPH_AUX_FUNCTIONS_H
#define PRAKTIKUMSTEINERBAUM_GRAPH_AUX_FUNCTIONS_H

#include "graph.h"
#include "Subgraph.h"

namespace GraphAux{

    //gibt 1 aus gdw. die Kanten der Eingabe-Graphen bis auf Permutation der EdgeIds einander entsprechen (Terminale werden nicht betrachtet)
    //Eingabe: endliche Kantengewichte, einfache Graphen, NodeIds müssen "gleich" sein (d. h. wir prüfen nicht, ob Graphen ggf. nach Umnummerierung der Knoten gleich sind)
    bool check_if_isomorph(const Graph& graph1, const Graph& graph2);

    // prüft, ob NodeId im Graphen ist und gültig ist (gibt Fehlermeldung aus, wenn nicht)
    void check_node_id(Graph::NodeId input_node_id, unsigned int num_nodes, const std::string& function_name);
    // prüft, ob EdgeId im Graphen ist und gültig ist (gibt Fehlermeldung aus, wenn nicht)
    void check_edge_id(Graph::EdgeId input_edge_id, unsigned int num_edges, const std::string& function_name);
    // prüft, ob Kantengewicht nicht-negativ (gibt Fehlermeldung aus, wenn nicht)
    void check_if_weight_nonnegative(Graph::EdgeWeight input_weight, const std::string& function_name);
    // prüft, ob Kantengewicht endlich (gibt Fehlermeldung aus, wenn nicht)
    void check_if_weight_finite(Graph::EdgeWeight input_weight, const std::string& function_name);

    //gibt 1 aus gdw. nicht-negative Kantengewichte vorliegen
    bool edgeweight_nonnegative(const Graph& input_graph) ;
    //gibt 1 aus gdw. alle Kanten Gewicht (im Betrag) < infinite_weight haben
    bool edgeweight_finite(const Graph& input_graph);
    //gibt 1 aus gdw. Graph einfach
    bool check_if_simple(const Graph& input_graph);

    // gibt die isolierten Knoten des Eingabegraphen aus
    std::vector<Graph::NodeId> get_isolated_nodes(const Graph& input_graph);
    // gibt 1 aus gdw. der Graph zusammenhängend ist
    bool check_if_connected(const Graph& input_graph);
    // gibt 1 aus gdw. alle Terminale in einer Zusammenhangskmponente liegen
    bool check_if_terminals_connected(const Graph& input_graph);

    // gibt die Steinerknoten des Eingabegraphen aus (debug)
    std::vector<Graph::NodeId> get_steinerleafs(const Graph& input_graph);
    //Ausgabe: Steinerknoten in einer post-order bzgl. einer Graphendurchmusterung-Arboreszenz mit dem Eingabeknoten als Wurzel
    //diese Wurzel muss Terminal sein
    std::vector<Graph::NodeId> get_steinernodes_in_postorder(const Graph& input_graph, Graph::NodeId terminal_root);
    // Ausgabe: für jeden Knoten gibt es einen Eintrag, der 'true' ist gdw. der Knoten ein Steinerknoten ist,
    // der ein Blatt ist bzw. ein Blatt wird nach Entfernen von anderen 'Steiner-Blättern'
    std::vector<bool> compute_steiner_branches(const Graph& input_graph);

    //gibt Kopie des Graphen aus, aber ohne Kanten
    Graph copygraph_wo_edges(const Graph& input_graph);
    //gibt Kopie des Graphen aus, in der alle Knoten ohne Nachbarn entfernt wurden
    //beide löschen?
    Graph copygraph_wo_iso_nodes(const Graph& input_graph);
    Subgraph copy_subgraph_wo_iso_nodes(const Subgraph& input_graph);
    //gibt Kopie des Graphen aus, in der alle Nicht-Terminale mit Knotengrad 1 entfernt wurden
    //? Graph copygraph_wo_steinerleafs(const Graph& input_graph);
    //? Subgraph copy_subgraph_wo_steinerleafs_old(const Subgraph& input_subgraph);
    void remove_steinerbranches(Subgraph& input_subgraph);

    //gibt das Gewicht des gesamten Graphen aus
    Graph::PathLength length_of_all_edges(const Graph& input_graph);

}

#endif //PRAKTIKUMSTEINERBAUM_GRAPH_AUX_FUNCTIONS_H
