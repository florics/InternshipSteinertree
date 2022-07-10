//
// Created by Lukas on 10.07.2022.
//

#include "local_search_combinations.h"

#include "iostream"

#include "Union_Find_Structure.h"
#include "Ext_Union_Find_Structure.h"
#include "voronoi_diagram.h"
#include "Edge_Heaps.h"
#include "local_search_aux.h"
#include "key_vertex_elimination.h"
#include "key_path_exchange.h"
#include "Horizontal_Edges_Lists.h"
#include "graph_aux_functions.h"
#include "steiner_vertex_insertion.h"
#include "graph_printfunctions.h"


void LocalSearchCombi::find_local_minimum_with_svi_and_kvekpe_in_sequence_each_pass(Subgraph &input_subgraph) {

    bool kvekpe_local_min_of_reached = false;
    bool svi_local_min_of_reached = false;

    //Schleife bricht ab, wenn wir ein lokales Optimum erreicht haben
    while( true ) {

        svi_local_min_of_reached =
                not SteinerVertexInsertion::run_one_pass_and_perform_changements_on_graph(input_subgraph);


        if( kvekpe_local_min_of_reached && svi_local_min_of_reached ) {
            break;
        }


        std::vector<ImprovingChangement> kvekpe_improvements =
                LocalSearchCombi::evaluate_neighborhood_with_kve_kpe_in_sequence_for_each_node(input_subgraph);

        LocalSearchAux::perform_improving_changements(input_subgraph, kvekpe_improvements);

        kvekpe_local_min_of_reached = kvekpe_improvements.empty();


        if( kvekpe_local_min_of_reached && svi_local_min_of_reached ) {
            break;
        }

    }
}

std::vector<ImprovingChangement>
LocalSearchCombi::evaluate_neighborhood_with_kve_kpe_in_sequence_for_each_node(Subgraph &input_subgraph) {

    const Graph& original_graph = input_subgraph.original_graph();
    Graph& solution_graph = input_subgraph.this_graph();
    const std::vector<Graph::NodeId>& solution_nodeids_of_original_nodes = input_subgraph.subgraph_nodeids_of_nodes_in_originalgraph();
    const std::vector<Graph::NodeId>& original_nodeids = input_subgraph.original_nodeids();

    //Präprozessierung

    //Initialisieren der Union Find, erstelle Menge für alle Knoten in der aktuellen Lösung
    const unsigned int num_nodes_of_solution = solution_graph.num_nodes();
    Ext_Union_Find_Structure subtrees_ufs(num_nodes_of_solution);
    for(unsigned int i=0; i<num_nodes_of_solution; i++) {
        subtrees_ufs.make_set(i);
    }

    Voronoi_diagram vor_diag(original_nodeids, original_graph);

    Edge_Heaps vert_bound_edge_heaps(vor_diag, solution_nodeids_of_original_nodes);

    //richte den Graph mit pseudo-zufällig gewähltem Terminal als Wurzel,
    // berechne Reihenfolge der Knoten für die Prozessierung

    const std::vector<Graph::NodeId>& terminals = solution_graph.get_terminals();
    const Graph::NodeId root_id = terminals[ rand() % terminals.size()];

    solution_graph.make_rooted_arborescence(root_id);

    const std::vector<Graph::NodeId> crucialnodes_in_postorder = LocalSearchAux::get_crucialnodes_in_postorder(solution_graph, root_id);


    const Horizontal_Edges_Lists horiz_bound_edges_lists =
            KeyVertexElim::compute_horizontal_bound_edges(input_subgraph, root_id, vor_diag);


    // Hilfsstrukturen, um mehrere Verbesserungen in einem pass durchführen zu können
    // Knoten, die als forbidden markiert sind, dürfen nicht mehr verwendet werden, um die Subbäume wieder zu verbinden
    std::vector<bool> forbidden(solution_graph.num_nodes(), false);
    // Knoten, die als pinned markiert sind, dürfen nicht mehr entfernt werden
    std::vector<bool> pinned(solution_graph.num_nodes(), false);

    //main loop
    std::vector<ImprovingChangement> evaluated_neighborhood;
    for(auto curr_node_id: crucialnodes_in_postorder) {

        // das Bearbeiten des eingehenden KeyPath erfolgt sowohl in KVE als auch in KPE,
        // wir können es also hier einmal vorweg machen
        bool one_ingoing_intern_node_is_pinned = false;
        std::vector<Graph::NodeId> ingoing_internal_nodes;
        EdgeSequence ingoing_keypath =
                LocalSearchAux::find_and_process_ingoing_keypath(solution_graph, curr_node_id,
                                                                 ingoing_internal_nodes, subtrees_ufs,
                                                                 pinned, one_ingoing_intern_node_is_pinned);

        //versuche zunächst, ob Entfernen des aktuellen Knotens eine echte Verbesserung bringt
        const ImprovingChangement k_vertex_exch_changement =
                KeyVertexElim::process_node(input_subgraph, curr_node_id, vor_diag, subtrees_ufs,
                                            horiz_bound_edges_lists, vert_bound_edge_heaps,
                                            LocalSearchAux::MovesPerPass::several_moves, forbidden, pinned,
                                            LocalSearchAux::IngoingKeyPathProcessState::already_processed,
                                            ingoing_keypath, ingoing_internal_nodes, one_ingoing_intern_node_is_pinned);


        //falls wir den aktuellen Knoten entfernen, gehen wir zum nächsten Knoten
        if( k_vertex_exch_changement.getImprovementValue() > 0){
            evaluated_neighborhood.push_back(k_vertex_exch_changement);
            continue;
        }

        //falls wir den aktuellen Knoten nicht entfernen, versuchen wir seinen eingehenden KeyPath zu entfernen
        // da wir bereits KVE aufgerufen haben, brauchen wir die Heaps und die Union-Find nicht zu aktualisieren
        const ImprovingChangement k_path_el_changement =
                KeyPathExch::process_node(curr_node_id, input_subgraph, vor_diag, subtrees_ufs, vert_bound_edge_heaps,
                                          LocalSearchAux::MovesPerPass::several_moves, forbidden, pinned,
                                          LocalSearchAux::IngoingKeyPathProcessState::already_processed, ingoing_keypath,
                                          ingoing_internal_nodes, one_ingoing_intern_node_is_pinned,
                                          KeyPathExch::Need_for_Updates_of_Heaps_and_UFS::no_need);

        if( k_path_el_changement.getImprovementValue() > 0){
            evaluated_neighborhood.push_back(k_path_el_changement);
        }

    }

    return evaluated_neighborhood;
}

