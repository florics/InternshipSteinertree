//
// Created by Lukas on 30.05.2022.
//

#include <iostream>
#include <sstream>
#include <fstream>
#include <limits>
#include <utility>
#include <array>
#include "graph.h"

const Graph::EdgeWeight Graph::infinite_weight = std::numeric_limits<double>::max();
const Graph::PathLength Graph::infinite_length = std::numeric_limits<double>::max();
const Graph::NodeId Graph::invalid_node_id = std::numeric_limits<unsigned int>::max();
const Graph::EdgeId Graph::invalid_edge_id = std::numeric_limits<unsigned int>::max();

Graph::Graph(int n)
{
    Graph::add_nodes(n);
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
    _node_b = _node_a;
}

std::array<Graph::NodeId, 2> Graph::Edge::get_nodes_orderedbyid() {
    if(_node_a < _node_b){
        return {_node_a, _node_b};
    }else if(_node_a > _node_b){
        return {_node_b, _node_a};
    }else{
        std::cout << "Fehler bei Kante mit EdgeId " << _edge_id << "\n";
        throw std::runtime_error(" (std::array<Graph::NodeId, 2> Graph::Edge::get_nodes_orderedbyid) Kante ist Schleife ");
    }
}

void Graph::print() const{

    std::cout << "SECTION Graph \n";
    std::cout << "Nodes " << num_nodes() << "\n";
    std::cout << "Edges " << num_edges() << "\n";
    for(auto var_edge : _edges){
        Graph::NodeName a = _nodes[var_edge.node_a()].node_name();
        Graph::NodeName b = _nodes[var_edge.node_b()].node_name();
        Graph::EdgeWeight w = var_edge.weight();
        std::cout << "E " << a << " " << b << " " << w << "\n";
    }
    std::cout << "\n";

    std::cout<< "SECTION Terminals \n";
    std::vector<Graph::NodeId> vect_term = get_vect_term();
    std::cout << "Terminals " << vect_term.size() << "\n";
    for (unsigned int i : vect_term){
        std::cout << "T " << _nodes[i].node_name() << "\n";
    }
    std::cout << "\n";

}

void Graph::print_by_id() const{
    std::cout << "print_by_id \n";

    std::cout << "SECTION Graph \n";
    std::cout << "Nodes " << num_nodes() << "\n";
    std::cout << "Edges " << num_edges() << "\n";
    for (auto var_edge : _edges){
        Graph::NodeId a = _nodes[var_edge.node_a()].node_id();
        Graph::NodeId b = _nodes[var_edge.node_b()].node_id();
        Graph::EdgeWeight w = var_edge.weight();
        std::cout << "E " << a << " " << b << " " << w << "\n";
    }
    std::cout << "\n";

    std::cout<< "SECTION Terminals \n";
    std::vector<Graph::NodeId> vect_term = get_vect_term();
    std::cout << "Terminals " << vect_term.size() << "\n";
    for (unsigned int i : vect_term){
        std::cout << "T " << _nodes[i].node_id() << "\n";
    }
    std::cout << "\n";

}

// ? theoretisch müsste ich das in alle print Funktionen einfügen (oder brauche ich die Fktn nicht?)
void Graph::print_node(Graph::NodeId id) const{
    if(id != Graph::invalid_node_id){
        std::cout << id +1;
    }else{
     std::cout << " \" invalid_node \" ";
    }
}

void Graph::print_edgeid(Graph::EdgeId id) const {
    if(id != Graph::invalid_edge_id){
        std::cout << id;
    }else{
        std::cout << " 'invalid_edge' ";
    }
}

void Graph::print_edge_as_pair(Graph::EdgeId id) const {
    if(id < _edges.size()) {
        std::cout << "(";
        Graph::print_node( _edges[ id ].node_a() ),
        std::cout << ",";
        Graph::print_node( _edges[ id ].node_b() ),
        std::cout << ")";
    } else if(id == Graph::invalid_edge_id) {
        std::cout << " 'invalid_edge' ";
    } else {
        std::cout << "Fehler: Kante nicht im Graph.";
    }
}


