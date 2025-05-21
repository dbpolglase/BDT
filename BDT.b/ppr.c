#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <malloc.h>
#include <memory.h>
#include <unistd.h>
#include <limits.h>
#include <errno.h>
#include <ctype.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdarg.h>
#include <err.h>
#include <mdi.h>
#include <ppr.h>

/*--------------------------------------------------------------------
 * DIR_ERROR:   directiva no reconocida
 * DIR_INCLUDE: directiva "#include"
 * DIR_DEFINE:  directiva "#define"
 * DIR_UNDEF:   directiva "#undef"
 * DIR_IFDEF:   directiva "#ifdef"
 * DIR_IFNDEF:  directiva "#ifndef"
 * DIR_ELSE:    directiva "#else"
 * DIR_ENDIF:   directiva "#endif"
 * LINEA_TEXTO: linea de texto normal.
 *--------------------------------------------------------------------*/
#define DIR_ERROR		0
#define DIR_INCLUDE		1
#define DIR_DEFINE		2
#define DIR_UNDEF		3
#define DIR_IFDEF		4
#define DIR_IFNDEF		5
#define DIR_ELSE		6
#define DIR_ENDIF		7
#define LINEA_TEXTO		8

/*--------------------------------------------------------------------
 * TXT_INCLUDE: texto del token "include"
 * TXT_DEFINE:  texto del token "define"
 * TXT_UNDEF:   texto del token "undef"
 * TXT_IFDEF:   texto del token "ifdef"
 * TXT_IFNDEF:  texto del token "ifndef"
 * TXT_ELSE:    texto del token "else"
 * TXT_ENDIF:   texto del token "endif"
 *--------------------------------------------------------------------*/
#define TXT_INCLUDE		"include"
#define TXT_DEFINE		"define"
#define TXT_UNDEF		"undef"
#define TXT_IFDEF		"ifdef"
#define TXT_IFNDEF		"ifndef"
#define TXT_ELSE		"else"
#define TXT_ENDIF		"endif"

/*--------------------------------------------------------------------
 * LON_INCLUDE: longitud del texto del token "include"
 * LON_DEFINE:  longitud del texto del token "define"
 * LON_UNDEF:   longitud del texto del token "undef"
 * LON_IFDEF:   longitud del texto del token "ifdef"
 * LON_IFNDEF:  longitud del texto del token "ifndef"
 * LON_ELSE:    longitud del texto del token "else"
 * LON_ENDIF:   longitud del texto del token "endif"
 *--------------------------------------------------------------------*/
#define LON_INCLUDE	(sizeof(TXT_INCLUDE)-1)
#define LON_DEFINE	(sizeof(TXT_DEFINE)-1)
#define LON_UNDEF	(sizeof(TXT_UNDEF)-1)
#define LON_IFDEF	(sizeof(TXT_IFDEF)-1)
#define LON_IFNDEF	(sizeof(TXT_IFNDEF)-1)
#define LON_ELSE 	(sizeof(TXT_ELSE)-1)
#define LON_ENDIF 	(sizeof(TXT_ENDIF)-1)

/*--------------------------------------------------------------------
 | INI_SIMB: macro de primer caracter valido de un simbolo
 | CAR_SIMB: macro de caracter valido dentro de un simbolo 
 +--------------------------------------------------------------------*/
#define INI_SIMB(ch)	(isalpha(ch) || (ch == '_'))
#define CAR_SIMB(ch)	(isalnum(ch) || (ch == '_'))

/*--------------------------------------------------------------------
 | estados del analizador de expresiones:
 |  - ST_SEPARADOR: estado SEPARADOR de palabras
 |  - ST_PALABRA: estado EN PALABRA
 |  - ST_FIN_LIN: estado FIN DE LINEA
 +--------------------------------------------------------------------*/
#define ST_SEPARADOR	0
#define ST_PALABRA	1
#define ST_FIN_LIN	2

/*--------------------------------------------------------------------
 * acciones a realizar en preprocesado:
 * ESCRIBIR: accion ESCRIBIR todo el texto hasta fin de condicion
 * ELIMINAR: accion NO ESCRIBIR (ELIMINAR) el texto hasta fin de condicion.
 * ANULAR:   accion de ANULAR todo (independientemente de otras condiciones
 *	     que pueda haber anidadas) hasta fin de condicion.
 *--------------------------------------------------------------------*/
