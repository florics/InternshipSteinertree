//
// Created by Lukas on 18.06.2022.
//

#ifndef PRAKTIKUMSTEINERBAUM_UNION_FIND_STRUCTURE_H
#define PRAKTIKUMSTEINERBAUM_UNION_FIND_STRUCTURE_H

#include "vector"

//? es gibt auch andere Möglichkeiten für union (union_by_size) und find (path splitting, path halving)

class Union_Find_Structure {
public:
    using ElementId = unsigned int;
    using Rank = unsigned int;

    Union_Find_Structure(unsigned int num_elts);

    //klassische Funktionen:

    //erstellt ein-elementige Menge
    //Eingabe: darf nicht bereits in einer Menge liegen
    void make_set(ElementId input_elt);
    //todo: union by size ist besser oder?
    //vereinigt die Mengen, in denen die eingegebenen Elemente liegen
    //Eingabe: beide Elemente müssen bereits in Mengen liegen, die sich insbesondere unterscheiden
    void union_by_rank(ElementId elt_x, ElementId elt_y);
    //gibt das Element aus, was als Label für die Menge, in der input_elt liegt, gilt (also die Wurzel)
    //setzt mit 'Path-Compression' die Eltern der Elemente, die auf dem Weg von input_elt zur Wurzel liegen, jeweils auf die Wurzel
    //Eingabe: muss bereits in einer Menge liegen, das ungueltige Element wird auch zugelassen (dann wird entsprechend das ungueltige Element wieder ausgegeben)
    ElementId find(ElementId input_elt);

    //weitere Funktionen:

    //gibt 1 aus gdw. Eingabeelement Wurzel ist
    bool check_if_root(ElementId input_elt);

    //gibt 1 aus gdw. das Element in einer Menge liegt
    bool check_if_in_existing_set(ElementId input_elt);
    // vereinigt alle Mengen, in denen die eingegebenen Elemente liegen
    void union_multiple_sets( std::vector<ElementId> input_elements );
    // gibt 1 aus gdw. Eingabeelemente in der gleichen Menge liegen (falls beide in keiner Menge liegen, wird 0 ausgegeben)
    bool check_if_in_same_set(ElementId elt_x, ElementId elt_y);
    // gibt 1 aus gdw. das Eingabeelement in der gleichen Menge liegt wie eines der Elemente aus dem Eingabevektor
    bool check_if_elt_belongs_to_other_elts(Union_Find_Structure::ElementId input_elt, const std::vector<ElementId>& other_elts);

    unsigned int num_elements();

    //zeigt an, dass Element noch nicht zu einer Menge zugewiesen wurde
    static const ElementId invalid_elt_id;

private:
    const unsigned int _num_elements;
    std::vector<ElementId> _parents;
    std::vector<Rank> _ranks;

};


#endif //PRAKTIKUMSTEINERBAUM_UNION_FIND_STRUCTURE_H
