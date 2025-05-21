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
#include <gen/tpc.h>
#include <gen/err.h>
#include <aer.h>
 
/*--------------------------------------------------------------------
 | PF_COMPAR: punt. a funcion comparac. para "qsort" y "BuscaElemento"
 +--------------------------------------------------------------------*/
typedef int (*PF_COMPAR)(const void *, const void *);

/*--------------------------------------------------------------------
 | BYTE:     sustituye a "unsigned char" en el modulo.
 | FUNC_BDT: nombre de la funcion para los errores.
 +--------------------------------------------------------------------*/
#define BYTE		unsigned char
#define FUNC_BDT	"Almacen "
 
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
 | TAM_ERROR: tamano de la struct TipoERROR
 | IND_ERROR: indice de un elemento segun una posicion en memoria.
 |
 | LON_ERROR: longitud del elemento ERROR en el fichero
 | POS_ERROR: posicion de ERROR en el fichero
 | BUF_ERROR: tamano de ERROR en el fichero.
 | ELE_ERROR: posicion de un elemento en el fichero segun el indice
 +--------------------------------------------------------------------*/
#define TAM_ERROR	(sizeof(TipoERROR))
#define IND_ERROR(pos)	((pos - posError) / TAM_ERROR)

#define LON_ERROR	210
#define POS_ERROR	332L
#define BUF_ERROR	(MAX_ERROR * LON_ERROR)
#define ELE_ERROR(i)	(long)(POS_ERROR+(LON_ERROR*i))

/*====================================================================
 *		FUNCIONES DE INICIALIZACION DE LA BASE
 *====================================================================*/
static void IniPunteros();
static int  IniInformacion();

/*====================================================================
 *	FUNCIONES DE TRATAMIENTO DE LA TABLA ERROR
 *====================================================================*/
static void CodError(char *, TipoERROR *);
static void DecError(char *, TipoERROR *);
static void OrdError();
static int *ClaveError(const int);
static int  CmpSecuenciaError(const TipoERROR *,
				const TipoERROR *);
static int  CmpDesSecuenciaError(const int *, const int *); 
static int  CmpAplicacionError(const TipoERROR *,
				const TipoERROR *);
static int  CmpDesAplicacionError(const int *, const int *); 

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
static char *fichImagen  = "AER.ibd";
static int   fdFBD	 = -1;
static int   fichAbierto = 1;
static char *pMemoria	 = NULL;
static int   lonMemoria	 = 0;

/*--------------------------------------------------------------------
 | Variables asociadas a la tabla ERROR:
 |	posError: posicion de los elementos.
 |	numError: punt. al numero de elementos.
 |	maxError: numero maximo de elementos (redimensionado).
 |	ovfError: indicativo de si la tabla admite overflow.
 |	clvSecuenciaError: indice de acceso por la clave SECUENCIA
 |	clvAplicacionError: indice de acceso por la clave APLICACION
 +--------------------------------------------------------------------*/
static int  posError = 0;
static int *numError = NULL;
static int  maxError = MAX_ERROR;
static int  ovfError = 0;
static int *clvSecuenciaError = NULL;
static int *clvAplicacionError = NULL;


/*====================== PROCEDIMIENTO PUBLICO ======================
 * AER_Directorio
 *	Notifica el directorio de donde  se debe cogen el fichero
 *	imagen de la base.
 *
 *    Parametros:
 *	- directorio: path del directorio.
 *====================================================================*/
void AER_Directorio(char *directorio)
{
	char *nuevoPath;

/*--------------------------------------------------------------------
 | Se pide mem. dinamica para el path del fichero imagen.
 +--------------------------------------------------------------------*/
nuevoPath = (char *) malloc(strlen(directorio) + sizeof("AER.ibd") + 1);
if (nuevoPath == NULL)
    ERR_SistOper(ERR_FATAL, FUNC_BDT, "AER_Directorio", "malloc",
		 "(%d bytes) path de la base", lonMemoria);

/*--------------------------------------------------------------------
 | Se compone el nuevo path.
 +--------------------------------------------------------------------*/
sprintf(nuevoPath, "%s/AER.ibd", directorio);
fichImagen = nuevoPath;
}

/*====================== PROCEDIMIENTO PUBLICO ======================
 * AER_Dimensionar
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
void AER_Dimensionar(int tabla, int dimension)
{
switch(tabla) {
    case TBL_ERROR:
	/*------------------------------------------------------------
	 | Si la dimension es mayor que la definida como maxima para la
	 | tabla ERROR no se acepta y se da un error.
	 +------------------------------------------------------------*/
	if (dimension > MAX_ERROR)
	    ERR_Aplicacion(ERR_NO_FATAL, FUNC_BDT, "AER_Dimensionar",
	      "redimensionado %d de ERROR excesivo (max: 5000)",
	       dimension);
	else
	    maxError = dimension;
	break;
    }
}

