//
// Created by Lukas on 24.06.2022.
//

#include "Edge_Heaps.h"

#include "iostream"

#include "vor_diag_aux_functions.h"
#include "graph_printfunctions.h"


Edge_Heaps::Edge_Heaps(const Voronoi_diagram& original_vd, const std::vector<Graph::NodeId>& solution_node_ids):
    _solution_node_ids(solution_node_ids)
{
    std::priority_queue< std::pair<Graph::PathLength, Graph::EdgeId>,
            std::vector<  std::pair<Graph::PathLength, Graph::EdgeId> >,
            std::greater< std::pair<Graph::PathLength, Graph::EdgeId> > > empty_q;
    _heap_vect.assign(original_vd.num_bases(), empty_q);

    Edge_Heaps::initialise_with_bound_edges(original_vd);
}


Edge_Heaps::Edge_Heaps(const std::vector<Graph::NodeId> &solution_node_ids, unsigned int num_solution_nodes):
        _solution_node_ids(solution_node_ids)
{
    std::priority_queue< std::pair<Graph::PathLength, Graph::EdgeId>,
            std::vector<  std::pair<Graph::PathLength, Graph::EdgeId> >,
            std::greater< std::pair<Graph::PathLength, Graph::EdgeId> > > empty_q;
    _heap_vect.assign(num_solution_nodes, empty_q);
}

void Edge_Heaps::initialise_with_bound_edges(const Voronoi_diagram& original_vd) {
    //Schleife über alle boundary edges
    for(const auto& curr_edge : original_vd.original_graph().edges() ) {
        if( original_vd.check_if_bound_edge(curr_edge) ) {

            Graph::EdgeId curr_edge_id = curr_edge.edge_id();

            //finde entsprechende Endknoten, Länge des boundary path, zugehörige Basen (als NodeIds in der aktuellen Lösung)

            Graph::PathLength length_bound_path = VorDiagAux::compute_length_of_boundpath(original_vd, curr_edge_id);

            Graph::NodeId curr_node_a = curr_edge.node_a();
            Graph::NodeId curr_node_b = curr_edge.node_b();
            Graph::NodeId base_a = _solution_node_ids[ original_vd.base()[curr_node_a] ];
            Graph::NodeId base_b = _solution_node_ids[ original_vd.base()[curr_node_b] ];

            if(base_a == Graph::invalid_node_id || base_b == Graph::invalid_node_id) {
                throw std::runtime_error("(LocalSearchAux::initialise_heaps_w_bound_edges) eine gefundene Basis ist ungueltig.");
            }

            _heap_vect[base_a].push({length_bound_path, curr_edge_id} );
            _heap_vect[base_b].push({length_bound_path, curr_edge_id} );
        }
    }
}

std::priority_queue<std::pair<Graph::PathLength, Graph::EdgeId>,
        std::vector<std::pair<Graph::PathLength, Graph::EdgeId> >,
        std::greater<std::pair<Graph::PathLength, Graph::EdgeId> > > &
Edge_Heaps::get_heap(Graph::NodeId input_node) {
    return _heap_vect[input_node];
}

/*
std::vector<std::pair<Graph::PathLength, Graph::EdgeId>>
Edge_Heaps::cleanup_multiple_heaps(const std::vector<Graph::NodeId>& nodes_to_cleanup,
                                       Union_Find_Structure &ufs,
                                       const std::vector<Union_Find_Structure::ElementId>& endpoints_to_discard) {

    std::vector<std::pair<Graph::PathLength, Graph::EdgeId>> output;

    //betrachte alle Heaps, die zu Nachfolgern des zu bearbeitenden Knotes gehören
    for( auto curr_node_to_cleanup: nodes_to_cleanup) {

        std::pair<Graph::PathLength, Graph::EdgeId> curr_best_heap_elt = cleanup_one_heap_kpe(curr_node_to_cleanup, ufs, endpoints_to_discard);

        //falls wir ein gültiges Element im Heap gefunden haben, fügen wir es zu dem Ausgabevektor hinzu
        if(curr_best_heap_elt.second != Graph::invalid_edge_id) {
            output.push_back(curr_best_heap_elt);
        }
    }

    return output;
}
*/

