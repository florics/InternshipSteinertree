//
// Created by Lukas on 11.06.2022.
//

#include "mehlhorns_algo.h"

#include "iostream"
#include "array"
#include "algorithm"

#include "graph_algorithms.h"
#include "graph_aux_functions.h"
#include "graph_printfunctions.h"
#include "general_aux_functions.h"
#include "vor_diag_aux_functions.h"

namespace Mehlhorn {

    //berechnet die boundary edges (1. Eintrag des pair), die auf kürz. Weg zwischen den entsprechenden Basen liegen,
    // sowie die Länge dieses Weges (2. Eintrag des pair)
    // (genauer: die Kanten (v,w), die _dist_to_base[v] + (v,w).weight() + _dist_to_base[w] minimieren)
    //Eintrag (i,j) der ausgegebenen "Matrix" entspricht den Basen i+1 und j (wobei i>=j)
    // (wenn man die Basen durchnummeriert (in der Reihenfolge ihrer NodeIds), wie in 'Voronoi_diagram::compute_base_ids')
    std::vector< std::vector< std::pair< Graph::EdgeId, Graph::PathLength >>>
    compute_min_bound_edges_of_vordiag(const Voronoi_diagram& input_vd);

    //erstellt den Hilfsgraphen (auxiliary graph) für Mehlhorns ALgo. (in dem Paper als G'_1 notiert)
    // die NodeIds der Knoten entsprechen der Durchnummerierung der Basen wie bei 'compute_min_bound_edges_of_vordiag'
    Graph construct_aux_graph_of_vordiag(const Voronoi_diagram& input_vd,
                                         const std::vector<std::vector<std::pair<
                                                 Graph::EdgeId, Graph::PathLength>>>& min_bound_edges);

    //gibt den dem eingebenen Graphen (in Mehlhorns Algo. der MST des Hilfsgraphen(auxiliary graph)) entsprechenden
    // Subgraphen des zugrundeliegenden Graphen aus
    //Eingabe: min_bound_edges ist in dem Format wie die Ausgabe der Funktion compute_min_bound_edges_of_vordiag
    // (so dass sich Eintrag (i,j) dieser 'Matrix' auf die Knoten des Hilfsgraphen mit NodeId i+1 und j bezieht)
    Subgraph turn_into_subgraph_of_vordiags_original_graph(const Voronoi_diagram& input_vd,
                                                           const Graph& graph_to_process,
                                                           const std::vector<std::vector<std::pair<
                                                                   Graph::EdgeId,Graph::PathLength>>>& min_bound_edges);

    //debug
    //ruft min_bound_edges auf und gibt die ausgegebene Matrix auf Konsole aus (debug)
    void print_min_bound_edges_of_vd(const Voronoi_diagram& input_vd);

}


Subgraph Mehlhorn::complete_algorithm(const Graph& input_graph) {

    Voronoi_diagram vor_diag(input_graph.get_terminals(), input_graph);

    std::vector<std::vector<std::pair<Graph::EdgeId, Graph::PathLength>>> min_bound_edges =
            Mehlhorn::compute_min_bound_edges_of_vordiag(vor_diag);


    Graph auxiliary_graph = Mehlhorn::construct_aux_graph_of_vordiag(vor_diag, min_bound_edges);


    if( not GraphAux::check_if_graph_is_connected(auxiliary_graph) ) {
        throw std::runtime_error("(Mehlhorn::complete_algorithm) Der auxiliary graph ist nicht zsmhängend, "
                                 "d. h. es liegen nicht alle Terminale in einer Zusammnehangskomponente.");
    }


    GraphAlgorithms::compute_mst_for_graphs(auxiliary_graph, 0);


    Subgraph solution_subgraph = Mehlhorn::turn_into_subgraph_of_vordiags_original_graph(vor_diag,
                                                                                         auxiliary_graph,
                                                                                         min_bound_edges);

    GraphAlgorithms::compute_mst_for_subgraphs(solution_subgraph, 0);

    GraphAux::remove_steinerbranches(solution_subgraph);


    return solution_subgraph;
}

std::vector<std::vector<std::pair<Graph::EdgeId, Graph::PathLength>>>
Mehlhorn::compute_min_bound_edges_of_vordiag(const Voronoi_diagram& input_vd) {

    //Initialisierung der Ausgabe-Matrix: output[i][j] entspricht den Basen i+1, j
    // (diese einfache Nummerierung der Basen wird hier mit base_ids gespeichert)
    std::vector<std::vector<std::pair<Graph::EdgeId, Graph::PathLength>>> output;
    output.reserve(input_vd.num_bases() -1);
    std::pair<Graph::EdgeId, Graph::PathLength> invalid_pair = {Graph::invalid_edge_id, Graph::infinite_length};
    for(unsigned int i = 0; i < input_vd.num_bases() -1; i++){
        std::vector<std::pair<Graph::EdgeId, Graph::PathLength>> var_vector (i+1, invalid_pair);
        output.push_back(var_vector);
    }

    //Schleife über alle Kanten des Graphen, die boundary edges sind
    const std::vector<Graph::Edge>& original_graph_edges = input_vd.original_graph().edges();
    const std::vector<Voronoi_diagram::BaseId> base_ids = input_vd.compute_base_ids();
    for(const auto& curr_edge : original_graph_edges){
        if( input_vd.check_if_bound_edge(curr_edge) ){

            //prüfe, ob aktuelle Kante besser als gespeicherte Kante (update falls ja)
            std::array<Graph::NodeId, 2> curr_bases = { base_ids[ input_vd.base()[curr_edge.node_a()] ],
                                                        base_ids[ input_vd.base()[curr_edge.node_b()] ] };
            std::sort(curr_bases.begin(), curr_bases.end());
            //debug
            if(curr_bases[0] > curr_bases[1]){
                throw std::runtime_error("(compute_min_bound_edges_of_vordiag) sort tut nicht das was es soll");
            }
            Graph::PathLength curr_dist = curr_edge.weight() + input_vd.dist_to_base()[ curr_edge.node_a() ] + input_vd.dist_to_base()[ curr_edge.node_b() ];
            if( curr_dist < output[ curr_bases[1]-1 ][ curr_bases[0] ].second){
                output[ curr_bases[1]-1 ][ curr_bases[0] ].first = curr_edge.edge_id();
                output[ curr_bases[1]-1 ][ curr_bases[0] ].second = curr_dist;
            }

        }
    }

    return output;
}

