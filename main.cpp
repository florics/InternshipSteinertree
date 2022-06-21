#include <iostream>

#include "graph.h"
#include "graph_printfunctions.h"
#include "graph_aux_functions.h"
#include "graph_algorithms.h"
#include "voronoi_diagram.h"
#include "vor_diag_aux_functions.h"
#include "mehlhorns_algo.h"

int main(int argc, char *argv[]) {
    if(argc!=2) {
        std::cout << "Kein Dateiname eingegeben\n";
        return 1;
    }


    Graph g( argv[1] );
    //print_graph(g);

    const std::vector<Graph::NodeId>& base1 = g.get_vect_term();

    std::vector<Graph::NodeId> base_first_half;
    std::cout << "base_first_half: \n";
    for(unsigned int i = 0; i < base1.size()/2; i++){
        base_first_half.push_back( base1[i] );
        std::cout << base1[i] << ", ";
    }
    std::cout << "\n";
    std::vector<Graph::NodeId> base_second_half;
    std::cout << "base_second_half: \n";
    for(unsigned int i = base1.size()/2; i < base1.size(); i++){
        base_second_half.push_back( base1[i] );
        std::cout << base1[i] << ", ";
    }
    std::cout << "\n";

    std::vector<Graph::NodeId> base4;
    for(unsigned int i = 0; i < base1.size(); i=i+2){
        base4.push_back( base1[i] );
    }

    Voronoi_diagram my_vd1(base1, g);
    Voronoi_diagram my_vd1_copy(base1, g);
    Voronoi_diagram my_vd2(base_first_half, g);
    Voronoi_diagram my_vd3(base_second_half, g);

    std::cout << "für {2, 7}: ";
    print_edge_as_pair(my_vd1.original_graph(), vd_get_min_bound_edge_inc_to_nodeset(my_vd1, {1, 6}));
    std::cout << "\n";

    std::cout << "für {1}: ";
    print_edge_as_pair(my_vd1.original_graph(), vd_get_min_bound_edge_inc_to_nodeset(my_vd1, {0}));
    std::cout << "\n";

    std::cout << "für base1: ";
    print_edge_as_pair(my_vd1.original_graph(), vd_get_min_bound_edge_inc_to_nodeset(my_vd1, base1));
    std::cout << "\n";



    return 0;
}

// C:\\Users\\Lukas\\CLionProjects\\PraktikumSteinerbaum\\Test-Instanzen\\graph-vd1.txt
// "C:\\Users\\Lukas\\CLionProjects\\PraktikumSteinerbaum\\Test-Instanzen\\test-graph1.txt"
// C:\\Users\\Lukas\\CLionProjects\\PraktikumSteinerbaum\\Test-Instanzen4\\rect_instance_1.stp
// C:\Users\Lukas\Documents\Uni\Mathestudium\Programmierpraktikum\Praxis\Instanzen\13_hard\rect_instance_1.stp
// C:\Users\Lukas\Documents\Uni\Mathestudium\Programmierpraktikum\Praxis\Instanzen\SteinLib-Website\1R\1r111.stp
// C:\Users\Lukas\Documents\Uni\Mathestudium\Programmierpraktikum\Praxis\Instanzen\SteinLib-Website\X\berlin52.stp