//
// Created by Lukas on 02.06.2022.
//

#ifndef PRAKTIKUMSTEINERBAUM_VORONOI_DIAGRAM_H
#define PRAKTIKUMSTEINERBAUM_VORONOI_DIAGRAM_H

#include "vector"

#include "graph.h"

class Voronoi_diagram {

public:

    //berechnet das Voronoi-Diagramm des Graphen, mit den Knoten in set_of_bases als Basen
    //Eingabe: set_of_b ist nichtleere Teilmenge der Knoten von g, g hat nichtnegative "endliche" Kantengewichte
    Voronoi_diagram(const std::vector<Graph::NodeId>& set_of_bases,
                    const Graph& input_graph);

    //prüft ob var_node Basis ist (genauer: prüft, ob der Eingabeknoten sich selbst als Basis hat)
    //Eingabe: Knoten, der im Graphen liegt oder Knoten mit ungültiger NodeId
    bool check_if_base(Graph::NodeId var_node) const;

    //prüft, ob Kante eine boundary edge ist (d. h. die Endknoten liegen in verschiedenen V.-Regionen)
    // (liegt einer der Endknoten in keiner V-Region, so wird 'false' ausgegeben)
    //Eingabe: Kante, die zwischen 2 Knoten verläuft, die beide im Graphen liegen
    bool check_if_bound_edge(const Graph::Edge& var_edge) const;

    const Graph& original_graph() const;
    const std::vector<Graph::NodeId>& base() const;
    const std::vector<Graph::PathLength>& dist_to_base() const;
    const std::vector< std::pair< Graph::NodeId, Graph::EdgeId>>& predecessor() const;
    unsigned int num_bases() const;

    //gibt Menge der Basen aus (muss aber erst berechnet werden)
    std::vector<Graph::NodeId> compute_set_of_bases() const;
    //gibt Vektor mit Eintrag für jeden Knoten des zugrundeliegenden Graphen aus, in dem die Basen nummeriert werden,
    // andere Knoten erhalten invalid_node_id
    std::vector<Graph::NodeId> compute_base_ids() const;

    //gibt V-Region des Eingabeknotens aus (dieser muss Basis sein)
    std::vector<Graph::NodeId> compute_vor_region(Graph::NodeId input_base);
    //gibt Voronoi-Regionen der eingegebenen Knoten (müssen alle Basen sein) aus
    // (Ausgabe als eine Menge, ohne Differenzierung nach Basis/Region)
    std::vector<Graph::NodeId> compute_some_vor_regions(const std::vector<Graph::NodeId>& subset_of_bases);

    //Datenstruktur zum Speichern der alten Werte der Knoten, die bei repair aktualisiert werden
    struct RestoreData{
        std::vector<Graph::NodeId> node_ids;
        std::vector<Graph::NodeId> bases;
        std::vector<std::pair<Graph::NodeId, Graph::EdgeId>> predecessor;
        std::vector<Graph::PathLength> dist_to_base;
        unsigned int num_bases_deleted;
    };

    //entspricht der Methode repair, wie sie in dem Paper (Seite 4) beschrieben ist
    //Eingabe: echte Teilmenge der aktuellen Basis
    //Methode berechnet Voronoi-Diagramm mit einer neuen Basis,
    // die aus der alten durch Entfernen der Eingabemenge entsteht
    Voronoi_diagram::RestoreData repair(const std::vector<Graph::NodeId>& bases_to_delete);

    //setzt die Informationen aus der RestoreData-Eingabe in das Diagramm ein
    // stellt das alte Diagramm wieder her (wie vor repair), wenn die Ausgabe von repair eingegeben wird
    void restore(const RestoreData& input_restore_data);


private:

    unsigned int _num_bases;

    //speichert zu jedem Knoten die zugehörige Basis (an der Stelle, die der NodeId des Knotens entspricht)
    // (hat ein Knoten keine Basis, so wird invalid_node_id gespeichert)
    std::vector<Graph::NodeId> _base;

    //speichert zu jedem Knoten die Vorgänger auf kürzestem Weg von der Basis des Knoten zu dem Knoten
    //(an der Stelle, die der NodeId des Knotens entspricht)
    std::vector< std::pair< Graph::NodeId, Graph::EdgeId> > _predecessor;

    //speichert zu jedem Knoten die Länge des kürz. Weg von der Basis des Knoten zu dem Knoten
    //(an der Stelle, die der NodeId des Knotens entspricht)
    std::vector<Graph::PathLength> _dist_to_base;

    //der zugrundeliegende Graph
    const Graph& _original_graph;


    //macht, dass alle Basen sich selbst als Basis haben sowie dass ihr Abstand zur Basis 0 ist; setzt _num_bases
    void assign_bases(const std::vector<Graph::NodeId>& new_set_of_bases);

    // berechnet die restore-data für jeden Eingabeknoten (d. h. Eingabe entspricht V-Regionen und nicht Basen)
    RestoreData get_restoredata_of_nodeset (const std::vector<Graph::NodeId>& input_nodeids,
                                                  unsigned int num_bases_deleted);

};

#endif //PRAKTIKUMSTEINERBAUM_VORONOI_DIAGRAM_H