#define ESCRIBIR		'E'
#define ELIMINAR		'X'
#define ANULAR			'A'

/*--------------------------------------------------------------------
 | Estados del automata de analisis de IFDEF/IFNDEF [ELSE] ENDIF
 |  - ST_INIIF:   estado de inicio de IFDEF/IFNDEF.
 |  - ST_ELSE:    estado del ELSE de un IFDEF/IFNDEF.
 |  - ST_INICIAL: estado de INICIO (no hay ningun IFDEF/IFNDEF)
 +--------------------------------------------------------------------*/
#define	ST_INIIF	'I'
#define	ST_ELSE		'E'
#define	ST_INICIAL	'0'

/*--------------------------------------------------------------------
 | LON_LINEA: tamano maximo de una linea.
 | LON_EXPR: tamano maximo de una expresion.
 +--------------------------------------------------------------------*/
#define LON_LINEA	512
#define LON_EXPR	LON_LINEA

/*--------------------------------------------------------------------
 | TAM_DEFINICION: tamano en bytes de la struct DEFINICION
 | TAM_PILA: tamano de la pila del automata.
 +--------------------------------------------------------------------*/
#define TAM_DEFINICION 	(sizeof(DEFINICION))
#define TAM_PILA	256

/*--------------------------------------------------------------------
 | struct DEFINICION:
 |	- patron: texto patron de una definicion.
 |	- lonPatr: longitud del patron de una definicion.
 |	- sustit: texto sustituto de una definicion.
 |	- lonSust: longitud del sustituto de una definicion.
 +--------------------------------------------------------------------*/
typedef struct {
	char *patron;
	int   lonPatr;
	char *sustit;
	int   lonSust;
	} DEFINICION;

/*--------------------------------------------------------------------
 | struct PILA:
 |	- posicion: posicion de la cabeza de la pila.
 |	- accion: accion asociada al elemento de pila.
 |	- nLinea: linea referente al fichero.
 +--------------------------------------------------------------------*/
typedef struct {
	int  posicion;
	char accion[TAM_PILA];
	int  nLinea[TAM_PILA];
	} PILA;

/*====================================================================
 *		DECLARACION DE FUNCIONES PRIVADAS
 *====================================================================*/
static char *PreprocFichero(char *, int);
static void  IncluirFichero(char *, char *, int);
static int   ExpresionDef(char *, char *, int);
static int   TipoDeLinea(char *, char **);

static void  InsDefinicion(char *, char *, char *, int);
static void  EliDefinicion(char *, char *, int);
static int   BusDefinicion(char *);
static void  SusDefinicion(char *);
static int   Definido(char *);

/*====================================================================
 * DECLARACION DE FUNCIONES PRIVADAS: manejo de la pila
 *====================================================================*/
static void IniciarPila(PILA *);
static void InsertarEnPila(PILA *, char, int);
static void EliminarDePila(PILA *);
static void InvertirCabeceraPila(PILA *, int);
static char AccionCabeceraPila(PILA *);
static int  PilaVacia(PILA *);
static void VaciarPila(PILA *, char *);

/*--------------------------------------------------------------------
 | dirInclude: lista de directorios donde buscar los includes
 | numDirIncl: numero de directorios.
 +--------------------------------------------------------------------*/
static char **dirInclude = NULL;
static int    numDirIncl = 0;

/*--------------------------------------------------------------------
 | numDef: numero de definiciones.
 | definic: lista de definiciones.
 +--------------------------------------------------------------------*/
static int	   numDef  = 0;
static DEFINICION *definic = NULL;

/*===================== PROCEDIMIENTO PUBLICO =======================
 * PPR_Directorio
 *	Incluye el path de un directorio en la lista para buscar los
 *	includes durante el preprocesado.
 *
 *    Parametros:
 *	path: path a incluir.
 *====================================================================*/
void PPR_Directorio(char *path)
{
MDI_ElemArray((void *) &dirInclude, &numDirIncl, sizeof(char *));
dirInclude[numDirIncl - 1] = path;
}

