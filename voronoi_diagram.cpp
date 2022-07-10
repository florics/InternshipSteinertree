//
// Created by Lukas on 02.06.2022.
//

#include <utility>
#include <iostream>
#include <queue>

#include "voronoi_diagram.h"

#include "graph_aux_functions.h"

Voronoi_diagram::Voronoi_diagram(const std::vector<Graph::NodeId>& set_of_bases, const Graph& input_graph):
    _original_graph(input_graph),
    _num_bases(set_of_bases.size())
{

    //Prüfe, ob Eingabe Bedingungen erfüllt:
    //für die Menge der Basen set_of_b
    if(set_of_bases.empty()){
        throw std::runtime_error("(constructor Voronoi_diagram) set_of_b ist leer");
    }
    for(auto var_node : set_of_bases){
        if(var_node >= _original_graph.num_nodes()){
            std::cout << "Basis mit NodeId " << var_node << " ist nicht im Graph g.";
            throw std::runtime_error("(constructor Voronoi_diagram) set_of_b ist keine Teilmenge der Knoten von g");
        }
    }
    //und für die Kantengewichte von dem Graphen g
    if(not GraphAux::check_if_graph_has_nonnegative_weights(_original_graph)){
        throw std::runtime_error("(constructor Voronoi_diagram) es gibt negative Kantengewichte im Eingabegraphen");
    }
    if(not GraphAux::check_if_graph_has_finite_weights(_original_graph) ){
        throw std::runtime_error("(constructor Voronoi_diagram) es gibt Kante im Eingabegraphen mit 'unendlichem' Gewicht");
    }

    //Initialisieren der Attribute der Klasse Voronoi-D.

    unsigned int num_nodes = _original_graph.num_nodes();

    _base.assign(num_nodes, Graph::invalid_node_id);

    std::pair< Graph::NodeId, Graph::EdgeId> invalid_pair = {Graph::invalid_node_id, Graph::invalid_edge_id};
    _predecessor.assign(num_nodes, invalid_pair);

    _dist_to_base.assign(num_nodes, Graph::infinite_length);

    assign_bases(set_of_bases);


    //Initialisierung der priority_queue, mit der wir in jeder Iteration den nächsten besten Knoten (Kandidat) finden
    std::priority_queue<
            std::pair<Graph::PathLength,Graph::NodeId>,
            std::vector< std::pair<Graph::PathLength,Graph::NodeId> >,
            std::greater< std::pair<Graph::PathLength,Graph::NodeId> >
                > candidates;

    // Basen sind die ersten Kandidaten
    for(auto curr_base : set_of_bases) {
        candidates.push( {0, curr_base} );
    }

    while( not candidates.empty() ) {

        //finde nächsten besten Knoten/Kandidaten
        const Graph::NodeId next_node = candidates.top().second;
        const Graph::PathLength dist_of_next_node = candidates.top().first;
        candidates.pop();

        //prüfe, ob restliche Knoten alle unerreichbar sind
        if (dist_of_next_node == Graph::infinite_length) {
            break;
        }

        //prüfe, ob die Länge des obersten Elements des Heaps zu der gespeicherten Distanz des Knotens des obersten
        // Elements des Heaps gehört (wenn nicht, wurde eine kürzere Länge zu diesem Knoten gefunden und er wurde
        // dementsprechend bereits zu einer Basis/Vor-Region hinzugefügt)
        if (dist_of_next_node == _dist_to_base[next_node]) {

            //füge zur Basis des Vorgängers hinzu
            if(not check_if_base(next_node)){
                _base[next_node] = _base[_predecessor[next_node].first];
            }

            //Schleife über die zu next_node inzidenten Kanten
            std::vector<Graph::EdgeId> next_node_edges = _original_graph.get_node(next_node).incident_edge_ids();
            for (auto curr_edge_id: next_node_edges) {
                const Graph::Edge& curr_edge = _original_graph.get_edge(curr_edge_id);
                const Graph::NodeId curr_neighbor = curr_edge.get_other_node(next_node);

                //aktualisiere ggf. _dist_to_base sowie _predecessor für diesen Nachbarknoten
                // und füge entsprechendes Element zu candidates hinzu
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
    //prüfe die Eingabe
    for(auto curr_old_base : bases_to_delete){
        if( curr_old_base >= _original_graph.num_nodes() ) {
            throw std::runtime_error("(Voronoi_diagram::repair) "
                                     "Knoten aus Eingabemenge liegt nicht im zugrundeliegenden Graphen");
        }
    }
    for(auto curr_old_base : bases_to_delete){
        if( not check_if_base(curr_old_base) ) {
            throw std::runtime_error("(Voronoi_diagram::repair) "
                                     "Knoten aus Eingabemenge liegt nicht in urspruenglicher Basis");
        }
    }
    if(bases_to_delete.size() == _num_bases){
        throw std::runtime_error("(Voronoi_diagram::repair) Eingabemenge entspricht Menge aller Basen");
    }


    _num_bases -= bases_to_delete.size();

    //finde alle Knoten, deren Basis etc. aktualisiert werden müssen; im Paper wird diese Menge mit 'C' notiert
    const std::vector<Graph::NodeId> nodes_to_update = compute_some_vor_regions(bases_to_delete);

    //bereite die Ausgabe vor (bevor das Diagramm bearbeitet wird)
    RestoreData output_restore_data = get_restoredata_of_nodeset(nodes_to_update, bases_to_delete.size());

    //Initialisierung der priority_queue, mit der wir in jeder Iteration den nächsten besten Knoten (Kandidat) finden
    std::priority_queue<
            std::pair<Graph::PathLength,Graph::NodeId>,
            std::vector< std::pair<Graph::PathLength,Graph::NodeId> >,
            std::greater< std::pair<Graph::PathLength,Graph::NodeId> >
    > candidates;

    //'first stage': jeder Knoten aus C wird zu der Region seines besten Nachbarn (der nicht in C liegt)
    // (wenn ein solcher existiert) hinzugefügt:

    //Zurücksetzen der Knoten aus C
    for(auto curr_node_id : nodes_to_update) {
        _base[curr_node_id] = Graph::invalid_node_id;
        _predecessor[curr_node_id] = {Graph::invalid_node_id, Graph::invalid_edge_id};
        _dist_to_base[curr_node_id] = Graph::infinite_length;
    }

    //die eigentliche Initialisierung
    for(auto curr_node_id : nodes_to_update) {
        for(auto curr_edge_id : _original_graph.get_node(curr_node_id).incident_edge_ids() ) {
            const Graph::Edge& curr_edge = _original_graph.get_edge(curr_edge_id);
            const Graph::NodeId curr_neighbor_id = curr_edge.get_other_node(curr_node_id);

            //im Unterschied zum Paper wird hier nicht jedes Mal gecheckt, dass der aktuelle Nachbar nicht in C liegt
            // wegen der vorherigen Schleife ("Zurücksetzen") ist die Bedingung der if-Abfrage aber nur für Knoten,
            // die nicht in C liegen, erfüllt, sowie für Knoten aus C, die bereits zu einer neuen Basis hinzugefügt
            // wurden
            if( _dist_to_base[curr_node_id] > _dist_to_base[curr_neighbor_id] + curr_edge.weight() ) {

                _base[curr_node_id] = _base[curr_neighbor_id];
                _predecessor[curr_node_id] = {curr_neighbor_id, curr_edge_id};
                _dist_to_base[curr_node_id] = _dist_to_base[curr_neighbor_id] + curr_edge.weight();
            }
        }

        candidates.push({_dist_to_base[curr_node_id], curr_node_id});
    }

    //'second stage' (modified dijkstra): entspricht der Schleife aus dem Konstruktor
    // es werden (automatisch) nur Knoten aus C aktualisiert
    while ( not candidates.empty() ) {
        //finde nächsten besten Knoten/Kandidaten
        const Graph::NodeId next_node = candidates.top().second;
        const Graph::PathLength dist_of_next_node = candidates.top().first;
        candidates.pop();

        //prüfe, ob restliche Knoten alle unerreichbar sind
        if (dist_of_next_node == Graph::infinite_length) {
            break;
        }

        //prüfe, ob die Länge des obersten Elements des Heaps zu der gespeicherten Distanz des Knotens des obersten
        // Elements des Heaps gehört (wenn nicht, wurde eine kürzere Länge zu diesem Knoten gefunden und er wurde
        // dementsprechend bereits zu einer Basis/Vor-Region hinzugefügt
        if (dist_of_next_node == _dist_to_base[next_node]) {

            //füge zur Basis des Vorgängers hinzu
            //bemerke, dass _predecessor[next_node] 'wohldefiniert' ist, da next_node niemals Basis sein kann,
            // da nur Knoten aus node_to_update zu candidates hinzugefügt werden
            _base[next_node] = _base[_predecessor[next_node].first];

            //Schleife über die zu next_node inzidenten Kanten
            std::vector<Graph::EdgeId> next_node_edges = _original_graph.get_node(next_node).incident_edge_ids();
            for (auto curr_edge_id: next_node_edges) {
                const Graph::Edge& curr_edge = _original_graph.get_edge(curr_edge_id);
                const Graph::NodeId curr_neighbor = curr_edge.get_other_node(next_node);

                //aktualisiere ggf. _dist_to_base sowie _predecessor für diesen Nachbarknoten
                // und füge entsprechendes Element zu candidates hinzu
                //es werden nur Knoten aus C geupdatet, da für Knoten, deren Basis sich nicht ändert,
                // die Bedingung der if-Abfrage nie erfüllt ist
                if (_dist_to_base[curr_neighbor] > _dist_to_base[next_node] + curr_edge.weight()) {
                    _dist_to_base[curr_neighbor] = _dist_to_base[next_node] + curr_edge.weight();
                    _predecessor[curr_neighbor] = {next_node, curr_edge_id};
                    candidates.push({_dist_to_base[next_node] + curr_edge.weight(), curr_neighbor});
                }
            }
        }
    }

    return output_restore_data;
}


Voronoi_diagram::RestoreData
Voronoi_diagram::get_restoredata_of_nodeset (const std::vector<Graph::NodeId>& input_nodeids,
                                             unsigned int num_bases_deleted) {

    std::vector<Graph::NodeId> old_bases_of_nodes_to_update;
    old_bases_of_nodes_to_update.reserve(input_nodeids.size());

    std::vector<std::pair<Graph::NodeId, Graph::EdgeId>> old_predecessor_of_nodes_to_update;
    old_predecessor_of_nodes_to_update.reserve(input_nodeids.size());

    std::vector<Graph::PathLength> old_dist_to_base_of_nodes_to_updates;
    old_dist_to_base_of_nodes_to_updates.reserve(input_nodeids.size());

    for(auto curr_nodeid: input_nodeids) {
        if( curr_nodeid == Graph::invalid_node_id) {
            throw std::runtime_error("(Voronoi_diagram::get_restoredata_of_nodeset) Ein Eingabeknoten ist ungueltig.");
        }
        if( curr_nodeid >= _original_graph.num_nodes()) {
            throw std::runtime_error("(Voronoi_diagram::get_restoredata_of_nodeset) "
                                     "Ein Eingabeknoten liegt nicht im zugrundeliegenden Graphen.");
        }

        old_bases_of_nodes_to_update.push_back( _base[curr_nodeid] );
        old_predecessor_of_nodes_to_update.push_back( _predecessor[curr_nodeid] );
        old_dist_to_base_of_nodes_to_updates.push_back(_dist_to_base[curr_nodeid]);
    }

    return {input_nodeids, old_bases_of_nodes_to_update, old_predecessor_of_nodes_to_update,
            old_dist_to_base_of_nodes_to_updates, num_bases_deleted};
}


void Voronoi_diagram::restore(const Voronoi_diagram::RestoreData& input_restore_data) {

    const std::vector<Graph::NodeId>& nodes_to_update = input_restore_data.node_ids;
    const std::vector<Graph::NodeId>& old_bases_of_nodes_to_update = input_restore_data.bases;
    const std::vector<std::pair<Graph::NodeId, Graph::EdgeId>>& old_predecessor_of_nodes_to_update =
            input_restore_data.predecessor;
    const std::vector<Graph::PathLength>& old_dist_to_base_of_nodes_to_updates = input_restore_data.dist_to_base;

    for( unsigned int i = 0; i < nodes_to_update.size(); i++) {

        const Graph::NodeId curr_nodeid = nodes_to_update[i];

        _base[curr_nodeid] = old_bases_of_nodes_to_update[i];
        _predecessor[curr_nodeid] = old_predecessor_of_nodes_to_update[i];
        _dist_to_base[curr_nodeid] = old_dist_to_base_of_nodes_to_updates[i];
    }

    _num_bases += input_restore_data.num_bases_deleted;
}


void Voronoi_diagram::assign_bases(const std::vector<Graph::NodeId>& new_set_of_bases) {
    _num_bases = new_set_of_bases.size();

    if(_num_bases > original_graph().num_nodes()){
        throw std::runtime_error("(Voronoi_diagram::assign_bases) neue Menge der Basen zu groß");
    }

    for(unsigned int i = 0; i<_num_bases; i++){
        Graph::NodeId var_base = new_set_of_bases[i];
        _base[var_base] = var_base;
        _dist_to_base[var_base] = 0;
    }
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

std::vector<Graph::NodeId> Voronoi_diagram::compute_base_ids() const {
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

    //es folgt eine Graphendurchmusterung (die von dem Eingabeknoten aus startet)

    std::vector<Graph::NodeId> next_nodes(1, input_base);

    //Markierung als 'betrachtet', siehe unten
    _base[input_base] = Graph::invalid_node_id;

    while(not next_nodes.empty()) {
        Graph::NodeId curr_node_id = next_nodes.back();
        next_nodes.pop_back();

        for(auto curr_edge_id: original_graph().incident_edge_ids(curr_node_id) ) {
            Graph::NodeId curr_neighbor_id =  original_graph().get_edge(curr_edge_id).get_other_node(curr_node_id);

            if(_base[curr_neighbor_id] == input_base) {
                next_nodes.push_back(curr_neighbor_id);

                //markiere den Knoten als "betrachtet"
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


std::vector<Graph::NodeId>
Voronoi_diagram::compute_some_vor_regions(const std::vector<Graph::NodeId>& subset_of_bases) {

    std::vector< Graph::NodeId > output;
    output.reserve(subset_of_bases.size());

    for( auto curr_base : subset_of_bases) {
        if(check_if_base(curr_base)) {
            const std::vector<Graph::NodeId>& curr_vor_region = compute_vor_region(curr_base);
            output.insert( output.end(), curr_vor_region.begin(), curr_vor_region.end() );
        } else {
            throw std::runtime_error("(Voronoi_diagram::compute_some_vor_regions) "
                                     "Eingabemenge keine Teilmenge der Basen.");
        }
    }

    return output;
}

