//
// Created by Lukas on 22.06.2022.
//

#include "key_path_exchange.h"

#include "iostream"
#include "algorithm"

#include "vor_diag_aux_functions.h"
#include "EdgeSequence.h"


std::vector<ImprovingChangement> KeyPathExch::evaluate_neighborhood(Graph& original_graph,
                                                                    Graph& solution_graph,
                                                                    std::pair< std::vector<bool>, std::vector<bool> >& subgraph_vectors,
                                                                    std::vector<Graph::NodeId> solution_nodeids_of_original_nodes) {
    //ist das debug?
    if( original_graph.num_nodes() != subgraph_vectors.first.size() ) {
        throw std::runtime_error("(KeyPathExch::key_path_exchange) Anzahl Knoten in original_graph ungleich Anzahl Einträge in subgraph_vectors.");
    }
    if( original_graph.num_edges() != subgraph_vectors.second.size() ) {
        throw std::runtime_error("(KeyPathExch::key_path_exchange) Anzahl Kanten in original_graph ungleich Anzahl Einträge in subgraph_vectors.");
    }

    // die Knotenmenge der aktuellen Lösung als NodeIds des zugrundeliegenden Graphen (ist aber keine Übersetzung der NodeIds)
    std::vector<Graph::NodeId> solution_nodes_as_original_nodeids;
    for(unsigned  int i = 0; i<original_graph.num_nodes(); i++) {
        if( subgraph_vectors.first[i] ) {
            solution_nodes_as_original_nodeids.push_back(i);
        }
    }

    //ist das debug?
    if( solution_graph.num_nodes() != solution_nodes_as_original_nodeids.size() ) {
        throw std::runtime_error("(KeyPathExch::key_path_exchange) Anzahl Knoten in original_graph ungleich Anzahl Einträge in subgraph_vectors.");
    }

    unsigned int num_nodes_of_solution = solution_graph.num_nodes();

    //hier beginnt die eigentliche Präprozessierung

    //Initialisieren der Union Find, erstelle Menge für alle Knoten in der aktuellen Lösung
    Union_Find_Structure subtrees_ufs(num_nodes_of_solution);
    for(unsigned int i=0; i<num_nodes_of_solution; i++) {
        subtrees_ufs.make_set(i);
    }

    Voronoi_diagram vor_diag(solution_nodes_as_original_nodeids, original_graph);

    BoundEdgeHeaps bound_edge_heaps(vor_diag, solution_nodeids_of_original_nodes);

    //richte den Graph mit beliebiger (?) Wurzel, berechne Reihenfolge der Knoten für die Prozessierung
    //? hier kann man root_id frei wählen
    Graph::NodeId root_id = solution_graph.get_vect_term()[0];
    solution_graph.make_rooted_arborescence(root_id);
    std::vector<Graph::NodeId> crucialnodes_in_postorder = LocalSearchAux::get_crucialnodes_in_postorder(solution_graph, root_id);

    //main loop der Methode
    std::vector<ImprovingChangement> evaluated_neighborhood;
    for(auto curr_node_id: crucialnodes_in_postorder) {
        //todo: nur hinzufgen, falls value>0
        evaluated_neighborhood.push_back(
                KeyPathExch::process_node(curr_node_id, original_graph, solution_graph, vor_diag, subtrees_ufs, bound_edge_heaps) );
    }

    return evaluated_neighborhood;
}

