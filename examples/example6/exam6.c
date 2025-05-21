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
#include <exam6.h>
 
/*--------------------------------------------------------------------
 | PF_COMPAR: punt. a funcion comparac. para "qsort" y "BuscaElemento"
 +--------------------------------------------------------------------*/
typedef int (*PF_COMPAR)(const void *, const void *);

/*--------------------------------------------------------------------
 | BYTE:     sustituye a "unsigned char" en el modulo.
 | FUNC_BDT: nombre de la funcion para los errores.
 +--------------------------------------------------------------------*/
#define BYTE		unsigned char
#define FUNC_BDT	"Almacen example6 "
 
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
 | TAM_TABLE1: tamano de la struct TypeTABLE1
 | IND_TABLE1: indice de un elemento segun una posicion en memoria.
 |
 | LON_TABLE1: longitud del elemento TABLE1 en el fichero
 | POS_TABLE1: posicion de TABLE1 en el fichero
 | BUF_TABLE1: tamano de TABLE1 en el fichero.
 | ELE_TABLE1: posicion de un elemento en el fichero segun el indice
 +--------------------------------------------------------------------*/
#define TAM_TABLE1	(sizeof(TypeTABLE1))
#define IND_TABLE1(pos)	((pos - posTable1) / TAM_TABLE1)

#define LON_TABLE1	6
#define POS_TABLE1	747L
#define BUF_TABLE1	(MAX_TABLE1 * LON_TABLE1)
#define ELE_TABLE1(i)	(long)(POS_TABLE1+(LON_TABLE1*i))
/*--------------------------------------------------------------------
 | TAM_TABLE2: tamano de la struct TypeTABLE2
 | IND_TABLE2: indice de un elemento segun una posicion en memoria.
 |
 | LON_TABLE2: longitud del elemento TABLE2 en el fichero
 | POS_TABLE2: posicion de TABLE2 en el fichero
 | BUF_TABLE2: tamano de TABLE2 en el fichero.
 | ELE_TABLE2: posicion de un elemento en el fichero segun el indice
 +--------------------------------------------------------------------*/
#define TAM_TABLE2	(sizeof(TypeTABLE2))
#define IND_TABLE2(pos)	((pos - posTable2) / TAM_TABLE2)

#define LON_TABLE2	297
#define POS_TABLE2	1347L
#define BUF_TABLE2	(MAX_TABLE2 * LON_TABLE2)
#define ELE_TABLE2(i)	(long)(POS_TABLE2+(LON_TABLE2*i))

/*====================================================================
 *		FUNCIONES DE INICIALIZACION DE LA BASE
 *====================================================================*/
static void IniPunteros();
static int  IniInformacion();

/*====================================================================
 *	FUNCIONES DE TRATAMIENTO DE LA TABLA TABLE1
 *====================================================================*/
static void CodTable1(char *, TypeTABLE1 *);
static void DecTable1(char *, TypeTABLE1 *);
static void OrdTable1();
static int *ClaveTable1(const int);
static int  CmpSequenceTable1(const TypeTABLE1 *,
				const TypeTABLE1 *);
static int  CmpDesSequenceTable1(const int *, const int *); 
/*====================================================================
 *	FUNCIONES DE TRATAMIENTO DE LA TABLA TABLE2
 *====================================================================*/
static void CodTable2(char *, TypeTABLE2 *);
static void DecTable2(char *, TypeTABLE2 *);
static void OrdTable2();
static int *ClaveTable2(const int);
static int  CmpSequenceTable2(const TypeTABLE2 *,
				const TypeTABLE2 *);
static int  CmpDesSequenceTable2(const int *, const int *); 

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
static char *fichImagen  = "EXAM6.ibd";
static int   fdFBD	 = -1;
static int   fichAbierto = 1;
static char *pMemoria	 = NULL;
static int   lonMemoria	 = 0;

/*--------------------------------------------------------------------
 | Variables asociadas a la tabla TABLE1:
 |	posTable1: posicion de los elementos.
 |	numTable1: punt. al numero de elementos.
 |	maxTable1: numero maximo de elementos (redimensionado).
 |	ovfTable1: indicativo de si la tabla admite overflow.
 |	clvSequenceTable1: indice de acceso por la clave SEQUENCE
 +--------------------------------------------------------------------*/
static int  posTable1 = 0;
static int *numTable1 = NULL;
static int  maxTable1 = MAX_TABLE1;
static int  ovfTable1 = 0;
static int *clvSequenceTable1 = NULL;

/*--------------------------------------------------------------------
 | Variables asociadas a la tabla TABLE2:
 |	posTable2: posicion de los elementos.
 |	numTable2: punt. al numero de elementos.
 |	maxTable2: numero maximo de elementos (redimensionado).
 |	ovfTable2: indicativo de si la tabla admite overflow.
 |	clvSequenceTable2: indice de acceso por la clave SEQUENCE
 +--------------------------------------------------------------------*/
static int  posTable2 = 0;
static int *numTable2 = NULL;
static int  maxTable2 = MAX_TABLE2;
static int  ovfTable2 = 0;
static int *clvSequenceTable2 = NULL;


/*====================== PROCEDIMIENTO PUBLICO ======================
 * EXAM6_Directorio
 *	Notifica el directorio de donde  se debe cogen el fichero
 *	imagen de la base.
 *
 *    Parametros:
 *	- directorio: path del directorio.
 *====================================================================*/
void EXAM6_Directorio(char *directorio)
{
	char *nuevoPath;

/*--------------------------------------------------------------------
 | Se pide mem. dinamica para el path del fichero imagen.
 +--------------------------------------------------------------------*/
nuevoPath = (char *) malloc(strlen(directorio) + sizeof("EXAM6.ibd") + 1);
if (nuevoPath == NULL)
    ERR_SistOper(ERR_FATAL, FUNC_BDT, "EXAM6_Directorio", "malloc",
		 "(%d bytes) path de la base", lonMemoria);

/*--------------------------------------------------------------------
 | Se compone el nuevo path.
 +--------------------------------------------------------------------*/
sprintf(nuevoPath, "%s/EXAM6.ibd", directorio);
fichImagen = nuevoPath;
}