/*====================== PROCEDIMIENTO PUBLICO ======================
 * AER_Desbordamiento
 *	Notifica si se quiere el desbordamiento activado o no.
 *	Si el codigo de tabla no es correcto no se hace nada.
 *
 *    Parametros:
 *	- tabla: identificador de la tabla.
 *	- desbord: valor 0/1 (NO/SI) de admision de desbordamiento.
 *====================================================================*/
void AER_Desbordamiento(int tabla, int desbord)
{
switch(tabla) {
    case TBL_ERROR:
	ovfError = desbord;
        break;
    }
}

/*========================= FUNCION PUBLICA =========================
 * AER_Inicializar
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
int AER_Inicializar(int dejarAbierto)
{
/*--------------------------------------------------------------------
 | Se calcula la memoria (segun el dimensionado) necesaria para la base.
 +--------------------------------------------------------------------*/
lonMemoria  = sizeof(int) * 1 ;
lonMemoria += maxError * sizeof(int *) * 2;
lonMemoria += maxError * sizeof(int *) * 2;
lonMemoria += maxError * TAM_ERROR;

/*--------------------------------------------------------------------
 | Se pide mem. dinamica para la base da datos.
 +--------------------------------------------------------------------*/
pMemoria = (char *) malloc(lonMemoria);
if (pMemoria == NULL) {
    ERR_SistOper(ERR_NO_FATAL, FUNC_BDT, "AER_Inicializar", "malloc",
    		 "(%d bytes) memoria para la base", lonMemoria);
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
 * AER_Finalizar
 *	Finaliza el acceso a la base de datos
 *====================================================================*/
int AER_Finalizar()
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
	ERR_SistOper(ERR_NO_FATAL, FUNC_BDT, "AER_Finalizar", "close",
			"fichero \"%s\"", fichImagen);
	return -1;
	}
return 0;
}

/*========================= FUNCION PUBLICA =========================
 * AER_NumRegistros
 *	Informa del numero de registros que tiene la tabla.
 *
 *    Parametros:
 *	- tabla: identificador de la tabla a dimensionar.
 *    Devuelve:
 *	 n: el numero de registros.
 *	-1: si la tabla no es correcta
 *====================================================================*/
int AER_NumRegistros(int tabla)
{
switch(tabla) {
    case TBL_ERROR:
	return *numError;
    }
return -1;
}


/*========================= FUNCION PUBLICA =========================
 * AER_InsError
 *	Inserta un nuevo elemento ERROR, segun la clave
 *	principal, para lo cual la struct debe tener todos los
 *	campos de las claves con los valores actualizados.
 *
 *    Parametros:
 *	perror: punt. a la struct del elemento a insertar.
 *    Devuelve:
 *	 0: si la insercion se realiza correctamente.
 *	-1: si se produce un error.
 *====================================================================*/
int AER_InsError(TipoERROR *pError)
{
	char elemCod[LON_ERROR];
	int  posicMem;
	int  posicFic;
	int  indElem;
	int  numElem;
	int  indClave;

/*--------------------------------------------------------------------
 | Se obtiene el numero actual de elementos.
 +--------------------------------------------------------------------*/
numElem = *numError;

/*--------------------------------------------------------------------
 | No debe existir otro elemento con igual valor en la clave principal
 +--------------------------------------------------------------------*/
if (BuscaElemento(pError, clvSecuenciaError,
		  numElem, (PF_COMPAR)CmpSecuenciaError) >= 0) {
    ERR_Aplicacion(ERR_NO_FATAL, FUNC_BDT, "AER_InsError",
		 "clave SECUENCIA de ERROR ya existente");
    return -1;
    }

/*--------------------------------------------------------------------
 | No debe existir otro elemento con igual clave unica: APLICACION
 +--------------------------------------------------------------------*/
if (BuscaElemento(pError, clvAplicacionError,
		  numElem, (PF_COMPAR)CmpAplicacionError) >= 0) {
    ERR_Aplicacion(ERR_NO_FATAL, FUNC_BDT, "AER_InsError",
		   "clave APLICACION de ERROR ya existente");
    return -1;
    }

/*--------------------------------------------------------------------
 | Si hay elementos libres, se toma para insercion el primero libre.
 | Si se sobrepasa el limite de elementos max5000 solo se
 | acepta si se admite overflow. En ese caso se inserta sobre el ultimo
 | elemento que es el elemento 0 y que tiene la clave menor.
 +--------------------------------------------------------------------*/
if (numElem < maxError) {
    indClave = *numError;
    numElem++;
    }
else if (ovfError)
    indClave = 0;
else {
    ERR_Aplicacion(ERR_NO_FATAL, FUNC_BDT, "AER_InsError",
	      "alcanzado el maximo de ERROR (%d)", maxError);
    return -1;
    }

/*--------------------------------------------------------------------
 | Se calculan las posiciones para la actualizacion.
 +--------------------------------------------------------------------*/
posicMem = clvSecuenciaError[indClave];
indElem  = IND_ERROR(posicMem);
posicFic = ELE_ERROR(indElem);


/*--------------------------------------------------------------------
 | Codifica la struct TipoERROR sobre el elemento como OCUPADO
 +--------------------------------------------------------------------*/
CodError(elemCod, pError);
PONER_OCUPADO(elemCod);
if (ActElemento(elemCod, LON_ERROR, posicFic,
		pError, TAM_ERROR, posicMem) == -1) {
    return -1;
    }

*numError = numElem;
OrdError();
return 0;
}

