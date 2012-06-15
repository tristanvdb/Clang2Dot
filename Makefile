
all:
	make -C clang2dot
	make -C tests

check:
	make -C clang2dot check
	make -C tests check

clean:
	make -C clang2dot clean
	make -C tests clean

