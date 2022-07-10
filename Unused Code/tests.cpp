//
// Created by Lukas on 26.06.2022.
//

#include "tests.h"

#include <string>
#include <fstream>
#include <sstream>

#include "../key_vertex_elimination.h"
#include "../graph_printfunctions.h"

/*
void DebugTests::kpe_evaluate_neighborhood(){
    Graph or_g( "C:\\Users\\Lukas\\CLionProjects\\PraktikumSteinerbaum\\Test-Instanzen\\KeyPathExchange-Tests\\kpe-or_graph3.txt" );
    Graph sol_g( "C:\\Users\\Lukas\\CLionProjects\\PraktikumSteinerbaum\\Test-Instanzen\\KeyPathExchange-Tests\\kpe-sol_graph3.txt" );
    //GraphAuxPrint::print_graph(g);

    std::vector<Graph::NodeId>  solution_nodeids_of_original_nodes {4, 5, 3, 2, Graph::invalid_node_id, 1, 0, 6};

    std::pair< std::vector<bool>, std::vector<bool> > subgraph_vectors;
    subgraph_vectors.first = {1, 1, 1, 1, 0, 1, 1, 1};
    subgraph_vectors.second = {1, 1, 1, 0, 1, 0, 1, 1};

    //std::vector<ImprovingChangement> im_ch = KeyPathExch::evaluate_neighborhood_simple(or_g, sol_g, subgraph_vectors, solution_nodeids_of_original_nodes);

}
*/

std::vector<Graph> DebugTests::get_standard_test_instances() {
    //im Ordner 4 gibt es 19 Test Instanzen
    //im Ordner 10 gibt es 22 Test Instanzen
    //im Ordner 13 gibt es 20 Test Instanzen
    std::vector<Graph> output;
    for(unsigned int i=1; i<19+1; i++) {
        //std::string var_string = "C:\\Users\\Lukas\\CLionProjects\\PraktikumSteinerbaum\\Test-Instanzen10\\rect_instance_";
        std::string var_string = "C:\\Users\\Lukas\\Documents\\Uni\\Mathestudium\\Programmierpraktikum\\Praxis\\Instanzen\\4\\rect_instance_";
        var_string += std::to_string(i);
        var_string += ".stp";

        const char * var_char = var_string.c_str();
        Graph var_graph(var_char);

        output.push_back(var_graph);
    }
    for(unsigned int i=1; i<22+1; i++) {
        //std::string var_string = "C:\\Users\\Lukas\\CLionProjects\\PraktikumSteinerbaum\\Test-Instanzen10\\rect_instance_";
        std::string var_string = "C:\\Users\\Lukas\\Documents\\Uni\\Mathestudium\\Programmierpraktikum\\Praxis\\Instanzen\\10\\rect_instance_";
        var_string += std::to_string(i);
        var_string += ".stp";

        const char * var_char = var_string.c_str();
        Graph var_graph(var_char);

        output.push_back(var_graph);
    }
    for(unsigned int i=1; i<20+1; i++) {
        //std::string var_string = "C:\\Users\\Lukas\\CLionProjects\\PraktikumSteinerbaum\\Test-Instanzen10\\rect_instance_";
        std::string var_string = "C:\\Users\\Lukas\\Documents\\Uni\\Mathestudium\\Programmierpraktikum\\Praxis\\Instanzen\\13_hard\\rect_instance_";
        var_string += std::to_string(i);
        var_string += ".stp";

        const char * var_char = var_string.c_str();
        Graph var_graph(var_char);

        output.push_back(var_graph);
    }
    return output;
}

void DebugTests::kve_1_evaluate_neighborhood() {
    Graph original_graph("C:\\Users\\Lukas\\CLionProjects\\PraktikumSteinerbaum\\Test-Instanzen\\KeyVertexElimination-Tests\\kve-graph1_or.txt");
    Graph this_graph("C:\\Users\\Lukas\\CLionProjects\\PraktikumSteinerbaum\\Test-Instanzen\\KeyVertexElimination-Tests\\kve-graph1_sol.txt");

    std::vector<Graph::NodeId> subgraph_nodeids_of_nodes_in_originalgraph = {Graph::invalid_node_id, 2, 3, 0, Graph::invalid_node_id, 1};
    std::vector<Graph::NodeId> original_nodeids = {3, 5, 1, 2};
    std::vector<Graph::EdgeId> original_edgeids = {1, 0, 6};

    Subgraph instance(original_graph, this_graph, subgraph_nodeids_of_nodes_in_originalgraph, original_nodeids, original_edgeids);

    KeyVertexElim::find_local_minimum(instance);

    GraphAuxPrint::print_graph(instance.this_graph());

}

