/* $STR: strFuenteCompa */
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
#include <exam5.h>
 
/*--------------------------------------------------------------------
 | PF_COMPAR: punt. a funcion comparac. para "qsort" y "BuscaElemento"
 +--------------------------------------------------------------------*/
typedef int (*PF_COMPAR)(const void *, const void *);

/*--------------------------------------------------------------------
 | FUNC_BDT: nombre de la funcion para los errores.
 +--------------------------------------------------------------------*/
#define FUNC_BDT	"Simple example3"
 
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
#define PONER_LIBRE(pE)		pE[0] = 'L'
#define PONER_OCUPADO(pE)	pE[0] = 'O'
#define ESTA_LIBRE(pE)		(pE[0] == 'L')
#define ESTA_OCUPADO(pE)	(pE[0] == 'O')

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
#define IND_TABLE1(pos)	((pos - *pPosTable1) / TAM_TABLE1)

#define LON_TABLE1	56
#define POS_TABLE1	540L
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
#define IND_TABLE2(pos)	((pos - *pPosTable2) / TAM_TABLE2)

#define LON_TABLE2	59
#define POS_TABLE2	56540L
#define BUF_TABLE2	(MAX_TABLE2 * LON_TABLE2)
#define ELE_TABLE2(i)	(long)(POS_TABLE2+(LON_TABLE2*i))
/*--------------------------------------------------------------------
 | TAM_TABLE3: tamano de la struct TypeTABLE3
 | IND_TABLE3: indice de un elemento segun una posicion en memoria.
 |
 | LON_TABLE3: longitud del elemento TABLE3 en el fichero
 | POS_TABLE3: posicion de TABLE3 en el fichero
 | BUF_TABLE3: tamano de TABLE3 en el fichero.
 | ELE_TABLE3: posicion de un elemento en el fichero segun el indice
 +--------------------------------------------------------------------*/
#define TAM_TABLE3	(sizeof(TypeTABLE3))
#define IND_TABLE3(pos)	((pos - *pPosTable3) / TAM_TABLE3)

#define LON_TABLE3	59
#define POS_TABLE3	115540L
#define BUF_TABLE3	(MAX_TABLE3 * LON_TABLE3)
#define ELE_TABLE3(i)	(long)(POS_TABLE3+(LON_TABLE3*i))

/*--------------------------------------------------------------------
 | numSEMAF:   numero de semaforos para el acceso a Configuracion.
 | SEMAF_LECT: indice del semaforo de acceso para lectura.
 | SEMAF_ESCR: indice del semaforo de acceso para escritura.
 | ID_EXAM5:     identificador para la clave de M. Compartida y Semaforos.
 | DMC_ERR:    direccion de M. Compartida erronea.
 +--------------------------------------------------------------------*/
#define numSEMAF	2
#define SEMAF_LECT	0
#define SEMAF_ESCR	1
#define ID_EXAM5		'X'
#define DMC_ERR		((char *)-1)
 
#ifndef SEMUN
typedef union {
	int               val;
	struct semid_ds * buf;
	unsigned short  * array;
	} SEMUN;
#endif
 
/*====================================================================
 *		FUNCIONES DE ACCESO A REGION CRITICA
 *====================================================================*/
static void AccLectura();
static void SalLectura();
static void AccEscritura();
static void SalEscritura();

/*====================================================================
 *		FUNCIONES DE INICIALIZACION DE LA BASE
 *====================================================================*/
static void CalcularMemoria();
static void IniPunteros(int);
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
static int  CmpPairTable1(const TypeTABLE1 *,
				const TypeTABLE1 *);
static int  CmpDesPairTable1(const int *, const int *); 

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
 *	FUNCIONES DE TRATAMIENTO DE LA TABLA TABLE3
 *====================================================================*/
static void CodTable3(char *, TypeTABLE3 *);
static void DecTable3(char *, TypeTABLE3 *);
static void OrdTable3();
static int *ClaveTable3(const int);
static int  CmpSequenceTable3(const TypeTABLE3 *,
				const TypeTABLE3 *);
static int  CmpDesSequenceTable3(const int *, const int *); 

/*====================================================================
 *		FUNCIONES GENERALES A LA BASE
 *====================================================================*/
static int BuscaElemento(void *, int *, int, PF_COMPAR);
static int ActElemento(char *eleFic, int lonFic, int posFic,
		       void *eleMem, int lonMem, int posMem);
static void PosicionLibre(int, int *, int);

/*====================================================================
 *		 VARIABLES LOCALES DE LA BASE Simple example3
 *====================================================================*/
/*--------------------------------------------------------------------
 | fichImagen:	fichero imagen de la base de datos
 | fdFBD:	descriptor del fichero base de datos
 | fichAbierto:	indicativo de si el fichero permanece abierto o no.
 | pMemoria:	memoria (compartida o dinamica) para la base.
 | tamMemoria:	tamano de la memoria en bytes para la base.
 +--------------------------------------------------------------------*/
static char *fichImagen  = "EXAM5.ibd";
static int   fdFBD	 = -1;
static int   fichAbierto = 1;
static char *pMemoria	 = NULL;
static int   lonMemoria	 = 0;

/*--------------------------------------------------------------------
 | identSem: identificador de los semaforos de acceso.
 | identMC: identificador de la memoria compartida.
 | basePrivada: indica si la base se ha creado como privada o compartida.
 | vectorSig: vector de senales para el bloque y desbloqueo de estas.
 +--------------------------------------------------------------------*/
static int   identSem	 = -1;
static int   identMC	 = -1;
static int   basePrivada = 0;
static sigset_t vectorSig;

/*--------------------------------------------------------------------
 | Variables de la tabla TABLE1 para datos compartidos:
 |	pPosTable1: punt. a la posicion de los elementos.
 |	pNumTable1: punt. al numero de elementos.
 |	pMaxTable1: punt. al numero maximo de elementos.
 |	pOvfTable1: punt. al indic. de si la tabla admite overflow.
 |	clvSequenceTable1: indice de acceso por la clave SEQUENCE
 |	clvPairTable1: indice de acceso por la clave PAIR
 | Variables de la tabla TABLE1 locales:
 |	ovfTable1: indicativo de si la tabla admite overflow.
 |	maxTable1: numero maximo de elementos (redimensionado).
 +--------------------------------------------------------------------*/
static int *pPosTable1 = NULL;
static int *pNumTable1 = NULL;
static int *pMaxTable1 = NULL;
static int *pOvfTable1 = NULL;
static int *clvSequenceTable1 = NULL;
static int *clvPairTable1 = NULL;
static int  ovfTable1 = 0;
static int  maxTable1 = MAX_TABLE1;

/*--------------------------------------------------------------------
 | Variables de la tabla TABLE2 para datos compartidos:
 |	pPosTable2: punt. a la posicion de los elementos.
 |	pNumTable2: punt. al numero de elementos.
 |	pMaxTable2: punt. al numero maximo de elementos.
 |	pOvfTable2: punt. al indic. de si la tabla admite overflow.
 |	clvSequenceTable2: indice de acceso por la clave SEQUENCE
 | Variables de la tabla TABLE2 locales:
 |	ovfTable2: indicativo de si la tabla admite overflow.
 |	maxTable2: numero maximo de elementos (redimensionado).
 +--------------------------------------------------------------------*/
static int *pPosTable2 = NULL;
static int *pNumTable2 = NULL;
static int *pMaxTable2 = NULL;
static int *pOvfTable2 = NULL;
static int *clvSequenceTable2 = NULL;
static int  ovfTable2 = 0;
static int  maxTable2 = MAX_TABLE2;

/*--------------------------------------------------------------------
 | Variables de la tabla TABLE3 para datos compartidos:
 |	pPosTable3: punt. a la posicion de los elementos.
 |	pNumTable3: punt. al numero de elementos.
 |	pMaxTable3: punt. al numero maximo de elementos.
 |	pOvfTable3: punt. al indic. de si la tabla admite overflow.
 |	clvSequenceTable3: indice de acceso por la clave SEQUENCE
 | Variables de la tabla TABLE3 locales:
 |	ovfTable3: indicativo de si la tabla admite overflow.
 |	maxTable3: numero maximo de elementos (redimensionado).
 +--------------------------------------------------------------------*/
static int *pPosTable3 = NULL;
static int *pNumTable3 = NULL;
static int *pMaxTable3 = NULL;
static int *pOvfTable3 = NULL;
static int *clvSequenceTable3 = NULL;
static int  ovfTable3 = 0;
static int  maxTable3 = MAX_TABLE3;


/*========================= FUNCION PUBLICA =========================
 * EXAM5_Directory
 *	Notifica el directorio de donde  se debe cogen el fichero
 *	imagen de la base.
 *
 *    Parametros:
 *	- directorio: path del directorio.
 *    Devuelve:
 *	 0: si se realiza correctamente.
 *	-1: si hay error.
 *====================================================================*/
int EXAM5_Directory(char *directorio)
{
	char *nuevoPath;

/*--------------------------------------------------------------------
 | Se pide mem. dinamica para el path del fichero imagen.
 +--------------------------------------------------------------------*/
nuevoPath = (char *) malloc(strlen(directorio) + sizeof("EXAM5.ibd") + 1);
if (nuevoPath == NULL) {
    ERR_SistOper(ERR_NO_FATAL, FUNC_BDT, "EXAM5_Directorio", "malloc",
		 "(%d bytes) path base", lonMemoria);
    return -1;
    }

/*--------------------------------------------------------------------
 | Se compone el nuevo path.
 +--------------------------------------------------------------------*/
sprintf(nuevoPath, "%s/EXAM5.ibd", directorio);
fichImagen = nuevoPath;
return 0;
}

/*========================= FUNCION PUBLICA =========================
 * EXAM5_Resize
 *	Notifica la nueva dimension de una determinada tabla. Siempre
 *	debe ser menor o igual a la definida como maxima.
 *
 *    Parametros:
 *	- tabla: identificador de la tabla a dimensionar.
 *	- dimension: valor de la nueva dimension.
 *    Devuelve:
 *	 0: si se realiza correctamente.
 *	-1: si hay error.
 *====================================================================*/
int EXAM5_Resize(int tabla, int dimension)
{
switch(tabla) {
    case TBL_TABLE1:
	/*------------------------------------------------------------
	 | Si la dimension es mayor que la definida como maxima para la
	 | tabla TABLE1 no se acepta y se da un error.
	 +------------------------------------------------------------*/
	if (dimension > MAX_TABLE1) {
	    ERR_Aplicacion(ERR_NO_FATAL, FUNC_BDT, "EXAM5_Dimensionar",
	      "resize %d of TABLE1 overflow (max: 1000)",
	       dimension);
	    return -1;
	    }
	maxTable1 = dimension;
	break;
    case TBL_TABLE2:
	/*------------------------------------------------------------
	 | Si la dimension es mayor que la definida como maxima para la
	 | tabla TABLE2 no se acepta y se da un error.
	 +------------------------------------------------------------*/
	if (dimension > MAX_TABLE2) {
	    ERR_Aplicacion(ERR_NO_FATAL, FUNC_BDT, "EXAM5_Dimensionar",
	      "resize %d of TABLE2 overflow (max: 1000)",
	       dimension);
	    return -1;
	    }
	maxTable2 = dimension;
	break;
    case TBL_TABLE3:
	/*------------------------------------------------------------
	 | Si la dimension es mayor que la definida como maxima para la
	 | tabla TABLE3 no se acepta y se da un error.
	 +------------------------------------------------------------*/
	if (dimension > MAX_TABLE3) {
	    ERR_Aplicacion(ERR_NO_FATAL, FUNC_BDT, "EXAM5_Dimensionar",
	      "resize %d of TABLE3 overflow (max: 1000)",
	       dimension);
	    return -1;
	    }
	maxTable3 = dimension;
	break;
    }
return 0;
}

