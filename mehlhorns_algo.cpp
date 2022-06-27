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

//? Graph const machen!
Subgraph mehlhorns_algo(Graph& input_graph) {

    //todo: irgendwo checken, dass Graph zsmhängend?

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

    Graph output_graph = GraphAux::copygraph_wo_edges(original_graph);

    //speichert zu jeder Kante des Ausgabegraphen die EdgeIds des zugrundeliegenden Graphen
    std::vector<Graph::EdgeId> output_original_edge_ids;

    //speichert die schon hinzugefügten Kanten, sodass wir keine Kanten mehrfach hinzufügen
    std::vector<bool> added_edges(original_graph.num_edges(), false);

    for( auto curr_edge : input_graph.edges() ) {
        //zugehörige boundary edge des ursprünglichen Graphen finden
        // ? wenn ich Graph gerichtet machen kann, könnte ich das hier ggf. benutzen (anstatt von der nächsten Zeile)
        std::pair<Graph::NodeId, Graph::NodeId> curr_edge_nodes = curr_edge.get_nodes_orderedbyid();
        Graph::EdgeId original_bound_edge_id = min_bound_edges[curr_edge_nodes.second - 1][curr_edge_nodes.first].first;
        Graph::Edge original_bound_edge = original_graph.get_edge( original_bound_edge_id );

        //hier die Klasse EdgeSequence benutzen?

        //diese Kante hinzufügen
        output_graph.add_edge(original_bound_edge.node_a(), original_bound_edge.node_b(), original_bound_edge.weight());
        output_original_edge_ids.push_back( original_bound_edge_id );
        added_edges[original_bound_edge.edge_id()] = true;

        //Kanten auf den jeweiligen kürz. Wegen von den Endpunkten der aktuellen Kante zu deren jeweiliger Basis hinzufügen

        const std::vector<std::pair<Graph::NodeId, Graph::EdgeId>>& predecessor = input_vd.predecessor();

        Graph::NodeId var_node_id = original_bound_edge.node_a();
        Graph::EdgeId edge_to_add_id = predecessor[var_node_id].second;
        while( edge_to_add_id != Graph::invalid_edge_id && not added_edges[ edge_to_add_id ] ){
            Graph::Edge edge_to_add = original_graph.get_edge( edge_to_add_id );
            output_graph.add_edge( edge_to_add.node_a(),
                             edge_to_add.node_b(),
                             edge_to_add.weight() );
            output_original_edge_ids.push_back(edge_to_add_id);
            added_edges[ edge_to_add_id ] = true;

            var_node_id = predecessor[var_node_id].first;
            edge_to_add_id = predecessor[var_node_id].second;
        }

        var_node_id = original_bound_edge.node_b();
        edge_to_add_id = predecessor[var_node_id].second;
        while( edge_to_add_id != Graph::invalid_edge_id && not added_edges[ edge_to_add_id ] ){
            Graph::Edge edge_to_add = original_graph.get_edge( edge_to_add_id );
            output_graph.add_edge( edge_to_add.node_a(),
                             edge_to_add.node_b(),
                             edge_to_add.weight() );
            output_original_edge_ids.push_back(edge_to_add_id);
            added_edges[ edge_to_add_id ] = true;

            var_node_id = predecessor[var_node_id].first;
            edge_to_add_id = predecessor[var_node_id].second;
        }

    }

    // die original_edge_ids haben wir oben gefunden, die neuen NodeIds sind die alten, da alle Knoten des zugrundeliegenden Graphen im output_graph liegen
    Subgraph output_w_iso_nodes(original_graph, output_graph,
                                GeneralAux::get_range_of_uns_ints(0, original_graph.num_nodes()),
                                GeneralAux::get_range_of_uns_ints(0, original_graph.num_nodes()),
                                output_original_edge_ids);
    // entferne nun die isolierten Knoten
    Subgraph output = GraphAux::copy_subgraph_wo_iso_nodes(output_w_iso_nodes);

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
            print_edge_as_pair(input_vd.original_graph(), output[i][j].first );
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
            print_pathlength( output[i][j].second );
            std::cout << ", ";
        }
        std::cout << "\n";
    }

}