/*========================= FUNCION PUBLICA =========================
 * AER_EliError
 *	Elimina un elemento ERROR ya existente, segun la
 *	clave Principal.
 *
 *    Parametros:
 *	pError: punt. a la struct del elemento a eliminar.
 *    Devuelve:
 *	 0: si la eliminacion se realiza correctamente.
 *	-1: si se produce un error.
 *====================================================================*/
int AER_EliError(TipoERROR *pError)
{
	char elemCod[LON_ERROR];
	int  posicMem;
	int  posicFic;
	int  indElem;

/*--------------------------------------------------------------------
 | No debe existir otro elemento con igual valor en la clave principal
 +--------------------------------------------------------------------*/
indElem = BuscaElemento(pError, clvSecuenciaError,
		*numError, (PF_COMPAR)CmpSecuenciaError);
if (indElem == -1) {
    ERR_Aplicacion(ERR_NO_FATAL, FUNC_BDT, "AER_EliError",
		 "clave SECUENCIA de ERROR no existente");
    return -1;
    }

/*--------------------------------------------------------------------
 | Se calculan las posiciones para la actualizacion.
 +--------------------------------------------------------------------*/
posicMem = clvSecuenciaError[indElem];
indElem  = IND_ERROR(posicMem);
posicFic = ELE_ERROR(indElem);

/*--------------------------------------------------------------------
 | Codifica la struct TipoERROR sobre el elemento como OCUPADO
 +--------------------------------------------------------------------*/
memset(elemCod, 0, LON_ERROR);
PONER_LIBRE(elemCod);
if (ActElemento(elemCod, LON_ERROR, posicFic,
		pError, TAM_ERROR, posicMem) == -1)
    return -1;

PosicionLibre(posicMem, clvSecuenciaError, *numError);
PosicionLibre(posicMem, clvAplicacionError, *numError);
(*numError)--;
return 0;
}

/*========================= FUNCION PUBLICA =========================
 * AER_ModError
 *	Modifica un elemento ERROR ya existente, segun la
 *	clave principal, para lo cual la struct debe tener todos los
 *	campos con los valores actualizados.
 *
 *    Parametros:
 *	error: punt. a la struct del elemento a eliminar.
 *    Devuelve:
 *	 0: si la modificacion se realiza correctamente.
 *	-1: si se produce un error.
 *====================================================================*/
int AER_ModError(TipoERROR *pError)
{
	char elemCod[LON_ERROR];
	int  posicMem;
	int  posicFic;
	int  indElem;

/*--------------------------------------------------------------------
 | No debe existir otro elemento con igual valor en la clave principal
 +--------------------------------------------------------------------*/
indElem = BuscaElemento(pError, clvSecuenciaError,
		*numError, (PF_COMPAR)CmpSecuenciaError);
if (indElem == -1) {
    ERR_Aplicacion(ERR_NO_FATAL, FUNC_BDT, "AER_ModError",
		   "clave SECUENCIA de ERROR no existente");
    return -1;
    }

/*--------------------------------------------------------------------
 | Se calculan las posiciones para la actualizacion.
 +--------------------------------------------------------------------*/
posicMem = clvSecuenciaError[indElem];
indElem  = IND_ERROR(posicMem);
posicFic = ELE_ERROR(indElem);

/*--------------------------------------------------------------------
 | Las clave unica APLICACION debe coincidir con la actual.
 +--------------------------------------------------------------------*/
if (CmpAplicacionError(pError,
		(TipoERROR *) DIR_MEM(posicMem)) != 0) {
    ERR_Aplicacion(ERR_NO_FATAL, FUNC_BDT, "AER_ModError",
		   "clave APLICACION de ERROR no coincide");
    return -1;
    }


/*--------------------------------------------------------------------
 | Codifica la struct TipoERROR sobre el elemento como OCUPADO
 +--------------------------------------------------------------------*/
CodError(elemCod, pError);
PONER_OCUPADO(elemCod);
if (ActElemento(elemCod, LON_ERROR, posicFic,
		pError, TAM_ERROR, posicMem) == -1)
    return -1;

return 0;
}