/*====================== PROCEDIMIENTO PUBLICO ======================
 * EXAM5_Overflow
 *	Notifica si se quiere el desbordamiento activado o no.
 *	Si el codigo de tabla no es correcto no se hace nada.
 *
 *    Parametros:
 *	- tabla: identificador de la tabla.
 *	- desbord: valor 0/1 (NO/SI) de admision de desbordamiento.
 *====================================================================*/
void EXAM5_Overflow(int tabla, int desbord)
{
switch(tabla) {
    case TBL_TABLE1:
	ovfTable1 = desbord;
	break;
    case TBL_TABLE2:
	ovfTable2 = desbord;
	break;
    case TBL_TABLE3:
	ovfTable3 = desbord;
	break;
    }
}

/*====================== PROCEDIMIENTO PUBLICO ======================
 * EXAM5_LetFileOpen
 *	Notifica si se quiere actualizar dejando con el fichero imagen
 *	abierto (mas eficiente, pero consume un descriptor) o abriendo
 *	y cerrando con cada actualizacion.
 *
 *    Parametros:
 *	- abierto: 1 indica siempre abierto, 0 abrir y cerrar.
 *====================================================================*/
void EXAM5_LetFileOpen(int abierto)
{
fichAbierto = abierto;
}

/*========================= FUNCION PUBLICA =========================
 * EXAM5_CreateBase
 *	Crea la memoria compartida y semaforos de la base.
 *	Inicializa la memoria on el fichero base de datos.
 *	Inicializa los array de punteros de las claves.
 *
 *    Devuelve:
 *	0: si se crea correctamente.
 *	-1: si hay error.
 *====================================================================*/
int EXAM5_CreateBase()
{
	key_t clave;
	SEMUN unSemaf;
 	unsigned short arraySemaf[numSEMAF];

/*--------------------------------------------------------------------
 | Se obtiene la clave para la memoria compartida y los semaforos.
 +--------------------------------------------------------------------*/
clave = ftok(fichImagen, ID_EXAM5);

/*--------------------------------------------------------------------
 | Se intenta acceder a los semaforos y si existe, se borra, pues debe
 | ser de una ejecucion anterior.
 +--------------------------------------------------------------------*/
if (semget(clave, 0, 0) != -1)
    semctl(identSem, 0, IPC_RMID);

/*--------------------------------------------------------------------
 | Se crea el identificador de los semaforos de acceso a la base.
 +--------------------------------------------------------------------*/
identSem = semget(clave, numSEMAF, IPC_CREAT | 0666);
if (identSem == -1) {
    ERR_SistOper(ERR_NO_FATAL, FUNC_BDT, "EXAM5_CreBase", "semget",
		"IPC_CREAT (key: file \"%s\", identifier '%c')",
    		fichImagen, ID_EXAM5);
    return -1;
    }

/*--------------------------------------------------------------------
 | Se inicializan los semaforos de control de acceso:
 |	- lectura   (0: lectores)
 |	- escritura (1: libre)
 +--------------------------------------------------------------------*/
arraySemaf[SEMAF_LECT] = 0;
arraySemaf[SEMAF_ESCR] = 1;
unSemaf.array = arraySemaf;
if (semctl(identSem, 0, SETALL, unSemaf) == -1) {
    ERR_SistOper(ERR_NO_FATAL, FUNC_BDT, "EXAM5_CreBase", "semctl",
		"SETALL (key: file \"%s\", identifier '%c')",
    		fichImagen, ID_EXAM5);
    return -1;
    }

/*--------------------------------------------------------------------
 | Se intenta acceder la m. compartida y si existe, se borra para
 | evitar conflictos con una posible version anterior.
 +--------------------------------------------------------------------*/
if (shmget(clave, 0, 0) != -1)
    shmctl(identMC, IPC_RMID, (struct shmid_ds *)NULL);

/*--------------------------------------------------------------------
 | Se calcula la memoria (segun el dimensionado) necesaria para la base.
 | Se crea la memoria compartida para la la base
 +--------------------------------------------------------------------*/
CalcularMemoria();
identMC = shmget(clave, (size_t)lonMemoria, IPC_CREAT | 0666);
if (identMC == -1) {
    ERR_SistOper(ERR_NO_FATAL, FUNC_BDT, "EXAM5_CreBase", "shmget",
		"IPC_CREAT (key: file \"%s\", identifier '%c')",
    		fichImagen, ID_EXAM5);
    return -1;
    }

/*--------------------------------------------------------------------
 | Se accede a la memoria comartida.
 +--------------------------------------------------------------------*/
pMemoria = shmat(identMC, (char *)NULL, SHM_RND);
if (pMemoria == DMC_ERR) {
    ERR_SistOper(ERR_NO_FATAL, FUNC_BDT, "EXAM5_CreBase", "shmat",
	       "key: file \"%s\", identifier '%c'", fichImagen, ID_EXAM5);
    return -1;
    }

/*--------------------------------------------------------------------
 | Se inicializa el vector de proteccion contra la "signals"
 | Se guarda el modo de uso del fichero (abierto/ abrir y cerar).
 | Se pone el indicador de base privada a FALSO.
 +--------------------------------------------------------------------*/
sigfillset(&vectorSig);
sigdelset(&vectorSig,SIGTRAP);
sigdelset(&vectorSig,SIGBUS);
sigdelset(&vectorSig,SIGSEGV);
basePrivada = 0;

/*--------------------------------------------------------------------
 | Se inicializan los punt. de acceso a las zonas de la m. compartida.
 +--------------------------------------------------------------------*/
IniPunteros(1);
return IniInformacion();
}

/*========================= FUNCION PUBLICA =========================
 * EXAM5_DeleteBase
 *	Elimina los recursos de la base de datos:
 *		memoria compartida y semaforos.
 *
 *    Devuelve:
 *	0:  si se elimina correctamente.
 *	-1: si hay error.
 *====================================================================*/
int EXAM5_DeleteBase()
{
/*--------------------------------------------------------------------
 | Se desengancha de la memoria compartida.
 | se borra el segmento de memoria compartida y el semaforo asociado
 | Se finaliza el acceso al fichero imagen.
 | OJO controlar lo devuelto por: shmdt shmctl semctl.
 +--------------------------------------------------------------------*/
if (shmdt(pMemoria) == -1) {
    ERR_SistOper(ERR_NO_FATAL, FUNC_BDT, "EXAM5_DeleteBase", "shmdt", "");
    return -1;
    }
if (shmctl(identMC, IPC_RMID, (struct shmid_ds *)NULL) == -1) {
    ERR_SistOper(ERR_NO_FATAL, FUNC_BDT, "EXAM5_DeleteBase", "shmctl", "IPC_RMID");
    return -1;
    }
if (semctl(identSem, 0, IPC_RMID) == -1) {
    ERR_SistOper(ERR_NO_FATAL, FUNC_BDT, "EXAM5_DeleteBase", "semctl", "IPC_RMID");
    return -1;
    }

/*--------------------------------------------------------------------
 | Se cierra el fichero imagen de la base de datos.
 +--------------------------------------------------------------------*/
if (fdFBD >= 0)
    if (close(fdFBD) == -1) {
	ERR_SistOper(ERR_NO_FATAL, FUNC_BDT, "EXAM5_DeleteBase", "close",
		    "file \"%s\"", fichImagen);
	return -1;
	}

fdFBD = -1;
return 0;
}

/*========================= FUNCION PUBLICA =========================
 * EXAM5_Open
 *	Inicializa el acceso a la base de datos que debe estar
 *	creada previamente.
 *
 *    Devuelve:
 *	 0: si el acceso se realiza correctamente.
 *	-1: si se produce un error.
 *====================================================================*/
int EXAM5_Open()
{
	key_t clave;

/*--------------------------------------------------------------------
 | Se obtiene la clave para la m. compartida y los semaforos.
 +--------------------------------------------------------------------*/
clave = ftok(fichImagen, ID_EXAM5);
identMC = shmget(clave, 0, 0);
if (identMC == -1) {
    ERR_SistOper(ERR_NO_FATAL, FUNC_BDT, "EXAM5_IniAcceso", "shmget",
	       "key: file \"%s\", identifier '%c'", fichImagen, ID_EXAM5);
    return -1;
    }

/*--------------------------------------------------------------------
 | Se engancha a la m. compartida que debe estar ya creada.
 +--------------------------------------------------------------------*/
pMemoria = shmat(identMC, (char *)NULL, SHM_RND);
if (pMemoria == DMC_ERR) {
    ERR_SistOper(ERR_NO_FATAL, FUNC_BDT, "EXAM5_IniAcceso", "shmat",
	       "key: file \"%s\", identifier '%c'", fichImagen, ID_EXAM5);
    return -1;
    }

/*--------------------------------------------------------------------
 | Se accede a los semaforos que tambien debeb estar creados.
 +--------------------------------------------------------------------*/
identSem = semget(clave, 0, 0);
if (identSem == -1) {
    ERR_SistOper(ERR_NO_FATAL, FUNC_BDT, "EXAM5_IniAcceso", "semget",
	       "key: file \"%s\", identifier '%c'", fichImagen, ID_EXAM5);
    return -1;
    }

/*--------------------------------------------------------------------
 | Se inicializa el vector de proteccion contra las "signals".
 | Se pone el indicador de base privada a FALSO.
 +--------------------------------------------------------------------*/
sigfillset(&vectorSig);
sigdelset(&vectorSig,SIGTRAP);
sigdelset(&vectorSig,SIGBUS);
sigdelset(&vectorSig,SIGSEGV);
basePrivada = 0;

TPC_HazPortable(0);
/*--------------------------------------------------------------------
 | Se inicializan los punteros de acceso a las tablas.
 +--------------------------------------------------------------------*/
IniPunteros(0);

return 0;
}

/*========================= FUNCION PUBLICA =========================
 * EXAM5_Close
 *	Le liberan los recursos de acceso a la base de datos
 *
 *    Devuelve:
 *	 0: si el fin acceso se realiza correctamente.
 *	-1: si se produce un error. (de momento no se devuelve).
 *====================================================================*/
int EXAM5_Close()
{
/*--------------------------------------------------------------------
 | Se desengancha de la memoria compartida
 +--------------------------------------------------------------------*/
if (shmdt(pMemoria) == -1) {
    ERR_SistOper(ERR_NO_FATAL, FUNC_BDT, "EXAM5_Close", "shmdt", "");
    return -1;
    }

return 0;
}

/*========================= FUNCION PUBLICA =========================
 * EXAM5_CreatePrivate
 *	Inicializa la base de datos como local creandola e iniciando
 *	el acceso.
 *	La base puede dejar abierto el fichero imagen con lo que las
 *	actualizaciones seran mas rapidas pero consume un descriptor.
 *
 *    Devuelve:
 *	0: si se crea correctamente.
 *	-1: si hay error.
 *====================================================================*/
int EXAM5_CreatePrivate()
{
/*--------------------------------------------------------------------
 | Se calcula la memoria (segun el dimensionado) necesaria para la base.
 | Se pide mem. dinamica para la base da datos.
 +--------------------------------------------------------------------*/
CalcularMemoria();
pMemoria = (char *) malloc(lonMemoria);
if (pMemoria == NULL) {
    ERR_SistOper(ERR_NO_FATAL, FUNC_BDT, "EXAM5_CrePrivada", "malloc",
		"(%d bytes) memory for base", lonMemoria);
    return -1;
    }

/*--------------------------------------------------------------------
 | Se pone el indicador de base privada a CIERTO.
 | Se guarda el modo de uso del fichero (abierto siempre o abrir y cerar).
 +--------------------------------------------------------------------*/
basePrivada = 1;

/*--------------------------------------------------------------------
 | Se inicializan los punteros de acceso a la memoria.
 | Se inicializan los arrays de indices de acceso por clave.
 +--------------------------------------------------------------------*/
TPC_HazPortable(0);
IniPunteros(1);
return IniInformacion();
}

