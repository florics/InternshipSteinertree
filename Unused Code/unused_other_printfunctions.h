//
// Created by Lukas on 10.07.2022.
//

#ifndef PRAKTIKUMSTEINERBAUM_UNUSED_OTHER_PRINTFUNCTIONS_H
#define PRAKTIKUMSTEINERBAUM_UNUSED_OTHER_PRINTFUNCTIONS_H

#include "../graph.h"
#include "../voronoi_diagram.h"
#include "../Subgraph.h"
#include "../ImprovingChangement.h"

namespace OtherPrint {

    //Ausgabe auf Konsole für jeden Knoten: NodeId, Basis, Distanz zur Basis, Vorgängerknoten auf kürz. Weg von Basis
    void print_simple(const Voronoi_diagram& input_vordiag);
    //Ausgabe auf Konsole: Voronoi-Region der Basis var_base (var_base muss Basis sein)
    //geht dabei alle Knoten durch
    void print_region_naiv(const Voronoi_diagram& input_vordiag, Graph::NodeId input_base);
    //Ausgabe auf Konsole: Voronoi-Region der Basis var_base (var_base muss Basis sein)
    // benutzt die Funktion compute_vor_region
    void print_region_fast(Voronoi_diagram& input_vordiag, Graph::NodeId input_base);

}

#endif //PRAKTIKUMSTEINERBAUM_UNUSED_OTHER_PRINTFUNCTIONS_H
