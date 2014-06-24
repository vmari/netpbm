#ifndef NETPBM_H
#define	NETPBM_H

#include <stdio.h>

typedef struct {
    char magic[3]; // P5 o P6
    int width;
    int height;
    short maxval;
    short bpp; //Bytes por pixel
    unsigned char *data;
} Netpbm;

unsigned char *netpbm_get_pixel(Netpbm *img, int i, int f);

/* Inicializa la estructura */
void netpbm_create(Netpbm *img);

/* Carga en el arreglo de la estructura toda la información 
 * en fileIn si es un archivo compatible.
 * devuelve cero si no se puede */
int netpbm_set_file(Netpbm *img, char *fileIn);

/* Guarda el arreglo almacenado en la estructura,
 * formatos, comentarios y headers en fileOut,
 * el archivo DEBE estar abierto.
 * devuelve cero si no se puede */
int netpbm_dump_file(Netpbm *img, FILE *fileOut);

/* Muestra en la salida estandar la información de la imagen */
void netpbm_info(Netpbm *img);

/* Libera toda la memoria alocada de la imágen */
void netpbm_destroy(Netpbm *img);

/* Comienzo de operaciones */

void netpbm_invertir(Netpbm *img);

void netpbm_voltear_horizontal(Netpbm *img);

void netpbm_voltear_vertical(Netpbm *img);

void netpbm_multiplicar(Netpbm *img);

void netpbm_dividir(Netpbm *img);

void netpbm_rotar_izquierda(Netpbm *img);

void netpbm_rotar_derecha(Netpbm *img);

void netpbm_desenfocar(Netpbm *img);

#endif