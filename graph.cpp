//
// Created by Lukas on 30.05.2022.
//

#include <iostream>
#include <sstream>
#include <fstream>
#include <limits>
#include <utility>

#include "graph.h"
#include "graph_aux_functions.h"

const Graph::EdgeWeight Graph::infinite_weight = std::numeric_limits<double>::max();
const Graph::PathLength Graph::infinite_length = std::numeric_limits<double>::max();
const Graph::NodeId Graph::invalid_node_id = std::numeric_limits<unsigned int>::max();
const Graph::EdgeId Graph::invalid_edge_id = std::numeric_limits<unsigned int>::max();

Graph::Graph(int n)
{
    Graph::add_nodes(n);
    //instance_name etc nicht definiert
}

Graph::Edge::Edge(Graph::EdgeId id, Graph::NodeId a, Graph::NodeId b, Graph::EdgeWeight w):
        _edge_id(id),
        _node_a(a),
        _node_b(b),
        _weight(w)
{}

Graph::Node::Node(Graph::NodeId id, Graph::NodeName name, Graph::TerminalState t):
        _node_id(id),
        _node_name(name),
        _terminal_state(t)
{}

unsigned int Graph::num_nodes() const{
    return _nodes.size();
}

unsigned int Graph::num_edges() const{
    return _edges.size();
}

Graph::EdgeId Graph::Edge::edge_id() const {
    return _edge_id;
}

std::vector<Graph::Edge> Graph::edges() const{
    return _edges;
}

Graph::NodeId Graph::Edge::node_a() const{
    return _node_a;
}

Graph::NodeId Graph::Edge::node_b() const{
    return _node_b;
}

Graph::EdgeWeight Graph::Edge::weight() const{
    return _weight;
}

bool Graph::Edge::check_incident(Graph::NodeId c) const{
    if( c==_node_a || c==_node_b ){
        return true;
    }else{
        return false;
    }
}

Graph::NodeId Graph::Edge::get_other_node(Graph::NodeId curr_node)  const{
    if( not check_incident(curr_node) ) {
        std::cout << "curr_node = " << curr_node << "; 'diese' Kante hat EdgeId " << _edge_id << "\n";
        throw std::runtime_error("(Graph::Edge::get_other_node) curr_node ist nicht mit dieser Kante inzident.");
    }
    if(curr_node != _node_a){
        return _node_a;
    }
    if(curr_node != _node_b){
        return _node_b;
    }
    throw std::runtime_error("(Graph::Edge::get_other_node) Kante ist Schleife von curr_node");
}

void Graph::Edge::reverse_node_order() {
    Graph::NodeId var_node = _node_a;
    _node_a = _node_b;
    _node_b = var_node;
}

std::pair<Graph::NodeId, Graph::NodeId> Graph::Edge::get_nodes_orderedbyid() {
    if(_node_a < _node_b){
        return {_node_a, _node_b};
    }else if(_node_a > _node_b){
        return {_node_b, _node_a};
    }else{
        std::cout << "Fehler bei Kante mit EdgeId " << _edge_id << "\n";
        throw std::runtime_error("(Graph::Edge::get_nodes_orderedbyid) Kante ist Schleife ");
    }
}

void Graph::print() const{

    std::cout << "SECTION Graph \n";
    std::cout << "Nodes " << num_nodes() << "\n";
    std::cout << "Edges " << num_edges() << "\n";
    for(auto var_edge : edges()){
        std::cout << "E ";
        print_nodename(var_edge.node_a());
        std::cout << " ";
        print_nodename(var_edge.node_b());
        std::cout << " ";
        print_edgeweight( var_edge.weight() );
        std::cout << "\n";
    }
    std::cout << "\n";

    std::cout<< "SECTION Terminals \n";
    std::vector<Graph::NodeId> vect_term = get_vect_term();
    std::cout << "Terminals " << vect_term.size() << "\n";
    for (unsigned int id : vect_term){
        std::cout << "T ";
        print_nodename(id);
        std::cout << "\n";
    }
    std::cout << "\n";

}

