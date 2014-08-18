#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <math.h>
#include <regex.h>

#include "netpbm.h"
#include "errors.h"
#include "cola_gen.h"
#include "operaciones.h"


// FUNCIONES PRIVADAS

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

float degtorad(float rad) {
    return rad*(180.0/M_PI);
}


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
	char *fmasc;
	char *fsuper;
	
	cola_desencolar(args, &fmasc, char*);
	cola_desencolar(args, &fsuper, char*);	
	
	Netpbm masc;
	Netpbm super;
	
	if( !netpbm_set_file(&masc, fmasc) ){
		netpbm_exit(SUB_INV,fmasc);
	}
	
	if( !netpbm_set_file(&super, fsuper) ){
		netpbm_exit(SUB_INV,fsuper);
	}
	
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

void netpbm_rdeg(Netpbm *img , Cola_gen *args){
	int degrees;
	cola_desencolar(args, &degrees, int);
	
	if( degrees == 90 ){
		netpbm_rot_d(img,args);
	}else if( degrees == -90 ){
		netpbm_rot_i(img,args);
	}else{
		float rad = degtorad(degrees);
		
		unsigned char *data = NULL;
		
		int new_width  = abs(cos(rad)*img->width+cos((M_PI/2)-rad)*img->height),
			new_height = abs(cos(rad)*img->height+cos((M_PI/2)-rad)*img->width);
			
		long size = new_width * new_height * img->bpp;
		data = malloc(size);
		if (!data) netpbm_exit(NO_MEM);
		
		int i, f, newi, newf;
		for (i = 0; i < img->height; i++) {
			for (f = 0; f < img->width; f++) {
				newi = sin((M_PI/2)-rad)*i+cos((M_PI/2)-rad)*f;
				newf = cos(rad)*f 
					- cos((M_PI/2)-rad)*i 
					+ cos((M_PI/2)-rad)*img->height;
				memcpy(
					data + newi * new_width * img->bpp + newf * img->bpp,
					netpbm_get_pixel(img,i,f),
					img->bpp);
			}
		}
		
		free(img->data);
		img->data = data;
		
		img->width = new_width;
		img->height = new_height;
	}
}

void netpbm_kern(Netpbm *img , Cola_gen *args){
	char *k;
	
	cola_desencolar(args, &k, char*);
	
	unsigned char *data = NULL;
	
	int kernel[9];
	int div;
	
	if( sscanf(k,"%d,%d,%d,%d,%d,%d,%d,%d,%d,%d", 
			&kernel[0], &kernel[1],	&kernel[2],
			&kernel[3], &kernel[4], &kernel[5],
			&kernel[6], &kernel[7],	&kernel[8], &div) != 10 )
			
			netpbm_exit(SUB_INV, k);
	
	size_t size = img->width * img->height * img->bpp;
	data = malloc(size);
	if (!data) netpbm_exit(NO_MEM);

	int i, f;
	int sum[img->bpp];
	int a, b, c, cant;
	for (i = 0; i < img->height; i++) {
		for (f = 0; f < img->width; f++) {
			unsigned char *px = data + i * img->width * img->bpp + f * img->bpp;
			cant = 0;
			for (c = 0; c < img->bpp; c++) sum[c] = 0;
			for (a = ((i)?-1:0); a <= ((img->height-i-1)?1:0); a++) {
				for (b = ((f)?-1:0); b <= ((img->width-f-1)?1:0); b++) {
					cant++;
					for (c = 0; c < img->bpp; c++) {
						sum[c] += (*(netpbm_get_pixel(img, i + a, f + b) + c)) *
									(*(kernel + 1*3 + 1 + a*3 + b ));
					}
				}
			}
			for (c = 0; c < img->bpp; c++) {
				sum[c] /= ((div)?cant:1);
				if(sum[c] > 255)
					px[c] = 255;
				else if(sum[c] < 0)
					px[c] = 0;
				else
					px[c] = sum[c];
			}
		}
	}
	free(img->data);
	img->data = data;
}

