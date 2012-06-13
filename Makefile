
CXXFLAGS=`llvm-config --cppflags`
LDFLAGS=`llvm-config --ldflags` -lclangFrontendTool -lclangStaticAnalyzerFrontend -lclangStaticAnalyzerCheckers -lclangStaticAnalyzerCore -lclangIndex -lclangFrontend -lclangCodeGen  -lclangARCMigrate -lclangRewrite -lclangSerialization -lclangDriver -lclangParse -lclangSema -lclangAnalysis -lclangAST -lclangLex -lclangBasic  `llvm-config --libs engine ipo bitwriter linker asmparser instrumentation`

all: clang2dot

clang2dot: main.o clang-to-dot.o declarations.o statements.o types.o
	g++ $(LDFLAGS) clang-to-dot.o -o clang2dot

main.o: main.cpp clang-to-dot.hpp
	g++ $(CXXFLAGS) -c main.cpp -o main.o

clang-to-dot.o: clang-to-dot.cpp clang-to-dot.hpp
	g++ $(CXXFLAGS) -c clang-to-dot.cpp -o clang-to-dot.o

declarations.o: declarations.cpp clang-to-dot.hpp
	g++ $(CXXFLAGS) -c declarations.cpp -o declarations.o

statements.o: statements.cpp clang-to-dot.hpp
	g++ $(CXXFLAGS) -c statements.cpp -o statements.o

types.o: types.cpp clang-to-dot.hpp
	g++ $(CXXFLAGS) -c types.cpp -o types.o

check: clang2dot

clean:
	rm -f *.o clang2dot