/*========================= FUNCION PUBLICA =========================
 * PPR_Definicion
 *	Incluye una definicion y su sustituto en la lista.
 *
 *    Parametros:
 *	txtPat: texto patron.
 *	txtSus: texto sustituto.
 *    Devuelve:
 *	 0: si todo es correcto.
 *	-1: ni el texto patron es NULL.
 *	-2: si el texto patron tiene un formato erroneo.
 *	-3: si el  texto patron ya existe.
 *====================================================================*/
int PPR_Definicion(char *txtPat, char *txtSus)
{
	int indDef;
	char *pPat;

/*--------------------------------------------------------------------
 | Si falta el texto patron. (error = -1)
 +--------------------------------------------------------------------*/
if (txtPat == NULL)
    return -1;

/*--------------------------------------------------------------------
 | Si falta el texto sustituto, se pone como texto vacio.
 +--------------------------------------------------------------------*/
if (txtSus == NULL)
    txtSus = "";

/*--------------------------------------------------------------------
 | Se analiza que el simbolo tenga un formato correcto. (error = -2)
 +--------------------------------------------------------------------*/
pPat = txtPat;
if (!INI_SIMB(*pPat))
    return -2;
while (*pPat) {
    if (!CAR_SIMB(*pPat))
	return -2;
    pPat++;
    }

/*--------------------------------------------------------------------
 | Se comprueba si ya existe la definicion (repetida). (error = -3)
 +--------------------------------------------------------------------*/
indDef = BusDefinicion(txtPat);
if (indDef == -1) {
    indDef = numDef;
    MDI_ElemArray((void *) &definic, &numDef, TAM_DEFINICION);
    }
else
    return -3;

/*--------------------------------------------------------------------
 | Si todo lo anterior es correcto se guarda el patron y el sustituto
 +--------------------------------------------------------------------*/
definic[indDef].patron  = MDI_String(txtPat);
definic[indDef].lonPatr = strlen(definic[indDef].patron);
definic[indDef].sustit  = MDI_String(txtSus);
definic[indDef].lonSust = strlen(definic[indDef].sustit);
return 0;
}

/*========================= FUNCION PUBLICA =========================
 * PPR_PreprocFichero
 *	Preprocesa un fichero determinado en base a las definiciones
 *	informadas en ese momento.
 *
 *    Parametros:
 *	fichOri: nombre del fichero a preprocesar.
 *    Devuelve:
 *	fichPpr: nombre del fichero preprocesado.
 *====================================================================*/
char *PPR_PreprocFichero(char *fichOri)
{
return PreprocFichero(fichOri, 1);
}

