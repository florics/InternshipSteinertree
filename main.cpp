#include <iostream>

#include "graph.h"
#include "mehlhorn_with_local_search.h"


int main(int argc, char *argv[]) {

    if(argc!=2) {
        std::cout << "Kein Dateiname eingegeben\n";
        return 1;
    }


    Graph g( argv[1] );

    Mehlhorn_with_LocalSearch::complete_algorithm(g);



    return 0;
}