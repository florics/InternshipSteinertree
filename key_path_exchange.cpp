//
// Created by Lukas on 22.06.2022.
//

#include "key_path_exchange.h"

#include "iostream"
#include "algorithm"

#include "vor_diag_aux_functions.h"
#include "EdgeSequence.h"
#include "graph_aux_functions.h"
#include "graph_printfunctions.h"
#include "general_aux_functions.h"


std::vector<ImprovingChangement> KeyPathExch::evaluate_neighborhood(Subgraph& input_subgraph, LocalSearchAux::MovesPerPass moves_per_pass) {
    /*
    //ist das debug?
    if( original_graph.num_nodes() != subgraph_vectors.first.size() ) {
        throw std::runtime_error("(KeyPathExch::key_path_exchange) Anzahl Knoten in original_graph ungleich Anzahl Einträge in subgraph_vectors.");
    }
    if( original_graph.num_edges() != subgraph_vectors.second.size() ) {
        throw std::runtime_error("(KeyPathExch::key_path_exchange) Anzahl Kanten in original_graph ungleich Anzahl Einträge in subgraph_vectors.");
    }
     */

    const Graph& original_graph = input_subgraph.original_graph();
    Graph& solution_graph = input_subgraph.this_graph();
    const std::vector<Graph::NodeId>& solution_nodeids_of_original_nodes = input_subgraph.subgraph_nodeids_of_nodes_in_originalgraph();
    const std::vector<Graph::NodeId>& original_nodeids = input_subgraph.original_nodeids();

    /*
    // die Knotenmenge der aktuellen Lösung als NodeIds des zugrundeliegenden Graphen (ist aber keine Übersetzung der NodeIds)
     //? sollte ich nicht mehr brauchen wegen original_nodeids
    std::vector<Graph::NodeId> solution_nodes_as_original_nodeids;
    for(unsigned  int i = 0; i<original_graph.num_nodes(); i++) {
        if( solution_nodeids_of_original_nodes[i] != Graph::invalid_node_id ) {
            solution_nodes_as_original_nodeids.push_back(i);
        }
    }
     */

    /*
    //ist das debug?
    if( solution_graph.num_nodes() != solution_nodes_as_original_nodeids.size() ) {
        throw std::runtime_error("(KeyPathExch::key_path_exchange) Anzahl Knoten im Subgraph ungleich Anzahl Einträge in subgraph_vectors.");
    }
     */

    //hier beginnt die eigentliche Präprozessierung

    //Initialisieren der Union Find, erstelle Menge für alle Knoten in der aktuellen Lösung
    const unsigned int num_nodes_of_solution = solution_graph.num_nodes();
    Union_Find_Structure subtrees_ufs(num_nodes_of_solution);
    for(unsigned int i=0; i<num_nodes_of_solution; i++) {
        subtrees_ufs.make_set(i);
    }

    Voronoi_diagram vor_diag(original_nodeids, original_graph);

    Edge_Heaps bound_edge_heaps(vor_diag, solution_nodeids_of_original_nodes);

    //richte den Graph mit beliebiger (?) Wurzel, berechne Reihenfolge der Knoten für die Prozessierung
    //? hier kann man root_id frei wählen
    const Graph::NodeId root_id = solution_graph.get_vect_term()[0];
    solution_graph.make_rooted_arborescence(root_id);
    std::vector<Graph::NodeId> crucialnodes_in_postorder = LocalSearchAux::get_crucialnodes_in_postorder(solution_graph, root_id);

    // Hilfsstrukturen, um mehrere Verbesserung in einem pass durchführen zu können
    // Knoten, die als forbidden markiert sind, dürfen nicht mehr verwendet werden, um die Subbäume wieder zu verbinden
    std::vector<bool> forbidden(solution_graph.num_nodes(), false);
    // Knoten, die als pinned markiert sind, dürfen nicht mehr entfernt werden
    std::vector<bool> pinned(solution_graph.num_nodes(), false);

    //main loop der Methode
    std::vector<ImprovingChangement> evaluated_neighborhood;
    for(auto curr_node_id: crucialnodes_in_postorder) {
        const ImprovingChangement curr_changement =
                KeyPathExch::process_node(curr_node_id, input_subgraph, vor_diag, subtrees_ufs, bound_edge_heaps, moves_per_pass, forbidden, pinned);
        if( curr_changement.getImprovementValue() > 0){
            evaluated_neighborhood.push_back(curr_changement);
        }
    }

    return evaluated_neighborhood;
}

