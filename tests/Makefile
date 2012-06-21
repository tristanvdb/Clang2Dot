
all:

check: ../clang2dot/clang2dot
	../clang2dot/clang2dot test_0.c > test_0.dot
	dot -Tpng test_0.dot -o test_0.png
	../clang2dot/clang2dot test_1.c > test_1.dot
	dot -Tpng test_1.dot -o test_1.png
	../clang2dot/clang2dot test_2.c > test_2.dot
	dot -Tpng test_2.dot -o test_2.png

../clang2dot/clang2dot:
	make -C ../clang2dot/

clean:
	rm -f *.dot *.png