/*========================= FUNCION PUBLICA =========================
 * EXAM5_DeletePrivate
 *	Finaliza el acceso local a la base de datos.
 *
 *	Devuelve:
 *	0: si se elimina correctamente.
 *	-1: si hay error.
 *====================================================================*/
int EXAM5_DeletePrivate()
{
/*--------------------------------------------------------------------
 | Se libera la memoria dinamica.
 +--------------------------------------------------------------------*/
free(pMemoria);

/*--------------------------------------------------------------------
 | Se cierra el fichero imagen de la base de datos.
 +--------------------------------------------------------------------*/
if (fdFBD >= 0)
    if (close(fdFBD) == -1) {
	ERR_SistOper(ERR_NO_FATAL, FUNC_BDT, "EXAM5_DeletePrivate", "close",
		    "file \"%s\"", fichImagen);
	return -1;
	}
fdFBD = -1;
return 0;
}

/*========================= FUNCION PUBLICA =========================
 * EXAM5_Table1BySequence
 *	Busca un elemento TABLE1 , por la clave principal
 *	para lo cual la struct debe tener todos los campos de la
 *	clave con los valores actualizados.
 *
 *    Parametros:
 *	pTable1: punt. a la struct del elemento a buscar.
  *	mykey1: campo MYKEY1 de busqueda.
  *    Devuelve:
 *	 0: si la modificacion se realiza correctamente.
 *	-1: si se produce un error.
 *	Busca un elemento
 *====================================================================*/
int EXAM5_Table1BySequence(TypeTABLE1 *pTable1,
				int            mykey1)
{
	int  indElem;

/*--------------------------------------------------------------------
 | Se codifican los campos de la clave principal sobre el elemento.
 +--------------------------------------------------------------------*/
pTable1->mykey1 = mykey1;

/*--------------------------------------------------------------------
 | Se busca el elemento accediendo por la clave principal y si no
 | existe se devuelve -1.
 +--------------------------------------------------------------------*/
AccLectura();
indElem = BuscaElemento(pTable1, clvSequenceTable1,
	      *pNumTable1, (PF_COMPAR)CmpSequenceTable1);
if (indElem == -1) {
    SalLectura();
    return -1;
    }

/*--------------------------------------------------------------------
 | Si existe, se decodifica el elemento completo sobre la struct.
 +--------------------------------------------------------------------*/
memcpy(pTable1, DIR_MEM(clvSequenceTable1[indElem]), TAM_TABLE1);
SalLectura();

return 0;
}



/*========================= FUNCION PUBLICA =========================
 * EXAM5_VolTable1
 *	Realiza un volcado de la tabla TABLE1 ordenado por
 *	la clave segun el parametro.
 *    Parametros:
 *	claveOrd: codigo de la clave para ordenar el volcado.
 *	pLista: puntero para la lista del volcado.
 *    Devuelve:
 *	n: el numero de elementos seleccionados.
 *	-1: si hay error en el acceso.
 *====================================================================*/
int EXAM5_VolTable1(int claveOrd, TypeTABLE1 **pLista)
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
AccLectura();
if (*pNumTable1 == 0) {
    SalLectura();
    return 0;
    }

/*--------------------------------------------------------------------
 | Se pide memoria dinamica para almacenar los elementos del recorrido.
 +--------------------------------------------------------------------*/
pTable1 = (TypeTABLE1 *) malloc(*pNumTable1 * TAM_TABLE1);
if (pTable1 == NULL) {
    ERR_SistOper(ERR_NO_FATAL, FUNC_BDT, "EXAM5_VolTable1",
	     "malloc", "(%d bytes) for select of Table1", lonMemoria);
    SalLectura();
    return -1;
    }

*pLista = pTable1;

/*--------------------------------------------------------------------
 | Se copia la informacion de cada elemento de la lista segun la
 | ordenacion de la clave.
 +--------------------------------------------------------------------*/
for (i = 0; i < *pNumTable1; i++) {
    memcpy(pTable1, DIR_MEM(claveAcceso[i]), TAM_TABLE1);
    pTable1++;
    }

SalLectura();
return i;
}


/*========================= FUNCION PUBLICA =========================
 * EXAM5_InsTable1
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
int EXAM5_InsTable1(TypeTABLE1 *pTable1)
{
	char elemCod[LON_TABLE1];
	int  posicMem;
	int  posicFic;
	int  indElem;
	int  numElem;
	int  indClave;

/*--------------------------------------------------------------------
 | Se accede para escritura y se obtiene el numero actual de elementos.
 +--------------------------------------------------------------------*/
AccEscritura();
numElem = *pNumTable1;

/*--------------------------------------------------------------------
 | No debe existir otro elemento con igual valor en la clave principal
 +--------------------------------------------------------------------*/
if (BuscaElemento(pTable1, clvSequenceTable1,
		  numElem, (PF_COMPAR)CmpSequenceTable1) >= 0) {
    ERR_Aplicacion(ERR_NO_FATAL, FUNC_BDT, "EXAM5_InsTable1",
		 "key SEQUENCE of TABLE1 exists");
    SalEscritura();
    return -1;
    }

/*--------------------------------------------------------------------
 | Si hay elementos libres, se toma para insercion el primero libre.
 | Si se sobrepasa el limite de elementos max1000 solo se
 | acepta si se admite overflow. En ese caso se inserta sobre el ultimo
 | elemento que es el elemento 0 y que tiene la clave menor.
 +--------------------------------------------------------------------*/
if (numElem < *pMaxTable1) {
    indClave = *pNumTable1;
    numElem++;
    }
else if (*pOvfTable1)
    indClave = 0;
else {
    ERR_Aplicacion(ERR_NO_FATAL, FUNC_BDT, "EXAM5_InsTable1",
	      "overflow in TABLE1 (%d)", *pMaxTable1);
    SalEscritura();
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
    SalEscritura();
    return -1;
    }

/*--------------------------------------------------------------------
 | Se actualiza el numero de elementos, se reordenan las claves y se
 | sale de la base para escritura.
 +--------------------------------------------------------------------*/
*pNumTable1 = numElem;
OrdTable1();
SalEscritura();
return 0;
}


/*========================= FUNCION PUBLICA =========================
 * EXAM5_DelTable1
 *	Elimina un elemento TABLE1 ya existente, segun la
 *	clave Principal.
 *
 *    Parametros:
 *	pTable1: punt. a la struct del elemento a eliminar.
 *    Devuelve:
 *	 0: si la eliminacion se realiza correctamente.
 *	-1: si se produce un error.
 *====================================================================*/
int EXAM5_DelTable1(TypeTABLE1 *pTable1)
{
	char elemCod[LON_TABLE1];
	int  posicMem;
	int  posicFic;
	int  indElem;

AccEscritura();

/*--------------------------------------------------------------------
 | No debe existir otro elemento con igual valor en la clave principal
 +--------------------------------------------------------------------*/
indElem = BuscaElemento(pTable1, clvSequenceTable1,
		*pNumTable1, (PF_COMPAR)CmpSequenceTable1);
if (indElem == -1) {
    ERR_Aplicacion(ERR_NO_FATAL, FUNC_BDT, "EXAM5_DelTable1",
		 "key SEQUENCE of TABLE1 don't exists");
    SalEscritura();
    return -1;
    }

posicMem = clvSequenceTable1[indElem];
indElem  = IND_TABLE1(posicMem);
posicFic = ELE_TABLE1(indElem);

/*--------------------------------------------------------------------
 | Codifica la struct TypeTABLE1 sobre el elemento como OCUPADO
 +--------------------------------------------------------------------*/
memset(elemCod, 0, LON_TABLE1);
PONER_LIBRE(elemCod);
if (ActElemento(elemCod, LON_TABLE1, posicFic,
		pTable1, TAM_TABLE1, posicMem) == -1) {
    SalEscritura();
    return -1;
    }

PosicionLibre(posicMem, clvSequenceTable1, *pNumTable1);
PosicionLibre(posicMem, clvPairTable1, *pNumTable1);
(*pNumTable1)--;
SalEscritura();
return 0;
}

/*========================= FUNCION PUBLICA =========================
 * EXAM5_ModTable1
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
int EXAM5_ModTable1(TypeTABLE1 *pTable1)
{
	char elemCod[LON_TABLE1];
	int  posicMem;
	int  posicFic;
	int  indElem;

AccEscritura();

/*--------------------------------------------------------------------
 | No debe existir otro elemento con igual valor en la clave principal
 +--------------------------------------------------------------------*/
indElem = BuscaElemento(pTable1, clvSequenceTable1,
		*pNumTable1, (PF_COMPAR)CmpSequenceTable1);
if (indElem == -1) {
    ERR_Aplicacion(ERR_NO_FATAL, FUNC_BDT, "EXAM5_ModTable1",
		   "key SEQUENCE of TABLE1 don't exists");
    SalEscritura();
    return -1;
    }

posicMem = clvSequenceTable1[indElem];
indElem  = IND_TABLE1(posicMem);
posicFic = ELE_TABLE1(indElem);


/*--------------------------------------------------------------------
 | Codifica la struct TypeTABLE1 sobre el elemento como OCUPADO
 +--------------------------------------------------------------------*/
CodTable1(elemCod, pTable1);
PONER_OCUPADO(elemCod);
if (ActElemento(elemCod, LON_TABLE1, posicFic,
		pTable1, TAM_TABLE1, posicMem) == -1) {
    SalEscritura();
    return -1;
    }

if (CmpPairTable1(pTable1,
		(TypeTABLE1 *) DIR_MEM(posicMem)) != 0)
    qsort(clvPairTable1, *pNumTable1,
	  sizeof(int), (PF_COMPAR)CmpDesPairTable1);
SalEscritura();
return 0;
}

/*========================= FUNCION PUBLICA =========================
 * EXAM5_Table2BySequence
 *	Busca un elemento TABLE2 , por la clave principal
 *	para lo cual la struct debe tener todos los campos de la
 *	clave con los valores actualizados.
 *
 *    Parametros:
 *	pTable2: punt. a la struct del elemento a buscar.
  *	mykey2: campo MYKEY2 de busqueda.
  *    Devuelve:
 *	 0: si la modificacion se realiza correctamente.
 *	-1: si se produce un error.
 *	Busca un elemento
 *====================================================================*/
int EXAM5_Table2BySequence(TypeTABLE2 *pTable2,
				int            mykey2)
{
	int  indElem;

/*--------------------------------------------------------------------
 | Se codifican los campos de la clave principal sobre el elemento.
 +--------------------------------------------------------------------*/
pTable2->mykey2 = mykey2;

/*--------------------------------------------------------------------
 | Se busca el elemento accediendo por la clave principal y si no
 | existe se devuelve -1.
 +--------------------------------------------------------------------*/
AccLectura();
indElem = BuscaElemento(pTable2, clvSequenceTable2,
	      *pNumTable2, (PF_COMPAR)CmpSequenceTable2);
if (indElem == -1) {
    SalLectura();
    return -1;
    }

/*--------------------------------------------------------------------
 | Si existe, se decodifica el elemento completo sobre la struct.
 +--------------------------------------------------------------------*/
memcpy(pTable2, DIR_MEM(clvSequenceTable2[indElem]), TAM_TABLE2);
SalLectura();

return 0;
}



/*========================= FUNCION PUBLICA =========================
 * EXAM5_VolTable2
 *	Realiza un volcado de la tabla TABLE2 ordenado por
 *	la clave segun el parametro.
 *    Parametros:
 *	claveOrd: codigo de la clave para ordenar el volcado.
 *	pLista: puntero para la lista del volcado.
 *    Devuelve:
 *	n: el numero de elementos seleccionados.
 *	-1: si hay error en el acceso.
 *====================================================================*/
