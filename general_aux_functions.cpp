//
// Created by Lukas on 26.06.2022.
//

#include "general_aux_functions.h"

std::vector<unsigned int> GeneralAux::get_range_of_uns_ints(unsigned int start, unsigned int end) {
    std::vector<unsigned int> output;
    for(unsigned int i=start; i<end; i++) {
        output.push_back(i);
    }
    return output;
}
