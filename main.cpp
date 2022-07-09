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
#include "Edge_Heaps.h"
#include "tests.h"
#include "key_vertex_elimination.h"
#include "NCA_Tree_Data_Structure.h"
#include "steiner_vertex_elimination.h"
#include "Insertion_Tree_Data_Structure.h"
#include "steiner_vertex_insertion.h"


int main(int argc, char *argv[]) {

/*
    if(argc!=2) {
        std::cout << "Kein Dateiname eingegeben\n";
        return 1;
    }


    Graph g( argv[1] );
    //GraphAuxPrint::print_graph(g);

    //Berechnung eines Steinerbaums
    Subgraph sol_sg = Mehlhorn::complete_algorithm(g);

    //GraphAuxPrint::print_length_of_all_edges(sol_sg.this_graph());


    //Verbesserung durch lokale Suchen (Reihenfolge nicht klar)
    SteinerVertexElim::complete_algorithm(sol_sg);
    SteinerVertexInsertion::find_local_minimum(sol_sg);
    KeyVertexElim::complete_algorithm(sol_sg);
    KeyPathExch::complete_algorithm(sol_sg);

    //GraphAuxPrint::print_length_of_all_edges(sol_sg.this_graph());

    //Ausgabe der Lösung
    GraphAuxPrint::print_subgraph(sol_sg);

*/
    //für mehrere Instanzen auf einmal:

    std::vector<Graph> my_graphs =
          DebugTests::get_test_instances_from_file("C:\\Users\\Lukas\\CLionProjects\\PraktikumSteinerbaum\\Pfade_von_TestInstanzen\\I080");
    //std::vector<Graph> my_graphs = DebugTests::get_standard_test_instances();


    //int improve_counter = 0;
    for(unsigned int i=0; i<my_graphs.size(); i++) {

        std::cout << i << "\n";
        fflush(stdout);

        Subgraph sol_sg = Mehlhorn::complete_algorithm(my_graphs[i]);

        //GraphAuxPrint::print_subgraph(sol_sg);

        //Graph::PathLength value_before = GraphAux::length_of_all_edges(sol_sg.this_graph());
        //GraphAuxPrint::print_length_of_all_edges(sol_sg.this_graph());


        //SteinerVertexElim::complete_algorithm(sol_sg);
        //SteinerVertexInsertion::find_local_minimum(sol_sg);
        KeyVertexElim::complete_algorithm(sol_sg);
        KeyPathExch::complete_algorithm(sol_sg);;


        //Graph::PathLength value_after = GraphAux::length_of_all_edges(sol_sg.this_graph());
        //GraphAuxPrint::print_length_of_all_edges(sol_sg.this_graph());

        //if( value_after < value_before) {
            //improve_counter ++;
        //}

    }

    //std::cout << improve_counter << " von " << my_graphs.size() << " Instanzen wurden verbessert. \n";


    return 0;
}

// C:\Users\Lukas\CLionProjects\PraktikumSteinerbaum\Test-Instanzen\VD\graph-vd1.txt
// C:\Users\Lukas\CLionProjects\PraktikumSteinerbaum\Test-Instanzen\graph-solution.txt
// C:\Users\Lukas\CLionProjects\PraktikumSteinerbaum\Test-Instanzen\test-graph1.txt
// C:\Users\Lukas\Documents\Uni\Mathestudium\Programmierpraktikum\Praxis\Instanzen\SteinLib-Website\1R\1r111.stp
// C:\Users\Lukas\Documents\Uni\Mathestudium\Programmierpraktikum\Praxis\Instanzen\SteinLib-Website\B\b01.stp
// C:\Users\Lukas\Documents\Uni\Mathestudium\Programmierpraktikum\Praxis\Instanzen\SteinLib-Website\X\berlin52.stp