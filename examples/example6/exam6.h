#ifndef __EXAM6_H
#define __EXAM6_H

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
 | TABLA: TABLE1
 |  First Key: SEQUENCE
 |  Fields = Mykey
 +--------------------------------------------------------------------*/
#define MAX_TABLE1	100

typedef struct {
	int            mykey;
	char           caracter;
	} TypeTABLE1;

/*--------------------------------------------------------------------
 | TABLA: TABLE2
 |  First Key: SEQUENCE
 |  Fields = Mykey
 +--------------------------------------------------------------------*/
#define MAX_TABLE2	100

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
	} TypeTABLE2;


void EXAM6_Directory(char *directory);
void EXAM6_Resize(int table, int dimension);
void EXAM6_Overflow(int table, int over);
int  EXAM6_NumRecords(int table);

int  EXAM6_Open(int dejarAbierto);
int  EXAM6_Close();

int  EXAM6_InsTable1(TypeTABLE1 *table1);
int  EXAM6_DelTable1(TypeTABLE1 *table1);
int  EXAM6_ModTable1(TypeTABLE1 *table1);
 
int  EXAM6_Table1BySequence(TypeTABLE1 *table1,
				int            mykey);
int  EXAM6_VolTable1(int claveOrd, TypeTABLE1 **pLista);
int  EXAM6_InsTable2(TypeTABLE2 *table2);
int  EXAM6_DelTable2(TypeTABLE2 *table2);
int  EXAM6_ModTable2(TypeTABLE2 *table2);
 
int  EXAM6_Table2BySequence(TypeTABLE2 *table2,
				int            mykey);
int  EXAM6_VolTable2(int claveOrd, TypeTABLE2 **pLista);

#ifdef __cplusplus
}
#endif
#endif /* __EXAM6_H */
