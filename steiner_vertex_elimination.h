//
// Created by Lukas on 05.07.2022.
//

#ifndef PRAKTIKUMSTEINERBAUM_STEINER_VERTEX_ELIMINATION_H
#define PRAKTIKUMSTEINERBAUM_STEINER_VERTEX_ELIMINATION_H

#include "vector"

#include "local_search_aux.h"
#include "Subgraph.h"
#include "ImprovingChangement.h"

namespace SteinerVertexElim {

    // gibt die Nachbarschaft der Eingabe-Lösung aus (als Veränderungen ausgehend von der Eingabe-Lösung,
    // siehe auch Ausgabe von process_node)
    //falls ein move per pass eingegeben wird, so wird die gesamte Nachbarschaft ausgegeben
    //falls mehrere moves per pass eingegeben wird, so wird eine Reihe von Verbesserungen ausgegeben,
    // die simultan ausgeführt werden können
    std::vector<ImprovingChangement> evaluate_neighborhood(Subgraph& input_subgraph,
                                                           LocalSearchAux::MovesPerPass moves_per_pass);

    //verbessert die Eingabe-Lösung bis zu einem lokalen Optimum
    // verwendet dabei mehrere moves per pass
    void find_local_minimum(Subgraph& input_subgraph);

}


#endif //PRAKTIKUMSTEINERBAUM_STEINER_VERTEX_ELIMINATION_H