void Graph::print_by_id() const{
    std::cout << "print_by_id \n";

    std::cout << "SECTION Graph \n";
    std::cout << "Nodes " << num_nodes() << "\n";
    std::cout << "Edges " << num_edges() << "\n";
    for (auto var_edge : edges()){
        std::cout << "E ";
        print_nodeid( var_edge.node_a() );
        std::cout << " ";
        print_nodeid( var_edge.node_b() );
        std::cout << " ";
        print_edgeweight( var_edge.weight() );
        std::cout << "\n";
    }
    std::cout << "\n";

    std::cout<< "SECTION Terminals \n";
    std::vector<Graph::NodeId> vect_term = get_vect_term();
    std::cout << "Terminals " << vect_term.size() << "\n";
    for (unsigned int id : vect_term){
        std::cout << "T ";
        print_nodeid(id);
        std::cout << "\n";
    }
    std::cout << "\n";

}

// ? theoretisch müsste ich das in alle print Funktionen einfügen (oder brauche ich die Fktn nicht?)
void Graph::print_nodename(Graph::NodeId id) const{
    if(id == Graph::invalid_node_id){
        std::cout << " \" invalid_node \" ";
    }else if(num_nodes() <= id) {
        throw std::runtime_error("(Graph::print_nodename) Knoten mit NodeId id liegt nicht im Graph");
    }else{
        get_node(id).print_nodename();
    }
}

void Graph::print_edge_as_pair(Graph::EdgeId id) const {
    if(id < num_edges()) {
        std::cout << "(";
        Graph::print_nodename(get_edge(id).node_a() ),
        std::cout << ",";
        Graph::print_nodename( get_edge(id).node_b() ),
        std::cout << ")";
    } else if(id == Graph::invalid_edge_id) {
        std::cout << " 'invalid_edge' ";
    } else {
        throw std::runtime_error("(Graph::print_edge_as_pair) Kante mit EdgeId id nicht im Graph");
    }
}

void Graph::Node::set_terminal(Graph::TerminalState t){
    _terminal_state = t;
}

bool Graph::Node::check_terminal() const{
    if(_terminal_state == Graph::terminal){
        return true;
    }
    return false;
}

Graph::TerminalState Graph::Node::terminal_state() const {
    return _terminal_state;
}

void Graph::set_terminal(Graph::NodeId v, Graph::TerminalState t){
    if(v < num_nodes()){
        set_terminal(v,t);
    }else{
        std::cout << "v=" << v << "\n";
        throw std::runtime_error("Der Knoten v ist nicht im Graphen.");
    }
}

std::vector<Graph::NodeId> Graph::get_vect_term() const{
    std::vector<Graph::NodeId> output;
    for(Graph::NodeId i = 0; i<num_nodes(); i++){
        if(get_node(i).check_terminal()){
            output.push_back(i);
        }
    }
    return output;
}

void Graph::add_one_node(Graph::NodeName name, Graph::TerminalState t){
    NodeId id = num_nodes();
    Graph::Node new_node(id, name, t);
    add_one_existing_node(new_node);
}

void Graph::add_one_existing_node(const Graph::Node& new_node){
    Graph::NodeId id = new_node.node_id();
    unsigned int new_n = num_nodes();

    if(id != new_n){
        std::cout << "Knoten mit NodeId " << id << " kann nicht hinzugefuegt werden, ";
        std::cout << "da seine NodeId ungleich der neuen Anzahl an Knoten ist (" << new_n << ") \n";
        throw std::runtime_error("Funktion add_one_existing_node");
    }

    _nodes.push_back(new_node);
}

void Graph::add_nodes(int num_new_nodes){
    if(num_new_nodes<0){
        throw std::runtime_error("(Graph::add_nodes) num_new_nodes<0");
    }

    unsigned int num_old_nodes = num_nodes();
    for(unsigned int i=0; i< (unsigned) num_new_nodes; i++){
        //_node_name wird einfach auf "1 + Stelle in _nodes, wo der neue Knoten gespeichert wird" gesetzt
        Graph::add_one_node( (Graph::NodeName) num_old_nodes +i +1, Graph::no_terminal);
    }
}


Graph::NodeId Graph::Node::node_id() const{
    return _node_id;
}

/*  ?
void Graph::Node::set_node_id(Graph::NodeId new_id){
    _node_id = new_id;
}
*/

Graph::NodeName Graph::Node::node_name() const{
    return _node_name;
}

void Graph::Node::set_node_name(Graph::NodeName new_name){
    _node_name = new_name;
}