/*====================== PROCEDIMIENTO PUBLICO ======================
 * EXAM6_Dimensionar
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
void EXAM6_Resize(int tabla, int dimension)
{
switch(tabla) {
    case TBL_TABLE1:
	/*------------------------------------------------------------
	 | Si la dimension es mayor que la definida como maxima para la
	 | tabla TABLE1 no se acepta y se da un error.
	 +------------------------------------------------------------*/
	if (dimension > MAX_TABLE1)
	    ERR_Aplicacion(ERR_NO_FATAL, FUNC_BDT, "EXAM6_Resize",
	      "Resize %d of TABLE1 overflow (max: 100)",
	       dimension);
	else
	    maxTable1 = dimension;
	break;
    case TBL_TABLE2:
	/*------------------------------------------------------------
	 | Si la dimension es mayor que la definida como maxima para la
	 | tabla TABLE2 no se acepta y se da un error.
	 +------------------------------------------------------------*/
	if (dimension > MAX_TABLE2)
	    ERR_Aplicacion(ERR_NO_FATAL, FUNC_BDT, "EXAM6_Resize",
	      "Resize %d of TABLE2 overflow (max: 100)",
	       dimension);
	else
	    maxTable2 = dimension;
	break;
    }
}

/*====================== PROCEDIMIENTO PUBLICO ======================
 * EXAM6_Overflow
 *	Notifica si se quiere el desbordamiento activado o no.
 *	Si el codigo de tabla no es correcto no se hace nada.
 *
 *    Parametros:
 *	- tabla: identificador de la tabla.
 *	- desbord: valor 0/1 (NO/SI) de admision de desbordamiento.
 *====================================================================*/
void EXAM6_Overflow(int tabla, int desbord)
{
switch(tabla) {
    case TBL_TABLE1:
	ovfTable1 = desbord;
        break;
    case TBL_TABLE2:
	ovfTable2 = desbord;
        break;
    }
}

/*========================= FUNCION PUBLICA =========================
 * EXAM6_Open
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
int EXAM6_Open(int dejarAbierto)
{
TPC_HazPortable(0);
/*--------------------------------------------------------------------
 | Se calcula la memoria (segun el dimensionado) necesaria para la base.
 +--------------------------------------------------------------------*/
lonMemoria  = sizeof(int) * 2 ;
lonMemoria += maxTable1 * sizeof(int *) * 1;
lonMemoria += maxTable1 * TAM_TABLE1;
lonMemoria += maxTable2 * sizeof(int *) * 1;
lonMemoria += maxTable2 * TAM_TABLE2;

/*--------------------------------------------------------------------
 | Se pide mem. dinamica para la base da datos.
 +--------------------------------------------------------------------*/
pMemoria = (char *) malloc(lonMemoria);
if (pMemoria == NULL) {
    ERR_SistOper(ERR_NO_FATAL, FUNC_BDT, "EXAM6_Inicializar", "malloc",
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
 * EXAM6_Close
 *	Finaliza el acceso a la base de datos
 *====================================================================*/
int EXAM6_Close()
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
	ERR_SistOper(ERR_NO_FATAL, FUNC_BDT, "EXAM6_Close", "close",
			"file \"%s\"", fichImagen);
	return -1;
	}
return 0;
}

/*========================= FUNCION PUBLICA =========================
 * EXAM6_NumRecords
 *	Informa del numero de registros que tiene la tabla.
 *
 *    Parametros:
 *	- tabla: identificador de la tabla a dimensionar.
 *    Devuelve:
 *	 n: el numero de registros.
 *	-1: si la tabla no es correcta
 *====================================================================*/
int EXAM6_NumRecords(int tabla)
{
switch(tabla) {
    case TBL_TABLE1:
	return *numTable1;
    case TBL_TABLE2:
	return *numTable2;
    }
return -1;
}


/*========================= FUNCION PUBLICA =========================
 * EXAM6_InsTable1
 *	Inserta un nuevo elemento TABLE1, segun la clave
 *	principal, para lo cual la struct debe tener todos los
 *	campos de las claves con los valores actualizados.
 *
 *    Parametros:
 *	ptable1: punt. a la struct del elemento a insertar.
 *    Devuelve:
 *	 0: si la insercion se realiza correctamente.
 *	-1: si se produce un error.
 *====================================================================*/
int EXAM6_InsTable1(TypeTABLE1 *pTable1)
{
	char elemCod[LON_TABLE1];
	int  posicMem;
	int  posicFic;
	int  indElem;
	int  numElem;
	int  indClave;

/*--------------------------------------------------------------------
 | Se obtiene el numero actual de elementos.
 +--------------------------------------------------------------------*/
numElem = *numTable1;

/*--------------------------------------------------------------------
 | No debe existir otro elemento con igual valor en la clave principal
 +--------------------------------------------------------------------*/
if (BuscaElemento(pTable1, clvSequenceTable1,
		  numElem, (PF_COMPAR)CmpSequenceTable1) >= 0) {
    ERR_Aplicacion(ERR_NO_FATAL, FUNC_BDT, "EXAM6_InsTable1",
		 "key SEQUENCE of TABLE1 exits");
    return -1;
    }

/*--------------------------------------------------------------------
 | Si hay elementos libres, se toma para insercion el primero libre.
 | Si se sobrepasa el limite de elementos max100 solo se
 | acepta si se admite overflow. En ese caso se inserta sobre el ultimo
 | elemento que es el elemento 0 y que tiene la clave menor.
 +--------------------------------------------------------------------*/
if (numElem < maxTable1) {
    indClave = *numTable1;
    numElem++;
    }
else if (ovfTable1)
    indClave = 0;
else {
    ERR_Aplicacion(ERR_NO_FATAL, FUNC_BDT, "EXAM6_InsTable1",
	      "overflow on TABLE1 (%d)", maxTable1);
    return -1;
    }

/*--------------------------------------------------------------------
 | Se calculan las posiciones para la actualizacion.
 +--------------------------------------------------------------------*/
posicMem = clvSequenceTable1[indClave];
indElem  = IND_TABLE1(posicMem);
posicFic = ELE_TABLE1(indElem);


/*--------------------------------------------------------------------
 | Codifica la struct TypeTABLE1 sobre el elemento como OCUPADO
 +--------------------------------------------------------------------*/
CodTable1(elemCod, pTable1);
PONER_OCUPADO(elemCod);
if (ActElemento(elemCod, LON_TABLE1, posicFic,
		pTable1, TAM_TABLE1, posicMem) == -1) {
    return -1;
    }

*numTable1 = numElem;
OrdTable1();
return 0;
}

/*========================= FUNCION PUBLICA =========================
 * EXAM6_DelTable1
 *	Elimina un elemento TABLE1 ya existente, segun la
 *	clave Principal.
 *
 *    Parametros:
 *	pTable1: punt. a la struct del elemento a eliminar.
 *    Devuelve:
 *	 0: si la eliminacion se realiza correctamente.
 *	-1: si se produce un error.
 *====================================================================*/
