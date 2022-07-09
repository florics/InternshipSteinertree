//
// Created by Lukas on 05.07.2022.
//

#include "steiner_vertex_elimination.h"

#include "iostream"
#include "algorithm"

#include "graph.h"
#include "Subgraph.h"
#include "Horizontal_Edges_Lists.h"
#include "NCA_Tree_Data_Structure.h"
#include "local_search_aux.h"
#include "Ext_Union_Find_Structure.h"
#include "Edge_Heaps.h"
#include "Supergraph.h"
#include "ImprovingChangement.h"
#include "graph_algorithms.h"
#include "graph_aux_functions.h"
#include "graph_printfunctions.h"
#include "general_aux_functions.h"

namespace SteinerVertexElim {

    // falls das Entfernen des Eingabeknotens (Steinerknoten) zu einer Verbesserung der Lösung führt,
    // wird die entsprechende Veränderung des Graphen ausgegeben
    // falls es keine echte Verbesserung gibt, wird ein leeres ImprovingChangement ausgegeben
    // Ausgabe: die edges_to_remove sind EdgeIds der aktuellen Lösung,
    // die edges_to_insert sind EdgeIds des zugrundeliegenden Graphen.
    // der Eingabeknoten darf auch ein Terminal sein, dann werden lediglich die Strukturen aktualisiert
    ImprovingChangement process_node(const Subgraph& input_subgraph,
                                     Graph::NodeId start_node_id,
                                     Ext_Union_Find_Structure& subtrees_ufs,
                                     const Horizontal_Edges_Lists& horiz_edges_lists,
                                     Edge_Heaps& vert_edge_heaps,
                                     LocalSearchAux::MovesPerPass moves_per_pass,
                                     std::vector<bool>& forbidden,
                                     std::vector<bool>& pinned);

    //gibt alle Knoten des Eingabegraphen aus, in einer post-order bzgl. einer Graphendurchmusterung-Arboreszenz mit dem
    // Eingabeknoten Wurzel, die Wurzel selbst ist nicht in der Ausgabe
    // Wurzel muss Terminal sein
    std::vector<Graph::NodeId> get_nodes_in_postorder(const Graph& input_graph, Graph::NodeId root_id);

    // gibt alle Nicht-Terminale des Eingabegraphen aus
    std::vector<Graph::NodeId> get_steiner_nodes(const Graph& input_graph);

    // in der Ausgabe steht für jeden Steinerknoten eine Liste mit allen Kanten, deren NCA dieser Steinerknoten ist
    // (mit Ausnahme der Kanten, die zu diesem Knoten inzident sind)
    // es werden nur Kanten betrachtet deren Endpunkte beide im Lösungsgraphen liegen
    Horizontal_Edges_Lists compute_horizontal_edges(const Subgraph &input_subgraph, Graph::NodeId root);

    // fügt alle zum aktuellen Knoten (start_node_id) inzidenten Kanten,
    // deren anderer Endpunkt im Lösungsgraphen, aber nicht in einer der UFS-Mengen der endpoints_to_discard liegt,
    // (d.h. im Algo.: nicht einem der Subbäume, die zu Nachfolgern (des aktuellen Knotens) gehören),
    // zu dem Heap des aktuellen Knotens hinzu
    void add_relevant_edges_to_heap(const Subgraph& input_subgraph,
                                    Graph::NodeId start_node_id,
                                    Ext_Union_Find_Structure& subtrees_ufs,
                                    Edge_Heaps& vert_edge_heaps,
                                    const std::vector<Graph::NodeId>& endpoints_to_discard);

    // ruft add_relevant_edges_to_heap auf und fügt die Heaps der Nachfolger (des aktuellen Knotens)
    // zum Heap des aktuellen Knotens hinzu (merge)
    // vereinigt die ufs-Menge des aktuellen Knotens mit denen seiner Nachfolger
    void update_ufs_and_edge_heaps(const Subgraph& input_subgraph,
                                   Graph::NodeId start_node_id,
                                   const std::vector<Graph::NodeId>& children,
                                   Ext_Union_Find_Structure& subtrees_ufs,
                                   Edge_Heaps& vert_edge_heaps);

