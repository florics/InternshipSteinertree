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

    if( g.dir_type() == Graph::undirected) {
        g.make_rooted_arborescence(1);
        std::cout << "erster Schritt \n";
    }
    if( g.dir_type() == Graph::directed) {
        print_graph(g);
    }



    return 0;
}

// C:\\Users\\Lukas\\CLionProjects\\PraktikumSteinerbaum\\Test-Instanzen\\graph-vd1.txt
// C:\Users\Lukas\CLionProjects\PraktikumSteinerbaum\Test-Instanzen\graph-tree.txt
// C:\\Users\\Lukas\\CLionProjects\\PraktikumSteinerbaum\\Test-Instanzen4\\rect_instance_1.stp
// C:\Users\Lukas\Documents\Uni\Mathestudium\Programmierpraktikum\Praxis\Instanzen\13_hard\rect_instance_1.stp
// C:\Users\Lukas\Documents\Uni\Mathestudium\Programmierpraktikum\Praxis\Instanzen\SteinLib-Website\1R\1r111.stp
// C:\Users\Lukas\Documents\Uni\Mathestudium\Programmierpraktikum\Praxis\Instanzen\SteinLib-Website\X\berlin52.stp