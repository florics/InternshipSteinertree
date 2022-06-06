//
// Created by Lukas on 02.06.2022.
//

#include "voronoi_diagram.h"
#include <utility>
#include <iostream>
#include <queue>
#include <functional>
//? #include <algorithm>


Voronoi_diagram::Voronoi_diagram(unsigned int num_n){
    _num_nodes = num_n;
    _num_bases = 0;

    std::vector<Graph::NodeId> var_set_of_bases;
    _set_of_bases = var_set_of_bases;

    _base.assign(num_n, Graph::invalid_node_id);

    std::pair< Graph::NodeId, Graph::EdgeId> var_pair = {Graph::invalid_node_id, Graph::invalid_edge_id};
    _predecessor.assign(num_n, var_pair);

    std::vector<Graph::PathLength> var_dist_to_base (num_n, Graph::infinite_length);
    _dist_to_base = var_dist_to_base;
}

//todo: reached wieder rausnehmen, pre... und dist... wieder aktualisieren, auf Abfragen achten (wenn ich durch die Nachbarschaft des nächsten Knoten gehe)
//(todo) candidates wieder in anderes Format (nur mit dem Knoten)
Voronoi_diagram::Voronoi_diagram(std::vector<Graph::NodeId> set_of_b, Graph g){

    //Prüfe, ob Eingabe Bedingungen erfüllt
    //für die Menge der Basen set_of_b
    if(set_of_b.size() == 0){
        throw std::runtime_error("(constructor Voronoi_diagram) set_of_b ist leer");
    }
    for(auto var_node : set_of_b){
        if(var_node >= g.num_nodes()){
            std::cout << "Basis mit NodeId " << var_node << " ist nicht im Graph g.";
            throw std::runtime_error("(constructor Voronoi_diagram) set_of_b ist keine Teilmenge der Knoten von g");
        }
    }
    //und für die Kantengewichte von dem Graphen g
    if(! g.edgeweight_nonnegative()){
        throw std::runtime_error("(constructor Voronoi_diagram) es gibt negative Kantengewichte in g");
    }
    if( ! g.edgeweight_finite() ){
        throw std::runtime_error("(constructor Voronoi_diagram) es gibt Kante in g mit Gewicht 'numeric_limits <double>::max()' ");
    }

    //Initialisieren der Attribute der Klasse Voronoi-D.

    _num_nodes = g.num_nodes();

    _base.assign(_num_nodes, Graph::invalid_node_id);

    std::pair< Graph::NodeId, Graph::EdgeId> var_pair = {Graph::invalid_node_id, Graph::invalid_edge_id};
    _predecessor.assign(_num_nodes, var_pair);

    std::vector<Graph::PathLength> var_dist_to_base (_num_nodes, Graph::infinite_length);
    _dist_to_base = var_dist_to_base;

    assign_set_of_bases(set_of_b);

    //Initialisierung der priority_queue, mit der wir in jeder Iteration den nächsten besten Knoten (Kandidat) finden
    //std::greater< std::pair<Graph::PathLength,Graph::NodeId> > sorgt dafür, dass wir immer an den Knoten mit der kleinsten Distanz gelangen
    std::priority_queue<
            std::pair<Graph::PathLength,Graph::NodeId>,
            std::vector< std::pair<Graph::PathLength,Graph::NodeId> >,
            std::greater< std::pair<Graph::PathLength,Graph::NodeId> >
                > candidates;

    //? Alternative: alles in die while Schleife (also inkl. erste dijkstra-Iterationen für die Basen)
    //? und _predecessor[basis] = basis, dann brauchen wir reached auch nicht mehr,
    //? aber Nachteil weil diese Form von _predecessor Probleme machen könnte

    //erste dijkstra-Iterationen für die Basen
    for(auto curr_base : _set_of_bases) {

        //Schleife über die zu curr_base inzidenten Kanten
        std::vector<Graph::EdgeId> curr_base_edges = g.get_node(curr_base).incidence_vect();
        for(auto curr_edge_id: curr_base_edges){
            Graph::Edge curr_edge = g.get_edge(curr_edge_id);
            //finde den Nachbarknoten der zu der aktuellen Kante gehört
            Graph::NodeId curr_neighbor = curr_edge.get_other_node(curr_base);

            //aktualisiere ggf. _dist_to_base sowie _predecessor für diesen Nachbarknoten und füge entsprechendes Element zu candidates hinzu
            //bemerke, dass !reached[curr_neighbor] && _dist_to_base[curr_neighbor] > _dist_to_base[curr_base] + curr_edge.weight() gdw. ????
            //bemerke, dass _dist_to_base[curr_neighbor] > _dist_to_base[curr_base] + curr_edge.weight() gdw. _dist_to_base[curr_neighbor] > curr_edge.weight()
            if( _dist_to_base[curr_neighbor] > curr_edge.weight() ){
                _dist_to_base[curr_neighbor] = curr_edge.weight();
                _predecessor[curr_neighbor] = {curr_base, curr_edge_id};
                candidates.push({curr_edge.weight(), curr_neighbor});
            }
        }

    }

    int debug_count_while_loop = 0;
    while( !candidates.empty() ) {
        debug_count_while_loop++;
        std::cout << "Iteration while-Schleife nr. " << debug_count_while_loop << std::endl;

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
            std::cout << dist_of_next_cand << " < " << _dist_to_base[next_node] << std::endl;
        }

        if (dist_of_next_cand == _dist_to_base[next_node]) {

            //füge zur Basis des Vorgängers hinzu
            _base[next_node] = _base[_predecessor[next_node].first];

            //Schleife über die zu next_node inzidenten Kanten
            std::vector<Graph::EdgeId> next_node_edges = g.get_node(next_node).incidence_vect();
            for (auto curr_edge_id: next_node_edges) {
                Graph::Edge curr_edge = g.get_edge(curr_edge_id);
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
    for(auto var_base: _set_of_bases){
        _base[var_base] = var_base;
        _dist_to_base[var_base] = 0;
    }
}

void Voronoi_diagram::print_simple() const {
    std::cout << "Ausgabe des Voronoi-Diagramms (als NodeNames ausgegeben)\n";
    std::cout << "(Knoten) (Basis) (Distanz zur Basis) (Vorgaenger auf kuerz. Weg von Basis) \n";
    for(Graph::NodeId i = 0; i<_num_nodes; i++){
        print_node(i);
        std::cout << " ";
        print_node(_base[i]);
        std::cout << " ";
        print_pathlength(_dist_to_base[i]);
        std::cout << " ";
        print_node(_predecessor[i].first);
        std::cout << std::endl;
    }
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
            std::cout << std::endl;
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
    if( _base[var_node] == var_node){
        return true;
    }else {
        return false;
    }
}