    // findet für jeden Nachfolger (des aktuellen Knotens) die beste vertikale Kante
    // und fügt diese zum Supergraphen hinzu
    void add_vertical_edges_to_supergraph(const Graph& original_graph,
                            Graph::NodeId start_node,
                            const std::vector<Graph::NodeId>& children,
                            Ext_Union_Find_Structure &subtrees_ufs,
                            Edge_Heaps& vert_edge_heaps,
                            LocalSearchAux::MovesPerPass moves_per_pass,
                            const std::vector<bool> &forbidden,
                            Supergraph& input_supergraph);

    // fügt die horizontalen Kanten zum Supergraph hinzu
    void add_horizontal_edges_to_supergraph(const Subgraph& input_subgraph,
                              const std::vector<Graph::NodeId>& children,
                              Ext_Union_Find_Structure &subtrees_ufs,
                              const std::vector<Graph::EdgeId>& horiz_edges,
                              LocalSearchAux::MovesPerPass moves_per_pass,
                              const std::vector<bool> &forbidden,
                              Supergraph& input_supergraph);

    // findet die Kanten des zugrundeliegenden Graphen, die den Kanten des MST des Supergraphen entsprechen
    std::vector<Graph::EdgeId> get_edges_to_insert(const Subgraph& instance_subgraph,
                                                   const Supergraph& super_graph,
                                                   const Subgraph& mst_of_supergraph);

    //markiert die Endknoten aller Eingabe-Kanten (edges_to_insert) als pinned
    void mark_endpoints_pinned(const Subgraph& input_subgraph,
                               std::vector<bool>& pinned,
                               std::vector<Graph::EdgeId> edges_to_insert);
}


void SteinerVertexElim::complete_algorithm(Subgraph &input_subgraph) {
    int debug_while_loop_counter = 0;

    while(true) {
        debug_while_loop_counter++;

        //debug
        //GraphAuxPrint::print_subgraph(input_subgraph);
        //fflush(stdout);

        std::vector<ImprovingChangement> improvements = SteinerVertexElim::evaluate_neighborhood(input_subgraph, LocalSearchAux::several_moves);

        if(improvements.empty()) {
            // lokales Optimum erreicht
            break;
        }


        /*//debug
        std::cout << "Schleife " << debug_while_loop_counter << "\n";
        for(auto var_im_ch: improvements) {
            var_im_ch.print(input_subgraph);
        }
        fflush(stdout);*/

        LocalSearchAux::perform_improving_changements(input_subgraph, improvements);

        GraphAux::remove_steinerbranches(input_subgraph);

        //debug
        if( not GraphAux::get_isolated_nodes(input_subgraph.this_graph()).empty() ) {
            throw std::runtime_error("Fehler SVE");
        }
    }
}

