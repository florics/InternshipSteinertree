#include <iostream>
#include "voronoi_diagram.cpp"

int main(int argc, char *argv[]) {
    if(argc!=2) {
        std::cout << "Kein Dateiname eingegeben\n";
        return 1;
    }

    // C:\Users\Lukas\CLionProjects\PraktikumSteinerbaum\Test-Instanzen\graph-vd1.txt
    // "C:\\Users\\Lukas\\CLionProjects\\PraktikumSteinerbaum\\Test-Instanzen\\test-graph1.txt"
    // "C:\\Users\\Lukas\\Documents\\Uni\\Mathestudium\\Programmierpraktikum\\Praxis\\Instanzen\\stp_instances.zip\\4\\rect_instance_1.txt"
    Graph g( argv[1] );
    g.print();

    std::vector<Graph::NodeId> my_bases = g.get_vect_term();
    Voronoi_diagram my_vd(my_bases, g);
    my_vd.print_simple();
    my_vd.print_min_bound_edges(g);


    return 0;
}
