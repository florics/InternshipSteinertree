//
// Created by Lukas on 01.07.2022.
//

#ifndef PRAKTIKUMSTEINERBAUM_EXT_UNION_FIND_STRUCTURE_H
#define PRAKTIKUMSTEINERBAUM_EXT_UNION_FIND_STRUCTURE_H

#include "Union_Find_Structure.h"
#include "graph.h"

// eine Erweiterung der Union-Find-Structure für SVE, KVE

class Ext_Union_Find_Structure: public Union_Find_Structure{

public:

    Ext_Union_Find_Structure(unsigned int num_elts);

    // setzt den super_ids-Wert der Wurzel des Eingabeelements auf new_superid
    void set_superid(Union_Find_Structure::ElementId input_elt, Graph::NodeId new_superid);
    // setzt die super_id's der Eingabe-Elemente wieder auf den default-Wert 0
    void reset_superids(const std::vector<Union_Find_Structure::ElementId>& input_elts);

    // gibt den super_ids-Wert der Wurzel des Eingabeelements aus
    Graph::NodeId get_superid(Union_Find_Structure::ElementId input_elt);

    // setzt den _allowed-Wert der Wurzel des Eingabeelements entsprechend
    void set_allowed(Union_Find_Structure::ElementId input_elt, bool new_value);
    // für mehrere Elemente
    void set_allowed_multiple_elts(std::vector<Union_Find_Structure::ElementId> input_set, bool new_value);

    // gibt den _allowed-Wert der Wurzel des Eingabeelements aus
    bool allowed(Union_Find_Structure::ElementId input_elt);

    //debug
    //void print_sets();

private:

    // weist den Wurzeln und damit den Mengen der UFS eine NodeId zu
    // (die dann die NodeId des entsprechenden Superknoten im Supergraphen der Methode sein wird)
    //? default-Wert ist 0
    std::vector<Graph::NodeId> _super_ids;

    // Hilfsstruktur, um schneller zu prüfen, ob ein Knoten in einer von mehreren Mengen liegt
    std::vector<bool> _allowed;

};


#endif //PRAKTIKUMSTEINERBAUM_EXT_UNION_FIND_STRUCTURE_H
