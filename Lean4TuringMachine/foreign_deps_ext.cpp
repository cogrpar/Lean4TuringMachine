#include <stdio.h>
#include <iostream>
#include <string>
#include <lean/lean.h>

extern "C" lean_object * general_transition_function(/*lean_object *Q, lean_object *Gamma, lean_object *transitions, lean_object *program,*/ lean_object *q, lean_object *s, lean_object /* w */) {
    // returns (String × (String × String)) of the form Q × Γ × {−1, 1}

    //std::string TransitionsArray [2] = {(char*)lean_array_get_core(transitions, 0), (char*)lean_array_get_core(transitions, 1)};

    //lean_object *ReturnState = lean_mk_string("done");
    //lean_object *ReturnSymbol = lean_mk_string("0");
    //lean_object *ReturnTransition = lean_mk_string("-1");
    //lean_object *res = lean_alloc_ctor(0, 3, 0);
    //lean_ctor_set(res, 0, ReturnState);
    //lean_ctor_set(res, 1, ReturnSymbol);
    //lean_ctor_set(res, 2, ReturnTransition);

    return lean_mk_string("done; 0; -1");
}