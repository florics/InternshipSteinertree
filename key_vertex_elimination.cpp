//
// Created by Lukas on 30.06.2022.
//

#include "key_vertex_elimination.h"

#include "iostream"
#include "algorithm"

#include "local_search_aux.h"
#include "vor_diag_aux_functions.h"
#include "NCA_Tree_Data_Structure.h"
#include "graph_algorithms.h"
#include "general_aux_functions.h"
#include "graph_aux_functions.h"
#include "graph_printfunctions.h"


std::vector<EdgeSequence> KeyVertexElim::find_outgoing_keypaths(const Graph &input_graph, Graph::NodeId start_node,
                                                                 std::vector<std::vector<Graph::NodeId>>& vect_internal_node_ids) {
    if( input_graph.dir_type() == Graph::undirected) {
        throw std::runtime_error("(KeyVertexElim::find_outgoing_keypaths) Graph ist ungerichtet.");
    }
    if( start_node == Graph::invalid_node_id) {
        throw std::runtime_error("(KeyVertexElim::find_outgoing_keypaths) Eingabeknoten ist ungueltig.");
    }
    if( start_node > input_graph.num_nodes() ) {
        throw std::runtime_error("(KeyVertexElim::find_outgoing_keypaths) Eingabeknoten liegt nicht im Eingabegraphen.");
    }
    if( not LocalSearchAux::check_if_crucial(input_graph.get_node(start_node)) ) {
        throw std::runtime_error("(KeyVertexElim::find_outgoing_keypaths) Eingabeknoten crucial noe.");
    }

    std::vector<EdgeSequence> output;
    output.reserve(input_graph.get_node(start_node).num_neighbors()-1);

    vect_internal_node_ids.clear();
    vect_internal_node_ids.reserve(input_graph.get_node(start_node).num_neighbors()-1);

    //nummeriert die gefundenen KeyPaths durch
    unsigned int out_kpaths_counter = 0;

    const std::vector<Graph::EdgeId> vect_start_out_edges = input_graph.get_outgoing_edge_ids(start_node);

    for(auto start_out_edge_id: vect_start_out_edges ){
        const Graph::Edge& start_out_edge = input_graph.get_edge(start_out_edge_id);

        Graph::PathLength key_path_length = start_out_edge.weight();
        std::vector<Graph::EdgeId> key_path_edges(1, start_out_edge_id);

        Graph::NodeId curr_node_id = input_graph.get_head(start_out_edge_id);

        std::vector<Graph::NodeId> var_intern_node_ids;

        //Schleife endet, wenn crucial child gefunden
        while (true) {
            const Graph::Node& curr_node = input_graph.get_node(curr_node_id);

            if (LocalSearchAux::check_if_crucial(curr_node)) {
                if (curr_node_id == start_node) {
                    throw std::runtime_error("(LocalSearchAux::find_outgoing_keypaths) Eingabegraph hat Kreis");
                }
                //In dem Fall ist der aktuelle Knoten der andere Endpunkt des Key-Paths
                break;
            }

            //finde ausgehende Kante des aktuellen Knoten
            const std::vector<Graph::EdgeId> curr_out_edge_id = input_graph.get_outgoing_edge_ids(curr_node_id);

            if (curr_out_edge_id.empty()) {
                if (not input_graph.check_if_terminal(curr_node_id)) {
                    std::cout << "aktueller Knoten hat NodeId: " << curr_node_id << "\n";
                    throw std::runtime_error(
                            "(LocalSearchAux::find_outgoing_keypaths) Eingabegraph hat ein Steinerblatt");
                } else {
                    break;
                }
            }

            // der aktuelle Knoten ist kein crucial node und hat eine ausgehenden sowie eine eingehende Kante,
            // er liegt also als innerer Knoten auf dem KeyPath
            var_intern_node_ids.push_back(curr_node_id);

            //füge das Gewicht der Kante zur Länge des Key-Paths hinzu
            const Graph::Edge& curr_out_edge = input_graph.get_edge(curr_out_edge_id.front());
            key_path_length += curr_out_edge.weight();
            //füge die Kante zur Kantenmenge des Key-Path hinzu
            key_path_edges.push_back(curr_out_edge_id.front());

            const Graph::Node& curr_out_neighbor = input_graph.get_node(input_graph.get_head(curr_out_edge_id.front()));
            curr_node_id = curr_out_neighbor.node_id();
        }

        EdgeSequence var_path(key_path_edges, start_node, curr_node_id, key_path_length);
        output.push_back(var_path);

        vect_internal_node_ids.push_back(var_intern_node_ids);

        out_kpaths_counter++;
    }

    return output;
}

