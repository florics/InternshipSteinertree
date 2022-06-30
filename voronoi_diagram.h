//
// Created by Lukas on 02.06.2022.
//

#ifndef PRAKTIKUMSTEINERBAUM_VORONOI_DIAGRAM_H
#define PRAKTIKUMSTEINERBAUM_VORONOI_DIAGRAM_H

#include "vector"

#include "graph.h"

class Voronoi_diagram {
public:
    using BaseId = unsigned int; //löschen?

    //kann ich die Eingabe der Konstruktoren iwie const machen?
    //erstellt ein Diagramm zur leeren Basis in einem Graphen mit num_n Knoten, checkt aber nicht, ob Kantengewichten negativ/unendlich sind
    //? diese fehlenden Checks könnten Problem sein für repair
    // ?löschen
    Voronoi_diagram(Graph& input_graph);
    //soll das Voronoi-Diagramm des Graphen g berechnen, mit den Knoten in set_of_b als Basen
    //Eingabe: set_of_b ist nichtleere Teilmenge der Knoten von g, g hat nichtnegative "endliche" Kantengewichte
    Voronoi_diagram(const std::vector<Graph::NodeId>& set_of_b,
                    const Graph& input_graph);

    // print-Funktionen später auslagern, manche löschen! ?
    //Ausgabe auf Konsole für jeden Knoten: NodeId, Basis, Distanz zur Basis, Vorgängerknoten auf kürz. Weg von Basis
    void print_simple() const;
    //Ausgabe auf Konsole: Voronoi-Region der Basis var_base (var_base muss Basis sein)
    //geht dabei alle Knoten durch
    void print_region_naiv(Graph::NodeId input_base) const;
    //Ausgabe auf Konsole: Voronoi-Region der Basis var_base (var_base muss Basis sein)
    // benutzt die Funktion compute_vor_region
    void print_region_fast(Graph::NodeId input_base);

    //macht, dass alle Basen sich selbst als Basis haben sowie dass ihr Abstand zur Basis 0 ist; setzt _num_bases
    //nach Aufruf dieser Methode ist das Voronoi-Diagramm ggf. nicht mehr korrekt
    void assign_bases(const std::vector<Graph::NodeId>& new_set_of_bases);

    //auslagern??
    //prüft ob var_node Basis ist
    //genauer: prüft, ob der Eingabeknoten sich selbst als Basis hat
    //Eingabe: Knoten, der im Graphen liegt oder Knoten mit ungültiger NodeId
    bool check_if_base(Graph::NodeId var_node) const;
    //prüft, ob Kante eine boundary edge ist (d. h. die Endknoten liegen in verschiedenen V.-Regionen, insbesondere liegen sie in einer V.-Region)
    //Eingabe: Kante, die zwischen 2 Knoten verläuft, die beide im Graphen liegen
    //(?)! prüft nicht, ob die eingegebene Kante wirklich im zugrundeliegenden Graphen liegt (zu aufwendig)
    bool check_if_bound_edge(const Graph::Edge& var_edge) const;

    const Graph& original_graph() const;
    const std::vector<Graph::NodeId>& base() const;
    const std::vector<Graph::PathLength>& dist_to_base() const;
    const std::vector< std::pair< Graph::NodeId, Graph::EdgeId>>& predecessor() const;
    unsigned int num_bases() const;

    //gibt Menge der Basen aus (muss aber erst berechnet werden)
    std::vector<Graph::NodeId> compute_set_of_bases() const;
    //gibt Vektor mit Eintrag für jeden Knoten des zugrundeliegenden Graphen aus, in dem die Basen nummeriert werden, andere Knoten erhalten invalid_node_id
    std::vector<Voronoi_diagram::BaseId> compute_base_ids() const;

    //gibt Voronoi-Region des eingegebenen Knoten (muss Basis sein) aus
    std::vector<Graph::NodeId> compute_vor_region(Graph::NodeId input_base);
    //gibt Voronoi-Regionen der eingegebenen Knoten (müssen alle Basen sein) aus (Ausgabe als eine Menge, ohne Differenzierung nach Basis/Region)
    std::vector< Graph::NodeId > compute_some_vor_regions(const std::vector<Graph::NodeId>& subset_of_bases);

    //Datenstruktur zum Speichern der alten Werte der Knoten, die bei repair aktualisiert werden
    struct RestoreData{
        std::vector<Graph::NodeId> node_ids;
        std::vector<Graph::NodeId> bases;
        std::vector<std::pair<Graph::NodeId, Graph::EdgeId>> predecessor;
        std::vector<Graph::PathLength> dist_to_base;
    };

    //entspricht der Methode repair, wie sie in dem Paper (Seite 4) beschrieben ist
    //Eingabe: Teilmenge der aktuellen Basis
    //Methode berechnet Voronoi-Diagramm mit einer neuen Basis, die aus der alten durch Entfernen der Eingabemenge entsteht
    Voronoi_diagram::RestoreData repair(const std::vector<Graph::NodeId>& bases_to_delete);
    // berechnet die restore-data für jeden Eingabeknoten (d. h. Eingabe entspricht V-Regionen und nicht Basen)
    RestoreData get_restoredata_of_nodeset (const std::vector<Graph::NodeId>& input_nodeids);
    // stellt das alte Diagramm wieder her (wie vor repair), wenn die Ausgabe von repair eingegeben wird
    // setzt die Informationen aus der RestoreData-Eingabe in das Diagramm ein
    void restore(const RestoreData& input_restore_data);


private:
    unsigned int _num_bases;

    //speichert zu jedem Knoten die zugehörige Basis (an der Stelle, die der NodeId des Knotens entspricht)
    std::vector<Graph::NodeId> _base;

    //speichert die Vorgänger auf kürzestem Weg von der Basis des Knoten zu dem Knoten
    //(an der Stelle, die der NodeId des Knotens entspricht)
    std::vector< std::pair< Graph::NodeId, Graph::EdgeId> > _predecessor;
    //speichert die Länge des kürz. Weg von der Basis des Knoten zu dem Knoten
    //(an der Stelle, die der NodeId des Knotens entspricht)
    std::vector<Graph::PathLength> _dist_to_base;

    // brauche ich _predecessor, _dist_to_base, ... für alle Berechnungen?

    //der zugrundeliegende Graph
    // möchte ich const machen aber Problem?
    const Graph& _original_graph;

    //Subroutine für die Funktion compute_vor_region
    //geht die Nachbarn des aktuellen Knoten durch und prüft, ob diese in der V-Region der eingegebenen Basis liegen
    // falls ja, so wird der Nachbar zu vor_region hinzugefügt und für diesen die Funktion rekursiv aufgerufen
    void compute_vor_region_subroutine(unsigned int input_base,
                                       unsigned int curr_node_id,
                                       std::vector<Graph::NodeId> &vor_region);
};

#endif //PRAKTIKUMSTEINERBAUM_VORONOI_DIAGRAM_H
