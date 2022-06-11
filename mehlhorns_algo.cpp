//
// Created by Lukas on 11.06.2022.
//

#include "mehlhorns_algo.h"

Graph mehlhorns_algo(Graph& original_graph){

    Voronoi_diagram vor_diag(original_graph.get_vect_term(), original_graph);
    std::vector<std::vector<std::pair<Graph::EdgeId, Graph::PathLength>>> vect_min_bound_edges = vor_diag.min_bound_edges(original_graph);
    Graph aux_graph = vor_diag.construct_aux_graph(original_graph);

    Graph mst_aux_graph = aux_graph.mst_prim(0);

    Graph sub_mst_aux_graph = vor_diag.turn_into_subgraph( mst_aux_graph, original_graph, vect_min_bound_edges);

    Graph mst_sub_mst_aux_graph = sub_mst_aux_graph.mst_prim(0);

    Graph output = mst_sub_mst_aux_graph.copygraph_wo_steinerleafs();

    return output;
}