//? löschen oder überarbeiten
/*
std::vector<Graph::NodeId>
KeyVertexElim::get_keynodes_in_postorder(const Graph &input_graph, Graph::NodeId root_id) {
    // Checks weglassen ?
    if( root_id == Graph::invalid_node_id) {
        throw std::runtime_error("(KeyPathExch::get_crucialnodes_in_postorder) Eingabeknoten ungueltig");
    }
    if( root_id > input_graph.num_nodes()) {
        throw std::runtime_error("(KeyPathExch::get_crucialnodes_in_postorder) Eingabeknoten nicht im Graphen");
    }

    //speichert die crucial nodes in Reihenfolge der Graphendurchmusterung
    std::vector<Graph::NodeId> output;
    //? output.push_back(root_id);

    // es folgt eine Graphendurchmusterung

    std::vector<Graph::NodeId> next_nodes (1, root_id);
    //speichert die bereits erreichten Knoten (die, die bereits zu next_nodes hinzugfügt wurden)
    //Laufzeit O(num_nodes()) Problem?
    std::vector<bool> reached_nodes (input_graph.num_nodes(), false);
    reached_nodes[root_id] = true;

    while( not next_nodes.empty()) {
        Graph::NodeId curr_node_id = next_nodes.back();
        next_nodes.pop_back();

        for( auto curr_edge_id : input_graph.incident_edge_ids(curr_node_id) ) {
            Graph::NodeId curr_neighbor_id = input_graph.get_edge(curr_edge_id).get_other_node(curr_node_id);

            if( not reached_nodes[curr_neighbor_id] ) {
                reached_nodes[curr_neighbor_id] = true;
                next_nodes.push_back(curr_neighbor_id);

                if( KeyVertexElim::check_if_keyvertex(input_graph, curr_neighbor_id) ) {
                    output.push_back( curr_neighbor_id );
                }
            }
        }
    }

    //kehre die Reihenfolge um
    std::reverse(output.begin(), output.end());

    return output;
}
*/

bool KeyVertexElim::check_if_keyvertex(const Graph &input_graph, Graph::NodeId input_node_id) {
    if( input_node_id == Graph::invalid_node_id) {
        throw std::runtime_error("(KeyVertexElim::check_if_keyvertex) Eingabeknoten ungueltig");
    }
    if( input_node_id > input_graph.num_nodes()) {
        throw std::runtime_error("(KeyVertexElim::check_if_keyvertex) Eingabeknoten nicht im Eingabegraphen");
    }

    const Graph::Node& input_node = input_graph.get_node(input_node_id);
    if(input_node.num_neighbors() > 2 && not input_node.check_if_terminal() ) {
        return true;
    } else {
        return false;
    }
}

Horizontal_Edges_Lists KeyVertexElim::compute_horizontal_bound_edges(const Subgraph &input_subgraph, Graph::NodeId root,
                                              const Voronoi_diagram &vor_diag) {

    const Graph& original_graph = input_subgraph.original_graph();
    const Graph& solution_graph = input_subgraph.this_graph();
    const std::vector<Graph::NodeId>& solution_nodeids_of_original_nodes = input_subgraph.subgraph_nodeids_of_nodes_in_originalgraph();

    const std::vector<Graph::NodeId> key_nodes = KeyVertexElim::get_key_nodes(solution_graph);

    const std::vector<Horizontal_Edges_Lists::ListId> list_ids =
            LocalSearchAux::compute_list_ids_for_horizon_edges_lists(solution_graph.num_nodes(), key_nodes);

    std::vector<std::vector<Graph::EdgeId> > vect_of_lists(key_nodes.size(), std::vector<Graph::EdgeId>());

    NCA_Tree_Data_Structure tree_structure(solution_graph, root);

    //Schleife über alle boundary edges
    for( const auto& curr_edge: original_graph.edges()) {
        if( vor_diag.check_if_bound_edge(curr_edge) ){

            Graph::EdgeId curr_edge_id = curr_edge.edge_id();

            // finde die zur boundary edge gehörenden Basen (als NodeIds im Lösungsgraphen)
            const std::pair<Graph::NodeId, Graph::NodeId> curr_bases = VorDiagAux::get_bases_of_edge(vor_diag, curr_edge_id);
            const Graph::NodeId base_a = solution_nodeids_of_original_nodes[curr_bases.first];
            const Graph::NodeId base_b = solution_nodeids_of_original_nodes[curr_bases.second];

            //finde den NCA der Kante und füge ihn zur entsprechenden Liste hinzu

            Graph::NodeId nearest_common_ancestor = tree_structure.nearest_common_ancestor(base_a, base_b);

            //falls der NCA der Kante einer der Endpunkte selbst ist, ist die Kante keine horizontale boundary edge
            if( nearest_common_ancestor != Graph::invalid_node_id) {

                // falls der NCA ein Knoten ist, für den wir eine Liste anlegen, fügen wir ihn zu dieser hinzu
                Horizontal_Edges_Lists::ListId curr_list_id = list_ids[nearest_common_ancestor];
                if( curr_list_id != Horizontal_Edges_Lists::no_list_available) {
                    vect_of_lists[curr_list_id].push_back(curr_edge_id);
                }
            }

        }
    }

    return Horizontal_Edges_Lists(list_ids, vect_of_lists);
}

