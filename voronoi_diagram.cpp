//
// Created by Lukas on 02.06.2022.
//

#include "voronoi_diagram.h"
#include <utility>
#include <iostream>
#include <queue>
#include <functional>
//? #include <algorithm>
//? #include <array>

const Voronoi_diagram::BaseId Voronoi_diagram::invalid_base_id = std::numeric_limits<unsigned int>::max();

Voronoi_diagram::Voronoi_diagram(Graph& input_graph){
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

    //Graph var_graph = Graph(_num_nodes);   //?
    //original_graph = input_graph;
}

Voronoi_diagram::Voronoi_diagram(std::vector<Graph::NodeId> set_of_b, Graph& input_graph){

    //Prüfe, ob Eingabe Bedingungen erfüllt
    //für die Menge der Basen set_of_b
    if(set_of_b.size() == 0){
        throw std::runtime_error("(constructor Voronoi_diagram) set_of_b ist leer");
    }
    for(auto var_node : set_of_b){
        if(var_node >= input_graph.num_nodes()){
            std::cout << "Basis mit NodeId " << var_node << " ist nicht im Graph g.";
            throw std::runtime_error("(constructor Voronoi_diagram) set_of_b ist keine Teilmenge der Knoten von g");
        }
    }
    //und für die Kantengewichte von dem Graphen g
    if(! input_graph.edgeweight_nonnegative()){
        throw std::runtime_error("(constructor Voronoi_diagram) es gibt negative Kantengewichte in g");
    }
    if( ! input_graph.edgeweight_finite() ){
        throw std::runtime_error("(constructor Voronoi_diagram) es gibt Kante in g mit Gewicht 'numeric_limits <double>::max()' ");
    }

    //Initialisieren der Attribute der Klasse Voronoi-D.

    _num_nodes = input_graph.num_nodes();

    _base.assign(_num_nodes, Graph::invalid_node_id);

    std::pair< Graph::NodeId, Graph::EdgeId> var_pair = {Graph::invalid_node_id, Graph::invalid_edge_id};
    _predecessor.assign(_num_nodes, var_pair);

    std::vector<Graph::PathLength> var_dist_to_base (_num_nodes, Graph::infinite_length);
    _dist_to_base = var_dist_to_base;

    _base_ids.assign(_num_nodes, Voronoi_diagram::invalid_base_id);

    assign_set_of_bases(set_of_b);

    //Graph var_graph = input_graph; //?
    //original_graph = input_graph;

    //Initialisierung der priority_queue, mit der wir in jeder Iteration den nächsten besten Knoten (Kandidat) finden
    //std::greater< std::pair<Graph::PathLength,Graph::NodeId> > sorgt dafür, dass wir immer an den Knoten mit der kleinsten Distanz gelangen
    std::priority_queue<
            std::pair<Graph::PathLength,Graph::NodeId>,
            std::vector< std::pair<Graph::PathLength,Graph::NodeId> >,
            std::greater< std::pair<Graph::PathLength,Graph::NodeId> >
                > candidates;

    //erste dijkstra-Iterationen für die Basen
    for(auto curr_base : _set_of_bases) {

        //Schleife über die zu curr_base inzidenten Kanten
        std::vector<Graph::EdgeId> curr_base_edges = input_graph.get_node(curr_base).incidence_vect();
        for(auto curr_edge_id: curr_base_edges){
            Graph::Edge curr_edge = input_graph.get_edge(curr_edge_id);
            //finde den Nachbarknoten der zu der aktuellen Kante gehört
            Graph::NodeId curr_neighbor = curr_edge.get_other_node(curr_base);

            //aktualisiere ggf. _dist_to_base sowie _predecessor für diesen Nachbarknoten und füge entsprechendes Element zu candidates hinzu
            //bemerke, dass _dist_to_base[curr_neighbor] > _dist_to_base[curr_base] + curr_edge.weight() gdw. _dist_to_base[curr_neighbor] > curr_edge.weight()
            if( _dist_to_base[curr_neighbor] > curr_edge.weight() ){
                _dist_to_base[curr_neighbor] = curr_edge.weight();
                _predecessor[curr_neighbor] = {curr_base, curr_edge_id};
                candidates.push({curr_edge.weight(), curr_neighbor});
            }
        }

    }

    int debug_count_while_loop = 0;
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
            _base[next_node] = _base[_predecessor[next_node].first];

            //Schleife über die zu next_node inzidenten Kanten
            std::vector<Graph::EdgeId> next_node_edges = input_graph.get_node(next_node).incidence_vect();
            for (auto curr_edge_id: next_node_edges) {
                Graph::Edge curr_edge = input_graph.get_edge(curr_edge_id);
                //finde den Nachbarknoten der zu der aktuellen Kante gehört
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

    return;
}

void Voronoi_diagram::assign_set_of_bases(std::vector<Graph::NodeId> new_set_of_bases) {
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

void Voronoi_diagram::print_simple(Graph& original_graph) const {
    std::cout << "Ausgabe des Voronoi-Diagramms (als NodeNames ausgegeben)\n";
    std::cout << "(Knoten) (Basis) (Distanz zur Basis) (Vorgaengerknoten und -kante auf kuerz. Weg von Basis) \n";
    for(Graph::NodeId i = 0; i<_num_nodes; i++){
        print_node(i);
        std::cout << " ";
        print_node(_base[i]);
        std::cout << " ";
        print_pathlength(_dist_to_base[i]);
        std::cout << " ";
        print_node(_predecessor[i].first);
        std::cout << " ";
        original_graph.print_edge_as_pair(_predecessor[i].second);
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
            print_node(i);
            std::cout << " ";
            print_pathlength(_dist_to_base[i]);
            std::cout << " ";
            print_node(_predecessor[i].first);
            std::cout << "\n";
        }
    }
}

void Voronoi_diagram::print_node(Graph::NodeId id) const {
    if(id != Graph::invalid_node_id){
        std::cout << id +1;
    }else{
        std::cout << " 'invalid_node' ";
    }
}

void Voronoi_diagram::print_pathlength(Graph::PathLength l) const {
    if(l != Graph::infinite_length){
        std::cout << l;
    }else{
        std::cout << " 'infinite_length' ";
    }
}

bool Voronoi_diagram::check_if_base(Graph::NodeId var_node) const {
    //Knoten ist Basis gdw. wenn er sich selbst als Basis hat
    if( _base_ids[var_node] != invalid_base_id){
        return true;
    }else {
        return false;
    }
}

std::vector<std::vector<std::pair<Graph::EdgeId, Graph::PathLength>>> Voronoi_diagram::min_bound_edges(Graph& input_graph) const {

    //Initialisierung der Ausgabe-Matrix: output[i][j] entspricht den Basen i+1, j
    std::vector<std::vector<std::pair<Graph::EdgeId, Graph::PathLength>>> output;
    std::pair<Graph::EdgeId, Graph::PathLength> invalid_pair = {Graph::invalid_edge_id, Graph::infinite_length};
    for(unsigned int i = 0; i < _num_bases-1; i++){
        std::vector<std::pair<Graph::EdgeId, Graph::PathLength>> var_vector (i+1, invalid_pair);
        output.push_back(var_vector);
    }

    //Schleife über alle Kanten des Graphen, die boundary edges sind
    std::vector<Graph::Edge> input_graph_edges = input_graph.edges();
    for(auto curr_edge : input_graph_edges){
        if( check_if_bound_edge(curr_edge) ){

            //prüfe, ob aktuelle Kante besser als gespeicherte Kante (update falls ja)
            std::array<Voronoi_diagram::BaseId, 2> curr_bases = get_base_ids_ordered( _base[curr_edge.node_a()], _base[curr_edge.node_b()]);
            Graph::PathLength curr_dist = curr_edge.weight() + _dist_to_base[ curr_edge.node_a() ] + _dist_to_base[ curr_edge.node_b() ];
            if( curr_dist < output[ curr_bases[1]-1 ][ curr_bases[0] ].second){
                output[ curr_bases[1]-1 ][ curr_bases[0] ].first = curr_edge.edge_id();
                output[ curr_bases[1]-1 ][ curr_bases[0] ].second = curr_dist;
            }

        }
    }

    return output;
}

void Voronoi_diagram::print_min_bound_edges(Graph& input_graph) const {
    std::vector<std::vector<std::pair<Graph::EdgeId, Graph::PathLength>>> output = min_bound_edges(input_graph);

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
            input_graph.print_edge_as_pair( output[i][j].first );
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
            input_graph.print_pathlength( output[i][j].second );
            std::cout << ", ";
        }
        std::cout << "\n";
    }

}

