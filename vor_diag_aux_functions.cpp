//
// Created by Lukas on 17.06.2022.
//

#include "vor_diag_aux_functions.h"

#include "iostream"

#include "EdgeSequence.h"

bool VorDiagAux::check_if_vor_regions_equal(const Voronoi_diagram& vd1, const Voronoi_diagram& vd2) {
    if(vd1.original_graph().num_nodes() != vd2.original_graph().num_nodes()) {
        throw std::runtime_error("(VorDiagAux::check_if_vor_regions_equal) zugrundeliegende Graphen haben unterschiedliche Knotenanzahl");
    }
    unsigned int num_nodes = vd1.original_graph().num_nodes();

    std::vector<Graph::NodeId> vd1_base = vd1.base();
    std::vector<Graph::NodeId> vd2_base = vd2.base();

    for(Graph::NodeId curr_node=0; curr_node<num_nodes; curr_node++){
        if( vd1_base[curr_node] != vd2_base[curr_node] ) {
            return false;
        }
    }

    return true;
}

bool VorDiagAux::check_if_vor_dist_equal(const Voronoi_diagram& vd1, const Voronoi_diagram& vd2) {
    if(vd1.original_graph().num_nodes() != vd2.original_graph().num_nodes()) {
        throw std::runtime_error("(VorDiagAux::check_if_vor_dist_equal) zugrundeliegende Graphen haben unterschiedliche Knotenanzahl");
    }
    unsigned int num_nodes = vd1.original_graph().num_nodes();

    std::vector<Graph::PathLength> vd1_dist = vd1.dist_to_base();
    std::vector<Graph::PathLength> vd2_dist = vd2.dist_to_base();

    for(Graph::NodeId curr_node=0; curr_node<num_nodes; curr_node++){
        if( vd1_dist[curr_node] != vd2_dist[curr_node] ) {
            return false;
        }
    }

    return true;
}

void VorDiagAux::compare_vor_diag(const Voronoi_diagram& vd1, const Voronoi_diagram& vd2){
    std::cout << "Vergleich der Voronoi-Diagramme: \n";
    std::cout << "check_if_vor_regions_equal: " << VorDiagAux::check_if_vor_regions_equal(vd1, vd2) << "\n";
    std::cout << "check_if_vor_dist_equal: " << VorDiagAux::check_if_vor_dist_equal(vd1, vd2) << "\n";
    std::cout << "\n";
}

std::vector< EdgeSequence > VorDiagAux::get_bound_paths_inc_to_nodeset(const Voronoi_diagram& vd, const std::vector<Graph::NodeId>& input_nodes) {
    std::vector< EdgeSequence > output;

    for( auto curr_node_id : input_nodes ) {
        for( auto curr_edge_id : vd.original_graph().get_node(curr_node_id).incident_edge_ids() ) {
            Graph::Edge curr_edge = vd.original_graph().get_edge(curr_edge_id);
            if( vd.check_if_bound_edge(curr_edge) ) {
                EdgeSequence curr_bound_path = VorDiagAux::compute_bound_path(vd, curr_edge_id);
                output.push_back(curr_bound_path);
            }
        }
    }

    //debug
    if( output.empty() ) {
        std::cout << "(VorDiagAux::get_bound_paths_inc_to_nodeset) Ausgabe ist leer. \n";
    }

    return output;
}

std::vector<Graph::EdgeId> VorDiagAux::get_all_bound_edges(const Voronoi_diagram& input_vd) {
    std::vector<Graph::EdgeId> output;

    for(auto curr_edge : input_vd.original_graph().edges()) {
        if( input_vd.check_if_bound_edge(curr_edge) ) {
            output.push_back(curr_edge.edge_id());
        }
    }

    //debug
    if( output.empty() ) {
        std::cout << "(vd_get_min_bound_edge_inc_to_nodeset) Ausgabe ist leer. \n";
    }

    return output;
}

EdgeSequence VorDiagAux::compute_bound_path(Voronoi_diagram original_vd, Graph::EdgeId bound_edge_id) {

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

    //konstruire die Ausgabe mit den entsprechenden Basen als Endknoten und entsprechender Länge

    Graph::NodeId node_a = bound_edge.node_a();
    Graph::NodeId node_b = bound_edge.node_b();

    Graph::NodeId base_a = original_vd.base()[node_a];
    Graph::NodeId base_b = original_vd.base()[node_b];

    Graph::PathLength output_length = original_vd.dist_to_base()[node_a] + original_vd.dist_to_base()[node_b] + bound_edge.weight();

    EdgeSequence output_path(output_edge_ids, base_a, base_b, output_length);

    return output_path;
}

std::pair<Graph::NodeId, Graph::NodeId> VorDiagAux::get_bases_of_edge(Voronoi_diagram original_vd, Graph::EdgeId input_edge_id) {
    std::pair<Graph::NodeId, Graph::NodeId> output;

    output.first = original_vd.base()[original_vd.original_graph().get_edge(input_edge_id).node_a()];
    output.second = original_vd.base()[original_vd.original_graph().get_edge(input_edge_id).node_b()];

    return output;
}
