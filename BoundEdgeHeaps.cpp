//
// Created by Lukas on 24.06.2022.
//

#include "BoundEdgeHeaps.h"

#include "iostream"
#include "vor_diag_aux_functions.h"

//? kann ich hier auch einfach nur das vd übergeben und daraus dann die base_ids berechnen? (-> Fehlermeldung wegen const...)
BoundEdgeHeaps::BoundEdgeHeaps(const Voronoi_diagram& original_vd, const std::vector<Voronoi_diagram::BaseId>& base_ids):
    _original_vd(original_vd),
    _base_ids(base_ids)
{
    std::priority_queue< std::pair<Graph::PathLength, Graph::EdgeId>,
            std::vector<  std::pair<Graph::PathLength, Graph::EdgeId> >,
            std::greater< std::pair<Graph::PathLength, Graph::EdgeId> > > empty_q;
    heap_vect.assign(_original_vd.num_bases(), empty_q);
    BoundEdgeHeaps::initialise_with_bound_edges();

    //Denke das macht keine Probleme, das wegzulassen?
    /*
    std::vector<Voronoi_diagram::BaseId> base_ids_check = original_vd.compute_base_ids();
    if( base_ids_check.size() != base_ids.size()) {
        throw std::runtime_error("(BoundEdgeHeaps::BoundEdgeHeaps) Eingaben stimmen nicht überein");
    }
    for(unsigned int i=0; i<base_ids.size(); i++) {
        if( base_ids_check[i] != base_ids[i]) {
            throw std::runtime_error("(BoundEdgeHeaps::BoundEdgeHeaps) Eingaben stimmen nicht überein");
        }
    }
     */
}


void BoundEdgeHeaps::initialise_with_bound_edges() {
    //Schleife über alle boundary edges
    for( auto curr_edge : _original_vd.original_graph().edges() ) {
        if( _original_vd.check_if_bound_edge(curr_edge) ) {

            Graph::EdgeId curr_edge_id = curr_edge.edge_id();

            //finde entsprechende Endknoten, Länge des boundary path, zugehörige Basen

            Graph::PathLength length_bound_path = VorDiagAux::compute_length_of_boundegde(_original_vd, curr_edge_id);

            Graph::NodeId curr_node_a = curr_edge.node_a();
            Graph::NodeId curr_node_b = curr_edge.node_b();
            Graph::NodeId base_a = _base_ids[ _original_vd.base()[curr_node_a] ];
            Graph::NodeId base_b = _base_ids[ _original_vd.base()[curr_node_b] ];

            if(base_a == Graph::invalid_node_id || base_b == Graph::invalid_node_id) {
                throw std::runtime_error("(LocalSearchAux::initialise_heaps_w_bound_edges) eine gefundene Basis ist ungueltig.");
            }

            //boundedges erstellen?

            heap_vect[base_a].push( {length_bound_path, curr_edge_id} );
            heap_vect[base_b].push( {length_bound_path, curr_edge_id} );
        }
    }
}

std::priority_queue<std::pair<Graph::PathLength, Graph::EdgeId>,
        std::vector<std::pair<Graph::PathLength, Graph::EdgeId> >,
        std::greater<std::pair<Graph::PathLength, Graph::EdgeId> > > &
BoundEdgeHeaps::get_heap_of_base(Voronoi_diagram::BaseId input_base) {
    return heap_vect[input_base];
}

/*
std::vector<std::pair<Graph::PathLength, Graph::EdgeId>>
BoundEdgeHeaps::cleanup_multiple_heaps(const std::vector<Voronoi_diagram::BaseId>& nodes_to_cleanup,
                                       Union_Find_Structure &ufs,
                                       const std::vector<Union_Find_Structure::ElementId>& endpoints_to_discard) {

    std::vector<std::pair<Graph::PathLength, Graph::EdgeId>> output;

    //betrachte alle Heaps, die zu Nachfolgern des zu bearbeitenden Knotes gehören
    for( auto curr_node_to_cleanup: nodes_to_cleanup) {

        std::pair<Graph::PathLength, Graph::EdgeId> curr_best_heap_elt = cleanup_one_heap(curr_node_to_cleanup, ufs, endpoints_to_discard);

        //falls wir ein gültiges Element im Heap gefunden haben, fügen wir es zu dem Ausgabevektor hinzu
        if(curr_best_heap_elt.second != Graph::invalid_edge_id) {
            output.push_back(curr_best_heap_elt);
        }
    }

    return output;
}
*/

std::pair<Graph::PathLength, Graph::EdgeId>
BoundEdgeHeaps::cleanup_one_heap(Voronoi_diagram::BaseId node_to_cleanup,
                                 Union_Find_Structure &ufs,
                                 const std::vector<Union_Find_Structure::ElementId> &endpoints_to_discard,
                                 LocalSearchAux::MovesPerPass moves_per_pass,
                                 const std::vector<bool>& forbidden) {

    std::priority_queue<std::pair<Graph::PathLength, Graph::EdgeId>,
            std::vector<std::pair<Graph::PathLength, Graph::EdgeId>>,
            std::greater<std::pair<Graph::PathLength, Graph::EdgeId>>>& heap_to_cleanup = get_heap_of_base(node_to_cleanup);


    while(not heap_to_cleanup.empty()) {
        std::pair<Graph::PathLength, Graph::EdgeId> top_heap_entry = heap_to_cleanup.top();

        //finde die Basen der aktuellen boundary edge
        std::pair<Graph::NodeId, Graph::NodeId> bases_of_top_entry = VorDiagAux::get_bases_of_edge( _original_vd, top_heap_entry.second);
        Graph::NodeId endbase_a = _base_ids[bases_of_top_entry.first];
        Graph::NodeId endbase_b = _base_ids[bases_of_top_entry.second];

        //falls die Endpunkte der aktuellen boundary edge in der gleichen Menge liegen (also beide Endbasen in S_i), suchen wir weiter
        if(ufs.check_if_in_same_set(endbase_a, endbase_b)) {
            heap_to_cleanup.pop();
            continue;
        }

        //falls die aktuelle boundary edge eine Endbase in einer der Mengen hat, die wir nicht zulassen wollen, suchen wir weiter
        else if(ufs.check_if_elt_belongs_to_other_elts(endbase_a, endpoints_to_discard)
                || ufs.check_if_elt_belongs_to_other_elts(endbase_b, endpoints_to_discard)) {
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

void BoundEdgeHeaps::merge(Graph::NodeId destination_node_id, std::vector<Graph::NodeId> nodes_to_merge) {

    // der heap, in den wir die Elemente der anderen Heaps verschieben wollen
    std::priority_queue<std::pair<Graph::PathLength, Graph::EdgeId>,
            std::vector<std::pair<Graph::PathLength, Graph::EdgeId>>,
            std::greater<std::pair<Graph::PathLength, Graph::EdgeId>>>& destination_heap = get_heap_of_base(destination_node_id);

    for(auto curr_node_id: nodes_to_merge) {
        std::priority_queue<std::pair<Graph::PathLength, Graph::EdgeId>,
                std::vector<std::pair<Graph::PathLength, Graph::EdgeId>>,
                std::greater<std::pair<Graph::PathLength, Graph::EdgeId>>>& curr_heap_to_merge = get_heap_of_base(curr_node_id);
        while(not curr_heap_to_merge.empty() ) {
            //'delete min'
            std::pair<Graph::PathLength, Graph::EdgeId> top_heap_entry = curr_heap_to_merge.top();
            curr_heap_to_merge.pop();

            destination_heap.push(top_heap_entry);
        }
    }
}


