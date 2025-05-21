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
int nroRecordsVol;
long total=0;

TypeTABLE1 table1Register = {0,'p',"TEXT"};
TypeTABLE1 * table1Records;

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

while (1)
{
/* -----------------
 * Dump of Table1 
 * ------------------- */
nroRecordsVol = EXAM5_VolTable1(KEY_PAIR_TABLE1,&table1Records);
if (nroRecordsVol < 0)
{
  printf("Error getting Table1 records\n");
}

/* -----------------
 * Delete records
 * ------------------------------------------------------------ */
for (i=0;i<nroRecordsVol;i++)
{
    table1Register.mykey1 = table1Records[i].mykey1;
    if ((error = EXAM5_DelTable1(&table1Register)) <0)
     printf("\033[15;1H Error delete register %d \n",i);
   total += nroRecordsVol;
}
     printf("\033[15;1H Delete now %d records total %ld \n",nroRecordsVol,total);
/* ------------------------
 * !! Don't forget
 * Free memory 
 *
 * ------------------------------- */
 if (nroRecordsVol > 0)
   free(table1Records);
}


/* ------------------------
 * Closing BBDD.
 * ------------------------------- */
EXAM5_Close();
return 0;
}
