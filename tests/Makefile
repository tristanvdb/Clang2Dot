
all:

###############################
# Tests Targets and Variables #
###############################

  # Test Codes

C_TEST_CODES = \
test_0.c \
test_1.c \
test_2.c

CXX_TEST_CODES = \
test_0.cpp \
test_1.cpp \
test_2.cpp \
test_3.cpp \
test_4.cpp \
test_5.cpp

  # GraphViz targets

C_DOT_TARGETS = ${C_TEST_CODES:.c=_c.dot}

CXX_DOT_TARGETS = ${CXX_TEST_CODES:.cpp=_cpp.dot}

$(C_DOT_TARGETS): %_c.dot: %.c ../clang2dot/clang2dot
	../clang2dot/clang2dot $< > $@

$(CXX_DOT_TARGETS): %_cpp.dot: %.cpp ../clang2dot/clang2dot
	../clang2dot/clang2dot $< > $@

  # png targets

C_PNG_TARGETS = ${C_TEST_CODES:.c=_c.png}

CXX_PNG_TARGETS = ${CXX_TEST_CODES:.cpp=_cpp.png}

%.png: %.dot
	dot -Tpng $< -o $@

  # check: generate GraphViz files

dot:
	make $(C_DOT_TARGETS)
	make $(CXX_DOT_TARGETS)

  # make png

png:
	make $(C_PNG_TARGETS)
	make $(CXX_PNG_TARGETS)


  # check rule: generate the png to check if generated dot files are correct

check: png

###########################
# Binary building targets #
###########################

../clang2dot/clang2dot:
	make -C ../clang2dot/ clang2dot

##################
# 'clean' target #
##################

clean:
	rm -f *.dot *.png


