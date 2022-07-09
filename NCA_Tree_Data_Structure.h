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

    // füllt die Attribute der Datenstruktur dem Eingabegraphen (der eine Arboreszenz mit Wurzel root sein muss) entsprechend aus
    NCA_Tree_Data_Structure(const Graph& input_graph, Graph::NodeId root);

    Graph::NodeId get_parent(Graph::NodeId input_node) const;
    NCA_Tree_Data_Structure::Depth get_depth(Graph::NodeId input_node) const;

    // gibt den tiefsten gemeinsamen Vorgänger der Eingabeknoten aus
    // falls dieser Vorgänger einer der beiden Knoten selbst ist, so wird der ungültige Knoten ausgegeben
    // (dieser Spezialfall ist für die Anwendung (Finden der horizontalen Kanten) nicht relevant)
    Graph::NodeId nearest_common_ancestor(Graph::NodeId input_node_x, Graph::NodeId input_node_y) const;

    static const Depth infinite_depth;

protected:

    //speichert zu jedem Knoten einen Vorgängerknoten (die Wurzel erhält den ungueltigen Knoten)
    std::vector<Graph::NodeId> _parents;

    // speichert zu jedem Knoten den Abstand zur Wurzel (in Anzahl Kanten)
    std::vector<Depth> _depths;

};

// Datenstruktur für Steiner-Vertex-Insertion
//löschen? auch in der cpp-Datei!

class Weighted_Tree_Data_Structure: public NCA_Tree_Data_Structure{

public:

    // füllt die Attribute der Datenstruktur dem Eingabegraphen (der eine Arboreszenz mit Wurzel root sein muss) entsprechend aus
    Weighted_Tree_Data_Structure(const Subgraph &input_subgraph, Graph::NodeId root);

    Graph::EdgeWeight get_weight_of_ingoing_edge(Graph::NodeId input_node) const;

    Graph::EdgeWeight get_original_edge_id_of_ingoing_edge(Graph::NodeId input_node) const;

    const std::vector<Graph::EdgeId>& original_edge_ids_of_ingoing_edges() const;

    // fügt die Eingabekante hinzu und entfernt die teuerste Kante auf dem Fundamentalkreis der Eingabekante,
    // falls dies eine echte Verbesserung darstellt
    // Ausgabe: Wert der Verbesserung
    Graph::PathLength try_to_insert_edge(Graph::NodeId node_a, Graph::NodeId node_b,
                                         Graph::EdgeWeight weight, Graph::EdgeId input_orig_edge_id);

    // fügt Knoten mit entsprechenden Parametern zur Datenstruktur hinzu
    // seine NodeId innerhalb der Struktur ist dann die alte Anzahl an Knoten
    void add_node(Graph::NodeId parent, Graph::EdgeWeight weight, Graph::EdgeId orig_edge_id);

    //entfernt den Knoten, der an letzter Stelle in den Vektoren (_parents etc.) gespeichert ist
    // dieser Knoten darf keine Nachfolger haben, dies wird aber nicht geprüft
    void remove_last_node();

    //Knoten-Variable, die alle Daten, die zu einem Knoten gehören, beinhaltet
    struct VarTreeNode {
        Graph::NodeId node_id;
        Graph::NodeId parent;
        Depth depth;
        Graph::EdgeWeight weight;
        Graph::EdgeId orig_edge_id;
    };

    // gibt die dem Eingabeknoten entsprechende Knoten-Variable aus
    VarTreeNode get_var_tree_node(Graph::NodeId input_node);

private:

    std::vector<Graph::EdgeWeight> _weights_of_ingoing_edges;

    std::vector<Graph::EdgeId> _original_edge_ids_of_ingoing_edges;


};


#endif //PRAKTIKUMSTEINERBAUM_NCA_TREE_DATA_STRUCTURE_H
