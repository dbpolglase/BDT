#ifndef __EXAM1_H
#define __EXAM1_H

#ifdef __cplusplus
extern "C" {
#endif

/* $STR: strHeaderLocal */
/*--------------------------------------------------------------------
 | Codigo asignado a la tabla TABLE y a sus claves
 +--------------------------------------------------------------------*/
#define TBL_TABLE	0      
#define KEY_SEQUENCE_TABLE	1      

/*--------------------------------------------------------------------
 | TABLA: TABLE
 |  First Key: SEQUENCE
 |  Fields = Mykey
 +--------------------------------------------------------------------*/
#define MAX_TABLE	10000

typedef struct {
	int            mykey;
	char           text[50];
	} TypeTABLE;


void EXAM1_Directory(char *directory);
void EXAM1_Resize(int table, int dimension);
void EXAM1_Overflow(int table, int over);
int  EXAM1_NumRecords(int table);

int  EXAM1_Open(int dejarAbierto);
int  EXAM1_Close();

int  EXAM1_InsTable(TypeTABLE *table);
int  EXAM1_DelTable(TypeTABLE *table);
int  EXAM1_ModTable(TypeTABLE *table);
 
int  EXAM1_TableBySequence(TypeTABLE *table,
				int            mykey);
int  EXAM1_VolTable(int claveOrd, TypeTABLE **pLista);

#ifdef __cplusplus
}
#endif
#endif /* __EXAM1_H */
