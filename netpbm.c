#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <math.h>

#include "netpbm.h"
#include "errors.h"

void netpbm_create(Netpbm *img) {
	img->data = NULL;
	img->width = 0;
	img->height = 0;
}

void netpbm_copy(Netpbm *dest, Netpbm *src) {
	strcpy( dest->magic, src->magic );
	dest->width = src->width;
	dest->height = src->height;
	dest->bpp = src->bpp;
	dest->maxval = src->maxval;
	long int size = src->width * src->height * src->bpp;
	dest->data = realloc(dest->data, size);
	memcpy( dest->data, src->data, size);
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
	extern char *comment;
	if (!fp) return 0;

	fprintf(fp, "%s\n", img->magic);
	fprintf(fp, "#%s\n", comment);
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