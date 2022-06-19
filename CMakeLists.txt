cmake_minimum_required(VERSION 3.22)
project(PraktikumSteinerbaum)

set(CMAKE_CXX_STANDARD 20)

add_executable(PraktikumSteinerbaum main.cpp graph.cpp graph_aux_functions.cpp graph_printfunctions.cpp voronoi_diagram.cpp graph_algorithms.cpp mehlhorns_algo.cpp string_aux_functions.cpp vor_diag_aux_functions.cpp vor_diag_aux_functions.h Union_Find_Structure.cpp Union_Find_Structure.h)