ImprovingChangement KeyPathExch::process_node(Graph::NodeId input_node_id,
                                              const Subgraph& input_subgraph,
                                              Voronoi_diagram& vor_diag,
                                              Union_Find_Structure& subtrees_ufs,
                                              Edge_Heaps& bound_edge_heaps,
                                              LocalSearchAux::MovesPerPass moves_per_pass,
                                              std::vector<bool>& forbidden,
                                              std::vector<bool>& pinned) {

    //const Graph& original_graph = input_subgraph.original_graph();
    const Graph& solution_graph = input_subgraph.this_graph();
    const std::vector<Graph::NodeId>& solution_nodeids_of_original_nodes = input_subgraph.subgraph_nodeids_of_nodes_in_originalgraph();
    const std::vector<Graph::NodeId>& original_nodeids = input_subgraph.original_nodeids();

    // finde den keypath der im Eingabeknoten endet, sowie dessen Anfangsknoten & Länge
    std::vector<Graph::NodeId> internal_node_ids;
    const EdgeSequence key_path = LocalSearchAux::find_ingoing_keypath(solution_graph, input_node_id, internal_node_ids);
    const Graph::NodeId crucial_parent_id = key_path.endnode_a();

    //debug
    if( input_node_id == crucial_parent_id ) {
        throw std::runtime_error("key path exchange main loop");
    }
    if( input_node_id != key_path.endnode_b() ) {
        throw std::runtime_error("key path exchange main loop");
    }

    //füge die internen Knoten des keypath zu einer Menge in der Union Find zusammen
    subtrees_ufs.union_multiple_sets(internal_node_ids);
    // der Schlüssel des subtree der internen Knoten in der Union-Find
    //? Fehleranfälligkeit: der könnte sich ggf. verändern
    Union_Find_Structure::ElementId internal_nodes_ufsroot = Union_Find_Structure::invalid_elt_id;
    if(not internal_node_ids.empty()) {
        internal_nodes_ufsroot = subtrees_ufs.find(internal_node_ids.front());
    }

    // prüfe, ob die internen Knoten pinned sind
    // todo: trotzdem weiter machen und später prüfen... (ist das sinnvoll? Laufzeit...)
    if( moves_per_pass == LocalSearchAux::several_moves) {
        for(auto intern_node_id: internal_node_ids) {
            if( pinned[intern_node_id]) {
                KeyPathExch::update_heaps_and_ufs(bound_edge_heaps, subtrees_ufs, input_node_id, crucial_parent_id,
                                                  internal_node_ids, internal_nodes_ufsroot);

                return ImprovingChangement(std::vector<Graph::EdgeId>(), std::vector<Graph::EdgeId>(), 0);
            }
        }
    }

    //finde die beste original boundary edge
    const std::pair<Graph::PathLength, Graph::EdgeId>
            best_original_bound_edge = bound_edge_heaps.cleanup_one_heap_kpe(input_node_id, vor_diag, subtrees_ufs, internal_nodes_ufsroot, moves_per_pass, forbidden);


    // berechne die NodeIds im zugrundeliegenden Graphen von den internen Knoten des Key-Path
    std::vector<Graph::NodeId> internal_nodes_as_original_nodeids;
    for( auto var_intern_node: internal_node_ids) {
        Graph::NodeId var_or_node_id = original_nodeids[var_intern_node];
        internal_nodes_as_original_nodeids.push_back( var_or_node_id );
    }

    // rufe repair auf und speichere die Daten, die später für restore notwendig
    const Voronoi_diagram::RestoreData vor_diag_restore_data = vor_diag.repair(internal_nodes_as_original_nodeids);

    const std::pair<Graph::PathLength, Graph::EdgeId> best_new_bound_edge =
            compute_best_new_boundedge(vor_diag, subtrees_ufs, solution_nodeids_of_original_nodes, input_node_id,
                                       vor_diag_restore_data.node_ids, moves_per_pass, forbidden);


    // finde die beste boundary edge von den beiden gefundenen, konstruiere Ausgabe
    //todo: schöner machen, Code-Dopplung vermeiden

    std::pair<Graph::PathLength, Graph::EdgeId> bound_edge_to_insert = best_original_bound_edge;
    std::vector<Graph::EdgeId> edges_to_insert;
    Graph::PathLength improve_value = key_path.length() - bound_edge_to_insert.first;

    if( best_new_bound_edge.first < best_original_bound_edge.first ) {
        bound_edge_to_insert = best_new_bound_edge;
        edges_to_insert = VorDiagAux::compute_bound_path_as_vect(vor_diag, bound_edge_to_insert.second);
        improve_value = key_path.length() - bound_edge_to_insert.first;

        //aktualisieren von pinned und forbidden
        if( moves_per_pass == LocalSearchAux::several_moves) {
            if(improve_value > 0){
                LocalSearchAux::update_forbidden(solution_graph, forbidden, crucial_parent_id);
                LocalSearchAux::update_pinned_for_bound_egde(vor_diag, solution_nodeids_of_original_nodes, pinned, bound_edge_to_insert.second);
            }
        }

        // in dem Fall wollen wir das Diagramm NACH den Berechnungen restoren
        vor_diag.restore(vor_diag_restore_data);
    } else {
        // in dem Fall wollen wir das Diagramm VOR den Berechnungen restoren
        vor_diag.restore(vor_diag_restore_data);

        edges_to_insert = VorDiagAux::compute_bound_path_as_vect(vor_diag, bound_edge_to_insert.second);

        //aktualisieren von pinned und forbidden
        if( moves_per_pass == LocalSearchAux::several_moves) {
            if(improve_value > 0){
                LocalSearchAux::update_forbidden(solution_graph, forbidden, crucial_parent_id);
                LocalSearchAux::update_pinned_for_bound_egde(vor_diag, solution_nodeids_of_original_nodes, pinned, bound_edge_to_insert.second);
            }
        }
    }


    //Updates damit die Invarianten des Algo. erhalten bleiben
    KeyPathExch::update_heaps_and_ufs(bound_edge_heaps, subtrees_ufs, input_node_id, crucial_parent_id, internal_node_ids, internal_nodes_ufsroot);

    //beachte, dass die edgesToRemove Kanten im solution-Graph sind und die edgesToInsert Kanten im or-Graph
    // (entsprechend beziehen sich die EdgeIds auf verschiedene Graphen)
    ImprovingChangement output(key_path.edge_ids(), edges_to_insert, improve_value);

    return output;
}

