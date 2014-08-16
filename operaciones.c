#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <math.h>

#include "netpbm.h"
#include "errors.h"

// ETAPA 1

void netpbm_inv(Netpbm *img, Cola_gen *args) {
	int i, f;
	for (i = 0; i < img->height; i++) {
		for (f = 0; f < img->width; f++) {
			netpbm_invertir_pixel(img, i, f);
		}
	}
}

void netpbm_volt_h(Netpbm *img, Cola_gen *args) {
	int i, f, k, mid = img->height / 2;
	for (i = 0, k = (img->height - 1); i < mid; i++, k--) {
		for (f = 0; f < img->width; f++) {
			netpbm_swap_pixel(img, i, f, k, f);
		}
	}
}

void netpbm_volt_v(Netpbm *img, Cola_gen *args) {
	int i, f, k, mid = img->width / 2;
	for (i = 0; i < img->height; i++) {
		for (f = 0, k = (img->width - 1); f < mid; f++, k--) {
			netpbm_swap_pixel(img, i, f, i, k);
		}
	}
}

// ETAPA 2

void netpbm_mult(Netpbm *img, Cola_gen *args) {
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

void netpbm_div(Netpbm *img, Cola_gen *args) {
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

void netpbm_rot_i(Netpbm *img, Cola_gen *args) {
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

void netpbm_rot_d(Netpbm *img, Cola_gen *args) {
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

void netpbm_blur(Netpbm *img, Cola_gen *args) {
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
			for (c = 0; c < img->bpp; c++) {
				px[c] = sum[c] / cant;
			}
		}
	}
}

// ETAPA 3 - ADICIONALES - TESTEOS

void netpbm_masc(Netpbm *img , Cola_gen *args){
	extern Netpbm masc;
	extern Netpbm super;
	
	if( strcmp(masc.magic,"P5") != 0 ){
		netpbm_exit(MASC_ERR);
	}
	
	if( img->width != masc.width || masc.width != super.width ||
		img->height != masc.height || masc.height != super.height ){
		netpbm_exit(MASC_SIZE);
	}
	
	int i, j, k;
	unsigned char *orig_px, *masc_px, *super_px;
	float masc_v;
	
	for( i = 0 ; i < img->height ; i++ ){
		for( j = 0 ; j < img->width ; j++ ){
			orig_px = netpbm_get_pixel(img,i,j);
			masc_px = netpbm_get_pixel(&masc,i,j);
			super_px = netpbm_get_pixel(&super,i,j);
			masc_v = (float) *masc_px / (float) masc.maxval;
			for( k = 0; k < img->bpp ; k++ ){
				orig_px[k] = orig_px[k] * (1 - masc_v) + super_px[k] * masc_v;
			}
		}
	}
}

void netpbm_rdeg(Netpbm *img , Cola_gen *args){}

void netpbm_kern(Netpbm *img , Cola_gen *args){}

void netpbm_hist(Netpbm *img , Cola_gen *args){}