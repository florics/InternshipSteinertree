//
// Created by Lukas on 26.06.2022.
//

#ifndef PRAKTIKUMSTEINERBAUM_TESTS_H
#define PRAKTIKUMSTEINERBAUM_TESTS_H

#include <iostream>

#include "../graph.h"
#include "../graph_aux_functions.h"
#include "../graph_algorithms.h"
#include "../voronoi_diagram.h"
#include "../vor_diag_aux_functions.h"
#include "../mehlhorns_algo.h"
#include "../local_search_aux.h"
#include "../key_path_exchange.h"
#include "../EdgeSequence.h"
#include "../Edge_Heaps.h"
#include "../key_vertex_elimination.h"

namespace DebugTests{

    //void kpe_evaluate_neighborhood();

    void kve_1_evaluate_neighborhood();
    void kve_2_evaluate_neighborhood();
    void kve_3_evaluate_neighborhood();

    std::vector<Graph> get_standard_test_instances();

    //liest Datei mit Pfaden aus und erstellt für jeden Pfad aus der entsprechenden Datei einen Graphen
    std::vector<Graph> get_test_instances_from_file(char const* filename);

}

#endif //PRAKTIKUMSTEINERBAUM_TESTS_H
