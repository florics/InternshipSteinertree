//
// Created by Lukas on 22.06.2022.
//

#ifndef PRAKTIKUMSTEINERBAUM_KEY_PATH_EXCHANGE_H
#define PRAKTIKUMSTEINERBAUM_KEY_PATH_EXCHANGE_H

#include "vector"
#include "utility"
#include "queue"

#include "voronoi_diagram.h"
#include "graph.h"
#include "local_search_aux.h"

namespace KeyPathExch{

    //später zu bool machen?
    void key_path_exchange( Voronoi_diagram& input_vd,
                            Graph& solution,
                            std::pair< std::vector<bool>, std::vector<bool> >& subgraph_vectors);

    //Ausgabe: die crucial nodes des Eingabegraphens in einer post-order von dem Eingabeknoten aus (so dass children immer vor ihrem parent stehen)
    //Laufzeit: könnte man auch gleich in Graph::make_rooted_arborescence ausgeben
    std::vector<Graph::NodeId> get_crucialnodes_in_postorder(const Graph& input_graph, Graph::NodeId root_id);

}

#endif //PRAKTIKUMSTEINERBAUM_KEY_PATH_EXCHANGE_H
