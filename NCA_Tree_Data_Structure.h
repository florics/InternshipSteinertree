//
// Created by Lukas on 02.07.2022.
//

#ifndef PRAKTIKUMSTEINERBAUM_NCA_TREE_DATA_STRUCTURE_H
#define PRAKTIKUMSTEINERBAUM_NCA_TREE_DATA_STRUCTURE_H

#include "vector"

#include "graph.h"
#include "Subgraph.h"


// Datenstruktur zum Finden der nearest common ancestor (NCA) in Steiner- & Key-Vertex-Elimination


class NCA_Tree_Data_Structure {

public:

    using Depth = unsigned int;

    //füllt die Attribute der Datenstruktur dem Eingabegraphen (der eine Arboreszenz mit Wurzel root sein muss)
    // entsprechend aus
    NCA_Tree_Data_Structure(const Graph& input_graph, Graph::NodeId root);

    //gibt den tiefsten gemeinsamen Vorgänger der Eingabeknoten aus
    //falls dieser Vorgänger einer der beiden Knoten selbst ist, so wird der ungültige Knoten ausgegeben
    // (dieser Spezialfall ist für die Anwendung (Finden der horizontalen Kanten) nicht relevant)
    Graph::NodeId nearest_common_ancestor(Graph::NodeId input_node_x, Graph::NodeId input_node_y) const;

    static const Depth infinite_depth;

private:

    //speichert zu jedem Knoten einen Vorgängerknoten (die Wurzel erhält den ungueltigen Knoten)
    std::vector<Graph::NodeId> _parents;

    //speichert zu jedem Knoten den Abstand zur Wurzel (in Anzahl Kanten)
    std::vector<Depth> _depths;

};


#endif //PRAKTIKUMSTEINERBAUM_NCA_TREE_DATA_STRUCTURE_H
