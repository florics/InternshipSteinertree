//
// Created by Lukas on 17.06.2022.
//

#ifndef PRAKTIKUMSTEINERBAUM_VOR_DIAG_AUX_FUNCTIONS_H
#define PRAKTIKUMSTEINERBAUM_VOR_DIAG_AUX_FUNCTIONS_H

#include "voronoi_diagram.h"

#include "EdgeSequence.h"

namespace VorDiagAux{

    //gibt boundary edges (bzgl des eingegebenen Vor.-Diag.) aus, die mit der eingegebenen Knotenmenge inzident sind,
    // ausgegeben als EdgeIds im zugrundeliegenden Graphen
    //Eingabe: input_nodes muss Teilmenge der Knoten des zugrundeliegenden Graphen des Eingabe-Vor-Diag. sein
    // (wird aber nicht explizit geprüft)
    std::vector<Graph::EdgeId> get_bound_edges_inc_to_nodeset(const Voronoi_diagram& vor_diag,
                                                              const std::vector<Graph::NodeId>& input_nodes);

    //berechnet den zur Eingabekante (muss boundary edge sein) gehörenden boundary path (nicht in korrekter Reihenfolge)
    // (ist die Eingabe die ungueltigen EdgeIdm, wird ein leerer Vektor ausgegeben)
    std::vector<Graph::EdgeId> compute_bound_path(const Voronoi_diagram& original_vd,
                                                  Graph::EdgeId bound_edge_id);

    //gibt die Basen der Voronoi-Regionen aus, zu denen die Endpunkte der Eingabekante gehören
    std::pair<Graph::NodeId, Graph::NodeId> get_bases_of_edge(const Voronoi_diagram& original_vd,
                                                              Graph::EdgeId input_edgeid);

    // berechnet die Länge des zur Eingabekante (die muss eine boundary edge sein) gehörenden boundary path
    Graph::PathLength compute_length_of_boundpath(const Voronoi_diagram& original_vd, Graph::EdgeId input_edgeid);

}

#endif //PRAKTIKUMSTEINERBAUM_VOR_DIAG_AUX_FUNCTIONS_H
