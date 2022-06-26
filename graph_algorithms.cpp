//
// Created by Lukas on 14.06.2022.
//

#include "queue"
#include "iostream" //nur für einmal runtime_error (?)

#include "graph_algorithms.h"

#include "graph_aux_functions.h"
#include "graph_printfunctions.h"
#include "Union_Find_Structure.h"

Graph mst_prim(const Graph& input_graph, Graph::NodeId start_node){
    Graph output = copygraph_wo_edges(input_graph);

    //Heap, mit dem wir jeweils den besten Kandidaten (Knoten) finden
    std::priority_queue<
            std::pair<Graph::EdgeWeight ,Graph::NodeId>,
            std::vector< std::pair<Graph::EdgeWeight ,Graph::NodeId> >,
            std::greater< std::pair<Graph::EdgeWeight ,Graph::NodeId> >
    > candidates;
    //speichert für jeden Knoten die aktuell beste bekannte Kante, wenn wir diesen Knoten hinzufügen wollen
    std::vector<Graph::EdgeId> best_edges (input_graph.num_nodes(), Graph::invalid_edge_id);
    //speichert die bereits hinzugefügten Knoten
    std::vector<bool> reached (input_graph.num_nodes(), false);
    reached [start_node] = true;

    //erste Schleife über die Nachbarn des Startknoten //? irgendwie in die Hauptschleife packen?
    for(auto curr_edge_id: input_graph.get_node(start_node).incident_edge_ids()){
        Graph::Edge curr_edge = input_graph.get_edge(curr_edge_id);
        Graph::NodeId curr_neighbor = curr_edge.get_other_node(start_node);
        if( best_edges[curr_neighbor] == Graph::invalid_edge_id ){
            candidates.push( {curr_edge.weight(), curr_neighbor} );
            best_edges[curr_neighbor] = curr_edge_id;
        }else if( curr_edge.weight() < input_graph.get_edge( best_edges[curr_neighbor] ).weight() ){
            candidates.push( {curr_edge.weight(), curr_neighbor} );
            best_edges[curr_neighbor] = curr_edge_id;
        }
    }

    while( not candidates.empty() ){
        Graph::NodeId next_cand = candidates.top().second;
        Graph::EdgeWeight next_cand_weight = candidates.top().first;
        candidates.pop();

        //debug: der Fall kann eig nicht eintreten
        if( best_edges[next_cand] == Graph::invalid_edge_id ){
            throw std::runtime_error ("mst_prim: best_edges[next_cand] == Graph::invalid_edge_id.");
        }

        //was ist, wenn next_cand_weight unendlich ?? denke das ist ok so

        //falls die Bedingung der if-Abfrage eintritt, haben wir tatsächlich den besten nächsten Kandidaten gefunden
        //bemerke, dass get_edge( best_edges[next_cand] ) "wohldefiniert" ist,
        // da jeder Knoten, der mind. einmal zu candidates hinzugefügt wird, eine valide Kante in best_edges erhält
        if( next_cand_weight == input_graph.get_edge( best_edges[next_cand] ).weight() ){
            output.add_edge(
                    input_graph.get_edge(best_edges[next_cand]).node_a(),
                    input_graph.get_edge(best_edges[next_cand]).node_b(),
                    input_graph.get_edge(best_edges[next_cand]).weight()
            );
            reached[next_cand] = true;

            //Finde neue Kandidaten
            for(auto curr_edge_id: input_graph.get_node(next_cand).incident_edge_ids()){
                Graph::Edge curr_edge = input_graph.get_edge(curr_edge_id);
                Graph::NodeId curr_neighbor = curr_edge.get_other_node(next_cand);
                if( not reached[curr_neighbor] ){

                    if( best_edges[curr_neighbor] == Graph::invalid_edge_id ){
                        candidates.push( {curr_edge.weight(), curr_neighbor} );
                        best_edges[curr_neighbor] = curr_edge_id;
                    }else if( curr_edge.weight() < input_graph.get_edge( best_edges[curr_neighbor] ).weight() ){
                        candidates.push( {curr_edge.weight(), curr_neighbor} );
                        best_edges[curr_neighbor] = curr_edge_id;
                    }

                }
            }

        }

    }

    //Laufzeit?
    output = copygraph_wo_iso_nodes(output);

    return output;
}

Graph compute_spann_forest(const Graph& input_graph) {
    Graph output_graph(input_graph.num_nodes());
    Union_Find_Structure ufs(input_graph.num_nodes());

    for( auto curr_edge : input_graph.edges() ){
        Graph::NodeId curr_node_a = curr_edge.node_a();
        Graph::NodeId curr_node_b = curr_edge.node_b();

        if(not ufs.check_if_in_existing_set(curr_node_a)) {
            ufs.make_set(curr_node_a);
        }
        if(not ufs.check_if_in_existing_set(curr_node_b)) {
            ufs.make_set(curr_node_b);
        }

        if( ufs.find(curr_node_a) != ufs.find(curr_node_b) ) {
            output_graph.add_existing_edge_w_newid(curr_edge);
            ufs.union_by_rank(curr_node_a, curr_node_b);
        }
    }


    std::cout << "Der von 'compute_spann_forest' berechnete Spannwald ist: \n";
    print_graph(output_graph);
    std::cout << "\n";

    return output_graph;
}