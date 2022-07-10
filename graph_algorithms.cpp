//
// Created by Lukas on 14.06.2022.
//

#include "queue"
#include "iostream"

#include "graph_algorithms.h"


void GraphAlgorithms::compute_mst_for_graphs(Graph& input_graph, Graph::NodeId start_node) {
    if (start_node == Graph::invalid_node_id) {
        throw std::runtime_error("(GraphAlgorithms::compute_mst_for_graphs) Eingabeknoten ungültig");
    }
    if (start_node >= input_graph.num_nodes()) {
        throw std::runtime_error("(GraphAlgorithms::compute_mst_for_graphs) Eingabeknoten liegt nicht im Graphen");
    }


    //Heap, mit dem wir jeweils den besten Kandidaten (Knoten) finden
    std::priority_queue<
            std::pair<Graph::EdgeWeight, Graph::NodeId>,
            std::vector<std::pair<Graph::EdgeWeight, Graph::NodeId> >,
            std::greater<std::pair<Graph::EdgeWeight, Graph::NodeId> >
    > candidates;

    //speichert für jeden Knoten die aktuell beste bekannte Kante, wenn wir diesen Knoten hinzufügen wollen
    std::vector<Graph::EdgeId> best_edges(input_graph.num_nodes(), Graph::invalid_edge_id);

    //speichert, ob Knoten bereits hinzugefügt wurde
    std::vector<bool> reached(input_graph.num_nodes(), false);
    reached[start_node] = true;

    //speichert die hinzugefügten Kanten
    std::vector<Graph::Edge> mst_edges;
    mst_edges.reserve(input_graph.num_nodes() - 1);

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
            throw std::runtime_error(
                    "(GraphAlgorithms::compute_mst_for_graphs) best_edges[next_cand] == Graph::invalid_edge_id.");
        }

        //was ist, wenn next_cand_weight unendlich ?? denke das ist ok so

        //falls die Bedingung der if-Abfrage eintritt, haben wir tatsächlich den besten nächsten Kandidaten gefunden
        //bemerke, dass get_edge( best_edges[next_cand] ) "wohldefiniert" ist,
        // da jeder Knoten, der mind. einmal zu candidates hinzugefügt wird, eine valide Kante in best_edges erhält
        if (next_cand_weight == input_graph.get_edge(best_edges[next_cand]).weight()) {

            mst_edges.emplace_back(mst_edges.size(),
                                   input_graph.get_edge(best_edges[next_cand]).node_a(),
                                   input_graph.get_edge(best_edges[next_cand]).node_b(),
                                   input_graph.get_edge(best_edges[next_cand]).weight());

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

    for (auto reached_node: reached) {
        if (not reached_node) {
            throw std::runtime_error(
                    "(GraphAlgorithms::compute_mst_for_subgraphs) Es wurden nicht alle Knoten erreicht.");
        }
    }

    //setze die Kantenmenge des Graphen auf die des berechneten MST

    input_graph.clear_edges();

    for (const auto &curr_edge: mst_edges) {
        input_graph.add_edge(curr_edge.node_a(), curr_edge.node_b(), curr_edge.weight());
    }
}

void GraphAlgorithms::compute_mst_for_subgraphs(Subgraph& input_subgraph, Graph::NodeId start_node) {
    if( start_node == Graph::invalid_node_id){
        throw std::runtime_error("(GraphAlgorithms::compute_mst_for_subgraphs) Eingabeknoten ungültig");
    }
    if( start_node >= input_subgraph.this_graph().num_nodes()) {
        throw std::runtime_error("(GraphAlgorithms::compute_mst_for_subgraphs) Eingabeknoten liegt nicht im Graphen");
    }

    const Graph& input_this_graph = input_subgraph.this_graph();

    //speichert für jeden Knoten die aktuell beste bekannte Kante, wenn wir diesen Knoten hinzufügen wollen
    std::vector<Graph::EdgeId> best_edges(input_this_graph.num_nodes(), Graph::invalid_edge_id);

    //speichert die bereits hinzugefügten Knoten
    std::vector<bool> reached(input_this_graph.num_nodes(), false);
    reached[start_node] = true;

    const std::vector<Graph::EdgeId>& old_original_edge_ids = input_subgraph.original_edgeids();

    // entspricht (später) den original_edge_ids des MST (siehe Subgraph-Klasse)
    std::vector<Graph::EdgeId> new_original_edge_ids;

    //Heap, mit dem wir jeweils den besten Kandidaten (Knoten) finden
    std::priority_queue<
            std::pair<Graph::EdgeWeight, Graph::NodeId>,
            std::vector<std::pair<Graph::EdgeWeight, Graph::NodeId> >,
            std::greater<std::pair<Graph::EdgeWeight, Graph::NodeId> >
    > candidates;

    //erste Schleife über die Nachbarn des Startknoten //? irgendwie in die Hauptschleife packen?
    for (auto curr_edge_id: input_this_graph.get_node(start_node).incident_edge_ids()) {

        const Graph::Edge& curr_edge = input_this_graph.get_edge(curr_edge_id);
        Graph::NodeId curr_neighbor = curr_edge.get_other_node(start_node);

        if (best_edges[curr_neighbor] == Graph::invalid_edge_id) {

            candidates.push({curr_edge.weight(), curr_neighbor});
            best_edges[curr_neighbor] = curr_edge_id;

        } else if (curr_edge.weight() < input_this_graph.get_edge(best_edges[curr_neighbor]).weight()) {

            candidates.push({curr_edge.weight(), curr_neighbor});
            best_edges[curr_neighbor] = curr_edge_id;
        }
    }

    //Hauptschleife
    while (not candidates.empty()) {
        Graph::NodeId next_cand = candidates.top().second;
        Graph::EdgeWeight next_cand_weight = candidates.top().first;
        candidates.pop();

        //falls die Bedingung der if-Abfrage eintritt, haben wir tatsächlich den besten nächsten Kandidaten gefunden
        //bemerke, dass get_edge( best_edges[next_cand] ) "wohldefiniert" ist,
        // da jeder Knoten, der mind. einmal zu candidates hinzugefügt wird, eine valide Kante in best_edges erhält
        if (next_cand_weight == input_this_graph.get_edge(best_edges[next_cand]).weight()) {

            new_original_edge_ids.push_back(old_original_edge_ids[best_edges[next_cand]]);

            reached[next_cand] = true;

            //Finde neue Kandidaten
            for (auto curr_edge_id: input_this_graph.get_node(next_cand).incident_edge_ids()) {

                const Graph::Edge& curr_edge = input_this_graph.get_edge(curr_edge_id);
                Graph::NodeId curr_neighbor = curr_edge.get_other_node(next_cand);

                if (not reached[curr_neighbor]) {

                    if (best_edges[curr_neighbor] == Graph::invalid_edge_id) {

                        candidates.push({curr_edge.weight(), curr_neighbor});
                        best_edges[curr_neighbor] = curr_edge_id;

                    } else if (curr_edge.weight() < input_this_graph.get_edge(best_edges[curr_neighbor]).weight()) {

                        candidates.push({curr_edge.weight(), curr_neighbor});
                        best_edges[curr_neighbor] = curr_edge_id;
                    }
                }
            }

        }
    }

    for(auto reached_node: reached) {
        if(not reached_node) {
            throw std::runtime_error("(GraphAlgorithms::compute_mst_for_subgraphs) Es wurden nicht alle Knoten erreicht.");
        }
    }

    //die Kanten des MST als EdgeIds des zugrundeliegenden Graphen haben wir mit new_original_edge_ids gefunden
    input_subgraph.reset_edges(new_original_edge_ids);

}