std::array<Voronoi_diagram::BaseId, 2> Voronoi_diagram::get_base_ids_ordered(Graph::NodeId node_one, Graph::NodeId node_two) const{
    //Checks
    if(node_one == node_two){
        throw std::runtime_error("(std::array<Voronoi_diagram::BaseId, 2> Voronoi_diagram::get_base_ids_ordered) Knoten sind gleich");
    }
    if(node_one > _num_nodes || node_two > _num_nodes){
        throw std::runtime_error("(std::array<Voronoi_diagram::BaseId, 2> Voronoi_diagram::get_base_ids_ordered) NodeIds zu groß");
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

bool Voronoi_diagram::check_if_bound_edge(Graph::Edge var_edge) const {
    if(var_edge.node_a()>=_num_nodes || var_edge.node_b()>=_num_nodes){
        throw std::runtime_error("(Voronoi_diagram::check_if_bound_edge) Knoten der Kante sind nicht im Graphen");
        //? wenn original_graph als Attribut, dann hier checken, ob Kante im Graph
    }

    Graph::NodeId base_a = _base[var_edge.node_a()];
    Graph::NodeId base_b = _base[var_edge.node_b()];

    if( base_a != base_b && check_if_base(base_a) && check_if_base(base_b) ) {
        return true;
    } else {
        return false;
    }
}

Graph Voronoi_diagram::construct_aux_graph(Graph& input_graph) const{
    Graph output(0);

    //Basen als Knoten hinzufügen
    for( auto curr_base : _set_of_bases){
        output.add_one_node( curr_base+1, input_graph.get_node(curr_base).terminal_state() );
    }

    //Kanten hinzufügen
    std::vector<std::vector<std::pair<Graph::EdgeId, Graph::PathLength>>> vect_min_bound_edges = min_bound_edges(input_graph);
    for(unsigned int i = 0; i < _num_bases-1; i++){
        for(unsigned int j = 0; j < i+1; j++){
            if( vect_min_bound_edges[i][j].first != Graph::invalid_edge_id ){
                output.add_edge( j, i+1, vect_min_bound_edges[i][j].second);
            }
        }
    }

    return output;
}

Graph Voronoi_diagram::turn_into_subgraph(Graph &var_graph, Graph &original_graph,
                                          std::vector<std::vector<std::pair<Graph::EdgeId, Graph::PathLength>>> &vect_min_bound_edges) const {
    Graph output = original_graph.copygraph_wo_edges();

    //speichert die schon hinzugefügten Kanten, sodass wir keine Kanten mehrfach hinzufügen
    std::vector<bool> added_edges(original_graph.num_edges(), false);

    for( auto curr_edge : var_graph.edges() ) {
        //zugehörige boundary edge des ursprünglichen Graphen finden
        // ? wenn ich Graph gerichtet machen kann, könnte ich das hier ggf. benutzen (anstatt von der nächsten Zeile)
        std::array<Graph::NodeId, 2> curr_edge_nodes = curr_edge.get_nodes_orderedbyid();
        Graph::Edge original_bound_edge = original_graph.get_edge( vect_min_bound_edges[curr_edge_nodes[1] - 1][curr_edge_nodes[0]].first );

        //diese Kante hinzufügen
        output.add_edge(original_bound_edge.node_a(), original_bound_edge.node_b(), original_bound_edge.weight());
        added_edges[original_bound_edge.edge_id()] = true;

        //Kanten auf den jeweiligen kürz. Wegen von den Endpunkten der aktuellen Kante zu deren jeweiliger Basis hinzufügen
        Graph::NodeId var_node_id = original_bound_edge.node_a();
        Graph::EdgeId edge_to_add_id = _predecessor[var_node_id].second;
        while( edge_to_add_id != Graph::invalid_edge_id && not added_edges[ edge_to_add_id ] ){
            output.add_edge( original_graph.get_edge( edge_to_add_id ).node_a(),
                             original_graph.get_edge( edge_to_add_id ).node_b(),
                             original_graph.get_edge( edge_to_add_id ).weight() );
            added_edges[ edge_to_add_id ] = true;

            var_node_id = _predecessor[var_node_id].first;
            edge_to_add_id = _predecessor[var_node_id].second;
        }

        var_node_id = original_bound_edge.node_b();
        edge_to_add_id = _predecessor[var_node_id].second;
        while( edge_to_add_id != Graph::invalid_edge_id && not added_edges[ edge_to_add_id ] ){
            output.add_edge( original_graph.get_edge( edge_to_add_id ).node_a(),
                             original_graph.get_edge( edge_to_add_id ).node_b(),
                             original_graph.get_edge( edge_to_add_id ).weight() );
            added_edges[ edge_to_add_id ] = true;

            var_node_id = _predecessor[var_node_id].first;
            edge_to_add_id = _predecessor[var_node_id].second;
        }

    }

    output = output.copygraph_wo_iso_nodes();

    return output;
}
