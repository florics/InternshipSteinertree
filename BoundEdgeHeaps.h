//
// Created by Lukas on 24.06.2022.
//

#ifndef PRAKTIKUMSTEINERBAUM_BOUNDEDGEHEAPS_H
#define PRAKTIKUMSTEINERBAUM_BOUNDEDGEHEAPS_H

#include "vector"
#include "queue"

#include "graph.h"
#include "voronoi_diagram.h"
#include "EdgeSequence.h"
#include "Union_Find_Structure.h"
#include "local_search_aux.h"

//todo:debug alles

class BoundEdgeHeaps {

public:

    BoundEdgeHeaps(const Voronoi_diagram& original_vd, const std::vector<Voronoi_diagram::BaseId>& base_ids);

    //fügt die boundary edges des Voronoi-Diagramms zu den Heaps hinzu,
    // sodass jeder Heap eines Knotens alle zur entsprechenden V.-Region inzidenten boundary edges enthält
    void initialise_with_bound_edges();

    //gibt Referenz auf den Heap der Eingabebasis aus, die NodeId ist die entsprechende Stelle im Heap-Vektor (d.h. es ist die NodeId in dem Graphen mit den Basen als Knoten)
    std::priority_queue< std::pair<Graph::PathLength, Graph::EdgeId>,
                            std::vector<  std::pair<Graph::PathLength, Graph::EdgeId> >,
                                std::greater< std::pair<Graph::PathLength, Graph::EdgeId> > >& get_heap_of_base(Voronoi_diagram::BaseId input_base);

    //cleanup für Key-Vertex-Elimination
    //? näher beschreiben?
    std::vector<std::pair<Graph::PathLength, Graph::EdgeId>> cleanup_multiple_heaps(const std::vector<Voronoi_diagram::BaseId>& nodes_to_cleanup,
                                                                                    Union_Find_Structure &ufs,
                                                                                    const std::vector<Union_Find_Structure::ElementId>& endpoints_to_discard);

    //cleanup für Key-EdgeSequence-Exchange
    // todo: ggf. vereinfachen, wenn ich cleanup für KVE umschreibe
    std::pair<Graph::PathLength, Graph::EdgeId> cleanup_one_heap(Voronoi_diagram::BaseId node_to_cleanup,
                                                                 Union_Find_Structure &ufs,
                                                                 const std::vector<Union_Find_Structure::ElementId>& endpoints_to_discard,
                                                                 LocalSearchAux::MovesPerPass moves_per_pass,
                                                                 const std::vector<bool>& forbidden);

    void merge(Voronoi_diagram::BaseId destination_id, std::vector<Voronoi_diagram::BaseId> nodes_to_merge);


private:

    std::vector< std::priority_queue< std::pair<Graph::PathLength, Graph::EdgeId>,
                                        std::vector<  std::pair<Graph::PathLength, Graph::EdgeId> >,
                                            std::greater< std::pair<Graph::PathLength, Graph::EdgeId> > >> heap_vect;

    //zugrundeliegendes Voronoi-Diagramm
    const Voronoi_diagram& _original_vd;

    // die Base-Ids geben an, welche Stelle im Heap-Vektor einem gegebenen Knoten aus dem zugrundeliegenden Graphen entspricht
    const std::vector<Voronoi_diagram::BaseId>& _base_ids;


};


#endif //PRAKTIKUMSTEINERBAUM_BOUNDEDGEHEAPS_H