void netpbm_hist(Netpbm *img , Cola_gen *args){
	char *canal;
	cola_desencolar(args, &canal, char*);
	
	regex_t regex;
	int ret = regcomp(&regex, "^[rgb]{1,3}$", REG_EXTENDED | REG_NEWLINE);
	if(ret) netpbm_exit(CUSTOM,"No se pudo compilar la expresión regular");
	
	ret = regexec(&regex, canal, 0, NULL, 0);
	if (ret || ret == REG_NOMATCH){
		netpbm_exit(SUB_INV,canal);
	}
	regfree(&regex);
	
	unsigned char mask = 0;	
	if( index(canal,'r') ) mask |= R_COLOR;
	if( index(canal,'g') ) mask |= G_COLOR;
	if( index(canal,'b') ) mask |= B_COLOR;

	int ancho = img->maxval + 1;
	long int histogram[img->bpp][ancho];
	
	int i,f,k;
	for( i = 0 ; i < img->bpp ; i++ ){
		for( f = 0 ; f < ancho ; f++ ){
			histogram[i][f] = 0;
		}
	}
	
	long int pos,max = 0;
	
	for( i = 0 ; i < img->height ; i++ ){
		for( f = 0 ; f < img->width ; f++ ){
			for( k = 0 ; k < img->bpp ; k++ ){
				pos = *(netpbm_get_pixel(img,i,f) + ((img->bpp > 1)?k:0));
				histogram[k][pos]++;
				if( histogram[k][pos] > max ){
					max = histogram[k][pos];
				}
			}
		}
	}
	
	Netpbm hist;
	if( strcmp(img->magic, "P5") == 0 ){
		strcpy(hist.magic,"P5");
		hist.bpp = 1;
	}else{
		strcpy(hist.magic,"P6");
		hist.bpp = 3;
	}
	hist.width = ancho;
	hist.maxval = 255;
	hist.height = HISTOGRAM_HEIGHT;
	
	long int tam = hist.width * hist.height * hist.bpp; 
	
	hist.data = malloc( tam );
	
	
	for( i = 0 ; i < tam ; i++){
		hist.data[i] = 0x2D;
	}
	
	float alto;
	float factor = (float) HISTOGRAM_HEIGHT / (float) max;
	
	for( i = 0 ; i < hist.width ; i++ ){
		for( f = 0 ; f < img->bpp ; f++ ){
			if( (1 << f) & mask || hist.bpp == 1 ){
				alto = (float) histogram[f][i] * factor;
				for( k = (int) alto ; k >= 0 ; k-- ){
					*(netpbm_get_pixel(&hist,( hist.height - 1 - k ) ,i)+f) = 0xFF;
				}
			}
		}
	}

	netpbm_copy(img, &hist);
	
	netpbm_destroy(&hist);
}

void netpbm_crop(Netpbm *img , Cola_gen *args){
	char *param;
	cola_desencolar(args, &param, char*);
	
	
	int dims[4];	
	if( sscanf(param,"%d,%d,%d,%d", &dims[0], &dims[1], &dims[2], &dims[3]) != 4 )
		netpbm_exit(SUB_INV, param);
	
	Netpbm hist;
	strcpy(hist.magic,img->magic);
	hist.bpp = img->bpp;
	hist.width = abs(dims[2]-dims[0]);
	hist.height = abs(dims[3]-dims[1]);
	hist.maxval = img->maxval;
	
	long int tam = hist.width * hist.height * hist.bpp; 
	
	hist.data = malloc( tam );
	
	
	int i,f,j,k,l;
	for( i = dims[0], j = 0 ; i < dims[2] ; i++, j++ ){
		for( f = dims[1], k = 0 ; f < dims[3] ; f++, k++ ){
			for( l = 0 ; l < img->bpp ; l++ )
				*(netpbm_get_pixel(&hist,j,k)+l) = *(netpbm_get_pixel(img,i,f)+l);
		}
	}
	
	netpbm_copy(img, &hist);	
	netpbm_destroy(&hist);
}