/*========================= FUNCION PUBLICA =========================
 * AER_ErrorPorSecuencia
 *	Busca un elemento ERROR , por la clave principal
 *	para lo cual la struct debe tener todos los campos de la
 *	clave con los valores actualizados.
 *
 *    Parametros:
 *	error: punt. a la struct del elemento a buscar.
  *	secuencia: campo SECUENCIA de busqueda.
  *    Devuelve:
 *	 0: si la modificacion se realiza correctamente.
 *	-1: si se produce un error.
 *	Busca un elemento
 *====================================================================*/
int AER_ErrorPorSecuencia(TipoERROR *pError,
				int            secuencia)
{
	int  indElem;

/*--------------------------------------------------------------------
 | Se codifican los campos de la clave principal sobre el elemento.
 +--------------------------------------------------------------------*/
pError->secuencia = secuencia;

/*--------------------------------------------------------------------
 | Se busca el elemento accediendo por la clave principal y si no
 | existe se devuelve -1.
 +--------------------------------------------------------------------*/
indElem = BuscaElemento(pError, clvSecuenciaError,
	      *numError, (PF_COMPAR)CmpSecuenciaError);
if (indElem == -1)
    return -1;

/*--------------------------------------------------------------------
 | Si existe, se decodifica el elemento completo sobre la struct.
 +--------------------------------------------------------------------*/
memcpy(pError, DIR_MEM(clvSecuenciaError[indElem]), TAM_ERROR);
return 0;
}

/*========================= FUNCION PUBLICA =========================
 * AER_VolError
 *	Realiza un volcado de la tabla ERROR ordenado por
 *	la clave segun el parametro.
 *    Parametros:
 *	claveOrd: codigo de la clave para ordenar el volcado.
 *	pLista: puntero para la lista del volcado.
 *    Devuelve:
 *	n: el numero de elementos seleccionados.
 *	-1: si hay error en el acceso.
 *====================================================================*/
int AER_VolError(int claveOrd, TipoERROR **pLista)
{
	int i;
	TipoERROR *pError;
	int *claveAcceso;

/*--------------------------------------------------------------------
 | Se recoge el array de acceso por la clave definida por "claveOrd"
 +--------------------------------------------------------------------*/
claveAcceso = ClaveError(claveOrd);
if (claveAcceso == NULL)
    return -1;

/*--------------------------------------------------------------------
 | Si la tabla esta vacia no se hace nada mas y se devuelve 0.
 +--------------------------------------------------------------------*/
if (*numError == 0)
    return 0;

/*--------------------------------------------------------------------
 | Se pide memoria dinamica para almacenar los elementos del recorrido.
 +--------------------------------------------------------------------*/
pError = (TipoERROR *) malloc(*numError * TAM_ERROR);
if (pError == NULL) {
    ERR_SistOper(ERR_NO_FATAL, FUNC_BDT, "AER_VolError",
	     "malloc", "(%d bytes) para volcado de Error", lonMemoria);
    return -1;
    }

*pLista = pError;

/*--------------------------------------------------------------------
 | Se copia la informacion de cada elemento de la lista segun la
 | ordenacion de la clave.
 +--------------------------------------------------------------------*/
for (i = 0; i < *numError; i++) {
    memcpy(pError, DIR_MEM(claveAcceso[i]), TAM_ERROR);
    pError++;
    }

return i;
}

/*========================= FUNCION PUBLICA =========================
 * AER_SelErrorPorCampos
 *	Realiza una seleccion de la tabla ERROR segun el
 *	criterio CAMPOS y ordenado por una clave segun parametro.
 *
 *    Parametros:
 *	claveOrd: codigo de la clave para ordenar el volcado.
 *	pLista: puntero a la lista de la seleccion.
 *	secdesde: valor para la seleccion.
 *	sechasta: valor para la seleccion.
 *    Devuelve:
 *	n: numero de elementos seleccionados (puede ser 0).
 *	-1: si hay error en la seleccion.
 *====================================================================*/
