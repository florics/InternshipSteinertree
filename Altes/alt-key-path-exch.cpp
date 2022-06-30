//
// Created by Lukas on 24.06.2022.
//

//das alte cleanup in key path exchange

Graph::EdgeId best_original_bound_edge_id = Graph::invalid_edge_id;
Graph::PathLength best_original_bound_edge_length = Graph::infinite_length;
while(not bound_edge_heaps[input_node_id].empty()) {
//'delete min'
Graph::EdgeId best_original_bound_edge_id = bound_edge_heaps[input_node_id].top().second;
Graph::PathLength best_original_bound_edge_length = bound_edge_heaps[input_node_id].top().first;
bound_edge_heaps[input_node_id].pop();

//prüfe, ob die Kante zwischen den subtrees verläuft
// (bemerke, dass sie mind. einen Endpunkt in dem subtree von curr_node hat)

Graph::Edge var_bound_edge = original_graph.get_edge(best_original_bound_edge_id);

Union_Find_Structure::ElementId var_node_a_ufsroot = subtrees_ufs.find(var_bound_edge.node_a());
Union_Find_Structure::ElementId var_node_b_ufsroot = subtrees_ufs.find(var_bound_edge.node_b());

if(( var_node_a_ufsroot==input_node_ufsroot &&var_node_b_ufsroot==input_node_ufsroot )
|| var_node_a_ufsroot == internal_nodes_ufsroot
|| var_node_b_ufsroot == internal_nodes_ufsroot) {
continue;
} else {
break;
}
}