static char *PreprocFichero(char *fichOri, int fichInicial)
{
	int fichPpr;
	char * nfichPpr;
	FILE *fpOri;
	FILE *fpPpr;
	char  linea[LON_LINEA];
	char *linEscr;
	char *pComent;
	char *pPatron;
	char *pSustit;
	char *pFich;
	char *pResto;
	PILA  pila;
	char  estado;
	int   numLinea;

/*--------------------------------------------------------------------
 | Si el preprocesado es del fichero inicial, se crea un temporal.
 +--------------------------------------------------------------------*/
if (fichInicial) {
/*    fichPpr = tempnam("/tmp", "PPR");*/
    nfichPpr = malloc(PATH_MAX);
    strcpy(nfichPpr,"/tmp/bdtpprXXXXXX");
    fichPpr = mkstemp(nfichPpr);
    if (fichPpr == -1)
    {
	ERR_PorTerminal("tempnam in tmp with prefix \"bdrppr\" fail"); 
        return NULL;
    }
    fpPpr = fdopen(fichPpr, "w");
    if (fpPpr == NULL)
	ERR_PorTerminal("fopen file \"%s\" (write) fail", fichPpr);
    }

/*--------------------------------------------------------------------
 | Se abren el fichero a preprocesar (entrada)
 +--------------------------------------------------------------------*/
fpOri = fopen(fichOri, "r");
if (fpOri == NULL)
    ERR_PorTerminal("fopen file \"%s\" (read) fail", fichOri);
numLinea = 0;

/*--------------------------------------------------------------------
 | Se inicializa la pila del automata reconocedor (vacia), asi como el
 | estado del automata que analiza IFDEF/IFNDEF [ELSE] ENDIF
 +--------------------------------------------------------------------*/
IniciarPila(&pila);
estado = ST_INICIAL;

/*--------------------------------------------------------------------
 | Se lee el fichero de entrada linea a linea.
 +--------------------------------------------------------------------*/
while (fgets(linea, LON_LINEA, fpOri)) {
    numLinea++;
    linEscr = "\n";
    switch(TipoDeLinea(linea, &pResto)) {
	/*------------------------------------------------------------
	 | #include: se recogen las definiciones de un fichero
	 +------------------------------------------------------------*/
	case DIR_INCLUDE:
	    if (AccionCabeceraPila(&pila) == ESCRIBIR) {
		pFich = strtok(pResto, " \t\n");
		IncluirFichero(pFich, fichOri, numLinea);
		}
	    break;
	/*------------------------------------------------------------
	 | #define: se inserta en la tabla de definiciones.
	 +------------------------------------------------------------*/
	case DIR_DEFINE:
	    if (AccionCabeceraPila(&pila) == ESCRIBIR) {
		pPatron = strtok(pResto, " \t\n");
		pSustit = strtok(NULL, " \t\n");
		InsDefinicion(pPatron, pSustit, fichOri, numLinea);
		}
	    break;
	/*------------------------------------------------------------
	 | #undef: se elimina en la tabla de definiciones.
	 +------------------------------------------------------------*/
	case DIR_UNDEF:
	    if (AccionCabeceraPila(&pila) == ESCRIBIR) {
		pPatron = strtok(pResto, " \t\n");
		EliDefinicion(pPatron, fichOri, numLinea);
		}
	    break;
	/*------------------------------------------------------------
	 | #ifdef: si la expresion asociada es cierta, se pone como
	 | estado (cima de pila) ESCRIBIR, si no, se pone ELIMINAR
	 +------------------------------------------------------------*/
	case DIR_IFDEF:
	    if (ExpresionDef(pResto, fichOri, numLinea))
		InsertarEnPila(&pila, ESCRIBIR, numLinea);
	    else
		InsertarEnPila(&pila, ELIMINAR, numLinea);
	    estado = ST_INIIF;
	    break;
	/*------------------------------------------------------------
	 | #ifndef: si la expresion asociada no es cierta, se pone
	 | como estado (cima de pila) ESCRIBIR, si no, se pone
	 | ELIMINAR
	 +------------------------------------------------------------*/
	case DIR_IFNDEF:
	    if (! ExpresionDef(pResto, fichOri, numLinea))
		InsertarEnPila(&pila, ESCRIBIR, numLinea);
	    else
		InsertarEnPila(&pila, ELIMINAR, numLinea);
	    estado = ST_INIIF;
	    break;
	/*------------------------------------------------------------
	 | #else: se invierte el estado de la pila, pero solo si va
	 | despues de un IFDEF/IFNDEF (ST_INICIAL).
	 +------------------------------------------------------------*/
	case DIR_ELSE:
	    if (estado != ST_INIIF)
		ERR_PorTerminal("%d %s: directive \"%s\" out of context",
			     numLinea,fichOri,  TXT_ELSE);
	    else
		InvertirCabeceraPila(&pila, numLinea);
	    estado = ST_ELSE;
	    break;
	/*------------------------------------------------------------
	 | #endif: se elimina la cima de la pila al estado anterior
	 +------------------------------------------------------------*/
	case DIR_ENDIF:
	    if (estado == ST_INICIAL)
		ERR_PorTerminal("%d %s: directive \"%s\" out of context",
			     numLinea,fichOri,  TXT_ENDIF);
	    else
		EliminarDePila(&pila);
	    if (PilaVacia(&pila))
		estado = ST_INICIAL;
	    else
		estado = ST_INIIF;
	    break;
	/*------------------------------------------------------------
	 | cualquier otra cosa que comience por '#'  en el fichero
	 | inicial a preprocesar, es un error
	 +------------------------------------------------------------*/
	case DIR_ERROR:
	    if (fichInicial)
		ERR_PorTerminal("%d %s: wrong directive", numLinea,fichOri);
	    break;
	/*------------------------------------------------------------
	 | Si es texto y el estado es ESCRIBIR, se trata dicho texto:
	 |  - Se localiza el posible comentario y si existe, se trunca
	 |    la linea desde esa posicion.
	 |  - Se sustituyen las definiciones que pueda tener la linea.
	 +-----------------------------------------------------------*/
	case LINEA_TEXTO:
	    if ((AccionCabeceraPila(&pila) == ESCRIBIR) && fichInicial) {
		linEscr = linea;
		pComent = strstr(linea, "--");
		if (pComent == NULL)
		    pComent = strstr(linea, "//");
		if (pComent != NULL)
		    strcpy(pComent, "\n");
		SusDefinicion(linea);
		}
	    break;
	}
    if (fichInicial)
	fputs(linEscr, fpPpr);
    }

/*--------------------------------------------------------------------
 | Se mira que la pila este vacia, si no, hay error de sintaxis.
 +--------------------------------------------------------------------*/
VaciarPila(&pila, fichOri);

/*--------------------------------------------------------------------
 | Se cierran los ficheros y si ha habido errores, se borra el fichero
 | preprocesado de salida y se devuelve NULL como nombre de fichero.
 +--------------------------------------------------------------------*/
fclose(fpOri);
if (fichInicial) {
    fclose(fpPpr);
    if (ERR_HayErroresTerminal()) {
        unlink(nfichPpr);
        return NULL;
        }
    return nfichPpr;
  }

return NULL;
}

