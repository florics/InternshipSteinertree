//
// Created by Lukas on 11.06.2022.
//

#include "mehlhorns_algo.h"
#include "graph_algorithms.h"
#include "graph_aux_functions.h"
#include "voronoi_diagram.h"

//? Graph const machen!
Graph mehlhorns_algo(Graph& original_graph){

    Voronoi_diagram vor_diag(original_graph.get_vect_term(), original_graph);
    std::vector<std::vector<std::pair<Graph::EdgeId, Graph::PathLength>>> vect_min_bound_edges = vor_diag.min_bound_edges();
    Graph aux_graph = vor_diag.construct_aux_graph();

    Graph mst_of_aux_graph = mst_prim(aux_graph, 0);

    Graph sub_mst_of_aux_graph = vor_diag.turn_into_subgraph( mst_of_aux_graph, vect_min_bound_edges);

    Graph mst_of_sub_mst_of_aux_graph = mst_prim(sub_mst_of_aux_graph, 0);

    Graph output = copygraph_wo_steinerleafs(mst_of_sub_mst_of_aux_graph);

    return output;
}

