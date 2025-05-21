/* $STR: strFuenteLocal*/
#include <errno.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <malloc.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/sem.h>
#include <fcntl.h>
#include <string.h>
#include <memory.h>
#include <tpc.h>
#include <err.h>
#include <exam2.h>
 
/*--------------------------------------------------------------------
 | PF_COMPAR: punt. a funcion comparac. para "qsort" y "BuscaElemento"
 +--------------------------------------------------------------------*/
typedef int (*PF_COMPAR)(const void *, const void *);

/*--------------------------------------------------------------------
 | BYTE:     sustituye a "unsigned char" en el modulo.
 | FUNC_BDT: nombre de la funcion para los errores.
 +--------------------------------------------------------------------*/
#define BYTE		unsigned char
#define FUNC_BDT	"Almacen example2 "
 
/*--------------------------------------------------------------------
 | CMP_VAL: compara valores simples, resultado: 1 mayor, 0 igual -1 menor
 | POS_MEM: posicion en la memoria correspondiente a una direccion.
 | DIR_MEM: direccion de la memoria correspondiente a una posicion.
 +--------------------------------------------------------------------*/
#define CMP_VAL(v1, v2)		((v1 == v2)? 0 : (((v1 < v2)? -1 : 1)))
#define POS_MEM(punt)		((char *)(punt) - pMemoria)
#define DIR_MEM(desp)		(pMemoria + (desp))

/*--------------------------------------------------------------------
 | macros para el manejo del estado de los elementos (codificados) de
 | una tabla:
 |      PONER_LIBRE:   pone un elemento como LIBRE.
 |      PONER_OCUPADO: pone un elemento como OCUPADO.
 |      ESTA_LIBRE:    dice si un elemento como LIBRE.
 |      ESTA_OCUPADO:  dice si un elemento como OCUPADO .
 +--------------------------------------------------------------------*/
#define PONER_LIBRE(pE)		pE[0]='L'
#define PONER_OCUPADO(pE)	pE[0]='O'
#define ESTA_LIBRE(pE)		(pE[0]=='L')
#define ESTA_OCUPADO(pE)	(pE[0]=='O')

/*--------------------------------------------------------------------
 | TAM_TABLE: tamano de la struct TypeTABLE
 | IND_TABLE: indice de un elemento segun una posicion en memoria.
 |
 | LON_TABLE: longitud del elemento TABLE en el fichero
 | POS_TABLE: posicion de TABLE en el fichero
 | BUF_TABLE: tamano de TABLE en el fichero.
 | ELE_TABLE: posicion de un elemento en el fichero segun el indice
 +--------------------------------------------------------------------*/
#define TAM_TABLE	(sizeof(TypeTABLE))
#define IND_TABLE(pos)	((pos - posTable) / TAM_TABLE)

#define LON_TABLE	297
#define POS_TABLE	620L
#define BUF_TABLE	(MAX_TABLE * LON_TABLE)
#define ELE_TABLE(i)	(long)(POS_TABLE+(LON_TABLE*i))

/*====================================================================
 *		FUNCIONES DE INICIALIZACION DE LA BASE
 *====================================================================*/
static void IniPunteros();
static int  IniInformacion();

/*====================================================================
 *	FUNCIONES DE TRATAMIENTO DE LA TABLA TABLE
 *====================================================================*/
static void CodTable(char *, TypeTABLE *);
static void DecTable(char *, TypeTABLE *);
static void OrdTable();
static int *ClaveTable(const int);
static int  CmpSequenceTable(const TypeTABLE *,
				const TypeTABLE *);
static int  CmpDesSequenceTable(const int *, const int *); 

/*====================================================================
 *		     FUNCIONES GENERALES A LA BASE
 *====================================================================*/

static int BuscaElemento(void *, int *, int, PF_COMPAR);
static int ActElemento(char *eleFic, int lonFic, int posFic,
		       void *eleMem, int lonMem, int posMem);
static void PosicionLibre(int, int *, int);

/*====================================================================
 *		 	VARIABLES LOCALES
 *====================================================================*/
/*--------------------------------------------------------------------
 | fichImagen:	fichero imagen de la base de datos
 | fdFBD:	descriptor del fichero base de datos
 | fichAbierto:	indicativo de si el fichero permanece abierto o no.
 | pMemoria:	memoria (compartida o dinamica) para la base.
 | tamMemoria:	tamano de la memoria en bytes para la base.
 +--------------------------------------------------------------------*/
static char *fichImagen  = "EXAM2.ibd";
static int   fdFBD	 = -1;
static int   fichAbierto = 1;
static char *pMemoria	 = NULL;
static int   lonMemoria	 = 0;

/*--------------------------------------------------------------------
 | Variables asociadas a la tabla TABLE:
 |	posTable: posicion de los elementos.
 |	numTable: punt. al numero de elementos.
 |	maxTable: numero maximo de elementos (redimensionado).
 |	ovfTable: indicativo de si la tabla admite overflow.
 |	clvSequenceTable: indice de acceso por la clave SEQUENCE
 +--------------------------------------------------------------------*/
