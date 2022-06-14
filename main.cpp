#include <iostream>

#include "graph.h"
//#include "graph_aux_functions.h"
//#include "graph_algorithms.h"
//#include "voronoi_diagram.h"
#include "mehlhorns_algo.h"

int main(int argc, char *argv[]) {
    if(argc!=2) {
        std::cout << "Kein Dateiname eingegeben\n";
        return 1;
    }

    // C:\\Users\\Lukas\\CLionProjects\\PraktikumSteinerbaum\\Test-Instanzen\\graph-vd1.txt
    // "C:\\Users\\Lukas\\CLionProjects\\PraktikumSteinerbaum\\Test-Instanzen\\test-graph1.txt"
    // "C:\\Users\\Lukas\\CLionProjects\\PraktikumSteinerbaum\\Test-Instanzen4\\rect_instance_1.stp"
    Graph g( argv[1] );
    g.print();


    Graph st = mehlhorns_algo(g);
    st.print();


    return 0;
}
