//
// Created by Lukas on 02.06.2022.
//

#include <utility>
#include <iostream>
#include <queue>
#include "limits"
//? #include <functional>
//? #include <algorithm>

#include "voronoi_diagram.h"
#include "graph_aux_functions.h"
#include "graph_printfunctions.h"

//#include "graph_algorithms.cpp" //? ändern

const Voronoi_diagram::BaseId Voronoi_diagram::invalid_base_id = std::numeric_limits<unsigned int>::max();

Voronoi_diagram::Voronoi_diagram(Graph& input_graph):
    _original_graph(input_graph)
{
    _num_nodes = input_graph.num_nodes();
    _num_bases = 0;

    std::vector<Graph::NodeId> var_set_of_bases;
    _set_of_bases = var_set_of_bases;

    _base.assign(_num_nodes, Graph::invalid_node_id);

    std::pair< Graph::NodeId, Graph::EdgeId> var_pair = {Graph::invalid_node_id, Graph::invalid_edge_id};
    _predecessor.assign(_num_nodes, var_pair);

    std::vector<Graph::PathLength> var_dist_to_base (_num_nodes, Graph::infinite_length);
    _dist_to_base = var_dist_to_base;

    _base_ids.assign(_num_nodes, Voronoi_diagram::invalid_base_id);
}

Voronoi_diagram::Voronoi_diagram(const std::vector<Graph::NodeId>& set_of_b, Graph& input_graph):
    _original_graph(input_graph)
{

    //Prüfe, ob Eingabe Bedingungen erfüllt
    //für die Menge der Basen set_of_b
    if(set_of_b.empty()){
        throw std::runtime_error("(constructor Voronoi_diagram) set_of_b ist leer");
    }
    for(auto var_node : set_of_b){
        if(var_node >= _original_graph.num_nodes()){
            std::cout << "Basis mit NodeId " << var_node << " ist nicht im Graph g.";
            throw std::runtime_error("(constructor Voronoi_diagram) set_of_b ist keine Teilmenge der Knoten von g");
        }
    }
    //und für die Kantengewichte von dem Graphen g
    if(! edgeweight_nonnegative(_original_graph)){
        throw std::runtime_error("(constructor Voronoi_diagram) es gibt negative Kantengewichte im Eingabegraphen");
    }
    if( ! edgeweight_finite(_original_graph) ){
        throw std::runtime_error("(constructor Voronoi_diagram) es gibt Kante im Eingabegraphen mit 'unendlichem' Gewicht");
    }

    //Initialisieren der Attribute der Klasse Voronoi-D.

    _num_nodes = _original_graph.num_nodes();

    _base.assign(_num_nodes, Graph::invalid_node_id);

    std::pair< Graph::NodeId, Graph::EdgeId> var_pair = {Graph::invalid_node_id, Graph::invalid_edge_id};
    _predecessor.assign(_num_nodes, var_pair);

    std::vector<Graph::PathLength> var_dist_to_base (_num_nodes, Graph::infinite_length);
    _dist_to_base = var_dist_to_base;

    _base_ids.assign(_num_nodes, Voronoi_diagram::invalid_base_id);

    assign_set_of_bases(set_of_b);

    //Initialisierung der priority_queue, mit der wir in jeder Iteration den nächsten besten Knoten (Kandidat) finden
    //std::greater< std::pair<Graph::PathLength,Graph::NodeId> > sorgt dafür, dass wir immer an den Knoten mit der kleinsten Distanz gelangen
    std::priority_queue<
            std::pair<Graph::PathLength,Graph::NodeId>,
            std::vector< std::pair<Graph::PathLength,Graph::NodeId> >,
            std::greater< std::pair<Graph::PathLength,Graph::NodeId> >
                > candidates;
    // Basen sind die ersten Kandidaten
    for(auto curr_base : _set_of_bases) {
        candidates.push( {0, curr_base} );
    }

    //int debug_count_while_loop = 0;
    while( not candidates.empty() ) {
        /*
        debug_count_while_loop++;
        std::cout << "Iteration while-Schleife nr. " << debug_count_while_loop << "\n";
        */

        //finde nächsten besten Knoten/Kandidaten
        Graph::NodeId next_node = candidates.top().second;
        Graph::PathLength dist_of_next_cand = candidates.top().first;
        candidates.pop();

        //checke ob restliche Knoten alle unerreichbar sind. besser mit oder ohne diesen Check?
        if (dist_of_next_cand == Graph::infinite_length) {
            //cout<< "Iteration " << count_while_loop << ": break weil alle l = infty \n"; //debug
            break;
        }

        //debug
        if (dist_of_next_cand < _dist_to_base[next_node]) {
            std::cout << "Seltsam: dist_of_next_cand < _dist_to_base[next_node], in Zahlen: ";
            std::cout << dist_of_next_cand << " < " << _dist_to_base[next_node] << "\n";
        }

        if (dist_of_next_cand == _dist_to_base[next_node]) {

            //füge zur Basis des Vorgängers hinzu
            if(not check_if_base(next_node)){
                _base[next_node] = _base[_predecessor[next_node].first];
            }

            //Schleife über die zu next_node inzidenten Kanten
            std::vector<Graph::EdgeId> next_node_edges = _original_graph.get_node(next_node).incidence_vect();
            for (auto curr_edge_id: next_node_edges) {
                Graph::Edge curr_edge = _original_graph.get_edge(curr_edge_id);
                Graph::NodeId curr_neighbor = curr_edge.get_other_node(next_node);

                //aktualisiere ggf. _dist_to_base sowie _predecessor für diesen Nachbarknoten und füge entsprechendes Element zu candidates hinzu
                if (_dist_to_base[curr_neighbor] > _dist_to_base[next_node] + curr_edge.weight()) {
                    _dist_to_base[curr_neighbor] = _dist_to_base[next_node] + curr_edge.weight();
                    _predecessor[curr_neighbor] = {next_node, curr_edge_id};
                    candidates.push({_dist_to_base[next_node] + curr_edge.weight(), curr_neighbor});
                }
            }
        }
    }

}