int EXAM5_VolTable2(int claveOrd, TypeTABLE2 **pLista)
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
AccLectura();
if (*pNumTable2 == 0) {
    SalLectura();
    return 0;
    }

/*--------------------------------------------------------------------
 | Se pide memoria dinamica para almacenar los elementos del recorrido.
 +--------------------------------------------------------------------*/
pTable2 = (TypeTABLE2 *) malloc(*pNumTable2 * TAM_TABLE2);
if (pTable2 == NULL) {
    ERR_SistOper(ERR_NO_FATAL, FUNC_BDT, "EXAM5_VolTable2",
	     "malloc", "(%d bytes) for select of Table2", lonMemoria);
    SalLectura();
    return -1;
    }

*pLista = pTable2;

/*--------------------------------------------------------------------
 | Se copia la informacion de cada elemento de la lista segun la
 | ordenacion de la clave.
 +--------------------------------------------------------------------*/
for (i = 0; i < *pNumTable2; i++) {
    memcpy(pTable2, DIR_MEM(claveAcceso[i]), TAM_TABLE2);
    pTable2++;
    }

SalLectura();
return i;
}

/*========================= FUNCION PUBLICA =========================
 * EXAM5_SelTable2ByBusqueda
 *	Realiza una seleccion de la tabla TABLE2 segun el
 *	criterio BUSQUEDA y ordenado por una clave segun parametro.
 *
 *    Parametros:
 *	claveOrd: codigo de la clave para ordenar el volcado.
 *	pLista: puntero a la lista de la seleccion.
 *	valuefrom: valor para la seleccion.
 *	valueto: valor para la seleccion.
 *    Devuelve:
 *	n: numero de elementos seleccionados (puede ser 0).
 *	-1: si hay error en la seleccion.
 *====================================================================*/
int EXAM5_SelTable2ByBusqueda(int claveOrd,
			TypeTABLE2 **pLista,
			int valueFrom,
			int valueTo)
{
	int   i;
	int   totSelec;
	TypeTABLE2 *pTable2;
	TypeTABLE2 lstSelec[MAX_TABLE2];
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
AccLectura();
if (*pNumTable2 == 0) {
    SalLectura();
    return 0;
    }

/*--------------------------------------------------------------------
 | Se recorre toda la tabla, decodificando cada elemento sobre el
 | elemento siguiente de la lista. Si cumple la condicion de seleccion
 | se avanza el puntero al siguiente elemento y se incrementa el numero
 | de elementos seleccionados.
 +--------------------------------------------------------------------*/
totSelec = 0;
for (i = 0; i < *pNumTable2; i++) {
    pTable2 = (TypeTABLE2 *) DIR_MEM(claveAcceso[i]);
    if ((pTable2->mykey2 >= valueFrom) &&
	(pTable2->mykey2 <= valueTo)) {
	memcpy(lstSelec + totSelec, pTable2, TAM_TABLE2);
	totSelec++;
	}
    }
SalLectura();

/*--------------------------------------------------------------------
 | Se pide memoria dinamica para almacenar los elementos de seleccion
 | (solo si se ha seleccionado alguno y se copian sobre esta memoria.
 +--------------------------------------------------------------------*/
i = totSelec * TAM_TABLE2;
if (i > 0) {
    *pLista = (TypeTABLE2 *) malloc((size_t) i);
    if (*pLista == NULL) {
	ERR_SistOper(ERR_NO_FATAL, FUNC_BDT,
		"EXAM5_SelTable2ByBusqueda", "malloc",
		"(%d bytes) for select Busqueda of Table2", i);
	return -1;
	}
    memcpy((void *) *pLista, (void *) lstSelec, i);
    }
return totSelec;
}

/*========================= FUNCION PUBLICA =========================
 * EXAM5_InsTable2
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
int EXAM5_InsTable2(TypeTABLE2 *pTable2)
{
	char elemCod[LON_TABLE2];
	int  posicMem;
	int  posicFic;
	int  indElem;
	int  numElem;
	int  indClave;

/*--------------------------------------------------------------------
 | Se accede para escritura y se obtiene el numero actual de elementos.
 +--------------------------------------------------------------------*/
AccEscritura();
numElem = *pNumTable2;

/*--------------------------------------------------------------------
 | No debe existir otro elemento con igual valor en la clave principal
 +--------------------------------------------------------------------*/
if (BuscaElemento(pTable2, clvSequenceTable2,
		  numElem, (PF_COMPAR)CmpSequenceTable2) >= 0) {
    ERR_Aplicacion(ERR_NO_FATAL, FUNC_BDT, "EXAM5_InsTable2",
		 "key SEQUENCE of TABLE2 exists");
    SalEscritura();
    return -1;
    }

/*--------------------------------------------------------------------
 | Si hay elementos libres, se toma para insercion el primero libre.
 | Si se sobrepasa el limite de elementos max1000 solo se
 | acepta si se admite overflow. En ese caso se inserta sobre el ultimo
 | elemento que es el elemento 0 y que tiene la clave menor.
 +--------------------------------------------------------------------*/
if (numElem < *pMaxTable2) {
    indClave = *pNumTable2;
    numElem++;
    }
else if (*pOvfTable2)
    indClave = 0;
else {
    ERR_Aplicacion(ERR_NO_FATAL, FUNC_BDT, "EXAM5_InsTable2",
	      "overflow in TABLE2 (%d)", *pMaxTable2);
    SalEscritura();
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
    SalEscritura();
    return -1;
    }

/*--------------------------------------------------------------------
 | Se actualiza el numero de elementos, se reordenan las claves y se
 | sale de la base para escritura.
 +--------------------------------------------------------------------*/
*pNumTable2 = numElem;
OrdTable2();
SalEscritura();
return 0;
}


/*========================= FUNCION PUBLICA =========================
 * EXAM5_DelTable2
 *	Elimina un elemento TABLE2 ya existente, segun la
 *	clave Principal.
 *
 *    Parametros:
 *	pTable2: punt. a la struct del elemento a eliminar.
 *    Devuelve:
 *	 0: si la eliminacion se realiza correctamente.
 *	-1: si se produce un error.
 *====================================================================*/
int EXAM5_DelTable2(TypeTABLE2 *pTable2)
{
	char elemCod[LON_TABLE2];
	int  posicMem;
	int  posicFic;
	int  indElem;

AccEscritura();

/*--------------------------------------------------------------------
 | No debe existir otro elemento con igual valor en la clave principal
 +--------------------------------------------------------------------*/
indElem = BuscaElemento(pTable2, clvSequenceTable2,
		*pNumTable2, (PF_COMPAR)CmpSequenceTable2);
if (indElem == -1) {
    ERR_Aplicacion(ERR_NO_FATAL, FUNC_BDT, "EXAM5_DelTable2",
		 "key SEQUENCE of TABLE2 don't exists");
    SalEscritura();
    return -1;
    }

posicMem = clvSequenceTable2[indElem];
indElem  = IND_TABLE2(posicMem);
posicFic = ELE_TABLE2(indElem);

/*--------------------------------------------------------------------
 | Codifica la struct TypeTABLE2 sobre el elemento como OCUPADO
 +--------------------------------------------------------------------*/
memset(elemCod, 0, LON_TABLE2);
PONER_LIBRE(elemCod);
if (ActElemento(elemCod, LON_TABLE2, posicFic,
		pTable2, TAM_TABLE2, posicMem) == -1) {
    SalEscritura();
    return -1;
    }

PosicionLibre(posicMem, clvSequenceTable2, *pNumTable2);
(*pNumTable2)--;
SalEscritura();
return 0;
}

/*========================= FUNCION PUBLICA =========================
 * EXAM5_ModTable2
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
int EXAM5_ModTable2(TypeTABLE2 *pTable2)
{
	char elemCod[LON_TABLE2];
	int  posicMem;
	int  posicFic;
	int  indElem;

AccEscritura();

/*--------------------------------------------------------------------
 | No debe existir otro elemento con igual valor en la clave principal
 +--------------------------------------------------------------------*/
indElem = BuscaElemento(pTable2, clvSequenceTable2,
		*pNumTable2, (PF_COMPAR)CmpSequenceTable2);
if (indElem == -1) {
    ERR_Aplicacion(ERR_NO_FATAL, FUNC_BDT, "EXAM5_ModTable2",
		   "key SEQUENCE of TABLE2 don't exists");
    SalEscritura();
    return -1;
    }

posicMem = clvSequenceTable2[indElem];
indElem  = IND_TABLE2(posicMem);
posicFic = ELE_TABLE2(indElem);


/*--------------------------------------------------------------------
 | Codifica la struct TypeTABLE2 sobre el elemento como OCUPADO
 +--------------------------------------------------------------------*/
CodTable2(elemCod, pTable2);
PONER_OCUPADO(elemCod);
if (ActElemento(elemCod, LON_TABLE2, posicFic,
		pTable2, TAM_TABLE2, posicMem) == -1) {
    SalEscritura();
    return -1;
    }

SalEscritura();
return 0;
}

/*========================= FUNCION PUBLICA =========================
 * EXAM5_Table3BySequence
 *	Busca un elemento TABLE3 , por la clave principal
 *	para lo cual la struct debe tener todos los campos de la
 *	clave con los valores actualizados.
 *
 *    Parametros:
 *	pTable3: punt. a la struct del elemento a buscar.
  *	mykey3: campo MYKEY3 de busqueda.
  *    Devuelve:
 *	 0: si la modificacion se realiza correctamente.
 *	-1: si se produce un error.
 *	Busca un elemento
 *====================================================================*/
int EXAM5_Table3BySequence(TypeTABLE3 *pTable3,
				int            mykey3)
{
	int  indElem;

/*--------------------------------------------------------------------
 | Se codifican los campos de la clave principal sobre el elemento.
 +--------------------------------------------------------------------*/
pTable3->mykey3 = mykey3;

/*--------------------------------------------------------------------
 | Se busca el elemento accediendo por la clave principal y si no
 | existe se devuelve -1.
 +--------------------------------------------------------------------*/
AccLectura();
indElem = BuscaElemento(pTable3, clvSequenceTable3,
	      *pNumTable3, (PF_COMPAR)CmpSequenceTable3);
if (indElem == -1) {
    SalLectura();
    return -1;
    }

/*--------------------------------------------------------------------
 | Si existe, se decodifica el elemento completo sobre la struct.
 +--------------------------------------------------------------------*/
memcpy(pTable3, DIR_MEM(clvSequenceTable3[indElem]), TAM_TABLE3);
SalLectura();

return 0;
}



/*========================= FUNCION PUBLICA =========================
 * EXAM5_VolTable3
 *	Realiza un volcado de la tabla TABLE3 ordenado por
 *	la clave segun el parametro.
 *    Parametros:
 *	claveOrd: codigo de la clave para ordenar el volcado.
 *	pLista: puntero para la lista del volcado.
 *    Devuelve:
 *	n: el numero de elementos seleccionados.
 *	-1: si hay error en el acceso.
 *====================================================================*/
