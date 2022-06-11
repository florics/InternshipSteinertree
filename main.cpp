#include <iostream>
//#include "voronoi_diagram.cpp"
#include "mehlhorns_algo.cpp"
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

    Graph st = mehlhorns_algo(g);
    st.print();

    return 0;
}