/*===================================================================*/
static void IncluirFichero(char *fichero, char *fichIncluye, int linea)
{
	static char path[PATH_MAX];
	char *pFich;
	int   lonFich;
	int   encontrado;
	char  sepIni;
	char  sepFin;
	int   i;

pFich	   = fichero + 1;
lonFich	   = strlen(pFich) - 1;
sepIni	   = *fichero;
encontrado = 0;

if (sepIni == '<')
    sepFin = '>';
else if (sepIni == '"')
    sepFin = '"';
else
    sepFin = 'E';

if ((sepFin == 'E') || (pFich[lonFich] != sepFin)) {
    ERR_PorTerminal("%d %s: separators of file %s",
    		 linea,fichIncluye, fichero);
    return;
    }
    
switch(sepIni) {
    case '<':
	for (i = 0; i < numDirIncl; i++) {
	    sprintf(path, "%s/%.*s", dirInclude[i], lonFich, pFich);
	    if (access(path, F_OK) == 0) {
		encontrado = 1;
		break;
		}
	    }
	break;
    case '"':
	sprintf(path, "./%.*s", lonFich, pFich);
	if (access(path, F_OK) == 0)
	    encontrado = 1;
	break;
    }

if (encontrado)
    PreprocFichero(path, 0);
else
    ERR_PorTerminal("%d %s: file %s not found", linea,fichIncluye, fichero);
}

/*===================================================================*/
static int TipoDeLinea(char *linea, char **pResto)
{
	int directiva;

while(isspace(*linea))
    linea++;
if (*linea != '#')
    return LINEA_TEXTO;

linea++;
while(isspace(*linea))
    linea++;
*pResto = linea;

if (!strncmp(linea, TXT_INCLUDE, LON_INCLUDE)) {
    *pResto += LON_INCLUDE;
    directiva = DIR_INCLUDE;
    }
else if (!strncmp(linea, TXT_DEFINE, LON_DEFINE)) {
    *pResto += LON_DEFINE;
    directiva = DIR_DEFINE;
    }
else if (!strncmp(linea, TXT_UNDEF,  LON_UNDEF)) {
    *pResto += LON_UNDEF;
    directiva = DIR_UNDEF;
    }
else if (!strncmp(linea, TXT_IFDEF,  LON_IFDEF)) {
    *pResto += LON_IFDEF;
    directiva = DIR_IFDEF;
    }
else if (!strncmp(linea, TXT_IFNDEF, LON_IFNDEF)) {
    *pResto += LON_IFNDEF;
    directiva = DIR_IFNDEF;
    }
else if (!strncmp(linea, TXT_ELSE,   LON_ELSE)) {
    *pResto += LON_ELSE;
    directiva = DIR_ELSE;
    }
else if (!strncmp(linea, TXT_ENDIF,  LON_ENDIF)) {
    *pResto += LON_ENDIF;
    directiva = DIR_ENDIF;
    }
else
    return DIR_ERROR;

if (isgraph(**pResto))
    return DIR_ERROR;
return directiva;
}

