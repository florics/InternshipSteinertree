//
// Created by Lukas on 02.06.2022.
//

#include <utility>
#include <iostream>
#include <queue>
#include <tuple>
//? #include <functional>
//? #include <algorithm>

#include "voronoi_diagram.h"

#include "graph_aux_functions.h"
#include "graph_printfunctions.h"

Voronoi_diagram::Voronoi_diagram(Graph& input_graph):
    _original_graph(input_graph)
{
    //? Checks fehlen (siehe anderer Konstruktor)
    // diesen Konstruktor vll löschen ?

    unsigned int num_nodes = input_graph.num_nodes();
    _num_bases = 0;

    _base.assign(num_nodes, Graph::invalid_node_id);

    std::pair< Graph::NodeId, Graph::EdgeId> invalid_pair = {Graph::invalid_node_id, Graph::invalid_edge_id};
    _predecessor.assign(num_nodes, invalid_pair);

    _dist_to_base.assign(num_nodes, Graph::infinite_length);
}

Voronoi_diagram::Voronoi_diagram(const std::vector<Graph::NodeId>& set_of_b, const Graph& input_graph):
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
    if(not GraphAux::edgeweight_nonnegative(_original_graph)){
        throw std::runtime_error("(constructor Voronoi_diagram) es gibt negative Kantengewichte im Eingabegraphen");
    }
    if(not GraphAux::edgeweight_finite(_original_graph) ){
        throw std::runtime_error("(constructor Voronoi_diagram) es gibt Kante im Eingabegraphen mit 'unendlichem' Gewicht");
    }

    //Initialisieren der Attribute der Klasse Voronoi-D.

    unsigned int num_nodes = _original_graph.num_nodes();

    _base.assign(num_nodes, Graph::invalid_node_id);

    std::pair< Graph::NodeId, Graph::EdgeId> invalid_pair = {Graph::invalid_node_id, Graph::invalid_edge_id};
    _predecessor.assign(num_nodes, invalid_pair);

    _dist_to_base.assign(num_nodes, Graph::infinite_length);

    assign_bases(set_of_b);

    //Initialisierung der priority_queue, mit der wir in jeder Iteration den nächsten besten Knoten (Kandidat) finden
    //std::greater< std::pair<Graph::PathLength,Graph::NodeId> > sorgt dafür, dass wir immer an den Knoten mit der kleinsten Distanz gelangen
    std::priority_queue<
            std::pair<Graph::PathLength,Graph::NodeId>,
            std::vector< std::pair<Graph::PathLength,Graph::NodeId> >,
            std::greater< std::pair<Graph::PathLength,Graph::NodeId> >
                > candidates;
    // Basen sind die ersten Kandidaten
    for(auto curr_base : set_of_b) {
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
            std::vector<Graph::EdgeId> next_node_edges = _original_graph.get_node(next_node).incident_edge_ids();
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

Voronoi_diagram::RestoreData Voronoi_diagram::repair(const std::vector<Graph::NodeId>& bases_to_delete){
    //prüfe die Eingabe (ersten und letzten Check weglassen?)
    /*
    if(bases_to_delete.empty()){
        throw std::runtime_error("(Voronoi_diagram::repair) Eingabemenge ist leer");
    }
     */
    for(auto curr_old_base : bases_to_delete){
        if( curr_old_base >= _original_graph.num_nodes() ) {
            throw std::runtime_error("(Voronoi_diagram::repair) Knoten aus Eingabemenge liegt nicht im zugrundeliegenden Graphen");
        }
    }
    for(auto curr_old_base : bases_to_delete){
        if( not check_if_base(curr_old_base) ) {
            throw std::runtime_error("(Voronoi_diagram::repair) Knoten aus Eingabemenge liegt nicht in urspruenglicher Basis");
        }
    }
    if(bases_to_delete.size() == _num_bases){
        throw std::runtime_error("(Voronoi_diagram::repair) Eingabemenge entspricht Menge aller Basen");
    }

    _num_bases -= bases_to_delete.size();

    //finde alle Knoten, deren Basis etc. aktualisiert werden müssen; im Paper wird diese Menge mit 'C' notiert
    std::vector<Graph::NodeId> nodes_to_update = compute_some_vor_regions(bases_to_delete);

    //bereite die Ausgabe vor
    RestoreData output_restore_data = get_restoredata_of_nodeset(nodes_to_update);

    //Initialisierung der priority_queue, mit der wir in jeder Iteration den nächsten besten Knoten (Kandidat) finden
    //std::greater< std::pair<Graph::PathLength,Graph::NodeId> > sorgt dafür, dass wir immer an den Knoten mit der kleinsten Distanz gelangen
    std::priority_queue<
            std::pair<Graph::PathLength,Graph::NodeId>,
            std::vector< std::pair<Graph::PathLength,Graph::NodeId> >,
            std::greater< std::pair<Graph::PathLength,Graph::NodeId> >
    > candidates;

    // 'first stage' jeder Knoten aus C wird zu der Region seines besten Nachbarn (der nicht in C liegt) (wenn ein solcher existiert) hinzugefügt
    //Zurücksetzen der Knoten aus C
    for(auto curr_node_id : nodes_to_update) {
        _base[curr_node_id] = Graph::invalid_node_id;
        _predecessor[curr_node_id] = {Graph::invalid_node_id, Graph::invalid_edge_id};
        _dist_to_base[curr_node_id] = Graph::infinite_length;
    }
    //die eigentliche Initialisierung
    for(auto curr_node_id : nodes_to_update) {
        for(auto curr_edge_id : _original_graph.get_node(curr_node_id).incident_edge_ids() ) {
            Graph::Edge curr_edge = _original_graph.get_edge(curr_edge_id);
            Graph::NodeId curr_neighbor_id = curr_edge.get_other_node(curr_node_id);

            //im Unterschied zum Paper wird hier nicht jedes Mal gecheckt, dass der aktuelle Nachbar nicht in C liegt
            // wegen der vorherigen Schleife ("Zurücksetzen") ist die Bedingung der if-Abfrage aber nur für solche Knoten erfüllt,
            // sowie für Knoten aus C, die bereits zu einer neuen Basis hinzugefügt wurden
            if( _dist_to_base[curr_node_id] > _dist_to_base[curr_neighbor_id] + curr_edge.weight() ) {
                _base[curr_node_id] = _base[curr_neighbor_id];
                _predecessor[curr_node_id] = {curr_neighbor_id, curr_edge_id};
                _dist_to_base[curr_node_id] = _dist_to_base[curr_neighbor_id] + curr_edge.weight();
            }
        }

        candidates.push({_dist_to_base[curr_node_id], curr_node_id});
    }

    // 'second stage' modified dijkstra, entspricht der Schleife aus dem Konstruktor
    // es werden (automatisch) nur Knoten aus C bzw. aus nodes_to_update aktualisiert
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
                std::cout << "Kandidat: " << next_node << "\n";
                throw std::runtime_error("(Voronoi_diagram::repair) Kandidat ist Basis");
            }

            //füge zur Basis des Vorgängers hinzu
            //bemerke, dass _predecessor[next_node] 'wohldefiniert' ist, da next_node niemals Basis sein kann,
            // da nur Knoten aus node_to_update zu candidates hinzugefügt werden (bemerkung löschen?)
            _base[next_node] = _base[_predecessor[next_node].first];

            //Schleife über die zu next_node inzidenten Kanten
            std::vector<Graph::EdgeId> next_node_edges = _original_graph.get_node(next_node).incident_edge_ids();
            for (auto curr_edge_id: next_node_edges) {
                Graph::Edge curr_edge = _original_graph.get_edge(curr_edge_id);
                Graph::NodeId curr_neighbor = curr_edge.get_other_node(next_node);

                //aktualisiere ggf. _dist_to_base sowie _predecessor für diesen Nachbarknoten und füge entsprechendes Element zu candidates hinzu
                // es werden nur Knoten aus C geupdatet, da für Knoten, deren Basis sich nicht ändert, die Bedingung der if-Abfrage nie erfüllt ist (?)
                if (_dist_to_base[curr_neighbor] > _dist_to_base[next_node] + curr_edge.weight()) {
                    _dist_to_base[curr_neighbor] = _dist_to_base[next_node] + curr_edge.weight();
                    _predecessor[curr_neighbor] = {next_node, curr_edge_id};
                    candidates.push({_dist_to_base[next_node] + curr_edge.weight(), curr_neighbor});
                }
                /*
                 für die lokalen Suchen hier boundary edges finden?
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

    return output_restore_data;
}


Voronoi_diagram::RestoreData Voronoi_diagram::get_restoredata_of_nodeset (const std::vector<Graph::NodeId>& input_nodeids) {

    std::vector<Graph::NodeId> old_bases_of_nodes_to_update;
    std::vector<std::pair<Graph::NodeId, Graph::EdgeId>> old_predecessor_of_nodes_to_update;
    std::vector<Graph::PathLength> old_dist_to_base_of_nodes_to_updates;

    for(auto curr_nodeid: input_nodeids) {
        if( curr_nodeid == Graph::invalid_node_id) {
            throw std::runtime_error("(Voronoi_diagram::get_restoredata_of_nodeset) ein Eingabeknoten ist ungueltig.");
        }
        if( curr_nodeid >= _original_graph.num_nodes()) {
            throw std::runtime_error("(Voronoi_diagram::get_restoredata_of_nodeset) ein Eingabeknoten liegt nicht im zugrundeliegenden Graphen.");
        }

        old_bases_of_nodes_to_update.push_back( _base[curr_nodeid] );
        old_predecessor_of_nodes_to_update.push_back( _predecessor[curr_nodeid] );
        old_dist_to_base_of_nodes_to_updates.push_back(_dist_to_base[curr_nodeid]);
    }

    //struct Voronoi_diagram::RestoreData output(input_nodeids, old_bases_of_nodes_to_update, old_predecessor_of_nodes_to_update, old_dist_to_base_of_nodes_to_updates);

    return {input_nodeids, old_bases_of_nodes_to_update, old_predecessor_of_nodes_to_update, old_dist_to_base_of_nodes_to_updates};
}


void Voronoi_diagram::restore(const Voronoi_diagram::RestoreData& input_restore_data) {
    std::vector<Graph::NodeId> nodes_to_update = input_restore_data.node_ids;
    std::vector<Graph::NodeId> old_bases_of_nodes_to_update = input_restore_data.bases;
    std::vector<std::pair<Graph::NodeId, Graph::EdgeId>> old_predecessor_of_nodes_to_update = input_restore_data.predecessor;
    std::vector<Graph::PathLength> old_dist_to_base_of_nodes_to_updates = input_restore_data.dist_to_base;

    for( unsigned int i = 0; i < nodes_to_update.size(); i++) {
        Graph::NodeId curr_nodeid = nodes_to_update[i];
        _base[curr_nodeid] = old_bases_of_nodes_to_update[i];
        _predecessor[curr_nodeid] = old_predecessor_of_nodes_to_update[i];
        _dist_to_base[curr_nodeid] = old_dist_to_base_of_nodes_to_updates[i];
    }

}


void Voronoi_diagram::assign_bases(const std::vector<Graph::NodeId>& new_set_of_bases) {
    _num_bases = new_set_of_bases.size();

    if(_num_bases > original_graph().num_nodes()){
        throw std::runtime_error("(void Voronoi_diagram::assign_set_of_bases) neue Menge der Basen zu groß");
    }

    for(unsigned int i = 0; i<_num_bases; i++){
        Graph::NodeId var_base = new_set_of_bases[i];
        _base[var_base] = var_base;
        _dist_to_base[var_base] = 0;
    }
}

void Voronoi_diagram::print_simple() const {
    std::cout << "Ausgabe des Voronoi-Diagramms (als NodeNames ausgegeben)\n";
    std::cout << "(Knoten) (Basis) (Distanz zur Basis) (Vorgaengerknoten und -kante auf kuerz. Weg von Basis) \n";
    for(Graph::NodeId i = 0; i<original_graph().num_nodes(); i++){
        GraphAuxPrint::print_nodename(_original_graph, i);
        std::cout << " ";
        GraphAuxPrint::print_nodename(_original_graph, _base[i]);
        std::cout << " ";
        GraphAuxPrint::print_pathlength(_dist_to_base[i]);
        std::cout << " ";
        GraphAuxPrint::print_nodename(_original_graph, _predecessor[i].first);
        std::cout << " ";
        GraphAuxPrint::print_edge_as_pair(_original_graph, _predecessor[i].second);
        std::cout << "\n";
    }
    std::cout << "\n";
}

void Voronoi_diagram::print_region_naiv(Graph::NodeId input_base) const {
    if(!check_if_base(input_base)){
        std::cout << "(void Voronoi_diagram::print_region_naiv(Graph::NodeId var_base)) var_base = " << input_base << " ist keine Basis \n";
    }
    std::cout << "Die Voronoi-Region des Knotens ";
    GraphAuxPrint::print_nodename(_original_graph, input_base);
    std::cout << " ist: (als NodeNames ausgegeben)\n";
    std::cout << "(Knoten) (Distanz zur Basis) (Vorgaenger auf kuerz. Weg von Basis) \n";
    unsigned int region_num_nodes = 0;
    for(Graph::NodeId i=0; i<original_graph().num_nodes(); i++){
        if(_base[i] == input_base){
            GraphAuxPrint::print_nodename(_original_graph, i);
            std::cout << " ";
            GraphAuxPrint::print_pathlength(_dist_to_base[i]);
            std::cout << " ";
            GraphAuxPrint::print_nodename(_original_graph, _predecessor[i].first);
            std::cout << "\n";

            region_num_nodes++;
        }
    }
    std::cout << "Insgesamt liegen " << region_num_nodes << " Knoten in dieser Voronoi-Region. \n";
}

void Voronoi_diagram::print_region_fast(Graph::NodeId input_base) {
    if(!check_if_base(input_base)){
        std::cout << "(void Voronoi_diagram::print_region_fast(Graph::NodeId var_base)) var_base = " << input_base << " ist keine Basis \n";
    }
    std::cout << "Die Voronoi-Region des Knotens ";
    GraphAuxPrint::print_nodename(_original_graph, input_base);
    std::cout << " ist: (als NodeNames ausgegeben)\n";
    std::cout << "(Knoten) (Distanz zur Basis) (Vorgaenger auf kuerz. Weg von Basis) \n";
    for(auto curr_node : compute_vor_region(input_base)){
            GraphAuxPrint::print_nodename(_original_graph, curr_node);
            std::cout << " ";
            GraphAuxPrint::print_pathlength(_dist_to_base[curr_node]);
            std::cout << " ";
            GraphAuxPrint::print_nodename(_original_graph, _predecessor[curr_node].first);
            std::cout << "\n";
    }
    std::cout << "Insgesamt liegen " << compute_vor_region(input_base).size() << " Knoten in dieser Voronoi-Region. \n";
}

bool Voronoi_diagram::check_if_base(Graph::NodeId var_node) const {
    if(var_node == Graph::invalid_node_id){
        return false;
    }
    if( var_node > original_graph().num_nodes() ){
        throw std::runtime_error("(Voronoi_diagram::check_if_base) Knoten liegt nicht im zugrundeliegenden Graphen");
    }
    //Knoten ist Basis gdw. wenn er sich selbst als Basis hat
    if( _base[var_node] == var_node){
        return true;
    }else {
        return false;
    }
}

bool Voronoi_diagram::check_if_bound_edge(const Graph::Edge& var_edge) const {
    if(var_edge.node_a()>=original_graph().num_nodes() || var_edge.node_b()>=original_graph().num_nodes()){
        throw std::runtime_error("(Voronoi_diagram::check_if_bound_edge) Knoten der Kante sind nicht im Graphen");
    }
    if(var_edge.edge_id()>=original_graph().num_edges() ){
        throw std::runtime_error("(Voronoi_diagram::check_if_bound_edge) Kante ist nicht im Graphen");
    }

    Graph::NodeId base_a = _base[var_edge.node_a()];
    Graph::NodeId base_b = _base[var_edge.node_b()];

    if( base_a != base_b && check_if_base(base_a) && check_if_base(base_b) ) {
        return true;
    } else {
        return false;
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

const std::vector<std::pair<Graph::NodeId, Graph::EdgeId>>& Voronoi_diagram::predecessor() const {
    return _predecessor;
}

unsigned int Voronoi_diagram::num_bases() const {
    return _num_bases;
}

std::vector<Graph::NodeId> Voronoi_diagram::compute_set_of_bases() const {
    std::vector<Graph::NodeId> set_of_bases;

    for( Graph::NodeId id = 0; id < original_graph().num_nodes(); id++) {
        if(check_if_base(id)) {
            set_of_bases.push_back(id);
        }
    }

    return set_of_bases;
}

std::vector<Voronoi_diagram::BaseId> Voronoi_diagram::compute_base_ids() const {
    std::vector<Graph::NodeId> base_ids (_original_graph.num_nodes(), Graph::invalid_node_id);

    std::vector<Graph::NodeId> set_of_bases = compute_set_of_bases();

    for(unsigned int i=0; i<set_of_bases.size(); i++) {
        Graph::NodeId curr_base = set_of_bases[i];
        base_ids[curr_base] = i;
    }

    return base_ids;
}

std::vector<Graph::NodeId> Voronoi_diagram::compute_vor_region(Graph::NodeId input_base) {
    if(not check_if_base(input_base)) {
        throw std::runtime_error("(Voronoi_diagram::compute_vor_region) Eingabeknoten ist keine Basis.");
    }

    // Ausgabe
    std::vector<Graph::NodeId> vor_region(1, input_base);

    //es folgt eine Graphendurchmusterung

    std::vector<Graph::NodeId> next_nodes(1, input_base);

    //Markierung als 'betrachtet', siehe unten
    _base[input_base] = Graph::invalid_node_id;

    while(not next_nodes.empty()) {
        Graph::NodeId curr_node_id = next_nodes.back();
        next_nodes.pop_back();

        for(auto curr_neighbor_id: original_graph().adjacency_vect(curr_node_id) ) {
            if(_base[curr_neighbor_id] == input_base) {
                next_nodes.push_back(curr_neighbor_id);

                //markiere den Knoten als "betrachtet", um die Rekursion nicht mehrfach für den gleichen Knoten aufzurufen
                //das ist der Grund, weshalb diese Funktion (sowie alle, die sie benutzen) nicht const ist
                // (ein Vektor mit Länge original_nodes.num_nodes() würde zu Laufzeit O(n) führen)
                _base[curr_neighbor_id] = Graph::invalid_node_id;

                vor_region.push_back(curr_neighbor_id);
            }
        }

    }

    //mache die Markierungen rückgängig
    for(auto var_node_id : vor_region) {
        _base[var_node_id] = input_base;
    }

    return vor_region;
}

void Voronoi_diagram::compute_vor_region_subroutine(Graph::NodeId input_base, Graph::NodeId curr_node_id,
                                                    std::vector<Graph::NodeId>& vor_region) {
    vor_region.push_back(curr_node_id);
    //markiere den Knoten als "betrachtet", um die Rekursion nicht mehrfach für den gleichen Knoten aufzurufen
    //das ist der Grund, weshalb diese Funktion (sowie alle, die sie benutzen) nicht const ist
    // (ein Vektor mit Länge original_nodes.num_nodes() würde zu Laufzeit O(n) führen)
    _base[curr_node_id] = Graph::invalid_node_id;

    for( auto curr_neighbor_id: original_graph().adjacency_vect(curr_node_id) ){

        if (_base[curr_neighbor_id] == input_base) {
            compute_vor_region_subroutine(input_base, curr_neighbor_id, vor_region);
        }

    }

}

std::vector< Graph::NodeId > Voronoi_diagram::compute_some_vor_regions(const std::vector<Graph::NodeId>& subset_of_bases) {
    std::vector< Graph::NodeId > output;

    for( auto curr_base : subset_of_bases) {
        if(check_if_base(curr_base)) {
            const std::vector<Graph::NodeId>& curr_vor_region = compute_vor_region(curr_base);
            output.insert( output.end(), curr_vor_region.begin(), curr_vor_region.end() );
        } else {
            throw std::runtime_error("(Voronoi_diagram::compute_some_vor_regions) Eingabemenge keine Teilmenge der Basen.");
        }
    }

    return output;
}