int AER_SelErrorPorCampos(int claveOrd,
			TipoERROR **pLista,
			int secDesde,
			int secHasta)
{
	int   i;
	int   totSelec;
	TipoERROR *pError;
	TipoERROR lstSelec[MAX_ERROR];
	int *claveAcceso;

/*--------------------------------------------------------------------
 | Se recoge el array de acceso por la clave definida por "claveOrd"
 +--------------------------------------------------------------------*/
claveAcceso = ClaveError(claveOrd);
if (claveAcceso == NULL)
    return -1;

/*--------------------------------------------------------------------
 | Si la tabla esta vacia no se hace nada mas y se devuelve 0.
 +--------------------------------------------------------------------*/
if (*numError == 0)
    return 0;

/*--------------------------------------------------------------------
 | Se recorre toda la tabla, decodificando cada elemento sobre el
 | elemento siguiente de la lista. Si cumple la condicion de seleccion
 | se avanza el puntero al siguiente elemento y se incrementa el numero
 | de elementos seleccionados.
 +--------------------------------------------------------------------*/
totSelec = 0;
for (i = 0; i < *numError; i++) {
    pError = (TipoERROR *) DIR_MEM(claveAcceso[i]);
    if ((pError->secuencia >= secDesde) ||
	(pError->secuencia <= secHasta)) {
	memcpy(lstSelec + totSelec, pError, TAM_ERROR);
	totSelec++;
	}
    }

/*--------------------------------------------------------------------
 | Se pide memoria dinamica para almacenar los elementos de seleccion
 | (solo si se ha seleccionado alguno y se copian sobre esta memoria.
 +--------------------------------------------------------------------*/
i = totSelec * TAM_ERROR;
if (i > 0) {
    *pLista = (TipoERROR *) malloc((size_t) i);
    if (*pLista == NULL) {
	ERR_SistOper(ERR_NO_FATAL, FUNC_BDT,
		"AER_SelErrorPorCampos", "malloc",
		"(%d bytes) para la seleccion Campos de Error", i);
	return -1;
	}
    memcpy((void *) *pLista, (void *) lstSelec, i);
    }
return totSelec;
}
/*========================= FUNCION PUBLICA =========================
 * AER_SelErrorPorFuncion
 *	Realiza una seleccion de la tabla ERROR segun el
 *	criterio FUNCION y ordenado por una clave segun parametro.
 *
 *    Parametros:
 *	claveOrd: codigo de la clave para ordenar el volcado.
 *	pLista: puntero a la lista de la seleccion.
 *	secdesde: valor para la seleccion.
 *    Devuelve:
 *	n: numero de elementos seleccionados (puede ser 0).
 *	-1: si hay error en la seleccion.
 *====================================================================*/
int AER_SelErrorPorFuncion(int claveOrd,
			TipoERROR **pLista,
			char *secDesde)
{
	int   i;
	int   totSelec;
	TipoERROR *pError;
	TipoERROR lstSelec[MAX_ERROR];
	int *claveAcceso;

/*--------------------------------------------------------------------
 | Se recoge el array de acceso por la clave definida por "claveOrd"
 +--------------------------------------------------------------------*/
claveAcceso = ClaveError(claveOrd);
if (claveAcceso == NULL)
    return -1;

/*--------------------------------------------------------------------
 | Si la tabla esta vacia no se hace nada mas y se devuelve 0.
 +--------------------------------------------------------------------*/
if (*numError == 0)
    return 0;

/*--------------------------------------------------------------------
 | Se recorre toda la tabla, decodificando cada elemento sobre el
 | elemento siguiente de la lista. Si cumple la condicion de seleccion
 | se avanza el puntero al siguiente elemento y se incrementa el numero
 | de elementos seleccionados.
 +--------------------------------------------------------------------*/
totSelec = 0;
for (i = 0; i < *numError; i++) {
    pError = (TipoERROR *) DIR_MEM(claveAcceso[i]);
    if ((strcmp(pError->funcion, secDesde) >= 0)) {
	memcpy(lstSelec + totSelec, pError, TAM_ERROR);
	totSelec++;
	}
    }

/*--------------------------------------------------------------------
 | Se pide memoria dinamica para almacenar los elementos de seleccion
 | (solo si se ha seleccionado alguno y se copian sobre esta memoria.
 +--------------------------------------------------------------------*/
i = totSelec * TAM_ERROR;
if (i > 0) {
    *pLista = (TipoERROR *) malloc((size_t) i);
    if (*pLista == NULL) {
	ERR_SistOper(ERR_NO_FATAL, FUNC_BDT,
		"AER_SelErrorPorFuncion", "malloc",
		"(%d bytes) para la seleccion Funcion de Error", i);
	return -1;
	}
    memcpy((void *) *pLista, (void *) lstSelec, i);
    }
return totSelec;
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
    ERR_Aplicacion(ERR_NO_FATAL, FUNC_BDT, "AER: IniInformacion",
		   "fichero \"%s\" no existente", fichImagen);
    return -1;
    }

