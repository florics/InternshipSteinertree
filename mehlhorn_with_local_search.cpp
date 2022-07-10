//
// Created by Lukas on 10.07.2022.
//

#include "mehlhorn_with_local_search.h"

#include "mehlhorns_algo.h"
#include "local_search_combinations.h"
#include "steiner_vertex_elimination.h"
#include "graph_printfunctions.h"
#include "Subgraph.h"

void Mehlhorn_with_LocalSearch::complete_algorithm(const Graph &input_graph) {

    Subgraph solution_subgraph = Mehlhorn::complete_algorithm(input_graph);

    LocalSearchCombi::find_local_minimum_with_svi_and_kvekpe_in_sequence_each_pass(solution_subgraph);

    SteinerVertexElim::find_local_minimum(solution_subgraph);

    //todo auskommentieren rückgängig
    //GraphAuxPrint::print_subgraph(solution_subgraph);
    GraphAuxPrint::print_length_of_all_edges(solution_subgraph.this_graph());

}