std::vector<ImprovingChangement>
SteinerVertexElim::evaluate_neighborhood(Subgraph &input_subgraph, LocalSearchAux::MovesPerPass moves_per_pass) {

    //? const Graph& original_graph = input_subgraph.original_graph();
    Graph& solution_graph = input_subgraph.this_graph();
    const std::vector<Graph::NodeId>& solution_nodeids_of_original_nodes = input_subgraph.subgraph_nodeids_of_nodes_in_originalgraph();
    //? const std::vector<Graph::NodeId>& original_nodeids = input_subgraph.original_nodeids();

    //Initialisieren der Union Find, erstelle Menge für alle Knoten in der aktuellen Lösung
    unsigned int num_nodes_of_solution = solution_graph.num_nodes();
    Ext_Union_Find_Structure subtrees_ufs(num_nodes_of_solution);
    for(unsigned int i=0; i<num_nodes_of_solution; i++) {
        subtrees_ufs.make_set(i);
    }

    Edge_Heaps vert_edge_heaps(solution_nodeids_of_original_nodes, num_nodes_of_solution);

    //richte den Graph mit beliebiger (?) Wurzel, berechne Reihenfolge der Knoten für die Prozessierung
    //? hier kann man root_id frei wählen
    Graph::NodeId root_id = solution_graph.get_vect_term()[0];
    solution_graph.make_rooted_arborescence(root_id);
    std::vector<Graph::NodeId> nodes_in_postorder = SteinerVertexElim::get_nodes_in_postorder(solution_graph, root_id);

    //debug
    /*
    std::cout << "Hier kommt der Subgraph, der gerade gerichtet wurde: \n";
    GraphAuxPrint::print_subgraph(input_subgraph);
    fflush(stdout);*/

    Horizontal_Edges_Lists horiz_edges_lists =
            SteinerVertexElim::compute_horizontal_edges(input_subgraph, root_id);

    // Hilfsstrukturen, um mehrere Verbesserung in einem pass durchführen zu können
    // Knoten, die als forbidden markiert sind, dürfen nicht mehr verwendet werden, um die Subbäume wieder zu verbinden
    std::vector<bool> forbidden(solution_graph.num_nodes(), false);
    // Knoten, die als pinned markiert sind, dürfen nicht mehr entfernt werden
    std::vector<bool> pinned(solution_graph.num_nodes(), false);


    //main loop der Methode
    std::vector<ImprovingChangement> evaluated_neighborhood;
    for(auto curr_node_id: nodes_in_postorder) {
        const ImprovingChangement curr_changement =
                SteinerVertexElim::process_node(input_subgraph, curr_node_id, subtrees_ufs, horiz_edges_lists,
                                                vert_edge_heaps, moves_per_pass, forbidden, pinned);

        if( curr_changement.getImprovementValue() > 0){
            evaluated_neighborhood.push_back(curr_changement);
        }
    }

    return evaluated_neighborhood;

}


