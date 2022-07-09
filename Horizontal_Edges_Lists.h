//
// Created by Lukas on 05.07.2022.
//

#ifndef PRAKTIKUMSTEINERBAUM_HORIZONTAL_EDGES_LISTS_H
#define PRAKTIKUMSTEINERBAUM_HORIZONTAL_EDGES_LISTS_H

#include "vector"

#include "graph.h"
#include "voronoi_diagram.h"
#include "Subgraph.h"

// Hilfsstruktur für die Listen mit horizontalen (boundary) Kanten in Steiner- bzw. Key-Vertex-Elimination
// die Listen werden als Vektoren gespeichert, in dem Bezeichnungen & Kommentaren wird aber 'Liste' verwendet
// (als Abkürzung für 'Vektor mit horizontalen Kanten'

class Horizontal_Edges_Lists {

public:

    using ListId = unsigned int;

    Horizontal_Edges_Lists(const std::vector<ListId>& list_ids, const std::vector< std::vector<Graph::EdgeId> >& vect_of_lists);


    //Konstruktor für Steiner-Vertex-Elimination
    // nodes_to_process sind die Knoten, für die eine Liste angelegt werden soll??
    //Horizontal_Edges_Lists(const Subgraph& input_subgraph, Graph::NodeId root, const std::vector<Graph::NodeId>& nodes_to_process);

    //Konstruktor für Key-Vertex-Elimination
    // nodes_to_process sind die Knoten, für die eine Liste angelegt werden soll??
    //Horizontal_Edges_Lists(const Subgraph& input_subgraph, Graph::NodeId root,
      //                     const std::vector<Graph::NodeId>& nodes_to_process, const Voronoi_diagram& vor_diag);

    // gibt die dem Eingabeknoten entsprechende Liste aus
    const std::vector<Graph::EdgeId>& get_list(Graph::NodeId input_node) const;


    static const ListId no_list_available;

private:

    // speichert für jeden Knoten (der aktuellen Lösung), für den eine Liste angelegt wurde,
    // die Stelle, an der diese Liste in _vect_of_lists steht
    const std::vector<ListId> _list_ids;

    // speichert die Listen mit den horizontalen Kanten
    const std::vector< std::vector<Graph::EdgeId> > _vect_of_lists;

};


#endif //PRAKTIKUMSTEINERBAUM_HORIZONTAL_EDGES_LISTS_H
