CC := leanc
mkfile_path := $(subst /Makefile,,$(abspath $(lastword $(MAKEFILE_LIST))))

compile_ProgrammableTuringMachine : compile_AxiomaticSystem
	${CC} ProgrammableTuringMachine.c -c -o bin/ProgrammableTuringMachine.o

compile_AxiomaticSystem : bin_dir
	${CC} AxiomaticSystem.c -c -o bin/AxiomaticSystem.o
	${CC} AxiomaticSystem/dep.c -c -o bin/AxiomaticSystem/dep.o

bin_dir: clean
	@mkdir ${mkfile_path}/bin
	@mkdir ${mkfile_path}/bin/AxiomaticSystem

clean:
	@rm -rf ${mkfile_path}/bin