ImprovingChangement SteinerVertexElim::process_node(const Subgraph& input_subgraph,
                                                    Graph::NodeId start_node_id,
                                                    Ext_Union_Find_Structure& subtrees_ufs,
                                                    const Horizontal_Edges_Lists& horiz_edges_lists,
                                                    Edge_Heaps& vert_edge_heaps,
                                                    LocalSearchAux::MovesPerPass moves_per_pass,
                                                    std::vector<bool>& forbidden,
                                                    std::vector<bool>& pinned) {

    const Graph& original_graph = input_subgraph.original_graph();
    const Graph& solution_graph = input_subgraph.this_graph();
    //? const std::vector<Graph::NodeId>& solution_nodeids_of_original_nodes = input_subgraph.subgraph_nodeids_of_nodes_in_originalgraph();
    //? const std::vector<Graph::NodeId>& original_nodeids = input_subgraph.original_nodeids();

    const std::vector<Graph::NodeId> children = solution_graph.get_outgoing_neighbors(start_node_id);

    //falls der aktuelle Knoten ein Terminal ist oder pinned ist, wollen wir lediglich die Strukturen aktualisieren
    // die Ausgabe ist dann leer
    if( solution_graph.check_if_terminal(start_node_id) || pinned[start_node_id] ) {
        update_ufs_and_edge_heaps(input_subgraph, start_node_id, children, subtrees_ufs, vert_edge_heaps);

        return ImprovingChangement(std::vector<Graph::EdgeId>(), std::vector<Graph::EdgeId>(), 0);
    }

    //falls der aktuelle Knoten kein Terminal ist, aber keine Nachfolger besitzt, ist er ein Steinerblatt
    if( children.empty() ) {
        throw std::runtime_error("(SteinerVertexElim::process_node) Der Eingabeknoten ist ein Steinerblatt.");
    }

    Supergraph super_graph(children.size() +1);

    SteinerVertexElim::add_vertical_edges_to_supergraph(original_graph, start_node_id, children, subtrees_ufs,
                                                        vert_edge_heaps, moves_per_pass, forbidden, super_graph);

    SteinerVertexElim::add_horizontal_edges_to_supergraph(input_subgraph, children, subtrees_ufs,
                                                          horiz_edges_lists.get_list(start_node_id),
                                                          moves_per_pass, forbidden, super_graph);

    //falls der Supergraph nicht zusammenhängend ist, können wir für den aktuellen Knoten keine Nachbarschaftslösung finden,
    // wir aktualisieren also nun nur noch die Strukturen, die Ausgabe ist dementsprechend leer
    if( not GraphAux::check_if_connected(super_graph.this_graph()) ) {
        update_ufs_and_edge_heaps(input_subgraph, start_node_id, children, subtrees_ufs, vert_edge_heaps);

        return ImprovingChangement(std::vector<Graph::EdgeId>(), std::vector<Graph::EdgeId>(), 0);
    }

    // der Supergraph als Subgraph von sich selbst
    const Subgraph supergraph_as_subgraph(super_graph.this_graph());

    const Subgraph mst_of_supergraph = GraphAlgorithms::mst_prim_for_subgraphs(supergraph_as_subgraph, 0);

    // Wert der Verbesserung berechnen
    Graph::PathLength weight_incident_edges = 0;
    const std::vector<Graph::EdgeId> incident_edges = solution_graph.incident_edge_ids(start_node_id);
    for( auto curr_edge_id: incident_edges){
        const Graph::Edge& curr_edge = solution_graph.get_edge(curr_edge_id);
        weight_incident_edges += curr_edge.weight();
    }
    Graph::PathLength weight_mst_of_supergraph = GraphAux::length_of_all_edges( mst_of_supergraph.this_graph() );
    const Graph::PathLength improve_value = weight_incident_edges - weight_mst_of_supergraph;

    //debug
    /*
    if( improve_value < 0) {
        std::cout << "(SteinerVertexElim::process_node) Die Subroutine hat eine schlechtere Lösung als die aktuelle berechnet. \n";
        //throw std::runtime_error("(SteinerVertexElim::process_node) Die Subroutine hat eine schlechtere Lösung als die aktuelle berechnet");
    }*/

    //falls wir keine Verbesserung finden, wollen wir nun nur noch die Strukturen aktualisieren
    // die Ausgabe ist dann leer
    if( improve_value <= 0) {
        update_ufs_and_edge_heaps(input_subgraph, start_node_id, children, subtrees_ufs, vert_edge_heaps);

        return ImprovingChangement(std::vector<Graph::EdgeId>(), std::vector<Graph::EdgeId>(), 0);
    }

    const std::vector<Graph::EdgeId> edges_to_insert = SteinerVertexElim::get_edges_to_insert(input_subgraph, super_graph, mst_of_supergraph);

    if( moves_per_pass == LocalSearchAux::MovesPerPass::several_moves) {
        forbidden[start_node_id] = true;
        LocalSearchAux::update_forbidden(solution_graph, forbidden, start_node_id);

        SteinerVertexElim::mark_endpoints_pinned(input_subgraph, pinned, edges_to_insert);
    }

    update_ufs_and_edge_heaps(input_subgraph, start_node_id, children, subtrees_ufs, vert_edge_heaps);

    return ImprovingChangement(incident_edges, edges_to_insert, improve_value);
}



std::vector<Graph::NodeId> SteinerVertexElim::get_nodes_in_postorder(const Graph &input_graph, Graph::NodeId root_id) {

    // Checks weglassen ?
    if( root_id == Graph::invalid_node_id) {
        throw std::runtime_error("(SteinerVertexElim::get_nodes_in_postorder) Eingabeknoten ungueltig.");
    }
    if( root_id > input_graph.num_nodes()) {
        throw std::runtime_error("(SteinerVertexElim::get_nodes_in_postorder) Eingabeknoten nicht im Graphen.");
    }

    if( not input_graph.check_if_terminal(root_id) ) {
        throw std::runtime_error("(SteinerVertexElim::get_nodes_in_postorder) Eingabeknoten ist kein Terminal.");
    }

    std::vector<Graph::NodeId> output;
    output.reserve(input_graph.num_nodes() -1);

    //Graphendurchmusterung

    std::vector<Graph::NodeId> next_nodes (1, root_id);

    //speichert die bereits erreichten Knoten (die, die bereits zu next_nodes hinzugfügt wurden)
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

                output.push_back( curr_neighbor_id );
            }
        }
    }

    //kehre die Reihenfolge um
    std::reverse(output.begin(), output.end());

    return output;
}