int EXAM5_VolTable3(int claveOrd, TypeTABLE3 **pLista)
{
	int i;
	TypeTABLE3 *pTable3;
	int *claveAcceso;

/*--------------------------------------------------------------------
 | Se recoge el array de acceso por la clave definida por "claveOrd"
 +--------------------------------------------------------------------*/
claveAcceso = ClaveTable3(claveOrd);
if (claveAcceso == NULL)
    return -1;

/*--------------------------------------------------------------------
 | Si la tabla esta vacia no se hace nada mas y se devuelve 0.
 +--------------------------------------------------------------------*/
AccLectura();
if (*pNumTable3 == 0) {
    SalLectura();
    return 0;
    }

/*--------------------------------------------------------------------
 | Se pide memoria dinamica para almacenar los elementos del recorrido.
 +--------------------------------------------------------------------*/
pTable3 = (TypeTABLE3 *) malloc(*pNumTable3 * TAM_TABLE3);
if (pTable3 == NULL) {
    ERR_SistOper(ERR_NO_FATAL, FUNC_BDT, "EXAM5_VolTable3",
	     "malloc", "(%d bytes) for select of Table3", lonMemoria);
    SalLectura();
    return -1;
    }

*pLista = pTable3;

/*--------------------------------------------------------------------
 | Se copia la informacion de cada elemento de la lista segun la
 | ordenacion de la clave.
 +--------------------------------------------------------------------*/
for (i = 0; i < *pNumTable3; i++) {
    memcpy(pTable3, DIR_MEM(claveAcceso[i]), TAM_TABLE3);
    pTable3++;
    }

SalLectura();
return i;
}

/*========================= FUNCION PUBLICA =========================
 * EXAM5_SelTable3ByBusqueda
 *	Realiza una seleccion de la tabla TABLE3 segun el
 *	criterio BUSQUEDA y ordenado por una clave segun parametro.
 *
 *    Parametros:
 *	claveOrd: codigo de la clave para ordenar el volcado.
 *	pLista: puntero a la lista de la seleccion.
 *	valuefrom: valor para la seleccion.
 *	valuetext: valor para la seleccion.
 *    Devuelve:
 *	n: numero de elementos seleccionados (puede ser 0).
 *	-1: si hay error en la seleccion.
 *====================================================================*/
int EXAM5_SelTable3ByBusqueda(int claveOrd,
			TypeTABLE3 **pLista,
			int valueFrom,
			char *valueText)
{
	int   i;
	int   totSelec;
	TypeTABLE3 *pTable3;
	TypeTABLE3 lstSelec[MAX_TABLE3];
	int *claveAcceso;

/*--------------------------------------------------------------------
 | Se recoge el array de acceso por la clave definida por "claveOrd"
 +--------------------------------------------------------------------*/
claveAcceso = ClaveTable3(claveOrd);
if (claveAcceso == NULL)
    return -1;

/*--------------------------------------------------------------------
 | Si la tabla esta vacia no se hace nada mas y se devuelve 0.
 +--------------------------------------------------------------------*/
AccLectura();
if (*pNumTable3 == 0) {
    SalLectura();
    return 0;
    }

/*--------------------------------------------------------------------
 | Se recorre toda la tabla, decodificando cada elemento sobre el
 | elemento siguiente de la lista. Si cumple la condicion de seleccion
 | se avanza el puntero al siguiente elemento y se incrementa el numero
 | de elementos seleccionados.
 +--------------------------------------------------------------------*/
totSelec = 0;
for (i = 0; i < *pNumTable3; i++) {
    pTable3 = (TypeTABLE3 *) DIR_MEM(claveAcceso[i]);
    if ((pTable3->mykey3 >= valueFrom) &&
	strstr(pTable3->ispair, valueText)) {
	memcpy(lstSelec + totSelec, pTable3, TAM_TABLE3);
	totSelec++;
	}
    }
SalLectura();

/*--------------------------------------------------------------------
 | Se pide memoria dinamica para almacenar los elementos de seleccion
 | (solo si se ha seleccionado alguno y se copian sobre esta memoria.
 +--------------------------------------------------------------------*/
i = totSelec * TAM_TABLE3;
if (i > 0) {
    *pLista = (TypeTABLE3 *) malloc((size_t) i);
    if (*pLista == NULL) {
	ERR_SistOper(ERR_NO_FATAL, FUNC_BDT,
		"EXAM5_SelTable3ByBusqueda", "malloc",
		"(%d bytes) for select Busqueda of Table3", i);
	return -1;
	}
    memcpy((void *) *pLista, (void *) lstSelec, i);
    }
return totSelec;
}

/*========================= FUNCION PUBLICA =========================
 * EXAM5_InsTable3
 *	Inserta un nuevo elemento TABLE3, segun la clave
 *	principal, para lo cual la struct debe tener todos los
 *	campos de las claves con los valores actualizados.
 *
 *    Parametros:
 *	ptable3: punt. a la struct del elemento a insertar.
 *    Devuelve:
 *	 0: si la insercion se realiza correctamente.
 *	-1: si se produce un error.
 *====================================================================*/
int EXAM5_InsTable3(TypeTABLE3 *pTable3)
{
	char elemCod[LON_TABLE3];
	int  posicMem;
	int  posicFic;
	int  indElem;
	int  numElem;
	int  indClave;

/*--------------------------------------------------------------------
 | Se accede para escritura y se obtiene el numero actual de elementos.
 +--------------------------------------------------------------------*/
AccEscritura();
numElem = *pNumTable3;

/*--------------------------------------------------------------------
 | No debe existir otro elemento con igual valor en la clave principal
 +--------------------------------------------------------------------*/
if (BuscaElemento(pTable3, clvSequenceTable3,
		  numElem, (PF_COMPAR)CmpSequenceTable3) >= 0) {
    ERR_Aplicacion(ERR_NO_FATAL, FUNC_BDT, "EXAM5_InsTable3",
		 "key SEQUENCE of TABLE3 exists");
    SalEscritura();
    return -1;
    }

/*--------------------------------------------------------------------
 | Si hay elementos libres, se toma para insercion el primero libre.
 | Si se sobrepasa el limite de elementos max1000 solo se
 | acepta si se admite overflow. En ese caso se inserta sobre el ultimo
 | elemento que es el elemento 0 y que tiene la clave menor.
 +--------------------------------------------------------------------*/
if (numElem < *pMaxTable3) {
    indClave = *pNumTable3;
    numElem++;
    }
else if (*pOvfTable3)
    indClave = 0;
else {
    ERR_Aplicacion(ERR_NO_FATAL, FUNC_BDT, "EXAM5_InsTable3",
	      "overflow in TABLE3 (%d)", *pMaxTable3);
    SalEscritura();
    return -1;
    }

/*--------------------------------------------------------------------
 | Se calculan las posiciones para la actualizacion.
 +--------------------------------------------------------------------*/
posicMem = clvSequenceTable3[indClave];
indElem  = IND_TABLE3(posicMem);
posicFic = ELE_TABLE3(indElem);

/*--------------------------------------------------------------------
 | Codifica la struct TypeTABLE3 sobre el elemento como OCUPADO
 +--------------------------------------------------------------------*/
CodTable3(elemCod, pTable3);
PONER_OCUPADO(elemCod);
if (ActElemento(elemCod, LON_TABLE3, posicFic,
		pTable3, TAM_TABLE3, posicMem) == -1) {
    SalEscritura();
    return -1;
    }

/*--------------------------------------------------------------------
 | Se actualiza el numero de elementos, se reordenan las claves y se
 | sale de la base para escritura.
 +--------------------------------------------------------------------*/
*pNumTable3 = numElem;
OrdTable3();
SalEscritura();
return 0;
}


/*========================= FUNCION PUBLICA =========================
 * EXAM5_DelTable3
 *	Elimina un elemento TABLE3 ya existente, segun la
 *	clave Principal.
 *
 *    Parametros:
 *	pTable3: punt. a la struct del elemento a eliminar.
 *    Devuelve:
 *	 0: si la eliminacion se realiza correctamente.
 *	-1: si se produce un error.
 *====================================================================*/
int EXAM5_DelTable3(TypeTABLE3 *pTable3)
{
	char elemCod[LON_TABLE3];
	int  posicMem;
	int  posicFic;
	int  indElem;

AccEscritura();

/*--------------------------------------------------------------------
 | No debe existir otro elemento con igual valor en la clave principal
 +--------------------------------------------------------------------*/
indElem = BuscaElemento(pTable3, clvSequenceTable3,
		*pNumTable3, (PF_COMPAR)CmpSequenceTable3);
if (indElem == -1) {
    ERR_Aplicacion(ERR_NO_FATAL, FUNC_BDT, "EXAM5_DelTable3",
		 "key SEQUENCE of TABLE3 don't exists");
    SalEscritura();
    return -1;
    }

posicMem = clvSequenceTable3[indElem];
indElem  = IND_TABLE3(posicMem);
posicFic = ELE_TABLE3(indElem);

/*--------------------------------------------------------------------
 | Codifica la struct TypeTABLE3 sobre el elemento como OCUPADO
 +--------------------------------------------------------------------*/
memset(elemCod, 0, LON_TABLE3);
PONER_LIBRE(elemCod);
if (ActElemento(elemCod, LON_TABLE3, posicFic,
		pTable3, TAM_TABLE3, posicMem) == -1) {
    SalEscritura();
    return -1;
    }

PosicionLibre(posicMem, clvSequenceTable3, *pNumTable3);
(*pNumTable3)--;
SalEscritura();
return 0;
}

/*========================= FUNCION PUBLICA =========================
 * EXAM5_ModTable3
 *	Modifica un elemento TABLE3 ya existente, segun la
 *	clave principal, para lo cual la struct debe tener todos los
 *	campos con los valores actualizados.
 *
 *    Parametros:
 *	table3: punt. a la struct del elemento a eliminar.
 *    Devuelve:
 *	 0: si la modificacion se realiza correctamente.
 *	-1: si se produce un error.
 *====================================================================*/
int EXAM5_ModTable3(TypeTABLE3 *pTable3)
{
	char elemCod[LON_TABLE3];
	int  posicMem;
	int  posicFic;
	int  indElem;

AccEscritura();

/*--------------------------------------------------------------------
 | No debe existir otro elemento con igual valor en la clave principal
 +--------------------------------------------------------------------*/
indElem = BuscaElemento(pTable3, clvSequenceTable3,
		*pNumTable3, (PF_COMPAR)CmpSequenceTable3);
if (indElem == -1) {
    ERR_Aplicacion(ERR_NO_FATAL, FUNC_BDT, "EXAM5_ModTable3",
		   "key SEQUENCE of TABLE3 don't exists");
    SalEscritura();
    return -1;
    }

posicMem = clvSequenceTable3[indElem];
indElem  = IND_TABLE3(posicMem);
posicFic = ELE_TABLE3(indElem);


/*--------------------------------------------------------------------
 | Codifica la struct TypeTABLE3 sobre el elemento como OCUPADO
 +--------------------------------------------------------------------*/
CodTable3(elemCod, pTable3);
PONER_OCUPADO(elemCod);
if (ActElemento(elemCod, LON_TABLE3, posicFic,
		pTable3, TAM_TABLE3, posicMem) == -1) {
    SalEscritura();
    return -1;
    }

SalEscritura();
return 0;
}

/*====================================================================
 |	 	FUNIONES LOCALES DEPENDIENTES DE LA BASE
 =====================================================================*/

/*====================== PROCEDIMIENTO PRIVADO ======================
 * AccLectura
 *	Realiza el acceso a la base de datos (region critica) para
 *	lectura.
 *====================================================================*/
