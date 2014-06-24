#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <math.h>

#include "netpbm.h"
#include "errors.h"

unsigned char *netpbm_get_pixel(Netpbm *img, int i, int f) {
	return img->data + i * img->width * img->bpp + f * img->bpp;
}

void netpbm_invertir_pixel(Netpbm *img, int i, int f) {
	unsigned char *px = netpbm_get_pixel(img, i, f);
	int k;
	for (k = 0; k < img->bpp; k++) px[k] = img->maxval - px[k];
}

void netpbm_swap_pixel(Netpbm *img, int ai, int af, int bi, int bf) {
	unsigned char *tmp = malloc(img->bpp);
	memcpy(
			tmp,
			netpbm_get_pixel(img, bi, bf),
			img->bpp);
	memcpy(
			netpbm_get_pixel(img, bi, bf),
			netpbm_get_pixel(img, ai, af),
			img->bpp);
	memcpy(
			netpbm_get_pixel(img, ai, af),
			tmp,
			img->bpp);
	free(tmp);
}

void netpbm_create(Netpbm *img) {
	img->data = NULL;
	img->width = 0;
	img->height = 0;
}

char netpbm_get_char(FILE *fp) {
	int ch = getc(fp);
	if (ch == EOF) netpbm_exit(ERR_FIN);
	if (ch == '#') {
		do {
			ch = getc(fp);
			if (ch == EOF) netpbm_exit(ERR_FIN);
		} while (ch != '\n' && ch != '\r');
	}
	return (char) ch;
}

unsigned int netpbm_get_next_int(FILE *fp) {
	unsigned char ch;
	do {
		ch = netpbm_get_char(fp);
	} while (isspace(ch));

	int i;
	i = 0;
	do {
		int dig = ch - '0';
		i = i * 10 + dig;
		ch = netpbm_get_char(fp);
	} while (isdigit(ch));

	return i;
}

void netpbm_get_magic_number(Netpbm *img, FILE *fp) {
	int i;
	for (i = 0; i < 3; i++) {
		img->magic[i] = netpbm_get_char(fp);
	}
	img->magic[2] = '\0';
}

int netpbm_set_file(Netpbm *img, char *fileIn) {
	FILE *fp = NULL;
	fp = fopen(fileIn, "rb");

	if (!fp) return 0;

	netpbm_get_magic_number(img, fp);

	if (strcmp(img->magic, "P5") == 0) {
		img->bpp = 1;
	} else if (strcmp(img->magic, "P6") == 0) {
		img->bpp = 3;
	} else netpbm_exit(ERR_INF);

	img->width = netpbm_get_next_int(fp);
	img->height = netpbm_get_next_int(fp);
	img->maxval = netpbm_get_next_int(fp);

	long size = img->width * img->height * img->bpp;
	img->data = malloc(size);
	if (!img->data) netpbm_exit(NO_MEM);

	long readed = fread(img->data, 1, size, fp);
	if (size != readed) netpbm_exit(ERR_FIN);

	fclose(fp);
	return 1;
}

int netpbm_dump_file(Netpbm *img, FILE *fp) {
	if (!fp) return 0;

	fprintf(fp, "%s\n", img->magic);
	fprintf(fp, "# Hi there!\n");
	fprintf(fp, "%d %d\n", img->width, img->height);
	fprintf(fp, "%d\n", img->maxval);

	long size = img->width * img->height * img->bpp;
	long writed = fwrite(img->data, 1, size, fp);
	
	fclose(fp);
	
	return (size == writed);
}

void netpbm_info(Netpbm *img) {
	printf("Formato: %s\n", img->magic);
	printf("Tamaño : %dx%d\n", img->width, img->height);
	printf("MaxVal : %d\n", img->maxval);
}

void netpbm_destroy(Netpbm *img) {
	free(img->data);
}

/* Comienzo de operaciones */
// ETAPA 1

void netpbm_invertir(Netpbm *img) {
	int i, f;
	for (i = 0; i < img->height; i++) {
		for (f = 0; f < img->width; f++) {
			netpbm_invertir_pixel(img, i, f);
		}
	}
}

void netpbm_voltear_horizontal(Netpbm *img) {
	int i, f, k, mid = img->height / 2;
	for (i = 0, k = (img->height - 1); i < mid; i++, k--) {
		for (f = 0; f < img->width; f++) {
			netpbm_swap_pixel(img, i, f, k, f);
		}
	}
}

