#include <iostream>
#include "voronoi_diagram.cpp"
//#include "graph.cpp"

int main(int argc, char *argv[]) {
    if(argc!=2) {
        std::cout << "Kein Dateiname eingegeben\n";
        return 1;
    }

    // C:\\Users\\Lukas\\CLionProjects\\PraktikumSteinerbaum\\Test-Instanzen\\graph-vd1.txt
    // "C:\\Users\\Lukas\\CLionProjects\\PraktikumSteinerbaum\\Test-Instanzen\\test-graph1.txt"
    // "C:\\Users\\Lukas\\CLionProjects\\PraktikumSteinerbaum\\Test-Instanzen4\\rect_instance_1.stp"
    Graph g( argv[1] );
    //g.print();

    std::vector<Graph::NodeId> my_bases = g.get_vect_term();
    Voronoi_diagram my_vd(my_bases, g);
    //my_vd.print_simple(g);

    //my_vd.print_min_bound_edges(g);

    Graph g_1_strich = my_vd.construct_aux_graph(g);
    //g_1_strich.print();

    std::vector<std::vector<std::pair<Graph::EdgeId, Graph::PathLength>>> vect_min_bound_edges = my_vd.min_bound_edges(g);
    Graph g_3 = my_vd.turn_into_subgraph(g_1_strich, g, vect_min_bound_edges);
    g_3.print();

    return 0;
}
