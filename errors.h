#ifndef ERRORS_H
#define	ERRORS_H

#define ARG_REQ 1
#define SUB_REQ 2
#define ARG_REP 3
#define ARG_INV 4
#define ERR_FIN 5
#define ERR_OUT 6
#define ERR_INF 7
#define ERR_OPF 8
#define SUB_INV 9
#define NO_MEM 10
#define ARG_PRI 11
#define SUBS_REQ 12
#define MASC_SIZE 13
#define MASC_ERR 14

void netpbm_exit(int err, ...);

#endif