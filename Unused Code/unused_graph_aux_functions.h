//
// Created by Lukas on 10.07.2022.
//

#ifndef PRAKTIKUMSTEINERBAUM_UNUSED_GRAPH_AUX_FUNCTIONS_H
#define PRAKTIKUMSTEINERBAUM_UNUSED_GRAPH_AUX_FUNCTIONS_H


#include "../graph.h"
#include "../Subgraph.h"

namespace GraphAux {

    //gibt 1 aus gdw. die Kanten der Eingabe-Graphen bis auf Permutation der EdgeIds einander entsprechen
    // (Terminale werden nicht betrachtet)
    //Eingabe: endliche Kantengewichte, einfache Graphen, NodeIds müssen "gleich" sein
    // (d. h. wir prüfen nicht, ob Graphen ggf. nach Umnummerierung der Knoten gleich sind)
    bool check_if_isomorph(const Graph& graph1, const Graph& graph2);

    //gibt 1 aus gdw. Graph einfach
    bool check_if_graph_is_simple(const Graph& input_graph);

    // gibt 1 aus gdw. alle Terminale in einer Zusammenhangskmponente liegen
    bool check_if_terminals_connected(const Graph& input_graph);

    // gibt die Steinerknoten des Eingabegraphen aus (debug)
    bool check_for_steinerleafs(const Graph& input_graph);

    //gibt Kopie des Graphen aus, aber ohne Kanten
    Graph copygraph_wo_edges(const Graph& input_graph);
    //gibt Kopie des Graphen aus, in der alle Knoten ohne Nachbarn entfernt wurden
    Graph copygraph_wo_iso_nodes(const Graph& input_graph);
    Subgraph copy_subgraph_wo_iso_nodes(const Subgraph& input_graph);

    //Test-Funktion für die Union_Find_Structure
    //gibt Spannwald des eingegebenen Graphen aus (mit beliebigem Gewicht)
    Graph compute_spann_forest(const Graph& input_graph);

    // gibt die isolierten Knoten des Eingabegraphen aus
    std::vector<Graph::NodeId> get_isolated_nodes(const Graph& input_graph);

}

#endif //PRAKTIKUMSTEINERBAUM_UNUSED_GRAPH_AUX_FUNCTIONS_H
