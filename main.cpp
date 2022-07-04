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
#include "key_vertex_elimination.h"
#include "Tree_Data_Structure.h"

//int main() {
int main(int argc, char *argv[]) {

    /*
    if(argc!=2) {
        std::cout << "Kein Dateiname eingegeben\n";
        return 1;
    }*/

    //Graph g( "C:\\Users\\Lukas\\Documents\\Uni\\Mathestudium\\Programmierpraktikum\\Praxis\\Instanzen\\SteinLib-Website\\1r111.stp" );
    //GraphAuxPrint::print_graph(g);


    std::vector<Graph> my_graphs = DebugTests::get_test_instances_from_file("C:\\Users\\Lukas\\CLionProjects\\PraktikumSteinerbaum\\Pfade_von_TestInstanzen\\I080");
    int improve_counter = 0;
    for(unsigned int i=0; i<my_graphs.size(); i++) {

        std::cout << i << "\n";

        Subgraph sol_sg = mehlhorns_algo(my_graphs[i]);

        Graph::PathLength value_before = GraphAux::length_of_all_edges(sol_sg.getThisGraph());
        GraphAuxPrint::print_length_of_all_edges(sol_sg.getThisGraph());

        KeyVertexElim::complete_algorithm(sol_sg);

        Graph::PathLength value_after = GraphAux::length_of_all_edges(sol_sg.getThisGraph());
        GraphAuxPrint::print_length_of_all_edges(sol_sg.getThisGraph());

        if( value_after < value_before) {
            improve_counter ++;
        }
    }

    std::cout << improve_counter << " von " << my_graphs.size() << " Instanzen wurden verbessert. \n";

    return 0;
}

// C:\Users\Lukas\CLionProjects\PraktikumSteinerbaum\Test-Instanzen\VD\graph-vd1.txt
// C:\Users\Lukas\CLionProjects\PraktikumSteinerbaum\Test-Instanzen\graph-solution.txt
// C:\Users\Lukas\CLionProjects\PraktikumSteinerbaum\Test-Instanzen\test-graph1.txt
// C:\Users\Lukas\Documents\Uni\Mathestudium\Programmierpraktikum\Praxis\Instanzen\SteinLib-Website\1R\1r111.stp
// C:\Users\Lukas\Documents\Uni\Mathestudium\Programmierpraktikum\Praxis\Instanzen\SteinLib-Website\B\b01.stp
// C:\Users\Lukas\Documents\Uni\Mathestudium\Programmierpraktikum\Praxis\Instanzen\SteinLib-Website\X\berlin52.stp