static int  posTable = 0;
static int *numTable = NULL;
static int  maxTable = MAX_TABLE;
static int  ovfTable = 0;
static int *clvSequenceTable = NULL;


/*====================== PROCEDIMIENTO PUBLICO ======================
 * EXAM2_Directorio
 *	Notifica el directorio de donde  se debe cogen el fichero
 *	imagen de la base.
 *
 *    Parametros:
 *	- directorio: path del directorio.
 *====================================================================*/
void EXAM2_Directorio(char *directorio)
{
	char *nuevoPath;

/*--------------------------------------------------------------------
 | Se pide mem. dinamica para el path del fichero imagen.
 +--------------------------------------------------------------------*/
nuevoPath = (char *) malloc(strlen(directorio) + sizeof("EXAM2.ibd") + 1);
if (nuevoPath == NULL)
    ERR_SistOper(ERR_FATAL, FUNC_BDT, "EXAM2_Directorio", "malloc",
		 "(%d bytes) path de la base", lonMemoria);

/*--------------------------------------------------------------------
 | Se compone el nuevo path.
 +--------------------------------------------------------------------*/
sprintf(nuevoPath, "%s/EXAM2.ibd", directorio);
fichImagen = nuevoPath;
}

/*====================== PROCEDIMIENTO PUBLICO ======================
 * EXAM2_Dimensionar
 *	Notifica la nueva dimension de una determinada tabla. Siempre
 *	debe ser menor o igual a la definida como maxima.
 *	Si el codigo de tabla no es correcto o el valor de la nueva
 *	dimension supera el maximo definido, no se actualiza la
 *	dimension.
 *
 *    Parametros:
 *	- tabla: identificador de la tabla a dimensionar.
 *	- dimension: valor de la nueva dimension.
 *====================================================================*/
void EXAM2_Resize(int tabla, int dimension)
{
switch(tabla) {
    case TBL_TABLE:
	/*------------------------------------------------------------
	 | Si la dimension es mayor que la definida como maxima para la
	 | tabla TABLE no se acepta y se da un error.
	 +------------------------------------------------------------*/
	if (dimension > MAX_TABLE)
	    ERR_Aplicacion(ERR_NO_FATAL, FUNC_BDT, "EXAM2_Resize",
	      "Resize %d of TABLE overflow (max: 10000)",
	       dimension);
	else
	    maxTable = dimension;
	break;
    }
}

/*====================== PROCEDIMIENTO PUBLICO ======================
 * EXAM2_Overflow
 *	Notifica si se quiere el desbordamiento activado o no.
 *	Si el codigo de tabla no es correcto no se hace nada.
 *
 *    Parametros:
 *	- tabla: identificador de la tabla.
 *	- desbord: valor 0/1 (NO/SI) de admision de desbordamiento.
 *====================================================================*/
void EXAM2_Overflow(int tabla, int desbord)
{
switch(tabla) {
    case TBL_TABLE:
	ovfTable = desbord;
        break;
    }
}

/*========================= FUNCION PUBLICA =========================
 * EXAM2_Open
 *	Inicializa la base de datos local creandola e iniciando el acceso.
 *	La base puede dejar abierto el fichero imagen con lo que las
 *	actualizaciones seran mas rapidas pero consume un descriptor.
 *
 *    Parametros:
 *	- dejarAbierto: indicacion de si el fichero se deja abierto o no.
 *    Devuelve:
 *	0: si se crea correctamente.
 *	-1: si hay error.
 *====================================================================*/
int EXAM2_Open(int dejarAbierto)
{
TPC_HazPortable(0);
/*--------------------------------------------------------------------
 | Se calcula la memoria (segun el dimensionado) necesaria para la base.
 +--------------------------------------------------------------------*/
lonMemoria  = sizeof(int) * 1 ;
lonMemoria += maxTable * sizeof(int *) * 1;
lonMemoria += maxTable * TAM_TABLE;

/*--------------------------------------------------------------------
 | Se pide mem. dinamica para la base da datos.
 +--------------------------------------------------------------------*/
pMemoria = (char *) malloc(lonMemoria);
if (pMemoria == NULL) {
    ERR_SistOper(ERR_NO_FATAL, FUNC_BDT, "EXAM2_Inicializar", "malloc",
    		 "(%d bytes) memory for base", lonMemoria);
    return -1;
    }

/*--------------------------------------------------------------------
 | Se guarda el modo de uso del fichero (abierto siempre o abrir y cerar).
 | Se inicializan los punteros de acceso a la memoria.
 | Se inicializan los arrays de indices de acceso por clave.
 +--------------------------------------------------------------------*/
fichAbierto = dejarAbierto;
IniPunteros();
return IniInformacion();
}

/*====================== PROCEDIMIENTO PUBLICO ======================
 * EXAM2_Close
 *	Finaliza el acceso a la base de datos
 *====================================================================*/
