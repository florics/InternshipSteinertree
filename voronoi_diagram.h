//
// Created by Lukas on 02.06.2022.
//

#ifndef PRAKTIKUMSTEINERBAUM_VORONOI_DIAGRAM_H
#define PRAKTIKUMSTEINERBAUM_VORONOI_DIAGRAM_H

#include "graph.cpp"
#include "vector"

class Voronoi_diagram {
public:
    //erstellt ein Diagramm zur leeren Basis in einem Graphen mit num_n Knoten
    Voronoi_diagram(unsigned int num_n);
    //soll das Voronoi-Diagramm des Graphen g berechnen, mit den Knoten in set_of_b als Basen
    //Eingabe: set_of_b ist nichtleere Teilmenge der Knoten von g, g hat nichtnegative "endliche" Kantengewichte
    Voronoi_diagram(std::vector<Graph::NodeId> set_of_b, Graph g);

    //Ausgabe auf Konsole für jeden Knoten: NodeId, Basis, Distanz zur Basis, Vorgängerknoten auf kürz. Weg von Basis
    void print_simple();
    //Ausgabe auf Konsole: Voronoi-Region der Basis var_base (var_base muss Basis sein)
    void print_region(Graph::NodeId var_base);
    // Hilfsfunktion zur Ausgabe von Knoten auf Konsole
    void print_node(Graph::NodeId id);
    // Hilfsfunktion zur Ausgabe von Weg-Längen auf Konsole
    void print_pathlength(Graph::PathLength l);

    //prüft ob var_node Basis ist //(todo) evtl zu langsam
    bool check_if_base(Graph::NodeId var_node);

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
};




#endif //PRAKTIKUMSTEINERBAUM_VORONOI_DIAGRAM_H
