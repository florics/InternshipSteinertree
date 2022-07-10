//
// Created by Lukas on 10.07.2022.
//

#include "unused_other_printfunctions.h"

#include "iostream"
#include "unused_graph_printfunctions.h"
#include "../graph_printfunctions.h"

void OtherPrint::print_simple(const Voronoi_diagram& input_vordiag) {
    std::cout << "Ausgabe des Voronoi-Diagramms (als NodeNames ausgegeben)\n";
    std::cout << "(Knoten) (Basis) (Distanz zur Basis) (Vorgaengerknoten und -kante auf kuerz. Weg von Basis) \n";
    for(Graph::NodeId i = 0; i<input_vordiag.original_graph().num_nodes(); i++){
        GraphAuxPrint::print_nodename(i);
        std::cout << " ";
        GraphAuxPrint::print_nodename( input_vordiag.base()[i]);
        std::cout << " ";
        GraphAuxPrint::print_pathlength(input_vordiag.dist_to_base()[i]);
        std::cout << " ";
        GraphAuxPrint::print_nodename( input_vordiag.predecessor()[i].first);
        std::cout << " ";
        GraphAuxPrint::print_edge_as_pair(input_vordiag.original_graph(), input_vordiag.predecessor()[i].second);
        std::cout << "\n";
    }
    std::cout << "\n";
}

void OtherPrint::print_region_naiv(const Voronoi_diagram& input_vordiag, Graph::NodeId input_base) {
    if(not input_vordiag.check_if_base(input_base)){
        std::cout << "(void Voronoi_diagram::print_region_naiv(Graph::NodeId var_base)) var_base = " << input_base << " ist keine Basis \n";
    }
    std::cout << "Die Voronoi-Region des Knotens ";
    GraphAuxPrint::print_nodename(input_base);
    std::cout << " ist: (als NodeNames ausgegeben)\n";
    std::cout << "(Knoten) (Distanz zur Basis) (Vorgaenger auf kuerz. Weg von Basis) \n";
    unsigned int region_num_nodes = 0;
    for(Graph::NodeId i=0; i<input_vordiag.original_graph().num_nodes(); i++){
        if(input_vordiag.base()[i] == input_base){
            GraphAuxPrint::print_nodename(i);
            std::cout << " ";
            GraphAuxPrint::print_pathlength(input_vordiag.dist_to_base()[i]);
            std::cout << " ";
            GraphAuxPrint::print_nodename(input_vordiag.predecessor()[i].first);
            std::cout << "\n";

            region_num_nodes++;
        }
    }
    std::cout << "Insgesamt liegen " << region_num_nodes << " Knoten in dieser Voronoi-Region. \n";
}

void OtherPrint::print_region_fast(Voronoi_diagram& input_vordiag, Graph::NodeId input_base) {
    if(not input_vordiag.check_if_base(input_base)){
        std::cout << "(void Voronoi_diagram::print_region_fast(Graph::NodeId var_base)) var_base = " << input_base << " ist keine Basis \n";
    }
    std::cout << "Die Voronoi-Region des Knotens ";
    GraphAuxPrint::print_nodename( input_base);
    std::cout << " ist: (als NodeNames ausgegeben)\n";
    std::cout << "(Knoten) (Distanz zur Basis) (Vorgaenger auf kuerz. Weg von Basis) \n";
    for(auto curr_node : input_vordiag.compute_vor_region(input_base)){
        GraphAuxPrint::print_nodename( curr_node);
        std::cout << " ";
        GraphAuxPrint::print_pathlength(input_vordiag.dist_to_base()[curr_node]);
        std::cout << " ";
        GraphAuxPrint::print_nodename( input_vordiag.predecessor()[curr_node].first);
        std::cout << "\n";
    }
    std::cout << "Insgesamt liegen " << input_vordiag.compute_vor_region(input_base).size() << " Knoten in dieser Voronoi-Region. \n";
}

