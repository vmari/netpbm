#include <stdlib.h>
#include <stdarg.h>
#include <string.h>
#include "cola_gen.h"

void cola_create(Cola_gen *cola) {
	cola->pri = NULL;
	cola->ult = NULL;
}

int cola_vacia(Cola_gen cola) {
	return (cola.pri == NULL);
}

void _cola_encolar(Cola_gen *cola, void *dato, int size){
	Nodo_gen *tmp;
	tmp = malloc(sizeof(Nodo_gen));
	tmp->dato = malloc(size);
	memcpy(tmp->dato, dato, size);
	tmp->sig = NULL;

	if (cola_vacia(*cola)) {
		cola->pri = tmp;
		cola->ult = tmp;
	} else {
		cola->ult = cola->ult->sig = tmp;
	}
}

void _cola_desencolar(Cola_gen *cola, void *dest, int size) {
	if (cola->ult == cola->pri) {
		cola->ult = NULL;
	}
	memcpy(dest, cola->pri->dato, size);
	free(cola->pri->dato);
	Nodo_gen *tmp = cola->pri->sig;
	free(cola->pri);
	cola->pri = tmp;
}

void cola_destroy(Cola_gen *cola) {
	Nodo_gen *tmp = cola->pri;
	Nodo_gen *sig = cola->pri->sig;
	while (tmp){
		free(tmp->dato);
		free(tmp);
		tmp = sig;
		sig = sig->sig;
	}
	cola->pri = NULL;
	cola->ult = NULL;
}