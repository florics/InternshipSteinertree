#include <iostream>

#include "graph.h"
#include "graph_printfunctions.h"
#include "graph_aux_functions.h"
#include "graph_algorithms.h"
#include "voronoi_diagram.h"
#include "vor_diag_aux_functions.h"
#include "mehlhorns_algo.h"
#include "local_search_aux.h"
#include "key_path_exchange.h"
#include "EdgeSequence.h"
#include "BoundEdgeHeaps.h"
#include "tests.h"

int main(int argc, char *argv[]) {

    if(argc!=2) {
        std::cout << "Kein Dateiname eingegeben\n";
        return 1;
    }


    Graph g( argv[1] );
    //print_graph(g);

    Subgraph sol_sg = mehlhorns_algo(g);

    // print_graph(sol_subgraph.getThisGraph());
    print_length_of_all_edges(sol_sg.getThisGraph());
    double var = GraphAux::length_of_all_edges(sol_sg.getThisGraph());

    std::vector<ImprovingChangement> im_ch_vect = KeyPathExch::evaluate_neighborhood_simple(sol_sg);
    ImprovingChangement im_ch = KeyPathExch::best_neighbor_solution(sol_sg);

    return 0;
}

// C:\\Users\\Lukas\\CLionProjects\\PraktikumSteinerbaum\\Test-Instanzen\\graph-vd1.txt
// C:\Users\Lukas\CLionProjects\PraktikumSteinerbaum\Test-Instanzen\graph-solution.txt
// C:\Users\Lukas\CLionProjects\PraktikumSteinerbaum\Test-Instanzen\test-graph1.txt
// C:\\Users\\Lukas\\CLionProjects\\PraktikumSteinerbaum\\Test-Instanzen4\\rect_instance_1.stp
// C:\Users\Lukas\Documents\Uni\Mathestudium\Programmierpraktikum\Praxis\Instanzen\13_hard\rect_instance_1.stp
// C:\Users\Lukas\Documents\Uni\Mathestudium\Programmierpraktikum\Praxis\Instanzen\SteinLib-Website\1R\1r111.stp
// C:\Users\Lukas\Documents\Uni\Mathestudium\Programmierpraktikum\Praxis\Instanzen\SteinLib-Website\B\b01.stp
// C:\Users\Lukas\Documents\Uni\Mathestudium\Programmierpraktikum\Praxis\Instanzen\SteinLib-Website\X\berlin52.stp