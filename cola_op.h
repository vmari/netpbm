#ifndef COLA_OP_H
#define	COLA_OP_H

#include "netpbm.h"

typedef void (*Operacion)(Netpbm*);

typedef struct Nodo_op_tmp Nodo_op;

struct Nodo_op_tmp {
    Operacion op;
    Nodo_op *sig;
};

typedef struct{
    Nodo_op *pri;
    Nodo_op *ult; // Almaceno el ult para mantener T(n) = cte.
    int cant;
} Cola_op;


/* Crea una cola vacia */
void cola_create(Cola_op *cola);

/* Devuelve distinto de cero si la cola esta vacia */
int cola_vacia(Cola_op cola);

/* Encola una operacion en 'cola' */
void cola_encolar(Cola_op *cola, Operacion fn);

/* Desencola una operacion de 'cola' --No verifica si esta vacia-- */
Operacion cola_desencolar(Cola_op *cola);

/* Libera una cola */
void cola_destroy(Cola_op *cola);

#endif