void KeyVertexElim::add_horizontal_bound_edges(const Subgraph& input_subgraph,
                                               const Voronoi_diagram& vor_diag,
                                               Ext_Union_Find_Structure& subtrees_ufs,
                                               LocalSearchAux::MovesPerPass moves_per_pass,
                                               const std::vector<bool>& forbidden,
                                               Supergraph_KVE& super_graph,
                                               const std::vector<Graph::EdgeId>& horiz_bound_edges) {

    //? const Graph& solution_graph = input_subgraph.this_graph();
    //? const Graph& original_graph = input_subgraph.original_graph();
    const std::vector<Graph::NodeId>& solution_nodeids_of_original_nodes = input_subgraph.subgraph_nodeids_of_nodes_in_originalgraph();

    for( auto curr_edge_id: horiz_bound_edges) {


        //debug
        //std::cout << "Kandidat ist ";
        //GraphAuxPrint::print_edge_sequence(original_graph, VorDiagAux::compute_bound_path(vor_diag, curr_edge_id));

        // finde die zur boundary edge gehörenden Endpunkte bzw. Basen (als NodeIds in der aktuellen Lösung)
        const std::pair<Graph::NodeId, Graph::NodeId> curr_bases = VorDiagAux::get_bases_of_edge(vor_diag, curr_edge_id);
        const Graph::NodeId base_a = solution_nodeids_of_original_nodes[curr_bases.first];
        const Graph::NodeId base_b = solution_nodeids_of_original_nodes[curr_bases.second];

        if( moves_per_pass == LocalSearchAux::several_moves) {
            if( forbidden[base_a] || forbidden[base_b]) {
                continue;
            }
        }

        const Graph::NodeId supernode_id_a = subtrees_ufs.get_superid(base_a);
        const Graph::NodeId supernode_id_b = subtrees_ufs.get_superid(base_b);

        //bemerke, dass eine Kante aus dieser Liste keinen Endpunkt in dem Subbaum des crucial parent haben kann
        // (sie kann auch nicht mit dem aktuellen Knoten selbst, sowie dem eingehenden KeyPath inzident sein,
        // das ist aber hier nicht relevant)
        // Endpunkte mit super_id 0 müssen also interne Knoten der KeyPaths sein

        if( supernode_id_a == 0 || supernode_id_b == 0) {
            // innere Knoten als Endknoten wollen wir nicht zum Supergraphen hinzufügen
            continue;
        }

        //debug
        if( supernode_id_a == supernode_id_b) {
            throw std::runtime_error("(KeyVertexElim::add_horizontal_bound_edges) keine horizontale bound edge");
        }


        super_graph.add_edge(supernode_id_a, supernode_id_b,
                             VorDiagAux::compute_length_of_boundpath(vor_diag, curr_edge_id),
                             curr_edge_id, Supergraph_KVE::BoundEdgeType::original);

    }
}

