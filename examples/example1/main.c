#include <stdlib.h>
#include <stdio.h>
#include <stdlib.h>
#include <limits.h>
#include <float.h>
#include <arpa/inet.h>
#include <tpc.h>
#include <apf.h>
#include <fim.h>
#include <dat.h>
#include <exam1.h>

/* ----------------------------------------------------------
 *
 *   EXAMPLE1.
 *
 *   Simple BBDD with 1 Table with a key and a field.
 *
 *   Just, open, insert, delete, modify records and close.
 * ---------------------------------------------------------- */

int main()
{
int error;
int i;

TypeTABLE oneRegister = {0,"TEXT"};

/* ------------------------
 * Abrir la BBDD, let file open.
 * --------------------------------- */
EXAM1_Open(1);

/* ------------------------
 * Inserting several records
 * ------------------------------- */
printf("Inserting 10 records\n");
for (i=0;i!=10;i++)
{
 oneRegister.mykey = i;
 if ((error = EXAM1_InsTable(&oneRegister)) <0)
  printf("Error Insert register %d \n",i);
 else
  printf("Register %d inserted  \n",i);
}

printf ("Num of records %d \n",EXAM1_NumRecords(TBL_TABLE));
printf("Deleting 5 records\n");
/* ------------------------
 * Deleting several records
 * ------------------------------- */
for (i=5;i!=10;i++)
{
 oneRegister.mykey = i;
 if ((error = EXAM1_DelTable(&oneRegister)) <0)
  printf("Error Deleting register %d \n",i);
 else
  printf("Register %d delete  \n",i);
}
printf ("Num of records %d \n",EXAM1_NumRecords(TBL_TABLE));

printf ("Change of 5 records  \n");
/* ------------------------
 * Changing several records
 * ------------------------------- */
for (i=0;i!=5;i++)
{
 oneRegister.mykey = i;
 sprintf(oneRegister.text,"TEXT-%d",i);
 if ((error = EXAM1_ModTable(&oneRegister)) <0)
  printf("Error Modifing register %d \n",i);
 else
  printf("Register %d modify  \n",i);
}

printf ("Num of records %d \n",EXAM1_NumRecords(TBL_TABLE));
/* ------------------------
 * Closing BBDD.
 * ------------------------------- */
EXAM1_Close();
return 0;
}
