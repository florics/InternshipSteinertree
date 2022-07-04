//
// Created by Lukas on 26.06.2022.
//

#include "Subgraph.h"

#include "iostream"

Subgraph::Subgraph(const Graph &originalGraph, const Graph &thisGraph,
                   const std::vector<Graph::NodeId> &subgraphNodeidsOfNodesInOriginalgraph,
                   const std::vector<Graph::NodeId> &originalNodeids, const std::vector<Graph::EdgeId> &originalEdgeids):
                   _original_graph(originalGraph),
                   _this_graph(thisGraph),
                   _subgraph_nodeids_of_nodes_in_originalgraph(subgraphNodeidsOfNodesInOriginalgraph),
                   _original_nodeids(originalNodeids), _original_edgeids(originalEdgeids) {}

const Graph &Subgraph::getOriginalGraph() const {
    return _original_graph;
}

//? const machen? (die Funktion, nicht die Ausgabe)
const Graph &Subgraph::getThisGraph() const {
    return _this_graph;
}

const std::vector<Graph::NodeId> &Subgraph::getSubgraphNodeidsOfNodesInOriginalgraph() const {
    return _subgraph_nodeids_of_nodes_in_originalgraph;
}

const std::vector<Graph::EdgeId> &Subgraph::getOriginalEdgeids() const {
    return _original_edgeids;
}

const std::vector<Graph::NodeId> &Subgraph::getOriginalNodeids() const {
    return _original_nodeids;
}

Graph &Subgraph::accessThisGraph() {
    return _this_graph;
}

std::vector<Graph::NodeId> &Subgraph::accessSubgraphNodeidsOfNodesInOriginalgraph() {
    return _subgraph_nodeids_of_nodes_in_originalgraph;
}

std::vector<Graph::NodeId> &Subgraph::accessOriginalNodeids() {
    return _original_nodeids;
}

std::vector<Graph::EdgeId> &Subgraph::accessOriginalEdgeids() {
    return _original_edgeids;
}

void Subgraph::assign(Graph& new_this_graph, std::vector<Graph::NodeId>& new_subgraph_nodeids_of_nodes_in_originalgraph,
                      std::vector<Graph::NodeId>& new_original_nodeids,  std::vector<Graph::EdgeId>& new_original_edgeids) {
    //todo: Checks einbauen, dass original-Graphen gleich sind (nicht nötig, solange die Fktn private ist)

    //? hier Problem, wenn Dinge im Graph const
    _this_graph = new_this_graph;
    _subgraph_nodeids_of_nodes_in_originalgraph = new_subgraph_nodeids_of_nodes_in_originalgraph;
    _original_nodeids = new_original_nodeids;
    _original_edgeids = new_original_edgeids;
}

void Subgraph::reset_with_set_of_edges(std::vector<Graph::EdgeId>& new_original_edgeids) {

    Graph new_this_graph(0);
    unsigned int new_graph_num_nodes = 0;
    std::vector<Graph::NodeId> new_sub_nodeids_of_or_nodes(_original_graph.num_nodes(), Graph::invalid_node_id);
    std::vector<Graph::NodeId> new_or_node_ids;

    for( auto curr_or_edge_id: new_original_edgeids) {
        if(curr_or_edge_id == Graph::invalid_edge_id) {
            throw std::runtime_error("(Subgraph::reset_from_set_of_edges) eine eingegeben Kante ist ungueltig.");
        }
        if(curr_or_edge_id >= _original_graph.num_edges()) {
            throw std::runtime_error("(Subgraph::reset_from_set_of_edges) eine eingegeben Kante ist nicht im zugrundeliegenden Graphen.");
        }

        Graph::Edge curr_or_edge = _original_graph.get_edge(curr_or_edge_id);
        std::vector<Graph::NodeId> nodes_to_add = {curr_or_edge.node_a(), curr_or_edge.node_b()};

        for( auto node_to_add_id: nodes_to_add) {
            //prüfe, ob Knoten bereits hinzugefügt
            if (new_sub_nodeids_of_or_nodes[node_to_add_id] == Graph::invalid_node_id) {
                //füge Knoten zu Graph hinzu
                Graph::Node node_to_add = _original_graph.get_node(node_to_add_id);
                new_this_graph.add_one_node(node_to_add.node_name(), node_to_add.terminal_state());

                //aktualisiere new_sub_nodeids_of_or_nodes
                new_sub_nodeids_of_or_nodes[node_to_add_id] = new_graph_num_nodes;
                new_graph_num_nodes++;

                //aktualisiere new_or_node_ids
                new_or_node_ids.push_back(node_to_add_id);
            }
        }

        //füge die entsprechende Kante hinzu (siehe Initialisierung von nodes_to_add)
        new_this_graph.add_edge( new_sub_nodeids_of_or_nodes[nodes_to_add[0]],
                                 new_sub_nodeids_of_or_nodes[nodes_to_add[1]],
                                 curr_or_edge.weight());
    }

    //aktualisiere den Subgraphen mit den berechneten Werten
    assign(new_this_graph, new_sub_nodeids_of_or_nodes, new_or_node_ids, new_original_edgeids);
}
