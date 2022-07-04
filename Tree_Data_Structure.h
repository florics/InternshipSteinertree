//
// Created by Lukas on 02.07.2022.
//

#ifndef PRAKTIKUMSTEINERBAUM_TREE_DATA_STRUCTURE_H
#define PRAKTIKUMSTEINERBAUM_TREE_DATA_STRUCTURE_H


#include "graph.h"

#include "vector"

// Datenstruktur zum Finden der nearest common ancestor in Steiner- & Key-Vertex-Elimination

class Tree_Data_Structure {

public:

    using Depth = unsigned int;

    // füllt die Attribute der Datenstruktur dem Eingabegraphen (der eine Arboreszenz mit Wurzel root sein muss) entsprechend aus
    Tree_Data_Structure(const Graph& input_graph, Graph::NodeId root);

    const std::vector<Graph::NodeId>& parents() const;
    const std::vector<Tree_Data_Structure::Depth>& depths() const;

    // gibt den tiefsten gemeinsamen Vorgänger der Eingabeknoten aus
    // falls dieser Vorgänger einer der beiden Knoten selbst ist, so wird der ungültige Knoten ausgegeben
    // (dieser Spezialfall ist für die Anwendung (Finden der horizontalen Kanten) nicht relevant)
    Graph::NodeId nearest_common_ancestor(Graph::NodeId input_node_x, Graph::NodeId input_node_y) const;

    static const Depth infinite_depth;

private:

    //speichert zu jedem Knoten einen Vorgängerknoten (die Wurzel erhält den ungueltigen Knoten)
    std::vector<Graph::NodeId> _parents;

    // speichert zu jedem Knoten den Abstand zur Wurzel (in Anzahl Kanten)
    std::vector<Depth> _depths;

};

// Datenstruktur für Steiner-Vertex-Insertion

class Weighted_Tree_Data_Structure: public Tree_Data_Structure{

public:

    // füllt die Attribute der Datenstruktur dem Eingabegraphen (der eine Arboreszenz mit Wurzel root sein muss) entsprechend aus
    Weighted_Tree_Data_Structure(const Graph& input_graph, Graph::NodeId root);

    const std::vector<Graph::EdgeWeight>& weight_of_ingoing_edge() const;

private:

    std::vector<Graph::EdgeWeight> _weight_of_ingoing_edge;

};


#endif //PRAKTIKUMSTEINERBAUM_TREE_DATA_STRUCTURE_H
