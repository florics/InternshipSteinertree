//
// Created by Lukas on 30.05.2022.
//

#ifndef PRAKTIKUMSTEINERBAUM_GRAPH_H
#define PRAKTIKUMSTEINERBAUM_GRAPH_H

#include "vector"
#include "string"

//todo manchmal schreibe ich Graph::Edge und manchmal Edge, fehlende Konsistenz!

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
        //gibt die Knoten der Kanten in aufsteigender (entsprechend NodeId) Reihenfolge aus
        std::pair<NodeId, NodeId> get_nodes_orderedbyid();

    private:
        //Stelle, an der die Kante in _edges steht
        const EdgeId _edge_id;

        //die Knoten, mit denen die Kante inzident ist; die Werte sind hier die Stellen, an denen die Knoten in _nodes stehen
        NodeId _node_a;
        NodeId _node_b;

        //Kantengewicht
        EdgeWeight _weight;
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

        //Hilfsfunktion zur Ausgabe von Knoten auf Konsole (gibt _nodename oder 'invalid_node' aus) (löschen?)
        void print_nodename() const;

    private:
        //Stelle, an dem Knoten in _nodes gespeichert ist
        // const NodeId _node_id; ?? das hat irgendwie ganz große Probleme gemacht,
        // als ich Voronoi_diagram.cpp und graph_algorithms.cpp einbinden wollte (in CMake, add_executable)
        NodeId _node_id;

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

    Graph(int n);

    //copy-constructor
    //Graph(const Graph& g);

    unsigned int num_nodes() const;
    unsigned int num_edges() const;

    //todo: auslagern
    //gibt Graph auf Konsole aus, Knoten werden über _node_name ausgegeben
    void print() const;
    //gibt Graph auf Konsole aus, Knoten werden über _node_id ausgegeben
    void print_by_id() const;
    //gibt Inzidenz-Vektor auf Konsole aus, Knoten werden als _node_name ausgegeben
    void print_incidence_vect(NodeId id) const;
    //gibt Inzidenz-Vektor auf Konsole aus, Knoten werden als _node_id ausgegeben
    void print_incidence_vect_by_id(NodeId id) const;
    //gibt alle Inzidenz-Vektor auf Konsole aus, Knoten werden als _node_id ausgegeben
    void print_all_incidence_vect_by_id()  const;
    //Hilfsfunktion zur Ausgabe von Knoten auf Konsole (gibt _nodename oder 'invalid_node' aus) (löschen?)
    void print_nodename(NodeId id) const;
    // Hilfsfunktion zur Ausgabe von Kanten als Knotenpaar auf Konsole (Paar von NodeName's)
    void print_edge_as_pair(EdgeId id) const;

    //fügt einen Knoten zum Graphen hinzu (!prüft aber nicht ob _node_name eindeutig!)
    void add_one_node(NodeName name, TerminalState t);
    //fügt existierenden Knoten zum Graphen hinzu (!prüft aber nicht ob _node_name eindeutig!)
    //(NodeId von v wird nicht verändert und muss deshalb der Anzahl der Knoten des Graphen (vor Aufruf der Funktion) entsprechen)
    void add_one_existing_node(const Node& new_node);
    //fügt n Knoten hinzu (Knoten sind keine Terminale, _node_name wird einfach auf "1 + Stelle in _nodes, wo der neue Knoten gespeichert wird" gesetzt)
    void add_nodes(int num_new_nodes);

    //erstellt Kante entsprechend der Eingabe und fügt sie zum Graphen hinzu
    void add_edge(NodeId a, NodeId b, EdgeWeight w);
    //fügt existierende Kante zu Graph hinzu (EdgeId von new_edge wird nicht verändert und muss deshalb der Anzahl der Kanten des Graphen (vor Aufruf der Fktn) entsprechen)
    void add_existing_edge(const Edge& new_edge);
    //fügt existierende Kante zu Graph hinzu, wobei die EdgeId angepasst wird //? eher unnötig: ggf. löschen!
    void add_existing_edge_w_newid(Edge new_edge);


    //macht den Knoten v zu einem Terminal oder zu einem Nicht-Terminal
    void set_terminal(NodeId v, TerminalState t);
    //gibt Vektor mit allen Terminalen aus, in dem Vektor stehen jeweils die Stellen, an denen die Terminale in _nodes gespeichert sind, also die NodeId's
    std::vector<NodeId> get_vect_term() const;

    //Graph::Node& get_node(NodeId v) const; //brauche ich das überhaupt? direkter zugriff auf knoten v mit _nodes[v] !?

    std::vector<Node> nodes() const;
    //gibt Referenz auf Knoten mit NodeId v aus
    const Node& get_node(NodeId v) const;

    std::vector<Edge> edges() const;
    //gibt Referenz auf Kante mit EdgeId v aus
    const Edge& get_edge(EdgeId e) const;

    //todo: auslagern
    //gibt 1 aus gdw. nicht-negative Kantengewichte vorliegen
    bool edgeweight_nonnegative() const;
    //gibt 1 aus gdw. alle Kanten Gewicht (im Betrag) < infinite_weight haben
    bool edgeweight_finite() const;
    //gibt 1 aus gdw. Graph einfach
    bool check_if_simple() const;

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