/*========================= FUNCION PRIVADA =========================
 * ExpresionDef
 *	Analiza el valor (booleano: 0 o 1) de una expresion.
 *
 *    Parametros:
 *	pExpr: texto de la expresion.
 *	fichero: fichero que contiene la expresion.
 *	nLinea: numero de linea de la expresion.
 *    Devuelve:
 *	1: si la expresion es cierta.
 *	0: si la expresion no es cierta.
 *====================================================================*/
static int ExpresionDef(char *pExpr, char *fichero, int nLinea)
{
	char *pDef;
	char *pOpe;
	int   result;
	char  bufExpr[LON_EXPR];

/*--------------------------------------------------------------------
 | Se hace una copia de la linea a tratar (strtok pone NUL).
 +--------------------------------------------------------------------*/
pDef = bufExpr;
while ((*pExpr != '\n') && (*pExpr != '\0'))
    *pDef++ = *pExpr++;
*pDef = '\0';

/*--------------------------------------------------------------------
 | Se recoge la primera definicion, tal que si no existe: FALSA
 +--------------------------------------------------------------------*/
pDef = strtok(bufExpr, " \t\n");
if (pDef == NULL)
    return 0;

/*--------------------------------------------------------------------
 | Si existe, se toma como resultado inicial.
 +--------------------------------------------------------------------*/
result = Definido(pDef);

/*--------------------------------------------------------------------
 | - Se recorre el resto de la expresion intentando buscar parejas de
 |   "operador definicion".
 | - Un operador sin definicion u operador desconocido es un error.
 | - Se opera el resultado anterior con la nueva definicion segun sea
 |   el operador.
 +--------------------------------------------------------------------*/
while((pOpe = strtok(NULL, " \t\n")) != NULL) {
    pDef = strtok(NULL, " \t\n");
    if (pDef == NULL) {
	ERR_PorTerminal("%d %s: incomplete conditional expression ", nLinea,fichero);
	return 0;
	}
    if (strcmp(pOpe, "|") == 0)
	result |= Definido(pDef);
    else if (strcmp(pOpe, "&") == 0)
	result &= Definido(pDef);
    else {
	ERR_PorTerminal("%d %s: operator \"%s\" unkown", nLinea,fichero, pOpe);
	return 0;
	}
    }

/*--------------------------------------------------------------------
 | Si la expresion es correcta se devuelve el resultado.
 +--------------------------------------------------------------------*/
return result;
}

/******************** Tratamiento de DEFINICIONES *********************/
/*====================== PROCEDIMIENTO PRIVADO ======================
 * InsDefinicion
 *	Inserta una definicion (patron y sustituto), con comunicacion
 *	de errores con el usuario.
 *
 *    Parametros:
 *	txtPat: texto patron de la definicion
 *	txtSus: texto sustituto de la definicion
 *	fichero: fichero que contiene la definicion.
 *	nLinea: numero de linea de la directiva.
 *====================================================================*/
static void InsDefinicion(char *txtPat, char *txtSus, char *fichero, int nLinea)
{
/*--------------------------------------------------------------------
 | Inserta una definicion (y el sustituto) y si hay error, se reporta
 | al usuario.
 +--------------------------------------------------------------------*/
switch(PPR_Definicion(txtPat, txtSus)) {
    case -1:
	ERR_PorTerminal( "%d %s: lack of definition ", nLinea,fichero);
	break;
/*    case -2:
	ERR_PorTerminal( "%d %s: symbol of  definition \"%s\"",
		     nLinea,fichero, txtPat);
	break;
	*/
    case -3:
	ERR_PorTerminal( "%d %s: redefinition of \"%s\"", nLinea,fichero, txtPat);
	break;
    }
}

/*====================== PROCEDIMIENTO PRIVADO ======================
 * EliDefinicion
 *	Elimina una definicion de la lista. Si no existe se comunica
 *	al usuario.
 *
 *    Parametros:
 *	txtPat:  texto patron de la definicion a eliminar.
 *	fichero: fichero que contiene la definicion.
 *	nLinea:  numero de linea de la directiva.
 *====================================================================*/
