//
// Created by Lukas on 17.06.2022.
//

#include "vor_diag_aux_functions.h"

#include "iostream"

#include "EdgeSequence.h"

std::vector< Graph::EdgeId > VorDiagAux::get_bound_edges_inc_to_nodeset(const Voronoi_diagram& vor_diag,
                                                                        const std::vector<Graph::NodeId>& input_nodes) {

    std::vector< Graph::EdgeId > output;

    for( auto curr_node_id : input_nodes ) {

        for( auto curr_edge_id : vor_diag.original_graph().get_node(curr_node_id).incident_edge_ids() ) {
            const Graph::Edge& curr_edge = vor_diag.original_graph().get_edge(curr_edge_id);

            if( vor_diag.check_if_bound_edge(curr_edge) ) {

                output.push_back(curr_edge_id);
            }
        }
    }

    return output;
}

std::vector<Graph::EdgeId> VorDiagAux::compute_bound_path(const Voronoi_diagram& original_vd,
                                                          Graph::EdgeId bound_edge_id) {

    std::vector<Graph::EdgeId> output_edge_ids;

    if(bound_edge_id == Graph::invalid_node_id) {
        return output_edge_ids;
    }

    const std::vector<std::pair<Graph::NodeId, Graph::EdgeId>>& predecessor = original_vd.predecessor();
    const Graph::Edge& bound_edge = original_vd.original_graph().get_edge(bound_edge_id);

    if(not original_vd.check_if_bound_edge(bound_edge)) {
        throw std::runtime_error("(VorDiagAux::compute_bound_path) "
                                 "Eingabekante keine boundary edge im Eingabe-Voronoi-Diagramm.");
    }

    //finde die Kanten auf dem Weg von node_a zur Basis von node_a und füge sie zu output_edge_ids hinzu

    Graph::EdgeId curr_edge_id = predecessor[bound_edge.node_a()].second;
    Graph::NodeId next_node_id = predecessor[bound_edge.node_a()].first;

    while( curr_edge_id != Graph::invalid_edge_id) {
        output_edge_ids.push_back(curr_edge_id);
        curr_edge_id = predecessor[next_node_id].second;
        next_node_id = predecessor[next_node_id].first;
    }

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

    return output_edge_ids;
}

std::pair<Graph::NodeId, Graph::NodeId> VorDiagAux::get_bases_of_edge(const Voronoi_diagram& original_vd,
                                                                      Graph::EdgeId input_edgeid) {

    std::pair<Graph::NodeId, Graph::NodeId> output;

    output.first = original_vd.base()[original_vd.original_graph().get_edge(input_edgeid).node_a()];
    output.second = original_vd.base()[original_vd.original_graph().get_edge(input_edgeid).node_b()];

    return output;
}

Graph::PathLength VorDiagAux::compute_length_of_boundpath(const Voronoi_diagram& original_vd,
                                                          Graph::EdgeId input_edgeid) {
    if(input_edgeid >= original_vd.original_graph().num_edges() ){
        throw std::runtime_error("(VorDiagAux::compute_length_of_boundpath) "
                                 "Eingabekante liegt nicht in dem Eingabe-V-D zugrundeliegenden Graphen.");
    }

    const Graph::Edge& input_edge = original_vd.original_graph().get_edge(input_edgeid);

    if( not original_vd.check_if_bound_edge(input_edge)) {
        throw std::runtime_error("(VorDiagAux::compute_length_of_boundpath) "
                                 "Eingabekante ist keine boundary edge im Eingabe-Vor-Diagramm.");
    }

    return input_edge.weight() + original_vd.dist_to_base()[input_edge.node_a()] + original_vd.dist_to_base()[input_edge.node_b()];
}
