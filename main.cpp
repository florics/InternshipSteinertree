#include <iostream>
#include "voronoi_diagram.cpp"

int main() {

    // C:\Users\Lukas\CLionProjects\PraktikumSteinerbaum\Test-Instanzen\graph-vd1.txt
    // "C:\\Users\\Lukas\\CLionProjects\\PraktikumSteinerbaum\\Test-Instanzen\\test-graph1.txt
    Graph g("C:\\Users\\Lukas\\CLionProjects\\PraktikumSteinerbaum\\Test-Instanzen\\graph-vd1.txt");
    g.print();

    std::vector<Graph::NodeId> my_bases = g.get_vect_term();
    Voronoi_diagram my_vd(my_bases, g);
    my_vd.print_simple();


    return 0;
}
