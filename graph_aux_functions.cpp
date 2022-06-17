//
// Created by Lukas on 14.06.2022.
//

#include "graph_aux_functions.h"

#include "iostream"

#include "graph_printfunctions.h"

//todo Funktionennamen aktualisieren?

// ? nicht sicher ob das funzt: die Instanzen der Uni sind alle isomorph laut dieser Methode
bool check_if_isomorph(const Graph& graph1, const Graph& graph2){
    //Eingabe prüfen
    if(not edgeweight_finite(graph1)){
        throw std::runtime_error("(check_if_isomorph) eingegebener Graph graph1 hat unendliche Kantengewichte");
    }
    if(not edgeweight_finite(graph2)){
        throw std::runtime_error("(check_if_isomorph) eingegebener Graph graph2 hat unendliche Kantengewichte");
    }
    if(not check_if_simple(graph1) ){
        throw std::runtime_error("(check_if_isomorph) eingegebener Graph graph1 ist nicht einfach");
    }
    if(not check_if_simple(graph2) ){
        throw std::runtime_error("(check_if_isomorph) eingegebener Graph graph2 ist nicht einfach");
    }

    //leichte Checks
    if( graph1.num_nodes() != graph2.num_nodes() ){
        std::cout << "(check_if_isomorph) Knotenanzahl nicht gleich \n";
        return false;
    }
    if( graph1.num_edges() != graph2.num_edges() ){
        std::cout << "(check_if_isomorph) Kantenanzahl nicht gleich \n";
        return false;
    }

    //Matrix für alle Knotenpaare, Adjazenzmatrix
    // edge_weight_matrice[i][j] entspricht Knotenpaar i+1, j (wobei i >= j)
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
            std::cout << "(check_if_isomorph) Kante ";
            print_edge_as_pair(graph2, curr_edge.edge_id() );
            std::cout << " liegt im zweiten eingegebenen Graph aber nicht im ersten. \n";
            return false;
        }
        if( edge_weight_matrice[ curr_edge_nodes.second-1 ][ curr_edge_nodes.first ] != curr_edge.weight() ){
            std::cout << "(check_if_isomorph) Kantengewicht der Kante ";
            print_edge_as_pair(graph2, curr_edge.edge_id() );
            std::cout << "ungleich. \n";
            return false;
        }

        //am Ende sollte die Adjazenz-Matrix nur false enthalten
        adjacency_matrice[ curr_edge_nodes.second-1 ][ curr_edge_nodes.first ] = false;
    }

    for(unsigned int i = 0; i < graph1.num_nodes()-1; i++){
        for(unsigned int j = 0; j < i+1; j++){
            if( adjacency_matrice[i][j] ){
                std::cout << "(check_if_isomorph) Kante (" << i+1 << "," << j << ") liegt im ersten eingegebenen Graph aber nicht im zweiten. \n";
                return false;
            }
        }
    }

    return true;
}

bool edgeweight_nonnegative(const Graph& input_graph) {
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

bool edgeweight_finite(const Graph& input_graph) {
    for(auto var_edge : input_graph.edges()){
        if( var_edge.weight() == Graph::infinite_weight ){
            return false;
        }
    }
    return true;
}

bool check_if_simple(const Graph& input_graph) {
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
            std::cout << "(check_if_simple) Kante (bzw. Knotenpaar) ";
            print_edge_as_pair(input_graph, curr_edge.edge_id() );
            std::cout << " ist doppelt. \n";
            return false;
        }
        adjacency_matrice[ curr_edge_nodes.second-1 ][ curr_edge_nodes.first ] = true;
    }

    return true;
}

Graph copygraph_wo_edges(const Graph& input_graph){
    Graph output(0);
    for( const auto& curr_node : input_graph.nodes()){
        output.add_one_node( curr_node.node_name(), curr_node.terminal_state() );
    }
    return output;
}

Graph copygraph_wo_iso_nodes(const Graph& input_graph){
    Graph output_graph(0);
    std::vector< Graph::NodeId > new_node_ids ( input_graph.num_nodes(), Graph::invalid_node_id );

    //Knoten hinzufügen
    unsigned int output_graph_nodes = 0;
    for(Graph::NodeId i=0; i<input_graph.num_nodes(); i++){
        Graph::Node curr_node = input_graph.get_node(i);
        if( curr_node.num_neighbors() != 0 ){
            output_graph.add_one_node( curr_node.node_name(), curr_node.terminal_state() );

            //merke node_id bzgl output-Graph des Knoten mit node_id i bzgl g
            new_node_ids[ i ] = output_graph_nodes;
            output_graph_nodes++;
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

Graph copygraph_wo_steinerleafs(const Graph& input_graph){
    Graph output_graph(0);
    std::vector< Graph::NodeId > new_node_ids ( input_graph.num_nodes(), Graph::invalid_node_id );

    //Knoten hinzufügen
    unsigned int output_graph_nodes = 0;
    for(Graph::NodeId i=0; i<input_graph.num_nodes(); i++){
        Graph::Node curr_node = input_graph.get_node(i);
        if( curr_node.num_neighbors() == 1 && not curr_node.check_terminal() ) {
            continue;
        } else {
            output_graph.add_one_node( curr_node.node_name(), curr_node.terminal_state() );

            //merke node_id bzgl output-Graph des Knoten mit node_id i bzgl g
            new_node_ids[ i ] = output_graph_nodes;
            output_graph_nodes++;
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

Graph::PathLength length_of_all_edges(const Graph& input_graph) {
    Graph::PathLength output = 0;
    for(auto curr_edge : input_graph.edges() ) {
        output += curr_edge.weight();
    }
    return output;
}

void print_length_of_all_edges(const Graph& input_graph) {
    Graph::PathLength output = length_of_all_edges(input_graph);
    std::cout << "Das Gewicht des Graphen ist ";
    print_pathlength( output );
    std::cout << "\n";
}