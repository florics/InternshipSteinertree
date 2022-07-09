//
// Created by Lukas on 22.06.2022.
//

#ifndef PRAKTIKUMSTEINERBAUM_LOCAL_SEARCH_AUX_H
#define PRAKTIKUMSTEINERBAUM_LOCAL_SEARCH_AUX_H

#include "vector"
#include "utility"
#include "queue"
//? #include "functional"

#include "graph.h"
#include "voronoi_diagram.h"
#include "EdgeSequence.h"
#include "Subgraph.h"
#include "ImprovingChangement.h"
#include "Horizontal_Edges_Lists.h"

namespace LocalSearchAux{

    enum MovesPerPass {one_move, several_moves};

    //Ausgabe: die crucial vertices des Eingabegraphens in einer post-order bzgl einer Graphendurchmusterungs-Arboreszenz
    // mit dem Eingabeknoten als Wurzel (so dass children immer vor ihrem parent stehen), aber ohne den Eingabeknoten selbst (!)
    // Eingabeknoten muss Terminal sein
    //? Laufzeit: könnte man auch gleich in Graph::make_rooted_arborescence ausgeben
    std::vector<Graph::NodeId> get_crucialnodes_in_postorder(const Graph& input_graph, Graph::NodeId root_id);

    //findet den Key-Path, der in start_node endet
    // Ausgabe: Key-Path als EdgeSequence (endnode_a ist der crucial parent des Startknoten)
    // Eingabe: Graph muss Arboreszenz mit einem Terminal als Wurzel sein
    // start_node muss crucial node sein
    // internal_node_ids wird überschrieben mit den inneren Knoten des Key-Path (internal_node_ids speichert in post-order)
    EdgeSequence find_ingoing_keypath (const Graph& input_graph,
                                       Graph::NodeId start_node,
                                       std::vector<Graph::NodeId>& internal_node_ids);

    //gibt 1 aus gdw. Eingabeknoten ein crucial node im Eingabegraphen ist
    bool check_if_crucial (const Graph::Node& input_node);

    // findet alle Kanten, die nach Entfernen der bases_to_delete boundary edges werden
    //? std::vector<EdgeSequence> get_new_bound_paths(Voronoi_diagram input_vd, const std::vector<Graph::NodeId>& bases_to_delete);

    // führt die eingegebenen Verbesserungen auf dem Eingabe-Subgraph aus
    void perform_improving_changements(Subgraph &input_subgraph, std::vector<ImprovingChangement> changements);

    // aktualisiert die pinned-Markierungen für eine boundary edge, die zum Lösungsgraphen hinzugefügt werden soll
    // markiert also die Endpunkte des zugehörigen boundary path als pinned
    void update_pinned_for_bound_egde(const Voronoi_diagram& vor_diag, const std::vector<Graph::NodeId>& solution_nodeids_of_original_nodes,
                                      std::vector<bool>& pinned, Graph::EdgeId bound_edge_id);
    // aktualisiert die forbidden-Markierungen
    // markiert alle Nachfolger des Eingabeknotens als forbidden (nicht aber den Eingabeknoten selbst)
    void update_forbidden(const Graph& solution_graph, std::vector<bool>& forbidden, Graph::NodeId node_to_mark);

    // gibt Vektor der Länge ? aus, in dem die nodes_to_process durchnummeriert werden
    // alle anderen Knoten erhalten 'no_list_available'
    std::vector<Horizontal_Edges_Lists::ListId> compute_list_ids_for_horizon_edges_lists(unsigned int num_nodes,
                                                                                         const std::vector<Graph::NodeId>& nodes_to_process);

}



#endif //PRAKTIKUMSTEINERBAUM_LOCAL_SEARCH_AUX_H
