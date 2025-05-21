#ifndef __EXAM5_H
#define __EXAM5_H

#ifdef __cplusplus
extern "C" {
#endif

/* $STR: strHeaderCompa */
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
 |  Not Uniq key: PAIR
 |  Fields = Pair
 +--------------------------------------------------------------------*/
#define MAX_TABLE1	1000

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
#define MAX_TABLE2	1000

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
#define MAX_TABLE3	1000

typedef struct {
	int            mykey3;
	int            keytable1;
	char           ispair[50];
	} TypeTABLE3;


int  EXAM5_Directory(char *directory);
int  EXAM5_Resize(int table, int dimension);
void EXAM5_Overflow(int table, int over);
int  EXAM5_NumRecords(int table);
void EXAM5_LetFileOpen(int abierto);

int  EXAM5_CreateBase();
int  EXAM5_DeleteBase();
int  EXAM5_CreatePrivate();
int  EXAM5_DeletePrivate();
int  EXAM5_Open();
int  EXAM5_Close();

int  EXAM5_InsTable1(TypeTABLE1 *table1);
int  EXAM5_DelTable1(TypeTABLE1 *table1);
int  EXAM5_ModTable1(TypeTABLE1 *table1);
 
int  EXAM5_Table1BySequence(TypeTABLE1 *table1,
				int            mykey1);
int  EXAM5_VolTable1(int claveOrd, TypeTABLE1 **pLista);
int  EXAM5_InsTable2(TypeTABLE2 *table2);
int  EXAM5_DelTable2(TypeTABLE2 *table2);
int  EXAM5_ModTable2(TypeTABLE2 *table2);
 
int  EXAM5_Table2BySequence(TypeTABLE2 *table2,
				int            mykey2);
int  EXAM5_VolTable2(int claveOrd, TypeTABLE2 **pLista);
int  EXAM5_SelTable2ByBusqueda(int claveOrd,
			TypeTABLE2 **pLista,
			int valueFrom,
			int valueTo);
int  EXAM5_InsTable3(TypeTABLE3 *table3);
int  EXAM5_DelTable3(TypeTABLE3 *table3);
int  EXAM5_ModTable3(TypeTABLE3 *table3);
 
int  EXAM5_Table3BySequence(TypeTABLE3 *table3,
				int            mykey3);
int  EXAM5_VolTable3(int claveOrd, TypeTABLE3 **pLista);
int  EXAM5_SelTable3ByBusqueda(int claveOrd,
			TypeTABLE3 **pLista,
			int valueFrom,
			char *valueText);

#ifdef __cplusplus
}
#endif
#endif /* __EXAM5_H */
