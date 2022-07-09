//
// Created by Lukas on 26.06.2022.
//

#include "Subgraph.h"

#include "iostream"

#include "graph_aux_functions.h"
#include "general_aux_functions.h"

Subgraph::Subgraph(const Graph &originalGraph, const Graph &thisGraph,
                   const std::vector<Graph::NodeId> &subgraphNodeidsOfNodesInOriginalgraph,
                   const std::vector<Graph::NodeId> &originalNodeids, const std::vector<Graph::EdgeId> &originalEdgeids):
                   _original_graph(originalGraph),
                   _this_graph(thisGraph),
                   _subgraph_nodeids_of_nodes_in_originalgraph(subgraphNodeidsOfNodesInOriginalgraph),
                   _original_nodeids(originalNodeids), _original_edgeids(originalEdgeids) {}

const Graph &Subgraph::original_graph() const {
    return _original_graph;
}

const Graph &Subgraph::this_graph() const {
    return _this_graph;
}

const std::vector<Graph::NodeId> &Subgraph::subgraph_nodeids_of_nodes_in_originalgraph() const {
    return _subgraph_nodeids_of_nodes_in_originalgraph;
}

const std::vector<Graph::EdgeId> &Subgraph::original_edgeids() const {
    return _original_edgeids;
}

const std::vector<Graph::NodeId> &Subgraph::original_nodeids() const {
    return _original_nodeids;
}

Graph &Subgraph::this_graph() {
    return _this_graph;
}

/*?
std::vector<Graph::NodeId> &Subgraph::subgraph_nodeids_of_nodes_in_originalgraph() {
    return _subgraph_nodeids_of_nodes_in_originalgraph;
}*/

/*?
std::vector<Graph::NodeId> &Subgraph::original_nodeids() {
    return _original_nodeids;
}*/

std::vector<Graph::EdgeId> &Subgraph::original_edgeids() {
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

void Subgraph::reset(std::vector<Graph::EdgeId>& new_original_edgeids) {

    Graph new_this_graph(0);
    unsigned int new_graph_num_nodes = 0;
    std::vector<Graph::NodeId> new_sub_nodeids_of_or_nodes(_original_graph.num_nodes(), Graph::invalid_node_id);
    std::vector<Graph::NodeId> new_or_node_ids;
    new_or_node_ids.reserve(_original_graph.num_nodes());

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
                const Graph::Node& node_to_add = _original_graph.get_node(node_to_add_id);
                new_this_graph.add_one_node(node_to_add.terminal_state());

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
    new_or_node_ids.shrink_to_fit();

    //aktualisiere den Subgraphen mit den berechneten Werten
    assign(new_this_graph, new_sub_nodeids_of_or_nodes, new_or_node_ids, new_original_edgeids);
}

Subgraph::Subgraph(const Graph &input_graph):
    _original_graph(input_graph),
    _this_graph(input_graph),
    _subgraph_nodeids_of_nodes_in_originalgraph( GeneralAux::get_range_of_uns_ints(0, input_graph.num_nodes()) ),
    _original_nodeids( GeneralAux::get_range_of_uns_ints(0, input_graph.num_nodes()) ),
    _original_edgeids( GeneralAux::get_range_of_uns_ints(0, input_graph.num_edges()) )
    {}

void Subgraph::add_nodes(const std::vector<Graph::NodeId>& nodes_to_add) {
    //prüfe, ob Eingabeknoten tatsächlich im zugrundeliegenden Graphen liegen
    for(auto curr_node: nodes_to_add) {
        GraphAux::check_node_id(curr_node, _original_graph.num_nodes(), "Subgraph::add_nodes");
    }

    // jeder neu hinzugefügte Knoten erhält einfach die NodeId, die der Anzahl an Knoten entspricht,
    // bevor dieser Knoten hinzugefügt wurde
    for(unsigned int i=0; i<nodes_to_add.size(); i++) {
        Graph::NodeId curr_node = nodes_to_add[i];
        _subgraph_nodeids_of_nodes_in_originalgraph[curr_node] = this_graph().num_nodes() +i;
    }

    _this_graph.add_nodes(nodes_to_add.size());

    _original_nodeids.insert(_original_nodeids.end(), nodes_to_add.begin(), nodes_to_add.end());

    //todo? was für Terminale machen?
}

void Subgraph::add_edges(const std::vector<Graph::NodeId>& edges_to_add) {
    //prüfe, ob Eingabekanten tatsächlich im zugrundeliegenden Graphen liegen
    for(auto curr_edge_id: edges_to_add) {
        GraphAux::check_edge_id(curr_edge_id, _original_graph.num_nodes(), "Subgraph::add_edges");
    }

    for(auto curr_edge_id: edges_to_add) {
        Graph::Edge curr_edge = _original_graph.get_edge(curr_edge_id);

        //finde die Endpunkte der Kante als NodeIds im this_graph
        Graph::NodeId node_a = _subgraph_nodeids_of_nodes_in_originalgraph[curr_edge.node_a()];
        Graph::NodeId node_b = _subgraph_nodeids_of_nodes_in_originalgraph[curr_edge.node_b()];

        _this_graph.add_edge(node_a, node_b, curr_edge.weight());
    }

    _original_nodeids.insert(_original_nodeids.end(), edges_to_add.begin(), edges_to_add.end());
}

void Subgraph::reset_edges(const std::vector<Graph::EdgeId> &new_original_edgeids) {

    _this_graph.clear_edges();

    //Laufzeit? reserve für _edges?
    for( auto curr_orig_edge_id: new_original_edgeids) {
        const Graph::Edge& curr_edge = _original_graph.get_edge(curr_orig_edge_id);

        //finde die NodeIds der Endpunkte der aktuellen Kante im Subgraphen (this_graph)
        Graph::NodeId node_a = _subgraph_nodeids_of_nodes_in_originalgraph[curr_edge.node_a()];
        Graph::NodeId node_b = _subgraph_nodeids_of_nodes_in_originalgraph[curr_edge.node_b()];

        _this_graph.add_edge(node_a, node_b, curr_edge.weight());
    }
}

