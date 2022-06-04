//
// Created by Lukas on 30.05.2022.
//

#ifndef PRAKTIKUMSTEINERBAUM_GRAPH_H
#define PRAKTIKUMSTEINERBAUM_GRAPH_H

#include <list>
#include <vector>
#include <string>


class Graph {
public:
    using NodeId = unsigned int;
    using EdgeId = unsigned int;
    using NodeName = unsigned int;
    using EdgeWeight = double;
    using PathLength = double;
    enum TerminalState {no_terminal , terminal};


    class Edge{
    public:
        Edge(EdgeId id, NodeId a, NodeId b, EdgeWeight w);

        EdgeId edge_id() const;
        NodeId node_a() const;
        NodeId node_b() const;
        EdgeWeight weight() const;

        //prüft, ob Knoten c mit Kante inzident ist (genau in dem Fall wird 1 ausgegeben)
        bool check_incident(NodeId c) const;
        //gibt Knoten aus, der zur Kante inzident ist, aber nicht curr_node ist
        NodeId get_other_node(NodeId curr_node) const;
        //kehrt die Reihenfolge um, in der die zur Kante inzidenten Knoten gespeichert sind (also _node_a, _node_b)
        void reverse_node_order();

    private:
        //Stelle, an der die Kante in _edges steht
        const EdgeId _edge_id;

        //die Knoten, mit denen die Kante inzident ist; die Werte sind hier die Stellen, an denen die Knoten in _nodes stehen
        NodeId _node_a; //const?
        NodeId _node_b;

        //Kantengewicht
        EdgeWeight _weight; //const?
    };

    class Node{
    public:
        Node(NodeId id, NodeName name, TerminalState t);

        NodeId node_id() const;
        NodeName node_name() const;
        std::vector<EdgeId> incidence_vect() const;
        TerminalState terminal_state() const;

        //setzt _node_id auf einen neuen Wert (!prüft aber nicht, ob dadurch _node_id der Stelle des Knotens in _nodes entspricht)
        //void set_node_id(NodeId new_id); ?

        //setzt _node_name auf einen neuen Wert (!prüft aber nicht, ob dadurch 2 Knoten den gleichen _node_name erhalten)
        void set_node_name(NodeName new_name);

        //fügt Kante zum Inzidenz-Vektor des Knotens hinzu
        void add_neighbor_edge(EdgeId e);
        //gibt die Anzahl an Nachbarn des Knoten aus
        unsigned int num_neighbors() const;

        //macht den entsprechenden Knoten zu einem Terminal oder zu einem Nicht-Terminal
        void set_terminal(TerminalState t);
        //gibt 1 aus, wenn Knoten Terminal ist
        bool check_terminal() const;

    private:
        //Stelle, an dem Knoten in _nodes gespeichert ist
        const NodeId _node_id;

        //Name für den Knoten, wird vor allem für die Ausgabe benutzt, beim Einlesen wird hier genau die Zahl genommen, die im SteinLib_Format verwendet wird
        //nicht eindeutig
        //(in der Regel wird aber über die Stelle, an der der Knoten in _nodes gespeichert ist, auf den Knoten zugegriffen)
        NodeName _node_name;

        //Inzidenz-Vektor: enthält die EdgeId's der mit dem Knoten inzidenten Kanten
        std::vector<EdgeId> _incidence_vect;

        //gibt an, ob der Knoten ein Terminal ist oder nicht
        TerminalState _terminal_state;
    };

    //Graph-constructor mit Einleseroutine
    Graph(char const* filename);

    Graph(unsigned int n);

    unsigned int num_nodes() const;
    unsigned int num_edges() const;

    //gibt Graph auf Konsole aus, Knoten werden über _node_name ausgegeben
    void print() const;
    //gibt Graph auf Konsole aus, Knoten werden über _node_id ausgegeben
    void print_by_id() const;
    //gibt Inzidenz-Vektor auf Konsole aus, Knoten werden als _node_name ausgegeben
    void print_incidence_vect(Graph::NodeId id) const;
    //gibt Inzidenz-Vektor auf Konsole aus, Knoten werden als _node_id ausgegeben
    void print_incidence_vect_by_id(Graph::NodeId id) const;
    //gibt alle Inzidenz-Vektor auf Konsole aus, Knoten werden als _node_id ausgegeben
    void print_all_incidence_vect_by_id()  const;

    //fügt einen Knoten zum Graphen hinzu (!prüft aber nicht ob _node_name eindeutig!)
    void add_one_node(NodeName name, TerminalState t);
    //fügt existierenden Knoten zum Graphen hinzu (!prüft aber nicht ob _node_name eindeutig!)
    void add_one_existing_node(const Graph::Node& v);
    //fügt n Knoten hinzu (Knoten sind keine Terminale, _node_name wird einfach auf "1 + Stelle in _nodes, wo der neue Knoten gespeichert wird" gesetzt)
    void add_nodes(unsigned int num_new_nodes);

    //erstellt Kante entsprechend der Eingabe und fügt sie zum Graphen hinzu
    void add_edge(Graph::NodeId a, Graph::NodeId b, Graph::EdgeWeight w);
    //fügt existierende Kante zu Graph hinzu
    void add_existing_edge(Edge new_edge);

    //macht den Knoten v zu einem Terminal oder zu einem Nicht-Terminal
    void set_terminal(Graph::NodeId v, Graph::TerminalState t);
    //gibt Vektor mit allen Terminalen aus, in dem Vektor stehen jeweils die Stellen, an denen die Terminale in _nodes gespeichert sind, also die NodeId's
    std::vector<Graph::NodeId> get_vect_term() const;

    //Graph::Node& get_node(Graph::NodeId v) const; //brauche ich das überhaupt? direkter zugriff auf knoten v mit _nodes[v] !?

    //gibt Referenz auf Knoten mit NodeId v aus
    const Graph::Node& get_node(Graph::NodeId v) const;

    //gibt Referenz auf Kante mit EdgeId v aus
    const Graph::Edge& get_edge(Graph::EdgeId e) const;
    std::vector<Graph::Edge> edges();

    //gibt 1 aus gdw. nicht-negative Kantengewichte vorliegen
    bool edgeweight_nonnegative() const;
    //gibt 1 aus gdw. alle Kanten Gewicht < infinite_weight haben
    bool edgeweight_finite() const;
    static const EdgeWeight infinite_weight;
    static const PathLength infinite_length;

    static const NodeId invalid_node_id;
    static const EdgeId invalid_edge_id;

    //gibt die Werte der Variablen name, creator, remark, problem im Format der Section "Comment" des SteinLib-Formats
    void instance_comment() const;

private:
    //Vektor, der die Knoten des Graphen speichert
    std::vector <Node> _nodes;

    //Vektor mit allen Kanten des Graphen
    std::vector <Edge> _edges;

    std::string instance_name, instance_creator, instance_remark, instance_problem; //sind nur für die Instanz, kommen aus dem SteinLib-Format
};


#endif //PRAKTIKUMSTEINERBAUM_GRAPH_H
