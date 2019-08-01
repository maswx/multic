//========================================================================
//        author   : xiaomh                               
//        email    : masw@masw.tech     
//        creattime: 7/26 13:31:37 2019
//========================================================================
#include "multic.h"
int main(int argc, char **argv)
{
	int ret;
	SysSet set;
	ret = malloc_mem2set(&set); 
	if(ret != 0)
		return ret;
	ret = getopt_menu(argc, argv, &set);
	if(ret != 0)// ret < 0 mean error. 0 means success . 1 means get help
		return ret;

	//// close to const
	closeto_core(&set); // close to const
	out_display(&set);
	malloc_free(&set);
	return 0;
} 
//========================================================================
