#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "netpbm.h"
#include "errors.h"
#include "cola_op.h"

//Comentario de prueba para git

char *argumentos = "heimvrDdIbs";

Cola_op cola;
Netpbm img;

void onclose() {
	netpbm_destroy(&img);
	cola_destroy(&cola);
}

void usage() {
	printf("\n"
			"      -h            muestra una ayuda con todas estas opciones.\n"
			"      -e <archivo>  carga el archivo con la imagen a procesar.\n"
			"      -s <archivo>  nombre del archivo de salida.\n"
			"      \n"
			"      -i            mostrar información de la imágen.\n"
			"      -m            voltear horizontalmente.\n"
			"      -v            voltear verticalmente.\n"
			"      -r (i|d)      rotar a izquierda o derecha respectivamente.\n"
			"      -D            duplica el tamaño.\n"
			"      -d            divide a la mitad el tamaño.\n"
			"      -I            inviertir colores.\n"
			"      -b            desenfocar");
}

int is_arg(char *cmp_arg) {
	return ( (strlen(cmp_arg) == 2)
			&& (cmp_arg[0] == '-')
			&& (index(argumentos, cmp_arg[1]) != NULL));
}

int main(int argc, char *argv[]) {
	cola_create(&cola);
	netpbm_create(&img);

	atexit(onclose);

	int showHelp = 0,
		showInfo = 0,
		optind = 1;

	FILE *fp = NULL;

	char *fileIn = NULL,
		*fileOut = NULL;

	for (; optind < argc; optind++) {
		if (strcmp(argv[ optind ], "-h") == 0) {
			showHelp = 1;
			
		} else if (strcmp(argv[ optind ], "-e") == 0) {
			if (optind != 1) {
				netpbm_exit(ARG_PRI, argv[optind]);
			} else if ((optind + 1) >= argc || is_arg(argv[optind + 1])) {
				netpbm_exit(SUB_REQ, argv[optind]);
			} else {
				optind++;
				fileIn = argv[optind];
			}

		} else if (strcmp(argv[ optind ], "-i") == 0) {
			showInfo = 1;

		} else if (strcmp(argv[ optind ], "-m") == 0) {
			cola_encolar(&cola, netpbm_voltear_horizontal);

		} else if (strcmp(argv[ optind ], "-v") == 0) {
			cola_encolar(&cola, netpbm_voltear_vertical);

		} else if (strcmp(argv[ optind ], "-D") == 0) {
			cola_encolar(&cola, netpbm_multiplicar);

		} else if (strcmp(argv[ optind ], "-d") == 0) {
			cola_encolar(&cola, netpbm_dividir);

		} else if (strcmp(argv[ optind ], "-r") == 0) {
			if ((optind + 1) >= argc || is_arg(argv[optind + 1])) {
				netpbm_exit(SUB_REQ, argv[optind]);
			} else if (*argv[optind + 1] == 'i') {
				optind++;
				cola_encolar(&cola, netpbm_rotar_izquierda);
			} else if (*argv[optind + 1] == 'd') {
				optind++;
				cola_encolar(&cola, netpbm_rotar_derecha);
			} else {
				netpbm_exit(SUB_INV, argv[optind]);
			}
		} else if (strcmp(argv[ optind ], "-I") == 0) {
			cola_encolar(&cola, netpbm_invertir);

		} else if (strcmp(argv[ optind ], "-b") == 0) {
			cola_encolar(&cola, netpbm_desenfocar);

		} else if (strcmp(argv[ optind ], "-s") == 0) {
			if (fileOut) {
				netpbm_exit(ARG_REP, argv[optind]);
			} else if ((optind + 1) >= argc || is_arg(argv[optind + 1])) {
				netpbm_exit(SUB_REQ, argv[optind]);
			} else {
				optind++;
				fileOut = argv[optind];
			}
		} else {
			netpbm_exit(ARG_INV, argv[optind]);
		}
	}

	/* Si en cualquier argumento apareció -h muestro la ayuda unicamente */
	if (showHelp) {
		usage();
		netpbm_exit(EXIT_SUCCESS);
	}

	/* Verifico que haya un archivo de entrada */
	if (!fileIn) {
		netpbm_exit(ARG_REQ, "-e");
	}
	
	/* Si hay alguna operación es necesario un archivo de salida */
	if (!cola_vacia(cola) && !fileOut) {
		netpbm_exit(ARG_REQ, "-s");
	}

	/* Si no se puede leer el archivo de entrada termino todo */
	if (!netpbm_set_file(&img, fileIn)) {
		netpbm_exit(ERR_FIN);
	}

	/* Si fue solicitada la información, la muestro y cierro el programa */
	if (showInfo) {
		netpbm_info(&img);
		netpbm_exit(EXIT_SUCCESS);
	}
	
	/* Si no hay operaciones */
	if(cola_vacia(cola)){
		netpbm_exit(EXIT_SUCCESS);
	}

	/* Antes de aplicar las operaciones verifico si se puede crear 
	 * el archivo de salida */
	fp = fopen(fileOut, "wb+");
	if (!fp) {
		netpbm_exit(ERR_OUT);
	}
	
	/* Aplico los cambios encolados */
	Operacion fn;
	while (!cola_vacia(cola)) {
		fn = cola_desencolar(&cola);
		fn(&img);
	}
	
	/* Guardo el resultado y si no puedo (No hay espacio) cierro el programa */
	if (!netpbm_dump_file(&img, fp)) {
		remove(fileOut);
		netpbm_exit(ERR_OUT);
	}

	/* Ya en este punto del programa todo está ok. */
	return EXIT_SUCCESS;
}
