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

//todo: glaub man kann hier noch so Subgraph-Dinge beschleunigen, wie in LocalSearchAux::perform_improving_changements

//? Graph const machen!
Subgraph mehlhorns_algo(Graph& input_graph) {

    //todo: irgendwo checken, dass Graph zsmhängend? (zumindest, dass alle Terminale in einer ZHK)

    Voronoi_diagram vor_diag(input_graph.get_vect_term(), input_graph);
    std::vector<std::vector<std::pair<Graph::EdgeId, Graph::PathLength>>> min_bound_edges = compute_min_bound_edges_of_vd(vor_diag);

    Graph aux_graph = construct_aux_graph_of_vd(vor_diag, min_bound_edges);

    Graph mst_of_aux_graph = GraphAlgorithms::mst_prim(aux_graph, 0);

    Subgraph sub_of_mst_of_aux_graph = turn_into_subgraph_of_vds_original_graph(vor_diag, mst_of_aux_graph, min_bound_edges);

    Subgraph output_w_stleafs = GraphAlgorithms::mst_prim_for_subgraphs(sub_of_mst_of_aux_graph, 0);

    Subgraph output = GraphAux::copy_subgraph_wo_steinerleafs(output_w_stleafs);

    return output;
}

std::vector<std::vector<std::pair<Graph::EdgeId, Graph::PathLength>>> compute_min_bound_edges_of_vd(const Voronoi_diagram& input_vd) {

    //Initialisierung der Ausgabe-Matrix: output[i][j] entspricht den Basen i+1, j (diese einfache Nummerierung der Basen wird hier mit base_ids gespeichert)
    std::vector<std::vector<std::pair<Graph::EdgeId, Graph::PathLength>>> output;
    std::pair<Graph::EdgeId, Graph::PathLength> invalid_pair = {Graph::invalid_edge_id, Graph::infinite_length};
    for(unsigned int i = 0; i < input_vd.num_bases()-1; i++){
        std::vector<std::pair<Graph::EdgeId, Graph::PathLength>> var_vector (i+1, invalid_pair);
        output.push_back(var_vector);
    }

    //Schleife über alle Kanten des Graphen, die boundary edges sind
    std::vector<Graph::Edge> original_graph_edges = input_vd.original_graph().edges();
    std::vector<Voronoi_diagram::BaseId> base_ids = input_vd.compute_base_ids();
    for(auto curr_edge : original_graph_edges){
        if( input_vd.check_if_bound_edge(curr_edge) ){

            //prüfe, ob aktuelle Kante besser als gespeicherte Kante (update falls ja)
            std::array<Graph::NodeId, 2> curr_bases = { base_ids[ input_vd.base()[curr_edge.node_a()] ],
                                                        base_ids[ input_vd.base()[curr_edge.node_b()] ] };
            std::sort(curr_bases.begin(), curr_bases.end());
            //debug
            if(curr_bases[0] > curr_bases[1]){
                throw std::runtime_error("(compute_min_bound_edges_of_vd) sort tut nicht das was es soll");
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

Graph construct_aux_graph_of_vd(const Voronoi_diagram& input_vd,
                                const std::vector<std::vector<std::pair<Graph::EdgeId, Graph::PathLength>>>& min_bound_edges) {
    Graph output(0);

    //Laufzeit? O(n)
    const std::vector<Graph::NodeId> set_of_bases = input_vd.compute_set_of_bases();

    //Basen als Knoten hinzufügen
    for( auto curr_base : set_of_bases) {
        output.add_one_node( input_vd.original_graph().get_node(curr_base).node_name(), input_vd.original_graph().get_node(curr_base).terminal_state() );
    }

    //Kanten hinzufügen
    for(unsigned int i = 0; i < input_vd.num_bases()-1; i++){
        for(unsigned int j = 0; j < i+1; j++){
            if( min_bound_edges[i][j].first != Graph::invalid_edge_id ){
                output.add_edge( j, i+1, min_bound_edges[i][j].second);
            }
        }
    }

    //todo: checken, dass alle Terminale erreichbar, d. h. dass der Ausgabe-Graph zsmhängend ist

    return output;
}

Subgraph turn_into_subgraph_of_vds_original_graph(const Voronoi_diagram& input_vd,
                                                  const Graph& input_graph,
                                                  const std::vector<std::vector<std::pair<Graph::EdgeId, Graph::PathLength>>>& min_bound_edges) {
    const Graph& original_graph = input_vd.original_graph();

    //speichert zu jeder Kante des Ausgabegraphen die EdgeIds des zugrundeliegenden Graphen
    std::vector<Graph::EdgeId> output_original_edge_ids;

    //speichert die schon hinzugefügten Kanten, sodass wir keine Kanten mehrfach hinzufügen
    std::vector<bool> added_edges(original_graph.num_edges(), false);

    for( auto curr_input_edge : input_graph.edges() ) {
        //zugehörige boundary edge des ursprünglichen Graphen finden
        std::pair<Graph::NodeId, Graph::NodeId> curr_input_edge_nodes = curr_input_edge.get_nodes_orderedbyid();
        Graph::EdgeId original_bound_edge_id = min_bound_edges[curr_input_edge_nodes.second - 1][curr_input_edge_nodes.first].first;

        std::vector<Graph::EdgeId> edges_of_curr_path = VorDiagAux::compute_bound_path_as_vect(input_vd, original_bound_edge_id);

        for(auto var_edge_to_add: edges_of_curr_path ) {
            if( not added_edges[var_edge_to_add]) {
                output_original_edge_ids.push_back(var_edge_to_add);
                added_edges[var_edge_to_add] = true;
            }
        }
    }

    Graph null_graph(0);
    Subgraph output(original_graph, null_graph, std::vector<Graph::NodeId>(), std::vector<Graph::NodeId>(), std::vector<Graph::EdgeId>());
    output.reset_with_set_of_edges(output_original_edge_ids);

    return output;
}


void print_min_bound_edges_of_vd(const Voronoi_diagram& input_vd) {
    std::vector<std::vector<std::pair<Graph::EdgeId, Graph::PathLength>>> output = compute_min_bound_edges_of_vd(input_vd);
    const std::vector<Graph::NodeId>& set_of_bases = input_vd.compute_set_of_bases();

    std::cout << "Ausgabe von compute_min_bound_edges_of_vd \n";

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
