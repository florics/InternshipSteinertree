//
// Created by Lukas on 02.06.2022.
//

#ifndef PRAKTIKUMSTEINERBAUM_VORONOI_DIAGRAM_H
#define PRAKTIKUMSTEINERBAUM_VORONOI_DIAGRAM_H

#include "graph.cpp"
#include "vector"

class Voronoi_diagram {
public:
    using BaseId = unsigned int;

    //erstellt ein Diagramm zur leeren Basis in einem Graphen mit num_n Knoten
    Voronoi_diagram(Graph& input_graph);
    //soll das Voronoi-Diagramm des Graphen g berechnen, mit den Knoten in set_of_b als Basen
    //Eingabe: set_of_b ist nichtleere Teilmenge der Knoten von g, g hat nichtnegative "endliche" Kantengewichte
    Voronoi_diagram(std::vector<Graph::NodeId> set_of_b, Graph& input_graph);

    //Ausgabe auf Konsole für jeden Knoten: NodeId, Basis, Distanz zur Basis, Vorgängerknoten auf kürz. Weg von Basis
    void print_simple(Graph& original_graph) const;
    //Ausgabe auf Konsole: Voronoi-Region der Basis var_base (var_base muss Basis sein)
    void print_region(Graph::NodeId var_base) const;

    // ? die folgenden print-Fktn ggf. löschen wenn original graph
    // Hilfsfunktion zur Ausgabe von NodeIds als NodeNames auf Konsole
    void print_node(Graph::NodeId id) const;
    // Hilfsfunktion zur Ausgabe von Weg-Längen auf Konsole
    void print_pathlength(Graph::PathLength l) const;

    //setzt Menge der Basen auf new_set_of_bases; macht, dass alle Basen sich selbst als Basis haben sowie dass ihr Abstand zur Basis 0 ist; setzt BaseIds
    //nach Aufruf dieser Methode ist das Voronoi-Diagramm ggf. nicht mehr korrekt
    void assign_set_of_bases(std::vector<Graph::NodeId> new_set_of_bases);

    //prüft ob var_node Basis ist
    //genauer: prüft, ob var_node eine valide BaseId hat (und nicht, ob var_node in der Menge der Basen liegt)
    bool check_if_base(Graph::NodeId var_node) const;

    //berechnet die boundary edges (1. Eintrag des pair), die auf kürz. Weg zwischen den entsprechenden Basen liegen sowie die Länge dieses Weges (2. Eintrag des pair)
    //genauer: die Kanten (v,w), die _dist_to_base[v] + (v,w).weight() + _dist_to_base[w] minimieren
    //Eintrag (i,j) der ausgegebenen "Matrix" entspricht den Basen mit NodeId i+1 und j (wobei i>=j)
    std::vector<
        std::vector<
            std::pair< Graph::EdgeId, Graph::PathLength >
        >
    > min_bound_edges(Graph& input_graph) const;

    //ruft min_bound_edges auf und gibt die ausgegebene Matrix auf Konsole aus (debug)
    void print_min_bound_edges(Graph& input_graph) const;

    //gibt entsprechende BaseIds aus, aufsteigend geordnet
    //Eingabe: 2 unterschiedliche Knoten, die im Graphen liegen
    std::array<Voronoi_diagram::BaseId, 2> get_base_ids_ordered(Graph::NodeId node_one, Graph::NodeId node_two) const;

    //prüft, ob Kante eine boundary edge ist (d. h. die Endknoten liegen in verschiedenen V.-Regionen, insbesondere liegen sie in einer V.-Region)
    //Eingabe: Kante, die zwischen 2 Knoten verläuft, die beide im Graphen liegen
    bool check_if_bound_edge(Graph::Edge var_edge) const;

    //erstellt den Hilfsgraphen (auxiliary graph) für Mehlhorns ALgo. (in dem Paper als G'_1 notiert)
    Graph construct_aux_graph(Graph& input_graph) const;

    //gibt den dem eingebenen Graphen (Subgraph des Hilfsgraphen (auxiliary graph, Mehlhorns Algo.)) entsprechenden Subgraphen des original-Graphen aus
    //Eingabe: vect_min_bound_edges ist in dem Format wie die Ausgabe der Funktion min_bound_edges
    // ? besserer Name für var_graph
    Graph turn_into_subgraph(Graph& var_graph, Graph& original_graph,
                             std::vector<std::vector<std::pair<Graph::EdgeId, Graph::PathLength>>>& vect_min_bound_edges) const;

    static const BaseId invalid_base_id;

private:
    unsigned int _num_nodes;
    unsigned int _num_bases;

    std::vector<Graph::NodeId> _set_of_bases;
    //speichert zu jedem Knoten die zugehörige Basis (an der Stelle, die der NodeId des Knotens entspricht)
    std::vector<Graph::NodeId> _base;

    //speichert die Vorgänger auf kürzestem Weg von der Basis des Knoten zu dem Knoten
    //(an der Stelle, die der NodeId des Knotens entspricht)
    std::vector< std::pair< Graph::NodeId, Graph::EdgeId> > _predecessor;
    //speichert die Länge des kürz. Weg von der Basis des Knoten zu dem Knoten
    //(an der Stelle, die der NodeId des Knotens entspricht)
    std::vector<Graph::PathLength> _dist_to_base;

    // brauche ich _predecessor, _dist_to_base, ... für alle Berechnungen?

    //weist Basen eine eindeutige id zu, nicht-Basen besitzen die id invalid_base_id
    std::vector<BaseId> _base_ids;

    //der zugrundeliegende Graph
    //Graph original_graph;  //const ?
};


//input-Graphen als Referenz übergeben??

#endif //PRAKTIKUMSTEINERBAUM_VORONOI_DIAGRAM_H
