//
// Created by Lukas on 07.07.2022.
//

#include "steiner_vertex_insertion.h"

#include "iostream"

#include "graph_printfunctions.h"
#include "graph_aux_functions.h"

Graph::PathLength SteinerVertexInsertion::process_node(Graph::NodeId node_to_insert, const Graph& original_graph,
                                          Insertion_Tree_Data_Structure &tree_data_structure) {

    // bezieht sich auf den Eingabeknoten
    unsigned int num_neighbors = original_graph.get_node(node_to_insert).num_neighbors();

    //falls der Eingabeknoten weniger als 2 Nachbarn hat, können wir keine (echte) Verbesserung erzielen,
    // indem wir ihn zum Baum hinzufügen (bei positiven Kantengewichten)
    if( num_neighbors < 2) {
        return 0;
    }

    //? const std::vector<Insertion_Tree_Data_Structure::NodeId>& tree_nodeids_of_orig_nodes = tree_data_structure.get_tree_nodeid_of_original_node();

    bool node_added = false;

    //Wert der gefundenen Verbesserung (kann auch negativ sein)
    Graph::PathLength improve_value = 0;

    //Zwischenspeichern der Kanten, die wir beim Einsetzen von Kanten aus der Nachbarschaft des aktuellen Knotens entfernen
    std::vector<Insertion_Tree_Data_Structure::TreeEdge> removed_edges;
    removed_edges.reserve(num_neighbors);
    //Zwischenspeichern der Pfade, die wir beim Einsetzen von Kanten aus der Nachbarschaft des aktuellen Knotens umkehren
    std::vector<Insertion_Tree_Data_Structure::TreePath> reversed_paths;
    reversed_paths.reserve(num_neighbors);

    for(auto curr_inc_edge_id: original_graph.get_node(node_to_insert).incident_edge_ids()) {

        const Graph::Edge& curr_inc_edge = original_graph.get_edge(curr_inc_edge_id);
        Graph::NodeId curr_neighbor = curr_inc_edge.get_other_node(node_to_insert);

        //prüfe, ob der aktuelle Nachbarn in der Datenstruktur liegt
        if(tree_data_structure.get_tree_nodeid_of_original_node(curr_neighbor) != Graph::invalid_node_id ) {

            if( not node_added) {
                //falls der Knoten noch nicht hinzugefügt wurde, wollen wir ihn mit der aktuellen Kante hinzufügen
                tree_data_structure.add_node(node_to_insert, curr_neighbor, curr_inc_edge.weight(), curr_inc_edge_id);
                node_added = true;

                //die Veränderung des Gewichts der Lösung, beträgt nun minus das Gewicht der aktuellen Kante
                improve_value -= curr_inc_edge.weight();
            } else {
                //sonst fügen wir die aktuelle Kante hinzu (und entfernen dabei eine andere), sofern dies eine Verbesserung darstellt
                // aktualisiere dabei den Wert der Verbesserung
                improve_value += tree_data_structure.try_to_insert_edge(curr_inc_edge, removed_edges, reversed_paths);

            }
        }
    }

    //prüfe, ob durch das Hinzufügen des Knotens und der Kanten eine echte Verbesserung erreicht wurde
    if( improve_value <= 0 && node_added) {

        //in dem Fall machen wir diese Modifikationen rückgängig

        //debug
        if( reversed_paths.size() != removed_edges.size() ) {
            throw std::runtime_error("(SteinerVertexInsertion::process_node) Vektoren haben verschieden Größen.");
        }

        // dies erfolgt Schritt für Schritt für die Kanten und Pfade, um keine Konflikte zu erzeugen
        for(int i=reversed_paths.size()-1; i>-1; i--) {
            tree_data_structure.reverse_path(reversed_paths[i]);
            tree_data_structure.set_edge(removed_edges[i]);
        }

        tree_data_structure.remove_last_node(node_to_insert);

        return 0;
    }

    return improve_value;
}

Graph::PathLength SteinerVertexInsertion::evaluate_neighborhood(const Subgraph &input_subgraph,
                                                                Insertion_Tree_Data_Structure &tree_data_structure,
                                                                std::vector<Graph::NodeId>& added_nodes) {

    const Graph& original_graph = input_subgraph.original_graph();
    //? const Graph& solution_graph = input_subgraph.this_graph();
    //? const std::vector<Graph::NodeId>& solution_nodeids_of_original_nodes = input_subgraph.subgraph_nodeids_of_nodes_in_originalgraph();
    //? const std::vector<Graph::NodeId>& original_nodeids = input_subgraph.original_nodeids();

    Graph::PathLength improve_value = 0;

    for(Graph::NodeId id = 0; id < original_graph.num_nodes(); id++) {

        //debug
        if(id==50 || id==11) {

            //int var = 0;
            //std::cout << "AChtungg";

        }

        //prüfe, ob der Knoten im Lösungsgraphen liegt
        if(tree_data_structure.get_tree_nodeid_of_original_node(id) == Graph::invalid_node_id) {

            Graph::PathLength curr_improve_value = SteinerVertexInsertion::process_node(id, original_graph, tree_data_structure);

            //prüfe, ob wir eine echte Verbesserung gefunden haben
            if(curr_improve_value > 0) {
                // in dem Fall wurde der Knoten hinzugefügt
                added_nodes.push_back(id);

                improve_value += curr_improve_value;
            }
        }
    }

    return improve_value;
}

void SteinerVertexInsertion::find_local_minimum(Subgraph &input_subgraph) {

    //? Wurzel iwie wählen?
    Graph::NodeId root_of_tree_data_structure =  input_subgraph.this_graph().get_terminals()[0];

    Insertion_Tree_Data_Structure tree_data_structure(input_subgraph, root_of_tree_data_structure);

    //debug
    //std::cout << "Wurzel: " << root_of_tree_data_structure +1 << "\n";
    //GraphAuxPrint::print_edge_vect(input_subgraph.original_graph(), tree_data_structure.get_all_edges_as_original_edge_ids());

    // Zwischenspeicher für die hinzugefügten Knoten (als NodeIds im zugrundeliegenden Graphen)
    std::vector<Graph::NodeId> added_nodes;

    //Schleife bricht ab, wenn lokales Minimum erreicht
    int debug_counter = 0;
    while(true) {
        debug_counter++;

        Graph::PathLength curr_improve_value = evaluate_neighborhood(input_subgraph, tree_data_structure, added_nodes);

        //std::cout << "Iteration von find_local_min: " << debug_counter << "\n";
        //fflush(stdout);

        if( curr_improve_value == 0) {
            break;
        }
    }

    //debug
    //std::cout << "nach der Schleife: \n";
    //GraphAuxPrint::print_edge_vect(input_subgraph.original_graph(), tree_data_structure.get_all_edges_as_original_edge_ids());

    //führe die Modifikationen, die wir bereits in der Baum-Datenstruktur vollzogen haben, auch im Subgraphen aus
    input_subgraph.add_nodes(added_nodes);
    input_subgraph.reset_edges(tree_data_structure.get_all_edges_as_original_edge_ids());

    GraphAux::remove_steinerbranches(input_subgraph);

    //debug
    //GraphAuxPrint::print_subgraph(input_subgraph);
}
