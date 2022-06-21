//
// Created by Lukas on 17.06.2022.
//

#ifndef PRAKTIKUMSTEINERBAUM_VOR_DIAG_AUX_FUNCTIONS_H
#define PRAKTIKUMSTEINERBAUM_VOR_DIAG_AUX_FUNCTIONS_H

#include "voronoi_diagram.h"

//gibt 1 aus gdw. beide vd's die gleiche Partition der Knoten auf die Regionen (_base) haben
// (damit wird auch geprüft, ob Menge der Basen gleich ist)
//Eingabe: 2 Voronoi-Diagramme mit gleichem zugrundeliegenden Graphen (wird nicht geprüft, da zu aufwendig)
bool check_if_vor_regions_equal(const Voronoi_diagram& vd1, const Voronoi_diagram& vd2);

//gibt 1 aus gdw. beide vd's zu den Knoten die gleichen Abstände zur jeweiligen Basis speichern (_dist_to_base)
//Eingabe: 2 Voronoi-Diagramme mit gleichem zugrundeliegenden Graphen (wird nicht geprüft, da zu aufwendig)
bool check_if_vor_dist_equal(const Voronoi_diagram& vd1, const Voronoi_diagram& vd2);

//ruft check_if_vor_regions_equal, check_if_vor_dist_equal auf und gibt Ergebnisse auf Konsole aus
void compare_vor_diag(const Voronoi_diagram& vd1, const Voronoi_diagram& vd2);

//gibt die günstigste boundary edge (bzgl des eingegebenen Vor.-Diag.) aus, die mit der eingegebenen Knotenmenge inzident ist
// falls keine solche Kante existiert, so wird 'invalid_edge_id' ausgegeben
// Eingabe: input_nodes muss Teilmenge der Knoten des zugrundeliegenden Graphen des Vor-Diag. vd sein
//? in andere Datei verschieben?
Graph::EdgeId vd_get_min_bound_edge_inc_to_nodeset(const Voronoi_diagram& vd, const std::vector<Graph::NodeId>& input_nodes);

#endif //PRAKTIKUMSTEINERBAUM_VOR_DIAG_AUX_FUNCTIONS_H
