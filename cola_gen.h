#ifndef COLA_GEN_H
#define	COLA_GEN_H

typedef struct Nodo_gen_tmp Nodo_gen;

struct Nodo_gen_tmp {
    void *dato;
    Nodo_gen *sig;
};

typedef struct{
    Nodo_gen *pri;
    Nodo_gen *ult; // Almaceno el ult para mantener T(n) = cte.
    int cant;
} Cola_gen;


/* Crea una cola vacia */
void cola_create(Cola_gen *cola);

/* Devuelve distinto de cero si la cola esta vacia */
int cola_vacia(Cola_gen cola);

/* Encola en 'cola' */
void _cola_encolar(Cola_gen *cola, void *dato, int size);
#define cola_encolar(cola,dato,tipo) _cola_encolar(cola,dato,sizeof(tipo))

/* Desencola una operacion de 'cola' --No verifica si esta vacia-- */
void _cola_desencolar(Cola_gen *cola, void *dest, int size);
#define cola_desencolar(cola,dato,tipo) _cola_desencolar(cola,dato,sizeof(tipo))

/* Libera una cola */
void cola_destroy(Cola_gen *cola);

#endif