void DebugTests::kve_2_evaluate_neighborhood() {
    Graph original_graph("C:\\Users\\Lukas\\CLionProjects\\PraktikumSteinerbaum\\Test-Instanzen\\KeyVertexElimination-Tests\\kve-graph2_or.txt");
    Graph this_graph("C:\\Users\\Lukas\\CLionProjects\\PraktikumSteinerbaum\\Test-Instanzen\\KeyVertexElimination-Tests\\kve-graph2_sol.txt");

    std::vector<Graph::NodeId> subgraph_nodeids_of_nodes_in_originalgraph = {3, 0, 2, 1};
    std::vector<Graph::NodeId> original_nodeids = {1, 3, 2, 0};
    std::vector<Graph::EdgeId> original_edgeids = {2, 0, 1};

    Subgraph instance(original_graph, this_graph, subgraph_nodeids_of_nodes_in_originalgraph, original_nodeids, original_edgeids);

    KeyVertexElim::find_local_minimum(instance);

    GraphAuxPrint::print_graph(instance.this_graph());

}

void DebugTests::kve_3_evaluate_neighborhood() {
    Graph original_graph("C:\\Users\\Lukas\\CLionProjects\\PraktikumSteinerbaum\\Test-Instanzen\\KeyVertexElimination-Tests\\kve-graph3_or.txt");
    Graph this_graph("C:\\Users\\Lukas\\CLionProjects\\PraktikumSteinerbaum\\Test-Instanzen\\KeyVertexElimination-Tests\\kve-graph3_sol.txt");

    std::vector<Graph::NodeId> subgraph_nodeids_of_nodes_in_originalgraph = {0, 3, Graph::invalid_edge_id, 2, 1};
    std::vector<Graph::NodeId> original_nodeids = {0, 4, 3, 1};
    std::vector<Graph::EdgeId> original_edgeids = {0, 2, 1};

    Subgraph instance(original_graph, this_graph, subgraph_nodeids_of_nodes_in_originalgraph, original_nodeids, original_edgeids);

    KeyVertexElim::find_local_minimum(instance);

    GraphAuxPrint::print_graph(instance.this_graph());

}

std::vector<Graph> DebugTests::get_test_instances_from_file(const char *filename) {
    std::ifstream file(filename);
    if (not file) {
        throw std::runtime_error("(DebugTests::get_test_instances_from_file) Fehler beim Öffnen der Datei");
    }

    std::vector<Graph> output;

    std::string line;

    //debug
    int graph_counter = 1;
    while (getline(file, line)) {

        if(line == "EOF") {
            //Ende der Datei erreicht
            break;
        }

        const char * var_char = line.c_str();
        Graph var_graph(var_char);

        output.push_back(var_graph);

        //std::cout << "Graph Nr. " << graph_counter << "\n";
        //fflush(stdout);

        graph_counter++;

    }


    return output;
}

//  C:\Users\Lukas\Documents\Uni\Mathestudium\Programmierpraktikum\Praxis\Instanzen\10\rect_instance_1.stp

/*

 //std::cout << i << "\n";
        fflush(stdout);

        Subgraph sol_sg = Mehlhorn::complete_algorithm(my_graphs[i]);

        //GraphAuxPrint::print_subgraph(sol_sg);

        //Graph::PathLength value_before = GraphAux::length_of_all_edges(sol_sg.this_graph());
        //GraphAuxPrint::print_length_of_all_edges(sol_sg.this_graph());

        //LocalSearchCombi::find_local_minimum_with_svi_and_kvekpe_in_sequence_each_pass(sol_sg);


        SteinerVertexElim::find_local_minimum(sol_sg);
        SteinerVertexInsertion::find_local_minimum(sol_sg);
        KeyPathExch::complete_algorithm(sol_sg);
        KeyVertexElim::find_local_minimum(sol_sg);

        //Graph::PathLength value_after = GraphAux::length_of_all_edges(sol_sg.this_graph());
        //GraphAuxPrint::print_length_of_all_edges(sol_sg.this_graph());

        //if( value_after < value_before) {
            //improve_counter ++;
        //}

 */