simpleshell:  src/simpleshell.o src/commands.o src/paths.o src/run.o src/input.o src/tokens.o src/cmd_config.o src/builtin_cmd_redirect.o
	gcc -Wall src/simpleshell.o src/commands.o src/paths.o src/run.o src/input.o src/tokens.o src/cmd_config.o src/builtin_cmd_redirect.o -o ./bin/simpleshell

simpleshell.o: src/simpleshell.c
	gcc -Wall -c src/simpleshell.c

commands.o: src/commands.c
	gcc -Wall -c src/commands.c

paths.o: src/paths.c
	gcc -Wall -c src/paths.c

run.o: src/run.c
	gcc -Wall -c src/run.c

input.o: src/input.c
	gcc -Wall -c src/input.c

tokens.o: src/tokens.c
	gcc -Wall -c src/tokens.c

cmd_config.o: src/cmd_config.c
	gcc -Wall -c src/cmd_config.c

builtin_cmd_redirect.o: src/builtin_cmd_redirect.c
	gcc -Wall -c src/builtin_cmd_redirect.c

clean:
	rm -rf src/*.o 
	rm -f ./bin/simpleshell
	rm -f *.txt


#---------------------------------------------------------------------------------------------------------------------------------------------
#Name: Lucero Dafne Sangueza Espinoza
#Student Number: 23100952
#I declare that this assignment is my own work and that I acknowledge the DCU Academic Integrity Policy
#---------------------------------------------------------------------------------------------------------------------------------------------

