//
// Created by Lukas on 17.06.2022.
//

#ifndef PRAKTIKUMSTEINERBAUM_VOR_DIAG_AUX_FUNCTIONS_H
#define PRAKTIKUMSTEINERBAUM_VOR_DIAG_AUX_FUNCTIONS_H

#include "voronoi_diagram.h"

/*
//gibt 1 aus gdw beide vd's die gleichen Basen haben, die gleiche Partition der Knoten auf die Regionen (_base) haben,
// zu den Knoten die gleichen Abstände zur jeweiligen Basis speichern (_dist_to_base)
// prüft aber nicht, ob _predecessor gleich
//Eingabe: 2 Voronoi-Diagramme mit gleichem zugrundeliegenden Graphen (wird nicht geprüft, da zu aufwendig)
bool check_if_vor_diag_equal(Voronoi_diagram vd1, Voronoi_diagram vd2);
*/

//gibt 1 aus gdw. beide vd's die gleiche Partition der Knoten auf die Regionen (_base) haben
// (damit wird auch geprüft, ob Menge der Basen gleich ist)
//Eingabe: 2 Voronoi-Diagramme mit gleichem zugrundeliegenden Graphen (wird nicht geprüft, da zu aufwendig)
bool check_if_vor_regions_equal(const Voronoi_diagram& vd1, const Voronoi_diagram& vd2);

//gibt 1 aus gdw. beide vd's zu den Knoten die gleichen Abstände zur jeweiligen Basis speichern (_dist_to_base)
//Eingabe: 2 Voronoi-Diagramme mit gleichem zugrundeliegenden Graphen (wird nicht geprüft, da zu aufwendig)
bool check_if_vor_dist_equal(const Voronoi_diagram& vd1, const Voronoi_diagram& vd2);

//ruft check_if_vor_regions_equal, check_if_vor_dist_equal auf und gibt Ergebnisse auf Konsole aus
void compare_vor_diag(const Voronoi_diagram& vd1, const Voronoi_diagram& vd2);

#endif //PRAKTIKUMSTEINERBAUM_VOR_DIAG_AUX_FUNCTIONS_H
