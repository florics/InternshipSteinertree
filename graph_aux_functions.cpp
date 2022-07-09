//
// Created by Lukas on 14.06.2022.
//

#include "graph_aux_functions.h"

#include "iostream"
#include "algorithm"
#include "limits"

#include "graph_printfunctions.h"
#include "Union_Find_Structure.h"
#include "general_aux_functions.h"


//todo Funktionennamen aktualisieren?

// ? nicht sicher ob das funzt: die Instanzen der Uni sind alle isomorph laut dieser Methode
bool GraphAux::check_if_isomorph(const Graph& graph1, const Graph& graph2){
    //Eingabe prüfen
    if(not GraphAux::edgeweight_finite(graph1)){
        throw std::runtime_error("(GraphAux::check_if_isomorph) eingegebener Graph graph1 hat unendliche Kantengewichte");
    }
    if(not GraphAux::edgeweight_finite(graph2)){
        throw std::runtime_error("(GraphAux::check_if_isomorph) eingegebener Graph graph2 hat unendliche Kantengewichte");
    }
    if(not GraphAux::check_if_simple(graph1) ){
        throw std::runtime_error("(GraphAux::check_if_isomorph) eingegebener Graph graph1 ist nicht einfach");
    }
    if(not GraphAux::check_if_simple(graph2) ){
        throw std::runtime_error("(GraphAux::check_if_isomorph) eingegebener Graph graph2 ist nicht einfach");
    }

    //leichte Checks
    if( graph1.num_nodes() != graph2.num_nodes() ){
        std::cout << "(GraphAux::check_if_isomorph) Knotenanzahl nicht gleich \n";
        return false;
    }
    if( graph1.num_edges() != graph2.num_edges() ){
        std::cout << "(GraphAux::check_if_isomorph) Kantenanzahl nicht gleich \n";
        return false;
    }

    //Matrix für alle Knotenpaare, Adjazenzmatrix
    // Eintrag [i][j] entspricht Knotenpaar i+1, j (wobei i >= j)
    std::vector< std::vector<Graph::EdgeWeight> > edge_weight_matrice;
    std::vector< std::vector<bool> > adjacency_matrice;

    //Initialisieren
    for(unsigned int i = 0; i < graph1.num_nodes()-1; i++){
        std::vector<Graph::EdgeWeight> var_weight_vector (i+1, Graph::infinite_weight);
        edge_weight_matrice.push_back(var_weight_vector);

        std::vector<bool> var_bool_vector (i+1, false);
        adjacency_matrice.push_back(var_bool_vector);
    }


    //ausfüllen entsprechend der Kanten des ersten Graphen
    for(auto curr_edge : graph1.edges() ){
        std::pair<Graph::NodeId, Graph::NodeId> curr_edge_nodes = curr_edge.get_nodes_orderedbyid();

        edge_weight_matrice[ curr_edge_nodes.second-1 ][ curr_edge_nodes.first ] = curr_edge.weight();

        adjacency_matrice[ curr_edge_nodes.second-1 ][ curr_edge_nodes.first ] = true;
    }

    //Gleichheit der Kanten des zweiten Graphen prüfen
    for(auto curr_edge : graph2.edges() ){
        std::pair<Graph::NodeId, Graph::NodeId> curr_edge_nodes = curr_edge.get_nodes_orderedbyid();

        if( not adjacency_matrice[ curr_edge_nodes.second-1 ][ curr_edge_nodes.first ] ){
            std::cout << "(GraphAux::check_if_isomorph) Kante ";
            GraphAuxPrint::print_edge_as_pair(graph2, curr_edge.edge_id() );
            std::cout << " liegt im zweiten eingegebenen Graph aber nicht im ersten. \n";
            return false;
        }
        if( edge_weight_matrice[ curr_edge_nodes.second-1 ][ curr_edge_nodes.first ] != curr_edge.weight() ){
            std::cout << "(GraphAux::check_if_isomorph) Kantengewicht der Kante ";
            GraphAuxPrint::print_edge_as_pair(graph2, curr_edge.edge_id() );
            std::cout << "ungleich. \n";
            return false;
        }

        //am Ende sollte die Adjazenz-Matrix nur false enthalten
        adjacency_matrice[ curr_edge_nodes.second-1 ][ curr_edge_nodes.first ] = false;
    }

    for(unsigned int i = 0; i < graph1.num_nodes()-1; i++){
        for(unsigned int j = 0; j < i+1; j++){
            if( adjacency_matrice[i][j] ){
                std::cout << "(GraphAux::check_if_isomorph) Kante (" << i+1 << "," << j << ") liegt im ersten eingegebenen Graph aber nicht im zweiten. \n";
                return false;
            }
        }
    }

    return true;
}

