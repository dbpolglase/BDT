#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <limits.h>
#include <float.h>
#include <arpa/inet.h>
#include <tpc.h>
#include <apf.h>
#include <fim.h>
#include <dat.h>
#include <exam2.h>

/* --------------------------------------------------------------
 *  EXAMPLE2.
 *
 *  A local BBDD with 1 table with a field of each type: 
 *  char,short,unsigned short,int,unsigned int, float,
 *  long, unsigned long and array  and using max values.
 *
 *  Just open,insert,delete,modify and close.
 * -------------------------------------------------------------- */

int main()
{
int error;
int i;

/* ---------------
 * Register to use.
 * --------------------- */ 
TypeTABLE oneRegister = {0,'a',UCHAR_MAX,SHRT_MAX,USHRT_MAX,INT_MAX,UINT_MAX,LONG_MAX,ULONG_MAX,7.7,8.8,{1,2,3,4,5},{10,20,30,40,50},{100,200,300,400,500},{1000,2000,3000,4000,5000},{10000000,20000000,30000000,40000000,5000000},{1000000000,2000000000,300000000,400000000,5000000000},{10000.1,20000.1,30000.3,40000.1,50000.1},{1000000.1,2000000.1,3000000.2,4000000.3,5000000.4},"TEXT"};

TypeTABLE delRegister; 
TypeTABLE obtRegister; 

/* ---------------------------------
 * Open BBDD, let open false.
 * -------------------------------*/
EXAM2_Open(0);

/* ---------------------------------
 * Insert several registers.
 * -------------------------------*/
for (i=0;i!=10;i++)
{
 oneRegister.mykey = i;
 if ((error = EXAM2_InsTable(&oneRegister)) <0)
  printf("Error Insert register %d \n",i);
 else
  printf("Register %d inserted  \n",i);
}
/* ---------------------------------
 * Delete several registers.
 * -------------------------------*/
for (i=5;i!=10;i++)
{
 delRegister.mykey = i;
 if ((error = EXAM2_DelTable(&delRegister)) <0)
  printf("Error Deleting register %d \n",i);
 else
  printf("Register %d delete  \n",i);
}
/* ---------------------------------
 * Modify several registers.
 * -------------------------------*/
for (i=0;i!=5;i++)
{
 oneRegister.mykey = i;
 sprintf(oneRegister.achar,"TEXT-%d",i);
 if ((error = EXAM2_ModTable(&oneRegister)) <0)
  printf("Error Modify register %d \n",i);
 else
  printf("Register %d modify  \n",i);
}

/* ---------------------------------
 * Get last register  
 * -------------------------------*/
if (EXAM2_TableBySequence(&obtRegister,4))
  printf("Error getting last register inserted \n");
 else
  printf("Ok getting last register inserted \n");

/* ---------------------------------
 * Verify correct value
 * -------------------------------*/
if (memcmp(&oneRegister,&obtRegister,sizeof(TypeTABLE)) == 0)
  printf("obtained record correct\n"); 
else
  printf("obtained record wrong\n"); 


EXAM2_Close();
return 0;
}

