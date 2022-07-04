//
// Created by Lukas on 30.06.2022.
//

#ifndef PRAKTIKUMSTEINERBAUM_KEY_VERTEX_ELIMINATION_H
#define PRAKTIKUMSTEINERBAUM_KEY_VERTEX_ELIMINATION_H

#include "vector"

#include "graph.h"
#include "EdgeSequence.h"
#include "ImprovingChangement.h"
#include "Ext_Union_Find_Structure.h"
#include "BoundEdgeHeaps.h"
#include "local_search_aux.h"
#include "voronoi_diagram.h"
#include "Supergraph.h"

namespace KeyVertexElim{

    void complete_algorithm(Subgraph &input_subgraph);

    // berechnet die Nachbarschaft der Eingabelösung, falls ein move per pass eingegeben wird
    // falls mehrere moves per pass eingegeben wird, so wird eine Menge von Verbesserungen ausgegeben, die simultan ausgeführt werden können
    // (so dass die Lösung ein Baum bleibt)
    std::vector<ImprovingChangement> evaluate_neighborhood(Subgraph& input_subgraph, LocalSearchAux::MovesPerPass moves_per_pass);

    ImprovingChangement process_node(const Subgraph& input_subgraph,
                                     Graph::NodeId start_node_id,
                                     Voronoi_diagram& vor_diag,
                                     Ext_Union_Find_Structure& subtrees_ufs,
                                     const std::vector< std::vector<Graph::EdgeId> >& horiz_bound_edges,
                                     BoundEdgeHeaps& vert_bound_edge_heaps,
                                     LocalSearchAux::MovesPerPass moves_per_pass,
                                     std::vector<bool>& forbidden,
                                     std::vector<bool>& pinned);

    //Ausgabe: die key vertices des Eingabegraphens in einer post-order von dem Eingabeknoten aus (so dass children immer vor ihrem parent stehen)
    // aber ohne den Eingabeknoten selbst (!)
    //? Laufzeit: könnte man auch gleich in Graph::make_rooted_arborescence ausgeben
    std::vector<Graph::NodeId> get_keynodes_in_postorder(const Graph& input_graph, Graph::NodeId root_id);

    //findet die Key-Paths, die in start_node beginnen
    // Ausgabe: Key-Paths als EdgeSequence (endnode_b ist dann jeweils ein crucial child des Startknoten)
    // Eingabe: Graph muss Arboreszenz mit einem Terminal als Wurzel sein
    // start_node muss crucial node sein
    // vect_internal_node_ids wird überschrieben mit den inneren Knoten der Key-Paths (speichert jeweils in pre-order)
    std::vector<EdgeSequence> find_outgoing_keypaths (const Graph& input_graph,
                                                      Graph::NodeId start_node,
                                                      std::vector<std::vector<Graph::NodeId>>& vect_internal_node_ids);

    //gibt 1 aus gdw. der Eingabeknoten ein KeyVertex (im Eingabegraphen) ist
    bool check_if_keyvertex(const Graph& input_graph, Graph::NodeId input_node_id);

    std::vector< std::vector<Graph::EdgeId> > compute_horizontal_bound_edges(const Subgraph& input_subgraph,
                                                                             Graph::NodeId root,
                                                                             const Voronoi_diagram& vor_diag);

    // fügt die neuen boundary egdes zum Supergraphen hinzu
    // Ausgabe: die Daten, um das Voronoi-Diagramm später wiederherzustellen
    Voronoi_diagram::RestoreData find_and_add_new_bound_edges(const Subgraph& input_subgraph,
                                      Graph::NodeId start_node_id,
                                      Voronoi_diagram& vor_diag,
                                      Ext_Union_Find_Structure& subtrees_ufs,
                                      LocalSearchAux::MovesPerPass moves_per_pass,
                                      std::vector<bool>& forbidden,
                                      const std::vector<std::vector<Graph::NodeId>>& vect_internal_nodes,
                                      Supergraph_KVE& super_graph);

    // fügt die (alten) horizontalen boundary egdes zum Supergraphen hinzu
    void add_horizontal_bound_edges(const Subgraph& input_subgraph,
                                    const Voronoi_diagram& vor_diag,
                                    Ext_Union_Find_Structure& subtrees_ufs,
                                    LocalSearchAux::MovesPerPass moves_per_pass,
                                    const std::vector<bool>& forbidden,
                                    Supergraph_KVE& super_graph,
                                    const std::vector<Graph::EdgeId>& horiz_bound_edges);

    // fügt die (alten) vertikalen boundary egdes zum Supergraphen hinzu
    void find_and_add_vertical_bound_edges(const Subgraph& input_subgraph,
                                           const Voronoi_diagram& vor_diag,
                                           Graph::NodeId start_node_id,
                                           Ext_Union_Find_Structure &subtrees_ufs,
                                           BoundEdgeHeaps &vert_bound_edge_heaps,
                                           LocalSearchAux::MovesPerPass moves_per_pass,
                                           std::vector<bool> &forbidden,
                                           const std::vector<std::vector<Graph::NodeId>> &vect_internal_nodes,
                                           const std::vector<Graph::NodeId>& crucial_children,
                                           Supergraph_KVE &super_graph);

    // gibt die Kanten der boundary paths aus, die den Kanten des MST entsprechen
    // gibt dabei keine Kanten doppelt aus (sowie keine, die bereits in added_edges stehen)
    // je nach Eingabe (bound_edge_type) werden ausschließlich neue bzw. alte boundary paths betrachtet
    std::vector<Graph::EdgeId> get_edges_to_insert(Supergraph_KVE::BoundEdgeType bound_edge_type,
                                                   const std::vector<Graph::NodeId>& solution_nodeids_of_original_nodes,
                                                   const Voronoi_diagram& vor_diag,
                                                   const Supergraph_KVE& super_graph,
                                                   const Subgraph& mst_of_supergraph,
                                                   std::vector<bool>& added_edges,
                                                   LocalSearchAux::MovesPerPass moves_per_pass,
                                                   std::vector<bool>& pinned);

}

#endif //PRAKTIKUMSTEINERBAUM_KEY_VERTEX_ELIMINATION_H
