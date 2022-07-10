//
// Created by Lukas on 24.06.2022.
//

#ifndef PRAKTIKUMSTEINERBAUM_EDGESEQUENCE_H
#define PRAKTIKUMSTEINERBAUM_EDGESEQUENCE_H

#include "graph.h"
#include "voronoi_diagram.h"


class EdgeSequence {

public:

    EdgeSequence(const std::vector<Graph::EdgeId> edges, Graph::NodeId endnode_a, Graph::NodeId endnode_b, Graph::PathLength length);

    //erstellt leeres Objekt
    EdgeSequence();

    const std::vector<Graph::EdgeId>& edge_ids() const;

    Graph::NodeId endnode_a() const;
    Graph::NodeId endnode_b() const;

    // gibt Länge des Pfads aus (falls einer der Endknoten ungültig ist, wird unendlich ausgegeben)
    Graph::PathLength length()  const;

private:

    //Kanten des Pfads
    std::vector<Graph::EdgeId> _edge_ids;

    //Endpunkte des Pfads
    Graph::NodeId _endnode_a;
    Graph::NodeId _endnode_b;

    //Länge des Pfads
    Graph::PathLength _length;

};


#endif //PRAKTIKUMSTEINERBAUM_EDGESEQUENCE_H