/*--------------------------------------------------------------------
 | Se mira si el fichero imagen de Config. tiene permiso de lectura
 +--------------------------------------------------------------------*/
if (access(fichImagen, R_OK) == -1) {
    ERR_Aplicacion(ERR_NO_FATAL, FUNC_BDT, "AER: IniInformacion",
		   "fichero \"%s\" no accesible para lectura", fichImagen);
    return -1;
    }

/*--------------------------------------------------------------------
 | Se mira si el fichero imagen de Config. tiene permiso de escritura
 +--------------------------------------------------------------------*/
if (access(fichImagen, W_OK) == -1) {
    ERR_Aplicacion(ERR_NO_FATAL, FUNC_BDT, "AER: IniInformacion",
		   "fichero \"%s\" no accesible para escritura", fichImagen);
    return -1;
    }

/*--------------------------------------------------------------------
 | Se abre el fichero imagen de la base de datos
 +--------------------------------------------------------------------*/
fdFBD = open(fichImagen, O_RDWR | O_EXCL);
if (fdFBD == -1) {
    ERR_SistOper(ERR_NO_FATAL, FUNC_BDT, "AER: IniInformacion", "open",
		 "fichero \"%s\" (para lectura/escritura))", fichImagen);
    return -1;
    }
 
{
	char bufElems[BUF_ERROR];
	TipoERROR *pError;

/*--------------------------------------------------------------------
 | Se posiciona en la posicion  de la tabla ERROR en el fichero
 +--------------------------------------------------------------------*/
if (lseek(fdFBD, POS_ERROR, SEEK_SET) == -1) {
    ERR_SistOper(ERR_NO_FATAL, FUNC_BDT, "AER: IniInformacion", "lseek",
		"fichero \"%s\" (posicion POS_ERROR)", fichImagen);
    return -1;
    }

/*--------------------------------------------------------------------
 | Se leen todos los elementos de la tabla desde el fichero.
 +--------------------------------------------------------------------*/
if (read(fdFBD, bufElems, BUF_ERROR) != BUF_ERROR) {
    ERR_SistOper(ERR_NO_FATAL, FUNC_BDT, "AER: IniInformacion", "read",
		 "fichero \"%s\" (BUF_ERROR bytes)", fichImagen);
    return -1;
    }

/*--------------------------------------------------------------------
 | Se cuenta el numero de elementos que tiene el fichero y se comprueba
 | que el dimensionado es suficiente.
 +--------------------------------------------------------------------*/
pElem = bufElems;
*numError = 0;
for (i = 0; i < MAX_ERROR; i++, pElem += LON_ERROR)
    if (ESTA_OCUPADO(pElem))
	(*numError)++;

if (*numError > maxError) {
    ERR_Aplicacion(ERR_NO_FATAL, FUNC_BDT, "AER: IniInformacion",
    	"el numero de elementos %d de ERROR supera el dimensionado %d",
	*numError, maxError);
    return -1;
    }

/*--------------------------------------------------------------------
 | Se extraen los registros ocupados y se inicializan las posiciones
 | de cada clave.
 +--------------------------------------------------------------------*/
indOcu = 0;
indLib = *numError;
pElem  = bufElems;
pError = (TipoERROR *) DIR_MEM(posError);
for (i = 0; i < maxError; i++) {
    if (ESTA_OCUPADO(pElem)) {
	DecError(pElem, pError);
	indAct = indOcu;
	indOcu++;
	}
    else {
	indAct = indLib;
	indLib++;
	}
    clvSecuenciaError[indAct] = POS_MEM(pError);
    clvAplicacionError[indAct] = POS_MEM(pError);
    pElem += LON_ERROR;
    pError++;
    }
}

/*--------------------------------------------------------------------
 | Se ordenan los array de posiciones de las claves de la tabla.
 +--------------------------------------------------------------------*/
OrdError();


/*--------------------------------------------------------------------
 | Si no se deja abierto, se cierra el fichero imagen de la base.
 +--------------------------------------------------------------------*/
