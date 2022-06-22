//
// Created by Lukas on 22.06.2022.
//

#ifndef PRAKTIKUMSTEINERBAUM_LOCAL_SEARCH_AUX_H
#define PRAKTIKUMSTEINERBAUM_LOCAL_SEARCH_AUX_H

#include "vector"
#include "utility"
#include "queue"

#include "graph.h"
#include "voronoi_diagram.h"

namespace LocalSearch{

    // Vektor, der zu jedem Knoten einer Lösung einen Heap mit boundary edges speichert
    using BoundEdgeHeaps = std::vector< std::priority_queue< std::pair<Graph::PathLength, Graph::EdgeId>>>;

    //fügt die boundary edges des Voronoi-Diagramms zu den Heaps hinzu,
    // sodass jeder Heap eines Knotens alle zur entsprechenden V.-Region inzidenten boundary edges enthält
    void initialise_heaps_w_bound_edges( const Voronoi_diagram& input_vd,
                                         BoundEdgeHeaps& input_heap_vect );

    //findet den Key-Path, der in start_node endet, sowie dessen Länge
    // Ausgabe: anderer Endpunkt dieses Key-Paths
    // Eingabe: Graph muss gerichtet sein
    Graph::NodeId find_crucial_parent (const Graph& input_graph,
                                       Graph::NodeId start_node,
                                       std::vector<Graph::NodeId>& internal_node_ids,
                                       Graph::PathLength& key_path_length);

    // gibt 1 aus gdw. Eingabeknoten ein crucial node im Eingabegraphen ist
    bool check_if_crucial (const Graph::Node& input_node);


}


#endif //PRAKTIKUMSTEINERBAUM_LOCAL_SEARCH_AUX_H