static void EliDefinicion(char *txtPat, char *fichero, int nLinea)
{
	int indDef;

/*--------------------------------------------------------------------
 | Se busca el patron de la definicion 
 +--------------------------------------------------------------------*/
indDef = BusDefinicion(txtPat);
if (indDef == -1)
    ERR_PorTerminal("%d %s: definition \"%s\" not defined", nLinea,fichero, txtPat);
else {
    definic[indDef].patron  = NULL;
    definic[indDef].lonPatr = 0;
    }
}

/*========================= FUNCION PRIVADA =========================
 * BusDefinicion
 *	Busca una definicion en la lista.
 *
 *    Parametros:
 *	txtPat: texto patron de la definicion a buscar.
 *    Devuelve:
 *	el indice a la lista, si existe.
 *	-1: si no existe.
 *====================================================================*/
static int BusDefinicion(char *txtPat)
{
	int i;

/*--------------------------------------------------------------------
 | Se recorre la lista buscando el patron, saltando los anulados (NULL)
 +--------------------------------------------------------------------*/
for (i = 0; i < numDef; i++) {
    if (definic[i].patron == NULL)
	continue;
    if (strcmp(definic[i].patron, txtPat) == 0)
	return i;
    }
return -1;
}

/*========================= FUNCION PRIVADA =========================
 * Definido
 *	Informa de si un patron esta definido.
 *
 *    Parametros:
 *	txtPat: texto patron de la definicion.
 *    Devuelve:
 *	1: si esta definido.
 *	0: si no esta definido.
 *====================================================================*/
static int Definido(char *txtPat)
{
if (BusDefinicion(txtPat) == -1)
    return 0;
return 1;
}

/*====================== PROCEDIMIENTO PRIVADO ======================
 * SusDefinicion
 *	Dada una linea de texto, se realizan las sustituciones de los
 *	posibles patrones definidos por los correspondientes sustitutos.
 *	Es decir, realiza el preprocesado de la linea.
 *
 *    Parametros:
 *	linea: linea a procesar.
 *====================================================================*/
static void SusDefinicion(char *linea)
{
	char *palabra;
	int   lonPal;
	char *pLin;
	int   estado;
	int   i;

/*--------------------------------------------------------------------
 | Se recorre cada una de las definiciones y se aplica a la linea.
 +--------------------------------------------------------------------*/
for (i = 0; i < numDef; i++) {
    /*----------------------------------------------------------------
     * Si el patron esta anulado, se salta.
     *----------------------------------------------------------------*/
     if (definic[i].patron == NULL)
	continue;

    pLin   = linea;
    estado = ST_SEPARADOR;
    /*----------------------------------------------------------------
     * Se recorre la linea, sacando las palabras del texto.
     *----------------------------------------------------------------*/
    while (*pLin != '\0') {
	switch(estado) {
	    /*----------------------------------------------------------------
	     * Se saltan los caracteres hasta que comience un simbolo (palabra)
	     *----------------------------------------------------------------*/
	    case ST_SEPARADOR:
		if (INI_SIMB(*pLin)) {
		    palabra = pLin;
		    lonPal  = 1;
		    estado  = ST_PALABRA;
		    }
		break;
	    /*----------------------------------------------------------------
	     * Se delimita el simbolo y si coincide con el patron, se cambia
	     * este por el sustituto correspondiente.
	     *----------------------------------------------------------------*/
	    case ST_PALABRA:
		if (!CAR_SIMB(*pLin)) {
		    if ((definic[i].lonPatr == lonPal) &&
			(strncmp(palabra, definic[i].patron, lonPal) == 0)) {
			memmove(palabra + definic[i].lonSust, pLin,
				strlen(pLin) + 1);
			memcpy(palabra, definic[i].sustit, definic[i].lonSust);
			pLin = palabra + definic[i].lonSust;
			}
		    estado = ST_SEPARADOR;
		    }
		else
		    lonPal++;
		break;
	    }
	pLin++;
	}
    }
}

/**********************************************************************/
/*====================== PROCEDIMIENTO PRIVADO ======================
 * IniciarPila
 *	Inicializa el estado de la pila vacia.
 *
 *    Parametros:
 *	pPila: puntero a la pila.
 *====================================================================*/