bool GraphAux::edgeweight_nonnegative(const Graph& input_graph) {
    /* ??
    if( std::any_of(_edges, weight() < 0){
        return false
    } else {
        return true
    }
     */
    for(auto var_edge : input_graph.edges()){
        if(var_edge.weight() < 0){
            return false;
        }
    }
    return true;
}

bool GraphAux::edgeweight_finite(const Graph& input_graph) {
    for(auto var_edge : input_graph.edges()){
        if( var_edge.weight() == Graph::infinite_weight
            || var_edge.weight() == -Graph::infinite_weight ){
            return false;
        }
    }
    return true;
}

bool GraphAux::check_if_simple(const Graph& input_graph) {
    // adjacency_matrice[i][j] entspricht Knotenpaar i+1, j (wobei i >= j)
    std::vector< std::vector<bool> > adjacency_matrice;

    //Initialisieren
    for(unsigned int i = 0; i < input_graph.num_nodes()-1; i++){
        std::vector<bool> var_bool_vector (i+1, false);
        adjacency_matrice.push_back(var_bool_vector);
    }

    //ausfüllen entsprechend der Kanten des Graphen und prüfen, ob Kante (bzw. Knotenpaar) doppelt
    for(auto curr_edge : input_graph.edges() ){
        std::pair<Graph::NodeId, Graph::NodeId> curr_edge_nodes = curr_edge.get_nodes_orderedbyid();
        if( adjacency_matrice[ curr_edge_nodes.second-1 ][ curr_edge_nodes.first ] ){
            std::cout << "(GraphAux::check_if_simple) Kante (bzw. Knotenpaar) ";
            GraphAuxPrint::print_edge_as_pair(input_graph, curr_edge.edge_id() );
            std::cout << " ist doppelt. \n";
            return false;
        }
        adjacency_matrice[ curr_edge_nodes.second-1 ][ curr_edge_nodes.first ] = true;
    }

    return true;
}

Graph GraphAux::copygraph_wo_edges(const Graph& input_graph){
    Graph output(0);
    for( const auto& curr_node : input_graph.nodes()){
        output.add_one_node( curr_node.terminal_state() );
    }
    //? Laufzeit: beschleunigen durch add_nodes und danach Terminale markieren
    return output;
}

Graph GraphAux::copygraph_wo_iso_nodes(const Graph& input_graph){
    Graph output_graph(0);
    std::vector< Graph::NodeId > new_node_ids ( input_graph.num_nodes(), Graph::invalid_node_id );

    //Knoten hinzufügen
    unsigned int output_graph_numnodes = 0;
    //? Laufzeit: Speicher in _nodes reserven
    for(Graph::NodeId i=0; i<input_graph.num_nodes(); i++){
        Graph::Node curr_node = input_graph.get_node(i);
        if( curr_node.num_neighbors() != 0 ){
            output_graph.add_one_node( curr_node.terminal_state() );

            //merke node_id bzgl output-Graph des Knoten mit node_id i bzgl g
            new_node_ids[ i ] = output_graph_numnodes;
            output_graph_numnodes++;
        }
    }

    //Kanten hinzufügen
    for (auto var_edge : input_graph.edges() ){
        //"konstruiere" neue Kante
        Graph::NodeId new_node_a =  new_node_ids[var_edge.node_a()];
        Graph::NodeId new_node_b =  new_node_ids[var_edge.node_b()];

        //füge sie zu output hinzu
        output_graph.add_edge(new_node_a, new_node_b, var_edge.weight());
    }

    return output_graph;
}