Voronoi_diagram::RestoreData KeyVertexElim::find_and_add_new_bound_edges(const Subgraph& input_subgraph,
                                                 Graph::NodeId start_node_id,
                                                 Voronoi_diagram& vor_diag,
                                                 Ext_Union_Find_Structure& subtrees_ufs,
                                                 LocalSearchAux::MovesPerPass moves_per_pass,
                                                 std::vector<bool>& forbidden,
                                                 const std::vector<std::vector<Graph::NodeId>>& vect_internal_nodes,
                                                 Supergraph_KVE& super_graph) {

    //debug
    // std::cout << "Aufruf von KeyVertexElim::find_and_add_new_bound_edges: \n";

    const std::vector<Graph::NodeId>& original_nodeids = input_subgraph.original_nodeids();
    const std::vector<Graph::NodeId>& solution_nodeids_of_original_nodes = input_subgraph.subgraph_nodeids_of_nodes_in_originalgraph();
    
    //füge alle zu entfernenden Basen als NodeIds im zugrundeliegenden Graphen zu einem Vektor zusammen 
    std::vector<Graph::NodeId> bases_to_delete(1, original_nodeids[start_node_id]);
    for( const auto& curr_vect: vect_internal_nodes) {
        bases_to_delete.reserve(bases_to_delete.size() + curr_vect.size());
        for( auto curr_node: curr_vect) {
            bases_to_delete.push_back( original_nodeids[curr_node] );
        }
    }
    const Voronoi_diagram::RestoreData vor_diag_restore_data = vor_diag.repair(bases_to_delete);

    // bemerke, dass in vor_diag_restore_data.node_ids alle Knoten gespeichert sind, deren Basis während repair verändert wurde
    const std::vector<Graph::EdgeId> new_bound_edge_ids = VorDiagAux::get_bound_edges_inc_to_nodeset(vor_diag, vor_diag_restore_data.node_ids);

    for( auto curr_bound_edge_id: new_bound_edge_ids) {

        //finde die Endpunkte der boundary edge als NodeIds im solution-Graph
        const std::pair<Graph::NodeId, Graph::NodeId> curr_bases = VorDiagAux::get_bases_of_edge(vor_diag, curr_bound_edge_id);
        const Graph::NodeId var_base_a = solution_nodeids_of_original_nodes[curr_bases.first];
        const Graph::NodeId var_base_b = solution_nodeids_of_original_nodes[curr_bases.second];

        //prüfe ob diese Endpunkte forbidden sind
        if(moves_per_pass == LocalSearchAux::several_moves){
            if (forbidden[var_base_a] || forbidden[var_base_b]) {
                continue;
            }
        }

        const Graph::NodeId supernode_a = subtrees_ufs.get_superid(var_base_a);
        const Graph::NodeId supernode_b = subtrees_ufs.get_superid(var_base_b);


        if( supernode_a != supernode_b) {
            //debug
            //GraphAuxPrint::print_edge_sequence(input_subgraph.original_graph(), VorDiagAux::compute_bound_path(vor_diag, curr_bound_edge_id));

            //bemerke, dass die neuen boundary edges keinen der internen Knoten als Endpunkt haben können
            // ist die super_id eines Knotens 0, liegt dieser also in dem Subbaum des crucial parent
            // wir können die Kante also einfach mit den gefundenen super_id's hinzufügen
            super_graph.add_edge(supernode_a, supernode_b, VorDiagAux::compute_length_of_boundpath(vor_diag, curr_bound_edge_id),
                                 curr_bound_edge_id, Supergraph_KVE::BoundEdgeType::newly_found);
        }

    }

    return vor_diag_restore_data;
}

void KeyVertexElim::find_and_add_vertical_bound_edges(Graph::NodeId start_node_id,
                                                      const Voronoi_diagram& vor_diag,
                                                      Ext_Union_Find_Structure &subtrees_ufs,
                                                      Edge_Heaps &vert_bound_edge_heaps,
                                                      LocalSearchAux::MovesPerPass moves_per_pass,
                                                      std::vector<bool> &forbidden,
                                                      const std::vector<std::vector<Graph::NodeId>> &vect_internal_nodes,
                                                      const std::vector<Graph::NodeId>& crucial_children,
                                                      Supergraph_KVE &super_graph) {

    //clean up vorbereiten:
    // markiere alle Knoten aus den Subbäumen, die zu crucial children von dem aktuellen Knoten gehören,
    // alle internen Knoten auf den inzidenten KeyPaths, sowie den aktuellen Knoten selbst als verboten
    // (d.h. die Wurzeln der entsprechenden Mengen werden auf allowed=false gesetzt)
    subtrees_ufs.set_allowed(start_node_id, false);
    for(auto curr_intern_nodes: vect_internal_nodes) {
        if( not curr_intern_nodes.empty()) {
            subtrees_ufs.set_allowed(curr_intern_nodes.front(), false);
        }
    }
    for(auto curr_crucial_child: crucial_children ) {
        subtrees_ufs.set_allowed(curr_crucial_child, false);
    }

    std::vector<std::pair<Graph::PathLength, Graph::EdgeId>> vertical_bound_edges =
            vert_bound_edge_heaps.cleanup_heaps_kve(crucial_children, vor_diag, subtrees_ufs, moves_per_pass, forbidden);

    //Wiederherstellen der default-Werte von _allowed
    subtrees_ufs.set_allowed(start_node_id, true);
    for(auto curr_intern_nodes: vect_internal_nodes) {
        if( not curr_intern_nodes.empty()) {
            subtrees_ufs.set_allowed(curr_intern_nodes.front(), false);
        }
    }
    subtrees_ufs.set_allowed_multiple_elts(crucial_children, true);

    // vertikale boundary edges zum Supergraphen hinzufügen
    for(unsigned int i=0; i<vertical_bound_edges.size(); i++) {
        const std::pair<Graph::PathLength, Graph::EdgeId>& curr_bound_edge = vertical_bound_edges[i];

        //prüfe, ob wir für das i+1-ten crucial child eine vertikale boundary edge gefunden haben
        if( curr_bound_edge.first < Graph::infinite_length) {
            //debug
            //GraphAuxPrint::print_edge_sequence(input_subgraph.original_graph(), VorDiagAux::compute_bound_path(vor_diag, curr_bound_edge.second));

            //die boundary edge verläuft zwischen dem Subbaum, der zum (i+1)-ten crucial child gehört, und dem Subbaum des crucial parent
            super_graph.add_edge(0, i+1, curr_bound_edge.first, curr_bound_edge.second, Supergraph_KVE::BoundEdgeType::original);
        }
    }

}

