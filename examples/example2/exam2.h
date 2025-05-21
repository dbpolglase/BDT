#ifndef __EXAM2_H
#define __EXAM2_H

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
	char           caracter;
	unsigned char  ucaracter;
	short int      corto;
	unsigned short int ucorto;
	int            entero;
	unsigned int   uentero;
	long           lentero;
	unsigned long  ulentero;
	float          flotante;
	double         doble;
	short int      acortos[5];
	unsigned short int aucortos[5];
	int            aenteros[5];
	unsigned int   auenteros[5];
	long           alenteros[5];
	unsigned long  aulenteros[5];
	float          aflotantes[5];
	double         adoble[5];
	char           achar[50];
	} TypeTABLE;


void EXAM2_Directory(char *directory);
void EXAM2_Resize(int table, int dimension);
void EXAM2_Overflow(int table, int over);
int  EXAM2_NumRecords(int table);

int  EXAM2_Open(int dejarAbierto);
int  EXAM2_Close();

int  EXAM2_InsTable(TypeTABLE *table);
int  EXAM2_DelTable(TypeTABLE *table);
int  EXAM2_ModTable(TypeTABLE *table);
 
int  EXAM2_TableBySequence(TypeTABLE *table,
				int            mykey);
int  EXAM2_VolTable(int claveOrd, TypeTABLE **pLista);

#ifdef __cplusplus
}
#endif
#endif /* __EXAM2_H */