void Graph::print_pathlength(Graph::PathLength l) const {
    if(l != Graph::infinite_length){
        std::cout << l;
    }else{
        std::cout << " 'infinite_length' ";
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
    if(v < _nodes.size()){
        _nodes[v].set_terminal(t);
    }else{
        std::cout << "v=" << v << "\n";
        throw std::runtime_error("Der Knoten v ist nicht im Graphen.");
    }
}

std::vector<Graph::NodeId> Graph::get_vect_term() const{
    std::vector<Graph::NodeId> output;
    for(unsigned int i = 0; i<num_nodes(); i++){
        if(_nodes[i].check_terminal()){
            output.push_back( (Graph::NodeId) i);
        }
    }
    return output;
}

void Graph::add_one_node(Graph::NodeName name, Graph::TerminalState t){
    NodeId id = _nodes.size();
    Graph::Node v(id, name, t);
    add_one_existing_node(v);
}

void Graph::add_one_existing_node(const Graph::Node& v){
    Graph::NodeId id = v.node_id();
    unsigned int new_n = _nodes.size();

    if(id != new_n){
        std::cout << "Knoten mit NodeId " << id << " kann nicht hinzugefuegt werden, ";
        std::cout << "da seine NodeId ungleich der neuen Anzahl an Knoten ist (" << new_n << ") \n";
        throw std::runtime_error("Funktion add_one_existing_node");
    }

    _nodes.push_back(v);
}

void Graph::add_nodes(int num_new_nodes){
    if(num_new_nodes<0){
        throw std::runtime_error("(Graph::add_nodes) num_new_nodes<0");
    }

    unsigned int num_old_nodes = _nodes.size();
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
    if(v >= _nodes.size() ){
        throw std::runtime_error("(Graph::Node& Graph::get_node) Knoten v liegt nicht im Graphen");
    }
    return _nodes[v];
}
 */

const Graph::Node& Graph::get_node(Graph::NodeId v) const{
    if(v >= _nodes.size() ){
        throw std::runtime_error("(const Graph::Node& Graph::get_node) Knoten v liegt nicht im Graphen");
    }
    return _nodes[v];
}

std::vector<Graph::EdgeId> Graph::Node::incidence_vect() const{
    return _incidence_vect;
}

void Graph::print_incidence_vect(Graph::NodeId id) const{
    const Graph::Node& v = get_node(id); //einfach auf _nodes zugreifen?
    std::vector<Graph::EdgeId> inc_vect = v.incidence_vect();
    unsigned int num_neighb = inc_vect.size();
    std::cout << "Knoten " << v.node_name() << " ist zu " << num_neighb << " Kanten inzident: \n";
    for(unsigned int i = 0; i < num_neighb; i++){
        Graph::Edge curr_edge = _edges[i];
        //finden der _node_name's der zu curr_edge inzidenten Knoten
        const Graph::NodeName& name_a = get_node( curr_edge.node_a() ).node_name(); //einfach auf _nodes zugreifen?
        const Graph::NodeName& name_b = get_node( curr_edge.node_b() ).node_name(); //einfach auf _nodes zugreifen?
        std::cout << "E " << name_a << " " << name_b << " " << curr_edge.weight() << "\n";
    }
}

void Graph::print_incidence_vect_by_id(Graph::NodeId id) const{
    const Graph::Node& v = get_node(id); //einfach auf _nodes zugreifen?
    std::vector<Graph::EdgeId> inc_vect = v.incidence_vect();
    unsigned int num_neighb = inc_vect.size();
    std::cout << "Knoten " << v.node_id() << " ist zu " << num_neighb << " Kanten inzident: (hier werden die _node_id's verwendet) \n";
    for(unsigned int i = 0; i < num_neighb; i++){
        Graph::Edge curr_edge = _edges[i];
        std::cout << "E " << curr_edge.node_a() << " " << curr_edge.node_b() << " " << curr_edge.weight() << "\n";
    }
}

void Graph::print_all_incidence_vect_by_id() const{
    for(unsigned int i=0; i<_nodes.size(); i++){
        print_incidence_vect_by_id((Graph::NodeId) i);
    }
}

void Graph::Node::add_neighbor_edge(EdgeId e){
    _incidence_vect.push_back(e);
}

unsigned int Graph::Node::num_neighbors()  const{
    return _incidence_vect.size();
}

const Graph::Edge& Graph::get_edge(Graph::EdgeId e) const {
    if(e >= _edges.size() ){
        throw std::runtime_error("(const Graph::Edge& Graph::get_edge) Kante e liegt nicht im Graphen");
    }
    return _edges[e];
}

void Graph::add_existing_edge(Edge e){
    Graph::NodeId a = e.node_a();
    Graph::NodeId b = e.node_b();
    Graph::EdgeId id = e.edge_id();
    unsigned int new_m = _edges.size();

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
    EdgeId id = _edges.size();
    //Erstellen der Kante
    Edge e(id, a, b, w);
    //Hinzufügen der Kante zum Graphen (siehe add_existing_edge)
    add_existing_edge(e);
}

void Graph::add_existing_edge_w_newid(Graph::Edge new_edge) {
    add_edge(new_edge.node_a(), new_edge.node_b(), new_edge.weight());
}

bool Graph::edgeweight_nonnegative() const{
    for(auto var_edge : _edges){
        if(var_edge.weight() < 0){
            return false;
        }
    }
    return true;
}

bool Graph::edgeweight_finite() const{
    for(auto var_edge : _edges){
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
        std::vector<bool> var_bool_vector (i+1, 0);
        adjacency_matrice.push_back(var_bool_vector);
    }

    //ausfüllen entsprechend der Kanten des Graphen und prüfen, ob Kante (bzw. Knotenpaar) doppelt
    for(auto curr_edge : edges() ){
        std::array<Graph::NodeId, 2> curr_edge_nodes = curr_edge.get_nodes_orderedbyid();
        if( adjacency_matrice[ curr_edge_nodes[1]-1 ][ curr_edge_nodes[0] ] ){
            std::cout << "(check_if_simple) Kante (bzw. Knotenpaar) ";
            print_edge_as_pair( curr_edge.edge_id() );
            std::cout << " ist doppelt. \n";
            return 0;
        }
        adjacency_matrice[ curr_edge_nodes[1]-1 ][ curr_edge_nodes[0] ] = 1;
    }

    return 1;
}

// ? nicht sicher ob das funzt: die Instanzen der Uni sind alle isomorph laut dieser Methode
bool Graph::check_if_isomorph(const Graph& other_graph) const{
    //Eingabe prüfen
    if(not edgeweight_finite()){
        throw std::runtime_error("(check_if_isomorph) Graph hat unendliche Kantengewichte");
    }
    if(not other_graph.edgeweight_finite()){
        throw std::runtime_error("(check_if_isomorph) eingegebener Graph hat unendliche Kantengewichte");
    }
    if(not check_if_simple() ){
        throw std::runtime_error("(check_if_isomorph) Graph ist nicht einfach");
    }
    if(not other_graph.check_if_simple() ){
        throw std::runtime_error("(check_if_isomorph) eingegebener Graph ist nicht einfach");
    }

    //leichte Checks
    if( num_nodes() != other_graph.num_nodes() ){
        std::cout << "(check_if_isomorph) Knotenanzahl nicht gleich \n";
        return 0;
    }
    if( num_edges() != other_graph.num_edges() ){
        std::cout << "(check_if_isomorph) Kantenanzahl nicht gleich \n";
        return 0;
    }

    //Matrix für alle Knotenpaare, Adjazenzmatrix
    // edge_weight_matrice[i][j] entspricht Knotenpaar i+1, j (wobei i >= j)
    std::vector< std::vector<Graph::EdgeWeight> > edge_weight_matrice;
    std::vector< std::vector<bool> > adjacency_matrice;

    //Initialisieren
    for(unsigned int i = 0; i < num_nodes()-1; i++){
        std::vector<Graph::EdgeWeight> var_weight_vector (i+1, Graph::infinite_weight);
        edge_weight_matrice.push_back(var_weight_vector);

        std::vector<bool> var_bool_vector (i+1, 0);
        adjacency_matrice.push_back(var_bool_vector);
    }


    //ausfüllen entsprechend der Kanten des aktuellen Graphen
    for(auto curr_edge : edges() ){
        std::array<Graph::NodeId, 2> curr_edge_nodes = curr_edge.get_nodes_orderedbyid();

        edge_weight_matrice[ curr_edge_nodes[1]-1 ][ curr_edge_nodes[0] ] = curr_edge.weight();

        adjacency_matrice[ curr_edge_nodes[1]-1 ][ curr_edge_nodes[0] ] = 1;
    }

    //Gleichheit der Kanten des eingegebenen Graphen prüfen
    for(auto curr_edge : other_graph.edges() ){
        std::array<Graph::NodeId, 2> curr_edge_nodes = curr_edge.get_nodes_orderedbyid();

        if( not adjacency_matrice[ curr_edge_nodes[1]-1 ][ curr_edge_nodes[0] ] ){
            std::cout << "(check_if_isomorph) Kante ";
            other_graph.print_edge_as_pair( curr_edge.edge_id() );
            std::cout << " liegt in eingegebenem Graph aber nicht im aktuellen. \n";
            return 0;
        }
        if( edge_weight_matrice[ curr_edge_nodes[1]-1 ][ curr_edge_nodes[0] ] != curr_edge.weight() ){
            std::cout << "(check_if_isomorph) Kantengewicht der Kante ";
            other_graph.print_edge_as_pair( curr_edge.edge_id() );
            std::cout << "ungleich. \n";
            return 0;
        }

        //am Ende sollte die Adjazenz-Matrix nur Nullen enthalten
        adjacency_matrice[ curr_edge_nodes[1]-1 ][ curr_edge_nodes[0] ] = 0;
    }

    for(unsigned int i = 0; i < num_nodes()-1; i++){
        for(unsigned int j = 0; j < i+1; j++){
            if( adjacency_matrice[i][j] ){
                std::cout << "(check_if_isomorph) Kante (" << i+1 << "," << j << ") liegt im aktuellen Graph aber nicht im eingegebenen. \n";
                return 0;
            }
        }
    }

    return 1;
}

Graph Graph::copygraph_wo_edges() const{
    Graph output(0);
    for( auto curr_node : _nodes){
        output.add_one_node( curr_node.node_name(), curr_node.terminal_state() );
    }
    return output;
}


Graph Graph::copygraph_wo_iso_nodes() {
    Graph output_graph(0);
    std::vector< Graph::NodeId > new_node_ids ( num_nodes() );

    //Knoten hinzufügen
    unsigned int output_graph_nodes = 0;
    for(Graph::NodeId i=0; i<num_nodes(); i++){
        Graph::Node curr_node = get_node(i);
        if( curr_node.num_neighbors() != 0 ){
            output_graph.add_one_node( curr_node.node_name(), curr_node.terminal_state() );

            //merke node_id bzgl output-Graph des Knoten mit node_id i bzgl g
            new_node_ids[ i ] = output_graph_nodes;
            output_graph_nodes++;
        }
    }

    //Kanten hinzufügen
    for (auto var_edge : edges() ){
        //"konstruiere" neue Kante
        Graph::NodeId new_node_a =  new_node_ids[var_edge.node_a()];
        Graph::NodeId new_node_b =  new_node_ids[var_edge.node_b()];

        //füge sie zu output hinzu
        output_graph.add_edge(new_node_a, new_node_b, var_edge.weight());
    }

    return output_graph;
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
                unsigned int n; //Anzahl der Knoten
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
    if( _edges.size() != m){
        throw std::runtime_error("(Einlesen) angegebene Kantenanzahl ungleich Anzahl aufgelisteter Kanten");
    }
    if( get_vect_term().size() != t_num){
        throw std::runtime_error("(Einlesen) angegebene Terminalanzahl ungleich Anzahl aufgelisteter Terminale");
    }

}
