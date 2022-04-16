cpp_file = foreign_deps_ext# change this to the cpp file storing the foreign functions
lean_file = foreign_deps# change this to the lean file storing the 'extern' definitions of the foreign functions
CC := leanc
leanpkg := leanpkg
mkfile_path := $(subst /Makefile,,$(abspath $(lastword $(MAKEFILE_LIST))))

compile_ProgrammableTuringMachine : compile_deps
	${CC} ProgrammableTuringMachine.c -c -o bin/ProgrammableTuringMachine.o

compile_deps : copy_c_files
	${CC} temp/Lean4TuringMachine/foreign_deps.c -c -o bin/Lean4TuringMachine/foreign_deps.o
	${CC} temp/Lean4TuringMachine.c -c -o bin/Lean4TuringMachine.o

copy_c_files: build_lean_scripts#copy the c files for the lean scripts within the build directory to mkfile_path
	@cp -rf ${mkfile_path}/../build/temp/* ${mkfile_path}/temp

build_lean_scripts: bin_dir#build the lean project in the parent directory to compile them to c files
	@cd ${mkfile_path}/.. && ${leanpkg} build || true
	@echo "the above line might fail if leanpkg can't find the cpp external def of the foreign deps; this is resolved in the following command..."
	@cd ${mkfile_path}/.. && LEAN_PATH=././build:build/deps/AxiomaticSystem/./build lean -o build/Lean4TuringMachine/foreign_deps.olean -c build/temp/Lean4TuringMachine/foreign_deps.c ./Lean4TuringMachine/foreign_deps.lean --load-dynlib=${mkfile_path}/../Lean4TuringMachine/bin/${lean_file}.so
	@cd ${mkfile_path}/.. && LEAN_PATH=././build:build/deps/AxiomaticSystem/./build lean -o build/Lean4TuringMachine.olean -c build/temp/Lean4TuringMachine.c ./Lean4TuringMachine.lean --load-dynlib=${mkfile_path}/../Lean4TuringMachine/bin/${lean_file}.so

bin_dir: clean
	@mkdir ${mkfile_path}/bin
	@mkdir ${mkfile_path}/bin/Lean4TuringMachine
	@mkdir ${mkfile_path}/temp

clean:
	@rm -rf ${mkfile_path}/bin || true
	@rm -rf ${mkfile_path}/temp || true