if (! fichAbierto)
    if (close(fdFBD) == -1) {
	ERR_SistOper(ERR_NO_FATAL, FUNC_BDT, "AER: IniInformacion", "close",
		     "fichero \"%s\"", fichImagen);
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
 | Se inicializan los punteros de acceso a la tabla ERROR:
 *	numError: punt. al numero de elementos.
 *	clvSecuenciaError: array de acceso por la clave Secuencia
 *	posError: informacion de los elementos.
 *	clvAplicacionError: array de acceso por la clave Aplicacion
 *	posError: informacion de los elementos.
 +--------------------------------------------------------------------*/
numError = (int *) pActual;
pActual += sizeof(int);
clvSecuenciaError = (int *) pActual;
pActual += (maxError * sizeof(int *));
clvAplicacionError = (int *) pActual;
pActual += (maxError * sizeof(int *));
posError = POS_MEM(pActual);
pActual += (maxError * TAM_ERROR);

}


/*====================== PROCEDIMIENTO PRIVADO ======================
 * CodError
 *	Codifica un TipoERROR sobre un elemento Error.
 *    Parametros:
 *      elem: buffer del elemento sobre el que codificar.
 *	error: struct a realizar la codificacion.
 *====================================================================*/
static void CodError(char *elem, TipoERROR *pError)
{
TPC_CodInt(&pError->secuencia, 1, elem + 1);
TPC_CodChar(&pError->gravedad, 1, elem + 5);
TPC_CodChar(pError->aplicacion, 50, elem + 6);
TPC_CodChar(pError->proceso, 50, elem + 56);
TPC_CodChar(pError->funcion, 50, elem + 106);
TPC_CodInt(&pError->fecha, 1, elem + 156);
TPC_CodChar(pError->texto, 50, elem + 160);
}

/*====================== PROCEDIMIENTO PRIVADO ======================
 * DecError
 *	Decodifica un elemento Error sobre un TipoERROR
 *    Parametros:
 *      elem: buffer del elemento a decodificar.
 *	error: struct sobre la que realizar la decodificacion.
 *====================================================================*/
static void DecError(char *elem, TipoERROR *pError)
{
TPC_DecInt(&pError->secuencia, 1, elem + 1);
TPC_DecChar(&pError->gravedad, 1, elem + 5);
TPC_DecChar(pError->aplicacion, 50, elem + 6);
TPC_DecChar(pError->proceso, 50, elem + 56);
TPC_DecChar(pError->funcion, 50, elem + 106);
TPC_DecInt(&pError->fecha, 1, elem + 156);
TPC_DecChar(pError->texto, 50, elem + 160);
}

/*========================= FUNCION PRIVADA =========================
 * OrdError
 *	Ordena los punt. de acceso a la tabla ERROR, por:
 *		- SECUENCIA
 *		- APLICACION
 *====================================================================*/
static void OrdError()
{
/*--------------------------------------------------------------------
 | Ordena el array de acceso a la tabla ERROR por las claves:
 |	- SECUENCIA
 |	- APLICACION
 +--------------------------------------------------------------------*/
qsort(clvSecuenciaError, *numError,
      sizeof(int), (PF_COMPAR)CmpDesSecuenciaError);
qsort(clvAplicacionError, *numError,
      sizeof(int), (PF_COMPAR)CmpDesAplicacionError);
}

/*========================= FUNCION PRIVADA =========================
 * ClaveError
 *	Devuelve el array de acceso correspondiente el codigo de
 *	una determinada clave de la tabla ERROR.
 *
 *    Parametros:
 *	codClave: codigo de la clave.
 *    Devuelve:
 *	el array de acceso de la clave: si el codigo es correcto.
 *	NULL: si el codigo es erroneo.
 *====================================================================*/
static int *ClaveError(const int codClave)
{
switch(codClave) {
    case CLV_SECUENCIA_ERROR:
	return clvSecuenciaError;
    case CLV_APLICACION_ERROR:
	return clvAplicacionError;
    }
ERR_Aplicacion(ERR_NO_FATAL, FUNC_BDT, "AER: ClaveError",
	"codigo de clave (%d) de la tabla ERROR erroneo", codClave);
return NULL;
}

/*========================= FUNCION PRIVADA =========================
 * CmpDesSecuenciaError
 *	Compara la clave SECUENCIA de dos elementos de la tabla
 *	ERROR segun sus posiciones en la memoria.
 *    Parametros:
 *	pPosic1: punt. a la posicion del elemento 1 a comparar la clave.
 *	pPosic2: punt. a la posicion del elemento 2 a comparar la clave.
 *    Devuelve:
 *      num > 0: si la clave del pPosic1 es mayor que el del pPosic2.
 *      0:	 si la clave del pPosic1 es igual que el del pPosic2.
 *      num < 0: si la clave del pPosic1 es menor que el del pPosic2.
 *====================================================================*/
