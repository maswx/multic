//========================================================================
//        author   : xiaomh                               
//        email    : masw@masw.tech     
//        creattime: 7/27 13:32:19 2019
//========================================================================

#include "multic.h"

unsigned char c2symdat(double c, char * dsym, char *dexp)                                                                                                                                                           
{
    int i,j;
    unsigned long *c_addr;
    int c_exp;
    char c_sym;//1负数，0正数
    unsigned long c_lng;//c_long
    unsigned char lonum;//local num
    // begin 
    lonum = 0;
    c_addr = (unsigned long *)&c;
	if(*c_addr == 0)
		return 0;
    c_exp = (*c_addr>>52)&0x7ff;//取指数部分
    c_exp -= 1023;//
    c_sym = (*c_addr>>63)&1;
    c_lng = (*c_addr | 0x0010000000000000 ) & 0x001fffffffffffff ;//去掉指数位符号位
    for(i = 52; i > 0; i--)
    {   
        if( (c_lng >> i) & 1)
        {   
            for(j = 1; j < i+1; j++)//查看有多少个连续1
                if( ! ((c_lng >> (i-j))&1) )
                    break;//连续1的个数就是 j-1
            if(j > 2)//连续有连续3个1.此时可以采用减法。以减小迭代此次数
            {   
                dsym[lonum] = c_sym;
                dexp[lonum++] = c_exp + 1;

                if(i != j-1){
                    dsym[lonum] = (~c_sym) & 1;
                    dexp[lonum++] = c_exp + 1 - j;
                }   
                c_exp -= j-1;
                i -= j-1 ;
            }   
            else
            {   
                dsym[lonum] = c_sym;
                dexp[lonum++] = c_exp;
            }   
        }   
        c_exp --; 
    }   
    return lonum;
}
int closeto_core(SysSet *set)
{
	int i;
	for(i = 0; i < set -> rnum_const; i ++)
		set -> rnum[i] = c2symdat(set -> real_const[i], set -> rsym[i], set -> rdat[i]);
	for(i = 0; i < set -> inum_const; i ++)
		set -> inum[i] = c2symdat(set -> imag_const[i], set -> isym[i], set -> idat[i]);
	return 0;
}