Graph Mehlhorn::construct_aux_graph_of_vordiag(const Voronoi_diagram& input_vd,
                                               const std::vector<std::vector<std::pair<Graph::EdgeId,
                                          Graph::PathLength>>>& min_bound_edges) {
    Graph output(0);

    //Laufzeit? O(n)
    const std::vector<Graph::NodeId> set_of_bases = input_vd.compute_set_of_bases();

    //Basen als Knoten hinzufügen
    for( auto curr_base : set_of_bases) {
        output.add_one_node( input_vd.original_graph().get_node(curr_base).terminal_state() );
    }

    //Kanten hinzufügen
    for(unsigned int i = 0; i < input_vd.num_bases()-1; i++){
        for(unsigned int j = 0; j < i+1; j++){
            if( min_bound_edges[i][j].first != Graph::invalid_edge_id ){
                output.add_edge( j, i+1, min_bound_edges[i][j].second);
            }
        }
    }

    return output;
}

Subgraph Mehlhorn::turn_into_subgraph_of_vordiags_original_graph(const Voronoi_diagram& input_vd,
                                                                 const Graph& graph_to_process,
                                                                 const std::vector<
                                                                    std::vector<
                                                                            std::pair<
                                                                                    Graph::EdgeId, Graph::PathLength>>>&
                                                                                        min_bound_edges) {
    const Graph& original_graph = input_vd.original_graph();

    //speichert zu jeder Kante des Ausgabegraphen die EdgeIds des zugrundeliegenden Graphen
    std::vector<Graph::EdgeId> output_original_edge_ids;

    //speichert die schon hinzugefügten Kanten, sodass wir keine Kanten mehrfach hinzufügen
    std::vector<bool> added_edges(original_graph.num_edges(), false);

    for( const auto& curr_input_edge : graph_to_process.edges() ) {

        //zugehörige boundary edge des ursprünglichen Graphen finden
        std::pair<Graph::NodeId, Graph::NodeId> curr_input_edge_nodes = curr_input_edge.get_nodes_orderedbyid();
        Graph::EdgeId original_bound_edge_id =
                min_bound_edges[curr_input_edge_nodes.second - 1][curr_input_edge_nodes.first].first;

        std::vector<Graph::EdgeId> edges_of_curr_path =
                VorDiagAux::compute_bound_path_as_vect(input_vd, original_bound_edge_id);

        for(auto var_edge_to_add: edges_of_curr_path ) {

            if( not added_edges[var_edge_to_add]) {

                output_original_edge_ids.push_back(var_edge_to_add);
                added_edges[var_edge_to_add] = true;
            }
        }
    }

    return Subgraph(original_graph, output_original_edge_ids);
}


void Mehlhorn::print_min_bound_edges_of_vd(const Voronoi_diagram& input_vd) {
    std::vector<std::vector<std::pair<Graph::EdgeId, Graph::PathLength>>> output = compute_min_bound_edges_of_vordiag(input_vd);
    const std::vector<Graph::NodeId>& set_of_bases = input_vd.compute_set_of_bases();

    std::cout << "Ausgabe von compute_min_bound_edges_of_vordiag \n";

    std::cout << "Kanten (EdgeIds): \n";
    std::cout << "Basis     ";
    for(unsigned int j = 0; j < input_vd.num_bases()-1; j++) {
        std::cout << set_of_bases[j]+1 << ", ";
    }
    std::cout << "\n";
    for(unsigned int i = 0; i < input_vd.num_bases()-1; i++){
        std::cout << set_of_bases[i+1]+1 << ":    ";
        for(unsigned int j = 0; j < i+1; j++){
            GraphAuxPrint::print_edge_as_pair(input_vd.original_graph(), output[i][j].first );
            std::cout << ", ";
        }
        std::cout << "\n";
    }

    std::cout << "\n" << "Distanzen: \n";
    std::cout << "Basis     ";
    for(unsigned int j = 0; j < input_vd.num_bases()-1; j++){
        std::cout << set_of_bases[j]+1 << ", ";
    }
    std::cout << "\n";
    for(unsigned int i = 0; i < input_vd.num_bases()-1; i++){
        std::cout << set_of_bases[i+1]+1 << ":    ";
        for(unsigned int j = 0; j < i+1; j++){
            GraphAuxPrint::print_pathlength( output[i][j].second );
            std::cout << ", ";
        }
        std::cout << "\n";
    }

}
