//
// Created by Lukas on 05.07.2022.
//

#include "Horizontal_Edges_Lists.h"

#include "limits"

#include "NCA_Tree_Data_Structure.h"
#include "vor_diag_aux_functions.h"
#include "local_search_aux.h"

const Horizontal_Edges_Lists::ListId Horizontal_Edges_Lists::no_list_available = std::numeric_limits<unsigned int>::max();

Horizontal_Edges_Lists::Horizontal_Edges_Lists(const std::vector<ListId> &list_ids,
                                               const std::vector<std::vector<Graph::EdgeId>> &vect_of_lists):
                                               _list_ids(list_ids),
                                               _vect_of_lists(vect_of_lists)
                                               {}

//todo: checks einbauen, dass KNoten im graph liegt
const std::vector<Graph::EdgeId> &Horizontal_Edges_Lists::get_list(Graph::NodeId input_node) const {
    return _vect_of_lists[ _list_ids[input_node] ];
}