int EXAM2_Close()
{
/*--------------------------------------------------------------------
 | Se libera la memoria dinamica.
 +--------------------------------------------------------------------*/
free(pMemoria);

/*--------------------------------------------------------------------
 | Si esta abierto, se cierra el fichero imagen de la base de datos.
 +--------------------------------------------------------------------*/
if (fichAbierto)
    if (close(fdFBD) == -1) {
	ERR_SistOper(ERR_NO_FATAL, FUNC_BDT, "EXAM2_Close", "close",
			"file \"%s\"", fichImagen);
	return -1;
	}
return 0;
}

/*========================= FUNCION PUBLICA =========================
 * EXAM2_NumRecords
 *	Informa del numero de registros que tiene la tabla.
 *
 *    Parametros:
 *	- tabla: identificador de la tabla a dimensionar.
 *    Devuelve:
 *	 n: el numero de registros.
 *	-1: si la tabla no es correcta
 *====================================================================*/
int EXAM2_NumRecords(int tabla)
{
switch(tabla) {
    case TBL_TABLE:
	return *numTable;
    }
return -1;
}


/*========================= FUNCION PUBLICA =========================
 * EXAM2_InsTable
 *	Inserta un nuevo elemento TABLE, segun la clave
 *	principal, para lo cual la struct debe tener todos los
 *	campos de las claves con los valores actualizados.
 *
 *    Parametros:
 *	ptable: punt. a la struct del elemento a insertar.
 *    Devuelve:
 *	 0: si la insercion se realiza correctamente.
 *	-1: si se produce un error.
 *====================================================================*/
int EXAM2_InsTable(TypeTABLE *pTable)
{
	char elemCod[LON_TABLE];
	int  posicMem;
	int  posicFic;
	int  indElem;
	int  numElem;
	int  indClave;

/*--------------------------------------------------------------------
 | Se obtiene el numero actual de elementos.
 +--------------------------------------------------------------------*/
numElem = *numTable;

/*--------------------------------------------------------------------
 | No debe existir otro elemento con igual valor en la clave principal
 +--------------------------------------------------------------------*/
if (BuscaElemento(pTable, clvSequenceTable,
		  numElem, (PF_COMPAR)CmpSequenceTable) >= 0) {
    ERR_Aplicacion(ERR_NO_FATAL, FUNC_BDT, "EXAM2_InsTable",
		 "key SEQUENCE of TABLE exits");
    return -1;
    }

/*--------------------------------------------------------------------
 | Si hay elementos libres, se toma para insercion el primero libre.
 | Si se sobrepasa el limite de elementos max10000 solo se
 | acepta si se admite overflow. En ese caso se inserta sobre el ultimo
 | elemento que es el elemento 0 y que tiene la clave menor.
 +--------------------------------------------------------------------*/
if (numElem < maxTable) {
    indClave = *numTable;
    numElem++;
    }
else if (ovfTable)
    indClave = 0;
else {
    ERR_Aplicacion(ERR_NO_FATAL, FUNC_BDT, "EXAM2_InsTable",
	      "overflow on TABLE (%d)", maxTable);
    return -1;
    }

/*--------------------------------------------------------------------
 | Se calculan las posiciones para la actualizacion.
 +--------------------------------------------------------------------*/
posicMem = clvSequenceTable[indClave];
indElem  = IND_TABLE(posicMem);
posicFic = ELE_TABLE(indElem);


/*--------------------------------------------------------------------
 | Codifica la struct TypeTABLE sobre el elemento como OCUPADO
 +--------------------------------------------------------------------*/
CodTable(elemCod, pTable);
PONER_OCUPADO(elemCod);
if (ActElemento(elemCod, LON_TABLE, posicFic,
		pTable, TAM_TABLE, posicMem) == -1) {
    return -1;
    }

*numTable = numElem;
OrdTable();
return 0;
}

/*========================= FUNCION PUBLICA =========================
 * EXAM2_DelTable
 *	Elimina un elemento TABLE ya existente, segun la
 *	clave Principal.
 *
 *    Parametros:
 *	pTable: punt. a la struct del elemento a eliminar.
 *    Devuelve:
 *	 0: si la eliminacion se realiza correctamente.
 *	-1: si se produce un error.
 *====================================================================*/
int EXAM2_DelTable(TypeTABLE *pTable)
{
	char elemCod[LON_TABLE];
	int  posicMem;
	int  posicFic;
	int  indElem;

/*--------------------------------------------------------------------
 | No debe existir otro elemento con igual valor en la clave principal
 +--------------------------------------------------------------------*/
indElem = BuscaElemento(pTable, clvSequenceTable,
		*numTable, (PF_COMPAR)CmpSequenceTable);
if (indElem == -1) {
    ERR_Aplicacion(ERR_NO_FATAL, FUNC_BDT, "EXAM2_EliTable",
		 "key SEQUENCE of TABLE not exists");
    return -1;
    }

/*--------------------------------------------------------------------
 | Se calculan las posiciones para la actualizacion.
 +--------------------------------------------------------------------*/
posicMem = clvSequenceTable[indElem];
indElem  = IND_TABLE(posicMem);
posicFic = ELE_TABLE(indElem);

/*--------------------------------------------------------------------
 | Codifica la struct TypeTABLE sobre el elemento como OCUPADO
 +--------------------------------------------------------------------*/
memset(elemCod, 0, LON_TABLE);
PONER_LIBRE(elemCod);
if (ActElemento(elemCod, LON_TABLE, posicFic,
		pTable, TAM_TABLE, posicMem) == -1)
    return -1;

PosicionLibre(posicMem, clvSequenceTable, *numTable);
(*numTable)--;
return 0;
}

