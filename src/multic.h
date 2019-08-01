//========================================================================
//        author   : xiaomh                               
//        email    : masw@masw.tech     
//        creattime: 7/27 13:31:04 2019
//========================================================================

#ifndef __MULTIC_H__
#define __MULTIC_H__

#include<stdlib.h>
#include<unistd.h>
#include<stdio.h>
#include<stdint.h>
#include<stdlib.h>
#include<string.h>
#include<math.h>
#include<strings.h>
#include <fcntl.h> // for open


#define CONST_LEN (64)
#define PRECI_LEN (127)


typedef struct _SYSTEM_CFG_
{
	double *real_const     ;//[CONST_LEN ]//input value, which const data you want to multi
	double *imag_const     ;//[CONST_LEN ]//input value, which const data you want to multi
	int     rnum_const     ;//num of const
	int     inum_const     ;//num of const
	char    preci           ;//打印数字宽度后的preci bit

	char **rdat;//[CONST_LEN ][PRECI_LEN]//
	char **rsym;//[CONST_LEN ][PRECI_LEN]//
	unsigned char *rnum;//[CONST_LEN ][PRECI_LEN]//
	char **idat;//[CONST_LEN ][PRECI_LEN]//
	char **isym;//[CONST_LEN ][PRECI_LEN]//
	unsigned char *inum;//[CONST_LEN ][PRECI_LEN]//


	char  wi_sym;//输入符号位位宽,默认1
	char  wi_int;//
	char  wi_flo;
	char  wo_sym;
	char  wo_int;
	char  wo_flo;

	char *wi_symbol;//[128]//using symbol for data width. 
	char *wo_symbol;//[128]//using symbol for data width.
	char *out_filename;//[128]//the output file name 
}SysSet;



int malloc_free(SysSet *set);
int malloc_mem2set(SysSet *set);
int getopt_menu(int argc, char **argv, SysSet *set);
int closeto_core(SysSet *set); 
void out_display(SysSet *set);


#endif //__MULTIC_H__


//=======================================================================

