//
// Created by Lukas on 26.06.2022.
//

#include "tests.h"

void DebugTests::kpe_evaluate_neighborhood(){
    Graph or_g( "C:\\Users\\Lukas\\CLionProjects\\PraktikumSteinerbaum\\Test-Instanzen\\KeyPathExchange-Tests\\kpe-or_graph3.txt" );
    Graph sol_g( "C:\\Users\\Lukas\\CLionProjects\\PraktikumSteinerbaum\\Test-Instanzen\\KeyPathExchange-Tests\\kpe-sol_graph3.txt" );
    //print_graph(g);

    std::vector<Graph::NodeId>  solution_nodeids_of_original_nodes {4, 5, 3, 2, Graph::invalid_node_id, 1, 0, 6};

    std::pair< std::vector<bool>, std::vector<bool> > subgraph_vectors;
    subgraph_vectors.first = {1, 1, 1, 1, 0, 1, 1, 1};
    subgraph_vectors.second = {1, 1, 1, 0, 1, 0, 1, 1};

    //std::vector<ImprovingChangement> im_ch = KeyPathExch::evaluate_neighborhood_simple(or_g, sol_g, subgraph_vectors, solution_nodeids_of_original_nodes);

}