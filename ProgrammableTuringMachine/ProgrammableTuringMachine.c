#include <lean/lean.h>

lean_object* initialize_Lean4TuringMachine(lean_object*);

int main(int argc, char *argv[]) { // TEMP compile with leanc bin/ProgrammableTuringMachine.o bin/Lean4TuringMachine.o bin/AxiomaticSystem.o bin/AxiomaticSystem/dep.o -o bin/output
    printf ("hello world\n");  

    return EXIT_SUCCESS;
}