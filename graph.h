//
// Created by Lukas on 30.05.2022.
//

#ifndef PRAKTIKUMSTEINERBAUM_GRAPH_H
#define PRAKTIKUMSTEINERBAUM_GRAPH_H

#include "vector"
#include "string"


class Graph {
public:

    using NodeId = unsigned int;
    using EdgeId = unsigned int;
    using NodeName = unsigned int;
    using EdgeWeight = double;
    using PathLength = double;

    enum TerminalState {no_terminal , terminal};
    enum DirType {directed , undirected };


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

        //richtet die Kante entsprechend den Eingabeknoten
        void direct_edge(NodeId head, NodeId tail);

        //gibt die Knoten der Kanten in aufsteigender (entsprechend NodeId) Reihenfolge aus
        std::pair<NodeId, NodeId> get_nodes_orderedbyid();

    private:
        //Stelle, an der die Kante in _edges steht
        //? const machen
        EdgeId _edge_id;

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
        const std::vector<EdgeId>& incident_edge_ids() const;
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
        bool check_if_terminal() const;

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
        std::vector<EdgeId> _incident_edge_ids;

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

    //fügt einen Knoten zum Graphen hinzu (!prüft aber nicht ob _node_name eindeutig!)
    void add_one_node(NodeName name, TerminalState t);
    //fügt existierenden Knoten zum Graphen hinzu (!prüft aber nicht ob _node_name eindeutig!)
    //(NodeId von v wird nicht verändert und muss deshalb der Anzahl der Knoten des Graphen (vor Aufruf der Funktion) entsprechen)
    void add_one_existing_node(const Node new_node);
    //entspricht add_one_existing_node, hier wird aber die NodeId des Eingabeknotens einfach angepasst
    //todo: diese Fktn ggf. an ein paar Stellen eher verwenden als die anderen
    void add_one_existing_node_w_newid(const Node new_node);
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
    //gibt Referenz auf Kante mit EdgeId e aus
    const Edge& get_edge(EdgeId e) const;

    const std::vector<EdgeId>& incident_edge_ids(NodeId input_node) const;
    //gibt alle zum Eingabeknoten inzidenten Kanten aus ??
    //std::vector<const EdgeId&> incident_edges(NodeId input_node) const;
    //gibt Nachbarknoten des Eingabeknotens aus
    std::vector<NodeId> adjacency_vect(NodeId input_node_id) const;

    //gibt Vektor mit allen eingehenden Kanten des Eingabeknotens aus
    std::vector<EdgeId> get_ingoing_edge_ids(NodeId input_node_id) const;
    //gibt Vektor mit allen ausgehenden Kanten des Eingabeknoten aus
    std::vector<EdgeId> get_outgoing_edge_ids(NodeId input_node_id) const;
    //gibt Vektor mit allen Nachbarn aus, die zu eingehenden Kanten gehören
    std::vector<NodeId> get_ingoing_neighbors(NodeId input_node_id) const;
    //gibt Vektor mit allen Nachbarn aus, die zu eingehenden Kanten gehören
    std::vector<NodeId> get_outgoing_neighbors(NodeId input_node_id) const;
    //? beachte, dass Laufzeit von ingoing_neighbors, outgoing_neighbors jeweils O( grad(input_node) ) ist



    // nach Edge verschieben? Problem: Edge weiß nicht, ob Graph gerichtet
    //gibt node_a der Eingabekante aus, Graph muss gerichtet sein
    NodeId get_tail(EdgeId input_edge_id) const;
    //gibt node_b der Eingabekante aus, Graph muss gerichtet sein
    NodeId get_head(EdgeId input_edge_id) const;

    DirType dir_type() const;
    //macht den Graph zu einer gerichteten Arboreszenz mit Wurzel entsprechend der Eingabe,
    // in dem Sinne, dass die Knoten in den Kanten so gespeichert werden, dass node_a dem tail und node_b dem head entspricht
    //Eingabe: Graph muss Baum sein
    //Ausgabe: Knoten des Graphen in einer 'post-order' bzgl der Arboreszenz (neben der Modifizierung des Graphen)
    void make_rooted_arborescence(NodeId root_id);


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

    //gibt an, ob Graph gerichtet ist (default ist ungerichtet)
    DirType _dir_type;

    std::string instance_name, instance_creator, instance_remark, instance_problem; //sind nur für die Instanz, kommen aus dem SteinLib-Format
};


#endif //PRAKTIKUMSTEINERBAUM_GRAPH_H
