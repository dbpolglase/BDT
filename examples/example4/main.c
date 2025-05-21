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
#include <exam4.h>

/* ----------------------------------------------------------
 *
 *   EXAMPLE4.
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

TypeTABLE1 table1Register = {0,'p',"TEXT"};
TypeTABLE1 * table1Records;
TypeTABLE2 table2Register;
TypeTABLE2 * table2Records;
TypeTABLE3 table3Register;
TypeTABLE3 * table3Records;

/* ------------------------
 * Abrir la BBDD, let file open.
 * --------------------------------- */
EXAM4_Open(1);

/* ------------------------
 * Inserting several records
 * ------------------------------- */
printf("Inserting 10 records\n");
for (i=0;i!=10;i++)
{
 table1Register.mykey1 = i;
 if (i%2 == 0) table1Register.pair='p'; else table1Register.pair='o';
 if ((error = EXAM4_InsTable1(&table1Register)) <0)
  printf("Error Insert register %d \n",i);
 else
  printf("Register %d inserted  \n",i);
}

printf ("Num of records in table1 %d \n",EXAM4_NumRecords(TBL_TABLE1));

/* -----------------
 * Dump of Table1 
 * ------------------- */
nroRecordsVol = EXAM4_VolTable1(KEY_PAIR_TABLE1,&table1Records);
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
    table2Register.mykey2 = i;
    table2Register.keytable1 = table1Records[i].mykey1;
    strcpy(table2Register.text,table1Records[i].text);
    if ((error = EXAM4_InsTable2(&table2Register)) <0)
     printf("Error Insert register %d \n",i);
    else
     printf("Register %d inserted  \n",i);

    table3Register.mykey3 = i;
    table3Register.keytable1 = table1Records[i].mykey1;
    strcpy(table3Register.ispair,(table1Records[i].pair=='p'?"IS PAIR":"IS ODD"));
    if ((error = EXAM4_InsTable3(&table3Register)) <0)
     printf("Error Insert register %d \n",i);
    else
     printf("Register %d inserted  \n",i);
}
/* ------------------------
 * !! Don't forget
 * Free memory 
 *
 * ------------------------------- */
if (nroRecordsVol >0) free(table1Records);

/* ------------------------
 * Select records in table2 
 * from key 1 to key 5
 * ----------------------------- */
nroRecordsVol = EXAM4_SelTable2ByBusqueda(KEY_SEQUENCE_TABLE2,
                        &table2Records,
                        1,
                        5);

printf(" Records in table2 beetwen key 1 and 5= %d\n",nroRecordsVol);
if (nroRecordsVol >0) free(table2Records);

/* ------------------------
 * Select records in table3 
 * key > 4 and isPair ODD
 * ----------------------------- */
nroRecordsVol = EXAM4_SelTable3ByBusqueda(KEY_SEQUENCE_TABLE3,
                        &table3Records,
                        4,
                        "OD");
printf(" Records in table3 with key >=4 and text ODD = %d\n",nroRecordsVol);
if (nroRecordsVol >0) free(table3Records);


/* ------------------------
 * Closing BBDD.
 * ------------------------------- */
EXAM4_Close();
return 0;
}
