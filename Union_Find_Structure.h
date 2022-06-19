//
// Created by Lukas on 18.06.2022.
//

#ifndef PRAKTIKUMSTEINERBAUM_UNION_FIND_STRUCTURE_H
#define PRAKTIKUMSTEINERBAUM_UNION_FIND_STRUCTURE_H

#include "vector"

class Union_Find_Structure {
public:
    using ElementId = unsigned int;
    using Rank = unsigned int;

    Union_Find_Structure(unsigned int num_elts);

    //erstellt ein-elementige Menge
    //Eingabe: darf nicht bereits in einer Menge liegen
    void make_set(ElementId input_elt);
    //vereinigt die Mengen, in denen die eingegebenen Elemente liegen
    //Eingabe: beide Elemente müssen bereits in Mengen liegen, die sich insbesondere unterscheiden
    void union_by_rank(ElementId elt_x, ElementId elt_y);
    //gibt das Element aus, was als Label für die Menge, in der input_elt liegt, gilt (also die Wurzel)
    //setzt mit 'Path-Compression' die Eltern der Elemente, die auf dem Weg von input_elt zur Wurzel liegen, jeweils auf die Wurzel
    //Eingabe: muss bereits in einer Menge liegen
    ElementId find(ElementId input_elt);

    //gibt 1 aus gdw. das Element in einer Menge liegt
    bool check_if_in_set(ElementId input_elt);

    //zeigt an, dass Element noch nicht zu einer Menge zugewiesen wurde
    static const ElementId invalid_elt_id;

private:
    const unsigned int _num_elements;
    std::vector<ElementId> _parents;
    std::vector<Rank> _ranks;

};


#endif //PRAKTIKUMSTEINERBAUM_UNION_FIND_STRUCTURE_H
