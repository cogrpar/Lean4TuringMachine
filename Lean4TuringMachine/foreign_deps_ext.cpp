#include <stdio.h>
#include <assert.h>
#include <string>
#include <lean/lean.h>

extern "C" lean_object * general_transition_function(lean_object *Q, lean_object *Gamma, lean_object *transitions, lean_object *program, lean_object *q, lean_object *s, lean_object /* w */) {
    // returns (String × (String × String)) of the form Q × Γ × {−1, 1}

    lean_object *ReturnState = lean_mk_string("done");
    lean_object *ReturnSymbol = lean_mk_string("0");
    lean_object *ReturnTransition = lean_mk_string("-1");

    lean_object *res_1 = lean_alloc_ctor(0, 2, 0);
    lean_object *res_2 = lean_alloc_ctor(0, 2, 0);

    lean_ctor_set(res_2, 0, ReturnSymbol);
    lean_ctor_set(res_2, 1, ReturnTransition); // (Γ × {−1, 1})
    lean_ctor_set(res_1, 0, ReturnState);
    lean_ctor_set(res_1, 1, res_2); // (Q × (Γ × {−1, 1}))

    return res_1;
}