std::pair<Graph::PathLength, Graph::EdgeId>
KeyPathExch::compute_best_new_boundedge(const Voronoi_diagram& vor_diag,
                                        Union_Find_Structure& subtrees_ufs,
                                        const std::vector<Graph::NodeId>& solution_nodeids_of_original_nodes,
                                        Graph::NodeId input_node_id,
                                        const std::vector<Graph::NodeId>& nodes_updated_in_repair,
                                        LocalSearchAux::MovesPerPass moves_per_pass,
                                        const std::vector<bool>& forbidden) {


    //finde alle neuen boundary paths (die nach Entfernen der internen Knoten des keypath auftreten)
    const std::vector<Graph::EdgeId> new_bound_edge_ids = VorDiagAux::get_bound_edges_inc_to_nodeset(vor_diag, nodes_updated_in_repair);

    // der Schlüssel des subtree des aktuellen Knoten in der Union-Find (im Paper: S_v)
    //? Fehleranfälligkeit: der könnte sich ggf. verändern
    const Union_Find_Structure::ElementId input_node_ufsroot = subtrees_ufs.find(input_node_id);

    //finde den besten neuen boundary path, der zwischen den subtrees von curr_node und crucial_parent verläuft
    std::pair<Graph::PathLength, Graph::EdgeId> best_new_bound_edge = {Graph::infinite_length, Graph::invalid_edge_id};
    for(auto curr_bound_edge_id: new_bound_edge_ids) {

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

        // finde die Wurzeln bzgl. der Union Find der Endpunkte bzw. -basen der boundary paths
        const Union_Find_Structure::ElementId var_base_a_ufsroot = subtrees_ufs.find(var_base_a);
        const Union_Find_Structure::ElementId var_base_b_ufsroot = subtrees_ufs.find(var_base_b);

        /*//debug
        if( var_base_a_ufsroot == input_node_ufsroot && var_base_b_ufsroot == input_node_ufsroot ){
            std::cout << "(KeyPathExch::compute_best_new_boundedge) Der Fall kann eintreten \n";
        }*/

        //prüfe, ob der bound path zwischen dem subtree des Eingabeknotens und dem des Vorgängers des Eingabeknotens verläuft
        // (bemerke, dass er keinen Endknoten haben kann, der zu den internen Knoten des keypath gehört,
        // da er ein boundary path bzgl des repaired Vor-D. ist)
        // effektiv prüfen wir, ob der bound path genau einen Endpunkt in dem subtree des Eingabeknotens hat
        if(( var_base_a_ufsroot == input_node_ufsroot && var_base_b_ufsroot != input_node_ufsroot )
           || ( var_base_a_ufsroot != input_node_ufsroot && var_base_b_ufsroot == input_node_ufsroot )) {

            //prüfe, ob die Kante zu einem besseren boundary path gehört als der aktuell beste
            Graph::PathLength curr_length = VorDiagAux::compute_length_of_boundpath(vor_diag, curr_bound_edge_id);
            if( curr_length < best_new_bound_edge.first) {
                best_new_bound_edge = {curr_length, curr_bound_edge_id};
            }
        }
    }

    return best_new_bound_edge;
}