/*?
Graph GraphAux::copygraph_wo_steinerleafs(const Graph& input_graph){
    Graph output_graph(0);
    std::vector< Graph::NodeId > new_node_ids ( input_graph.num_nodes(), Graph::invalid_node_id );

    //Knoten hinzufügen
    unsigned int output_graph_numnodes = 0;
    //? Laufzeit: Speicher in _nodes reserven
    for(Graph::NodeId i=0; i<input_graph.num_nodes(); i++){
        Graph::Node curr_node = input_graph.get_node(i);
        if( curr_node.num_neighbors() == 1 && not curr_node.check_if_terminal() ) {
            continue;
        } else {
            output_graph.add_one_node( curr_node.terminal_state() );

            //merke node_id bzgl output-Graph des Knoten mit node_id i bzgl g
            new_node_ids[ i ] = output_graph_numnodes;
            output_graph_numnodes++;
        }
    }

    //Kanten hinzufügen
    for (auto var_edge : input_graph.edges() ){
        //"konstruiere" neue Kante
        Graph::NodeId new_node_a =  new_node_ids[var_edge.node_a()];
        Graph::NodeId new_node_b =  new_node_ids[var_edge.node_b()];

        //füge sie zu output hinzu
        if( new_node_a != Graph::invalid_edge_id && new_node_b != Graph::invalid_edge_id){
            output_graph.add_edge(new_node_a, new_node_b, var_edge.weight());
        }
    }

    return output_graph;
}
*/

Subgraph GraphAux::copy_subgraph_wo_iso_nodes(const Subgraph &input_subgraph) {
    const Graph& input_graph = input_subgraph.this_graph();
    const Graph& original_graph = input_subgraph.original_graph();

    Graph output_graph(0);

    // speichert für jeden Knoten des zugrundeliegenden Graphen die NodeId in dem Ausgabegraphen
    std::vector< Graph::NodeId > new_node_ids ( original_graph.num_nodes(), Graph::invalid_node_id );
    // speichert für jeden Knoten des Ausgabegraphen die NodeId im zugrundeliegenden Graphen
    std::vector< Graph::NodeId > old_node_ids;
    old_node_ids.reserve(input_graph.num_nodes());

    //Knoten hinzufügen

    unsigned int output_graph_numnodes = 0;
    const std::vector<Graph::NodeId>& input_subgraph_nodeids_of_nodes_in_or_graph = input_subgraph.subgraph_nodeids_of_nodes_in_originalgraph();

    //? Laufezeit: Speicher in _nodes reserven
    for(Graph::NodeId i=0; i<original_graph.num_nodes(); i++){
        if(input_subgraph_nodeids_of_nodes_in_or_graph[i] != Graph::invalid_node_id){
            const Graph::Node& curr_node = input_graph.get_node( input_subgraph_nodeids_of_nodes_in_or_graph[i] );
            if (curr_node.num_neighbors() != 0) {
                output_graph.add_one_node( curr_node.terminal_state());

                //merke NodeId bzgl output-Graph des Knoten mit node_id i bzgl des zugrundeliegenden Graphen
                new_node_ids[i] = output_graph_numnodes;
                output_graph_numnodes++;

                // merke NodeId des zugrundeliegenden Graphen für den Knoten im Ausgabegraphen
                old_node_ids.push_back(i);
            }
        }
    }

    old_node_ids.shrink_to_fit();

    //Kanten hinzufügen
    const std::vector<Graph::EdgeId>& input_original_edge_ids = input_subgraph.original_edgeids();
    for (auto curr_edge_id : input_original_edge_ids ){
        //Kante aus dem zugrundeliegenden Graphen
        const Graph::Edge& curr_edge = original_graph.get_edge(curr_edge_id);

        //"konstruiere" neue Kante
        Graph::NodeId new_node_a =  new_node_ids[curr_edge.node_a()];
        Graph::NodeId new_node_b =  new_node_ids[curr_edge.node_b()];

        //füge sie zu output hinzu
        output_graph.add_edge(new_node_a, new_node_b, curr_edge.weight());
    }

    //die alten & die neuen NodeIds haben wir oben gefunden, die EdgeIds bleiben gleich
    Subgraph output(original_graph, output_graph, new_node_ids, old_node_ids, input_original_edge_ids);

    return output;
}

