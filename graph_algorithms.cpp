//
// Created by Lukas on 14.06.2022.
//

#include "queue"
#include "iostream" //nur für einmal runtime_error (?)

#include "graph_algorithms.h"

#include "graph_aux_functions.h"
#include "graph_printfunctions.h"
#include "Union_Find_Structure.h"
#include "general_aux_functions.h"

Graph GraphAlgorithms::mst_prim(const Graph &input_graph, Graph::NodeId start_node) {
    Graph output = GraphAux::copygraph_wo_edges(input_graph);

    //Heap, mit dem wir jeweils den besten Kandidaten (Knoten) finden
    std::priority_queue<
            std::pair<Graph::EdgeWeight, Graph::NodeId>,
            std::vector<std::pair<Graph::EdgeWeight, Graph::NodeId> >,
            std::greater<std::pair<Graph::EdgeWeight, Graph::NodeId> >
    > candidates;
    //speichert für jeden Knoten die aktuell beste bekannte Kante, wenn wir diesen Knoten hinzufügen wollen
    std::vector<Graph::EdgeId> best_edges(input_graph.num_nodes(), Graph::invalid_edge_id);
    //speichert die bereits hinzugefügten Knoten
    std::vector<bool> reached(input_graph.num_nodes(), false);
    reached[start_node] = true;

    //erste Schleife über die Nachbarn des Startknoten //? irgendwie in die Hauptschleife packen?
    for (auto curr_edge_id: input_graph.get_node(start_node).incident_edge_ids()) {
        Graph::Edge curr_edge = input_graph.get_edge(curr_edge_id);
        Graph::NodeId curr_neighbor = curr_edge.get_other_node(start_node);
        if (best_edges[curr_neighbor] == Graph::invalid_edge_id) {
            candidates.push({curr_edge.weight(), curr_neighbor});
            best_edges[curr_neighbor] = curr_edge_id;
        } else if (curr_edge.weight() < input_graph.get_edge(best_edges[curr_neighbor]).weight()) {
            candidates.push({curr_edge.weight(), curr_neighbor});
            best_edges[curr_neighbor] = curr_edge_id;
        }
    }

    while (not candidates.empty()) {
        Graph::NodeId next_cand = candidates.top().second;
        Graph::EdgeWeight next_cand_weight = candidates.top().first;
        candidates.pop();

        //debug: der Fall kann eig nicht eintreten
        if (best_edges[next_cand] == Graph::invalid_edge_id) {
            throw std::runtime_error("(GraphAlgorithms::mst_prim) best_edges[next_cand] == Graph::invalid_edge_id.");
        }

        //was ist, wenn next_cand_weight unendlich ?? denke das ist ok so

        //falls die Bedingung der if-Abfrage eintritt, haben wir tatsächlich den besten nächsten Kandidaten gefunden
        //bemerke, dass get_edge( best_edges[next_cand] ) "wohldefiniert" ist,
        // da jeder Knoten, der mind. einmal zu candidates hinzugefügt wird, eine valide Kante in best_edges erhält
        if (next_cand_weight == input_graph.get_edge(best_edges[next_cand]).weight()) {
            output.add_edge(
                    input_graph.get_edge(best_edges[next_cand]).node_a(),
                    input_graph.get_edge(best_edges[next_cand]).node_b(),
                    input_graph.get_edge(best_edges[next_cand]).weight()
            );
            reached[next_cand] = true;

            //Finde neue Kandidaten
            for (auto curr_edge_id: input_graph.get_node(next_cand).incident_edge_ids()) {
                Graph::Edge curr_edge = input_graph.get_edge(curr_edge_id);
                Graph::NodeId curr_neighbor = curr_edge.get_other_node(next_cand);
                if (not reached[curr_neighbor]) {

                    if (best_edges[curr_neighbor] == Graph::invalid_edge_id) {
                        candidates.push({curr_edge.weight(), curr_neighbor});
                        best_edges[curr_neighbor] = curr_edge_id;
                    } else if (curr_edge.weight() < input_graph.get_edge(best_edges[curr_neighbor]).weight()) {
                        candidates.push({curr_edge.weight(), curr_neighbor});
                        best_edges[curr_neighbor] = curr_edge_id;
                    }

                }
            }

        }

    }

    //Laufzeit? Weglassen ok?
    //output = GraphAux::copygraph_wo_iso_nodes(output);

    //debug
    if(output.num_edges() != input_graph.num_nodes()-1) {
        throw std::runtime_error("(GraphAlgorithms::mst_prim) Der berechnete Graph hat nicht n-1 Kanten.");
    }

    if( not GraphAux::get_isolated_nodes(output).empty()) {
        throw std::runtime_error("(GraphAlgorithms::mst_prim) Es wurden nicht alle Knoten erreicht.");
    }

    return output;
}