void KeyPathExch::update_heaps_and_ufs(Edge_Heaps& bound_edge_heaps,
                                       Union_Find_Structure& subtrees_ufs,
                                       Graph::NodeId input_node_id,
                                       Graph::NodeId crucial_parent_id,
                                       const std::vector<Graph::NodeId>& internal_node_ids,
                                       Union_Find_Structure::ElementId internal_nodes_ufsroot) {
    //aktualisieren der Heaps:
    // füge die Elemente der Heaps aller Knoten des KeyPath zum Heap des crucial parent des aktuellen Knoten hinzu
    std::vector<Graph::NodeId> nodes_to_merge = internal_node_ids;
    nodes_to_merge.push_back(input_node_id);
    bound_edge_heaps.merge(crucial_parent_id, nodes_to_merge);

    //aktualisieren der Union-Find:
    // Hinzufügen der subtree's vom aktuellen Knoten sowie von den internen Knoten zum subtree des crucial_parent
    subtrees_ufs.union_by_rank(crucial_parent_id, internal_nodes_ufsroot);
    subtrees_ufs.union_by_rank(crucial_parent_id, input_node_id);
}

ImprovingChangement KeyPathExch::best_neighbor_solution(Subgraph &input_subgraph) {
    const std::vector<ImprovingChangement>& neighborhood = evaluate_neighborhood(input_subgraph, LocalSearchAux::one_move);
    Graph::PathLength output_value = 0;
    int output_index = -1;
    for( unsigned int i=0; i<neighborhood.size(); i++) {
        if(neighborhood[i].getImprovementValue() > output_value){
            output_index = (signed) i;
            output_value = neighborhood[i].getImprovementValue();
        }
    }
    if( output_index == -1) {
        return ImprovingChangement(std::vector<Graph::EdgeId>(), std::vector<Graph::EdgeId>(), 0);
    } else {
        return neighborhood[output_index];
    }
}

void KeyPathExch::complete_algorithm(Subgraph &input_subgraph) {

    int debug_while_loop_counter = 0;

    while(true) {
        debug_while_loop_counter++;

        //debug
        //GraphAuxPrint::print_graph(input_subgraph.this_graph());

        const std::vector<ImprovingChangement> improvements = KeyPathExch::evaluate_neighborhood(input_subgraph, LocalSearchAux::several_moves);
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
            std::cout << "KeyPathExch: Es entstehen Steinerblätter. \n";
        }
    }
}