void SysSetInfo(SysSet *set)
{
	int i,j;
	printf("set -> rnum_const   = %d\n", set ->  rnum_const    ) ;//num of const
	printf("set -> preci        = %d\n", set -> preci        ) ;//cons precision.
	for( i = 0; i < set -> rnum_const; i++)
	{
		printf("set -> real_const[%d]= %f\n", i, set -> real_const[i]   ) ;//[CONST_LEN ]//input value, which const data you want to multi
		printf("set -> imag_const[%d]= %f\n", i, set -> imag_const[i]   ) ;//[CONST_LEN ]//input value, which const data you want to multi
		for( j = 0; j < 4; j++)
		{
			printf("set -> rdat[%d][%d]    = %d\n", i, j,  set -> rdat[i][j]          ) ;//[CONST_LEN ][PRECI_LEN]//0对应-63次方，126对应+63次方，参数1代表有，参数0代表没有。
			printf("set -> rsym[%d][%d]    = %d\n", i, j,  set -> rsym[i][j]          ) ;//[CONST_LEN ][PRECI_LEN]//0对应-63次方，126对应+63次方，参数1代表+1，参数0代表-1。
		}
	}
	printf("set -> wi_symbol       = %s\n", set -> wi_symbol       ) ;//[128]//using symbol for data width. 
	printf("set -> wo_symbol       = %s\n", set -> wo_symbol       ) ;//[128]//using symbol for data width.
	printf("set -> out_filename = %s\n", set -> out_filename ) ;//[128]//the output file name 
	
}
void out_display(SysSet *set)
{
	int i,j, num;
	if( set -> rnum_const > set -> inum_const)
		num = set -> rnum_const;
	else
		num = set -> inum_const;
	//打印数字,如果只有实数
	if(set -> inum_const == 0)
	{
		for(i = 0; i < num; i ++)
		{
			printf("\nconst[%d] = %lf = ",i, set -> real_const[i]);
			for(j = 0; j < set ->rnum[i]; j++)
				if(set -> rsym[i][j])
					printf(" - 2^%d", set -> rdat[i][j]);
				else 
					printf(" + 2^%d", set -> rdat[i][j]);
		}
		printf("\n");
	}
	else//这里包含了虚数
		for(i = 0; i < num; i ++)
		{
			printf("\nconst[%d]_real = %lf = ",i, set -> real_const[i]);
			for(j = 0; j < set ->rnum[i]; j++)
				if(set -> rsym[i][j])
					printf(" - 2^%d", set -> rdat[i][j]);
				else 
					printf(" + 2^%d", set -> rdat[i][j]);
			printf("\nconst[%d]_imag = %lf = ",i, set -> imag_const[i]);
			for(j = 0; j < set ->inum[i]; j++)
				if(set -> isym[i][j])
					printf(" - 2^%d", set -> idat[i][j]);
				else 
					printf(" + 2^%d", set -> idat[i][j]);
			printf("\n");
		}
	//打印方程 	
	//	
	char wo, wi, bnu;
	wo = set -> wo_sym + set -> wo_int + set -> wo_flo; 
	wi = set -> wi_sym + set -> wi_int + set -> wi_flo; 
	bnu = set -> wo_sym + set -> wo_int - set -> wi_sym - set -> wi_int;
	printf("wire [%d-1:0]OUT = ",wo);
	printf("//(%d+%d+%d)->(%d+%d+%d)//multic %lf.\n",set -> wi_sym , set -> wi_int , set -> wi_flo, set -> wo_sym , set -> wo_int , set -> wo_flo, set -> real_const[0]);
	for(i = 0; i < set -> rnum_const; i ++)
	{
		if(bnu - set -> rdat[i][0]<0)
			printf("                  //error: data overflow\n");
		for(j = 0; j < set ->rnum[i]; j++)
		{
			if(set -> rsym[i][j])//如果是-
				if(set -> rdat[i][j] <= set->wi_flo - set->wo_flo)
				{
					if(wo-bnu+set -> rdat[i][j] > 0)
				printf("                 - {{%2d{IN%d[%d-1]}}, IN%d[%d-1:%2d]       }//-2^%d\n", bnu - set -> rdat[i][j], i, wi, i, wi, wi-(wo-bnu+set -> rdat[i][j]), set -> rdat[i][j]);
				}
				else 
				printf("                 - {{%2d{IN%d[%d-1]}}, IN%d[%d-1: 0], % 2d'd0}//-2^%d\n", bnu - set -> rdat[i][j], i, wi, i, wi, wo-wi-(bnu - set -> rdat[i][j]), set -> rdat[i][j]);
			else 
				if(set -> rdat[i][j] <= set->wi_flo - set->wo_flo)//如果是右移且超过了位宽
				{
					if(wo-bnu+set -> rdat[i][j] > 0)
				printf("                 + {{%2d{IN%d[%d-1]}}, IN%d[%d-1:%2d]       }//+2^%d\n", bnu - set -> rdat[i][j], i, wi, i, wi, wi-(wo-bnu+set -> rdat[i][j]), set -> rdat[i][j]);
				}
				else 
				printf("                 + {{%2d{IN%d[%d-1]}}, IN%d[%d-1: 0], %2d'd0}//+2^%d\n", bnu - set -> rdat[i][j], i, wi, i, wi, wo-wi-(bnu - set -> rdat[i][j]), set -> rdat[i][j]);

		}
	}
			printf("                 ;\n");
	printf("\n");
}





int get_input_const(SysSet *set, char *datstr)
{
	int i,j;
	double dat;
	int rnumcnt, inumcnt;
	unsigned int datstr_len;
	char * strt;
	char * str0;
	datstr_len = (unsigned int) strlen(datstr);
	if(datstr_len < 1)
		return -1;
	strt = (char *) malloc(sizeof(char) * datstr_len );
	str0 = (char *) malloc(sizeof(char) * datstr_len );
	if(strt == NULL || str0 == NULL)
		return -1;
	memcpy(str0, datstr, datstr_len);
	for(i = 0; i < datstr_len; i ++)// log by masw: will be change.
		if(str0[i] == ';' || 
		   str0[i] == ',' || 
		   str0[i] == '|' || 
		   str0[i] == '/' || 
		   str0[i] == '\\' || 
		   str0[i] == '[' || str0[i] == ']' || 
		   str0[i] == '{' || str0[i] == '}')
			str0[i] = ' ';
	memcpy(strt, datstr, datstr_len);
	rnumcnt = 0;
	inumcnt = 0;
	for (i = 0; i < datstr_len; i++)
	{
		dat = strtod(str0, &strt);
		if(( strt[0] == 'i' || strt[0] == 'j') && dat!=0)
		{
			strt[0] = ' ';
			set -> imag_const[inumcnt++] = dat;
		}
		else if(dat !=0)
			set -> real_const[rnumcnt++] = dat;
		else 
			strt[0] = ' ';
		//exit
		if(strlen(strt) == 1)
			break;
		//clean
		for(j = 0; j < strlen(strt); j++)
			str0[j] = strt[j];
		for(; j < datstr_len; j++)
			str0[j] = '\0';

	}
	set -> rnum_const = rnumcnt;
	set -> inum_const = inumcnt;
	return 0;
}