void netpbm_voltear_vertical(Netpbm *img) {
	int i, f, k, mid = img->width / 2;
	for (i = 0; i < img->height; i++) {
		for (f = 0, k = (img->width - 1); f < mid; f++, k--) {
			netpbm_swap_pixel(img, i, f, i, k);
		}
	}
}

// ETAPA 2

void netpbm_multiplicar(Netpbm *img) {
	int new_width = img->width * 2;
	int new_height = img->height * 2;

	unsigned char *data = NULL;
	long size = new_width * new_height * img->bpp;
	data = malloc(size);
	if (!data) netpbm_exit(NO_MEM);

	int i, f, ni, nf;
	for (ni = 0, i = 0; ni < new_height; ni++) {
		if (!(ni % 2)) i++;
		for (nf = 0, f = 0; nf < new_width; nf++) {
			if (!(nf % 2)) f++;
			memcpy(
					data + ni * new_width * img->bpp + nf * img->bpp,
					netpbm_get_pixel(img, i-1, f-1),
					img->bpp);
		}
	}

	free(img->data);
	img->data = data;

	img->width *= 2;
	img->height *= 2;
}

void netpbm_dividir(Netpbm *img) {
	int new_width = img->width / 2;
	int new_height = img->height / 2;

	unsigned char *data = NULL;
	long size = new_width * new_height * img->bpp;
	data = malloc(size);
	if (!data) netpbm_exit(NO_MEM);

	int i, f, ni, nf;
	for (ni = 0, i = 0; ni < new_height; ni++, i += 2) {
		for (nf = 0, f = 0; nf < new_width; nf++, f += 2) {
			memcpy(
					data + ni * new_width * img->bpp + nf * img->bpp,
					netpbm_get_pixel(img, i, f),
					img->bpp);
		}
	}

	free(img->data);
	img->data = data;

	img->width /= 2;
	img->height /= 2;
}

void netpbm_rotar_izquierda(Netpbm *img) {
	unsigned char *data = NULL;
	long size = img->width * img->height * img->bpp;
	data = malloc(size);
	if (!data) netpbm_exit(NO_MEM);

	int ai, af, bf;
	for (ai = 0; ai < img->height; ai++) {
		for (af = 0, bf = (img->width - 1); af < img->width; af++, bf--) {
			memcpy(
					data + bf * img->height * img->bpp + ai * img->bpp,
					netpbm_get_pixel(img, ai, af),
					img->bpp);
		}
	}

	free(img->data);
	img->data = data;

	int tmp = img->width;
	img->width = img->height;
	img->height = tmp;
}

void netpbm_rotar_derecha(Netpbm *img) {
	unsigned char *data = NULL;
	long size = img->width * img->height * img->bpp;
	data = malloc(size);
	if (!data) netpbm_exit(NO_MEM);

	int ai, af, bi;
	for (ai = 0, bi = (img->height - 1); ai < img->height; ai++, bi--) {
		for (af = 0; af < img->width; af++) {
			memcpy(
					data + af * img->height * img->bpp + bi * img->bpp,
					netpbm_get_pixel(img, ai, af),
					img->bpp);
		}
	}

	free(img->data);
	img->data = data;

	int tmp = img->width;
	img->width = img->height;
	img->height = tmp;
}

void netpbm_desenfocar(Netpbm *img) {
	int i, f;
	unsigned int sum[img->bpp];
	int a, b;
	unsigned int c, cant;
	for (i = 0; i < img->height; i++) {
		for (f = 0; f < img->width; f++) {
			unsigned char *px = netpbm_get_pixel(img, i, f);
			cant = 0;
			for (c = 0; c < img->bpp; c++)
				sum[c] = 0;
			
			for (a = ((i)?-1:0); a <= ((img->height-i-1)?1:0); a++) {
				for (b = ((f)?-1:0); b <= ((img->width-f-1)?1:0); b++) {
					cant++;
					for (c = 0; c < img->bpp; c++) {
						sum[c] += (*(netpbm_get_pixel(img, i + a, f + b) + c));
					}
				}
			}
			if(cant){
				for (c = 0; c < img->bpp; c++) {
					px[c] = sum[c] / cant;
				}
			}
		}
	}
}