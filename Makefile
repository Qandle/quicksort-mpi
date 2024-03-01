run5:
	./main.exe ${NP}
	./main.exe ${NP}
	./main.exe ${NP}
	./main.exe ${NP}
	./main.exe ${NP}

ARG=-fopenmp -g -I/opt/homebrew/Cellar/mpich/4.1.2/include -L/opt/homebrew/Cellar/mpich/4.1.2/lib -lmpi -lpmpi

build:
	gcc-13 $(CODE) $(ARG) -o $(OUT)

buildchecker:
	gcc-13 issorted.c $(ARG) -o issorted

buildruncheck: buildchecker build
	./${OUT} ${NP}
	./issorted output.txt

buildruncheckall: buildchecker build
	./${OUT} 1
	./${OUT} 2
	./${OUT} 3
	./${OUT} 4
	./${OUT} 5
	./${OUT} 6
	./${OUT} 7
	./${OUT} 8
	./issorted output.txt
	