static void IniciarPila(PILA *pPila)
{
/*--------------------------------------------------------------------
 | La accion por defecto es escribir.
 +--------------------------------------------------------------------*/
pPila->posicion  = 0;
pPila->accion[0] = ESCRIBIR;
pPila->nLinea[0] = 0;
}

/*====================== PROCEDIMIENTO PRIVADO ======================
 * InsertarEnPila
 *	Inserta una nueva accion en la pila.
 *
 *    Parametros:
 *	pPila:  puntero a la pila.
 *	accion: accion asociada.
 * 	nLinea:  numero de linea del fichero preprocesado.
 *====================================================================*/
static void InsertarEnPila(PILA *pPila, char accion, int nLinea)
{
	int posPila;

/*--------------------------------------------------------------------
 | Si la accion en cabeza es ESCRIBIR, se introduce la accion definida
 | Si no (ELIMINAR o ANULAR), se anula todo lo que haya encima, inde-
 | pendientemente de lo que sea la accion definida.
 +--------------------------------------------------------------------*/
pPila->posicion++;
posPila = pPila->posicion;
pPila->nLinea[posPila] = nLinea;
if (pPila->accion[posPila - 1] == ESCRIBIR)
    pPila->accion[posPila] = accion;
else
    pPila->accion[posPila] = ANULAR;
}

/*====================== PROCEDIMIENTO PRIVADO ======================
 * EliminarDePila
 *	Elimina una accion de la pila.
 *
 *    Parametros:
 *	pPila: puntero a la pila.
 *====================================================================*/
static void EliminarDePila(PILA *pPila)
{
if (pPila->posicion)
    pPila->posicion--;
}

/*====================== PROCEDIMIENTO PRIVADO ======================
 * InvertirCabeceraPila
 *	Invierte la accion de cabeza de la pila.
 *
 *    Parametros:
 *	pPila: puntero a la pila.
 * 	nLinea:  numero de linea del fichero preprocesado.
 *====================================================================*/
static void InvertirCabeceraPila(PILA *pPila, int nLinea)
{
	int posPila;

/*--------------------------------------------------------------------
 | Si la accion es ESCRIBIR se pone ELIMINAR.
 | Si la accion es ELIMINAR se pone ESCRIBIR.
 | Caso contrario, la accion es ANULAR que se mantiene.
 +--------------------------------------------------------------------*/
posPila = pPila->posicion;
pPila->nLinea[posPila] = nLinea;
if (pPila->accion[posPila] == ESCRIBIR)
    pPila->accion[posPila] = ELIMINAR;
else if (pPila->accion[posPila] == ELIMINAR)
    pPila->accion[posPila] = ESCRIBIR;
else
    pPila->accion[posPila] = ANULAR;
}

/*========================= FUNCION PRIVADA =========================
 * AccionCabeceraPila
 *	Informa de la accion de la cabecera de la pila.
 *
 *    Parametros:
 *	pPila: puntero a la pila.
 *    Devuelve:
 *	la accion que hay en cabeza de la pila.
 *====================================================================*/
static char AccionCabeceraPila(PILA *pPila)
{
return pPila->accion[pPila->posicion];
}

/*========================= FUNCION PRIVADA =========================
 * PilaVacia
 *	Informa de si la pila esta vacia o no.
 *
 *    Parametros:
 *	pPila: puntero a la pila.
 *    Devuelve:
 *	1: si la pila esta vacia.
 *	0: si hay algo en la pila.
 *====================================================================*/
static int PilaVacia(PILA *pPila)
{
if (pPila->accion[pPila->posicion])
    return 0;
return 1;
}

/*====================== PROCEDIMIENTO PRIVADO ======================
 * VaciarPila
 *	Comprueba que la pila este vacia. Caso contrario se informa
 *	al usuario de los bucles no cerrados.
 * 
 *    Parametros:
 *	fichero: fichero preprocesado asociado al la pila
 *====================================================================*/
static void VaciarPila(PILA *pPila, char *fichero)
{
while(pPila->posicion > 0) {
    ERR_PorTerminal("%d %s: %s/%s not close",
		 pPila->nLinea[pPila->posicion],fichero, TXT_IFDEF, TXT_IFNDEF);
    pPila->posicion--;
    }
}
