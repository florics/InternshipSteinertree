//
// Created by Lukas on 24.06.2022.
//

#ifndef PRAKTIKUMSTEINERBAUM_EDGE_HEAPS_H
#define PRAKTIKUMSTEINERBAUM_EDGE_HEAPS_H

#include "vector"
#include "queue"

#include "graph.h"
#include "voronoi_diagram.h"
#include "EdgeSequence.h"
#include "Union_Find_Structure.h"
#include "Ext_Union_Find_Structure.h"
#include "local_search_aux.h"

// Hilfsstruktur für die Heaps in Steiner-Vertex-Elimination, Key-Path-Exchange und Key-Vertex-Elimination
// manche Funktionen sind entsprechend mit sve, kpe, kve markiert

class Edge_Heaps {

public:

    // Konstruktor für Key -Path-Exchange und -Vertex-Elimination
    Edge_Heaps(const Voronoi_diagram& original_vd, const std::vector<Graph::NodeId>& solution_node_ids);

    // Konstruktor für Steiner-Vertex-Elimination
    Edge_Heaps(const std::vector<Graph::NodeId>& solution_node_ids, unsigned int num_solution_nodes);

    //gibt den Heap der Eingabebasis aus, die NodeId ist die entsprechende Stelle im Heap-Vektor (d.h. es ist die NodeId in der aktuellen Lösung)
    std::priority_queue< std::pair<Graph::PathLength, Graph::EdgeId>,
                            std::vector<  std::pair<Graph::PathLength, Graph::EdgeId> >,
                                std::greater< std::pair<Graph::PathLength, Graph::EdgeId> > >& get_heap(Graph::NodeId input_node);


    // Ausgabe: in dem i-ten Eintrag steht die beste vertikale Kante, die zwischen dem Subbaum, der zum i-ten Nachfolger (des aktuellen Knotens)
    // gehört, und dem Subbaum des Vorgängers (des aktuellen Knotens) verläuft (i=1,...) (falls keine solche Kante gefunden wurde,
    // gibt es einen 'ungueltigen' Eintrag)
    std::vector<std::pair<Graph::PathLength, Graph::EdgeId>> cleanup_heaps_sve(Graph::NodeId start_node,
                                                                               const std::vector<Graph::NodeId>& children,
                                                                               const Graph& original_graph,
                                                                               Ext_Union_Find_Structure &ufs,
                                                                               LocalSearchAux::MovesPerPass moves_per_pass,
                                                                               const std::vector<bool>& forbidden);

    // Ausgabe: die beste vertikale Kante zwischen dem Subbaum des Eingabeknotens und dem Subbaum des Vorgängers (des aktuellen Knotens)
    std::pair<Graph::PathLength, Graph::EdgeId> cleanup_one_heap_sve(const Graph::NodeId node_to_cleanup,
                                                                     const Graph& original_graph,
                                                                               Ext_Union_Find_Structure &ufs,
                                                                               LocalSearchAux::MovesPerPass moves_per_pass,
                                                                               const std::vector<bool>& forbidden);

    // Ausgabe: in dem i-ten Eintrag steht die beste vertikale boundary edge, die zwischen dem Subbaum, der zum i-ten crucial child gehört,
    // und dem Subbaum des crucial parent verläuft (i=1,...) (falls keine solche Kante gefunden wurde, gibt es einen 'ungueltigen' Eintrag)
    // Eingabe (nodes_to_cleanup) muss entsprechend die crucial children enthalten
    std::vector<std::pair<Graph::PathLength, Graph::EdgeId>> cleanup_heaps_kve(const std::vector<Graph::NodeId>& nodes_to_cleanup,
                                                                               const Voronoi_diagram& original_vd,
                                                                               Ext_Union_Find_Structure &ufs,
                                                                               LocalSearchAux::MovesPerPass moves_per_pass,
                                                                               const std::vector<bool>& forbidden);

    // Ausgabe: die beste vertikale boundary edge zwischen dem Subbaum des Eingabeknotens und dem Subbaum des crucial parent
    std::pair<Graph::PathLength, Graph::EdgeId> cleanup_one_heap_kve(const Graph::NodeId node_to_cleanup,
                                                                     const Voronoi_diagram& original_vd,
                                                                               Ext_Union_Find_Structure &ufs,
                                                                               LocalSearchAux::MovesPerPass moves_per_pass,
                                                                               const std::vector<bool>& forbidden);

    /*//cleanup für Key-Vertex-Elimination
    //? näher beschreiben?
    std::vector<std::pair<Graph::PathLength, Graph::EdgeId>> cleanup_multiple_heaps(const std::vector<Graph::NodeId>& nodes_to_cleanup,
                                                                                    Union_Find_Structure &ufs,
                                                                                    const std::vector<Union_Find_Structure::ElementId>& endpoints_to_discard);
    */

    //cleanup für Key-EdgeSequence-Exchange
    std::pair<Graph::PathLength, Graph::EdgeId> cleanup_one_heap_kpe(Graph::NodeId node_to_cleanup,
                                                                     const Voronoi_diagram& original_vd,
                                                                     Union_Find_Structure &ufs,
                                                                     Union_Find_Structure::ElementId endpoint_to_discard,
                                                                     LocalSearchAux::MovesPerPass moves_per_pass,
                                                                     const std::vector<bool>& forbidden);



    void merge(Graph::NodeId destination_id, std::vector<Graph::NodeId> nodes_to_merge);


private:

    std::vector< std::priority_queue< std::pair<Graph::PathLength, Graph::EdgeId>,
                                        std::vector<  std::pair<Graph::PathLength, Graph::EdgeId> >,
                                            std::greater< std::pair<Graph::PathLength, Graph::EdgeId> > >> _heap_vect;

    // speichert für jeden Knoten aus dem zugrundeliegenden Graphen, welche Stelle im Heap-Vektor diesem Knoten entspricht
    // falls dieser Knoten keinen Heap hat (also nicht in der aktuellen Lösung liegt), wird der ungültige Knoten gespeichert
    const std::vector<Graph::NodeId>& _solution_node_ids;

    //fügt die boundary edges des Voronoi-Diagramms zu den Heaps hinzu,
    // sodass jeder Heap eines Knotens alle zur entsprechenden V.-Region inzidenten boundary edges enthält
    void initialise_with_bound_edges(const Voronoi_diagram& original_vd);

};


#endif //PRAKTIKUMSTEINERBAUM_EDGE_HEAPS_H
