//
// Created by Lukas on 17.06.2022.
//

#include "vor_diag_aux_functions.h"

#include "iostream"

bool check_if_vor_regions_equal(const Voronoi_diagram& vd1, const Voronoi_diagram& vd2) {
    if(vd1.original_graph().num_nodes() != vd2.original_graph().num_nodes()) {
        throw std::runtime_error("(check_if_vor_regions_equal) zugrundeliegende Graphen haben unterschiedliche Knotenanzahl");
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

bool check_if_vor_dist_equal(const Voronoi_diagram& vd1, const Voronoi_diagram& vd2) {
    if(vd1.original_graph().num_nodes() != vd2.original_graph().num_nodes()) {
        throw std::runtime_error("(check_if_vor_dist_equal) zugrundeliegende Graphen haben unterschiedliche Knotenanzahl");
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

void compare_vor_diag(const Voronoi_diagram& vd1, const Voronoi_diagram& vd2){
    std::cout << "Vergleich der Voronoi-Diagramme: \n";
    std::cout << "check_if_vor_regions_equal: " << check_if_vor_regions_equal(vd1, vd2) << "\n";
    std::cout << "check_if_vor_dist_equal: " << check_if_vor_dist_equal(vd1, vd2) << "\n";
    std::cout << "\n";
}