std::pair<Graph::PathLength, Graph::EdgeId>
Edge_Heaps::cleanup_one_heap_kpe(Graph::NodeId node_to_cleanup,
                                 const Voronoi_diagram& original_vd,
                                 Union_Find_Structure &ufs,
                                 Union_Find_Structure::ElementId endpoint_to_discard,
                                 LocalSearchAux::MovesPerPass moves_per_pass,
                                 const std::vector<bool>& forbidden) {

    std::priority_queue<std::pair<Graph::PathLength, Graph::EdgeId>,
            std::vector<std::pair<Graph::PathLength, Graph::EdgeId>>,
            std::greater<std::pair<Graph::PathLength, Graph::EdgeId>>>& heap_to_cleanup = get_heap(node_to_cleanup);


    while(not heap_to_cleanup.empty()) {
        std::pair<Graph::PathLength, Graph::EdgeId> top_heap_entry = heap_to_cleanup.top();

        //finde die Endpunkte bzw. Basen der aktuellen boundary edge (als NodeIds in der aktuellen Lösung)
        std::pair<Graph::NodeId, Graph::NodeId> bases_of_top_entry = VorDiagAux::get_bases_of_edge( original_vd, top_heap_entry.second);
        Graph::NodeId endbase_a = _solution_node_ids[bases_of_top_entry.first];
        Graph::NodeId endbase_b = _solution_node_ids[bases_of_top_entry.second];

        //falls die Endpunkte der aktuellen boundary edge in der gleichen Menge liegen (also beide Endbasen in S_i), suchen wir weiter
        if(ufs.check_if_in_same_set(endbase_a, endbase_b)) {
            heap_to_cleanup.pop();
            continue;
        }

        //falls die aktuelle boundary edge eine Endbase in einer der Mengen hat, die wir nicht zulassen wollen, suchen wir weiter
        else if(ufs.check_if_in_same_set(endbase_a, endpoint_to_discard)
                || ufs.check_if_in_same_set(endbase_b, endpoint_to_discard)) {
            heap_to_cleanup.pop();
            continue;
        }

        else if( moves_per_pass == LocalSearchAux::several_moves
                && (forbidden[endbase_a] || forbidden[endbase_b]) ) {
            // falls die Endpunkte als forbidden markiert sind, suchen wir weiter
            heap_to_cleanup.pop();
            continue;
        }

        else {
            return top_heap_entry;
        }
    }

    //falls wir keine passende boundary edge im heap gefunden haben, geben wir ein ungültiges Paar zurück
    return {Graph::infinite_length, Graph::invalid_edge_id};
}

void Edge_Heaps::merge(Graph::NodeId destination_node_id, std::vector<Graph::NodeId> nodes_to_merge) {

    // der heap, in den wir die Elemente der anderen Heaps verschieben wollen
    std::priority_queue<std::pair<Graph::PathLength, Graph::EdgeId>,
            std::vector<std::pair<Graph::PathLength, Graph::EdgeId>>,
            std::greater<std::pair<Graph::PathLength, Graph::EdgeId>>>& destination_heap = get_heap(destination_node_id);

    for(auto curr_node_id: nodes_to_merge) {
        std::priority_queue<std::pair<Graph::PathLength, Graph::EdgeId>,
                std::vector<std::pair<Graph::PathLength, Graph::EdgeId>>,
                std::greater<std::pair<Graph::PathLength, Graph::EdgeId>>>& curr_heap_to_merge = get_heap(curr_node_id);
        while(not curr_heap_to_merge.empty() ) {
            //'delete min'
            std::pair<Graph::PathLength, Graph::EdgeId> top_heap_entry = curr_heap_to_merge.top();
            curr_heap_to_merge.pop();

            destination_heap.push(top_heap_entry);
        }
    }
}

std::vector<std::pair<Graph::PathLength, Graph::EdgeId>>
Edge_Heaps::cleanup_heaps_kve(const std::vector<Graph::NodeId> &nodes_to_cleanup,
                              const Voronoi_diagram& original_vd,
                              Ext_Union_Find_Structure &ufs,
                              LocalSearchAux::MovesPerPass moves_per_pass,
                              const std::vector<bool>& forbidden) {

    std::vector<std::pair<Graph::PathLength, Graph::EdgeId>> output;
    output.reserve(nodes_to_cleanup.size());

    for( auto curr_node_to_cleanup: nodes_to_cleanup) {
        std::pair<Graph::PathLength, Graph::EdgeId> curr_bound_edge =
                Edge_Heaps::cleanup_one_heap_kve(curr_node_to_cleanup, original_vd, ufs, moves_per_pass, forbidden);
        output.push_back(curr_bound_edge);
    }

    return output;
}

