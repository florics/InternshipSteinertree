/*
Voronoi_diagram::Voronoi_diagram(std::vector<Graph::NodeId> set_of_b, Graph g){
    _num_nodes = g.num_nodes();
    _num_bases = set_of_b.size();
    _set_of_bases = set_of_b;

    //Prüfe, ob Eingabe Bedingungen erfüllt
    //für die Menge der Basen set_of_b
    if(_num_bases == 0){
        throw std::runtime_error("(constructor Voronoi_diagram) set_of_b ist leer");
    }
    for(auto var_node : _set_of_bases){
        if(var_node >= _num_nodes){
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

    //Initialisieren der Vektoren

    _base.assign(_num_nodes, Graph::invalid_node_id);

    std::pair< Graph::NodeId, Graph::EdgeId> var_pair = {Graph::invalid_node_id, Graph::invalid_edge_id};
    _predecessor.assign(_num_nodes, var_pair);

    std::vector<Graph::PathLength> var_dist_to_base (_num_nodes, Graph::infinite_length);
    _dist_to_base = var_dist_to_base;

    //Initialisierung von reached (markiert bereits betrachtete Knoten liegen mit 1)
    std::vector<bool> reached(_num_nodes, 0);

    //? Alternative: alles in die while Schleife (also inkl. erste dijkstra-Iterationen für die Basen)
    //? und _predecessor[basis] = basis, dann brauchen wir reached auch nicht mehr,
    //? aber Nachteil weil diese Form von _predecessor Probleme machen könnte
    //erste dijkstra-Iterationen für die Basen
    for(auto curr_base : _set_of_bases){
        //Die Basen selbst haben sich selbst als Basis
        _base[curr_base] = curr_base;
        _dist_to_base[curr_base] = 0;

        //Basen sind Startpunkte (sources) des Dijsktra-Algo.
        reached[curr_base] = 1;

        //Schleife über die zu curr_base inzidenten Kanten
        std::vector<Graph::EdgeId> curr_base_edges = g.get_node(curr_base).incidence_vect();
        for(auto curr_edge_id: curr_base_edges){
            Graph::Edge curr_edge = g.get_edge(curr_edge_id);

            //finde den Nachbarknoten der zu der aktuellen Kante gehört
            Graph::NodeId curr_neighbor = curr_edge.get_other_node(curr_base);

            //aktualisiere ggf. _dist_to_base und _predecessor für diesen Nachbarknoten
            if( !reached[curr_neighbor] && _dist_to_base[curr_neighbor] > _dist_to_base[curr_base] + curr_edge.weight() ){
                _dist_to_base[curr_neighbor] = _dist_to_base[curr_base] + curr_edge.weight();
                _predecessor[curr_neighbor] = {curr_base, curr_edge_id};
            }
        }

    }

    int debug_count_while_loop = 0;
    while(1){
        debug_count_while_loop ++;

        Graph::NodeId next_node;

        //suche Knoten, der noch nicht betrachtet wurde, mit _dist_to_base minimal
        Graph::PathLength min_dist_to_b = Graph::infinite_length;
        for (Graph::NodeId i = 0; i < _num_nodes; i++) {
            if( !reached[i] && _dist_to_base[i]<min_dist_to_b ){
                min_dist_to_b = _dist_to_base[i];
                next_node = i;
            }
        }
        if(min_dist_to_b == Graph::infinite_length){
            //cout<< "Iteration " << count_while_loop << ": break weil alle l = infty \n"; //debug
            break;
        }

        //debug
        if(_predecessor[next_node].first == Graph::invalid_node_id){
            std::cout << "next_node = " << next_node;
            throw std::runtime_error("(constructor Voronoi_diagram) _predecessor[next_node].first == Graph::invalid_node_id");
        }

        //markiere als betrachtet/reached und füge zur Basis des Vorgängers hinzu
        reached[next_node] = 1;
        _base[next_node] = _base[ _predecessor[next_node].first ];

        //Schleife über die zu next_node inzidenten Kanten
        std::vector<Graph::EdgeId> next_node_edges = g.get_node(next_node).incidence_vect();
        for(auto curr_edge_id: next_node_edges){
            Graph::Edge curr_edge = g.get_edge(curr_edge_id);

            //finde den Nachbarknoten der zu der aktuellen Kante gehört
            Graph::NodeId curr_neighbor = curr_edge.get_other_node(next_node);

            //aktualisiere ggf. _dist_to_base und _predecessor für diesen Nachbarknoten
            if( !reached[curr_neighbor] && _dist_to_base[curr_neighbor] > _dist_to_base[next_node] + curr_edge.weight() ){
                _dist_to_base[curr_neighbor] = _dist_to_base[next_node] + curr_edge.weight();
                _predecessor[curr_neighbor] = {next_node, curr_edge_id};
            }
        }

        bool r_complete = 1;
        for(Graph::NodeId i=0; i<_num_nodes; i++){
            if(!reached[i]){
                r_complete = 0;
                continue;
            }
        }
        if(r_complete){
            //cout<< "Iteration " << count_while_loop << ": break weil r alle Knoten enthaelt. \n"; //debug
            break;
        }
    }

    /*
    //prüfen, ob alle Knoten erreicht wurden (debug)
    for (unsigned int i = 0; i < n; i++) {
        if( l[i] == numeric_limits <double >::max() ){
            cout << "(dijkstra) Knoten " << i << " nicht erreichbar von " << v << "\n";
        }
    }


    return;
}


*/


