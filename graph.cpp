//
// Created by Lukas on 30.05.2022.
//

#include "graph.h"

#include <iostream>
#include <sstream>
#include <fstream>
#include <limits>
#include <utility>

#include "string_aux_functions.h"

const Graph::EdgeWeight Graph::infinite_weight = std::numeric_limits<double>::max();
const Graph::PathLength Graph::infinite_length = std::numeric_limits<double>::max();
const Graph::NodeId Graph::invalid_node_id = std::numeric_limits<unsigned int>::max();
const Graph::EdgeId Graph::invalid_edge_id = std::numeric_limits<unsigned int>::max();

Graph::Graph(int n)
{
    _dir_type = Graph::undirected;

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

void Graph::Edge::direct_edge(Graph::NodeId tail, Graph::NodeId head) {
    if( head == Graph::invalid_node_id ) {
        throw std::runtime_error("(Graph::Edge::direct_edge) Eingabeknoten head ungueltig");
    }
    if( tail == Graph::invalid_node_id ) {
        throw std::runtime_error("(Graph::Edge::direct_edge) Eingabeknoten tail ungueltig");
    }
    //kann ich hier prüfen, ob die Eingabeknoten im Graphen liegen? Wie greife ich von Edge aus auf num_nodes() zu?

    if( node_a() == tail && node_b() == head ) {
        return;
    } else if ( node_a() == head && node_b() == tail ) {
        reverse_node_order();
        return;
    } else {
        throw std::runtime_error("(Graph::Edge::direct_edge) Eingabeknoten sind nicht zur Kante inzident");
    }
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

void Graph::Node::set_terminal(Graph::TerminalState t){
    _terminal_state = t;
}

bool Graph::Node::check_if_terminal() const{
    if(_terminal_state == Graph::terminal){
        return true;
    }
    return false;
}

Graph::TerminalState Graph::Node::terminal_state() const {
    return _terminal_state;
}

void Graph::set_terminal(Graph::NodeId v, Graph::TerminalState t){
    unsigned int v_int = (unsigned int) v;
    if(v_int < num_nodes() ){   //?
        _nodes[v_int].set_terminal(t);
    }else{
        std::cout << "v=" << v << "\n";
        throw std::runtime_error("(Graph::set_terminal) Der Knoten v ist nicht im Graphen.");
    }
}

std::vector<Graph::NodeId> Graph::get_vect_term() const{
    std::vector<Graph::NodeId> output;
    for(Graph::NodeId i = 0; i<num_nodes(); i++){
        if(get_node(i).check_if_terminal()){
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

void Graph::add_one_existing_node(const Graph::Node new_node){
    Graph::NodeId id = new_node.node_id();
    unsigned int new_n = num_nodes();

    if(id != new_n){
        std::cout << "Knoten mit NodeId " << id << " kann nicht hinzugefuegt werden, ";
        std::cout << "da seine NodeId ungleich der neuen Anzahl an Knoten ist (" << new_n << ") \n";
        throw std::runtime_error("Funktion add_one_existing_node");
    }

    _nodes.push_back(new_node);
}

void Graph::add_one_existing_node_w_newid(const Graph::Node new_node){
    Graph::NodeId id = num_nodes();

    Graph::Node node_to_add(id, new_node.node_name(), new_node.terminal_state());

    _nodes.push_back(node_to_add);
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
        std::cout << "v hat NodeId " << v << "\n";
        throw std::runtime_error("(Graph::get_node) Knoten v liegt nicht im Graphen");
    }
    return _nodes[v];
}

const std::vector<Graph::EdgeId>& Graph::Node::incident_edge_ids() const{
    return _incident_edge_ids;
}

void Graph::Node::add_neighbor_edge(EdgeId e){
    _incident_edge_ids.push_back(e);
}

unsigned int Graph::Node::num_neighbors()  const{
    return _incident_edge_ids.size();
}

const std::vector<Graph::EdgeId>& Graph::incident_edge_ids(Graph::NodeId input_node) const {
    return get_node(input_node).incident_edge_ids();
}

/*
 *?
std::vector<const Graph::Edge&> Graph::incident_edges(Graph::NodeId input_node_id) const {
    //check?
    std::vector<const Graph::Edge&> output;
    for(auto curr_edge_id : incident_edge_ids(input_node_id)) {
        output.push_back(get_edge(curr_edge_id));
    }
    return output;
}
 */

std::vector<Graph::NodeId> Graph::adjacency_vect(Graph::NodeId input_node_id) const{
    if( input_node_id == Graph::invalid_node_id) {
        throw std::runtime_error("(Graph::adjacency_vect) Eingabeknoten ungueltig");
    }
    if( input_node_id > num_nodes()) {
        throw std::runtime_error("(Graph::adjacency_vect) Eingabeknoten nicht im Graph");
    }

    std::vector<Graph::NodeId> output;
    std::vector<EdgeId> incident_edge_ids_of_input = incident_edge_ids(input_node_id);
    for(auto curr_edge_id : incident_edge_ids_of_input) {
        Graph::Edge curr_edge = get_edge(curr_edge_id);
        Graph::NodeId curr_neighbor_id = curr_edge.get_other_node(input_node_id);
        output.push_back(curr_neighbor_id);
    }
    return output;
}

std::vector<Graph::EdgeId> Graph::get_ingoing_edge_ids(Graph::NodeId input_node_id) const {
    if( _dir_type != Graph::directed) {
        throw std::runtime_error("(Graph::get_ingoing_edge_ids) Graph ist ungerichtet");
    }
    if( input_node_id == Graph::invalid_node_id) {
        throw std::runtime_error("(Graph::get_ingoing_edge_ids) Eingabeknoten ungueltig");
    }
    if( input_node_id > num_nodes()) {
        throw std::runtime_error("(Graph::get_ingoing_edge_ids) Eingabeknoten nicht im Graph");
    }

    std::vector<Graph::EdgeId> output;

    std::vector<EdgeId> incident_edge_ids_of_input = incident_edge_ids(input_node_id);
    for(auto curr_edge_id : incident_edge_ids_of_input) {
        Graph::Edge curr_edge = get_edge(curr_edge_id);
        Graph::NodeId curr_neighbor_id = curr_edge.get_other_node(input_node_id);
        if( curr_neighbor_id == curr_edge.node_a()){
            output.push_back(curr_edge_id);
        }
    }

    return output;
}

std::vector<Graph::EdgeId> Graph::get_outgoing_edge_ids(Graph::NodeId input_node_id) const {
    if( _dir_type != Graph::directed) {
        throw std::runtime_error("(Graph::get_outgoing_edge_ids) Graph ist ungerichtet");
    }
    if( input_node_id == Graph::invalid_node_id) {
        throw std::runtime_error("(Graph::get_outgoing_edge_ids) Eingabeknoten ungueltig");
    }
    if( input_node_id > num_nodes()) {
        throw std::runtime_error("(Graph::get_outgoing_edge_ids) Eingabeknoten nicht im Graph");
    }

    std::vector<Graph::EdgeId> output;

    std::vector<EdgeId> incident_edge_ids_of_input = incident_edge_ids(input_node_id);
    for(auto curr_edge_id : incident_edge_ids_of_input) {
        Graph::Edge curr_edge = get_edge(curr_edge_id);
        Graph::NodeId curr_neighbor_id = curr_edge.get_other_node(input_node_id);
        if( curr_neighbor_id == curr_edge.node_b()){
            output.push_back(curr_edge_id);
        }
    }

    return output;
}

std::vector<Graph::NodeId> Graph::get_ingoing_neighbors(Graph::NodeId input_node_id) const {
    std::vector<Graph::EdgeId> ingoing_edge_ids = get_ingoing_edge_ids(input_node_id);

    std::vector<Graph::NodeId> output;

    for( auto curr_edge_id: ingoing_edge_ids) {
        const Graph::Edge& curr_edge = get_edge(curr_edge_id);
        output.push_back(curr_edge.node_a());
    }

    return output;
}

std::vector<Graph::NodeId> Graph::get_outgoing_neighbors(Graph::NodeId input_node_id) const {
    std::vector<Graph::EdgeId> outgoing_edge_ids = get_outgoing_edge_ids(input_node_id);

    std::vector<Graph::NodeId> output;

    for( auto curr_edge_id: outgoing_edge_ids) {
        const Graph::Edge& curr_edge = get_edge(curr_edge_id);
        output.push_back(curr_edge.node_b());
    }

    return output;
}

Graph::NodeId Graph::get_tail(Graph::EdgeId input_edge_id) const {
    if( dir_type() == Graph::undirected) {
        throw std::runtime_error("(Graph::get_tail) Graph ist ungerichtet.");
    } else {
        return get_edge(input_edge_id).node_a();
    }
}

Graph::NodeId Graph::get_head(Graph::EdgeId input_edge_id) const {
    if( dir_type() == Graph::undirected) {
        throw std::runtime_error("(Graph::get_head) Graph ist ungerichtet.");
    } else {
        return get_edge(input_edge_id).node_b();
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

    if(a == Graph::invalid_node_id){
        std::cout << "Kante (" << a << "," << b << ") kann nicht hinzugefuegt werden, da " << a  << " = invalid_node_id \n";
        throw std::runtime_error("Funktion add_existing_edge");
    }
    if(b == Graph::invalid_node_id){
        std::cout << "Kante (" << a << "," << b << ") kann nicht hinzugefuegt werden, da " << b  << " = invalid_node_id \n";
        throw std::runtime_error("Funktion add_existing_edge");
    }
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

Graph::DirType Graph::dir_type() const{
    return _dir_type;
}

void Graph::make_rooted_arborescence(Graph::NodeId root_id) {
    if( root_id == Graph::invalid_node_id) {
        throw std::runtime_error("(Graph::make_rooted_arborescence) Eingabeknoten ungueltig");
    }
    if( root_id > num_nodes()) {
        throw std::runtime_error("(Graph::make_rooted_arborescence) Eingabeknoten nicht im Graphen");
    }

    std::vector<Graph::NodeId> next_nodes (1, root_id);
    //speichert die bereits erreichten Knoten (die, die bereits zu next_nodes hinzugfügt wurden)
    //Laufzeit O(num_nodes()) Problem?
    std::vector<bool> reached_nodes (num_nodes(), false);
    reached_nodes[root_id] = true;
    //speichert, ob Kante bereits gerichtet
    //Laufzeit O(num_edges()) Problem?
    std::vector<bool> directed_edges (num_edges(), false);

    while( not next_nodes.empty()) {
        Graph::NodeId curr_node_id = next_nodes.back();
        next_nodes.pop_back();

        for( auto curr_edge_id : incident_edge_ids(curr_node_id) ) {
            Graph::NodeId curr_neighbor_id = get_edge(curr_edge_id).get_other_node(curr_node_id);

            if( not directed_edges[curr_edge_id] ){
                if( reached_nodes[curr_neighbor_id] ) {
                    // falls die Kante noch nicht erreicht wurde, der Knoten aber schon, haben wir einen Kreis gefunden
                    throw std::runtime_error("(Graph::make_rooted_arborescence) Graph ist kein Baum");
                } else {
                    _edges[curr_edge_id].direct_edge(curr_node_id, curr_neighbor_id);
                    directed_edges[curr_edge_id] = true;
                    reached_nodes[curr_neighbor_id] = true;
                    next_nodes.push_back(curr_neighbor_id);
                }
            }
        }
    }

    for( auto var_reached : reached_nodes ){
        if( not var_reached ) {
            throw std::runtime_error("(Graph::make_rooted_arborescence) Es wurden nicht alle Knoten erreicht, Graph ist also nicht zsmhaengend.");
        }
    }

    _dir_type = Graph::directed;
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
        throw std::runtime_error("(Graph::Graph) Fehler beim Öffnen der Datei");
    }

    _dir_type = Graph::undirected;

    //Werte für die Variablen instance_name, instance_creator, instance_remark, instance_problem
    //falls diese nicht durch die Datei filename gegeben werden
    instance_name = "no name";
    instance_creator = "no creator";
    instance_remark = "no remark";
    instance_problem = "no problem specified";

    std::string line;
    getline(file, line);	//einlesen 1. Zeile
    //prüfe, ob die 1. Zeile dem Format entspricht
    std::string firstline1 = "33D32945 STP File, STP Format Version 1.0";
    std::string firstline2 = "33D32945 STP File, STP Format Version  1.00";
    if(not compare_strings_caseinsensitive(firstline1, line) && not compare_strings_caseinsensitive(firstline2, line)) {
        //debug
        std::cout << "line = " << line << "\n";
        throw std::runtime_error("Nicht das SteinLib-Format (1.Zeile)");
    }

    unsigned int m, t_num;	//m ist Anzahl Kanten, t_num ist Anzahl Terminale
    //Einlesen des Dokuments, erste Fallunterscheidungen nach SECTION
    while (getline(file, line)) {
        std::string var;
        if(compare_strings_caseinsensitive(line, "SECTION Comment")) {

            while (getline(file, line)){

                if(compare_strings_caseinsensitive(line, "END")) {
                    //Ende der Section erreicht
                    break;
                }

                //Einlesen von instance_name, instance_creator, instance_remark, instance_problem
                var = line.substr(0,4);
                if(compare_strings_caseinsensitive(var, "Name")) {
                    instance_name = line.substr(4, line.length());
                }
                var = line.substr(0, 7);
                if(compare_strings_caseinsensitive(var, "Creator")) {
                    instance_creator = line.substr(7, line.length());
                }
                var = line.substr(0, 6);
                if(compare_strings_caseinsensitive(var, "Remark")) {
                    instance_remark = line.substr(6, line.length());
                }
                var = line.substr(0, 7);
                if(compare_strings_caseinsensitive(var, "Problem")) {
                    instance_problem = line.substr(7, line.length());
                }

            }

        }

        if(compare_strings_caseinsensitive(line, "SECTION Graph")) {

            //Zeile mit "Nodes"
            getline(file, line);
            var = line.substr(0,5);
            //prüfe ob Eingabeformat korrekt
            if(not compare_strings_caseinsensitive(var, "Nodes")) {
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
            if(compare_strings_caseinsensitive(var , "Edges")) {
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
                if(compare_strings_caseinsensitive(line, "END")) {
                    //Ende der Section erreicht
                    break;
                }

                var = line.substr(0,1);
                //teste ob SteinLib-Format eingehalten wird
                if(not compare_strings_caseinsensitive(var, "E") && not compare_strings_caseinsensitive(var, "A")) {
                    //debug
                    std::cout << "Zeile beginnt mit " << var << "\n";
                    throw std::runtime_error("Eingabeformat: Eine Zeile in der Graph-Section (nach 'Nodes', 'Edges') beginnt nicht mit 'E' bzw. 'A' ");
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

        if(compare_strings_caseinsensitive(line, "SECTION Terminals")) {
            //Zeile mit "Terminals"
            getline(file, line);
            var = line.substr(0,9);
            if(compare_strings_caseinsensitive(var, "Terminals")) {
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
                if(compare_strings_caseinsensitive(line, "END")) {
                    //Ende der Section erreicht
                    break;
                }

                var = line.substr(0,1);
                //prüfe ob Eingabeformat korrekt
                if(not compare_strings_caseinsensitive(var, "T")) {
                    //debug
                    std::cout << "Zeile beginnt mit " << var << "\n";
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

        if(compare_strings_caseinsensitive(line, "EOF")) {
            //Ende der Datei erreicht
            break;
        }

        //die Sections "MD" (MaximumDegrees) und D{n} (Coordinates) werden nicht betrachtet

    }

    //prüfen, ob angegebene Kanten-/Terminalanzahl mit aufgelisteten Kanten/Terminalen übereinstimmt
    if( num_edges() != m){
        //debug
        std::cout << "angegebene Kantenanzahl: " << m << "\n";
        std::cout << "Anzahl aufgelisteter Kanten " << num_edges() << "\n";
        if( num_edges() != m/2){
            //throw std::runtime_error("(Einlesen) angegebene Kantenanzahl ungleich Anzahl aufgelisteter Kanten");
        }
    }
    if( get_vect_term().size() != t_num){
        //debug
        std::cout << "angegebene Terminalanzahl: " << t_num << "\n";
        std::cout << "Anzahl aufgelisteter Terminale " << get_vect_term().size() << "\n";
        //? throw std::runtime_error("(Einlesen) angegebene Terminalanzahl ungleich Anzahl aufgelisteter Terminale");
    }

}

/*
 * ?
Graph::Graph(const Graph& g) {
    _nodes = g.nodes();
    _edges = g.edges();
    //instance_name etc nicht definiert
}
*/