void Voronoi_diagram::repair(const std::vector<Graph::NodeId>& new_set_of_bases){
    //prüfe die Eingabe
    for(auto curr_new_base : new_set_of_bases){
        if( curr_new_base >= _original_graph.num_nodes() ) {
            throw std::runtime_error("(Voronoi_diagram::repair) Knoten aus Eingabemenge liegt nicht im zugrundeliegenden Graphen");
        }
    }
    for(auto curr_new_base : new_set_of_bases){
        if( not check_if_base(curr_new_base) ) {
            throw std::runtime_error("(Voronoi_diagram::repair) Knoten aus Eingabemenge liegt nicht in urspruenglicher Basis");
        }
    }

    //update _base_ids, _set_of_bases, _num_bases
    _base_ids.assign(_num_nodes, Voronoi_diagram::invalid_base_id);
    assign_set_of_bases(new_set_of_bases);

    //Initialisierung der priority_queue, mit der wir in jeder Iteration den nächsten besten Knoten (Kandidat) finden
    //std::greater< std::pair<Graph::PathLength,Graph::NodeId> > sorgt dafür, dass wir immer an den Knoten mit der kleinsten Distanz gelangen
    std::priority_queue<
            std::pair<Graph::PathLength,Graph::NodeId>,
            std::vector< std::pair<Graph::PathLength,Graph::NodeId> >,
            std::greater< std::pair<Graph::PathLength,Graph::NodeId> >
    > candidates;

    //Merke die Knoten, deren Basis aus der Menge der Basen entfernt wird, für 'second stage', im Paper mit C notiert
    std::vector<bool> nodes_to_update (_num_nodes, false);

    // 'first stage' jeder Knoten aus C wird zu der Region seines besten Nachbarn (der nicht in C liegt) hinzugefügt
    for(Graph::NodeId curr_node = 0; curr_node < _num_nodes; curr_node++) {
        if( not check_if_base( _base[curr_node] ) ) {
            _base[curr_node] = Graph::invalid_node_id;
            _predecessor[curr_node] = {Graph::invalid_node_id, Graph::invalid_edge_id};
            _dist_to_base[curr_node] = Graph::infinite_length;

            for(auto curr_edge_id : _original_graph.get_node(curr_node).incidence_vect() ) {
                Graph::Edge curr_edge = _original_graph.get_edge(curr_edge_id);
                Graph::NodeId curr_neighbor = curr_edge.get_other_node(curr_node);

                if( check_if_base( _base[curr_neighbor] ) && _dist_to_base[curr_node] > _dist_to_base[curr_neighbor] + curr_edge.weight() ) {
                    _base[curr_node] = _base[curr_neighbor];
                    _predecessor[curr_node] = {curr_neighbor, curr_edge_id};
                    _dist_to_base[curr_node] = _dist_to_base[curr_neighbor] + curr_edge.weight();
                }
            }

            candidates.push({_dist_to_base[curr_node], curr_node});

            nodes_to_update[curr_node] = true;
        }
    }

    // 'second stage' modified dijkstra, entspricht bis auf Details der Schleife aus dem Konstruktor
    // es werden nur Knoten aus C bzw. aus nodes_to_update gescannt
    // todo: für die lokalen Suchen müssen jeweils ein paar Zeilen hinzugefügt werden
    while ( not candidates.empty() ) {
        //finde nächsten besten Knoten/Kandidaten
        Graph::NodeId next_node = candidates.top().second;
        Graph::PathLength dist_of_next_cand = candidates.top().first;
        candidates.pop();

        //checke ob restliche Knoten alle unerreichbar sind. besser mit oder ohne diesen Check?
        if (dist_of_next_cand == Graph::infinite_length) {
            //cout<< "Iteration " << count_while_loop << ": break weil alle l = infty \n"; //debug
            break;
        }

        //debug
        if (dist_of_next_cand < _dist_to_base[next_node]) {
            std::cout << "Seltsam: dist_of_next_cand < _dist_to_base[next_node], in Zahlen: ";
            std::cout << dist_of_next_cand << " < " << _dist_to_base[next_node] << "\n";
        }

        if (dist_of_next_cand == _dist_to_base[next_node]) {

            //debug
            if(check_if_base(next_node)){
                throw std::runtime_error("(Voronoi_diagram::repair) Kandidat ist Basis");
            }

            //füge zur Basis des Vorgängers hinzu
            //bemerke, dass _predecessor[next_node] 'wohldefiniert' ist, da next_node niemals Basis sein kann,
            // da nur Knoten aus node_to_update zu candidates hinzugefügt werden (bemerkung löschen?)
            _base[next_node] = _base[_predecessor[next_node].first];

            //Schleife über die zu next_node inzidenten Kanten
            std::vector<Graph::EdgeId> next_node_edges = _original_graph.get_node(next_node).incidence_vect();
            for (auto curr_edge_id: next_node_edges) {
                Graph::Edge curr_edge = _original_graph.get_edge(curr_edge_id);
                Graph::NodeId curr_neighbor = curr_edge.get_other_node(next_node);

                //aktualisiere ggf. _dist_to_base sowie _predecessor für diesen Nachbarknoten und füge entsprechendes Element zu candidates hinzu
                //Unterschied zum dijkstra aus dem Konstruktor: es werden nur Knoten aus nodes_to_update gescannt (Bemerkung löschen?)
                if(nodes_to_update[curr_neighbor]) {
                    if (_dist_to_base[curr_neighbor] > _dist_to_base[next_node] + curr_edge.weight()) {
                        _dist_to_base[curr_neighbor] = _dist_to_base[next_node] + curr_edge.weight();
                        _predecessor[curr_neighbor] = {next_node, curr_edge_id};
                        candidates.push({_dist_to_base[next_node] + curr_edge.weight(), curr_neighbor});
                    }
                }
                /*
                 für die lokalen Suchen hier boundary edges finden
                if( reached[curr_neighbor] ){
                    if( ... ) {
                        if( ... < ... ){

                        }
                    }
                }
                */
            }
        }
    }

}