int EXAM6_DelTable1(TypeTABLE1 *pTable1)
{
	char elemCod[LON_TABLE1];
	int  posicMem;
	int  posicFic;
	int  indElem;

/*--------------------------------------------------------------------
 | No debe existir otro elemento con igual valor en la clave principal
 +--------------------------------------------------------------------*/
indElem = BuscaElemento(pTable1, clvSequenceTable1,
		*numTable1, (PF_COMPAR)CmpSequenceTable1);
if (indElem == -1) {
    ERR_Aplicacion(ERR_NO_FATAL, FUNC_BDT, "EXAM6_EliTable1",
		 "key SEQUENCE of TABLE1 not exists");
    return -1;
    }

/*--------------------------------------------------------------------
 | Se calculan las posiciones para la actualizacion.
 +--------------------------------------------------------------------*/
posicMem = clvSequenceTable1[indElem];
indElem  = IND_TABLE1(posicMem);
posicFic = ELE_TABLE1(indElem);

/*--------------------------------------------------------------------
 | Codifica la struct TypeTABLE1 sobre el elemento como OCUPADO
 +--------------------------------------------------------------------*/
memset(elemCod, 0, LON_TABLE1);
PONER_LIBRE(elemCod);
if (ActElemento(elemCod, LON_TABLE1, posicFic,
		pTable1, TAM_TABLE1, posicMem) == -1)
    return -1;

PosicionLibre(posicMem, clvSequenceTable1, *numTable1);
(*numTable1)--;
return 0;
}

/*========================= FUNCION PUBLICA =========================
 * EXAM6_ModTable1
 *	Modifica un elemento TABLE1 ya existente, segun la
 *	clave principal, para lo cual la struct debe tener todos los
 *	campos con los valores actualizados.
 *
 *    Parametros:
 *	table1: punt. a la struct del elemento a eliminar.
 *    Devuelve:
 *	 0: si la modificacion se realiza correctamente.
 *	-1: si se produce un error.
 *====================================================================*/
int EXAM6_ModTable1(TypeTABLE1 *pTable1)
{
	char elemCod[LON_TABLE1];
	int  posicMem;
	int  posicFic;
	int  indElem;

/*--------------------------------------------------------------------
 | No debe existir otro elemento con igual valor en la clave principal
 +--------------------------------------------------------------------*/
indElem = BuscaElemento(pTable1, clvSequenceTable1,
		*numTable1, (PF_COMPAR)CmpSequenceTable1);
if (indElem == -1) {
    ERR_Aplicacion(ERR_NO_FATAL, FUNC_BDT, "EXAM6_ModTable1",
		   "key SEQUENCE of TABLE1 not exists");
    return -1;
    }

/*--------------------------------------------------------------------
 | Se calculan las posiciones para la actualizacion.
 +--------------------------------------------------------------------*/
posicMem = clvSequenceTable1[indElem];
indElem  = IND_TABLE1(posicMem);
posicFic = ELE_TABLE1(indElem);


/*--------------------------------------------------------------------
 | Codifica la struct TypeTABLE1 sobre el elemento como OCUPADO
 +--------------------------------------------------------------------*/
CodTable1(elemCod, pTable1);
PONER_OCUPADO(elemCod);
if (ActElemento(elemCod, LON_TABLE1, posicFic,
		pTable1, TAM_TABLE1, posicMem) == -1)
    return -1;

return 0;
}

/*========================= FUNCION PUBLICA =========================
 * EXAM6_Table1BySequence
 *	Busca un elemento TABLE1 , por la clave principal
 *	para lo cual la struct debe tener todos los campos de la
 *	clave con los valores actualizados.
 *
 *    Parametros:
 *	table1: punt. a la struct del elemento a buscar.
  *	mykey: campo MYKEY de busqueda.
  *    Devuelve:
 *	 0: si la modificacion se realiza correctamente.
 *	-1: si se produce un error.
 *	Busca un elemento
 *====================================================================*/
int EXAM6_Table1BySequence(TypeTABLE1 *pTable1,
				int            mykey)
{
	int  indElem;

/*--------------------------------------------------------------------
 | Se codifican los campos de la clave principal sobre el elemento.
 +--------------------------------------------------------------------*/
pTable1->mykey = mykey;

/*--------------------------------------------------------------------
 | Se busca el elemento accediendo por la clave principal y si no
 | existe se devuelve -1.
 +--------------------------------------------------------------------*/
indElem = BuscaElemento(pTable1, clvSequenceTable1,
	      *numTable1, (PF_COMPAR)CmpSequenceTable1);
if (indElem == -1)
    return -1;

/*--------------------------------------------------------------------
 | Si existe, se decodifica el elemento completo sobre la struct.
 +--------------------------------------------------------------------*/
memcpy(pTable1, DIR_MEM(clvSequenceTable1[indElem]), TAM_TABLE1);
return 0;
}

/*========================= FUNCION PUBLICA =========================
 * EXAM6_VolTable1
 *	Realiza un volcado de la tabla TABLE1 ordenado por
 *	la clave segun el parametro.
 *    Parametros:
 *	claveOrd: codigo de la clave para ordenar el volcado.
 *	pLista: puntero para la lista del volcado.
 *    Devuelve:
 *	n: el numero de elementos seleccionados.
 *	-1: si hay error en el acceso.
 *====================================================================*/
int EXAM6_VolTable1(int claveOrd, TypeTABLE1 **pLista)
{
	int i;
	TypeTABLE1 *pTable1;
	int *claveAcceso;

/*--------------------------------------------------------------------
 | Se recoge el array de acceso por la clave definida por "claveOrd"
 +--------------------------------------------------------------------*/
claveAcceso = ClaveTable1(claveOrd);
if (claveAcceso == NULL)
    return -1;

/*--------------------------------------------------------------------
 | Si la tabla esta vacia no se hace nada mas y se devuelve 0.
 +--------------------------------------------------------------------*/
if (*numTable1 == 0)
    return 0;

/*--------------------------------------------------------------------
 | Se pide memoria dinamica para almacenar los elementos del recorrido.
 +--------------------------------------------------------------------*/
pTable1 = (TypeTABLE1 *) malloc(*numTable1 * TAM_TABLE1);
if (pTable1 == NULL) {
    ERR_SistOper(ERR_NO_FATAL, FUNC_BDT, "EXAM6_VolTable1",
	     "malloc", "(%d bytes) para volcado de Table1", lonMemoria);
    return -1;
    }

*pLista = pTable1;

/*--------------------------------------------------------------------
 | Se copia la informacion de cada elemento de la lista segun la
 | ordenacion de la clave.
 +--------------------------------------------------------------------*/
for (i = 0; i < *numTable1; i++) {
    memcpy(pTable1, DIR_MEM(claveAcceso[i]), TAM_TABLE1);
    pTable1++;
    }

return i;
}