/*========================= FUNCION PUBLICA =========================
 * EXAM2_ModTable
 *	Modifica un elemento TABLE ya existente, segun la
 *	clave principal, para lo cual la struct debe tener todos los
 *	campos con los valores actualizados.
 *
 *    Parametros:
 *	table: punt. a la struct del elemento a eliminar.
 *    Devuelve:
 *	 0: si la modificacion se realiza correctamente.
 *	-1: si se produce un error.
 *====================================================================*/
int EXAM2_ModTable(TypeTABLE *pTable)
{
	char elemCod[LON_TABLE];
	int  posicMem;
	int  posicFic;
	int  indElem;

/*--------------------------------------------------------------------
 | No debe existir otro elemento con igual valor en la clave principal
 +--------------------------------------------------------------------*/
indElem = BuscaElemento(pTable, clvSequenceTable,
		*numTable, (PF_COMPAR)CmpSequenceTable);
if (indElem == -1) {
    ERR_Aplicacion(ERR_NO_FATAL, FUNC_BDT, "EXAM2_ModTable",
		   "key SEQUENCE of TABLE not exists");
    return -1;
    }

/*--------------------------------------------------------------------
 | Se calculan las posiciones para la actualizacion.
 +--------------------------------------------------------------------*/
posicMem = clvSequenceTable[indElem];
indElem  = IND_TABLE(posicMem);
posicFic = ELE_TABLE(indElem);


/*--------------------------------------------------------------------
 | Codifica la struct TypeTABLE sobre el elemento como OCUPADO
 +--------------------------------------------------------------------*/
CodTable(elemCod, pTable);
PONER_OCUPADO(elemCod);
if (ActElemento(elemCod, LON_TABLE, posicFic,
		pTable, TAM_TABLE, posicMem) == -1)
    return -1;

return 0;
}

/*========================= FUNCION PUBLICA =========================
 * EXAM2_TableBySequence
 *	Busca un elemento TABLE , por la clave principal
 *	para lo cual la struct debe tener todos los campos de la
 *	clave con los valores actualizados.
 *
 *    Parametros:
 *	table: punt. a la struct del elemento a buscar.
  *	mykey: campo MYKEY de busqueda.
  *    Devuelve:
 *	 0: si la modificacion se realiza correctamente.
 *	-1: si se produce un error.
 *	Busca un elemento
 *====================================================================*/
int EXAM2_TableBySequence(TypeTABLE *pTable,
				int            mykey)
{
	int  indElem;

/*--------------------------------------------------------------------
 | Se codifican los campos de la clave principal sobre el elemento.
 +--------------------------------------------------------------------*/
pTable->mykey = mykey;

/*--------------------------------------------------------------------
 | Se busca el elemento accediendo por la clave principal y si no
 | existe se devuelve -1.
 +--------------------------------------------------------------------*/
indElem = BuscaElemento(pTable, clvSequenceTable,
	      *numTable, (PF_COMPAR)CmpSequenceTable);
if (indElem == -1)
    return -1;

/*--------------------------------------------------------------------
 | Si existe, se decodifica el elemento completo sobre la struct.
 +--------------------------------------------------------------------*/
memcpy(pTable, DIR_MEM(clvSequenceTable[indElem]), TAM_TABLE);
return 0;
}

/*========================= FUNCION PUBLICA =========================
 * EXAM2_VolTable
 *	Realiza un volcado de la tabla TABLE ordenado por
 *	la clave segun el parametro.
 *    Parametros:
 *	claveOrd: codigo de la clave para ordenar el volcado.
 *	pLista: puntero para la lista del volcado.
 *    Devuelve:
 *	n: el numero de elementos seleccionados.
 *	-1: si hay error en el acceso.
 *====================================================================*/
