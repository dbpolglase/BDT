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
#include <exam3.h>

/* ----------------------------------------------------------
 *
 *   EXAMPLE3.
 *
 *   BBDD with 3 Tables with a key and a field.
 *
 *   Just, open, insert records on table1 and 
 *   recover and copy data for table1 to table2 and table3.
 * ---------------------------------------------------------- */

int main()
{
int error;
int i;
int nroRecordsVol;

TypeTABLE1 table1Register = {0,"TEXT"};
TypeTABLE1 * table1Records;
TypeTABLE2 table2Register;
TypeTABLE3 table3Register;

/* ------------------------
 * Abrir la BBDD, let file open.
 * --------------------------------- */
EXAM3_Open(1);

/* ------------------------
 * Inserting several records
 * ------------------------------- */
printf("Inserting 10 records\n");
for (i=0;i!=10;i++)
{
 table1Register.mykey1 = i;
 if ((error = EXAM3_InsTable1(&table1Register)) <0)
  printf("Error Insert register %d \n",i);
 else
  printf("Register %d inserted  \n",i);
}

printf ("Num of records in table1 %d \n",EXAM3_NumRecords(TBL_TABLE1));

/* -----------------
 * Dump of Table1 
 * ------------------- */
nroRecordsVol = EXAM3_VolTable1(KEY_SEQUENCE_TABLE1,&table1Records);
if (nroRecordsVol < 0)
{
  printf("Error getting Table1 records\n");
}
else
{
  printf("getting Table1 records OK\n");
}

/* -----------------
 * Inserting pair record on table2 and odd records on table3.
 * ------------------------------------------------------------ */
for (i=0;i<nroRecordsVol;i++)
{
  if (table1Records[i].mykey1%2 == 0)
  {
    table2Register.mykey2 = table1Records[i].mykey1;
    strcpy(table2Register.text,table1Records[i].text);
    if ((error = EXAM3_InsTable2(&table2Register)) <0)
     printf("Error Insert register %d \n",i);
    else
     printf("Register %d inserted  \n",i);
  }
  else
  {
    table3Register.mykey3 = table1Records[i].mykey1;
    strcpy(table3Register.text,table1Records[i].text);
    if ((error = EXAM3_InsTable3(&table3Register)) <0)
     printf("Error Insert register %d \n",i);
    else
     printf("Register %d inserted  \n",i);
  }
}
/* ------------------------
 * !! Don't forget
 * Free memory  id records != 0
 *
 * ------------------------------- */
if (nroRecordsVol > 0) free(table1Records);


/* ------------------------
 * Closing BBDD.
 * ------------------------------- */
EXAM3_Close();
return 0;
}