void Voronoi_diagram::assign_set_of_bases(const std::vector<Graph::NodeId>& new_set_of_bases) {
    _set_of_bases = new_set_of_bases;
    _num_bases = _set_of_bases.size();

    if(_num_bases >= invalid_base_id){
        throw std::runtime_error("(void Voronoi_diagram::assign_set_of_bases) neue Menge der Basen zu groß");
    }

    for(unsigned int i = 0; i<_num_bases; i++){
        Graph::NodeId var_base = _set_of_bases[i];
        _base[var_base] = var_base;
        _dist_to_base[var_base] = 0;
        _base_ids[var_base] = i;
    }
}

void Voronoi_diagram::print_simple() const {
    std::cout << "Ausgabe des Voronoi-Diagramms (als NodeNames ausgegeben)\n";
    std::cout << "(Knoten) (Basis) (Distanz zur Basis) (Vorgaengerknoten und -kante auf kuerz. Weg von Basis) \n";
    for(Graph::NodeId i = 0; i<_num_nodes; i++){
        print_nodename(_original_graph, i);
        std::cout << " ";
        print_nodename(_original_graph, _base[i]);
        std::cout << " ";
        print_pathlength(_dist_to_base[i]);
        std::cout << " ";
        print_nodename(_original_graph, _predecessor[i].first);
        std::cout << " ";
        print_edge_as_pair(_original_graph, _predecessor[i].second);
        std::cout << "\n";
    }
    std::cout << "\n";
}