std::vector<Graph::EdgeId> KeyVertexElim::get_edges_to_insert(Supergraph_KVE::BoundEdgeType bound_edge_type,
                                                              const std::vector<Graph::NodeId>& solution_nodeids_of_original_nodes,
                                                              const Voronoi_diagram& vor_diag,
                                                              const Supergraph_KVE& super_graph,
                                                              const Subgraph& mst_of_supergraph,
                                                              std::vector<bool>& added_edges,
                                                              LocalSearchAux::MovesPerPass moves_per_pass,
                                                              std::vector<bool>& pinned) {
    std::vector<Graph::EdgeId> output;

    //Schleife über alle Kanten des MST (Subgraph des Supergraphen)
    for(const auto& curr_edge: mst_of_supergraph.this_graph().edges()) {

        //finde die edge id im supergraph
        const Graph::EdgeId curr_edge_id_in_supergraph = mst_of_supergraph.original_edgeids()[ curr_edge.edge_id() ];

        //prüfe, ob neue bzw. alte boundary edge
        if(super_graph.superedges_corresponding_to_new_bound_edges()[curr_edge_id_in_supergraph] == bound_edge_type ) {
            //finde die edge id im zugrundeliegenden Graph (zugrundeliegend bzgl. der aktuellen Lösung)
            const Graph::EdgeId curr_original_edge_id = super_graph.original_edge_ids()[ curr_edge_id_in_supergraph ];

            //finde die Kanten des zugehörigen boundary paths
            const std::vector<Graph::EdgeId> curr_edges_to_add = VorDiagAux::compute_bound_path_as_vect(vor_diag, curr_original_edge_id);

            output.reserve(output.size() + curr_edges_to_add.size());
            for( auto var_edge_to_add: curr_edges_to_add) {
                if (not added_edges[var_edge_to_add]) {
                    output.push_back(var_edge_to_add);
                    added_edges[var_edge_to_add] = true;
                }
            }

            if(moves_per_pass == LocalSearchAux::MovesPerPass::several_moves) {
                LocalSearchAux::update_pinned_for_bound_egde(vor_diag, solution_nodeids_of_original_nodes, pinned, curr_original_edge_id);
            }

        }
    }

    output.shrink_to_fit();

    return output;
}


