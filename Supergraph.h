//
// Created by Lukas on 01.07.2022.
//

#ifndef PRAKTIKUMSTEINERBAUM_SUPERGRAPH_H
#define PRAKTIKUMSTEINERBAUM_SUPERGRAPH_H

#include "vector"

#include "graph.h"

// für die Supergraphen in Steiner- & Key-Vertex-Elimination

class Supergraph {

public:

    // erstellt Graphen mit num_supervertices-vielen Knoten, erstellt Matrix für _weight_of_superedges mit unendlichen Gewichten
    Supergraph(unsigned int num_supernodes);

    //todo: checken, was ich hier const machen kann, löschen kann
    Graph& this_graph(); //löschen?
    const Graph& get_this_graph() const;
    std::vector<Graph::EdgeId>& original_edge_ids();//löschen?
    const std::vector<Graph::EdgeId>& get_original_edge_ids() const;
    std::vector< std::vector<Graph::EdgeWeight> >& weight_of_superedges();

    //? Laufzeit: //bemerke, dass wir hier beim Hinzufügen der vertikalen Kanten nicht prüfen müssen,
    // ob im Supergraph bereits eine Kante mit den gleichen Endpunkten existiert,
    // da hier die ersten vertikalen boundary edges betrachtet werden (und für jedes crucial child nur eine boundary edge)
    void add_edge(Graph::NodeId node_a, Graph::NodeId node_b, Graph::EdgeWeight weight, Graph::EdgeId original_id);

private:

    Supergraph(const Graph& super_graph);

    // der eigentliche Supergraph
    Graph _this_graph;

    //SVE: speichert zu jeder Kante des Supergraphen die EdgeId der zugehörigen Kante im ursprünglichen Graphen
    //KVE: speichert zu jeder Kante des Supergraphen die EdgeId der zugehörigen boundary edge
    std::vector<Graph::EdgeId> _original_edge_ids;

    // 'Matrix', die zu jedem Knotenpaar das Gewicht der Kante speichert, die zwischen diesen Knoten verläuft (bzgl des Supergraphen)
    // falls keine Kante existiert, dann wird unendliches Gewicht gespeichert
    //? Eintrag [i][j] entspricht Knotenpaar i+1, j (wobei i >= j)
    std::vector< std::vector<Graph::EdgeWeight> > _weight_of_superedges;

};

class Supergraph_KVE: public Supergraph{

public:

    enum BoundEdgeType {newly_found, original};

    Supergraph_KVE(unsigned int num_supernodes);

    std::vector<Supergraph_KVE::BoundEdgeType>& superedges_corresponding_to_new_bound_edges();
    const std::vector<Supergraph_KVE::BoundEdgeType>& get_superedges_corresponding_to_new_bound_edges() const; //löschen?

    void add_edge(Graph::NodeId node_a, Graph::NodeId node_b, Graph::EdgeWeight weight, Graph::EdgeId original_id, BoundEdgeType b_e_type);

private:

    // speichert 1, gdw. die entsprechende Kante des Supergraphen eine neue boundary edge ist (also eine nach repair gefundene)
    std::vector<BoundEdgeType> _superedges_corresponding_to_new_bound_edges;

};

#endif //PRAKTIKUMSTEINERBAUM_SUPERGRAPH_H
