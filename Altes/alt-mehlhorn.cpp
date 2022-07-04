//
// Created by Lukas on 27.06.2022.
//

// Funktion turn_into_subgraph_of_vds_original_graph als Subgraph-Funktion in alter Version

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






// Funktion turn_into_subgraph_of_vds_original_graph als Graph-Funktion (und nicht als Subgraph-Fktn)
Graph turn_into_subgraph_of_vds_original_graph(const Voronoi_diagram& input_vd,
                                               const Graph& input_graph,
                                               const std::vector<std::vector<std::pair<Graph::EdgeId, Graph::PathLength>>>& min_bound_edges) {
const Graph& original_graph = input_vd.original_graph();

Graph output = GraphAux::copygraph_wo_edges(original_graph);
std::vector<Graph::EdgeId> original_edge_ids_of_outputgraph_edges;


//speichert die schon hinzugefügten Kanten, sodass wir keine Kanten mehrfach hinzufügen
std::vector<bool> added_edges(original_graph.num_edges(), false);

for( auto curr_edge : input_graph.edges() ) {
//zugehörige boundary edge des ursprünglichen Graphen finden
// ? wenn ich Graph gerichtet machen kann, könnte ich das hier ggf. benutzen (anstatt von der nächsten Zeile)
std::pair<Graph::NodeId, Graph::NodeId> curr_edge_nodes = curr_edge.get_nodes_orderedbyid();
Graph::Edge original_bound_edge = original_graph.get_edge( min_bound_edges[curr_edge_nodes.second - 1][curr_edge_nodes.first].first );

//hier die Klasse EdgeSequence benutzen?

//diese Kante hinzufügen
output.add_edge(original_bound_edge.node_a(), original_bound_edge.node_b(), original_bound_edge.weight());
original_edge_ids_of_outputgraph_edges.push_back(curr_edge.edge_id());
added_edges[original_bound_edge.edge_id()] = true;

//Kanten auf den jeweiligen kürz. Wegen von den Endpunkten der aktuellen Kante zu deren jeweiliger Basis hinzufügen

const std::vector<std::pair<Graph::NodeId, Graph::EdgeId>>& predecessor = input_vd.predecessor();

Graph::NodeId var_node_id = original_bound_edge.node_a();
Graph::EdgeId edge_to_add_id = predecessor[var_node_id].second;
while( edge_to_add_id != Graph::invalid_edge_id && not added_edges[ edge_to_add_id ] ){
Graph::Edge edge_to_add = original_graph.get_edge( edge_to_add_id );
output.add_edge( edge_to_add.node_a(),
        edge_to_add.node_b(),
        edge_to_add.weight() );
original_edge_ids_of_outputgraph_edges.push_back(edge_to_add_id);
added_edges[ edge_to_add_id ] = true;

var_node_id = predecessor[var_node_id].first;
edge_to_add_id = predecessor[var_node_id].second;
}

var_node_id = original_bound_edge.node_b();
edge_to_add_id = predecessor[var_node_id].second;
while( edge_to_add_id != Graph::invalid_edge_id && not added_edges[ edge_to_add_id ] ){
Graph::Edge edge_to_add = original_graph.get_edge( edge_to_add_id );
output.add_edge( edge_to_add.node_a(),
        edge_to_add.node_b(),
        edge_to_add.weight() );
original_edge_ids_of_outputgraph_edges.push_back(edge_to_add_id);
added_edges[ edge_to_add_id ] = true;

var_node_id = predecessor[var_node_id].first;
edge_to_add_id = predecessor[var_node_id].second;
}

}

//Laufzeit?
output = GraphAux::copygraph_wo_iso_nodes(output);

return output;
}