ImprovingChangement KeyVertexElim::process_node(const Subgraph& input_subgraph,
                                                Graph::NodeId start_node_id,
                                                Voronoi_diagram& vor_diag,
                                                Ext_Union_Find_Structure& subtrees_ufs,
                                                const Horizontal_Edges_Lists& horiz_bound_edges_lists,
                                                Edge_Heaps& vert_bound_edge_heaps,
                                                LocalSearchAux::MovesPerPass moves_per_pass,
                                                std::vector<bool>& forbidden,
                                                std::vector<bool>& pinned){


    const Graph& original_graph = input_subgraph.original_graph();
    const Graph& solution_graph = input_subgraph.this_graph();
    const std::vector<Graph::NodeId>& solution_nodeids_of_original_nodes = input_subgraph.subgraph_nodeids_of_nodes_in_originalgraph();
    //? const std::vector<Graph::NodeId>& original_nodeids = input_subgraph.original_nodeids();

    //debug
    //std::cout << "Startknoten: " << original_nodeids[start_node_id]+1 << "\n";


    // finde die KeyPaths die im Eingabeknoten starten
    std::vector<std::vector<Graph::NodeId>> vect_out_internal_nodes;
    const std::vector<EdgeSequence> outgoing_keypaths = KeyVertexElim::find_outgoing_keypaths(solution_graph, start_node_id, vect_out_internal_nodes);

    // speichert die crucial children des aktuellen Knoten
    std::vector<Graph::NodeId> crucial_children;
    crucial_children.reserve(outgoing_keypaths.size());
    for(const auto& curr_path: outgoing_keypaths) {
        crucial_children.push_back(curr_path.endnode_b());
    }

    //Vorbereiten der Aktualisierungen von Edge_Heaps, Union-Find

    std::vector<Graph::NodeId> heaps_to_merge = crucial_children;
    for( auto curr_intern_nodes: vect_out_internal_nodes) {
        heaps_to_merge.insert(heaps_to_merge.end(), curr_intern_nodes.begin(), curr_intern_nodes.end());
    }

    std::vector<Union_Find_Structure::ElementId> elements_to_union = crucial_children;
    for( auto curr_intern_nodes: vect_out_internal_nodes) {
        if( not curr_intern_nodes.empty()) {
            elements_to_union.push_back(curr_intern_nodes.front());
        }
    }
    elements_to_union.push_back(start_node_id);


    // finde den KeyPath der im Eingabeknoten endet
    std::vector<Graph::NodeId> in_internal_nodes;
    const EdgeSequence ingoing_keypath = LocalSearchAux::find_ingoing_keypath(solution_graph, start_node_id, in_internal_nodes);

    // füge die internen Knoten des eingehenden KeyPaths zu einer Menge der Union-Find zusammen
    // (denn die anderen sollten bereits in einer ufs-Menge liegen, wenn ich ich am Anfang einmal für alle Blätter
    // jeweils die internen Knoten von eingehenden KeyPaths durchgehe)
    subtrees_ufs.union_multiple_sets(in_internal_nodes);

    //falls der aktuelle Knoten ein Terminal ist, wollen wir lediglich die Strukturen aktualisieren
    //falls der aktuelle Knoten pinned ist, wollen wir lediglich die Strukturen aktualisieren
    if( solution_graph.check_if_terminal(start_node_id)
        || ( moves_per_pass == LocalSearchAux::MovesPerPass::several_moves && pinned[start_node_id] ) ) {

        vert_bound_edge_heaps.merge(start_node_id, heaps_to_merge);

        subtrees_ufs.union_multiple_sets(elements_to_union);

        return ImprovingChangement(std::vector<Graph::EdgeId>(), std::vector<Graph::EdgeId>(), 0);
    }

    //falls der aktuelle Knoten kein Terminal ist, aber keine crucial children besitzt, gibt es ein Steinerblatt
    if( crucial_children.empty() ) {
        throw std::runtime_error("(KeyVertexElim::process_node) Der Eingabegraph enthält ein Steinerblatt.");
    }

    // füge inzidente KeyPaths (bzw. interne Knoten) zu einem Vektor zusammen, in dem der Eintrag 0 dem eingehenden KeyPath entspricht
    std::vector<std::vector<Graph::NodeId>> vect_internal_nodes (1, in_internal_nodes);
    vect_internal_nodes.insert(vect_internal_nodes.end(), vect_out_internal_nodes.begin(), vect_out_internal_nodes.end());
    std::vector<EdgeSequence> incident_keypaths(1, ingoing_keypath);
    incident_keypaths.insert(incident_keypaths.end(), outgoing_keypaths.begin(), outgoing_keypaths.end());


    //prüfe, ob einer der internen Knoten der inzidenten KeyPaths pinned ist
    //? das hier anders prüfen (eher so lassen)
    if( moves_per_pass == LocalSearchAux::MovesPerPass::several_moves) {
        for (const auto& curr_vect: vect_internal_nodes) {
            for (auto curr_intern_node: curr_vect) {
                if (pinned[curr_intern_node]) {

                    vert_bound_edge_heaps.merge(start_node_id, heaps_to_merge);

                    subtrees_ufs.union_multiple_sets(elements_to_union);

                    return ImprovingChangement(std::vector<Graph::EdgeId>(), std::vector<Graph::EdgeId>(), 0);
                }
            }
        }
    }

    // setze die super_id-Werte der Union-Find so, dass die Mengen, die den crucial children entsprechen, durchnummeriert werden (beginnend bei 1)
    Graph::NodeId new_id = 1;
    for( auto curr_crucial_child: crucial_children) {
        subtrees_ufs.set_superid(curr_crucial_child, new_id);
        new_id++;
    }


    unsigned int num_supernodes = incident_keypaths.size();
    Supergraph_KVE super_graph(num_supernodes);

    KeyVertexElim::add_horizontal_bound_edges(input_subgraph, vor_diag, subtrees_ufs, moves_per_pass, forbidden,
                                              super_graph, horiz_bound_edges_lists.get_list(start_node_id) );

    KeyVertexElim::find_and_add_vertical_bound_edges(start_node_id, vor_diag, subtrees_ufs, vert_bound_edge_heaps,
                                                     moves_per_pass, forbidden, vect_internal_nodes, crucial_children,
                                                     super_graph);

    Voronoi_diagram::RestoreData vor_diag_restore_data =
            KeyVertexElim::find_and_add_new_bound_edges(input_subgraph, start_node_id, vor_diag, subtrees_ufs,
                                                        moves_per_pass, forbidden, vect_internal_nodes, super_graph);

    // reset der super_id's auf den default-Wert 0
    subtrees_ufs.reset_superids(crucial_children);

    //falls der Supergraph nicht zusammenhängend ist, können wir keine Nachbarschaftslösung finden
    // (also nur Aktualisieren der Strukturen)
    if( not GraphAux::check_if_graph_is_connected(super_graph.this_graph()) ){

        vor_diag.restore(vor_diag_restore_data);

        vert_bound_edge_heaps.merge(start_node_id, heaps_to_merge);

        subtrees_ufs.union_multiple_sets(elements_to_union);

        return ImprovingChangement(std::vector<Graph::EdgeId>(), std::vector<Graph::EdgeId>(), 0);
    }

    // der Supergraph als Subgraph von sich selbst
    const Subgraph supergraph_as_subgraph(super_graph.this_graph());

    const Subgraph mst_of_supergraph = GraphAlgorithms::mst_prim_for_subgraphs(supergraph_as_subgraph, 0);

    // Wert der Verbesserung berechnen
    Graph::PathLength weight_incident_key_paths = 0;
    for( const auto& curr_path: incident_keypaths){
        weight_incident_key_paths += curr_path.length();
    }
    const Graph::PathLength weight_mst_of_supergraph = GraphAux::length_of_all_edges( mst_of_supergraph.this_graph() );
    const Graph::PathLength improve_value = weight_incident_key_paths - weight_mst_of_supergraph;

    // falls die gefundene Nachbarschaftslösung keine echte Verbesserung ist, wollen wir lediglich die Strukturen aktualisieren
    if ( improve_value <= 0 ) {
        vor_diag.restore(vor_diag_restore_data);

        vert_bound_edge_heaps.merge(start_node_id, heaps_to_merge);

        subtrees_ufs.union_multiple_sets(elements_to_union);

        return ImprovingChangement(std::vector<Graph::EdgeId>(), std::vector<Graph::EdgeId>(), 0);
    }

    //finde die dem MST des Supergraphen entsprechenden Kanten im zugrundeliegenden Graphen
    // vor restore für die neuen boundary edges, nach restore für die alten
    // dabei wird zudem 'pinned' aktualisiert

    //? Laufzeit: könnte man hier beschleunigen, weil alle Vorgänger einer bereits hinzugefügten Kante auch bereits hinzugefügt wurden
    // (also überall wo ich added_edges verwenden)
    std::vector<bool> added_edges_insert (original_graph.num_edges(), false);

    std::vector<Graph::EdgeId> edges_to_insert_from_new_b_e =
            KeyVertexElim::get_edges_to_insert(Supergraph_KVE::BoundEdgeType::newly_found, solution_nodeids_of_original_nodes, vor_diag,
                                super_graph, mst_of_supergraph, added_edges_insert, moves_per_pass, pinned);

    vor_diag.restore(vor_diag_restore_data);

    std::vector<Graph::EdgeId> edges_to_insert_from_original_b_e =
            KeyVertexElim::get_edges_to_insert(Supergraph_KVE::BoundEdgeType::original, solution_nodeids_of_original_nodes, vor_diag,
                                super_graph, mst_of_supergraph, added_edges_insert, moves_per_pass, pinned);

    std::vector<Graph::EdgeId> edges_to_insert;
    edges_to_insert.insert( edges_to_insert.end(), edges_to_insert_from_new_b_e.begin(), edges_to_insert_from_new_b_e.end() );
    edges_to_insert.insert( edges_to_insert.end(), edges_to_insert_from_original_b_e.begin(), edges_to_insert_from_original_b_e.end() );

    // zu entfernende Kanten zu Vektor zusammenfassen
    std::vector<Graph::EdgeId> edges_to_remove;
    std::vector<bool> added_edges_remove (original_graph.num_edges(), false);
    for( const auto& curr_path: incident_keypaths) {
        for( auto edge_to_add: curr_path.edge_ids()){
            if( not added_edges_remove[edge_to_add] ) {

                edges_to_remove.push_back(edge_to_add);
                added_edges_remove[edge_to_add] = true;
            }
        }
    }

    // es folgen die Aktualisierungen der Hilfstrukturen

    vert_bound_edge_heaps.merge(start_node_id, heaps_to_merge);

    subtrees_ufs.union_multiple_sets(elements_to_union);

    if( moves_per_pass == LocalSearchAux::MovesPerPass::several_moves ) {
        LocalSearchAux::update_forbidden(solution_graph, forbidden, ingoing_keypath.endnode_a());
    }

    return ImprovingChangement(edges_to_remove, edges_to_insert, improve_value);
}