void Voronoi_diagram::print_region(Graph::NodeId var_base) const {
    if(!check_if_base(var_base)){
        std::cout << "(void Voronoi_diagram::print_region(Graph::NodeId var_base)) var_base = " << var_base << " ist keine Basis \n";
    }
    std::cout << "Die Voronoi-Region des Knoten " << var_base << " ist: (als NodeNames ausgegeben)\n";
    std::cout << "(Knoten) (Distanz zur Basis) (Vorgaenger auf kuerz. Weg von Basis) \n";
    for(Graph::NodeId i=0; i<_num_nodes; i++){
        if(_base[i] == var_base){
            print_nodename(_original_graph, i);
            std::cout << " ";
            print_pathlength(_dist_to_base[i]);
            std::cout << " ";
            print_nodename(_original_graph, _predecessor[i].first);
            std::cout << "\n";
        }
    }
}

bool Voronoi_diagram::check_if_base(Graph::NodeId var_node) const {
    if(var_node == Graph::invalid_node_id){
        return false;
    }
    if( var_node > _num_nodes ){
        throw std::runtime_error("(Voronoi_diagram::check_if_base) Knoten liegt nicht im zugrundeliegenden Graphen");
    }
    //Knoten ist Basis gdw. wenn er sich selbst als Basis hat
    if( _base_ids[var_node] != invalid_base_id){
        return true;
    }else {
        return false;
    }
}

std::vector<std::vector<std::pair<Graph::EdgeId, Graph::PathLength>>> Voronoi_diagram::min_bound_edges() const {

    //Initialisierung der Ausgabe-Matrix: output[i][j] entspricht den Basen i+1, j
    std::vector<std::vector<std::pair<Graph::EdgeId, Graph::PathLength>>> output;
    std::pair<Graph::EdgeId, Graph::PathLength> invalid_pair = {Graph::invalid_edge_id, Graph::infinite_length};
    for(unsigned int i = 0; i < _num_bases-1; i++){
        std::vector<std::pair<Graph::EdgeId, Graph::PathLength>> var_vector (i+1, invalid_pair);
        output.push_back(var_vector);
    }

    //Schleife über alle Kanten des Graphen, die boundary edges sind
    std::vector<Graph::Edge> _original_graph_edges = _original_graph.edges();
    for(auto curr_edge : _original_graph_edges){
        if( check_if_bound_edge(curr_edge) ){

            //prüfe, ob aktuelle Kante besser als gespeicherte Kante (update falls ja)
            std::pair<Voronoi_diagram::BaseId, Voronoi_diagram::BaseId> curr_bases = get_base_ids_ordered( _base[curr_edge.node_a()], _base[curr_edge.node_b()]);
            Graph::PathLength curr_dist = curr_edge.weight() + _dist_to_base[ curr_edge.node_a() ] + _dist_to_base[ curr_edge.node_b() ];
            if( curr_dist < output[ curr_bases.second-1 ][ curr_bases.first ].second){
                output[ curr_bases.second-1 ][ curr_bases.first ].first = curr_edge.edge_id();
                output[ curr_bases.second-1 ][ curr_bases.first ].second = curr_dist;
            }

        }
    }

    return output;
}