/*========================= FUNCION PUBLICA =========================
 * EXAM6_InsTable2
 *	Inserta un nuevo elemento TABLE2, segun la clave
 *	principal, para lo cual la struct debe tener todos los
 *	campos de las claves con los valores actualizados.
 *
 *    Parametros:
 *	ptable2: punt. a la struct del elemento a insertar.
 *    Devuelve:
 *	 0: si la insercion se realiza correctamente.
 *	-1: si se produce un error.
 *====================================================================*/
int EXAM6_InsTable2(TypeTABLE2 *pTable2)
{
	char elemCod[LON_TABLE2];
	int  posicMem;
	int  posicFic;
	int  indElem;
	int  numElem;
	int  indClave;

/*--------------------------------------------------------------------
 | Se obtiene el numero actual de elementos.
 +--------------------------------------------------------------------*/
numElem = *numTable2;

/*--------------------------------------------------------------------
 | No debe existir otro elemento con igual valor en la clave principal
 +--------------------------------------------------------------------*/
if (BuscaElemento(pTable2, clvSequenceTable2,
		  numElem, (PF_COMPAR)CmpSequenceTable2) >= 0) {
    ERR_Aplicacion(ERR_NO_FATAL, FUNC_BDT, "EXAM6_InsTable2",
		 "key SEQUENCE of TABLE2 exits");
    return -1;
    }

/*--------------------------------------------------------------------
 | Si hay elementos libres, se toma para insercion el primero libre.
 | Si se sobrepasa el limite de elementos max100 solo se
 | acepta si se admite overflow. En ese caso se inserta sobre el ultimo
 | elemento que es el elemento 0 y que tiene la clave menor.
 +--------------------------------------------------------------------*/
if (numElem < maxTable2) {
    indClave = *numTable2;
    numElem++;
    }
else if (ovfTable2)
    indClave = 0;
else {
    ERR_Aplicacion(ERR_NO_FATAL, FUNC_BDT, "EXAM6_InsTable2",
	      "overflow on TABLE2 (%d)", maxTable2);
    return -1;
    }

/*--------------------------------------------------------------------
 | Se calculan las posiciones para la actualizacion.
 +--------------------------------------------------------------------*/
posicMem = clvSequenceTable2[indClave];
indElem  = IND_TABLE2(posicMem);
posicFic = ELE_TABLE2(indElem);


/*--------------------------------------------------------------------
 | Codifica la struct TypeTABLE2 sobre el elemento como OCUPADO
 +--------------------------------------------------------------------*/
CodTable2(elemCod, pTable2);
PONER_OCUPADO(elemCod);
if (ActElemento(elemCod, LON_TABLE2, posicFic,
		pTable2, TAM_TABLE2, posicMem) == -1) {
    return -1;
    }

*numTable2 = numElem;
OrdTable2();
return 0;
}

/*========================= FUNCION PUBLICA =========================
 * EXAM6_DelTable2
 *	Elimina un elemento TABLE2 ya existente, segun la
 *	clave Principal.
 *
 *    Parametros:
 *	pTable2: punt. a la struct del elemento a eliminar.
 *    Devuelve:
 *	 0: si la eliminacion se realiza correctamente.
 *	-1: si se produce un error.
 *====================================================================*/
int EXAM6_DelTable2(TypeTABLE2 *pTable2)
{
	char elemCod[LON_TABLE2];
	int  posicMem;
	int  posicFic;
	int  indElem;

/*--------------------------------------------------------------------
 | No debe existir otro elemento con igual valor en la clave principal
 +--------------------------------------------------------------------*/
indElem = BuscaElemento(pTable2, clvSequenceTable2,
		*numTable2, (PF_COMPAR)CmpSequenceTable2);
if (indElem == -1) {
    ERR_Aplicacion(ERR_NO_FATAL, FUNC_BDT, "EXAM6_EliTable2",
		 "key SEQUENCE of TABLE2 not exists");
    return -1;
    }

/*--------------------------------------------------------------------
 | Se calculan las posiciones para la actualizacion.
 +--------------------------------------------------------------------*/
posicMem = clvSequenceTable2[indElem];
indElem  = IND_TABLE2(posicMem);
posicFic = ELE_TABLE2(indElem);

/*--------------------------------------------------------------------
 | Codifica la struct TypeTABLE2 sobre el elemento como OCUPADO
 +--------------------------------------------------------------------*/
memset(elemCod, 0, LON_TABLE2);
PONER_LIBRE(elemCod);
if (ActElemento(elemCod, LON_TABLE2, posicFic,
		pTable2, TAM_TABLE2, posicMem) == -1)
    return -1;

PosicionLibre(posicMem, clvSequenceTable2, *numTable2);
(*numTable2)--;
return 0;
}

/*========================= FUNCION PUBLICA =========================
 * EXAM6_ModTable2
 *	Modifica un elemento TABLE2 ya existente, segun la
 *	clave principal, para lo cual la struct debe tener todos los
 *	campos con los valores actualizados.
 *
 *    Parametros:
 *	table2: punt. a la struct del elemento a eliminar.
 *    Devuelve:
 *	 0: si la modificacion se realiza correctamente.
 *	-1: si se produce un error.
 *====================================================================*/
int EXAM6_ModTable2(TypeTABLE2 *pTable2)
{
	char elemCod[LON_TABLE2];
	int  posicMem;
	int  posicFic;
	int  indElem;

/*--------------------------------------------------------------------
 | No debe existir otro elemento con igual valor en la clave principal
 +--------------------------------------------------------------------*/
indElem = BuscaElemento(pTable2, clvSequenceTable2,
		*numTable2, (PF_COMPAR)CmpSequenceTable2);
if (indElem == -1) {
    ERR_Aplicacion(ERR_NO_FATAL, FUNC_BDT, "EXAM6_ModTable2",
		   "key SEQUENCE of TABLE2 not exists");
    return -1;
    }

/*--------------------------------------------------------------------
 | Se calculan las posiciones para la actualizacion.
 +--------------------------------------------------------------------*/
posicMem = clvSequenceTable2[indElem];
indElem  = IND_TABLE2(posicMem);
posicFic = ELE_TABLE2(indElem);


/*--------------------------------------------------------------------
 | Codifica la struct TypeTABLE2 sobre el elemento como OCUPADO
 +--------------------------------------------------------------------*/
CodTable2(elemCod, pTable2);
PONER_OCUPADO(elemCod);
if (ActElemento(elemCod, LON_TABLE2, posicFic,
		pTable2, TAM_TABLE2, posicMem) == -1)
    return -1;

return 0;
}