int EXAM2_VolTable(int claveOrd, TypeTABLE **pLista)
{
	int i;
	TypeTABLE *pTable;
	int *claveAcceso;

/*--------------------------------------------------------------------
 | Se recoge el array de acceso por la clave definida por "claveOrd"
 +--------------------------------------------------------------------*/
claveAcceso = ClaveTable(claveOrd);
if (claveAcceso == NULL)
    return -1;

/*--------------------------------------------------------------------
 | Si la tabla esta vacia no se hace nada mas y se devuelve 0.
 +--------------------------------------------------------------------*/
if (*numTable == 0)
    return 0;

/*--------------------------------------------------------------------
 | Se pide memoria dinamica para almacenar los elementos del recorrido.
 +--------------------------------------------------------------------*/
pTable = (TypeTABLE *) malloc(*numTable * TAM_TABLE);
if (pTable == NULL) {
    ERR_SistOper(ERR_NO_FATAL, FUNC_BDT, "EXAM2_VolTable",
	     "malloc", "(%d bytes) para volcado de Table", lonMemoria);
    return -1;
    }

*pLista = pTable;

/*--------------------------------------------------------------------
 | Se copia la informacion de cada elemento de la lista segun la
 | ordenacion de la clave.
 +--------------------------------------------------------------------*/
for (i = 0; i < *numTable; i++) {
    memcpy(pTable, DIR_MEM(claveAcceso[i]), TAM_TABLE);
    pTable++;
    }

return i;
}


/*####################################################################
 *	 	FUNIONES LOCALES DEPENDIENTES DE LA BASE
 *####################################################################*/

/*========================= FUNCION PRIVADA =========================
 * IniInformacion
 *	Inicializa el acceso al fichero imagen de la base de datos y
 *	se lee su contenido sobre la memoria dinamica.
 *
 *    Devuelve:
 *	 0: si el acceso se realiza correctamente.
 *	-1: si se produce un error.
 *====================================================================*/
static int IniInformacion()
{
	int   indOcu;
	int   indLib;
	int   indAct;
	char *pElem;
	int   i;  

/*--------------------------------------------------------------------
 | Se mira si el fichero imagen de la base de datos existe
 +--------------------------------------------------------------------*/
if (access(fichImagen, F_OK) == -1) {
    ERR_Aplicacion(ERR_NO_FATAL, FUNC_BDT, "EXAM2: IniInformacion",
		   "fichero \"%s\" no existente", fichImagen);
    return -1;
    }

/*--------------------------------------------------------------------
 | Se mira si el fichero imagen de Config. tiene permiso de lectura
 +--------------------------------------------------------------------*/
if (access(fichImagen, R_OK) == -1) {
    ERR_Aplicacion(ERR_NO_FATAL, FUNC_BDT, "EXAM2: IniInformacion",
		   "file \"%s\" can't read", fichImagen);
    return -1;
    }

/*--------------------------------------------------------------------
 | Se mira si el fichero imagen de Config. tiene permiso de escritura
 +--------------------------------------------------------------------*/
if (access(fichImagen, W_OK) == -1) {
    ERR_Aplicacion(ERR_NO_FATAL, FUNC_BDT, "EXAM2: IniInformacion",
		   "file \"%s\" can't write", fichImagen);
    return -1;
    }

/*--------------------------------------------------------------------
 | Se abre el fichero imagen de la base de datos
 +--------------------------------------------------------------------*/
fdFBD = open(fichImagen, O_RDWR | O_EXCL);
if (fdFBD == -1) {
    ERR_SistOper(ERR_NO_FATAL, FUNC_BDT, "EXAM2: IniInformacion", "open",
		 "file \"%s\" (for read/write))", fichImagen);
    return -1;
    }
 
{
        char * bufElems;
	TypeTABLE *pTable;

/*--------------------------------------------------------------------
 | Se pide mem. dinamica para el leer el fichero imagen.
 +--------------------------------------------------------------------*/
bufElems = (char *) malloc( BUF_TABLE + 1);
if (bufElems == NULL)
    ERR_SistOper(ERR_FATAL, FUNC_BDT, "EXAM2 IniInformacion", "malloc",
		 "(%d bytes) path of base", BUF_TABLE);

/*--------------------------------------------------------------------
 | Se posiciona en la posicion  de la tabla TABLE en el fichero
 +--------------------------------------------------------------------*/
if (lseek(fdFBD, POS_TABLE, SEEK_SET) == -1) {
    ERR_SistOper(ERR_NO_FATAL, FUNC_BDT, "EXAM2: IniInformacion", "lseek",
		"file \"%s\" (position POS_TABLE)", fichImagen);
    return -1;
    }

/*--------------------------------------------------------------------
 | Se leen todos los elementos de la tabla desde el fichero.
 +--------------------------------------------------------------------*/
if (read(fdFBD, bufElems, BUF_TABLE) != BUF_TABLE) {
    ERR_SistOper(ERR_NO_FATAL, FUNC_BDT, "EXAM2: IniInformacion", "read",
		 "file \"%s\" (BUF_TABLE bytes)", fichImagen);
    return -1;
    }

/*--------------------------------------------------------------------
 | Se cuenta el numero de elementos que tiene el fichero y se comprueba
 | que el dimensionado es suficiente.
 +--------------------------------------------------------------------*/
pElem = bufElems;
*numTable = 0;
for (i = 0; i < MAX_TABLE; i++, pElem += LON_TABLE)
    if (ESTA_OCUPADO(pElem))
	(*numTable)++;

if (*numTable > maxTable) {
    ERR_Aplicacion(ERR_NO_FATAL, FUNC_BDT, "EXAM2: IniInformacion",
    	"size  %d of TABLE great than max dimension %d",
	*numTable, maxTable);
    return -1;
    }

/*--------------------------------------------------------------------
 | Se extraen los registros ocupados y se inicializan las posiciones
 | de cada clave.
 +--------------------------------------------------------------------*/
indOcu = 0;
indLib = *numTable;
pElem  = bufElems;
pTable = (TypeTABLE *) DIR_MEM(posTable);
for (i = 0; i < maxTable; i++) {
    if (ESTA_OCUPADO(pElem)) {
	DecTable(pElem, pTable);
	indAct = indOcu;
	indOcu++;
	}
    else {
	indAct = indLib;
	indLib++;
	}
    clvSequenceTable[indAct] = POS_MEM(pTable);
    pElem += LON_TABLE;
    pTable++;
    }
    free(bufElems);
}

/*--------------------------------------------------------------------
 | Se ordenan los array de posiciones de las claves de la tabla.
 +--------------------------------------------------------------------*/
OrdTable();


/*--------------------------------------------------------------------
 | Si no se deja abierto, se cierra el fichero imagen de la base.
 +--------------------------------------------------------------------*/
if (! fichAbierto)
    if (close(fdFBD) == -1) {
	ERR_SistOper(ERR_NO_FATAL, FUNC_BDT, "EXAM2: IniInformacion", "close",
		     "file \"%s\"", fichImagen);
	return -1;
	}

return 0;
}