static void AccLectura()
{
	static struct sembuf opSem1   = { SEMAF_ESCR, -1, SEM_UNDO };
	static struct sembuf opSem2[] = { {SEMAF_LECT,  1, SEM_UNDO},
					  {SEMAF_ESCR,  1, SEM_UNDO} };

/*--------------------------------------------------------------------
 | Si la base se ha creado como privada, no se usan semaforos.
 +--------------------------------------------------------------------*/
if (basePrivada)
    return;

/*--------------------------------------------------------------------
 | Se bloquean las senales mientras se accede
 +--------------------------------------------------------------------*/
if (sigprocmask(SIG_BLOCK, &vectorSig, NULL) == -1)
    ERR_SistOper(ERR_NO_FATAL, FUNC_BDT, "EXAM5: AccLectura",
		 "sigprocmask", "SIG_BLOCK");

/*--------------------------------------------------------------------
 | Peticion de bloqueo del semaforo de escritura:  P (get)
 +--------------------------------------------------------------------*/
if (semop(identSem, &opSem1, 1) == -1)
    ERR_SistOper(ERR_NO_FATAL, FUNC_BDT, "EXAM5: AccLectura",
		 "semop", "operacion: P (get)");

/*--------------------------------------------------------------------
 | No hay escritores: se indica un nuevo lector y V (release)
 +--------------------------------------------------------------------*/
if (semop(identSem, opSem2, 2) == -1)
    ERR_SistOper(ERR_NO_FATAL, FUNC_BDT, "EXAM5: AccLectura",
		 "semop", "operacion: V (release)");
}

/*====================== PROCEDIMIENTO PRIVADO ======================
 * AccEscritura
 *	Realiza el acceso a la base de datos (region critica) para
 *	escritura.
 *====================================================================*/
static void AccEscritura()
{
	static struct sembuf opSem1 = { SEMAF_ESCR, -1, SEM_UNDO };
	static struct sembuf opSem2 = { SEMAF_LECT,  0, SEM_UNDO };

/*--------------------------------------------------------------------
 | Si la base se ha creado como privada, no se usan semaforos.
 +--------------------------------------------------------------------*/
if (basePrivada)
    return;

/*--------------------------------------------------------------------
 | Se bloquean las senales mientras se accede
 +--------------------------------------------------------------------*/
if (sigprocmask(SIG_BLOCK, &vectorSig, NULL) == -1)
    ERR_SistOper(ERR_NO_FATAL, FUNC_BDT, "EXAM5: AccEscritura",
		 "sigprocmask", "SIG_BLOCK");

/*--------------------------------------------------------------------
 | Peticion de bloqueo del semaforo de escritura:  P (get)
 +--------------------------------------------------------------------*/
if (semop(identSem, &opSem1, 1) == -1)
    ERR_SistOper(ERR_NO_FATAL, FUNC_BDT, "EXAM5: AccEscritura",
		 "semop", "operacion: P (get)");

/*--------------------------------------------------------------------
 | Espera a que no queden lectores.
 +--------------------------------------------------------------------*/
if (semop(identSem, &opSem2, 1) == -1)
    ERR_SistOper(ERR_NO_FATAL, FUNC_BDT, "EXAM5: AccEscritura",
		 "semop", "operacion: espera a los lectores");
}

/*====================== PROCEDIMIENTO PRIVADO ======================
 * SalLectura
 *	Realiza la salida de la base de datos (region critica) en
 *	lectura.
 *====================================================================*/
static void SalLectura()
{
	static struct sembuf opSem = { SEMAF_LECT, -1, SEM_UNDO };

/*--------------------------------------------------------------------
 | Si la base se ha creado como privada, no se usan semaforos.
 +--------------------------------------------------------------------*/
if (basePrivada)
    return;

/*--------------------------------------------------------------------
 | Se libera el semaforo de los lectores.
 +--------------------------------------------------------------------*/
if (semop(identSem, &opSem, 1) == -1)
    ERR_SistOper(ERR_NO_FATAL, FUNC_BDT, "EXAM5: SalLectura",
		 "semop", "operacion: liberacion de semaforo");

/*--------------------------------------------------------------------
 | Se desbloquean las senales despues del acceso.
 +--------------------------------------------------------------------*/
if (sigprocmask(SIG_UNBLOCK, &vectorSig, NULL) == -1)
    ERR_SistOper(ERR_NO_FATAL, FUNC_BDT, "EXAM5: SalLectura",
		 "sigprocmask", "SIG_UNBLOCK");
}

/*====================== PROCEDIMIENTO PRIVADO ======================
 * SalEscritura
 *	Realiza la salida de la base de datos (region critica) en
 *	escritura.
 *====================================================================*/
static void SalEscritura()
{
	static struct sembuf opSem = { SEMAF_ESCR, 1, SEM_UNDO };

/*--------------------------------------------------------------------
 | Si la base se ha creado como privada, no se usan semaforos.
 +--------------------------------------------------------------------*/
if (basePrivada)
    return;

/*--------------------------------------------------------------------
 | Se libera el semaforo de los escritor.
 +--------------------------------------------------------------------*/
if (semop(identSem, &opSem, 1) == -1)
    ERR_SistOper(ERR_NO_FATAL, FUNC_BDT, "EXAM5: SalEscritura",
		 "semop", "operacion: liberacion de semaforo");

/*--------------------------------------------------------------------
 | Se desbloquean las senales despues del acceso.
 +--------------------------------------------------------------------*/
if (sigprocmask(SIG_UNBLOCK, &vectorSig, NULL) == -1)
    ERR_SistOper(ERR_NO_FATAL, FUNC_BDT, "EXAM5: SalEscritura",
		 "sigprocmask", "SIG_UNBLOCK");
}

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
    ERR_Aplicacion(ERR_NO_FATAL, FUNC_BDT, "EXAM5: IniInformacion",
		   "file \"%s\" don't exists", fichImagen);
    return -1;
    }

/*--------------------------------------------------------------------
 | Se mira si el fichero imagen de Config. tiene permiso de lectura
 +--------------------------------------------------------------------*/
if (access(fichImagen, R_OK) == -1) {
    ERR_Aplicacion(ERR_NO_FATAL, FUNC_BDT, "EXAM5: IniInformacion",
		   "file \"%s\" can't read", fichImagen);
    return -1;
    }

/*--------------------------------------------------------------------
 | Se mira si el fichero imagen de Config. tiene permiso de escritura
 +--------------------------------------------------------------------*/
if (access(fichImagen, W_OK) == -1) {
    ERR_Aplicacion(ERR_NO_FATAL, FUNC_BDT, "EXAM5: IniInformacion",
		   "file \"%s\" can't write", fichImagen);
    return -1;
    }

/*--------------------------------------------------------------------
 | Se abre el fichero imagen de la base de datos
 +--------------------------------------------------------------------*/
