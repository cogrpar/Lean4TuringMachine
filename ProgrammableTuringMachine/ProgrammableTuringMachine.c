#include <lean/lean.h>

lean_object* initialize_Lean4TuringMachine(lean_object*);

int main(int argc, char *argv[]) { // TEMP compile with leanc bin/ProgrammableTuringMachine.o bin/Lean4TuringMachine.o bin/AxiomaticSystem.o bin/AxiomaticSystem/dep.o -o output
    printf ("hello world\n");  

    printf ("initializing test lean script...\n");
    lean_object *res = initialize_Lean4TuringMachine(lean_io_mk_world());
    if (lean_io_result_is_error(res)) {
        printf ("an error ocurred, exiting...\n");
        return EXIT_FAILURE;
    }
    printf ("done!\n");

    return EXIT_SUCCESS;
}