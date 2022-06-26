//
// Created by Lukas on 22.06.2022.
//

#ifndef PRAKTIKUMSTEINERBAUM_LOCAL_SEARCH_AUX_H
#define PRAKTIKUMSTEINERBAUM_LOCAL_SEARCH_AUX_H

#include "vector"
#include "utility"
#include "queue"
//? #include "functional"

#include "graph.h"
#include "voronoi_diagram.h"
#include "EdgeSequence.h"

namespace LocalSearchAux{

    //Ausgabe: die crucial nodes des Eingabegraphens in einer post-order von dem Eingabeknoten aus (so dass children immer vor ihrem parent stehen)
    // aber ohne den Eingabeknoten selbst (!)
    //Laufzeit: könnte man auch gleich in Graph::make_rooted_arborescence ausgeben
    std::vector<Graph::NodeId> get_crucialnodes_in_postorder(const Graph& input_graph, Graph::NodeId root_id);


    //findet den Key-Path, der in start_node endet
    // Ausgabe: Key-Path als EdgeSequence (endnode_a ist der crucial parent des Startknoten)(internal_node_ids speichert in post-order)
    // Eingabe: Graph muss Arboreszenz mit einem Terminal als Wurzel sein
    // start_node muss crucial node sein
    // internal_node_ids wird überschrieben mit den inneren Knoten des Key-Path (internal_node_ids speichert in post-order)
    EdgeSequence find_ingoing_keypath (const Graph& input_graph,
                                       Graph::NodeId start_node,
                                       std::vector<Graph::NodeId>& internal_node_ids);

    //gibt 1 aus gdw. Eingabeknoten ein crucial node im Eingabegraphen ist
    bool check_if_crucial (const Graph::Node& input_node);

    // findet alle Kanten, die nach Entfernen der bases_to_delete boundary edges werden
    // todo: nach repair ein restore durchführen, um so das vd nicht kopieren zu müssen
    std::vector<EdgeSequence> get_new_bound_paths(Voronoi_diagram input_vd, const std::vector<Graph::NodeId>& bases_to_delete);

}


#endif //PRAKTIKUMSTEINERBAUM_LOCAL_SEARCH_AUX_H