void Voronoi_diagram::print_min_bound_edges() const {
    std::vector<std::vector<std::pair<Graph::EdgeId, Graph::PathLength>>> output = min_bound_edges();

    std::cout << "Ausgabe von min_bound_edges \n";

    std::cout << "Kanten (EdgeIds): \n";
    std::cout << "Basis     ";
    for(unsigned int j = 0; j < _num_bases-1; j++) {
        std::cout << _set_of_bases[j]+1 << ", ";
    }
    std::cout << "\n";
    for(unsigned int i = 0; i < _num_bases-1; i++){
        std::cout << _set_of_bases[i+1]+1 << ":    ";
        for(unsigned int j = 0; j < i+1; j++){
            print_edge_as_pair(_original_graph, output[i][j].first );
            std::cout << ", ";
        }
        std::cout << "\n";
    }

    std::cout << "\n" << "Distanzen: \n";
    std::cout << "Basis     ";
    for(unsigned int j = 0; j < _num_bases-1; j++){
        std::cout << _set_of_bases[j]+1 << ", ";
    }
    std::cout << "\n";
    for(unsigned int i = 0; i < _num_bases-1; i++){
        std::cout << _set_of_bases[i+1]+1 << ":    ";
        for(unsigned int j = 0; j < i+1; j++){
            print_pathlength( output[i][j].second );
            std::cout << ", ";
        }
        std::cout << "\n";
    }

}

std::pair<Voronoi_diagram::BaseId, Voronoi_diagram::BaseId> Voronoi_diagram::get_base_ids_ordered(Graph::NodeId node_one, Graph::NodeId node_two) const{
    //Checks
    if(node_one == node_two){
        throw std::runtime_error("(Voronoi_diagram::get_base_ids_ordered) Knoten sind gleich");
    }
    if(node_one > _num_nodes || node_two > _num_nodes){
        throw std::runtime_error("Voronoi_diagram::get_base_ids_ordered) NodeIds zu groß");
    }

    Voronoi_diagram::BaseId base_one = _base_ids[node_one];
    Voronoi_diagram::BaseId base_two = _base_ids[node_two];

    //debug
    if(base_one == Voronoi_diagram::invalid_base_id || base_two == Voronoi_diagram::invalid_base_id){
        std::cout << "ACHTUNG! Aufruf von Voronoi_diagram::get_base_ids_ordered: mind. 1 Eingabeknoten ist keine Basis \n";
    }

    if(base_one < base_two){
        return {base_one, base_two};
    }else{
        return {base_two, base_one};
    }
}

const Graph &Voronoi_diagram::original_graph() const {
    return _original_graph;
}

const std::vector<Graph::NodeId> &Voronoi_diagram::base() const {
    return _base;
}

const std::vector<Graph::PathLength> &Voronoi_diagram::dist_to_base() const {
    return _dist_to_base;
}

bool Voronoi_diagram::check_if_bound_edge(const Graph::Edge& var_edge) const {
    if(var_edge.node_a()>=_num_nodes || var_edge.node_b()>=_num_nodes){
        throw std::runtime_error("(Voronoi_diagram::check_if_bound_edge) Knoten der Kante sind nicht im Graphen");
        //? wenn _original_graph als Attribut, dann hier checken, ob Kante im Graph
    }

    Graph::NodeId base_a = _base[var_edge.node_a()];
    Graph::NodeId base_b = _base[var_edge.node_b()];

    if( base_a != base_b && check_if_base(base_a) && check_if_base(base_b) ) {
        return true;
    } else {
        return false;
    }
}