/*========================= FUNCION PUBLICA =========================
 * EXAM6_Table2BySequence
 *	Busca un elemento TABLE2 , por la clave principal
 *	para lo cual la struct debe tener todos los campos de la
 *	clave con los valores actualizados.
 *
 *    Parametros:
 *	table2: punt. a la struct del elemento a buscar.
  *	mykey: campo MYKEY de busqueda.
  *    Devuelve:
 *	 0: si la modificacion se realiza correctamente.
 *	-1: si se produce un error.
 *	Busca un elemento
 *====================================================================*/
int EXAM6_Table2BySequence(TypeTABLE2 *pTable2,
				int            mykey)
{
	int  indElem;

/*--------------------------------------------------------------------
 | Se codifican los campos de la clave principal sobre el elemento.
 +--------------------------------------------------------------------*/
pTable2->mykey = mykey;

/*--------------------------------------------------------------------
 | Se busca el elemento accediendo por la clave principal y si no
 | existe se devuelve -1.
 +--------------------------------------------------------------------*/
indElem = BuscaElemento(pTable2, clvSequenceTable2,
	      *numTable2, (PF_COMPAR)CmpSequenceTable2);
if (indElem == -1)
    return -1;

/*--------------------------------------------------------------------
 | Si existe, se decodifica el elemento completo sobre la struct.
 +--------------------------------------------------------------------*/
memcpy(pTable2, DIR_MEM(clvSequenceTable2[indElem]), TAM_TABLE2);
return 0;
}

/*========================= FUNCION PUBLICA =========================
 * EXAM6_VolTable2
 *	Realiza un volcado de la tabla TABLE2 ordenado por
 *	la clave segun el parametro.
 *    Parametros:
 *	claveOrd: codigo de la clave para ordenar el volcado.
 *	pLista: puntero para la lista del volcado.
 *    Devuelve:
 *	n: el numero de elementos seleccionados.
 *	-1: si hay error en el acceso.
 *====================================================================*/
