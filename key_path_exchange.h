//
// Created by Lukas on 22.06.2022.
//

#ifndef PRAKTIKUMSTEINERBAUM_KEY_PATH_EXCHANGE_H
#define PRAKTIKUMSTEINERBAUM_KEY_PATH_EXCHANGE_H

#include "vector"
#include "utility"
#include "queue"

#include "voronoi_diagram.h"
#include "graph.h"
#include "local_search_aux.h"
#include "ImprovingChangement.h"
#include "BoundEdgeHeaps.h"
#include "Union_Find_Structure.h"
#include "Subgraph.h"


namespace KeyPathExch{

    void complete_algorithm(Subgraph& input_subgraph);

    // gibt beste gefundene Nachbarschaftlösung aus (wenn keine echte Verbesserung gefunden wurde, wird die "leere" Verbesserung ausgegeben)
    ImprovingChangement best_neighbor_solution(Subgraph& input_subgraph);

    // berechnet die Nachbarschaft der Eingabelösung
    std::vector<ImprovingChangement> evaluate_neighborhood(Subgraph& input_subgraph, LocalSearchAux::MovesPerPass moves_per_pass);

    //Subroutine der main loop des Algorithmus
    ImprovingChangement process_node(Graph::NodeId input_node_id,
                                            const Subgraph& input_subgraph,
                                            Voronoi_diagram& vor_diag,
                                            Union_Find_Structure& subtrees_ufs,
                                            BoundEdgeHeaps& bound_edge_heaps,
                                            LocalSearchAux::MovesPerPass moves_per_pass,
                                            std::vector<bool>& forbidden,
                                            std::vector<bool>& pinned);

    std::pair<Graph::PathLength, Graph::EdgeId> compute_best_new_boundedge(const Voronoi_diagram& vor_diag,
                                                                           Union_Find_Structure& subtrees_ufs,
                                                                           const std::vector<Graph::NodeId>& solution_nodeids_of_original_nodes,
                                                                           Graph::NodeId input_node_id,
                                                                           const std::vector<Graph::NodeId>& nodes_updated_in_repair,
                                                                           LocalSearchAux::MovesPerPass moves_per_pass,
                                                                           const std::vector<bool>& forbidden);


    void update_heaps_and_ufs(BoundEdgeHeaps& bound_edge_heaps, Union_Find_Structure& subtrees_ufs,
                              Graph::NodeId input_node_id,
                              Graph::NodeId crucial_parent_id, const std::vector<Graph::NodeId>& internal_node_ids,
                              Union_Find_Structure::ElementId internal_nodes_ufsroot);
}

#endif //PRAKTIKUMSTEINERBAUM_KEY_PATH_EXCHANGE_H
