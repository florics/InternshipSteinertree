//
// Created by Lukas on 15.06.2022.
//

#include "string_aux_functions.h"

#include "algorithm"

bool compare_strings_caseinsensitive(std::string s1, std::string s2) {
    std::transform(s1.begin(), s1.end(), s1.begin(), ::toupper);
    std::transform(s2.begin(), s2.end(), s2.begin(), ::toupper);
    if( s1 == s2 ){
        return true;
    } else {
        return false;
    }
}