/*====================== PROCEDIMIENTO PRIVADO ======================
 * IniPunteros
 *	Inicializa los punteros de acceso a cada una de las zonas de
 *	la memoria:
 *		- posicion correspondiente con el fichero imagen.
 *		- arrays de acceso por clave.
 *		- numero de elementos.
 *		- informacion de la tablas.
 *====================================================================*/
static void IniPunteros()
{
	char *pActual;

/*--------------------------------------------------------------------
 | Se inicializa pActual al comienzo de la zona de memoria de la base
 +--------------------------------------------------------------------*/
pActual = pMemoria;

/*--------------------------------------------------------------------
 | Se inicializan los punteros de acceso a la tabla TABLE:
 *	numTable: punt. al numero de elementos.
 *	clvSequenceTable: array de acceso por la clave Sequence
 *	posTable: informacion de los elementos.
 +--------------------------------------------------------------------*/
numTable = (int *) pActual;
pActual += sizeof(int);
clvSequenceTable = (int *) pActual;
pActual += (maxTable * sizeof(int *));
posTable = POS_MEM(pActual);
pActual += (maxTable * TAM_TABLE);

}


/*====================== PROCEDIMIENTO PRIVADO ======================
 * CodTable
 *	Codifica un TypeTABLE sobre un elemento Table.
 *    Parametros:
 *      elem: buffer del elemento sobre el que codificar.
 *	table: struct a realizar la codificacion.
 *====================================================================*/
static void CodTable(char *elem, TypeTABLE *pTable)
{
TPC_CodInt(&pTable->mykey, 1, elem + 1);
TPC_CodChar(&pTable->caracter, 1, elem + 5);
TPC_CodUnsChar(&pTable->ucaracter, 1, elem + 6);
TPC_CodShort(&pTable->corto, 1, elem + 7);
TPC_CodUnsShort(&pTable->ucorto, 1, elem + 9);
TPC_CodInt(&pTable->entero, 1, elem + 11);
TPC_CodUnsInt(&pTable->uentero, 1, elem + 15);
TPC_CodLong(&pTable->lentero, 1, elem + 19);
TPC_CodUnsLong(&pTable->ulentero, 1, elem + 27);
TPC_CodFloat(&pTable->flotante, 1, elem + 35);
TPC_CodDouble(&pTable->doble, 1, elem + 39);
TPC_CodShort(pTable->acortos, 5, elem + 47);
TPC_CodUnsShort(pTable->aucortos, 5, elem + 57);
TPC_CodInt(pTable->aenteros, 5, elem + 67);
TPC_CodUnsInt(pTable->auenteros, 5, elem + 87);
TPC_CodLong(pTable->alenteros, 5, elem + 107);
TPC_CodUnsLong(pTable->aulenteros, 5, elem + 147);
TPC_CodFloat(pTable->aflotantes, 5, elem + 187);
TPC_CodDouble(pTable->adoble, 5, elem + 207);
TPC_CodChar(pTable->achar, 50, elem + 247);
}

/*====================== PROCEDIMIENTO PRIVADO ======================
 * DecTable
 *	Decodifica un elemento Table sobre un TypeTABLE
 *    Parametros:
 *      elem: buffer del elemento a decodificar.
 *	table: struct sobre la que realizar la decodificacion.
 *====================================================================*/