Subgraph GraphAux::copy_subgraph_wo_steinerleafs_old(const Subgraph &input_subgraph) {
    const Graph& input_graph = input_subgraph.this_graph();
    const Graph& original_graph = input_subgraph.original_graph();

    // der Graph den wir später als Subgraph ausgeben
    Graph output_graph(0);

    // speichert für jeden Knoten des zugrundeliegenden Graphen die NodeId in dem Ausgabegraphen
    std::vector< Graph::NodeId > new_node_ids ( original_graph.num_nodes(), Graph::invalid_node_id );
    // speichert für jeden Knoten des Ausgabegraphen die NodeId im zugrundeliegenden Graphen
    std::vector< Graph::NodeId > old_node_ids;
    old_node_ids.reserve(input_graph.num_nodes());

    //Knoten hinzufügen

    unsigned int output_graph_numnodes = 0;
    const std::vector<Graph::NodeId>& input_subgraph_nodeids_of_nodes_in_or_graph = input_subgraph.subgraph_nodeids_of_nodes_in_originalgraph();

    //? Laufezeit: Speicher in _nodes reserven
    for(Graph::NodeId i=0; i<original_graph.num_nodes(); i++){
        if(input_subgraph_nodeids_of_nodes_in_or_graph[i] != Graph::invalid_node_id) {
            Graph::Node curr_node = input_graph.get_node( input_subgraph_nodeids_of_nodes_in_or_graph[i] );
            if (curr_node.num_neighbors() == 1 && not curr_node.check_if_terminal()) {
                continue;
            } else {
                output_graph.add_one_node( curr_node.terminal_state());

                //merke node_id bzgl output-Graph des Knotens mit node_id i bzgl des zugrundeliegenden Graphen
                new_node_ids[i] = output_graph_numnodes;
                output_graph_numnodes ++;

                // merke NodeId des zugrundeliegenden Graphen für den Knoten im Ausgabegraphen
                old_node_ids.push_back(i);
            }
        }
    }

    old_node_ids.shrink_to_fit();

    //Kanten hinzufügen
    const std::vector<Graph::EdgeId>& input_original_edge_ids = input_subgraph.original_edgeids();
    for (auto curr_edge_id : input_original_edge_ids ){
        //Kante aus dem zugrundeliegenden Graphen
        const Graph::Edge& curr_edge = original_graph.get_edge(curr_edge_id);

        //"konstruiere" neue Kante
        Graph::NodeId new_node_a =  new_node_ids[curr_edge.node_a()];
        Graph::NodeId new_node_b =  new_node_ids[curr_edge.node_b()];

        //füge sie zu output hinzu
        if( new_node_a != Graph::invalid_node_id && new_node_b != Graph::invalid_node_id){
            output_graph.add_edge(new_node_a, new_node_b, curr_edge.weight());
        }
    }

    //die neuen NodeIds haben wir mit new_node_ids gefunden, die EdgeIds bleiben gleich
    Subgraph output(original_graph, output_graph, new_node_ids, old_node_ids, input_original_edge_ids);

    return output;
}

Graph::PathLength GraphAux::length_of_all_edges(const Graph& input_graph) {
    Graph::PathLength output = 0;
    for(auto curr_edge : input_graph.edges() ) {
        output += curr_edge.weight();
    }
    return output;
}

std::vector<Graph::NodeId> GraphAux::get_isolated_nodes(const Graph& input_graph) {
    std::vector<Graph::NodeId> output;

    for( auto curr_node: input_graph.nodes()) {
        if( curr_node.num_neighbors() == 0) {
            output.push_back(curr_node.node_id());
        }
    }

    return output;
}

bool GraphAux::check_if_connected(const Graph &input_graph) {

    // wir sagen, dass der leere Graph zusammenhängend ist
    if( input_graph.num_nodes() == 0) {
        return true;
    }

    //Graphendurchmusterung

    std::vector<Graph::NodeId> next_nodes;
    std::vector<bool> reached(input_graph.num_nodes(), false);

    //starte bei dem Knoten mit NodeId 0
    next_nodes.push_back(0);
    reached[0] = true;

    while( not next_nodes.empty() ) {
        Graph::NodeId curr_node = next_nodes.back();
        next_nodes.pop_back();

        for(auto curr_neighbor: input_graph.adjacency_vect(curr_node)) {
            if( not reached[curr_neighbor]) {
                next_nodes.push_back(curr_neighbor);
                reached[curr_neighbor] = true;
            }
        }
    }

    for(unsigned int i=0; i<input_graph.num_nodes(); i++) {
        if( not reached[i]) {
            return false;
        }
    }

    return true;
}