std::vector<Graph::NodeId> SteinerVertexElim::get_steiner_nodes(const Graph& input_graph) {
    std::vector<Graph::NodeId> steiner_nodes;
    steiner_nodes.reserve(input_graph.num_nodes());

    for(auto curr_node: input_graph.nodes()) {
        if( not curr_node.check_if_terminal() ) {
            steiner_nodes.push_back(curr_node.node_id());
        }
    }

    steiner_nodes.shrink_to_fit();

    return steiner_nodes;
}


Horizontal_Edges_Lists SteinerVertexElim::compute_horizontal_edges(const Subgraph &input_subgraph, Graph::NodeId root) {

    const Graph& original_graph = input_subgraph.original_graph();
    const Graph& solution_graph = input_subgraph.this_graph();
    const std::vector<Graph::NodeId>& solution_nodeids_of_original_nodes = input_subgraph.subgraph_nodeids_of_nodes_in_originalgraph();

    std::vector<Graph::NodeId> steiner_nodes = get_steiner_nodes(solution_graph);

    std::vector<Horizontal_Edges_Lists::ListId> list_ids =
            LocalSearchAux::compute_list_ids_for_horizon_edges_lists(solution_graph.num_nodes(), steiner_nodes);

    std::vector<std::vector<Graph::EdgeId> > vect_of_lists(steiner_nodes.size(), std::vector<Graph::EdgeId>());

    NCA_Tree_Data_Structure tree_structure(solution_graph, root);

    for( auto curr_edge: original_graph.edges()) {

        Graph::EdgeId curr_edge_id = curr_edge.edge_id();

        // finde die Endpunkte der aktuellen Kante als NodeIds in der aktuellen Lösung
        Graph::NodeId sol_node_id_a = solution_nodeids_of_original_nodes[curr_edge.node_a()];
        Graph::NodeId sol_node_id_b = solution_nodeids_of_original_nodes[curr_edge.node_b()];

        if( sol_node_id_a == Graph::invalid_node_id || sol_node_id_b == Graph::invalid_node_id ) {
            //in dem Fall verläuft die aktuelle Kante nicht zwischen Knoten des Lösungsgraphen
            continue;
        }

        //finde den NCA der Kante und füge ihn zur entsprechenden Liste hinzu

        Graph::NodeId nearest_common_ancestor = tree_structure.nearest_common_ancestor(sol_node_id_a, sol_node_id_b);

        //falls der NCA der Kante einer der Endpunkte selbst ist, ist die Kante keine horizontale Kante
        // (siehe NCA_Tree_Data_Structure::nearest_common_ancestor)
        if( nearest_common_ancestor != Graph::invalid_node_id) {

            // falls der NCA ein Knoten ist, für den wir eine Liste anlegen, fügen wir ihn zu dieser hinzu
            Horizontal_Edges_Lists::ListId curr_list_id = list_ids[nearest_common_ancestor];
            if( curr_list_id != Horizontal_Edges_Lists::no_list_available) {
                vect_of_lists[curr_list_id].push_back(curr_edge_id);
            }
        }
    }

    return Horizontal_Edges_Lists(list_ids, vect_of_lists);
}

