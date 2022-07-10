//
// Created by Lukas on 10.07.2022.
//

#include "unused_graph_aux_functions.h"

#include "iostream"

#include "../graph_aux_functions.h"
#include "../graph_printfunctions.h"
#include "../Union_Find_Structure.h"
#include "unused_graph_printfunctions.h"

bool GraphAux::check_if_isomorph(const Graph& graph1, const Graph& graph2){
    //Eingabe prüfen
    if(not GraphAux::check_if_graph_has_finite_weights(graph1)){
        throw std::runtime_error("(GraphAux::check_if_isomorph) eingegebener Graph graph1 hat unendliche Kantengewichte");
    }
    if(not GraphAux::check_if_graph_has_finite_weights(graph2)){
        throw std::runtime_error("(GraphAux::check_if_isomorph) eingegebener Graph graph2 hat unendliche Kantengewichte");
    }
    if(not GraphAux::check_if_graph_is_simple(graph1) ){
        throw std::runtime_error("(GraphAux::check_if_isomorph) eingegebener Graph graph1 ist nicht einfach");
    }
    if(not GraphAux::check_if_graph_is_simple(graph2) ){
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
    for(const auto& curr_edge : graph1.edges() ){
        std::pair<Graph::NodeId, Graph::NodeId> curr_edge_nodes = curr_edge.get_nodes_orderedbyid();

        edge_weight_matrice[ curr_edge_nodes.second-1 ][ curr_edge_nodes.first ] = curr_edge.weight();

        adjacency_matrice[ curr_edge_nodes.second-1 ][ curr_edge_nodes.first ] = true;
    }

    //Gleichheit der Kanten des zweiten Graphen prüfen
    for(const auto& curr_edge : graph2.edges() ){
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

bool GraphAux::check_if_graph_is_simple(const Graph& input_graph) {
    // adjacency_matrice[i][j] entspricht Knotenpaar i+1, j (wobei i >= j)
    std::vector< std::vector<bool> > adjacency_matrice;

    //Initialisieren
    for(unsigned int i = 0; i < input_graph.num_nodes()-1; i++){
        std::vector<bool> var_bool_vector (i+1, false);
        adjacency_matrice.push_back(var_bool_vector);
    }

    //ausfüllen entsprechend der Kanten des Graphen und prüfen, ob Kante (bzw. Knotenpaar) doppelt
    for(const auto& curr_edge : input_graph.edges() ){
        std::pair<Graph::NodeId, Graph::NodeId> curr_edge_nodes = curr_edge.get_nodes_orderedbyid();
        if( adjacency_matrice[ curr_edge_nodes.second-1 ][ curr_edge_nodes.first ] ){
            std::cout << "(GraphAux::check_if_graph_is_simple) Kante (bzw. Knotenpaar) ";
            GraphAuxPrint::print_edge_as_pair(input_graph, curr_edge.edge_id() );
            std::cout << " ist doppelt. \n";
            return false;
        }
        adjacency_matrice[ curr_edge_nodes.second-1 ][ curr_edge_nodes.first ] = true;
    }

    return true;
}

bool GraphAux::check_if_terminals_connected(const Graph &input_graph) {
    std::vector<Graph::NodeId> terminals = input_graph.get_terminals();

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


Graph GraphAux::copygraph_wo_edges(const Graph& input_graph){
    Graph output(0);
    for( const auto& curr_node : input_graph.nodes()){
        output.add_one_node( curr_node.terminal_state() );
    }
    return output;
}


Graph GraphAux::copygraph_wo_iso_nodes(const Graph& input_graph){
    Graph output_graph(0);
    std::vector< Graph::NodeId > new_node_ids ( input_graph.num_nodes(), Graph::invalid_node_id );

    //Knoten hinzufügen
    unsigned int output_graph_numnodes = 0;

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
    for (const auto& var_edge : input_graph.edges() ){
        //"konstruiere" neue Kante
        Graph::NodeId new_node_a =  new_node_ids[var_edge.node_a()];
        Graph::NodeId new_node_b =  new_node_ids[var_edge.node_b()];

        //füge sie zu output hinzu
        output_graph.add_edge(new_node_a, new_node_b, var_edge.weight());
    }

    return output_graph;
}

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


Graph GraphAux::compute_spann_forest(const Graph& input_graph) {
    Graph output_graph(input_graph.num_nodes());
    Union_Find_Structure ufs(input_graph.num_nodes());

    for( const auto& curr_edge: input_graph.edges()) {
        Graph::NodeId curr_node_a = curr_edge.node_a();
        Graph::NodeId curr_node_b = curr_edge.node_b();

        if (not ufs.check_if_in_existing_set(curr_node_a)) {
            ufs.make_set(curr_node_a);
        }
        if (not ufs.check_if_in_existing_set(curr_node_b)) {
            ufs.make_set(curr_node_b);
        }

        if (ufs.find(curr_node_a) != ufs.find(curr_node_b)) {
            output_graph.add_edge(curr_node_a, curr_node_b, curr_edge.weight());
            ufs.union_by_rank(curr_node_a, curr_node_b);
        }
    }


    std::cout << "Der von 'GraphAlgorithms::compute_spann_forest' berechnete Spannwald ist: \n";
    GraphAuxPrint::print_graph(output_graph);
    std::cout << "\n";

    return output_graph;
}


std::vector<Graph::NodeId> GraphAux::get_isolated_nodes(const Graph& input_graph) {
    std::vector<Graph::NodeId> output;

    for( const auto& curr_node: input_graph.nodes()) {
        if( curr_node.num_neighbors() == 0) {
            output.push_back(curr_node.node_id());
        }
    }

    return output;
}