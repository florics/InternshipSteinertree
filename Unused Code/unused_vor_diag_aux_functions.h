//
// Created by Lukas on 10.07.2022.
//

#ifndef PRAKTIKUMSTEINERBAUM_UNUSED_VOR_DIAG_AUX_FUNCTIONS_H
#define PRAKTIKUMSTEINERBAUM_UNUSED_VOR_DIAG_AUX_FUNCTIONS_H

#include "../voronoi_diagram.h"
#include "../EdgeSequence.h"

namespace VorDiagAux {

    //gibt boundary edges (bzgl des eingegebenen Vor.-Diag.) aus, die mit der eingegebenen Knotenmenge inzident sind
    // Eingabe: input_nodes muss Teilmenge der Knoten des zugrundeliegenden Graphen des Vor-Diag. vd sein (wird aber nicht explizit geprüft)
    std::vector<EdgeSequence> get_bound_paths_inc_to_nodeset(const Voronoi_diagram &vd, const std::vector<Graph::NodeId> &input_nodes);

    //gibt alle boundary edges des Eingabe-Vor-Diagramm als EdgeIds aus
    std::vector<Graph::EdgeId> get_all_bound_edges(const Voronoi_diagram& input_vd);

    //berechnet den zur Eingabekante (muss bound edge sein) gehörenden boundary path,
    EdgeSequence compute_bound_path_as_edgesequ(const Voronoi_diagram& original_vd, Graph::EdgeId bound_edge_id);

    //gibt 1 aus gdw. beide Voronoi-Diagramme die gleiche Partition der Knoten auf die Regionen (_base) haben
    // (damit wird auch geprüft, ob Menge der Basen gleich ist)
    //Eingabe: 2 Voronoi-Diagramme mit gleichem zugrundeliegenden Graphen (wird nicht geprüft, da zu aufwendig)
    bool check_if_vor_regions_equal(const Voronoi_diagram &vd1, const Voronoi_diagram &vd2);
    //gibt 1 aus gdw. beide vd's zu den Knoten die gleichen Abstände zur jeweiligen Basis speichern (_dist_to_base)
    //Eingabe: 2 Voronoi-Diagramme mit gleichem zugrundeliegenden Graphen (wird nicht geprüft, da zu aufwendig)
    bool check_if_vor_dist_equal(const Voronoi_diagram &vd1, const Voronoi_diagram &vd2);
    //ruft check_if_vor_regions_equal, check_if_vor_dist_equal auf und gibt Ergebnisse auf Konsole aus
    void compare_vor_diag(const Voronoi_diagram &vd1, const Voronoi_diagram &vd2);

}

#endif //PRAKTIKUMSTEINERBAUM_UNUSED_VOR_DIAG_AUX_FUNCTIONS_H
