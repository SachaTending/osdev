// Kernel features, these features can be a part of diffient projects, such as catsay

#include "printf.h"
#include "libc.h"

// Catsay, by krisvers, github: https://github.com/krisvers/catsay/
const char * cat[7] = {" ^__^\n","(,  ,)\n",">/7Y/<\n","(    )\n","| __ |\n","| || |\n","[_][_]\n"};

void printcat(unsigned char * vpos)
{
	for (unsigned char i = 0; i < sizeof(cat)/8; i++)
	{
		for (unsigned char it = 0; it < *vpos-2; it++)
		{
			printf(" ");
		}
		printf("%s", cat[i]);
	}
}


int catsay_main(const char *text)
{

	unsigned char vpos = 0;

	printf(" ");
	for (unsigned char i = 1; i <= strlen(text); i++)
	{
		printf("_");
	}
	printf(" \n(");
	printf("%s", text);
	printf(")\n(");
	for (unsigned char i = 1; i <= strlen(text); i++)
	{
		if (i != strlen(text)-1)
		{
			printf("_");
		}
		else {
			printf(" ");
		}
	}
	printf(")\n ");
	for (unsigned char i = 1; i <= strlen(text); i++)
	{
		if (i != strlen(text)-1 && strlen(text) > 1)
		{
			printf(" ");	
		}
		else {
			vpos = i;
			printf("v");
		}
	}
	printf("\n");
	printcat(&vpos);
	return 0;
}