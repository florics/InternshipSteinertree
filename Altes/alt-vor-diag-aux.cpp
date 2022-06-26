//
// Created by Lukas on 25.06.2022.
//


//Funktion, die Path mit edgeids ausgibt
GraphPath VorDiagAux::compute_bound_path(Voronoi_diagram original_vd, Graph::EdgeId bound_edge_id) {

    std::vector<Graph::EdgeId> output_edge_ids;

    const std::vector<std::pair<Graph::NodeId, Graph::EdgeId>>& predecessor = original_vd.predecessor();
    Graph::Edge bound_edge = original_vd.original_graph().get_edge(bound_edge_id);

    if(not original_vd.check_if_bound_edge(bound_edge)) {
        throw std::runtime_error("(VorDiagAux::compute_bound_path) Eingabekante keine boundary edge im Eingabe-Voronoi-Diagramm.");
    }

    //finde die Kanten auf dem Weg von node_a zur Basis von node_a und füge sie zu output_edge_ids hinzu

    Graph::EdgeId curr_edge_id = predecessor[bound_edge.node_a()].second;
    Graph::NodeId next_node_id = predecessor[bound_edge.node_a()].first;

    while( curr_edge_id != Graph::invalid_edge_id) {
        output_edge_ids.push_back(curr_edge_id);
        curr_edge_id = predecessor[next_node_id].second;
        next_node_id = predecessor[next_node_id].first;
    }

    //kehre die Reihenfolge um, weil wir die Kantenfolge von der Basis von node_a aus ausgeben wollen
    std::reverse(output_edge_ids.begin(), output_edge_ids.end());

    //die boundary edge selbst hinzufügen
    output_edge_ids.push_back(bound_edge_id);

    //finde die Kanten auf dem Weg von node_b zu Basis von node_b und füge sie zu output_edge_ids hinzu
    curr_edge_id = predecessor[bound_edge.node_b()].second;
    next_node_id = predecessor[bound_edge.node_b()].first;
    while( curr_edge_id != Graph::invalid_edge_id) {
        output_edge_ids.push_back(curr_edge_id);
        curr_edge_id = predecessor[next_node_id].second;
        next_node_id = predecessor[next_node_id].first;
    }

    //konstruire den GraphPath mit den entsprechenden Basen als Endknoten
    Graph::NodeId base_a = original_vd.base()[bound_edge.node_a()];
    Graph::NodeId base_b = original_vd.base()[bound_edge.node_b()];
    GraphPath output_path(original_vd.original_graph(), output_edge_ids, base_a, base_b);

    return output_path;
}