int EXAM6_VolTable2(int claveOrd, TypeTABLE2 **pLista)
{
	int i;
	TypeTABLE2 *pTable2;
	int *claveAcceso;

/*--------------------------------------------------------------------
 | Se recoge el array de acceso por la clave definida por "claveOrd"
 +--------------------------------------------------------------------*/
claveAcceso = ClaveTable2(claveOrd);
if (claveAcceso == NULL)
    return -1;

/*--------------------------------------------------------------------
 | Si la tabla esta vacia no se hace nada mas y se devuelve 0.
 +--------------------------------------------------------------------*/
if (*numTable2 == 0)
    return 0;

/*--------------------------------------------------------------------
 | Se pide memoria dinamica para almacenar los elementos del recorrido.
 +--------------------------------------------------------------------*/
pTable2 = (TypeTABLE2 *) malloc(*numTable2 * TAM_TABLE2);
if (pTable2 == NULL) {
    ERR_SistOper(ERR_NO_FATAL, FUNC_BDT, "EXAM6_VolTable2",
	     "malloc", "(%d bytes) para volcado de Table2", lonMemoria);
    return -1;
    }

*pLista = pTable2;

/*--------------------------------------------------------------------
 | Se copia la informacion de cada elemento de la lista segun la
 | ordenacion de la clave.
 +--------------------------------------------------------------------*/
for (i = 0; i < *numTable2; i++) {
    memcpy(pTable2, DIR_MEM(claveAcceso[i]), TAM_TABLE2);
    pTable2++;
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
    ERR_Aplicacion(ERR_NO_FATAL, FUNC_BDT, "EXAM6: IniInformacion",
		   "fichero \"%s\" no existente", fichImagen);
    return -1;
    }

/*--------------------------------------------------------------------
 | Se mira si el fichero imagen de Config. tiene permiso de lectura
 +--------------------------------------------------------------------*/
if (access(fichImagen, R_OK) == -1) {
    ERR_Aplicacion(ERR_NO_FATAL, FUNC_BDT, "EXAM6: IniInformacion",
		   "file \"%s\" can't read", fichImagen);
    return -1;
    }

/*--------------------------------------------------------------------
 | Se mira si el fichero imagen de Config. tiene permiso de escritura
 +--------------------------------------------------------------------*/
if (access(fichImagen, W_OK) == -1) {
    ERR_Aplicacion(ERR_NO_FATAL, FUNC_BDT, "EXAM6: IniInformacion",
		   "file \"%s\" can't write", fichImagen);
    return -1;
    }

/*--------------------------------------------------------------------
 | Se abre el fichero imagen de la base de datos
 +--------------------------------------------------------------------*/
fdFBD = open(fichImagen, O_RDWR | O_EXCL);
if (fdFBD == -1) {
    ERR_SistOper(ERR_NO_FATAL, FUNC_BDT, "EXAM6: IniInformacion", "open",
		 "file \"%s\" (for read/write))", fichImagen);
    return -1;
    }
 
{
        char * bufElems;
	TypeTABLE1 *pTable1;

/*--------------------------------------------------------------------
 | Se pide mem. dinamica para el leer el fichero imagen.
 +--------------------------------------------------------------------*/
bufElems = (char *) malloc( BUF_TABLE1 + 1);
if (bufElems == NULL)
    ERR_SistOper(ERR_FATAL, FUNC_BDT, "EXAM6 IniInformacion", "malloc",
		 "(%d bytes) path of base", BUF_TABLE1);

/*--------------------------------------------------------------------
 | Se posiciona en la posicion  de la tabla TABLE1 en el fichero
 +--------------------------------------------------------------------*/
if (lseek(fdFBD, POS_TABLE1, SEEK_SET) == -1) {
    ERR_SistOper(ERR_NO_FATAL, FUNC_BDT, "EXAM6: IniInformacion", "lseek",
		"file \"%s\" (position POS_TABLE1)", fichImagen);
    return -1;
    }

/*--------------------------------------------------------------------
 | Se leen todos los elementos de la tabla desde el fichero.
 +--------------------------------------------------------------------*/
if (read(fdFBD, bufElems, BUF_TABLE1) != BUF_TABLE1) {
    ERR_SistOper(ERR_NO_FATAL, FUNC_BDT, "EXAM6: IniInformacion", "read",
		 "file \"%s\" (BUF_TABLE1 bytes)", fichImagen);
    return -1;
    }

/*--------------------------------------------------------------------
 | Se cuenta el numero de elementos que tiene el fichero y se comprueba
 | que el dimensionado es suficiente.
 +--------------------------------------------------------------------*/
pElem = bufElems;
*numTable1 = 0;
for (i = 0; i < MAX_TABLE1; i++, pElem += LON_TABLE1)
    if (ESTA_OCUPADO(pElem))
	(*numTable1)++;

if (*numTable1 > maxTable1) {
    ERR_Aplicacion(ERR_NO_FATAL, FUNC_BDT, "EXAM6: IniInformacion",
    	"size  %d of TABLE1 great than max dimension %d",
	*numTable1, maxTable1);
    return -1;
    }

/*--------------------------------------------------------------------
 | Se extraen los registros ocupados y se inicializan las posiciones
 | de cada clave.
 +--------------------------------------------------------------------*/
indOcu = 0;
indLib = *numTable1;
pElem  = bufElems;
pTable1 = (TypeTABLE1 *) DIR_MEM(posTable1);
for (i = 0; i < maxTable1; i++) {
    if (ESTA_OCUPADO(pElem)) {
	DecTable1(pElem, pTable1);
	indAct = indOcu;
	indOcu++;
	}
    else {
	indAct = indLib;
	indLib++;
	}
    clvSequenceTable1[indAct] = POS_MEM(pTable1);
    pElem += LON_TABLE1;
    pTable1++;
    }
    free(bufElems);
}

/*--------------------------------------------------------------------
 | Se ordenan los array de posiciones de las claves de la tabla.
 +--------------------------------------------------------------------*/
OrdTable1();

{
        char * bufElems;
	TypeTABLE2 *pTable2;

/*--------------------------------------------------------------------
 | Se pide mem. dinamica para el leer el fichero imagen.
 +--------------------------------------------------------------------*/
bufElems = (char *) malloc( BUF_TABLE2 + 1);
if (bufElems == NULL)
    ERR_SistOper(ERR_FATAL, FUNC_BDT, "EXAM6 IniInformacion", "malloc",
		 "(%d bytes) path of base", BUF_TABLE2);

/*--------------------------------------------------------------------
 | Se posiciona en la posicion  de la tabla TABLE2 en el fichero
 +--------------------------------------------------------------------*/
if (lseek(fdFBD, POS_TABLE2, SEEK_SET) == -1) {
    ERR_SistOper(ERR_NO_FATAL, FUNC_BDT, "EXAM6: IniInformacion", "lseek",
		"file \"%s\" (position POS_TABLE2)", fichImagen);
    return -1;
    }

/*--------------------------------------------------------------------
 | Se leen todos los elementos de la tabla desde el fichero.
 +--------------------------------------------------------------------*/
if (read(fdFBD, bufElems, BUF_TABLE2) != BUF_TABLE2) {
    ERR_SistOper(ERR_NO_FATAL, FUNC_BDT, "EXAM6: IniInformacion", "read",
		 "file \"%s\" (BUF_TABLE2 bytes)", fichImagen);
    return -1;
    }

/*--------------------------------------------------------------------
 | Se cuenta el numero de elementos que tiene el fichero y se comprueba
 | que el dimensionado es suficiente.
 +--------------------------------------------------------------------*/
pElem = bufElems;
*numTable2 = 0;
for (i = 0; i < MAX_TABLE2; i++, pElem += LON_TABLE2)
    if (ESTA_OCUPADO(pElem))
	(*numTable2)++;

if (*numTable2 > maxTable2) {
    ERR_Aplicacion(ERR_NO_FATAL, FUNC_BDT, "EXAM6: IniInformacion",
    	"size  %d of TABLE2 great than max dimension %d",
	*numTable2, maxTable2);
    return -1;
    }

/*--------------------------------------------------------------------
 | Se extraen los registros ocupados y se inicializan las posiciones
 | de cada clave.
 +--------------------------------------------------------------------*/
indOcu = 0;
indLib = *numTable2;
pElem  = bufElems;
pTable2 = (TypeTABLE2 *) DIR_MEM(posTable2);
for (i = 0; i < maxTable2; i++) {
    if (ESTA_OCUPADO(pElem)) {
	DecTable2(pElem, pTable2);
	indAct = indOcu;
	indOcu++;
	}
    else {
	indAct = indLib;
	indLib++;
	}
    clvSequenceTable2[indAct] = POS_MEM(pTable2);
    pElem += LON_TABLE2;
    pTable2++;
    }
    free(bufElems);
}

/*--------------------------------------------------------------------
 | Se ordenan los array de posiciones de las claves de la tabla.
 +--------------------------------------------------------------------*/
OrdTable2();


/*--------------------------------------------------------------------
 | Si no se deja abierto, se cierra el fichero imagen de la base.
 +--------------------------------------------------------------------*/
if (! fichAbierto)
    if (close(fdFBD) == -1) {
	ERR_SistOper(ERR_NO_FATAL, FUNC_BDT, "EXAM6: IniInformacion", "close",
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
 | Se inicializan los punteros de acceso a la tabla TABLE1:
 *	numTable1: punt. al numero de elementos.
 *	clvSequenceTable1: array de acceso por la clave Sequence
 *	posTable1: informacion de los elementos.
 +--------------------------------------------------------------------*/
numTable1 = (int *) pActual;
pActual += sizeof(int);
clvSequenceTable1 = (int *) pActual;
pActual += (maxTable1 * sizeof(int *));
posTable1 = POS_MEM(pActual);
pActual += (maxTable1 * TAM_TABLE1);

/*--------------------------------------------------------------------
 | Se inicializan los punteros de acceso a la tabla TABLE2:
 *	numTable2: punt. al numero de elementos.
 *	clvSequenceTable2: array de acceso por la clave Sequence
 *	posTable2: informacion de los elementos.
 +--------------------------------------------------------------------*/
numTable2 = (int *) pActual;
pActual += sizeof(int);
clvSequenceTable2 = (int *) pActual;
pActual += (maxTable2 * sizeof(int *));
posTable2 = POS_MEM(pActual);
pActual += (maxTable2 * TAM_TABLE2);

}


/*====================== PROCEDIMIENTO PRIVADO ======================
 * CodTable1
 *	Codifica un TypeTABLE1 sobre un elemento Table1.
 *    Parametros:
 *      elem: buffer del elemento sobre el que codificar.
 *	table1: struct a realizar la codificacion.
 *====================================================================*/
static void CodTable1(char *elem, TypeTABLE1 *pTable1)
{
TPC_CodInt(&pTable1->mykey, 1, elem + 1);
TPC_CodChar(&pTable1->caracter, 1, elem + 5);
}

/*====================== PROCEDIMIENTO PRIVADO ======================
 * DecTable1
 *	Decodifica un elemento Table1 sobre un TypeTABLE1
 *    Parametros:
 *      elem: buffer del elemento a decodificar.
 *	table1: struct sobre la que realizar la decodificacion.
 *====================================================================*/
static void DecTable1(char *elem, TypeTABLE1 *pTable1)
{
TPC_DecInt(&pTable1->mykey, 1, elem + 1);
TPC_DecChar(&pTable1->caracter, 1, elem + 5);
}

/*========================= FUNCION PRIVADA =========================
 * OrdTable1
 *	Ordena los punt. de acceso a la tabla TABLE1, por:
 *		- SEQUENCE
 *====================================================================*/
static void OrdTable1()
{
/*--------------------------------------------------------------------
 | Ordena el array de acceso a la tabla TABLE1 por las claves:
 |	- SEQUENCE
 +--------------------------------------------------------------------*/
qsort(clvSequenceTable1, *numTable1,
      sizeof(int), (PF_COMPAR)CmpDesSequenceTable1);
}

/*========================= FUNCION PRIVADA =========================
 * ClaveTable1
 *	Devuelve el array de acceso correspondiente el codigo de
 *	una determinada clave de la tabla TABLE1.
 *
 *    Parametros:
 *	codClave: codigo de la clave.
 *    Devuelve:
 *	el array de acceso de la clave: si el codigo es correcto.
 *	NULL: si el codigo es erroneo.
 *====================================================================*/
static int *ClaveTable1(const int codClave)
{
switch(codClave) {
    case KEY_SEQUENCE_TABLE1:
	return clvSequenceTable1;
    }
ERR_Aplicacion(ERR_NO_FATAL, FUNC_BDT, "EXAM6: KeyTable1",
	"key code (%d) on table TABLE1 wrong", codClave);
return NULL;
}

/*========================= FUNCION PRIVADA =========================
 * CmpDesSequenceTable1
 *	Compara la clave SEQUENCE de dos elementos de la tabla
 *	TABLE1 segun sus posiciones en la memoria.
 *    Parametros:
 *	pPosic1: punt. a la posicion del elemento 1 a comparar la clave.
 *	pPosic2: punt. a la posicion del elemento 2 a comparar la clave.
 *    Devuelve:
 *      num > 0: si la clave del pPosic1 es mayor que el del pPosic2.
 *      0:	 si la clave del pPosic1 es igual que el del pPosic2.
 *      num < 0: si la clave del pPosic1 es menor que el del pPosic2.
 *====================================================================*/
static int CmpDesSequenceTable1(const int *pPosic1, const int *pPosic2)
{
return CmpSequenceTable1((TypeTABLE1 *) DIR_MEM(*pPosic1),
			(TypeTABLE1 *) DIR_MEM(*pPosic2));
}

/*========================= FUNCION PRIVADA =========================
 * CmpSequenceTable1
 *	Compara la clave SEQUENCE de dos elementos de la tabla
 *	TABLE1.
 *    Parametros:
 *	table1:  punt. al elemento 1 a comparar la clave.
 *	table12: punt. al elemento 2 a comparar la clave.
 *    Devuelve:
 *      num > 0: si la clave de table1 es mayor que table12.
 *      0:       si la clave de table1 es igual que table12.
 *      num < 0: si la clave de table1 es menor que table12.
 *====================================================================*/
static int CmpSequenceTable1(const TypeTABLE1 *pTable1,
				const TypeTABLE1 *pTable12)
{
	int result;

/*--------------------------------------------------------------------
 | Si el campo MYKEY del elemento 1 y 2 es distinto, se devuelve
 | el resultado de la comparacion.
 +--------------------------------------------------------------------*/
if ((result = CMP_VAL(pTable1->mykey, pTable12->mykey)) != 0) 
    return result;

return 0;
}

/*====================== PROCEDIMIENTO PRIVADO ======================
 * CodTable2
 *	Codifica un TypeTABLE2 sobre un elemento Table2.
 *    Parametros:
 *      elem: buffer del elemento sobre el que codificar.
 *	table2: struct a realizar la codificacion.
 *====================================================================*/
static void CodTable2(char *elem, TypeTABLE2 *pTable2)
{
TPC_CodInt(&pTable2->mykey, 1, elem + 1);
TPC_CodChar(&pTable2->caracter, 1, elem + 5);
TPC_CodUnsChar(&pTable2->ucaracter, 1, elem + 6);
TPC_CodShort(&pTable2->corto, 1, elem + 7);
TPC_CodUnsShort(&pTable2->ucorto, 1, elem + 9);
TPC_CodInt(&pTable2->entero, 1, elem + 11);
TPC_CodUnsInt(&pTable2->uentero, 1, elem + 15);
TPC_CodLong(&pTable2->lentero, 1, elem + 19);
TPC_CodUnsLong(&pTable2->ulentero, 1, elem + 27);
TPC_CodFloat(&pTable2->flotante, 1, elem + 35);
TPC_CodDouble(&pTable2->doble, 1, elem + 39);
TPC_CodShort(pTable2->acortos, 5, elem + 47);
TPC_CodUnsShort(pTable2->aucortos, 5, elem + 57);
TPC_CodInt(pTable2->aenteros, 5, elem + 67);
TPC_CodUnsInt(pTable2->auenteros, 5, elem + 87);
TPC_CodLong(pTable2->alenteros, 5, elem + 107);
TPC_CodUnsLong(pTable2->aulenteros, 5, elem + 147);
TPC_CodFloat(pTable2->aflotantes, 5, elem + 187);
TPC_CodDouble(pTable2->adoble, 5, elem + 207);
TPC_CodChar(pTable2->achar, 50, elem + 247);
}

/*====================== PROCEDIMIENTO PRIVADO ======================
 * DecTable2
 *	Decodifica un elemento Table2 sobre un TypeTABLE2
 *    Parametros:
 *      elem: buffer del elemento a decodificar.
 *	table2: struct sobre la que realizar la decodificacion.
 *====================================================================*/
static void DecTable2(char *elem, TypeTABLE2 *pTable2)
{
TPC_DecInt(&pTable2->mykey, 1, elem + 1);
TPC_DecChar(&pTable2->caracter, 1, elem + 5);
TPC_DecUnsChar(&pTable2->ucaracter, 1, elem + 6);
TPC_DecShort(&pTable2->corto, 1, elem + 7);
TPC_DecUnsShort(&pTable2->ucorto, 1, elem + 9);
TPC_DecInt(&pTable2->entero, 1, elem + 11);
TPC_DecUnsInt(&pTable2->uentero, 1, elem + 15);
TPC_DecLong(&pTable2->lentero, 1, elem + 19);
TPC_DecUnsLong(&pTable2->ulentero, 1, elem + 27);
TPC_DecFloat(&pTable2->flotante, 1, elem + 35);
TPC_DecDouble(&pTable2->doble, 1, elem + 39);
TPC_DecShort(pTable2->acortos, 5, elem + 47);
TPC_DecUnsShort(pTable2->aucortos, 5, elem + 57);
TPC_DecInt(pTable2->aenteros, 5, elem + 67);
TPC_DecUnsInt(pTable2->auenteros, 5, elem + 87);
TPC_DecLong(pTable2->alenteros, 5, elem + 107);
TPC_DecUnsLong(pTable2->aulenteros, 5, elem + 147);
TPC_DecFloat(pTable2->aflotantes, 5, elem + 187);
TPC_DecDouble(pTable2->adoble, 5, elem + 207);
TPC_DecChar(pTable2->achar, 50, elem + 247);
}

/*========================= FUNCION PRIVADA =========================
 * OrdTable2
 *	Ordena los punt. de acceso a la tabla TABLE2, por:
 *		- SEQUENCE
 *====================================================================*/
static void OrdTable2()
{
/*--------------------------------------------------------------------
 | Ordena el array de acceso a la tabla TABLE2 por las claves:
 |	- SEQUENCE
 +--------------------------------------------------------------------*/
qsort(clvSequenceTable2, *numTable2,
      sizeof(int), (PF_COMPAR)CmpDesSequenceTable2);
}

/*========================= FUNCION PRIVADA =========================
 * ClaveTable2
 *	Devuelve el array de acceso correspondiente el codigo de
 *	una determinada clave de la tabla TABLE2.
 *
 *    Parametros:
 *	codClave: codigo de la clave.
 *    Devuelve:
 *	el array de acceso de la clave: si el codigo es correcto.
 *	NULL: si el codigo es erroneo.
 *====================================================================*/
static int *ClaveTable2(const int codClave)
{
switch(codClave) {
    case KEY_SEQUENCE_TABLE2:
	return clvSequenceTable2;
    }
ERR_Aplicacion(ERR_NO_FATAL, FUNC_BDT, "EXAM6: KeyTable2",
	"key code (%d) on table TABLE2 wrong", codClave);
return NULL;
}

/*========================= FUNCION PRIVADA =========================
 * CmpDesSequenceTable2
 *	Compara la clave SEQUENCE de dos elementos de la tabla
 *	TABLE2 segun sus posiciones en la memoria.
 *    Parametros:
 *	pPosic1: punt. a la posicion del elemento 1 a comparar la clave.
 *	pPosic2: punt. a la posicion del elemento 2 a comparar la clave.
 *    Devuelve:
 *      num > 0: si la clave del pPosic1 es mayor que el del pPosic2.
 *      0:	 si la clave del pPosic1 es igual que el del pPosic2.
 *      num < 0: si la clave del pPosic1 es menor que el del pPosic2.
 *====================================================================*/
static int CmpDesSequenceTable2(const int *pPosic1, const int *pPosic2)
{
return CmpSequenceTable2((TypeTABLE2 *) DIR_MEM(*pPosic1),
			(TypeTABLE2 *) DIR_MEM(*pPosic2));
}

/*========================= FUNCION PRIVADA =========================
 * CmpSequenceTable2
 *	Compara la clave SEQUENCE de dos elementos de la tabla
 *	TABLE2.
 *    Parametros:
 *	table2:  punt. al elemento 1 a comparar la clave.
 *	table22: punt. al elemento 2 a comparar la clave.
 *    Devuelve:
 *      num > 0: si la clave de table2 es mayor que table22.
 *      0:       si la clave de table2 es igual que table22.
 *      num < 0: si la clave de table2 es menor que table22.
 *====================================================================*/
static int CmpSequenceTable2(const TypeTABLE2 *pTable2,
				const TypeTABLE2 *pTable22)
{
	int result;

/*--------------------------------------------------------------------
 | Si el campo MYKEY del elemento 1 y 2 es distinto, se devuelve
 | el resultado de la comparacion.
 +--------------------------------------------------------------------*/
if ((result = CMP_VAL(pTable2->mykey, pTable22->mykey)) != 0) 
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
	ERR_SistOper(ERR_NO_FATAL, FUNC_BDT, "EXAM6: ActElemento", "open",
		 "file \"%s\" (for read/write))", fichImagen);
	return -1;
	}
    }
 
/*--------------------------------------------------------------------
 | Se prepara la escritura sobre la posicion del elemento.
 +--------------------------------------------------------------------*/
if (lseek(fdFBD, (off_t)posFic, SEEK_SET) == -1) {
    ERR_SistOper(ERR_NO_FATAL, FUNC_BDT, "EXAM6: ActElemento", "lseek",
    		 "file \"%s\" (position %ld)", fichImagen, posFic);
    return -1;
    }

/*--------------------------------------------------------------------
 | Se escribe el elemento sobre el fichero.
 +--------------------------------------------------------------------*/
if (write(fdFBD, (char *)eleFic, lonFic) != lonFic) {
    ERR_SistOper(ERR_NO_FATAL, FUNC_BDT, "EXAM6: ActElemento", "write",
    		 "file \"%s\" (%d bytes)", fichImagen, lonFic);
    return -1;
    }

/*--------------------------------------------------------------------
 | Si el fichero no se deja abierto, se cierra.
 +--------------------------------------------------------------------*/
if (! fichAbierto)
    if (close(fdFBD) == -1) {
	ERR_SistOper(ERR_NO_FATAL, FUNC_BDT, "EXAM6: ActElemento", "close",
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