bool GraphAux::check_if_terminals_connected(const Graph &input_graph) {
    std::vector<Graph::NodeId> terminals = input_graph.get_vect_term();

    // wenn höchstens ein Terminal existiert, liegen alle in einer ZHK
    if( terminals.size() < 2) {
        return true;
    }

    //speichert die Anzahl an gefundenen Terminalen
    unsigned int term_counter = 1;

    //Graphendurchmusterung

    std::vector<Graph::NodeId> next_nodes;
    std::vector<bool> reached(input_graph.num_nodes(), false);

    //starte bei dem Terminal mit der niedrigsten NodeId
    next_nodes.push_back(terminals[0]);
    reached[ terminals[0] ] = true;

    while( not next_nodes.empty() ) {
        Graph::NodeId curr_node = next_nodes.back();
        next_nodes.pop_back();

        const std::vector<Graph::NodeId> &var_vect = input_graph.adjacency_vect(curr_node);
        for(auto curr_neighbor: input_graph.adjacency_vect(curr_node)) {
            if( not reached[curr_neighbor]) {
                next_nodes.push_back(curr_neighbor);
                reached[curr_neighbor] = true;

                if( input_graph.check_if_terminal(curr_neighbor)) {
                    term_counter++;

                    //falls wir bereits alle Terminale gefunden haben, brauchen wir nicht weiter zu suchen
                    if( term_counter == terminals.size() ) {
                        return true;
                    }
                }
            }
        }
    }

    return false;
}

std::vector<Graph::NodeId>
GraphAux::get_steinernodes_in_postorder(const Graph &input_graph, Graph::NodeId terminal_root) {
    if( terminal_root == Graph::invalid_node_id) {
        throw std::runtime_error("(GraphAux::get_steinernodes_in_postorder) Eingabeknoten ungueltig");
    }
    if( terminal_root > input_graph.num_nodes()) {
        throw std::runtime_error("(GraphAux::get_steinernodes_in_postorder) Eingabeknoten nicht im Graphen");
    }

    if( not input_graph.check_if_terminal(terminal_root)) {
        throw std::runtime_error("(GraphAux::get_steinernodes_in_postorder) Wurzel ist kein Terminal");
    }

    //speichert die Steinerknoten zunächst in der Reihenfolge der Graphendurchmusterung
    std::vector<Graph::NodeId> output;
    output.reserve(input_graph.num_nodes());

    // es folgt eine Graphendurchmusterung

    std::vector<Graph::NodeId> next_nodes (1, terminal_root);
    //speichert die bereits erreichten Knoten (die, die bereits zu next_nodes hinzugfügt wurden)
    //Laufzeit O(num_nodes()) Problem?
    std::vector<bool> reached_nodes (input_graph.num_nodes(), false);
    reached_nodes[terminal_root] = true;

    while( not next_nodes.empty()) {
        Graph::NodeId curr_node_id = next_nodes.back();
        next_nodes.pop_back();

        for( auto curr_edge_id : input_graph.incident_edge_ids(curr_node_id) ) {
            Graph::NodeId curr_neighbor_id = input_graph.get_edge(curr_edge_id).get_other_node(curr_node_id);

            if( not reached_nodes[curr_neighbor_id] ) {
                reached_nodes[curr_neighbor_id] = true;
                next_nodes.push_back(curr_neighbor_id);

                if( not input_graph.check_if_terminal(curr_neighbor_id) ) {
                    output.push_back( curr_neighbor_id );
                }
            }
        }
    }

    output.shrink_to_fit();

    //kehre die Reihenfolge um
    std::reverse(output.begin(), output.end());

    return output;
}

