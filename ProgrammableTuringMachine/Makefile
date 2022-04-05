CC := leanc
leanpkg := leanpkg
mkfile_path := $(subst /Makefile,,$(abspath $(lastword $(MAKEFILE_LIST))))

compile_ProgrammableTuringMachine : compile_deps
	${CC} ProgrammableTuringMachine.c -c -o bin/ProgrammableTuringMachine.o

compile_deps : copy_c_files
	${CC} temp/AxiomaticSystem.c -c -o bin/AxiomaticSystem.o
	${CC} temp/AxiomaticSystem/dep.c -c -o bin/AxiomaticSystem/dep.o
	${CC} temp/Lean4TuringMachine.c -c -o bin/Lean4TuringMachine.o

copy_c_files: build_lean_scripts#copy the c files for the lean scripts within the build directory to mkfile_path
	@cp -rf ${mkfile_path}/../build/temp/* ${mkfile_path}/temp

build_lean_scripts: bin_dir#build the lean project in the parent directory to compile them to c files
	@cd ${mkfile_path}/.. && ${leanpkg} build

bin_dir: clean
	@mkdir ${mkfile_path}/bin
	@mkdir ${mkfile_path}/bin/AxiomaticSystem
	@mkdir ${mkfile_path}/temp

clean:
	@rm -rf ${mkfile_path}/bin || true
	@rm -rf ${mkfile_path}/temp/AxiomaticSystem || true
	@rm ${mkfile_path}/temp/AxiomaticSystem.c || true
	@rm ${mkfile_path}/temp/Lean4TuringMachine.c || true