static int CmpDesSecuenciaError(const int *pPosic1, const int *pPosic2)
{
return CmpSecuenciaError((TipoERROR *) DIR_MEM(*pPosic1),
			(TipoERROR *) DIR_MEM(*pPosic2));
}

/*========================= FUNCION PRIVADA =========================
 * CmpSecuenciaError
 *	Compara la clave SECUENCIA de dos elementos de la tabla
 *	ERROR.
 *    Parametros:
 *	error:  punt. al elemento 1 a comparar la clave.
 *	error2: punt. al elemento 2 a comparar la clave.
 *    Devuelve:
 *      num > 0: si la clave de error es mayor que error2.
 *      0:       si la clave de error es igual que error2.
 *      num < 0: si la clave de error es menor que error2.
 *====================================================================*/
static int CmpSecuenciaError(const TipoERROR *pError,
				const TipoERROR *pError2)
{
	int result;

/*--------------------------------------------------------------------
 | Si el campo SECUENCIA del elemento 1 y 2 es distinto, se devuelve
 | el resultado de la comparacion.
 +--------------------------------------------------------------------*/
if ((result = CMP_VAL(pError->secuencia, pError2->secuencia)) != 0) 
    return result;

return 0;
}

/*========================= FUNCION PRIVADA =========================
 * CmpDesAplicacionError
 *	Compara la clave APLICACION de dos elementos de la tabla
 *	ERROR segun sus posiciones en la memoria.
 *    Parametros:
 *	pPosic1: punt. a la posicion del elemento 1 a comparar la clave.
 *	pPosic2: punt. a la posicion del elemento 2 a comparar la clave.
 *    Devuelve:
 *      num > 0: si la clave del pPosic1 es mayor que el del pPosic2.
 *      0:	 si la clave del pPosic1 es igual que el del pPosic2.
 *      num < 0: si la clave del pPosic1 es menor que el del pPosic2.
 *====================================================================*/
static int CmpDesAplicacionError(const int *pPosic1, const int *pPosic2)
{
return CmpAplicacionError((TipoERROR *) DIR_MEM(*pPosic1),
			(TipoERROR *) DIR_MEM(*pPosic2));
}

/*========================= FUNCION PRIVADA =========================
 * CmpAplicacionError
 *	Compara la clave APLICACION de dos elementos de la tabla
 *	ERROR.
 *    Parametros:
 *	error:  punt. al elemento 1 a comparar la clave.
 *	error2: punt. al elemento 2 a comparar la clave.
 *    Devuelve:
 *      num > 0: si la clave de error es mayor que error2.
 *      0:       si la clave de error es igual que error2.
 *      num < 0: si la clave de error es menor que error2.
 *====================================================================*/
static int CmpAplicacionError(const TipoERROR *pError,
				const TipoERROR *pError2)
{
	int result;

/*--------------------------------------------------------------------
 | Si el campo APLICACION del elemento 1 y 2 es distinto, se devuelve
 | el resultado de la comparacion.
 +--------------------------------------------------------------------*/
if ((result = strcmp(pError->aplicacion, pError2->aplicacion)) != 0) 
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
	ERR_SistOper(ERR_NO_FATAL, FUNC_BDT, "AER: ActElemento", "open",
		 "fichero \"%s\" (para lectura/escritura))", fichImagen);
	return -1;
	}
    }
 
/*--------------------------------------------------------------------
 | Se prepara la escritura sobre la posicion del elemento.
 +--------------------------------------------------------------------*/
if (lseek(fdFBD, (off_t)posFic, SEEK_SET) == -1) {
    ERR_SistOper(ERR_NO_FATAL, FUNC_BDT, "AER: ActElemento", "lseek",
    		 "fichero \"%s\" (posicion %ld)", fichImagen, posFic);
    return -1;
    }

/*--------------------------------------------------------------------
 | Se escribe el elemento sobre el fichero.
 +--------------------------------------------------------------------*/
if (write(fdFBD, (char *)eleFic, lonFic) != lonFic) {
    ERR_SistOper(ERR_NO_FATAL, FUNC_BDT, "AER: ActElemento", "write",
    		 "fichero \"%s\" (%d bytes)", fichImagen, lonFic);
    return -1;
    }

/*--------------------------------------------------------------------
 | Si el fichero no se deja abierto, se cierra.
 +--------------------------------------------------------------------*/
if (! fichAbierto)
    if (close(fdFBD) == -1) {
	ERR_SistOper(ERR_NO_FATAL, FUNC_BDT, "AER: ActElemento", "close",
		     "fichero \"%s\"", fichImagen);
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