std::vector<ImprovingChangement>
KeyVertexElim::evaluate_neighborhood(Subgraph &input_subgraph, LocalSearchAux::MovesPerPass moves_per_pass) {

    const Graph& original_graph = input_subgraph.original_graph();
    Graph& solution_graph = input_subgraph.this_graph();
    const std::vector<Graph::NodeId>& solution_nodeids_of_original_nodes = input_subgraph.subgraph_nodeids_of_nodes_in_originalgraph();
    const std::vector<Graph::NodeId>& original_nodeids = input_subgraph.original_nodeids();

    //Initialisieren der Union Find, erstelle Menge für alle Knoten in der aktuellen Lösung
    const unsigned int num_nodes_of_solution = solution_graph.num_nodes();
    Ext_Union_Find_Structure subtrees_ufs(num_nodes_of_solution);
    for(unsigned int i=0; i<num_nodes_of_solution; i++) {
        subtrees_ufs.make_set(i);
    }

    Voronoi_diagram vor_diag(original_nodeids, original_graph);

    Edge_Heaps vert_bound_edge_heaps(vor_diag, solution_nodeids_of_original_nodes);

    //richte den Graph mit beliebiger (?) Wurzel, berechne Reihenfolge der Knoten für die Prozessierung
    //? hier kann man root_id frei wählen
    const Graph::NodeId root_id = solution_graph.get_terminals()[0];
    solution_graph.make_rooted_arborescence(root_id);
    std::vector<Graph::NodeId> crucialnodes_in_postorder = LocalSearchAux::get_crucialnodes_in_postorder(solution_graph, root_id);

    //debug
    //std::cout << "Wurzel: " << original_nodeids[root_id]+1 << "\n";

    const Horizontal_Edges_Lists horiz_bound_edges_lists =
            KeyVertexElim::compute_horizontal_bound_edges(input_subgraph, root_id, vor_diag);

    // Hilfsstrukturen, um mehrere Verbesserung in einem pass durchführen zu können
    // Knoten, die als forbidden markiert sind, dürfen nicht mehr verwendet werden, um die Subbäume wieder zu verbinden
    std::vector<bool> forbidden(solution_graph.num_nodes(), false);
    // Knoten, die als pinned markiert sind, dürfen nicht mehr entfernt werden
    std::vector<bool> pinned(solution_graph.num_nodes(), false);

    //main loop der Methode
    std::vector<ImprovingChangement> evaluated_neighborhood;
    for(auto curr_node_id: crucialnodes_in_postorder) {
        const ImprovingChangement curr_changement =
                KeyVertexElim::process_node(input_subgraph, curr_node_id, vor_diag, subtrees_ufs, horiz_bound_edges_lists,
                                            vert_bound_edge_heaps, moves_per_pass, forbidden, pinned);

        if( curr_changement.getImprovementValue() > 0){
            evaluated_neighborhood.push_back(curr_changement);
        }
    }

    return evaluated_neighborhood;
}

