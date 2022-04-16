cpp_file = foreign_deps_ext# change this to the cpp file storing the foreign functions
lean_file = foreign_deps# change this to the lean file storing the 'extern' definitions of the foreign functions
CC := leanc
leanpkg := leanpkg
mkfile_path := $(subst /Makefile,,$(abspath $(lastword $(MAKEFILE_LIST))))

load-dynlib: build_lean_scripts
	@echo "Done"
	@echo "load foreign objects from '--load-dynlib=${mkfile_path}/bin/${lean_file}.so'"

build_lean_scripts: link#run leanpkg build with additional commands to handle the foreign deps
	@cd ${mkfile_path}/.. && ${leanpkg} build || true
	@echo "the above line might fail if leanpkg can't find the cpp external def of the foreign deps; this is resolved in the following command..."
	@cd ${mkfile_path}/.. && LEAN_PATH=././build:build/deps/AxiomaticSystem/./build lean -o build/Lean4TuringMachine/foreign_deps.olean -c build/temp/Lean4TuringMachine/foreign_deps.c ./Lean4TuringMachine/foreign_deps.lean --load-dynlib=${mkfile_path}/bin/${lean_file}.so
	@cd ${mkfile_path}/.. && LEAN_PATH=././build:build/deps/AxiomaticSystem/./build lean -o build/Lean4TuringMachine.olean -c build/temp/Lean4TuringMachine.c ./Lean4TuringMachine.lean --load-dynlib=${mkfile_path}/bin/${lean_file}.so

link: compile_c
	@echo "linking files..."
	${CC} -shared -o ${mkfile_path}/bin/${lean_file}.so ${mkfile_path}/bin/${lean_file}.o ${mkfile_path}/bin/${cpp_file}.o

compile_c: lean_to_c
	@echo "compiling ${lean_file}.c..."
	${CC} -c -o ${mkfile_path}/bin/${lean_file}.o ${mkfile_path}/bin/${lean_file}.c

lean_to_c: compile_cpp
	@echo "converting ${lean_file}.lean to ${lean_file}.c..."
	lean -c ${mkfile_path}/bin/${lean_file}.c ${lean_file}.lean

compile_cpp: bin_dir
	@echo "compiling ${cpp_file}.cpp..."
	${CC} -c -o ${mkfile_path}/bin/${cpp_file}.o ${cpp_file}.cpp

bin_dir: clean
	@mkdir ${mkfile_path}/bin

clean:
	@rm -rf ${mkfile_path}/bin