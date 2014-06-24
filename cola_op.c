#include <stdlib.h>
#include <stdarg.h>
#include "cola_op.h"

void cola_create(Cola_op *cola) {
    cola->pri = NULL;
    cola->ult = NULL;
}

int cola_vacia(Cola_op cola) {
    return (cola.pri == NULL);
}

void cola_encolar(Cola_op *cola, Operacion fn) {
    Nodo_op *nodo = NULL;
    nodo = malloc(sizeof (Nodo_op));
    nodo->op = fn;
    nodo->sig = NULL;

    if (cola_vacia(*cola)) {
        cola->pri = nodo;
        cola->ult = nodo;
    } else {
        cola->ult = cola->ult->sig = nodo;
    }
}

Operacion cola_desencolar(Cola_op *cola) {
    if (cola->ult == cola->pri) {
        cola->ult = NULL;
    }
    Operacion ret = cola->pri->op;
    Nodo_op *tmp = cola->pri->sig;
    free(cola->pri);
    cola->pri = tmp;
    return ret;
}

void cola_destroy(Cola_op *cola) {
    while (!cola_vacia(*cola)) cola_desencolar(cola);
}