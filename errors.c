#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include "errors.h"

char *errores[] = {
	"",
	"Falta el argumento obligatorio %s.",
	"El argumento %s requiere un sub-argumento.",
	"Argumento %s repetido.",
	"Argumento %s no válido.",
	"No existe el archivo de entrada o no se puede leer",
	"No se puede crear el archivo de salida.",
	"Formato del archivo de entrada no válido o no soportado.",
	"El tipo o tamaño de la imágen %s no es compatible con la operación %s.",
	"Sub-argumento %s no válido.",
	"No hay memoria para alocar.",
	"El argumento -e debe estar primero",
	"El argumento %s requiere dos sub-argumentos.",
	"La mascara y la superpuesta deben ser de igual tamaño.",
	"Formato de mascara no válido o no soportado.",
	"%s"
};

void netpbm_exit(int err, ...) {
	if (!err) exit(EXIT_SUCCESS);
	va_list ap;
	va_start(ap, err);
	char *car = errores[err];
	for (; *car != '\0'; car++) {
		if (*car == '%' && *(car + 1) == 's') {
			fprintf(stderr, "%s", (char *) va_arg(ap, char *));
			car++;
		} else {
			fprintf(stderr, "%c", *car);
		}
	}
	va_end(ap);
	exit(err);
}