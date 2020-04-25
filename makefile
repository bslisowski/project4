# Brendan Lisowski
# CIS 3207 Project 4
procsig: procsig.c prochandle.c procgen.c procsig.h
	gcc -g -pthread procsig.c prochandle.c procgen.c -o procsig -Wall -Werror
threadsig: threadsig.c threadhandle.c threadgen.c threadsig.h
	gcc -g -pthread threadsig.c threadhandle.c threadgen.c -o threadsig -Wall -Werror