static void DecTable(char *elem, TypeTABLE *pTable)
{
TPC_DecInt(&pTable->mykey, 1, elem + 1);
TPC_DecChar(&pTable->caracter, 1, elem + 5);
TPC_DecUnsChar(&pTable->ucaracter, 1, elem + 6);
TPC_DecShort(&pTable->corto, 1, elem + 7);
TPC_DecUnsShort(&pTable->ucorto, 1, elem + 9);
TPC_DecInt(&pTable->entero, 1, elem + 11);
TPC_DecUnsInt(&pTable->uentero, 1, elem + 15);
TPC_DecLong(&pTable->lentero, 1, elem + 19);
TPC_DecUnsLong(&pTable->ulentero, 1, elem + 27);
TPC_DecFloat(&pTable->flotante, 1, elem + 35);
TPC_DecDouble(&pTable->doble, 1, elem + 39);
TPC_DecShort(pTable->acortos, 5, elem + 47);
TPC_DecUnsShort(pTable->aucortos, 5, elem + 57);
TPC_DecInt(pTable->aenteros, 5, elem + 67);
TPC_DecUnsInt(pTable->auenteros, 5, elem + 87);
TPC_DecLong(pTable->alenteros, 5, elem + 107);
TPC_DecUnsLong(pTable->aulenteros, 5, elem + 147);
TPC_DecFloat(pTable->aflotantes, 5, elem + 187);
TPC_DecDouble(pTable->adoble, 5, elem + 207);
TPC_DecChar(pTable->achar, 50, elem + 247);
}

/*========================= FUNCION PRIVADA =========================
 * OrdTable
 *	Ordena los punt. de acceso a la tabla TABLE, por:
 *		- SEQUENCE
 *====================================================================*/
static void OrdTable()
{
/*--------------------------------------------------------------------
 | Ordena el array de acceso a la tabla TABLE por las claves:
 |	- SEQUENCE
 +--------------------------------------------------------------------*/
qsort(clvSequenceTable, *numTable,
      sizeof(int), (PF_COMPAR)CmpDesSequenceTable);
}

/*========================= FUNCION PRIVADA =========================
 * ClaveTable
 *	Devuelve el array de acceso correspondiente el codigo de
 *	una determinada clave de la tabla TABLE.
 *
 *    Parametros:
 *	codClave: codigo de la clave.
 *    Devuelve:
 *	el array de acceso de la clave: si el codigo es correcto.
 *	NULL: si el codigo es erroneo.
 *====================================================================*/
static int *ClaveTable(const int codClave)
{
switch(codClave) {
    case KEY_SEQUENCE_TABLE:
	return clvSequenceTable;
    }
ERR_Aplicacion(ERR_NO_FATAL, FUNC_BDT, "EXAM2: KeyTable",
	"key code (%d) on table TABLE wrong", codClave);
return NULL;
}

/*========================= FUNCION PRIVADA =========================
 * CmpDesSequenceTable
 *	Compara la clave SEQUENCE de dos elementos de la tabla
 *	TABLE segun sus posiciones en la memoria.
 *    Parametros:
 *	pPosic1: punt. a la posicion del elemento 1 a comparar la clave.
 *	pPosic2: punt. a la posicion del elemento 2 a comparar la clave.
 *    Devuelve:
 *      num > 0: si la clave del pPosic1 es mayor que el del pPosic2.
 *      0:	 si la clave del pPosic1 es igual que el del pPosic2.
 *      num < 0: si la clave del pPosic1 es menor que el del pPosic2.
 *====================================================================*/
static int CmpDesSequenceTable(const int *pPosic1, const int *pPosic2)
{
return CmpSequenceTable((TypeTABLE *) DIR_MEM(*pPosic1),
			(TypeTABLE *) DIR_MEM(*pPosic2));
}

/*========================= FUNCION PRIVADA =========================
 * CmpSequenceTable
 *	Compara la clave SEQUENCE de dos elementos de la tabla
 *	TABLE.
 *    Parametros:
 *	table:  punt. al elemento 1 a comparar la clave.
 *	table2: punt. al elemento 2 a comparar la clave.
 *    Devuelve:
 *      num > 0: si la clave de table es mayor que table2.
 *      0:       si la clave de table es igual que table2.
 *      num < 0: si la clave de table es menor que table2.
 *====================================================================*/
static int CmpSequenceTable(const TypeTABLE *pTable,
				const TypeTABLE *pTable2)
{
	int result;

/*--------------------------------------------------------------------
 | Si el campo MYKEY del elemento 1 y 2 es distinto, se devuelve
 | el resultado de la comparacion.
 +--------------------------------------------------------------------*/
if ((result = CMP_VAL(pTable->mykey, pTable2->mykey)) != 0) 
    return result;

return 0;
}


/*========================= FUNCION PRIVADA =========================
 * BuscaElemento
 *	Busca un elemento por el algoritmo de busqueda binaria
 *
 *    Parametros:
 *	elemBuscar: buffer del elemento a buscar.
 *	accClave:   array de posiciones para acceso por clave.
 *	numElem:    numero de elementos de la tabla.
 *	FuncCompar: funcion de comparacion por la clave en cuestion.
 *    Devuelve:
 *	indice: indice (referido a puntClave) del elemento buscado.
 *	-1: si el elemento no existe.
 *====================================================================*/
