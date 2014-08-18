#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "netpbm.h"
#include "operaciones.h"
#include "errors.h"
#include "cola_gen.h"

#define FILE_COMMENT "Made in Argentina :)"

int cant_args = 15;
char *argumentos[] = { "h", "e", "i", "m", "v", "r", "D", "d", "I", "b", "s",
	"masc", "hist", "kern", "rdeg" };
	
char *comment = FILE_COMMENT;

int is_arg(char *cmp_arg) {
	int i;
	for( i = 0; i < cant_args ; i++ ){
		if( (strlen(cmp_arg) >= 2) && (cmp_arg[0] == '-') 
			&& ( strcmp(argumentos[i], (cmp_arg + 1) ) == 0 ) ){
			return 1;
		}
	}
	return 0;
}

void usage() {
	printf("\n"
	 "      -h               muestra una ayuda con todas estas opciones.\n"
	 "      -e <archivo>     nombre del archivo de entrada.\n"
	 "      -s <archivo>     nombre del archivo de salida.\n"
	 "      \n"
	 "      -i               mostrar información de la imágen.\n"
	 "      -m               voltear horizontalmente.\n"
	 "      -v               voltear verticalmente.\n"
	 "      -r (i|d)         rotar a izquierda o derecha respectivamente.\n"
	 "      -D               duplica el tamaño.\n"
	 "      -d               divide a la mitad el tamaño.\n"
	 "      -I               inviertir colores.\n"
	 "      -b               desenfocar\n"
	 "      -c               define el comentario del archivo de salida\n"
	 "      \n"
	 "      -masc <mascara> <superpuesta>  aplica la mascara\n"
	 "      -hist rgb                      genera histograma a <salida>\n"
	 "      -kern n,n,n,n,n,n,n,n,n,p      produce convulsion 3x3\n"
	 "      -rdeg <angulo>                 rota la imágen en grados\n");
}

Cola_gen 	cola_fn,
		cola_args;
Netpbm		img;
		
void onclose(){
	cola_destroy(&cola_fn);
	cola_destroy(&cola_args);
}

int main(int argc, char *argv[]) {
	Operacion fn;
	int i;
	
	cola_create(&cola_fn);
	cola_create(&cola_args);
	
	atexit(onclose);
	
	netpbm_create(&img);
	
	int 	showHelp = 0,
		showInfo = 0,
		optind = 1;

	FILE 	*pFileOut = NULL;

	char 	*fileIn = NULL,
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
			fn = netpbm_volt_h;
			cola_encolar(&cola_fn, &fn, Operacion);

		} else if (strcmp(argv[ optind ], "-v") == 0) {
			fn = netpbm_volt_v;
			cola_encolar(&cola_fn, &fn, Operacion);

		} else if (strcmp(argv[ optind ], "-D") == 0) {
			fn = netpbm_mult;
			cola_encolar(&cola_fn, &fn, Operacion);

		} else if (strcmp(argv[ optind ], "-d") == 0) {
			fn = netpbm_div;
			cola_encolar(&cola_fn, &fn, Operacion);

		} else if (strcmp(argv[ optind ], "-c") == 0) {
			if ((optind + 1) >= argc || is_arg(argv[optind + 1])) {
				netpbm_exit(SUB_REQ, argv[optind]);
			}else{
				optind++;
				comment = argv[optind];
			}
		} else if (strcmp(argv[ optind ], "-r") == 0) {
			if ((optind + 1) >= argc || is_arg(argv[optind + 1])) {
				netpbm_exit(SUB_REQ, argv[optind]);
			} else if (*argv[optind + 1] == 'i') {
				optind++;
				fn = netpbm_rot_i;
				cola_encolar(&cola_fn, &fn, Operacion);
			} else if (*argv[optind + 1] == 'd') {
				optind++;
				fn = netpbm_rot_d;
				cola_encolar(&cola_fn, &fn, Operacion);
			} else {
				netpbm_exit(SUB_INV, argv[optind]);
			}
		} else if (strcmp(argv[ optind ], "-I") == 0) {
			fn = netpbm_inv;
			cola_encolar(&cola_fn, &fn, Operacion);

		} else if (strcmp(argv[ optind ], "-b") == 0) {
			fn = netpbm_blur;
			cola_encolar(&cola_fn, &fn, Operacion);
			
		} else if (strcmp(argv[ optind ], "-s") == 0) {
			if (fileOut) {
				netpbm_exit(ARG_REP, argv[optind]);
			} else if ((optind + 1) >= argc || is_arg(argv[optind + 1])) {
				netpbm_exit(SUB_REQ, argv[optind]);
			} else {
				optind++;
				fileOut = argv[optind];
			}

		} else if (strcmp(argv[ optind ], "-masc") == 0) {
			if ((optind + 2) >= argc || is_arg(argv[optind + 1])
				|| is_arg(argv[optind + 2])) {
				netpbm_exit(SUBS_REQ, argv[optind]);
				
			} else {
				fn = netpbm_masc;
				cola_encolar(&cola_fn, &fn, Operacion);
				cola_encolar(&cola_args, &argv[++optind], char*);
				cola_encolar(&cola_args, &argv[++optind], char*);	
			}
			
		} else if (strcmp(argv[ optind ], "-hist") == 0) {
			if ((optind + 1) >= argc || is_arg(argv[optind + 1])) {
				netpbm_exit(SUB_REQ, argv[optind]);
			}else{
				fn = netpbm_hist;
				cola_encolar(&cola_fn, &fn, Operacion);
				cola_encolar(&cola_args, &argv[++optind], char*); //TODO: Canalizar
			}
			
		} else if (strcmp(argv[ optind ], "-kern") == 0) {
			if ((optind + 1) >= argc || is_arg(argv[optind + 1])) {
				netpbm_exit(SUB_REQ, argv[optind]);
				
			}else{
				fn = netpbm_kern;
				cola_encolar(&cola_fn, &fn, Operacion);
				cola_encolar(&cola_args, &argv[++optind], char*);
			}
			
		} else if (strcmp(argv[ optind ], "-rdeg") == 0) {
			if ((optind + 1) >= argc || is_arg(argv[optind + 1])) {
				netpbm_exit(SUB_REQ, argv[optind]);
			}else if( ( sscanf(argv[++optind],"%d",&i) != 1 )
				|| i >= 180 || i <= -180 ){
				netpbm_exit(SUB_INV, argv[optind]);
			}else{
				fn = netpbm_rdeg;
				cola_encolar(&cola_fn, &fn, Operacion);
				cola_encolar(&cola_args, &i, int);
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
	if (!cola_vacia(cola_fn) && !fileOut) {
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
	if (cola_vacia(cola_fn)) {
		netpbm_exit(EXIT_SUCCESS);
	}

	/* Antes de aplicar las operaciones verifico si se puede crear 
	 * el archivo de salida */
	pFileOut = fopen(fileOut, "wb+");
	
	if (!pFileOut) {
		netpbm_exit(ERR_OUT);
	}
	
	/* Aplico los cambios encolados */
	while (!cola_vacia(cola_fn)) {
		cola_desencolar(&cola_fn, &fn, Operacion);
		fn(&img, &cola_args);
	}
	
	/* Guardo el resultado y si no puedo (No hay espacio) cierro el prog. */
	if (!netpbm_dump_file(&img, pFileOut)) {
		remove(fileOut);
		netpbm_exit(ERR_OUT);
	}

	/* Ya en este punto del programa todo está ok. */
	return EXIT_SUCCESS;
}