fdFBD = open(fichImagen, O_RDONLY | O_EXCL);
if (fdFBD == -1) {
    ERR_SistOper(ERR_NO_FATAL, FUNC_BDT, "EXAM5: IniInformacion", "open",
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
    ERR_SistOper(ERR_FATAL, FUNC_BDT, "EXAM5 IniInformacion", "malloc",
                 "(%d bytes) path of base", BUF_TABLE1);



/*--------------------------------------------------------------------
 | Se posiciona en la posicion  de la tabla TABLE1 en el fichero
 +--------------------------------------------------------------------*/
if (lseek(fdFBD, POS_TABLE1, SEEK_SET) == -1) {
    ERR_SistOper(ERR_NO_FATAL, FUNC_BDT, "EXAM5: IniInformacion", "lseek",
		"file \"%s\" (position POS_TABLE1)", fichImagen);
    return -1;
    }

/*--------------------------------------------------------------------
 | Se leen todos los elementos de la tabla desde el fichero.
 +--------------------------------------------------------------------*/
if (read(fdFBD, bufElems, BUF_TABLE1) != BUF_TABLE1) {
    ERR_SistOper(ERR_NO_FATAL, FUNC_BDT, "EXAM5: IniInformacion", "read",
		 "file \"%s\" (BUF_TABLE1 bytes)", fichImagen);
    return -1;
    }

/*--------------------------------------------------------------------
 | Se cuenta el numero de elementos que tiene el fichero y se comprueba
 | que el dimensionado es suficiente.
 +--------------------------------------------------------------------*/
pElem = bufElems;
*pNumTable1 = 0;
for (i = 0; i < MAX_TABLE1; i++, pElem += LON_TABLE1)
    if (ESTA_OCUPADO(pElem))
	(*pNumTable1)++;

if (*pNumTable1 > *pMaxTable1) {
    ERR_Aplicacion(ERR_NO_FATAL, FUNC_BDT, "EXAM5: IniInformacion",
    	"el numero de elementos %d de TABLE1 supera el dimensionado %d",
	*pNumTable1, *pMaxTable1);
    return -1;
    }

/*--------------------------------------------------------------------
 | Se extraen los registros ocupados y se inicializan las posiciones
 | de cada clave.
 +--------------------------------------------------------------------*/
indOcu = 0;
indLib = *pNumTable1;
pElem  = bufElems;
pTable1 = (TypeTABLE1 *) DIR_MEM(*pPosTable1);
for (i = 0; i < *pMaxTable1; i++) {
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
    clvPairTable1[indAct] = POS_MEM(pTable1);
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
    ERR_SistOper(ERR_FATAL, FUNC_BDT, "EXAM5 IniInformacion", "malloc",
                 "(%d bytes) path of base", BUF_TABLE2);



/*--------------------------------------------------------------------
 | Se posiciona en la posicion  de la tabla TABLE2 en el fichero
 +--------------------------------------------------------------------*/
if (lseek(fdFBD, POS_TABLE2, SEEK_SET) == -1) {
    ERR_SistOper(ERR_NO_FATAL, FUNC_BDT, "EXAM5: IniInformacion", "lseek",
		"file \"%s\" (position POS_TABLE2)", fichImagen);
    return -1;
    }

/*--------------------------------------------------------------------
 | Se leen todos los elementos de la tabla desde el fichero.
 +--------------------------------------------------------------------*/
if (read(fdFBD, bufElems, BUF_TABLE2) != BUF_TABLE2) {
    ERR_SistOper(ERR_NO_FATAL, FUNC_BDT, "EXAM5: IniInformacion", "read",
		 "file \"%s\" (BUF_TABLE2 bytes)", fichImagen);
    return -1;
    }

/*--------------------------------------------------------------------
 | Se cuenta el numero de elementos que tiene el fichero y se comprueba
 | que el dimensionado es suficiente.
 +--------------------------------------------------------------------*/
pElem = bufElems;
*pNumTable2 = 0;
for (i = 0; i < MAX_TABLE2; i++, pElem += LON_TABLE2)
    if (ESTA_OCUPADO(pElem))
	(*pNumTable2)++;

if (*pNumTable2 > *pMaxTable2) {
    ERR_Aplicacion(ERR_NO_FATAL, FUNC_BDT, "EXAM5: IniInformacion",
    	"el numero de elementos %d de TABLE2 supera el dimensionado %d",
	*pNumTable2, *pMaxTable2);
    return -1;
    }

/*--------------------------------------------------------------------
 | Se extraen los registros ocupados y se inicializan las posiciones
 | de cada clave.
 +--------------------------------------------------------------------*/
indOcu = 0;
indLib = *pNumTable2;
pElem  = bufElems;
pTable2 = (TypeTABLE2 *) DIR_MEM(*pPosTable2);
for (i = 0; i < *pMaxTable2; i++) {
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

{
        char * bufElems;
        TypeTABLE3 *pTable3;

/*--------------------------------------------------------------------
 | Se pide mem. dinamica para el leer el fichero imagen.
 +--------------------------------------------------------------------*/
bufElems = (char *) malloc( BUF_TABLE3 + 1);
if (bufElems == NULL)
    ERR_SistOper(ERR_FATAL, FUNC_BDT, "EXAM5 IniInformacion", "malloc",
                 "(%d bytes) path of base", BUF_TABLE3);



/*--------------------------------------------------------------------
 | Se posiciona en la posicion  de la tabla TABLE3 en el fichero
 +--------------------------------------------------------------------*/
if (lseek(fdFBD, POS_TABLE3, SEEK_SET) == -1) {
    ERR_SistOper(ERR_NO_FATAL, FUNC_BDT, "EXAM5: IniInformacion", "lseek",
		"file \"%s\" (position POS_TABLE3)", fichImagen);
    return -1;
    }

/*--------------------------------------------------------------------
 | Se leen todos los elementos de la tabla desde el fichero.
 +--------------------------------------------------------------------*/
if (read(fdFBD, bufElems, BUF_TABLE3) != BUF_TABLE3) {
    ERR_SistOper(ERR_NO_FATAL, FUNC_BDT, "EXAM5: IniInformacion", "read",
		 "file \"%s\" (BUF_TABLE3 bytes)", fichImagen);
    return -1;
    }

/*--------------------------------------------------------------------
 | Se cuenta el numero de elementos que tiene el fichero y se comprueba
 | que el dimensionado es suficiente.
 +--------------------------------------------------------------------*/
pElem = bufElems;
*pNumTable3 = 0;
for (i = 0; i < MAX_TABLE3; i++, pElem += LON_TABLE3)
    if (ESTA_OCUPADO(pElem))
	(*pNumTable3)++;

if (*pNumTable3 > *pMaxTable3) {
    ERR_Aplicacion(ERR_NO_FATAL, FUNC_BDT, "EXAM5: IniInformacion",
    	"el numero de elementos %d de TABLE3 supera el dimensionado %d",
	*pNumTable3, *pMaxTable3);
    return -1;
    }

/*--------------------------------------------------------------------
 | Se extraen los registros ocupados y se inicializan las posiciones
 | de cada clave.
 +--------------------------------------------------------------------*/
indOcu = 0;
indLib = *pNumTable3;
pElem  = bufElems;
pTable3 = (TypeTABLE3 *) DIR_MEM(*pPosTable3);
for (i = 0; i < *pMaxTable3; i++) {
    if (ESTA_OCUPADO(pElem)) {
	DecTable3(pElem, pTable3);
	indAct = indOcu;
	indOcu++;
	}
    else {
	indAct = indLib;
	indLib++;
	}
    clvSequenceTable3[indAct] = POS_MEM(pTable3);
    pElem += LON_TABLE3;
    pTable3++;
    }
  free(bufElems);
}

/*--------------------------------------------------------------------
 | Se ordenan los array de posiciones de las claves de la tabla.
 +--------------------------------------------------------------------*/
OrdTable3();


/*--------------------------------------------------------------------
 | Se cierra el fichero imagen de la base de datos.
 +--------------------------------------------------------------------*/
if (close(fdFBD) == -1) {
    ERR_SistOper(ERR_NO_FATAL, FUNC_BDT, "EXAM5: IniInformacion", "close",
		     "file \"%s\"", fichImagen);
    return -1;
    }
fdFBD = -1;

return 0;
}

/*====================== PROCEDIMIENTO PRIVADO ======================
 * CalcularMemoria
 *	Calcula el tamano de la memoria necesaria para la base
 *	la memoria:
 *====================================================================*/
static void CalcularMemoria()
{
/*--------------------------------------------------------------------
 | Memoria para:
 |	- la posicion de los elementos de las tablas.
 |	- el numero de elementos de las tablas.
 |	- la el numero maximo de elementos.
 +--------------------------------------------------------------------*/
lonMemoria  = sizeof(int) * 3 * 3;

/*--------------------------------------------------------------------
 | Memoria para la/s clave/s  los elementos de la tabla TABLE1
 +--------------------------------------------------------------------*/
lonMemoria += maxTable1 * sizeof(int *) * 2;
lonMemoria += maxTable1 * sizeof(int *) * 2;
lonMemoria += maxTable1 * TAM_TABLE1;

/*--------------------------------------------------------------------
 | Memoria para la/s clave/s  los elementos de la tabla TABLE2
 +--------------------------------------------------------------------*/
lonMemoria += maxTable2 * sizeof(int *) * 1;
lonMemoria += maxTable2 * TAM_TABLE2;

/*--------------------------------------------------------------------
 | Memoria para la/s clave/s  los elementos de la tabla TABLE3
 +--------------------------------------------------------------------*/
lonMemoria += maxTable3 * sizeof(int *) * 1;
lonMemoria += maxTable3 * TAM_TABLE3;
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
static void IniPunteros(int inicValores)
{
	char *pActual;

/*--------------------------------------------------------------------
 | Se inicializa pActual al comienzo de la zona de memoria de la base
 +--------------------------------------------------------------------*/
pActual = pMemoria;

/*--------------------------------------------------------------------
 | Se inicializan los punteros de acceso a la tabla TABLE1:
 |	pPosTable1: punt. a la informacion de los elementos.
 |	pNumTable1: punt. al numero de elementos.
 |	pMaxTable1: punt. al maximo numero de elementos.
 |	pOvfTable1: punt. al indic. de si la tabla admite overflow.
 |	clvSequenceTable1: array de acceso por la clave Sequence
 |	clvPairTable1: array de acceso por la clave Pair
 | avanzando el puntero de asignacion de memoria para cada puntero.
 +--------------------------------------------------------------------*/
pPosTable1 = (int *) pActual;
pActual += sizeof(int);
pNumTable1 = (int *) pActual;
pActual += sizeof(int);
pMaxTable1 = (int *) pActual;
pActual += sizeof(int);
pOvfTable1 = (int *) pActual;
pActual += sizeof(int);
clvSequenceTable1 = (int *) pActual;
pActual += (maxTable1 * sizeof(int *));
clvPairTable1 = (int *) pActual;
pActual += (maxTable1 * sizeof(int *));

/*--------------------------------------------------------------------
 | Se inicializan los valores en M. compartida de los punteros a los
 | valores actuales en cada caso.
 +--------------------------------------------------------------------*/
if (inicValores) {
    *pPosTable1 = POS_MEM(pActual);
    *pMaxTable1 = maxTable1;
    *pNumTable1 = 0;
    *pOvfTable1 = ovfTable1;
    }

/*--------------------------------------------------------------------
 | Se avanza el puntero de asignacion de memoria segun los elementos
 +--------------------------------------------------------------------*/
pActual += (maxTable1 * TAM_TABLE1);

/*--------------------------------------------------------------------
 | Se inicializan los punteros de acceso a la tabla TABLE2:
 |	pPosTable2: punt. a la informacion de los elementos.
 |	pNumTable2: punt. al numero de elementos.
 |	pMaxTable2: punt. al maximo numero de elementos.
 |	pOvfTable2: punt. al indic. de si la tabla admite overflow.
 |	clvSequenceTable2: array de acceso por la clave Sequence
 | avanzando el puntero de asignacion de memoria para cada puntero.
 +--------------------------------------------------------------------*/
pPosTable2 = (int *) pActual;
pActual += sizeof(int);
pNumTable2 = (int *) pActual;
pActual += sizeof(int);
pMaxTable2 = (int *) pActual;
pActual += sizeof(int);
pOvfTable2 = (int *) pActual;
pActual += sizeof(int);
clvSequenceTable2 = (int *) pActual;
pActual += (maxTable2 * sizeof(int *));

/*--------------------------------------------------------------------
 | Se inicializan los valores en M. compartida de los punteros a los
 | valores actuales en cada caso.
 +--------------------------------------------------------------------*/
if (inicValores) {
    *pPosTable2 = POS_MEM(pActual);
    *pMaxTable2 = maxTable2;
    *pNumTable2 = 0;
    *pOvfTable2 = ovfTable2;
    }

/*--------------------------------------------------------------------
 | Se avanza el puntero de asignacion de memoria segun los elementos
 +--------------------------------------------------------------------*/
pActual += (maxTable2 * TAM_TABLE2);

/*--------------------------------------------------------------------
 | Se inicializan los punteros de acceso a la tabla TABLE3:
 |	pPosTable3: punt. a la informacion de los elementos.
 |	pNumTable3: punt. al numero de elementos.
 |	pMaxTable3: punt. al maximo numero de elementos.
 |	pOvfTable3: punt. al indic. de si la tabla admite overflow.
 |	clvSequenceTable3: array de acceso por la clave Sequence
 | avanzando el puntero de asignacion de memoria para cada puntero.
 +--------------------------------------------------------------------*/
pPosTable3 = (int *) pActual;
pActual += sizeof(int);
pNumTable3 = (int *) pActual;
pActual += sizeof(int);
pMaxTable3 = (int *) pActual;
pActual += sizeof(int);
pOvfTable3 = (int *) pActual;
pActual += sizeof(int);
clvSequenceTable3 = (int *) pActual;
pActual += (maxTable3 * sizeof(int *));

/*--------------------------------------------------------------------
 | Se inicializan los valores en M. compartida de los punteros a los
 | valores actuales en cada caso.
 +--------------------------------------------------------------------*/
if (inicValores) {
    *pPosTable3 = POS_MEM(pActual);
    *pMaxTable3 = maxTable3;
    *pNumTable3 = 0;
    *pOvfTable3 = ovfTable3;
    }

/*--------------------------------------------------------------------
 | Se avanza el puntero de asignacion de memoria segun los elementos
 +--------------------------------------------------------------------*/
pActual += (maxTable3 * TAM_TABLE3);

}


/*====================== PROCEDIMIENTO PRIVADO ======================
 * CodTable1
 *	Codifica un TypeTABLE1 sobre un elemento Table1.
 *    Parametros:
 *      elem: buffer del elemento sobre el que codificar.
 *	pTable1: struct a realizar la codificacion.
 *====================================================================*/
static void CodTable1(char *elem, TypeTABLE1 *pTable1)
{
TPC_CodInt(&pTable1->mykey1, 1, elem + 1);
TPC_CodChar(&pTable1->pair, 1, elem + 5);
TPC_CodChar(pTable1->text, 50, elem + 6);
}

/*====================== PROCEDIMIENTO PRIVADO ======================
 * DecTable1
 *	Decodifica un elemento Table1 sobre un TypeTABLE1
 *    Parametros:
 *      elem: buffer del elemento a decodificar.
 *	pTable1: struct sobre la que realizar la decodificacion.
 *====================================================================*/
static void DecTable1(char *elem, TypeTABLE1 *pTable1)
{
TPC_DecInt(&pTable1->mykey1, 1, elem + 1);
TPC_DecChar(&pTable1->pair, 1, elem + 5);
TPC_DecChar(pTable1->text, 50, elem + 6);
}

/*========================= FUNCION PRIVADA =========================
 * OrdTable1
 *	Ordena los punt. de acceso a la tabla TABLE1, por:
 *		- SEQUENCE
 *		- PAIR
 *====================================================================*/
static void OrdTable1()
{
/*--------------------------------------------------------------------
 | Ordena el array de acceso a la tabla TABLE1 por las claves:
 |	- SEQUENCE
 |	- PAIR
 +--------------------------------------------------------------------*/
qsort(clvSequenceTable1, *pNumTable1,
      sizeof(int), (PF_COMPAR)CmpDesSequenceTable1);
qsort(clvPairTable1, *pNumTable1,
      sizeof(int), (PF_COMPAR)CmpDesPairTable1);
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
    case KEY_PAIR_TABLE1:
	return clvPairTable1;
    }
ERR_Aplicacion(ERR_NO_FATAL, FUNC_BDT, "EXAM5: ClaveTable1",
	"key code (%d) of table TABLE1 erroneous", codClave);
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
 *	pTable1:  punt. al elemento 1 a comparar la clave.
 *	pTable12: punt. al elemento 2 a comparar la clave.
 *    Devuelve:
 *      num > 0: si la clave de pTable1 es mayor que pTable12.
 *      0:	 si la clave de pTable1 es igual que pTable12.
 *      num < 0: si la clave de pTable1 es menor que pTable12.
 *====================================================================*/
static int CmpSequenceTable1(const TypeTABLE1 *pTable1,
				const TypeTABLE1 *pTable12)
{
	int result;

/*--------------------------------------------------------------------
 | Si el campo MYKEY1 del elemento 1 y 2 es distinto, se devuelve
 | el resultado de la comparacion.
 +--------------------------------------------------------------------*/
if ((result = CMP_VAL(pTable1->mykey1, pTable12->mykey1)) != 0) 
    return result;

return 0;
}

/*========================= FUNCION PRIVADA =========================
 * CmpDesPairTable1
 *	Compara la clave PAIR de dos elementos de la tabla
 *	TABLE1 segun sus posiciones en la memoria.
 *    Parametros:
 *	pPosic1: punt. a la posicion del elemento 1 a comparar la clave.
 *	pPosic2: punt. a la posicion del elemento 2 a comparar la clave.
 *    Devuelve:
 *      num > 0: si la clave del pPosic1 es mayor que el del pPosic2.
 *      0:	 si la clave del pPosic1 es igual que el del pPosic2.
 *      num < 0: si la clave del pPosic1 es menor que el del pPosic2.
 *====================================================================*/
static int CmpDesPairTable1(const int *pPosic1, const int *pPosic2)
{
return CmpPairTable1((TypeTABLE1 *) DIR_MEM(*pPosic1),
			(TypeTABLE1 *) DIR_MEM(*pPosic2));
}

/*========================= FUNCION PRIVADA =========================
 * CmpPairTable1
 *	Compara la clave PAIR de dos elementos de la tabla
 *	TABLE1.
 *    Parametros:
 *	pTable1:  punt. al elemento 1 a comparar la clave.
 *	pTable12: punt. al elemento 2 a comparar la clave.
 *    Devuelve:
 *      num > 0: si la clave de pTable1 es mayor que pTable12.
 *      0:	 si la clave de pTable1 es igual que pTable12.
 *      num < 0: si la clave de pTable1 es menor que pTable12.
 *====================================================================*/
static int CmpPairTable1(const TypeTABLE1 *pTable1,
				const TypeTABLE1 *pTable12)
{
	int result;

/*--------------------------------------------------------------------
 | Si el campo PAIR del elemento 1 y 2 es distinto, se devuelve
 | el resultado de la comparacion.
 +--------------------------------------------------------------------*/
if ((result = CMP_VAL(pTable1->pair, pTable12->pair)) != 0) 
    return result;

return 0;
}

/*====================== PROCEDIMIENTO PRIVADO ======================
 * CodTable2
 *	Codifica un TypeTABLE2 sobre un elemento Table2.
 *    Parametros:
 *      elem: buffer del elemento sobre el que codificar.
 *	pTable2: struct a realizar la codificacion.
 *====================================================================*/
static void CodTable2(char *elem, TypeTABLE2 *pTable2)
{
TPC_CodInt(&pTable2->mykey2, 1, elem + 1);
TPC_CodInt(&pTable2->keytable1, 1, elem + 5);
TPC_CodChar(pTable2->text, 50, elem + 9);
}

/*====================== PROCEDIMIENTO PRIVADO ======================
 * DecTable2
 *	Decodifica un elemento Table2 sobre un TypeTABLE2
 *    Parametros:
 *      elem: buffer del elemento a decodificar.
 *	pTable2: struct sobre la que realizar la decodificacion.
 *====================================================================*/
static void DecTable2(char *elem, TypeTABLE2 *pTable2)
{
TPC_DecInt(&pTable2->mykey2, 1, elem + 1);
TPC_DecInt(&pTable2->keytable1, 1, elem + 5);
TPC_DecChar(pTable2->text, 50, elem + 9);
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
qsort(clvSequenceTable2, *pNumTable2,
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
ERR_Aplicacion(ERR_NO_FATAL, FUNC_BDT, "EXAM5: ClaveTable2",
	"key code (%d) of table TABLE2 erroneous", codClave);
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
 *	pTable2:  punt. al elemento 1 a comparar la clave.
 *	pTable22: punt. al elemento 2 a comparar la clave.
 *    Devuelve:
 *      num > 0: si la clave de pTable2 es mayor que pTable22.
 *      0:	 si la clave de pTable2 es igual que pTable22.
 *      num < 0: si la clave de pTable2 es menor que pTable22.
 *====================================================================*/
static int CmpSequenceTable2(const TypeTABLE2 *pTable2,
				const TypeTABLE2 *pTable22)
{
	int result;

/*--------------------------------------------------------------------
 | Si el campo MYKEY2 del elemento 1 y 2 es distinto, se devuelve
 | el resultado de la comparacion.
 +--------------------------------------------------------------------*/
if ((result = CMP_VAL(pTable2->mykey2, pTable22->mykey2)) != 0) 
    return result;

return 0;
}

/*====================== PROCEDIMIENTO PRIVADO ======================
 * CodTable3
 *	Codifica un TypeTABLE3 sobre un elemento Table3.
 *    Parametros:
 *      elem: buffer del elemento sobre el que codificar.
 *	pTable3: struct a realizar la codificacion.
 *====================================================================*/
static void CodTable3(char *elem, TypeTABLE3 *pTable3)
{
TPC_CodInt(&pTable3->mykey3, 1, elem + 1);
TPC_CodInt(&pTable3->keytable1, 1, elem + 5);
TPC_CodChar(pTable3->ispair, 50, elem + 9);
}

/*====================== PROCEDIMIENTO PRIVADO ======================
 * DecTable3
 *	Decodifica un elemento Table3 sobre un TypeTABLE3
 *    Parametros:
 *      elem: buffer del elemento a decodificar.
 *	pTable3: struct sobre la que realizar la decodificacion.
 *====================================================================*/
static void DecTable3(char *elem, TypeTABLE3 *pTable3)
{
TPC_DecInt(&pTable3->mykey3, 1, elem + 1);
TPC_DecInt(&pTable3->keytable1, 1, elem + 5);
TPC_DecChar(pTable3->ispair, 50, elem + 9);
}

/*========================= FUNCION PRIVADA =========================
 * OrdTable3
 *	Ordena los punt. de acceso a la tabla TABLE3, por:
 *		- SEQUENCE
 *====================================================================*/
static void OrdTable3()
{
/*--------------------------------------------------------------------
 | Ordena el array de acceso a la tabla TABLE3 por las claves:
 |	- SEQUENCE
 +--------------------------------------------------------------------*/
qsort(clvSequenceTable3, *pNumTable3,
      sizeof(int), (PF_COMPAR)CmpDesSequenceTable3);
}

/*========================= FUNCION PRIVADA =========================
 * ClaveTable3
 *	Devuelve el array de acceso correspondiente el codigo de
 *	una determinada clave de la tabla TABLE3.
 *
 *    Parametros:
 *	codClave: codigo de la clave.
 *    Devuelve:
 *	el array de acceso de la clave: si el codigo es correcto.
 *	NULL: si el codigo es erroneo.
 *====================================================================*/
static int *ClaveTable3(const int codClave)
{
switch(codClave) {
    case KEY_SEQUENCE_TABLE3:
	return clvSequenceTable3;
    }
ERR_Aplicacion(ERR_NO_FATAL, FUNC_BDT, "EXAM5: ClaveTable3",
	"key code (%d) of table TABLE3 erroneous", codClave);
return NULL;
}

/*========================= FUNCION PRIVADA =========================
 * CmpDesSequenceTable3
 *	Compara la clave SEQUENCE de dos elementos de la tabla
 *	TABLE3 segun sus posiciones en la memoria.
 *    Parametros:
 *	pPosic1: punt. a la posicion del elemento 1 a comparar la clave.
 *	pPosic2: punt. a la posicion del elemento 2 a comparar la clave.
 *    Devuelve:
 *      num > 0: si la clave del pPosic1 es mayor que el del pPosic2.
 *      0:	 si la clave del pPosic1 es igual que el del pPosic2.
 *      num < 0: si la clave del pPosic1 es menor que el del pPosic2.
 *====================================================================*/
static int CmpDesSequenceTable3(const int *pPosic1, const int *pPosic2)
{
return CmpSequenceTable3((TypeTABLE3 *) DIR_MEM(*pPosic1),
			(TypeTABLE3 *) DIR_MEM(*pPosic2));
}

/*========================= FUNCION PRIVADA =========================
 * CmpSequenceTable3
 *	Compara la clave SEQUENCE de dos elementos de la tabla
 *	TABLE3.
 *    Parametros:
 *	pTable3:  punt. al elemento 1 a comparar la clave.
 *	pTable32: punt. al elemento 2 a comparar la clave.
 *    Devuelve:
 *      num > 0: si la clave de pTable3 es mayor que pTable32.
 *      0:	 si la clave de pTable3 es igual que pTable32.
 *      num < 0: si la clave de pTable3 es menor que pTable32.
 *====================================================================*/
static int CmpSequenceTable3(const TypeTABLE3 *pTable3,
				const TypeTABLE3 *pTable32)
{
	int result;

/*--------------------------------------------------------------------
 | Si el campo MYKEY3 del elemento 1 y 2 es distinto, se devuelve
 | el resultado de la comparacion.
 +--------------------------------------------------------------------*/
if ((result = CMP_VAL(pTable3->mykey3, pTable32->mykey3)) != 0) 
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
 | Se abre el fichero imagen de la Configuracion (si no esta abierto)
 +--------------------------------------------------------------------*/
if (fdFBD < 0) {
    fdFBD = open(fichImagen, O_WRONLY);
    if (fdFBD == -1)
	ERR_SistOper(ERR_FATAL, FUNC_BDT, "EXAM5: ActElemento", "open",
			"file \"%s\" (for write)", fichImagen);
    }

/*--------------------------------------------------------------------
 | Se prepara la escritura sobre la posicion del elemento.
 +--------------------------------------------------------------------*/
if (lseek(fdFBD, (off_t)posFic, SEEK_SET) == -1) {
    ERR_SistOper(ERR_NO_FATAL, FUNC_BDT, "EXAM5: ActElemento", "lseek",
		 "file \"%s\" (position %ld)", fichImagen, posFic);
    return -1;
    }

/*--------------------------------------------------------------------
 | Se escribe el elemento sobre el fichero.
 +--------------------------------------------------------------------*/
if (write(fdFBD, (char *)eleFic, lonFic) != lonFic) {
    ERR_SistOper(ERR_NO_FATAL, FUNC_BDT, "EXAM5: ActElemento", "write",
		 "file \"%s\" (%d bytes)", fichImagen, lonFic);
    return -1;
    }

/*--------------------------------------------------------------------
 | Se escribe el elemento sobre la memoria.
 +--------------------------------------------------------------------*/
memcpy(DIR_MEM(posMem), eleMem, (size_t)lonMem);

/*--------------------------------------------------------------------
 | Se cierra el fichero imagen de la base de datos.
 +--------------------------------------------------------------------*/
if (! fichAbierto) {
    if (close(fdFBD) == -1) {
	ERR_SistOper(ERR_NO_FATAL, FUNC_BDT, "EXAM5: IniInformacion", "close",
		     "file \"%s\"", fichImagen);
	return -1;
	}
    fdFBD = -1;
    }

return 0;
}

/*====================== PROCEDIMIENTO PRIVADO ======================
 * PosicionLibre
 *	Deja libre una posicion en un array de posiciones.
 *
 *    Parametros:
 *	puntero: 
 *	array: 
 *	numElem: 
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
/*========================= FUNCION PUBLICA =========================
 * EXAM5_NumRecords
 *      Informa del numero de registros que tiene la tabla.
 *
 *    Parametros:
 *      - tabla: identificador de la tabla a dimensionar.
 *    Devuelve:
 *       n: el numero de registros.
 *      -1: si la tabla no es correcta
 *====================================================================*/
int EXAM5_NumRecords(int tabla)
{
switch(tabla) {
    case TBL_TABLE1:
        return *pNumTable1;
    case TBL_TABLE2:
        return *pNumTable2;
    case TBL_TABLE3:
        return *pNumTable3;
    }
return -1;
}