ImprovingChangement KeyPathExch::process_node(Graph::NodeId input_node_id,
                                              const Graph& original_graph,
                                              const Graph& solution_graph,
                                              Voronoi_diagram& vor_diag,
                                              Union_Find_Structure& subtrees_ufs,
                                              BoundEdgeHeaps& bound_edge_heaps) {

    // finde den keypath der im Eingabeknoten endet, sowie dessen Anfangsknoten & Länge
    std::vector<Graph::NodeId> internal_node_ids;
    EdgeSequence key_path = LocalSearchAux::find_ingoing_keypath(solution_graph, input_node_id, internal_node_ids);
    Graph::NodeId crucial_parent_id = key_path.endnode_a();

    //debug
    if( input_node_id == crucial_parent_id ) {
        throw std::runtime_error("key path exchange main loop");
    }
    if( input_node_id != key_path.endnode_b() ) {
        throw std::runtime_error("key path exchange main loop");
    }

    //todo: ggf das ganze newbound-zeug auslagern

    //finde alle neuen boundary paths, die nach Entfernen der internen Knoten des keypath auftreten
    //Laufzeit? Kopieren des Vor-Diag
    std::vector<EdgeSequence> new_bound_paths = LocalSearchAux::get_new_bound_paths(vor_diag, internal_node_ids);

    // der Schlüssel des subtree des aktuellen Knoten in der Union-Find (im Paper: S_v)
    //? Fehleranfälligkeit: der könnte sich ggf. verändern
    Union_Find_Structure::ElementId input_node_ufskey = subtrees_ufs.find(input_node_id);

    //finde den besten neuen boundary path, der zwischen den subtrees von curr_node und crucial_parent verläuft
    EdgeSequence best_new_bound_path({}, Graph::invalid_node_id, Graph::invalid_node_id, Graph::infinite_length);
    for(auto curr_bound_path: new_bound_paths) {

        // finde die keys bzgl. der Union Find der Endpunkte bzw. -basen der boundary paths
        Graph::NodeId var_base_a = curr_bound_path.endnode_a();
        Graph::NodeId var_base_b = curr_bound_path.endnode_b();
        Union_Find_Structure::ElementId var_base_a_ufskey = subtrees_ufs.find(var_base_a);
        Union_Find_Structure::ElementId var_base_b_ufskey = subtrees_ufs.find(var_base_b);

        //prüfe, ob der bound path zwischen dem subtree des Eingabeknotens und dem des Vorgängers des Eingabeknotens verläuft
        // (bemerke, dass er keinen Endknoten haben kann, der zu den internen Knoten des keypath gehört,
        // da er ein boundary path bzgl des repaired Vor-D. ist)

        if(( var_base_a_ufskey == input_node_ufskey && var_base_b_ufskey != input_node_ufskey )
            || ( var_base_a_ufskey != input_node_ufskey && var_base_b_ufskey == input_node_ufskey )) {

            //prüfe, ob die Kante zu einem besseren boundary path gehört als der aktuell beste
            if(curr_bound_path.length() < best_new_bound_path.length()) {
                best_new_bound_path = curr_bound_path;
            }
        }
    }

    //füge die internen Knoten des keypath zu einer Menge in der Union Find zusammen
    subtrees_ufs.union_multiple_sets(internal_node_ids);
    // der Schlüssel des subtree der internen Knoten in der Union-Find
    //? Fehleranfälligkeit: der könnte sich ggf. verändern
    Union_Find_Structure::ElementId internal_nodes_ufskey = Union_Find_Structure::invalid_elt_id;
    if(not internal_node_ids.empty()) {
        internal_nodes_ufskey = subtrees_ufs.find(internal_node_ids.front());
    }


    //finde die original boundary edge
    std::pair<Graph::PathLength, Graph::EdgeId>
            best_original_bound_edge = bound_edge_heaps.cleanup_one_heap(input_node_id, subtrees_ufs, {internal_nodes_ufskey});


    // füge die Elemente der Heaps aller Knoten des KeyPath zum Heap des crucial parent des aktuellen Knoten hinzu
    std::vector<Graph::NodeId> nodes_to_merge = internal_node_ids;
    nodes_to_merge.push_back(input_node_id);
    bound_edge_heaps.merge(crucial_parent_id, nodes_to_merge);

    //aktualisieren der Union-Find: Hinzufügen der subtree's vom aktuellen Knoten sowie von den internen Knoten zum subtree des crucial_parent
    subtrees_ufs.union_by_rank(crucial_parent_id, internal_nodes_ufskey);
    subtrees_ufs.union_by_rank(crucial_parent_id, input_node_id);


    // auslagern? (brauche ich aber nirgendwo sonst)
    // todo: schöner machen, hier brauche ich nicht extra eine EdgeSeque zu erstellen...
    // finde die beste boundary edge von den beiden gefundenen
    EdgeSequence bound_path_to_exchange = best_new_bound_path;
    if( best_new_bound_path.length() > best_original_bound_edge.first ) {
        bound_path_to_exchange = VorDiagAux::compute_bound_path(vor_diag, best_original_bound_edge.second);
    }

    Graph::PathLength improve_value = key_path.length() - bound_path_to_exchange.length();

    //beachte, dass die edgesToRemove Kanten im solution-G. sind und die edgesToInsert Kanten im or-G.
    ImprovingChangement output(key_path.edge_ids(), bound_path_to_exchange.edge_ids(), improve_value);

    return output;
}
