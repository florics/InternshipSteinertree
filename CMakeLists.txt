cmake_minimum_required(VERSION 3.22)
project(PraktikumSteinerbaum)

set(CMAKE_CXX_STANDARD 20)

add_executable(PraktikumSteinerbaum main.cpp
        string_aux_functions.cpp general_aux_functions.cpp
        graph.cpp graph_aux_functions.cpp graph_printfunctions.cpp graph_algorithms.cpp
        voronoi_diagram.cpp vor_diag_aux_functions.cpp
        mehlhorns_algo.cpp
        Union_Find_Structure.cpp Ext_Union_Find_Structure.cpp
        BoundEdgeHeaps.cpp EdgeSequence.cpp ImprovingChangement.cpp
        local_search_aux.cpp key_path_exchange.cpp key_vertex_elimination.cpp
        tests.cpp
        Subgraph.cpp Supergraph.cpp
        Tree_Data_Structure.cpp)