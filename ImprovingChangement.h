//
// Created by Lukas on 25.06.2022.
//

#ifndef PRAKTIKUMSTEINERBAUM_IMPROVINGCHANGEMENT_H
#define PRAKTIKUMSTEINERBAUM_IMPROVINGCHANGEMENT_H

#include <vector>

#include "graph.h"

class ImprovingChangement {
public:
    ImprovingChangement(std::vector<Graph::EdgeId> edgesToRemove,
                        std::vector<Graph::EdgeId> edgesToInsert,
                        Graph::PathLength improvementValue);

    const std::vector<Graph::EdgeId> &getEdgesToRemove() const;
    const std::vector<Graph::EdgeId> &getEdgesToInsert() const;
    const Graph::PathLength getImprovementValue() const;


private:

    //path anstelle von std::vector<Graph::EdgeId> verwenden ?

    const std::vector<Graph::EdgeId> _edges_to_remove;
    const std::vector<Graph::EdgeId> _edges_to_insert;

    //Wert, um den sich die Lösung bei Durchführung der Modifizierung verbessert
    const Graph::PathLength _improvement_value;
};


#endif //PRAKTIKUMSTEINERBAUM_IMPROVINGCHANGEMENT_H