/*  ?? siehe Header-Datei
Graph::Node& Graph::get_node(Graph::NodeId v) const{
    if(v >= num_nodes() ){
        throw std::runtime_error("(Graph::Node& Graph::get_node) Knoten v liegt nicht im Graphen");
    }
    return _nodes[v];
}
 */

std::vector<Graph::Node> Graph::nodes() const {
    return _nodes;
}

const Graph::Node& Graph::get_node(Graph::NodeId v) const{
    if(v >= num_nodes() ){
        throw std::runtime_error("(const Graph::Node& Graph::get_node) Knoten v liegt nicht im Graphen");
    }
    return _nodes[v];
}

std::vector<Graph::EdgeId> Graph::Node::incidence_vect() const{
    return _incidence_vect;
}

void Graph::Node::add_neighbor_edge(EdgeId e){
    _incidence_vect.push_back(e);
}

unsigned int Graph::Node::num_neighbors()  const{
    return _incidence_vect.size();
}

void Graph::Node::print_nodename() const {
    std::cout << node_name();
}

//? nicht getestet
void Graph::print_incidence_vect(Graph::NodeId id) const{
    const Graph::Node& v = get_node(id);
    std::cout << "Knoten ";
    print_nodename(id);
    std::cout << " ist zu " << v.num_neighbors() << " Kanten inzident: \n";
    for(auto curr_edgeid : v.incidence_vect() ){
        Graph::Edge curr_edge = get_edge(curr_edgeid);
        std::cout << "E ";
        print_nodename( curr_edge.node_a() );
        std::cout << " ";
        print_nodename( curr_edge.node_b() );
        std::cout << " ";
        print_edgeweight( curr_edge.weight() );
        std::cout << "\n";
    }
}

//? nicht getestet
void Graph::print_incidence_vect_by_id(Graph::NodeId id) const{
    const Graph::Node& v = get_node(id);
    std::cout << "Knoten mit NodeId";
    print_nodeid(id);
    std::cout << " ist zu " << v.num_neighbors() << " Kanten inzident: (hier werden die _node_id's verwendet) \n";
    for(auto curr_edgeid : v.incidence_vect() ){
        Graph::Edge curr_edge = get_edge(curr_edgeid);
        std::cout << "E ";
        print_nodeid( curr_edge.node_a() );
        std::cout << " ";
        print_nodeid( curr_edge.node_b() );
        std::cout << " ";
        print_edgeweight( curr_edge.weight() );
        std::cout << "\n";
    }
}

void Graph::print_all_incidence_vect_by_id() const{
    for(unsigned int i=0; i<num_nodes(); i++){
        print_incidence_vect_by_id((Graph::NodeId) i);
    }
}

const Graph::Edge& Graph::get_edge(Graph::EdgeId e) const {
    if(e >= num_edges() ){
        throw std::runtime_error("(const Graph::Edge& Graph::get_edge) Kante e liegt nicht im Graphen");
    }
    return _edges[e];
}

void Graph::add_existing_edge(const Edge& e){
    Graph::NodeId a = e.node_a();
    Graph::NodeId b = e.node_b();
    Graph::EdgeId id = e.edge_id();
    unsigned int new_m = num_edges();

    if(a >= Graph::num_nodes()){
        std::cout << "Kante (" << a << "," << b << ") kann nicht hinzugefuegt werden, da " << a  << " kein Knoten ist. \n";
        throw std::runtime_error("Funktion add_existing_edge");
    }
    if(b >= Graph::num_nodes()){
        std::cout << "Kante (" << a << "," << b << ") kann nicht hinzugefuegt werden, da " << b  << " kein Knoten ist. \n";
        throw std::runtime_error("Funktion add_existing_edge");
    }

    if(id != new_m){
        std::cout << "Kante (" << a << "," << b << ") mit EdgeId " << id << " kann nicht hinzugefuegt werden, ";
        std::cout << "da ihre EdgeId ungleich der neuen Anzahl an Kanten ist (" << new_m << ") \n";
        throw std::runtime_error("Funktion add_existing_edge");
    }

    if(e.weight() == Graph::infinite_weight){
        //ist sowas debug?
        std::cout << "(Funktion Graph::add_existing_edge) Warnung: Kante mit unendl. Gewicht hinzugefuegt. \n";
    }

    //Hinzufügen zur Kantenliste des Graphen
    _edges.push_back(e);

    //Hinzufügen zur Kanten(pointer)liste der Knoten
    _nodes[a].add_neighbor_edge(id);
    _nodes[b].add_neighbor_edge(id);
}

