#include <stdio.h>
#include <cstring>
#include <vector>
#include <lean/lean.h>

std::vector<char*> split (char* input, char* delim){
    char* inputCopy = strdup(input);
    std::vector<char*> result = {};
    result.push_back(std::strtok(inputCopy, delim));
    char* next = strtok(NULL, delim);
    int i = 1;
    while (next != NULL){
        result.push_back(next);
        next = strtok(NULL, delim);
        i++;
    }
    free(inputCopy);
    return result;
}

extern "C" lean_object * general_transition_function(lean_object *Q, lean_object *Gamma, lean_object *transitions, lean_object *table, lean_object *q, lean_object *s, double Q_len, double Gamma_len, lean_object /* w */) {
    // returns (String × (String × String)) of the form Q × Γ × {−1, 1}

    // convert Q from List String to a char* array
    char * Q_array[(int)Q_len];
    lean_object ** Q_ctor = (lean_to_ctor(Q)->m_objs);
    for (int i = 0; i < Q_len-1; i++){
        Q_array[i] = lean_to_string(Q_ctor[0])->m_data;
        Q_ctor = (lean_to_ctor(Q_ctor[1])->m_objs);
    }
    Q_array[(int)Q_len-1] = lean_to_string(Q_ctor[0])->m_data;

    // convert Gamma from List String to a char* array
    char * Gamma_array[(int)Gamma_len];
    lean_object ** Gamma_ctor = (lean_to_ctor(Gamma)->m_objs);
    for (int i = 0; i < Gamma_len-1; i++){
        Gamma_array[i] = lean_to_string(Gamma_ctor[0])->m_data;
        Gamma_ctor = (lean_to_ctor(Gamma_ctor[1])->m_objs);
    }
    Gamma_array[(int)Gamma_len-1] = lean_to_string(Gamma_ctor[0])->m_data;

    // convert transitions from (String × String) to a char* array
    char * transitions_array[2] = {lean_to_string(lean_ctor_get(transitions, 0))->m_data, lean_to_string(lean_ctor_get(transitions, 1))->m_data};

    // get the program, current state, and current symbol as strings
    char * program = lean_to_string(table)->m_data;
    char * currentState = lean_to_string(q)->m_data;
    char * currentSymbol = lean_to_string(s)->m_data;

    // split program into lines
    std::vector<char*> programLines = split(program, "\n");

    // make sure that the blank symbol is in Gamma
    

    lean_object * returnState = lean_mk_string(currentState);
    lean_object * returnSymbol = lean_mk_string(currentSymbol);
    lean_object * returnTransition = lean_mk_string(programLines.at(3));

    lean_object * res_1 = lean_alloc_ctor(0, 2, 0);
    lean_object * res_2 = lean_alloc_ctor(0, 2, 0);

    lean_ctor_set(res_2, 0, returnSymbol);
    lean_ctor_set(res_2, 1, returnTransition); // (Γ × {−1, 1})
    lean_ctor_set(res_1, 0, returnState);
    lean_ctor_set(res_1, 1, res_2); // (Q × (Γ × {−1, 1}))

    return res_1;
}