std::pair<Graph::PathLength, Graph::EdgeId>
Edge_Heaps::cleanup_one_heap_kve(const Graph::NodeId node_to_cleanup,
                                 const Voronoi_diagram& original_vd,
                                 Ext_Union_Find_Structure &ufs,
                                 LocalSearchAux::MovesPerPass moves_per_pass,
                                 const std::vector<bool> &forbidden) {

    std::priority_queue<std::pair<Graph::PathLength, Graph::EdgeId>,
            std::vector<std::pair<Graph::PathLength, Graph::EdgeId>>,
            std::greater<std::pair<Graph::PathLength, Graph::EdgeId>>>& heap_to_cleanup = get_heap(node_to_cleanup);


    while(not heap_to_cleanup.empty()) {
        std::pair<Graph::PathLength, Graph::EdgeId> top_heap_entry = heap_to_cleanup.top();

        //debug
        //GraphAuxPrint::print_edge_sequence(original_vd.original_graph(), VorDiagAux::compute_bound_path(original_vd, top_heap_entry.second) );
        //fflush(stdout);

        //finde die Endpunkte bzw. Basen der aktuellen boundary edge (als NodeIds in der aktuellen Lösung)
        std::pair<Graph::NodeId, Graph::NodeId> bases_of_top_entry = VorDiagAux::get_bases_of_edge( original_vd, top_heap_entry.second);
        Graph::NodeId endbase_a = _solution_node_ids[bases_of_top_entry.first];
        Graph::NodeId endbase_b = _solution_node_ids[bases_of_top_entry.second];

        if( moves_per_pass == LocalSearchAux::several_moves ) {
            if (forbidden[endbase_a] || forbidden[endbase_b]) {
                // falls die Endpunkte als forbidden markiert sind, suchen wir weiter
                heap_to_cleanup.pop();
                continue;
            }
        }

        //debug
        if( ufs.allowed(endbase_a) && ufs.allowed(endbase_b) ) {
            throw std::runtime_error("(Edge_Heaps::cleanup_one_heap_kve) Kante verläuft innerhalb des Subbaums des Vorgängers.");
        }

        if( not ufs.allowed(endbase_a) && not ufs.allowed(endbase_b) ) {
            // falls die Endpunkte in einer der Mengen der UFS liegen, die wir verboten haben, suchen wir weiter
            heap_to_cleanup.pop();
            continue;
        } else {
            return top_heap_entry;
        }
    }

    //falls wir keine passende boundary edge im heap gefunden haben, geben wir ein ungültiges Paar zurück
    return {Graph::infinite_length, Graph::invalid_edge_id};
}


std::vector<std::pair<Graph::PathLength, Graph::EdgeId>>
Edge_Heaps::cleanup_heaps_sve(Graph::NodeId start_node,
                              const std::vector<Graph::NodeId>& children,
                              const Graph& original_graph,
                              Ext_Union_Find_Structure &subtrees_ufs,
                              LocalSearchAux::MovesPerPass moves_per_pass,
                              const std::vector<bool> &forbidden) {

    std::vector<Graph::NodeId> endpoints_to_discard = children;
    endpoints_to_discard.push_back(start_node);

    // markiere die Knoten, die wir nicht als Endpunkte von Kanten zulassen wollen
    // (d.h. solche wollen wir nicht zum Heap hinzufügen)
    subtrees_ufs.set_allowed_multiple_elts(endpoints_to_discard, false);

    std::vector<std::pair<Graph::PathLength, Graph::EdgeId>> output;
    output.reserve(children.size());

    for( auto curr_node_to_cleanup: children) {
        std::pair<Graph::PathLength, Graph::EdgeId> curr_edge =
                Edge_Heaps::cleanup_one_heap_sve(curr_node_to_cleanup, original_graph, subtrees_ufs, moves_per_pass, forbidden);
        output.push_back(curr_edge);
    }

    // mache die Markierungen rückgängig
    subtrees_ufs.set_allowed_multiple_elts(endpoints_to_discard, true);

    return output;
}

std::pair<Graph::PathLength, Graph::EdgeId>
Edge_Heaps::cleanup_one_heap_sve(const Graph::NodeId node_to_cleanup,
                                 const Graph& original_graph,
                                 Ext_Union_Find_Structure &ufs,
                                 LocalSearchAux::MovesPerPass moves_per_pass,
                                 const std::vector<bool> &forbidden) {

    std::priority_queue<std::pair<Graph::PathLength, Graph::EdgeId>,
            std::vector<std::pair<Graph::PathLength, Graph::EdgeId>>,
            std::greater<std::pair<Graph::PathLength, Graph::EdgeId>>>& heap_to_cleanup = get_heap(node_to_cleanup);


    while(not heap_to_cleanup.empty()) {
        std::pair<Graph::PathLength, Graph::EdgeId> top_heap_entry = heap_to_cleanup.top();

        Graph::Edge curr_edge = original_graph.get_edge(top_heap_entry.second);

        // finde die Endpunkte der aktuellen Kante als NodeIds in der aktuellen Lösung
        Graph::NodeId sol_node_id_a = _solution_node_ids[curr_edge.node_a()];
        Graph::NodeId sol_node_id_b = _solution_node_ids[curr_edge.node_b()];

        if( moves_per_pass == LocalSearchAux::several_moves ) {
            if (forbidden[sol_node_id_a] || forbidden[sol_node_id_b]) {
                // falls die Endpunkte als forbidden markiert sind, suchen wir weiter
                heap_to_cleanup.pop();
                continue;
            }
        }

        //debug
        if( ufs.allowed(sol_node_id_a) && ufs.allowed(sol_node_id_b) ) {
            throw std::runtime_error("(Edge_Heaps::cleanup_one_heap_sve) Kante verläuft innerhalb des Subbaums des Vorgängers.");
        }

        if( not ufs.allowed(sol_node_id_a) && not ufs.allowed(sol_node_id_b) ) {
            // falls die Endpunkte in einer der Mengen der UFS liegen, die wir verboten haben, suchen wir weiter
            heap_to_cleanup.pop();
            continue;
        } else {
            return top_heap_entry;
        }
    }

    //falls wir keine passende boundary edge im heap gefunden haben, geben wir ein ungültiges Paar zurück
    return {Graph::infinite_length, Graph::invalid_edge_id};

}