void Graph::add_edge(Graph::NodeId a, Graph::NodeId b, Graph::EdgeWeight w){
    EdgeId id = num_edges();
    //Erstellen der Kante
    Edge e(id, a, b, w);
    //Hinzufügen der Kante zum Graphen (siehe add_existing_edge)
    add_existing_edge(e);
}

void Graph::add_existing_edge_w_newid(Graph::Edge new_edge) {
    add_edge(new_edge.node_a(), new_edge.node_b(), new_edge.weight());
}

bool Graph::edgeweight_nonnegative() const{
    /* ??
    if( std::any_of(_edges, weight() < 0){
        return false
    } else {
        return true
    }
     */
    for(auto var_edge : edges()){
        if(var_edge.weight() < 0){
            return false;
        }
    }
    return true;
}

bool Graph::edgeweight_finite() const{
    for(auto var_edge : edges()){
        if( var_edge.weight() == Graph::infinite_weight ){
            return false;
        }
    }
    return true;
}

bool Graph::check_if_simple() const{
    // adjacency_matrice[i][j] entspricht Knotenpaar i+1, j (wobei i >= j)
    std::vector< std::vector<bool> > adjacency_matrice;

    //Initialisieren
    for(unsigned int i = 0; i < num_nodes()-1; i++){
        std::vector<bool> var_bool_vector (i+1, false);
        adjacency_matrice.push_back(var_bool_vector);
    }

    //ausfüllen entsprechend der Kanten des Graphen und prüfen, ob Kante (bzw. Knotenpaar) doppelt
    for(auto curr_edge : edges() ){
        std::pair<Graph::NodeId, Graph::NodeId> curr_edge_nodes = curr_edge.get_nodes_orderedbyid();
        if( adjacency_matrice[ curr_edge_nodes.second-1 ][ curr_edge_nodes.first ] ){
            std::cout << "(check_if_simple) Kante (bzw. Knotenpaar) ";
            print_edge_as_pair( curr_edge.edge_id() );
            std::cout << " ist doppelt. \n";
            return false;
        }
        adjacency_matrice[ curr_edge_nodes.second-1 ][ curr_edge_nodes.first ] = true;
    }

    return true;
}

void Graph::instance_comment() const{
    std::cout << "SECTION Comment \n";
    std::cout << "Name   " << instance_name << "\n";
    std::cout << "Creator   " << instance_creator << "\n";
    std::cout << "Remark   " << instance_remark << "\n";
    std::cout << "Problem   " << instance_problem << "\n";
    std::cout << "END \n";
}