void SteinerVertexElim::add_relevant_edges_to_heap(const Subgraph& input_subgraph,
                                                   Graph::NodeId start_node_id,
                                                   Ext_Union_Find_Structure& subtrees_ufs,
                                                   Edge_Heaps& vert_edge_heaps,
                                                   const std::vector<Graph::NodeId>& endpoints_to_discard) {

    // markiere die Knoten, die wir nicht als Endpunkte von Kanten zulassen wollen
    // (d.h. solche wollen wir nicht zum Heap hinzufügen)
    subtrees_ufs.set_allowed_multiple_elts(endpoints_to_discard, false);

    std::priority_queue<std::pair<Graph::PathLength, Graph::EdgeId>,
            std::vector<std::pair<Graph::PathLength, Graph::EdgeId>>,
            std::greater<std::pair<Graph::PathLength, Graph::EdgeId>>>& heap_to_process = vert_edge_heaps.get_heap(start_node_id);

    const std::vector<Graph::NodeId> solution_node_ids = input_subgraph.subgraph_nodeids_of_nodes_in_originalgraph();
    const Graph& original_graph = input_subgraph.original_graph();

    Graph::NodeId start_node_original_id = input_subgraph.original_nodeids()[start_node_id];
    std::vector<Graph::EdgeId> incident_edges_in_orig_graph = original_graph.get_node(start_node_original_id).incident_edge_ids();

    //Schleife über alle Kanten, die mit dem aktuellen Knoten im zugrundeliegenden Graphen inzident sind
    for( auto curr_edge_id: incident_edges_in_orig_graph) {

        const Graph::Edge& curr_edge = original_graph.get_edge(curr_edge_id);

        // finde den zur Kante gehörenden Nachbarn (als NodeId im Lösungsgraphen)
        Graph::NodeId curr_neighbor_orig_id = curr_edge.get_other_node(start_node_original_id);
        Graph::NodeId curr_neighbor_sol_id = solution_node_ids[curr_neighbor_orig_id];

        if( curr_neighbor_sol_id == Graph::invalid_node_id) {
            //in dem Fall liegt der Nachbar nicht im Lösungsgraphen
            continue;
        }

        if( subtrees_ufs.allowed(curr_neighbor_sol_id) ) {
            heap_to_process.push({curr_edge.weight(), curr_edge_id});
        }
    }

    // mache die Markierungen rückgängig
    subtrees_ufs.set_allowed_multiple_elts(endpoints_to_discard, true);
}

void SteinerVertexElim::update_ufs_and_edge_heaps(const Subgraph& input_subgraph,
                                                  Graph::NodeId start_node_id,
                                                  const std::vector<Graph::NodeId>& children,
                                                  Ext_Union_Find_Structure& subtrees_ufs,
                                                  Edge_Heaps& vert_edge_heaps) {

    vert_edge_heaps.merge(start_node_id, children);

    SteinerVertexElim::add_relevant_edges_to_heap(input_subgraph, start_node_id, subtrees_ufs, vert_edge_heaps, children);

    std::vector<Union_Find_Structure::ElementId> elements_to_union = children;
    elements_to_union.push_back(start_node_id);
    subtrees_ufs.union_multiple_sets(elements_to_union);
}

void SteinerVertexElim::add_vertical_edges_to_supergraph(const Graph& original_graph,
                                           Graph::NodeId start_node,
                                           const std::vector<Graph::NodeId>& children,
                                           Ext_Union_Find_Structure &subtrees_ufs,
                                           Edge_Heaps& vert_edge_heaps,
                                           LocalSearchAux::MovesPerPass moves_per_pass,
                                           const std::vector<bool> &forbidden,
                                           Supergraph& input_supergraph) {

    std::vector<std::pair<Graph::PathLength, Graph::EdgeId>> vertical_edges =
            vert_edge_heaps.cleanup_heaps_sve(start_node, children, original_graph, subtrees_ufs, moves_per_pass, forbidden);

    // vertikale Kanten zum Supergraphen hinzufügen
    for(unsigned int i=0; i<vertical_edges.size(); i++) {
        std::pair<Graph::PathLength, Graph::EdgeId> curr_edge = vertical_edges[i];

        //prüfe, ob wir für den i+1-ten Nachfolger eine vertikale Kante gefunden haben
        if( curr_edge.first < Graph::infinite_length) {

            //die Kante verläuft zwischen dem Subbaum, der zum (i+1)-ten Nachfolger gehört, und dem Subbaum des Vorgängers
            input_supergraph.add_edge(0, i+1, curr_edge.first, curr_edge.second);
        }
    }

}

