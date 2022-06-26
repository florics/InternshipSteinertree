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


namespace KeyPathExch{

    //Eingabe: subgraph_vectors gibt an, ob Kante bzw. Knoten aus zugrundeliegendem Graphen in der aktuellen Lösung liegt,
    // node_ids_solution gibt an, welche node_id ein Knoten aus dem zugrundeliegendem Graphen in der aktuellen Lösung besitzt
    void key_path_exchange( Graph& original_graph,
                            Graph& solution,
                            std::pair< std::vector<bool>, std::vector<bool> >& subgraph_vectors,
                            std::vector<Graph::NodeId> node_ids_solution);

    //Eingabe: subgraph_vectors gibt an, ob Kante bzw. Knoten aus zugrundeliegendem Graphen in der aktuellen Lösung liegt,
    // solution_nodeids_of_original_nodes gibt an, welche node_id ein Knoten aus dem zugrundeliegendem Graphen in der aktuellen Lösung besitzt
    //? brauche ich noch std::pair< std::vector<bool>, std::vector<bool> >& subgraph_vectors, ?
    std::vector<ImprovingChangement> evaluate_neighborhood(Graph &original_graph,
                                                           Graph &solution,
                                                           std::pair< std::vector<bool>, std::vector<bool> >& subgraph_vectors,
                                                           std::vector<Graph::NodeId> solution_nodeids_of_original_nodes);

    ImprovingChangement process_node(Graph::NodeId input_node_id,
                                      const Graph& original_graph,
                                      const Graph& solution_graph,
                                      Voronoi_diagram& vor_diag,
                                      Union_Find_Structure& subtrees_ufs,
                                      BoundEdgeHeaps& bound_edge_heaps);

    std::pair<Graph::PathLength, Graph::EdgeId> find_best_original_bound_edge();

}

#endif //PRAKTIKUMSTEINERBAUM_KEY_PATH_EXCHANGE_H
