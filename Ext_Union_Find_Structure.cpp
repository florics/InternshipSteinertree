//
// Created by Lukas on 01.07.2022.
//

#include "Ext_Union_Find_Structure.h"


Ext_Union_Find_Structure::Ext_Union_Find_Structure(unsigned int num_elts) :
    Union_Find_Structure(num_elts),
    _super_ids(num_elts, 0),
    _allowed(num_elts, true)
    {}

void Ext_Union_Find_Structure::set_superid(Union_Find_Structure::ElementId input_elt, Graph::NodeId new_superid) {
    Union_Find_Structure::ElementId root = find(input_elt);
    _super_ids[root] = new_superid;
}

void Ext_Union_Find_Structure::reset_superids(const std::vector<Union_Find_Structure::ElementId>& input_elts) {
    for( auto curr_elt: input_elts) {
        set_superid(curr_elt, 0);
    }

}

Graph::NodeId Ext_Union_Find_Structure::get_superid(Union_Find_Structure::ElementId input_elt) {
    Union_Find_Structure::ElementId root = find(input_elt);
    return _super_ids[root];
}

void Ext_Union_Find_Structure::set_allowed(Union_Find_Structure::ElementId input_elt, bool new_value) {
    Union_Find_Structure::ElementId root = find(input_elt);
    _allowed[root] = new_value;
}

void Ext_Union_Find_Structure::set_allowed_multiple_elts(std::vector<Union_Find_Structure::ElementId> input_set, bool new_value){
    for(auto curr_input_elt: input_set) {
        set_allowed(curr_input_elt, false);
    }
}


bool Ext_Union_Find_Structure::allowed(Union_Find_Structure::ElementId input_elt) {
    Union_Find_Structure::ElementId root = find(input_elt);
    return _allowed[root];
}