void SteinerVertexElim::add_horizontal_edges_to_supergraph(const Subgraph& input_subgraph,
                                             const std::vector<Graph::NodeId>& children,
                                             Ext_Union_Find_Structure &subtrees_ufs,
                                             const std::vector<Graph::EdgeId>& horiz_edges,
                                             LocalSearchAux::MovesPerPass moves_per_pass,
                                             const std::vector<bool> &forbidden,
                                             Supergraph& input_supergraph) {

    // setze die super_id-Werte der Union-Find so, dass die Mengen, die den crucial children entsprechen, durchnummeriert werden (beginnend bei 1)
    Graph::NodeId new_id = 1;
    for( auto curr_child: children) {
        subtrees_ufs.set_superid(curr_child, new_id);
        new_id++;
    }

    const Graph& original_graph = input_subgraph.original_graph();
    const std::vector<Graph::NodeId>& solution_nodeids_of_original_nodes = input_subgraph.subgraph_nodeids_of_nodes_in_originalgraph();

    for( auto curr_edge_id: horiz_edges) {

        const Graph::Edge& curr_edge = original_graph.get_edge(curr_edge_id);

        // finde die Endpunkte der aktuellen Kante als NodeIds in der aktuellen Lösung
        Graph::NodeId sol_node_id_a = solution_nodeids_of_original_nodes[curr_edge.node_a()];
        Graph::NodeId sol_node_id_b = solution_nodeids_of_original_nodes[curr_edge.node_b()];


        if( moves_per_pass == LocalSearchAux::several_moves) {
            if( forbidden[sol_node_id_a] || forbidden[sol_node_id_b]) {
                continue;
            }
        }

        Graph::NodeId supernode_id_a = subtrees_ufs.get_superid(sol_node_id_a);
        Graph::NodeId supernode_id_b = subtrees_ufs.get_superid(sol_node_id_b);

        //debug
        if( supernode_id_a == 0 || supernode_id_b == 0) {
            throw std::runtime_error("(SteinerVertexElim::add_horizontal_edges_to_supergraph) keine horizontale Kante");
        }
        //debug
        if( supernode_id_a == supernode_id_b) {
            throw std::runtime_error("(SteinerVertexElim::add_horizontal_edges_to_supergraph) keine horizontale Kante");
        }

        input_supergraph.add_edge(supernode_id_a, supernode_id_b,
                             curr_edge.weight(),
                             curr_edge_id);
    }

    //reset der super_ids
    subtrees_ufs.reset_superids(children);
}

std::vector<Graph::EdgeId> SteinerVertexElim::get_edges_to_insert(const Subgraph& instance_subgraph,
                                               const Supergraph& super_graph,
                                               const Subgraph& mst_of_supergraph) {
    //Ausgabe
    std::vector<Graph::EdgeId> edges_to_insert;

    // zugrundeliegender Graph (zugrundeliegend bzgl. der aktuellen Lösung)
    const Graph& instance_original_graph = instance_subgraph.original_graph();

    // speichert die Kanten, die bereits zum Ausgabe-Vektor hinzugefügt wurden
    std::vector<bool> added_edges(instance_original_graph.num_edges(), false);

    //Schleife über alle Kanten des MST (Subgraph des Supergraphen)
    for(auto curr_edge: mst_of_supergraph.this_graph().edges()) {

        //finde die edge id im supergraph
        Graph::EdgeId curr_edge_id_in_supergraph = mst_of_supergraph.original_edgeids()[ curr_edge.edge_id() ];

        //finde die edge id im zugrundeliegenden Graph (zugrundeliegend bzgl. der aktuellen Lösung)
        Graph::EdgeId curr_original_edge_id = super_graph.original_edge_ids()[ curr_edge_id_in_supergraph ];

        // füge diese Kante zur Ausgabe hinzu
        edges_to_insert.push_back(curr_original_edge_id);
    }

    return edges_to_insert;

}

void SteinerVertexElim::mark_endpoints_pinned(const Subgraph& input_subgraph,
                           std::vector<bool>& pinned,
                           std::vector<Graph::EdgeId> edges_to_insert) {

    const Graph& original_graph = input_subgraph.original_graph();
    const std::vector<Graph::NodeId> solution_node_ids = input_subgraph.subgraph_nodeids_of_nodes_in_originalgraph();

    for( auto curr_edge_id: edges_to_insert) {
        const Graph::Edge& curr_edge = original_graph.get_edge(curr_edge_id);

        //finde die Endpunkte der aktuellen Kante als NodeIds im Lösungsgraphen
        Graph::NodeId sol_node_id_a = solution_node_ids[curr_edge.node_a()];
        Graph::NodeId sol_node_id_b = solution_node_ids[curr_edge.node_b()];

        pinned[sol_node_id_a] = true;
        pinned[sol_node_id_b] = true;
    }
}