static int BuscaElemento(void *elemBuscar, int *accClave, int numElem,
			 PF_COMPAR FuncCompar)
{
	int topeBajo;
	int topeAlto;
	int indice;
	int result;

/*--------------------------------------------------------------------
 | Se inicializa la franja de busqueda a la totalidad de elementos.
 +--------------------------------------------------------------------*/
topeBajo = 0;
topeAlto = numElem - 1;

/*--------------------------------------------------------------------
 | Se compara el elemento intermedio de la franja tal que si es:
 |  - IGUAL: elemento encontrado.
 |  - MAYOR: se toma la franja mitad superior (avanza topeBajo).
 |  - MENOR: se toma la franja mitad inferior (retrocede topeAlto).
 | Si se cruzan los indices, el elemento no existe.
 +--------------------------------------------------------------------*/
while (topeBajo <= topeAlto) {
    indice = (topeBajo + topeAlto) / 2;
    result = (*FuncCompar)(elemBuscar, DIR_MEM(accClave[indice]));
    if (result == 0)
	return indice;
    if (result > 0)
	topeBajo = indice + 1;
    else
	topeAlto = indice - 1;
    }
return -1;
}

/*========================= FUNCION PRIVADA =========================
 * ActElemento
 *	Actualiza el valor de un elemento de una tabla.
 *
 *    Parametros:
 *	eleFic: elemento con el formato del fichero (codificado).
 *	lonFic: longitud en bytes del elemento del fichero.
 *	posFic: posicion en el fichero del elemento.
 *	eleMem: elemento (struct).
 *	lonMem: longitud en bytes del elemento.
 *	posMem: posicion en la memoria.
 *    Devuelve:
 *	0: si la actualizacion es correcta.
 *	-1: si error en la actualizacion.
 *====================================================================*/
static int ActElemento(char *eleFic, int lonFic, int posFic,
		       void *eleMem, int lonMem, int posMem)
{
/*--------------------------------------------------------------------
 | Se abre el fichero imagen de la base de datos si no esta abierto.
 +--------------------------------------------------------------------*/
if (! fichAbierto) {
    fdFBD = open(fichImagen, O_RDWR | O_EXCL);
    if (fdFBD == -1) {
	ERR_SistOper(ERR_NO_FATAL, FUNC_BDT, "EXAM2: ActElemento", "open",
		 "file \"%s\" (for read/write))", fichImagen);
	return -1;
	}
    }
 
/*--------------------------------------------------------------------
 | Se prepara la escritura sobre la posicion del elemento.
 +--------------------------------------------------------------------*/
if (lseek(fdFBD, (off_t)posFic, SEEK_SET) == -1) {
    ERR_SistOper(ERR_NO_FATAL, FUNC_BDT, "EXAM2: ActElemento", "lseek",
    		 "file \"%s\" (position %ld)", fichImagen, posFic);
    return -1;
    }

/*--------------------------------------------------------------------
 | Se escribe el elemento sobre el fichero.
 +--------------------------------------------------------------------*/
if (write(fdFBD, (char *)eleFic, lonFic) != lonFic) {
    ERR_SistOper(ERR_NO_FATAL, FUNC_BDT, "EXAM2: ActElemento", "write",
    		 "file \"%s\" (%d bytes)", fichImagen, lonFic);
    return -1;
    }

/*--------------------------------------------------------------------
 | Si el fichero no se deja abierto, se cierra.
 +--------------------------------------------------------------------*/
if (! fichAbierto)
    if (close(fdFBD) == -1) {
	ERR_SistOper(ERR_NO_FATAL, FUNC_BDT, "EXAM2: ActElemento", "close",
		     "file \"%s\"", fichImagen);
	return -1;
	}

/*--------------------------------------------------------------------
 | Se escribe el elemento sobre la memoria.
 +--------------------------------------------------------------------*/
memcpy(DIR_MEM(posMem), eleMem, (size_t)lonMem);

return 0;
}

/*====================== PROCEDIMIENTO PRIVADO ======================
 * PosicionLibre
 *	Deja libre una posicion en un array de posiciones.
 *
 *    Parametros:
 *	posicion: valor de la posicion a dejar libre.
 *	array: array de posiciones a tratar.
 *	numElem: numero de elementos del array de posiciones.
 *====================================================================*/
static void PosicionLibre(int posicion, int *array, int numElem)
{
	int i;

/*--------------------------------------------------------------------
 | Se busca la posicion en el array
 +--------------------------------------------------------------------*/
for (i = 0; i < numElem; i++)
    if (array[i] == posicion)
	break;

/*--------------------------------------------------------------------
 | Elimina la posicion moviendo el bloque de posiciones y la pone al
 | final de dicho bloque.
 +--------------------------------------------------------------------*/
memmove(array + i, array + i + 1, (numElem - i - 1) * sizeof(int));
array[numElem - 1] = posicion;
}
