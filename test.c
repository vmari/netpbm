#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "cola_gen.h"

int main( int argc, char *argv[] ){
	
	Cola_gen cola;
	
	cola_create(&cola);
	
	int i = 23;
	
	cola_encolar(&cola, &i, int);
	
	int f;
	
	cola_desencolar(&cola, &f, int);
	
	printf("%d",f);
	
	return EXIT_SUCCESS;
}