Graph Voronoi_diagram::construct_aux_graph() const{
    Graph output(0);

    //Basen als Knoten hinzufügen
    for( auto curr_base : _set_of_bases){
        output.add_one_node( curr_base+1, _original_graph.get_node(curr_base).terminal_state() );
    }

    //Kanten hinzufügen
    std::vector<std::vector<std::pair<Graph::EdgeId, Graph::PathLength>>> vect_min_bound_edges = min_bound_edges();
    for(unsigned int i = 0; i < _num_bases-1; i++){
        for(unsigned int j = 0; j < i+1; j++){
            if( vect_min_bound_edges[i][j].first != Graph::invalid_edge_id ){
                output.add_edge( j, i+1, vect_min_bound_edges[i][j].second);
            }
        }
    }

    return output;
}

//todo nicht diesen Vektor übergeben sondern einfach innerhalb dieser Funktion min_bound_edges aufrufen?
Graph Voronoi_diagram::turn_into_subgraph(const Graph &var_graph, const std::vector<std::vector<std::pair<Graph::EdgeId, Graph::PathLength>>> &vect_min_bound_edges) const {
    Graph output = copygraph_wo_edges(_original_graph);

    //speichert die schon hinzugefügten Kanten, sodass wir keine Kanten mehrfach hinzufügen
    std::vector<bool> added_edges(_original_graph.num_edges(), false);

    for( auto curr_edge : var_graph.edges() ) {
        //zugehörige boundary edge des ursprünglichen Graphen finden
        // ? wenn ich Graph gerichtet machen kann, könnte ich das hier ggf. benutzen (anstatt von der nächsten Zeile)
        std::pair<Graph::NodeId, Graph::NodeId> curr_edge_nodes = curr_edge.get_nodes_orderedbyid();
        Graph::Edge original_bound_edge = _original_graph.get_edge( vect_min_bound_edges[curr_edge_nodes.second - 1][curr_edge_nodes.first].first );

        //diese Kante hinzufügen
        output.add_edge(original_bound_edge.node_a(), original_bound_edge.node_b(), original_bound_edge.weight());
        added_edges[original_bound_edge.edge_id()] = true;

        //Kanten auf den jeweiligen kürz. Wegen von den Endpunkten der aktuellen Kante zu deren jeweiliger Basis hinzufügen
        Graph::NodeId var_node_id = original_bound_edge.node_a();
        Graph::EdgeId edge_to_add_id = _predecessor[var_node_id].second;
        while( edge_to_add_id != Graph::invalid_edge_id && not added_edges[ edge_to_add_id ] ){
            output.add_edge( _original_graph.get_edge( edge_to_add_id ).node_a(),
                             _original_graph.get_edge( edge_to_add_id ).node_b(),
                             _original_graph.get_edge( edge_to_add_id ).weight() );
            added_edges[ edge_to_add_id ] = true;

            var_node_id = _predecessor[var_node_id].first;
            edge_to_add_id = _predecessor[var_node_id].second;
        }

        var_node_id = original_bound_edge.node_b();
        edge_to_add_id = _predecessor[var_node_id].second;
        while( edge_to_add_id != Graph::invalid_edge_id && not added_edges[ edge_to_add_id ] ){
            output.add_edge( _original_graph.get_edge( edge_to_add_id ).node_a(),
                             _original_graph.get_edge( edge_to_add_id ).node_b(),
                             _original_graph.get_edge( edge_to_add_id ).weight() );
            added_edges[ edge_to_add_id ] = true;

            var_node_id = _predecessor[var_node_id].first;
            edge_to_add_id = _predecessor[var_node_id].second;
        }

    }

    //Laufzeit?
    output = copygraph_wo_iso_nodes(output);

    return output;
}



