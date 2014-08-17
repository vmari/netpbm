#ifndef OPERACIONES_H
#define OPERACIONES_H

#include "netpbm.h"
#include "cola_gen.h"

#define HISTOGRAM_HEIGHT 120

#define R_COLOR 1 << 0
#define G_COLOR 1 << 1
#define B_COLOR 1 << 2

typedef void (*Operacion)(Netpbm *img, Cola_gen *args);

void netpbm_inv			(Netpbm *img, Cola_gen *args);

void netpbm_volt_h		(Netpbm *img, Cola_gen *args);

void netpbm_volt_v		(Netpbm *img, Cola_gen *args);

void netpbm_mult		(Netpbm *img, Cola_gen *args);

void netpbm_div			(Netpbm *img, Cola_gen *args);

void netpbm_rot_i		(Netpbm *img, Cola_gen *args);

void netpbm_rot_d		(Netpbm *img, Cola_gen *args);

void netpbm_blur		(Netpbm *img, Cola_gen *args);

void netpbm_masc		(Netpbm *img, Cola_gen *args);

void netpbm_rdeg		(Netpbm *img, Cola_gen *args);

void netpbm_kern		(Netpbm *img, Cola_gen *args);

void netpbm_hist		(Netpbm *img, Cola_gen *args);

#endif