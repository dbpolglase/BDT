#ifndef __EXAM3_H
#define __EXAM3_H

#ifdef __cplusplus
extern "C" {
#endif

/* $STR: strHeaderLocal */
/*--------------------------------------------------------------------
 | Codigo asignado a la tabla TABLE1 y a sus claves
 +--------------------------------------------------------------------*/
#define TBL_TABLE1	0      
#define KEY_SEQUENCE_TABLE1	1      
/*--------------------------------------------------------------------
 | Codigo asignado a la tabla TABLE2 y a sus claves
 +--------------------------------------------------------------------*/
#define TBL_TABLE2	2      
#define KEY_SEQUENCE_TABLE2	3      
/*--------------------------------------------------------------------
 | Codigo asignado a la tabla TABLE3 y a sus claves
 +--------------------------------------------------------------------*/
#define TBL_TABLE3	4      
#define KEY_SEQUENCE_TABLE3	5      

/*--------------------------------------------------------------------
 | TABLA: TABLE1
 |  First Key: SEQUENCE
 |  Fields = Mykey1
 +--------------------------------------------------------------------*/
#define MAX_TABLE1	100

typedef struct {
	int            mykey1;
	char           text[50];
	} TypeTABLE1;

/*--------------------------------------------------------------------
 | TABLA: TABLE2
 |  First Key: SEQUENCE
 |  Fields = Mykey2
 +--------------------------------------------------------------------*/
#define MAX_TABLE2	100

typedef struct {
	int            mykey2;
	char           text[50];
	} TypeTABLE2;

/*--------------------------------------------------------------------
 | TABLA: TABLE3
 |  First Key: SEQUENCE
 |  Fields = Mykey3
 +--------------------------------------------------------------------*/
#define MAX_TABLE3	100

typedef struct {
	int            mykey3;
	char           text[50];
	} TypeTABLE3;


void EXAM3_Directory(char *directory);
void EXAM3_Resize(int table, int dimension);
void EXAM3_Overflow(int table, int over);
int  EXAM3_NumRecords(int table);

int  EXAM3_Open(int dejarAbierto);
int  EXAM3_Close();

int  EXAM3_InsTable1(TypeTABLE1 *table1);
int  EXAM3_DelTable1(TypeTABLE1 *table1);
int  EXAM3_ModTable1(TypeTABLE1 *table1);
 
int  EXAM3_Table1BySequence(TypeTABLE1 *table1,
				int            mykey1);
int  EXAM3_VolTable1(int claveOrd, TypeTABLE1 **pLista);
int  EXAM3_InsTable2(TypeTABLE2 *table2);
int  EXAM3_DelTable2(TypeTABLE2 *table2);
int  EXAM3_ModTable2(TypeTABLE2 *table2);
 
int  EXAM3_Table2BySequence(TypeTABLE2 *table2,
				int            mykey2);
int  EXAM3_VolTable2(int claveOrd, TypeTABLE2 **pLista);
int  EXAM3_InsTable3(TypeTABLE3 *table3);
int  EXAM3_DelTable3(TypeTABLE3 *table3);
int  EXAM3_ModTable3(TypeTABLE3 *table3);
 
int  EXAM3_Table3BySequence(TypeTABLE3 *table3,
				int            mykey3);
int  EXAM3_VolTable3(int claveOrd, TypeTABLE3 **pLista);

#ifdef __cplusplus
}
#endif
#endif /* __EXAM3_H */