void KeyVertexElim::complete_algorithm(Subgraph &input_subgraph) {

    int debug_while_loop_counter = 0;

    while(true) {
        debug_while_loop_counter++;

        //debug
        //GraphAuxPrint::print_graph(input_subgraph.this_graph());

        std::vector<ImprovingChangement> improvements = KeyVertexElim::evaluate_neighborhood(input_subgraph, LocalSearchAux::several_moves);

        if(improvements.empty()) {
            // lokales Optimum erreicht
            break;
        }


        //debug
        /*std::cout << "Schleife " << debug_while_loop_counter << "\n";
        for(auto var_im_ch: improvements) {
            var_im_ch.print(input_subgraph);
        }
        fflush(stdout);*/

        LocalSearchAux::perform_improving_changements(input_subgraph, improvements);

        //debug
        if( not GraphAux::get_steinerleafs(input_subgraph.this_graph()).empty() ) {
            std::cout << "KeyVertexElim: Es entstehen Steinerblätter. \n";
        }
    }
}

std::vector<Graph::NodeId> KeyVertexElim::get_key_nodes(const Graph& input_graph) {
    std::vector<Graph::NodeId> key_nodes;
    key_nodes.reserve(input_graph.num_nodes());

    for(const auto& curr_node: input_graph.nodes()) {
        if( KeyVertexElim::check_if_keyvertex(input_graph, curr_node.node_id()) ) {
            key_nodes.push_back(curr_node.node_id());
        }
    }

    key_nodes.shrink_to_fit();

    return key_nodes;
}