std::vector<bool> GraphAux::compute_steiner_branches(const Graph &input_graph) {
    std::vector<bool> nodes_in_st_branch(input_graph.num_nodes(), false);

    std::vector<Graph::NodeId> steinernodes_in_postorder =
            GraphAux::get_steinernodes_in_postorder(input_graph, input_graph.get_vect_term()[0]);

    //berechne die Anzahl der Nachbarn für alle Steinerknoten

    using NumNeighbors = unsigned int;
    //? static const NumNeighbors invalid_num = std::numeric_limits<double>::max();

    std::vector<NumNeighbors> num_neighbors(input_graph.num_nodes(), 0);

    for(auto curr_node_id: steinernodes_in_postorder) {
        const Graph::Node& curr_node = input_graph.get_node(curr_node_id);
        num_neighbors[ curr_node_id ] = curr_node.num_neighbors();
    }


    for(auto curr_node_id: steinernodes_in_postorder) {

        if( num_neighbors[curr_node_id] <= 1) {
            nodes_in_st_branch[curr_node_id] = true;

            //aktualisiere die Anzahl an Nachbarn für alle Nachbarn des aktuellen Knotens
            for(auto curr_neighbor_id: input_graph.adjacency_vect(curr_node_id) ) {
                if( (not nodes_in_st_branch[curr_neighbor_id]) && (not input_graph.check_if_terminal(curr_neighbor_id)) ) {
                    num_neighbors[curr_neighbor_id] --;
                }
            }
        }
    }

    return nodes_in_st_branch;
}

void GraphAux::remove_steinerbranches(Subgraph &input_subgraph) {

    const Graph& this_graph = input_subgraph.this_graph();
    const std::vector<Graph::EdgeId>& old_original_edge_ids = input_subgraph.original_edgeids();

    // berechne alle Knoten, die entfernt werden sollen
    std::vector<bool> nodes_in_st_branches = GraphAux::compute_steiner_branches(input_subgraph.this_graph());

    //falls es keine solchen Knoten gibt, können wir abbrechen
    if ( std::all_of(nodes_in_st_branches.begin(), nodes_in_st_branches.end(), [](bool b){return not b;}) ) {
        return;
    }

    // hier sollen alle Kanten eingefügt werden, die nicht mit einem Steinerpunkt inzident sind, der entfernt werden soll
    // Kanten werden über die EdgeId's des zugrundeliegenden Graphen gespeichert
    std::vector<Graph::EdgeId> new_original_edge_ids;
    new_original_edge_ids.reserve(this_graph.num_edges());

    for( const auto& curr_edge: this_graph.edges() ) {

        if( (not nodes_in_st_branches[curr_edge.node_a()]) && (not nodes_in_st_branches[curr_edge.node_b()]) ) {

            Graph::EdgeId curr_edge_original_id = old_original_edge_ids[curr_edge.edge_id()];
            new_original_edge_ids.push_back(curr_edge_original_id);
        }
    }

    new_original_edge_ids.shrink_to_fit();

    input_subgraph.reset(new_original_edge_ids);
}

std::vector<Graph::NodeId> GraphAux::get_steinerleafs(const Graph &input_graph) {
    std::vector<Graph::NodeId> output;

    for( const auto& curr_node: input_graph.nodes()) {
        if( (not curr_node.check_if_terminal()) && curr_node.num_neighbors() == 1 ) {
            output.push_back(curr_node.node_id());
        }
    }

    return output;
}

void GraphAux::check_node_id(Graph::NodeId input_node_id, unsigned int num_nodes, const std::string& function_name) {
    if( input_node_id == Graph::invalid_node_id){
        throw std::runtime_error("In der Funktion " + function_name + ": Eingabeknoten ungültig");
    }
    if( input_node_id >= num_nodes) {
        throw std::runtime_error("In der Funktion " + function_name + ": Eingabeknoten liegt nicht im Graphen");
    }
}


void GraphAux::check_edge_id(Graph::EdgeId input_edge_id, unsigned int num_edges, const std::string& function_name) {
    if( input_edge_id == Graph::invalid_edge_id){
        throw std::runtime_error("In der Funktion " + function_name + ": Eingabekante ungültig");
    }
    if( input_edge_id >= num_edges) {
        throw std::runtime_error("In der Funktion " + function_name + ": Eingabekante liegt nicht im Graphen");
    }
}


void GraphAux::check_if_weight_nonnegative(Graph::EdgeWeight input_weight, const std::string& function_name) {
    if( input_weight < 0) {
        throw std::runtime_error("In der Funktion " + function_name + ": Kantengewicht negativ.");
    }
}

void GraphAux::check_if_weight_finite(Graph::EdgeWeight input_weight, const std::string& function_name) {
    if( input_weight == Graph::infinite_weight || input_weight == -Graph::infinite_weight) {
        throw std::runtime_error("In der Funktion " + function_name + ": Kantengewicht negativ.");
    }
}

