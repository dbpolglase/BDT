#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <limits.h>
#include <float.h>
#include <arpa/inet.h>
#include <tpc.h>
#include <apf.h>
#include <fim.h>
#include <dat.h>
#include <exam5.h>

/* ----------------------------------------------------------
 *
 *   EXAMPLE5.
 *
 *   BBDD with 3 Tables with a key and a field.
 *
 *   Just, open, insert records on table1 and 
 *   recover data from table1 by order of field pair 
 *   copy to table2 and table3.
 *   Finally 
 * ---------------------------------------------------------- */

int main()
{
int error;
int i;

TypeTABLE1 table1Register = {0,'p',"TEXT"};

/* ------------------------
 * Abrir la BBDD
 * --------------------------------- */
if (EXAM5_Open() < 0)
{
  EXAM5_CreateBase();
  if (EXAM5_Open() < 0)
  {
    printf("Can't open BBDD\n");
    exit(1) ;
  }
}

/* ------------------------
 * Inserting several records
 * ------------------------------- */
while (1)
{
for (i=0;i!=1000;i++)
{
 table1Register.mykey1 = i;
 if (i%2 == 0) table1Register.pair='p'; else table1Register.pair='o';
 if ((error = EXAM5_InsTable1(&table1Register)) <0)
  printf("Error Insert register %d \n",i);
 else
  printf("\033[2;1H Register %3d inserted  \n",i);
}

printf ("\033[4;1H Num of records now in table1 %d \n",EXAM5_NumRecords(TBL_TABLE1));
sleep(1);
}

/* ------------------------
 * Closing BBDD.
 * ------------------------------- */
EXAM5_Close();
return 0;
}
