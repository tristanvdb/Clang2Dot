
CXXFLAGS=`llvm-config --cppflags`
LDFLAGS=`llvm-config --ldflags` -lclangFrontendTool -lclangStaticAnalyzerFrontend -lclangStaticAnalyzerCheckers -lclangStaticAnalyzerCore -lclangIndex -lclangFrontend -lclangCodeGen  -lclangARCMigrate -lclangRewrite -lclangSerialization -lclangDriver -lclangParse -lclangSema -lclangAnalysis -lclangAST -lclangLex -lclangBasic  `llvm-config --libs engine ipo bitwriter linker asmparser instrumentation`

all: clang2dot

clang2dot: clang-to-dot.o
	g++ $(LDFLAGS) clang-to-dot.o -o clang2dot

clang-to-dot.o: clang-to-dot.cpp clang-to-dot.hpp
	g++ $(CXXFLAGS) -c clang-to-dot.cpp -o clang-to-dot.o

check: clang2dot

clean:
	rm -f clang-to-dot.o clang2dot