/*
Voronoi_diagram::Voronoi_diagram(std::vector<Graph::NodeId> set_of_b, Graph g){
    _num_nodes = g.num_nodes();
    _num_bases = set_of_b.size();
    _set_of_bases = set_of_b;

    //Prüfe, ob Eingabe Bedingungen erfüllt
    //für die Menge der Basen set_of_b
    if(_num_bases == 0){
        throw std::runtime_error("(constructor Voronoi_diagram) set_of_b ist leer");
    }
    for(auto var_node : _set_of_bases){
        if(var_node >= _num_nodes){
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

    //Initialisieren der Vektoren

    _base.assign(_num_nodes, Graph::invalid_node_id);

    std::pair< Graph::NodeId, Graph::EdgeId> var_pair = {Graph::invalid_node_id, Graph::invalid_edge_id};
    _predecessor.assign(_num_nodes, var_pair);

    std::vector<Graph::PathLength> var_dist_to_base (_num_nodes, Graph::infinite_length);
    _dist_to_base = var_dist_to_base;

    //Initialisierung von reached (markiert bereits betrachtete Knoten liegen mit 1) todo: alle reached löschen?
    std::vector<bool> reached(_num_nodes, 0);

    //Initialisierung der priority_queue, mit der wir in jeder Iteration die nächste beste Kante (Kandidat) finden
    //std::greater< std::pair<Graph::PathLength,Graph::NodeId> > sorgt dafür, dass wir immer an den Knoten mit der kleinsten Distanz gelangen
    std::priority_queue<
            std::pair<Graph::PathLength,Graph::EdgeId>,
            std::vector< std::pair<Graph::PathLength,Graph::EdgeId> >,
            std::greater< std::pair<Graph::PathLength,Graph::EdgeId> >
                > candidates;

    //Die Basen selbst haben sich selbst als Basis
    for(auto curr_base : _set_of_bases) {
        _base[curr_base] = curr_base;
        _dist_to_base[curr_base] = 0;
    }

    //? Alternative: alles in die while Schleife (also inkl. erste dijkstra-Iterationen für die Basen)
    //? und _predecessor[basis] = basis, dann brauchen wir reached auch nicht mehr,
    //? aber Nachteil weil diese Form von _predecessor Probleme machen könnte
    //erste dijkstra-Iterationen für die Basen
    for(auto curr_base : _set_of_bases) {
        //Basen sind Startpunkte (sources) des Dijsktra-Algo.
        reached[curr_base] = 1;

        //Schleife über die zu curr_base inzidenten Kanten
        std::vector<Graph::EdgeId> curr_base_edges = g.get_node(curr_base).incidence_vect();
        for(auto curr_edge_id: curr_base_edges){
            Graph::Edge curr_edge = g.get_edge(curr_edge_id);

            //finde den Nachbarknoten der zu der aktuellen Kante gehört
            Graph::NodeId curr_neighbor = curr_edge.get_other_node(curr_base);

            //aktualisiere ggf. _dist_to_base und _predecessor für diesen Nachbarknoten
            //bemerke, dass !check_if_base(curr_neighbor) && _dist_to_base[curr_neighbor] > _dist_to_base[curr_base] + curr_edge.weight() gdw. !check_if_base(curr_neighbor)
            if( !check_if_base(curr_neighbor) ){
                //_dist_to_base[curr_neighbor] = curr_edge.weight();
                //_predecessor[curr_neighbor] = {curr_base, curr_edge_id};
                candidates.push({curr_edge.weight(), curr_edge_id});
            }
        }

    }

    int debug_count_while_loop = 0;
    while( !candidates.empty() ) {
        debug_count_while_loop++;
        std::cout << "Iteration while-Schleife nr. " << debug_count_while_loop << "\n";

        //finde nächsten besten Kandidaten
        Graph::EdgeId next_cand_edge = candidates.top().second; //passt der Datentyp hier? (Referenz...)
        Graph::PathLength dist_of_next_cand = candidates.top().first;
        candidates.pop();

        /*
        //debug begin
        if (dist_of_next_cand == _dist_to_base[next_node]) {
            std::cout << dist_of_next_cand << " = " << _dist_to_base[next_node] << "\n";
        }
        if (dist_of_next_cand > _dist_to_base[next_node]) {
            std::cout << dist_of_next_cand << " > " << _dist_to_base[next_node] << "\n";
        }
        //debug end


//Finde Knoten, der zur Kandidaten-Kante inzident ist und noch nicht erreicht wurde
Graph::NodeId next_cand_node = g.get_edge(next_cand_edge).node_a();
if ( reached[next_cand_node] ) {
next_cand_node = g.get_edge(next_cand_edge).node_b();
}
if ( reached[next_cand_node] ) {
//es gibt keinen solchen Knoten, die Kandidaten-Kante ist also auf keinem kuerz. Weg
continue;
}

//checke ob restliche Knoten alle unerreichbar sind. besser mit oder ohne diesen Check?
if (dist_of_next_cand == Graph::infinite_length) {
//cout<< "Iteration " << count_while_loop << ": break weil alle l = infty \n"; //debug
break;
}

//if (dist_of_next_cand < _dist_to_base[next_node]) { //?

//markiere als betrachtet/reached und füge zur Basis des Vorgängers hinzu und setze den Abstand zur Basis
reached[next_cand_node] = 1;
_predecessor[next_cand_node].first = g.get_edge(next_cand_edge).get_other_node(next_cand_node);
_predecessor[next_cand_node].second = next_cand_edge;
_base[next_cand_node] = _base[_predecessor[next_cand_node].first];
_dist_to_base[next_cand_node] = dist_of_next_cand;

//Schleife über die zu next_cand_node inzidenten Kanten
std::vector<Graph::EdgeId> next_cand_node_edges = g.get_node(next_cand_node).incidence_vect();
for (auto curr_edge_id: next_cand_node_edges) {
Graph::Edge curr_edge = g.get_edge(curr_edge_id);

//finde den Nachbarknoten der zu der aktuellen Kante gehört
Graph::NodeId curr_neighbor = curr_edge.get_other_node(next_cand_node);

//aktualisiere ggf. _dist_to_base und _predecessor für diesen Nachbarknoten
//!reached[curr_neighbor] &&  gelöscht
if (_dist_to_base[curr_neighbor] > _dist_to_base[next_cand_node] + curr_edge.weight()) {
//debug
if(reached[curr_neighbor]){
std::cout << "kleinere Distanz obwohl schon reached (Zeile 310) \n";
}
// ? _dist_to_base[curr_neighbor] = _dist_to_base[next_cand_node] + curr_edge.weight();
candidates.push({_dist_to_base[next_cand_node] + curr_edge.weight(), curr_edge_id});
// ? _predecessor[curr_neighbor] = {next_cand_node, curr_edge_id};
}
}

}
/*
//prüfen, ob alle Knoten erreicht wurden (debug)
for (unsigned int i = 0; i < n; i++) {
    if( l[i] == numeric_limits <double >::max() ){
        cout << "Knoten " << i+1 << " nicht erreichbar \n";
    }
}


return;
}
*/