void printf_verinfo(void)
{
	printf("\nmultic version 0.1 . not support complex input yet.\n");
	printf("multic by masw@masw.tech. GPL v3 license.\n");
	printf("project link : https://github.com/maswell/multic.git\n\n");
}


void printf_usage(void)
{
	printf("\nmultic by masw@masw.tech. GPL v3 license.\n");
	printf("Example: \n");
	printf("multic                       #get help. the same as `multic -h` / `multic -help` / `multic --help`                                                               \n");	
	printf("multic -v                    #version info and current version feature. the same as `multic --version` / `multic -V` / `multic -Version` / `multic -version`     \n");	
	printf("multic 0.1678901             #multiplied by the constant 0.1678901.                                              \n");	
	printf("                                                                                                                                                                 \n");	
	printf("multic 0.12345678 -wi '1 1 8' -wo '1 4 10'                                                                                                                       \n");	
	printf("multic '0.12 -1.4'           # calculate  0.12*IN1 - 1.4*IN2 .                                                                                                   \n");	
	printf("\n\n");
}

int getopt_menu(int argc, char **argv, SysSet *set)
{
	int opt, ret,i,j,k;
	unsigned int width;
	char preci;
	char *pstr;
	extern char *optarg;
	extern int  optind, opterr, optopt;

	//init 
	set -> wi_sym = 1;
	set -> wi_int = 1;
	set -> wi_flo = 6;
	set -> wo_sym = 1;
	set -> wo_int = 1;
	set -> wo_flo = 8;
	strcpy(set -> wi_symbol, "WI") ;
	strcpy(set -> wo_symbol, "WO") ;
	set -> preci = 0;
	//开始获取外部参数 
	if(argc == 1)
		printf_usage();
	else if(argc == 2)
	{
		if( strcasecmp(argv[1], "-h") == 0 ||
		    strcasecmp(argv[1], "-H") == 0 ||
		    strcasecmp(argv[1], "-help") == 0 ||
		    strcasecmp(argv[1], "-Help") == 0 ||
		    strcasecmp(argv[1], "--help") == 0 ||
		    strcasecmp(argv[1], "--Help") == 0 )
		    printf_usage();
		else if(strcasecmp(argv[1], "-v") == 0 ||
		        strcasecmp(argv[1], "-V") == 0 ||
		        strcasecmp(argv[1], "-version") == 0 ||
		        strcasecmp(argv[1], "-Version") == 0 ||
		        strcasecmp(argv[1], "--version") == 0 ||
		        strcasecmp(argv[1], "--Version") == 0 )
		{
			printf_verinfo();
		}
		else
		{
			ret = get_input_const(set, argv[1]);//real part input as default
			if(ret < 0)
				return ret;
		}
	}
	else
	{
		ret = get_input_const(set, argv[1]);//real part input as default
		if(ret < 0)
			return ret;
		for(i = 2; i < argc; i++)
		{
			j = 0;
			if(strcasecmp(argv[i], "-wi") == 0)
				for(pstr=strtok(argv[i+1]," ");pstr;pstr=strtok(NULL," "))
				{
					switch (j++)
					{
						case  0: set -> wi_sym = atoi(pstr);break;
						case  1: set -> wi_int = atoi(pstr);break;
						default: set -> wi_flo = atoi(pstr);break;
					}
					//printf("wi-atoi(pstr) = %d\n", atoi(pstr));
				}
			else if(strcasecmp(argv[i], "-wo") == 0)
				for(pstr=strtok(argv[i+1]," ");pstr;pstr=strtok(NULL," "))
				{
					switch (j++)
					{
						case  0: set -> wo_sym = atoi(pstr);break;
						case  1: set -> wo_int = atoi(pstr);break;
						default: set -> wo_flo = atoi(pstr);break;
					}
					//printf("wo-atoi(pstr) = %d\n", atoi(pstr));
				}
			else if(strcasecmp(argv[i], "-f") == 0)
					strcpy(set -> out_filename, argv[i+1] ) ;
		}
		//====while((opt = getopt(argc, argv, "::hHvVc:f:w:p:")) != -1)
		//===={
		//====	switch(opt)
		//====	{
		//====		case 'p':
		//====			printf("pasdfasdfas\n");
		//====			set -> preci = (char) atoi(argv[optind-1]) ; 
		//====			break;
		//====		case 'w':
		//====			//width = (unsigned int) atoi(argv[optind-1]) ; 
		//====			printf("runing here 1");
		//====			if(strcasecmp(optarg , "i") == 0)
		//====			{
		//====				set -> wi_sym = atoi(argv[optind-1]);
		//====				set -> wi_int = atoi(argv[optind-1]+1);
		//====				set -> wi_flo = atoi(argv[optind-1]+3);
		//====			}
		//====			else if(strcasecmp(optarg , "o") == 0)
		//====			{
		//====				set -> wo_sym = atoi(argv[optind-1]);
		//====				set -> wo_int = atoi(argv[optind-1]+1);
		//====				set -> wo_flo = atoi(argv[optind-1]+3);
		//====			}
		//====			break;
		//====		case 'f':	
		//====			strcpy(set -> out_filename, argv[optind-1] ) ;
		//====			break;
		//====		case 'v':	
		//====		case 'V':	
		//====			printf_verinfo();
		//====			return 1;
		//====			break;
		//====		case 'h':	
		//====		case 'H':	
		//====			printf_usage();
		//====			return 1;
		//====			break;
		//====		default :
		//====			break;
		//====	} 
		//====} 
	}
	return 0;
}
int malloc_mem2set(SysSet *set)
{
	int ii,jj;
	set -> real_const = (double *) malloc(sizeof(double)*CONST_LEN);		
	set -> imag_const = (double *) malloc(sizeof(double)*CONST_LEN);		

	set -> rnum = (unsigned char  *) malloc(sizeof(unsigned char)*CONST_LEN);		
	set -> inum = (unsigned char  *) malloc(sizeof(unsigned char)*CONST_LEN);		
	set -> rdat = ( char **) malloc (sizeof( char *) * CONST_LEN);//
	set -> rsym = ( char **) malloc (sizeof( char *) * CONST_LEN);
	for(ii = 0; ii < CONST_LEN; ii++)
	{
		set -> rdat[ii] = ( char *) malloc(sizeof( char) * PRECI_LEN);
		set -> rsym[ii] = ( char *) malloc(sizeof( char) * PRECI_LEN);
	}
	set -> idat = ( char **) malloc (sizeof( char *) * CONST_LEN);//
	set -> isym = ( char **) malloc (sizeof( char *) * CONST_LEN);
	for(ii = 0; ii < CONST_LEN; ii++)
	{
		set -> idat[ii] = ( char *) malloc(sizeof( char) * PRECI_LEN);
		set -> isym[ii] = ( char *) malloc(sizeof( char) * PRECI_LEN);
	}
	set -> wi_symbol       = (char *) malloc(sizeof(char) * 128);		
	set -> wo_symbol       = (char *) malloc(sizeof(char) * 128);		
	set -> out_filename = (char *) malloc(sizeof(char) * 256);		

	if( set -> real_const    == NULL ||
        set -> imag_const    == NULL ||
        set -> rdat          == NULL || 
        set -> idat          == NULL || 
        set -> rsym          == NULL || 
        set -> isym          == NULL || 
        set -> wi_symbol        == NULL || 
        set -> wo_symbol        == NULL || 
        set -> out_filename  == NULL )
	{
		printf("error in malloc memery");
		return -1;
	}

	memset (set -> real_const  ,   0, sizeof(double) * CONST_LEN);
	memset (set -> imag_const  ,   0, sizeof(double) * CONST_LEN);

	memset (set -> wi_symbol      ,   0, sizeof(char)*128);
	memset (set -> wo_symbol      ,   0, sizeof(char)*128);
	memset (set -> out_filename,   0, sizeof(char)*256);

	for(ii = 0; ii < CONST_LEN; ii++)
	{
		for(jj = 0; jj < PRECI_LEN; jj++)
		{
			set -> rdat[ii][jj] = 0;
			set -> rsym[ii][jj] = 0;
			set -> idat[ii][jj] = 0;
			set -> isym[ii][jj] = 0;
		}
	}
	//printf("malloc memery ok !\n");
	return 0;
}
int malloc_free(SysSet *set)
{
	free(set -> real_const   );  
    free(set -> imag_const   ); 
    free(set -> rdat         );  
    free(set -> rsym         );  
    free(set -> rnum         );  
    free(set -> inum         );  
    free(set -> idat         );  
    free(set -> isym         );  
    free(set -> wi_symbol       );  
    free(set -> wo_symbol       );  
    free(set -> out_filename );
	return 0;
}






//========================================================================
