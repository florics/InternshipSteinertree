//
// Created by Lukas on 07.07.2022.
//

#ifndef PRAKTIKUMSTEINERBAUM_INSERTION_TREE_DATA_STRUCTURE_H
#define PRAKTIKUMSTEINERBAUM_INSERTION_TREE_DATA_STRUCTURE_H

#include "graph.h"
#include "Subgraph.h"

// Datenstruktur für Steiner-Vertex-Insertion


class Insertion_Tree_Data_Structure {
public:

    using NodeId = unsigned int;
    using Depth = unsigned int;

    static const Insertion_Tree_Data_Structure::NodeId invalid_node_id;

    // füllt die Attribute der Datenstruktur dem Eingabegraphen, gerichtet von der Eingabewurzel aus, entsprechend aus
    Insertion_Tree_Data_Structure(const Subgraph &input_subgraph, Graph::NodeId root);

    std::vector<Graph::EdgeId> get_all_edges_as_original_edge_ids() const;

    bool check_if_orig_node_in_tree_data_str(Graph::NodeId input_node) const;

    //fügt Knoten mit entsprechenden Parametern zur Datenstruktur hinzu
    // seine Baum-NodeId innerhalb der Struktur ist dann die alte Anzahl an Knoten
    // die Eingabeknoten werden als NodeIds im zugrundeliegenden Graphen übergeben
    void add_node(Graph::NodeId node_to_add, Graph::NodeId parent,
                  Graph::EdgeWeight weight, Graph::EdgeId orig_edge_id);

    //entfernt den Knoten, der an letzter Stelle in den Vektoren (_parents etc.) gespeichert ist
    // dieser Knoten darf keine Nachfolger haben, dies wird aber nicht geprüft
    //Eingabe: NodeId des zu entfernenden Knotens im zugrundeliegenden Graphen
    void remove_last_node(Graph::NodeId node_to_remove);

    //beinhaltet alle Daten, die zu einer Kante gehören
    struct TreeEdge {
        Insertion_Tree_Data_Structure::NodeId child;
        Insertion_Tree_Data_Structure::NodeId parent;
        Graph::EdgeWeight weight;
        Graph::EdgeId orig_edge_id;
    };


    //setzt die Eingabekante in die Datenstruktur ein
    // (überprüft dabei nicht, ob Kreise entstehen oder der Zusammenhang zerstört wird)
    void set_edge(const TreeEdge& edge_to_set);

    // repräsentiert einen (gerichteten) Pfad in dem Baum, definiert über die Endknoten
    struct TreePath {
        const Insertion_Tree_Data_Structure::NodeId startnode;
        const Insertion_Tree_Data_Structure::NodeId endnode;
    };

    //kehrt die Richtung aller Kanten auf dem Eingabepfad um
    // Start- bzw. Endknoten des Eingabepfads sind die ursprünglichen Start- bzw. Endknoten (und nicht die neuen)
    //dabei wird die ehemals eingehende Kante des (alten) Startknotens (implizit) gelöscht
    void reverse_path(Insertion_Tree_Data_Structure::TreePath path_to_reverse);


    // fügt die Eingabekante hinzu und entfernt die teuerste Kante auf dem Fundamentalkreis der Eingabekante,
    // falls dies eine echte Verbesserung darstellt
    // Eingabe: Kante aus dem zugrundeliegenden Graphen
    // Ausgabe: Wert der Verbesserung, hängt die entfernte Kante an removed_edges an
    Graph::PathLength try_to_insert_edge(const Graph::Edge& edge_to_insert,
                                         std::vector<TreeEdge>&  removed_edges,
                                         std::vector<TreePath>&  reversed_paths);

private:

    //speichert zu jedem Knoten einen Vorgängerknoten (die Wurzel erhält den ungueltigen Knoten)
    std::vector<Insertion_Tree_Data_Structure::NodeId> _parents;

    //speichert zu jedem Knoten das Gewicht der eingehenden Kante (die Wurzel erhält negatives unendliches Gewicht)
    std::vector<Graph::EdgeWeight> _weights_of_ingoing_edges;

    //speichert zu jedem Knoten die EdgeId bzgl des zugrundeliegenden Graphen (nicht des Graphen,
    // den die Datenstruktur repräsentiert) der eingehenden Kante (die Wurzel erhält die ungültige EdgeId)
    std::vector<Graph::EdgeId> _original_edge_ids_of_ingoing_edges;

    //speichert zu jedem Knoten aus dem zugrundeliegenden Graphen (nicht der Graph, den die Datenstruktur repräsentiert)
    // die NodeId in der Datenstruktur
    std::vector<Insertion_Tree_Data_Structure::NodeId> _tree_nodeids_of_original_nodes;


    // gibt die eingehende Kante des Eingabeknoten aus
    TreeEdge get_ingoing_tree_edge(Insertion_Tree_Data_Structure::NodeId input_node) const;

    // berechnet den Abstand des Eingabeknotens zur Wurzel
    Depth compute_depth(Insertion_Tree_Data_Structure::NodeId input_node) const;

};


#endif //PRAKTIKUMSTEINERBAUM_INSERTION_TREE_DATA_STRUCTURE_H