Graph::Graph(char const* filename){

    std::ifstream file(filename);
    if (not file) {
        throw std::runtime_error("Fehler beim Öffnen der Datei");
    }

    //Werte für die Variablen instance_name, instance_creator, instance_remark, instance_problem
    //falls diese nicht durch die Datei filename gegeben werden
    instance_name = "no name";
    instance_creator = "no creator";
    instance_remark = "no remark";
    instance_problem = "no problem specified";

    std::string line;
    getline(file, line);	//einlesen 1. Zeile
    //prüfe, ob die 1. Zeile dem Format entspricht
    std::string firstline = "33D32945 STP File, STP Format Version 1.0";
    if(firstline != line){
        throw std::runtime_error("Nicht das SteinLib-Format (1.Zeile)");
    }

    unsigned int m, t_num;	//m ist Anzahl Kanten, t_num ist Anzahl Terminale
    //Einlesen des Dokuments, erste Fallunterscheidungen nach SECTION
    while (getline(file, line)) {
        std::string var;
        if(line == "SECTION Comment"){

            while (getline(file, line)){

                if(line == "END"){
                    //Ende der Section erreicht
                    break;
                }

                //Einlesen von instance_name, instance_creator, instance_remark, instance_problem
                var = line.substr(0,4);
                if(var == "Name"){
                    instance_name = line.substr(4, line.length());
                }
                var = line.substr(0, 7);
                if(var == "Creator"){
                    instance_creator = line.substr(7, line.length());
                }
                var = line.substr(0, 6);
                if(var == "Remark"){
                    instance_remark = line.substr(6, line.length());
                }
                var = line.substr(0, 7);
                if(var == "Problem"){
                    instance_problem = line.substr(7, line.length());
                }

            }

        }

        if(line == "SECTION Graph"){

            //Zeile mit "Nodes"
            getline(file, line);
            var = line.substr(0,5);
            //prüfe ob Eingabeformat korrekt
            if(var != "Nodes"){
                throw std::runtime_error("Eingabeformat: 1. Zeile in Section Graph muss 'Nodes' enthalten");
            }
                //füge Knoten zum Graphen hinzu
            else{
                var = line.substr(5, line.length());
                std::stringstream ssn(var);
                int n; //Anzahl der Knoten
                ssn >> n;
                add_nodes(n);
            }

            //Zeile mit "Edges"
            getline(file, line);
            var = line.substr(0,5);
            if(var == "Edges"){
                var = line.substr(5, line.length());
                std::stringstream ssm(var);
                ssm >> m;
                // m wird nur einmal unten verwendet, um zu prüfen, ob die Eingabedatei korrekt ist
            }
            else{
                throw std::runtime_error("Eingabeformat: 2. Zeile in Section Graph muss 'Edges' enthalten");
            }

            //Kanten einlesen
            while (getline(file, line)){
                if(line == "END"){
                    //Ende der Section erreicht
                    break;
                }

                var = line.substr(0,1);
                //teste ob SteinLib-Format eingehalten wird
                if(var!="E"){
                    throw std::runtime_error("Eingabeformat: Eine Zeile in der Graph-Section (nach 'Nodes', 'Edges') beginnt nicht mit 'E' ");
                }
                    //Kante hinzufügen
                else{
                    var = line.substr(1, line.length());
                    std::stringstream sse(var);
                    Graph::NodeId a, b;
                    Graph::EdgeWeight w;
                    sse >> a >> b >> w;
                    a--;    //bemerke "-1", da SteinLib bei 1 anfängt die Knoten zu zählen
                    b--;
                    add_edge(a, b, w);
                }
            }

            //der Abschnitt zu "arcs" wird hier nicht betrachtet
        }

        if(line == "SECTION Terminals"){
            //Zeile mit "Terminals"
            getline(file, line);
            var = line.substr(0,9);
            if(var == "Terminals"){
                var = line.substr(9, line.length());
                std::stringstream sstn(var);
                sstn >> t_num;
                // t_num wird nur einmal unten verwendet, um zu prüfen, ob die Eingabedatei korrekt ist
            }
            else{
                throw std::runtime_error("Eingabeformat: 1. Zeile in Section Terminals muss 'Terminals' enthalten");
            }

            //Terminale einlesen
            while (getline(file, line)){
                if(line == "END"){
                    //Ende der Section erreicht
                    break;
                }

                var = line.substr(0,1);
                //prüfe ob Eingabeformat korrekt
                if(var!="T"){
                    throw std::runtime_error("Eingabeformat: Eine Zeile in der Terminals-Section (nach 'Terminals') beginnt nicht mit T");
                }
                    //markiere Knoten als Terminal
                else{
                    var = line.substr(1, line.length());
                    std::stringstream sst(var);
                    Graph::NodeId v;
                    sst >> v;
                    v--;    //bemerke "-1", da SteinLib bei 1 anfängt die Knoten zu zählen
                    set_terminal(v, Graph::terminal);
                }
            }

            //mögliche Zeilen mit RootP, Root, TP werden nicht betrachtet
        }

        if(line == "EOF"){
            //Ende der Datei erreicht
            break;
        }

        //die Sections "MD" (MaximumDegrees) und D{n} (Coordinates) werden nicht betrachtet

    }

    //prüfen, ob angegebene Kanten-/Terminalanzahl mit aufgelisteten Kanten/Terminalen übereinstimmt
    if( num_edges() != m){
        throw std::runtime_error("(Einlesen) angegebene Kantenanzahl ungleich Anzahl aufgelisteter Kanten");
    }
    if( get_vect_term().size() != t_num){
        throw std::runtime_error("(Einlesen) angegebene Terminalanzahl ungleich Anzahl aufgelisteter Terminale");
    }

}

/*
Graph::Graph(const Graph& g) {
    _nodes = g.nodes();
    _edges = g.edges();
    //instance_name etc nicht definiert
}
*/