Subgraph GraphAlgorithms::mst_prim_for_subgraphs(const Subgraph &input_subgraph, Graph::NodeId start_node) {
    //Checks für start_node?

    const Graph& input_graph = input_subgraph.this_graph();

    Graph output_graph = GraphAux::copygraph_wo_edges(input_graph);

    //Heap, mit dem wir jeweils den besten Kandidaten (Knoten) finden
    std::priority_queue<
            std::pair<Graph::EdgeWeight, Graph::NodeId>,
            std::vector<std::pair<Graph::EdgeWeight, Graph::NodeId> >,
            std::greater<std::pair<Graph::EdgeWeight, Graph::NodeId> >
    > candidates;
    //speichert für jeden Knoten die aktuell beste bekannte Kante, wenn wir diesen Knoten hinzufügen wollen
    std::vector<Graph::EdgeId> best_edges(input_graph.num_nodes(), Graph::invalid_edge_id);
    //speichert die bereits hinzugefügten Knoten
    std::vector<bool> reached(input_graph.num_nodes(), false);
    reached[start_node] = true;

    const std::vector<Graph::EdgeId>& input_original_edge_ids = input_subgraph.original_edgeids();
    // entspricht (später) den original_edge_ids des Ausgabegraphen (siehe Subgraph-Klasse)
    std::vector<Graph::EdgeId> output_original_edge_ids;

    //erste Schleife über die Nachbarn des Startknoten //? irgendwie in die Hauptschleife packen?
    for (auto curr_edge_id: input_graph.get_node(start_node).incident_edge_ids()) {
        const Graph::Edge& curr_edge = input_graph.get_edge(curr_edge_id);
        Graph::NodeId curr_neighbor = curr_edge.get_other_node(start_node);
        if (best_edges[curr_neighbor] == Graph::invalid_edge_id) {
            candidates.push({curr_edge.weight(), curr_neighbor});
            best_edges[curr_neighbor] = curr_edge_id;
        } else if (curr_edge.weight() < input_graph.get_edge(best_edges[curr_neighbor]).weight()) {
            candidates.push({curr_edge.weight(), curr_neighbor});
            best_edges[curr_neighbor] = curr_edge_id;
        }
    }

    while (not candidates.empty()) {
        Graph::NodeId next_cand = candidates.top().second;
        Graph::EdgeWeight next_cand_weight = candidates.top().first;
        candidates.pop();

        //debug: der Fall kann eig nicht eintreten
        if (best_edges[next_cand] == Graph::invalid_edge_id) {
            throw std::runtime_error("GraphAlgorithms::mst_prim_for_subgraphs: best_edges[next_cand] == Graph::invalid_edge_id.");
        }

        //was ist, wenn next_cand_weight unendlich ?? denke das ist ok so

        //falls die Bedingung der if-Abfrage eintritt, haben wir tatsächlich den besten nächsten Kandidaten gefunden
        //bemerke, dass get_edge( best_edges[next_cand] ) "wohldefiniert" ist,
        // da jeder Knoten, der mind. einmal zu candidates hinzugefügt wird, eine valide Kante in best_edges erhält
        if (next_cand_weight == input_graph.get_edge(best_edges[next_cand]).weight()) {
            Graph::Edge edge_to_add = input_graph.get_edge(best_edges[next_cand]);
            output_graph.add_edge(edge_to_add.node_a(),
                                  edge_to_add.node_b(),
                                  edge_to_add.weight());
            output_original_edge_ids.push_back(input_original_edge_ids[best_edges[next_cand]]);
            reached[next_cand] = true;

            //Finde neue Kandidaten
            for (auto curr_edge_id: input_graph.get_node(next_cand).incident_edge_ids()) {
                Graph::Edge curr_edge = input_graph.get_edge(curr_edge_id);
                Graph::NodeId curr_neighbor = curr_edge.get_other_node(next_cand);
                if (not reached[curr_neighbor]) {

                    if (best_edges[curr_neighbor] == Graph::invalid_edge_id) {
                        candidates.push({curr_edge.weight(), curr_neighbor});
                        best_edges[curr_neighbor] = curr_edge_id;
                    } else if (curr_edge.weight() < input_graph.get_edge(best_edges[curr_neighbor]).weight()) {
                        candidates.push({curr_edge.weight(), curr_neighbor});
                        best_edges[curr_neighbor] = curr_edge_id;
                    }

                }
            }

        }

    }

    //die alten EdgeIds haben wir mit output_original_edge_ids gefunden, die NodeIds ändern sich nicht
    const Graph &original_graph = input_subgraph.original_graph();
    Subgraph output(original_graph, output_graph,
                                input_subgraph.subgraph_nodeids_of_nodes_in_originalgraph(),
                                input_subgraph.original_nodeids(),
                                output_original_edge_ids);

    if( not GraphAux::get_isolated_nodes(output.this_graph()).empty()) {
        throw std::runtime_error("(GraphAlgorithms::mst_prim_for_subgraphs) Es wurden nicht alle Knoten erreicht.");
    }

    return output;
}

Graph GraphAlgorithms::compute_spann_forest(const Graph &input_graph) {
    Graph output_graph(input_graph.num_nodes());
    Union_Find_Structure ufs(input_graph.num_nodes());

    for( const auto& curr_edge: input_graph.edges()) {
        Graph::NodeId curr_node_a = curr_edge.node_a();
        Graph::NodeId curr_node_b = curr_edge.node_b();

        if (not ufs.check_if_in_existing_set(curr_node_a)) {
            ufs.make_set(curr_node_a);
        }
        if (not ufs.check_if_in_existing_set(curr_node_b)) {
            ufs.make_set(curr_node_b);
        }

        if (ufs.find(curr_node_a) != ufs.find(curr_node_b)) {
            output_graph.add_edge(curr_node_a, curr_node_b, curr_edge.weight());
            ufs.union_by_rank(curr_node_a, curr_node_b);
        }
    }


    std::cout << "Der von 'GraphAlgorithms::compute_spann_forest' berechnete Spannwald ist: \n";
    GraphAuxPrint::print_graph(output_graph);
    std::cout << "\n";

    return output_graph;
}