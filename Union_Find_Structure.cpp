//
// Created by Lukas on 18.06.2022.
//

#include "Union_Find_Structure.h"

#include "iostream"
#include "limits"

const Union_Find_Structure::ElementId Union_Find_Structure::invalid_elt_id = std::numeric_limits<unsigned int>::max();

Union_Find_Structure::Union_Find_Structure(unsigned int num_elts):
    _num_elements(num_elts)
{
    std::vector<Union_Find_Structure::ElementId> var_parents(_num_elements, Union_Find_Structure::invalid_elt_id);
    _parents = var_parents;

    std::vector<Union_Find_Structure::Rank> var_ranks(_num_elements, 0);
    _ranks = var_ranks;
}

void Union_Find_Structure::make_set(Union_Find_Structure::ElementId input_elt) {
    if(_parents[input_elt] != invalid_elt_id) {
        throw std::runtime_error("(Union_Find_Structure::make_set) eingegebenes Element liegt bereits in einer Menge");
    }
    _parents[input_elt] = input_elt;
}

void Union_Find_Structure::union_by_rank(Union_Find_Structure::ElementId elt_x,
                                         Union_Find_Structure::ElementId elt_y) {
    //finde die Wurzeln der eingegebenen Elemente
    Union_Find_Structure::ElementId root_x = find(elt_x);
    Union_Find_Structure::ElementId root_y = find(elt_y);

    if(root_x == root_y){
        throw std::runtime_error("(Union_Find_Structure::union_by_rank) eingegebene Elemente liegen in der gleichen Menge");
    }

    if( root_x == Union_Find_Structure::invalid_elt_id || root_y == Union_Find_Structure::invalid_elt_id){
        return;
    }

    //verbinden der Wurzeln und damit der Bäume und update der ranks, je nach ursprünglichen ranks der Wurzeln
    if( _ranks[root_x] < _ranks[root_y] ) {
        _parents[root_x] = root_y;
    } else {
        _parents[root_y] = root_x;
        if( _ranks[root_x] == _ranks[root_y] ) {
            _ranks[root_x] ++;
        }
    }
}

Union_Find_Structure::ElementId Union_Find_Structure::find(Union_Find_Structure::ElementId input_elt) {
    if( input_elt == Union_Find_Structure::invalid_elt_id) {
        return Union_Find_Structure::invalid_elt_id;
    }
    if(input_elt >= _num_elements) {
        std::cout << "Eingegebenes Element (input_elt): " << input_elt << "\n";
        throw std::runtime_error("(Union_Find_Structure::find) eingegebenes Element liegt nicht in der Datenstruktur");
    }
    if(_parents[input_elt] == Union_Find_Structure::invalid_elt_id) {
        throw std::runtime_error("(Union_Find_Structure::find) eingegebenes Element wurde noch nicht zu einer Menge hinzugefuegt.");
    }

    Union_Find_Structure::ElementId root = input_elt;

    //finde die zur Eingabe gehörende Wurzel
    while( _parents[root] != root ) {
        root = _parents[root];
        //debug? kann eig nicht eintreten oder? Laufzeit hier Problem?
        if(root == Union_Find_Structure::invalid_elt_id) {
            throw std::runtime_error("(Union_Find_Structure::find) ein Pfad führt zu Wurzel mit ungueltigem parent");
        }
    }

    //path compression
    Union_Find_Structure::ElementId next_elt = input_elt;
    while( _parents[next_elt] != root ) {
        Union_Find_Structure::ElementId curr_parent = _parents[next_elt];
        _parents[next_elt] = root;
        next_elt = curr_parent;
    }

    return root;
}

bool Union_Find_Structure::check_if_in_existing_set(Union_Find_Structure::ElementId input_elt) {
    if(input_elt >= _num_elements) {
        std::cout << "Eingegebenes Element (input_elt): " << input_elt << "\n";
        throw std::runtime_error("(Union_Find_Structure::check_if_in_existing_set) eingegebenes Element liegt nicht in der Datenstruktur");
    }

    if(_parents[input_elt] == Union_Find_Structure::invalid_elt_id){
        return false;
    } else {
        return true;
    }
}

void Union_Find_Structure::union_multiple_sets(std::vector<Union_Find_Structure::ElementId> input_elements) {
    if(input_elements.size() < 2) {
        return;
    }

    //(bemerke i<input_elements.size()-1, da wir in jeder Iteration schon auf das nächste Elt zugreifen)
    for(unsigned int i = 0; i<input_elements.size()-1; i++) {
        union_by_rank(input_elements[i], input_elements[i+1]);
    }
}

bool Union_Find_Structure::check_if_in_same_set(Union_Find_Structure::ElementId elt_x,
                                                Union_Find_Structure::ElementId elt_y) {
    Union_Find_Structure::ElementId root_x = find(elt_x);
    Union_Find_Structure::ElementId root_y = find(elt_y);

    if( root_x == root_y ) {
        if( root_x != Union_Find_Structure::invalid_elt_id){
            return true;
        }
    }

    return false;
}

bool Union_Find_Structure::check_if_elt_belongs_to_other_elts(Union_Find_Structure::ElementId input_elt, const std::vector<ElementId>& other_elts) {
    for(auto curr_other_elt: other_elts) {
        if(check_if_in_same_set(curr_other_elt, input_elt)) {
            return true;
        }
    }
    return false;
}

bool Union_Find_Structure::check_if_root(Union_Find_Structure::ElementId input_elt) {
    if(input_elt >= _num_elements) {
        std::cout << "Eingegebenes Element (input_elt): " << input_elt << "\n";
        throw std::runtime_error("(Union_Find_Structure::check_if_root) eingegebenes Element liegt nicht in der Datenstruktur");
    }

    if( _parents[input_elt] == input_elt) {
        return true;
    } else {
        return false;
    }
}

unsigned int Union_Find_Structure::num_elements() {
    return _num_elements;
}


