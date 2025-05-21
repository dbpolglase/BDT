#ifndef __EXAM4_H
#define __EXAM4_H

#ifdef __cplusplus
extern "C" {
#endif

/* $STR: strHeaderLocal */
/*--------------------------------------------------------------------
 | Codigo asignado a la tabla TABLE1 y a sus claves
 +--------------------------------------------------------------------*/
#define TBL_TABLE1	0      
#define KEY_SEQUENCE_TABLE1	1      
#define KEY_PAIR_TABLE1	2      
/*--------------------------------------------------------------------
 | Codigo asignado a la tabla TABLE2 y a sus claves
 +--------------------------------------------------------------------*/
#define TBL_TABLE2	3      
#define KEY_SEQUENCE_TABLE2	4      
/*--------------------------------------------------------------------
 | Codigo asignado a la tabla TABLE3 y a sus claves
 +--------------------------------------------------------------------*/
#define TBL_TABLE3	5      
#define KEY_SEQUENCE_TABLE3	6      

/*--------------------------------------------------------------------
 | TABLA: TABLE1
 |  First Key: SEQUENCE
 |  Fields = Mykey1
 |  Not Uniq Key : PAIR
 |  Fields = Pair
 +--------------------------------------------------------------------*/
#define MAX_TABLE1	100

typedef struct {
	int            mykey1;
	char           pair;
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
	int            keytable1;
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
	int            keytable1;
	char           ispair[50];
	} TypeTABLE3;


void EXAM4_Directory(char *directory);
void EXAM4_Resize(int table, int dimension);
void EXAM4_Overflow(int table, int over);
int  EXAM4_NumRecords(int table);

int  EXAM4_Open(int dejarAbierto);
int  EXAM4_Close();

int  EXAM4_InsTable1(TypeTABLE1 *table1);
int  EXAM4_DelTable1(TypeTABLE1 *table1);
int  EXAM4_ModTable1(TypeTABLE1 *table1);
 
int  EXAM4_Table1BySequence(TypeTABLE1 *table1,
				int            mykey1);
int  EXAM4_VolTable1(int claveOrd, TypeTABLE1 **pLista);
int  EXAM4_InsTable2(TypeTABLE2 *table2);
int  EXAM4_DelTable2(TypeTABLE2 *table2);
int  EXAM4_ModTable2(TypeTABLE2 *table2);
 
int  EXAM4_Table2BySequence(TypeTABLE2 *table2,
				int            mykey2);
int  EXAM4_VolTable2(int claveOrd, TypeTABLE2 **pLista);
int  EXAM4_SelTable2ByBusqueda(int claveOrd,
                        TypeTABLE2 **pLista,
                        int valueFrom,
                        int valueTo);
int  EXAM4_InsTable3(TypeTABLE3 *table3);
int  EXAM4_DelTable3(TypeTABLE3 *table3);
int  EXAM4_ModTable3(TypeTABLE3 *table3);
 
int  EXAM4_Table3BySequence(TypeTABLE3 *table3,
				int            mykey3);
int  EXAM4_VolTable3(int claveOrd, TypeTABLE3 **pLista);
int  EXAM4_SelTable3ByBusqueda(int claveOrd,
                        TypeTABLE3 **pLista,
                        int valueFrom,
                        char *valueText);

#ifdef __cplusplus
}
#endif
#endif /* __EXAM4_H */
