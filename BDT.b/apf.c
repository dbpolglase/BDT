#include <stdio.h>
#include <string.h>
#include <malloc.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdarg.h>
#include <sys/stat.h>
#include <libgen.h>
#include <err.h>
#include <mdi.h>
#include <ppr.h>
#include <apf.h>

/*--------------------------------------------------------------------
 | MAX_OPCION:  maximo numero de opciones permitidas.
 +--------------------------------------------------------------------*/
#define MAX_OPCION		20

/*--------------------------------------------------------------------
 | struct TipoOPCION:
 |	letra:	 letra correspondiente a la opcion.
 |	nombre:	 nombre de la opcion.
 |	descrip: texto de la descripcion de la opcion (una frase).
 |	conVal:	 indicativo de si la opcion tiene asociado valor.
 |	valor:	 valor asociado a la opcion.
 |	nomVal:	 nombre del valor asociado a la opcion.
 |	existe:	 flag de si la opcion existe en la llamada o no.
 +--------------------------------------------------------------------*/
typedef struct {
	char  letra;
	char *nombre;
	char *descrip;
	int   conVal;
	char *valor;
	char *nomVal;
	int   existe;
	} TipoOPCION;

/*====================================================================
 *		DECLARACION DE FUNCIONES PRIVADAS
 *====================================================================*/
static void Ayuda();
static void EscrUso();
static void EscrDescripcion();
static int  BuscaOpcion(char letraOpc);

/*--------------------------------------------------------------------
 | opcDef: flag para admitir definiciones (opcion -D, SI por defecto)
 | totOpc: numero total de opciones definidas (se inicializa a 1)
 | opcion: lista de opciones (se inicializa la opcion visualizar)
 +--------------------------------------------------------------------*/
static int opcDef = 1;
static int totOpc = 1;
static TipoOPCION opcion[MAX_OPCION] = {{
		'v', "verbose", "execute with messages "
		}};

/*--------------------------------------------------------------------
 | extFich: extension de el/los fichero/s a tratar.
 | desFich: descripcion de el/los fichero/s a tratar (para la ayuda).
 | argFich: flag que indica si hay definido fichero como argumento o no.
 | varFich: flag para admitir mas de un fichero (varios) como argumento.
 | lstFich: lista de ficheros dados en los argumentos (acaba en NULL).
 | totFich: numero de ficheros en la lista "lstFich".
 +--------------------------------------------------------------------*/
static char  *extFich = NULL;
static char  *desFich = NULL;
static int    argFich = 0;
static int    varFich = 1;
static char **lstFich = NULL;
static int    totFich = 0;

/*--------------------------------------------------------------------
 | nomProc: nombre del proceso (para la ayuda)
 | desProc: descripcion del proceso (para la ayuda)
 +--------------------------------------------------------------------*/
static char *nomProc  = NULL;
static char *dirProc  = NULL;
static char *desProc  = NULL;

/*====================== PROCEDIMIENTO PUBLICO ======================
 * APF_DefArgumento
 *	Declara un argumento (opcion con valor) como aceptable en
 *	los argumentos.
 *
 *    Parametros:
 *	letra: letra de la opcion asociada al argumento.
 *	nombre: nombre del argumento.
 *	descripcion: descripcion del argumento.
 *	nomVal: nombre del valor asociado a la opcion.
 *====================================================================*/
void APF_DefArgumento(char letra, char *nombre, char *descripcion, char *nomVal)
{
/*--------------------------------------------------------------------
 | Se guardan la definicion como una opcion, pero con valor asociado.
 +--------------------------------------------------------------------*/
APF_DefOpcion(letra, nombre, descripcion);
opcion[totOpc - 1].conVal = 1;
opcion[totOpc - 1].nomVal = nomVal;
}

/*====================== PROCEDIMIENTO PUBLICO ======================
 * APF_DefOpcion
 *	Declara una opcion como aceptable en los argumentos.
 *
 *    Parametros:
 *	letra: letra de la opcion.
 *	nombre: nombre de la opcion.
 *	descripcion: descripcion de la opcion.
 *====================================================================*/
void APF_DefOpcion(char letra, char *nombre, char *descripcion)
{
/*--------------------------------------------------------------------
 | Se guardan la definicion de la opcion y se pone como no existe.
 +--------------------------------------------------------------------*/
opcion[totOpc].letra   = letra;
opcion[totOpc].nombre  = nombre;
opcion[totOpc].descrip = descripcion;
opcion[totOpc].conVal  = 0;
opcion[totOpc].valor   = NULL;
opcion[totOpc].nomVal  = NULL;
opcion[totOpc].existe  = 0;
totOpc++;
if (totOpc >= MAX_OPCION)
    ERR_PorTerminal("increase MAX_OPCION in module apf.c");
}

/*====================== PROCEDIMIENTO PUBLICO ======================
 * APF_SinDefiniciones
 *	Declara que no se admiten definiciones en el fichero a tratar.
 *	O sea elimina la opcion -Dxxx=yyy.
 *	Se debe llamar antes de "APF_Analizar"
 *====================================================================*/
void APF_SinDefiniciones()
{
opcDef = 0;
}

/*====================== PROCEDIMIENTO PUBLICO ======================
 * APF_SinVisualizar
 *	Declara que no se admite la opcion de visualizar (opcion -v)
 *	(existe por defecto).
 *	Se debe llamar antes que ninguna otra funcion.
 *====================================================================*/
void APF_SinVisualizar()
{
totOpc = 0;
}

/*====================== PROCEDIMIENTO PUBLICO ======================
 * APF_DefFichero
 *	Define el formato (extension) y numero (1 o n) de los
 *	ficheros a procesar.
 *
 *    Parametros:
 *	numero: 1: solo un fichero / n: cualquier numero de ficheros
 *	extens: extension de los ficheros a procesar.
 *	descripcion: descripcion de los ficheros a procesar.
 *====================================================================*/
void APF_DefFichero(int numero, char *extens, char *descripcion)
{
/*--------------------------------------------------------------------
 | Se guardan la definicion de fichero (para solo uno o varios)
 +--------------------------------------------------------------------*/
if (numero > 1)
    varFich = 1;
else
    varFich = 0;
extFich = extens;
desFich = descripcion;
argFich = 1;
}

/*====================== PROCEDIMIENTO PUBLICO ======================
 * APF_DefDescripcion
 *	Texto que define la funcion de la herramienta. las sucesivas
 *	llamadas encadenan el texto.
 *
 *    Parametros:
 *	texto: texto de la funcionalidad.
 *====================================================================*/
void APF_DefDescripcion(char *texto)
{
/*--------------------------------------------------------------------
 | Se guarda el texto de descripcion (encadenandolo con el anterior)
 +--------------------------------------------------------------------*/
if (desProc == NULL)
    desProc = MDI_String(texto);
else
    desProc = MDI_EncString(desProc, texto);
}

/*====================== PROCEDIMIENTO PUBLICO ======================
 * APF_Uso
 *	Escribe el uso de la herramienta (opciones y su descripcion)
 *	encabezando el mensaje con un texto (formato printf) si se
 *	quiere, o NULL si no se quiere encabezamiento.
 *	Si hay texto de error (formatoErr no NULL), se finaliza el
 *	programa con -1, caso contrario se finaliza con 0.
 *
 *    Parametros:
 *	formatoErr: formato <printf> o NULL.
 *	...: parametros del formato.
 *====================================================================*/
void APF_Uso(char *formatoErr, ...)
{
/*--------------------------------------------------------------------
 | - Si hay texto de error asociado, se escribe este, y despues se
 |   escribe el USO. Se sale con (-1).
 | - Si no hay texto de error, solo se escribe el USO y se sale con 0.
 +--------------------------------------------------------------------*/
if (formatoErr) {
        va_list argum;
    va_start(argum, formatoErr);
    fprintf(stderr, "Error: ");
    vfprintf(stderr, formatoErr, argum);
    fprintf(stderr, "\n");
    fflush(stderr);
    va_end(argum);
    EscrUso();
    exit(-1);
    }

/*--------------------------------------------------------------------
 | Se escribe la lista de opciones.
 +--------------------------------------------------------------------*/
EscrUso();
exit(0);
}

/*====================== PROCEDIMIENTO PUBLICO ======================
 * APF_Analizar
 *	Realiza el analisis de argumentos de llamada a la herramienta
 *
 *    Parametros:
 *	argc: numero de argumentos en la llamada: main(int argc, ...)
 *	argv: array de argumentos de la llamada: main(..., char *argv)
 *	argv: 
 *====================================================================*/
void APF_Analizar(int argc, char *argv[])
{
	int i;
	char *pArgum;

/*--------------------------------------------------------------------
 | El nombre del proceso es el argumento numero 0
 +--------------------------------------------------------------------*/
dirProc = dirname(argv[0]);
nomProc = argv[0];

/*--------------------------------------------------------------------
 | Recorrido por todos los argumentos de llamada a la herramienta.
 +--------------------------------------------------------------------*/
for (i = 1; i < argc; i++) {
     pArgum = argv[i];
    /*----------------------------------------------------------------
     | 				OPCIONES
     +----------------------------------------------------------------*/
    if (*pArgum == '-') {
	pArgum++;
	/*------------------------------------------------------------
	 | Opcion ayuda: se ignoran el resto de argumentos
	 +------------------------------------------------------------*/
	if (*pArgum == '?')
	    Ayuda();

	/*------------------------------------------------------------
	 | Opcion de directorio: -Idir:dir:...:dir
	 | Se extraen todos los directorios y se pasan al modulo PPR
	 +------------------------------------------------------------*/
	else if (*pArgum == 'I') {
		char *pDirecc;

	    if (!opcDef)
		APF_Uso("opcion -I no aplicable");
	    pArgum++;
	    pDirecc = strtok(pArgum, ":");
	    while(pDirecc != NULL) {
		PPR_Directorio(pDirecc);
		pDirecc = strtok(NULL, ":");
		}
	    }

	/*------------------------------------------------------------
	 | Opcion de definicion: (1) -Dxxxx   (2) -Dxxxx=yyyy
	 | - Si no hay nada despues de la 'D', falta la definicion
	 | - Si no hay un '=' es el formato (1)
	 | - Si solo hay un '=' es el formato (2), si no error.
	 +------------------------------------------------------------*/
	else if (*pArgum == 'D') {
		char *pSust;

	    if (!opcDef)
		APF_Uso("option -D not apply");
	    pArgum++;
	    if (*pArgum == '\0')
		APF_Uso("option -D without definition");

	    pSust = strchr(pArgum, '=');
	    if (pSust == NULL)
	        pSust = "";
	    else if (strchr(pArgum, '=') == pSust)
		*pSust++ = '\0';
	    else
		APF_Uso("definition with multiple asignamnt ");

	    switch(PPR_Definicion(pArgum, pSust)) {
		case -1:
		    APF_Uso("lack of definition");
		    break;
		case -2:
		    APF_Uso("symbol of the definition \"%s\"", pArgum);
		    break;
		case -3:
		    APF_Uso("redefinition of \"%s\"", pArgum);
		    break;
		}
	    }
	/*------------------------------------------------------------
	 | Resto de opciones:
	 |  - Si encuentra la opcion, se comprueba si esta duplicada.
	 |    Si no lo esta, se pone la opcion como existente.
	 |  - Caso contrario es una opcion erronea.
	 |  - Se comprueba que la opcion con argumento lo tenga y si es
	 |    asi, se guarda, y que la opcion sin argumento no lo tenga.
	 +------------------------------------------------------------*/
	else {
		int j;

	    j = BuscaOpcion(*pArgum);
	    if (j == -1)
		APF_Uso("option '-%c' unknwon", *pArgum);
	    if (opcion[j].existe)
		APF_Uso("duplicate '-%c' option", *pArgum);
	    opcion[j].existe = 1;
	    pArgum++;
	    if (opcion[j].conVal) {
		if (*pArgum == '\0')
		    APF_Uso("missing asociate value for the option '-%c'",
				*(pArgum - 1));
		opcion[j].valor = pArgum;
		}
	    else
	        if (*pArgum != '\0')
		    APF_Uso("option '-%c' dont admit argument", *(pArgum-1));
	    }
	}
    /*----------------------------------------------------------------
     | 				FICHEROS
     | - Se comprueba que el numero de fichero es segun lo definido
     | - Se analiza que el fichero tenga extension y que sea la definida
     | - Se comprueba que el fichero exista y sea accesible para lectura
     | - Si es correcto, se crea una nueva entrada para el nuevo fichero
     +----------------------------------------------------------------*/
    else {
		char *pExtens;

	if (!argFich)
	    APF_Uso("unknown \"%s\" argument", pArgum);
	if (totFich && !varFich)
	    APF_Uso("only one file admit ");
	if (extFich) {
	    pExtens = strrchr(pArgum, '.');
	    if (pExtens == NULL)
		APF_Uso("file \"%s\" without extension", pArgum);
	    pExtens++;
	    if (strcmp(pExtens, extFich))
		APF_Uso("extension of file \"%s\"", pArgum);
	    }
	if (access(pArgum, F_OK) == -1)
        {
	   ERR_PorTerminal("file \"%s\" don't exists", pArgum);
           return;
        }
	if (access(pArgum, R_OK) == -1)
	    ERR_PorTerminal("file \"%s\" without read access", pArgum);

	MDI_ElemArray((void **)&lstFich, &totFich, sizeof(char *));
	lstFich[totFich - 1] = pArgum;
	}
    }

/*--------------------------------------------------------------------
 | Si no hay ningun fichero es un error (ha de haber minimo 1)
 +--------------------------------------------------------------------*/
if (argFich && (totFich == 0))
    APF_Uso("file missing ");

/*--------------------------------------------------------------------
 | Se pone el ultimo (nuevo) elemento del array de ficheros a NULL
 +--------------------------------------------------------------------*/
MDI_ElemArray((void **)&lstFich, &totFich, sizeof(char *));
lstFich[totFich - 1] = NULL;
}

/*========================= FUNCION PUBLICA =========================
 * APF_Argumento
 *	Consulta el valor asociado a una opcion con argumento.
 *
 *    Parametros:
 *	letraOpc: letra de opcion asociada al argumento.
 *    Devuelve:
 *	valor asociado: si la opcion existe en los argumentos de llamada.
 *	NULL: si la opcion no existe en los argumentos de llamada.
 *====================================================================*/
char *APF_Argumento(char letraArg)
{
	int i;

/*--------------------------------------------------------------------
 | Se busca la opcion entre las definidas. Si no se encuentra es un
 | error interno de programacion.
 +--------------------------------------------------------------------*/
i = BuscaOpcion(letraArg);
if (i >= 0)
    return opcion[i].valor;

ERR_PorTerminal("option '-%c' unkown", letraArg);
return NULL;
}


/*========================= FUNCION PUBLICA =========================
 * APF_Opcion
 *	Consulta si una opcion esta entre los argumentos de llamada.
 *
 *    Parametros:
 *	letraOpc: letra de opcion por la que se pregunta.
 *    Devuelve:
 *	1: si la opcion existe en los argumentos de llamada.
 *	0: si la opcion no existe en los argumentos de llamada.
 *====================================================================*/
int APF_Opcion(char letraOpc)
{
	int i;

/*--------------------------------------------------------------------
 | Se busca la opcion entre las definidas. Si no se encuentra es un
 | error interno de programacion.
 +--------------------------------------------------------------------*/
i = BuscaOpcion(letraOpc);
if (i >= 0)
    return opcion[i].existe;

ERR_PorTerminal("option '-%c' unknown", letraOpc);
return 0;
}

/*====================== FUNCION PUBLICA ======================
 * APF_DirName
 *	Retorna el directorio del fichero plantilla
 *
 *    Devuelve:
 *      El dirname del argv[0]
 *====================================================================*/
char *APF_DirName(char * fplantilla)
{
  struct stat st;
  char * path_env;
  char * path_busca;
  char * dir;
  char fpatron[1024];
  /* ------------------------
   * Si se ha ejecutado con path absoluto o relativo devolvemos ese path
   * en caso contrario buscamos el fichero en el path 
   * -------------------------------------------------------- */
  if (strcmp(dirProc,".") != 0)
   return dirProc;
  else
  {
    /* --------------------------
     *  Primero busco en el directorio actual.
     *  y si no en el path.
     * -------------------------------------------- */
    sprintf(fpatron,"./%s",fplantilla);
    if (stat(fpatron,&st) == 0) return "./";
    path_env = getenv("PATH");
    if (path_env == NULL) return NULL;
    path_busca = strdup(path_env);
    
    dir = strtok(path_busca,":"); 
    while (dir != NULL)
    {
       sprintf(fpatron,"%s/%s",dir,fplantilla);
       if (stat(fpatron,&st) == 0)
       {
         return dir;
       }
       dir = strtok(NULL,":");
    }
    return NULL;
  }
}


/*========================= FUNCION PUBLICA =========================
 * APF_Fichero
 *	Consulta el nombre del fichero  a procesar (cuando es solo 1).
 *
 *    Devuelve:
 *	El nombre del fichero.
 *====================================================================*/
char  *APF_Fichero()
{
/*--------------------------------------------------------------------
 | Si es un solo fichero es el primero de la lista.
 | error interno de programacion.
 +--------------------------------------------------------------------*/
return lstFich[0];
}

/*========================= FUNCION PUBLICA =========================
 * APF_LstFich
 *	Consulta la lista de ficheros a procesar (acabada en NULL).
 *
 *    Devuelve:
 *	La lista de ficheros correspondiente a los argumentos.
 *====================================================================*/
char **APF_LstFich()
{
/*--------------------------------------------------------------------
 | Se devuelve la lista  de ficheros completa
 +--------------------------------------------------------------------*/
return lstFich;
}

/*====================== PROCEDIMIENTO PRIVADO ======================
 * Ayuda
 *	Escribe en el stderr el texto de ayuda segun se hayan definido
 *	las opciones y definicion de la herramienta.
 *
 *====================================================================*/
static void Ayuda()
{
fprintf(stderr,"--------------------------------------------------------------------------------\n");

EscrDescripcion();
fprintf(stderr,"--------------------------------------------------------------------------------\n");
exit(0);
}

/*====================== PROCEDIMIENTO PRIVADO ======================
 * EscrUso
 *	Escribe en el stderr el formato de uso de la herramienta.
 *
 *====================================================================*/
static void EscrUso()
{
	int i;

/*--------------------------------------------------------------------
 | Se escribe el comando con todas las opciones y el fichero
 | Se escriben la opcion -D y el formato del fichero si es posible
 | fichero/s como argumento.
 +--------------------------------------------------------------------*/
fprintf(stderr, "USO: %s  -?", nomProc);
if (opcDef)
    fprintf(stderr, " -Dxxx=yyy -Idir:...:dir");
for (i = 0; i < totOpc; i++)
    fprintf(stderr, " -%c%s", opcion[i].letra, (opcion[i].conVal) ? "xxx" : "");
if (argFich) {
    if (extFich)
	fprintf(stderr, " fich.%s%s", extFich, (varFich ? " ..." : ""));
    else
	fprintf(stderr, " fich%s", (varFich ? " ..." : ""));
    }
printf("\n");
}

/*====================== PROCEDIMIENTO PRIVADO ======================
 * EscrDescripcion
 *	Escribe en el stderr texto descriptivo de la herramienta y a
 *	continuacion se escribe el formato de uso, junto con la
 *	descripcion de cada una de las opciones (segun definicion).
 *
 *====================================================================*/
static void EscrDescripcion()
{
	int i;

/*--------------------------------------------------------------------
 | Se escribe la descripcion de la funcion del comando (si la hay)
 | Se escribe la descripcion de cada una de las opciones y del fichero
 +--------------------------------------------------------------------*/
if (desProc)
    fprintf(stderr, "%s.- %s\n\n", nomProc, desProc);

/*--------------------------------------------------------------------
 | Se escribe el formato de uso, la descripcion de las opciones
 | standard asi como las particulares definidas en la herramienta
 | Se escriben la opcion -D y la descripcion del fichero si es posible
 | fichero/s como argumento.
 +--------------------------------------------------------------------*/
EscrUso();
fprintf(stderr, "  -?        help: this mensaje \n");
if (opcDef) {
    fprintf(stderr, "  -Dxxx=yyy definition: define \"xxx\" as \"yyy\"\n");
    fprintf(stderr, "  -Idir:...:dir directories: for the include files \n");
    }
for (i = 0; i < totOpc; i++) {
    fprintf(stderr, "  -%c%s     %s: %s\n",
		opcion[i].letra,
		(opcion[i].conVal) ? "xxx" : "   ",
		opcion[i].nombre,
		opcion[i].descrip);
    if (opcion[i].conVal)
	fprintf(stderr, "\t    xxx = \"%s\"\n", opcion[i].nomVal);
    }

if (argFich) {
    if (extFich)
	fprintf(stderr, "  fich.%-*s %s\n", 4, extFich, desFich);
    else
	fprintf(stderr, "  fich      %s\n", desFich);
    }
}

/*========================= FUNCION PRIVADA =========================
 * APF_Opcion
 *	Busca una opcion entre las predefinidas.
 *
 *    Parametros:
 *	letraOpc: letra de opcion por la que se pregunta.
 *    Devuelve:
 *	el indice en la lista: si la opcion existe.
 *	-1: si la opcion no existe.
 *====================================================================*/
static int BuscaOpcion(char letraOpc)
{
	int i;

/*--------------------------------------------------------------------
 | Se busca la opcion entre las definidas. Si la encuentra devuelve
 | el indice, si no devuelve -1;
 +--------------------------------------------------------------------*/
for (i = 0; i < totOpc; i++)
    if (opcion[i].letra == letraOpc)
	return i;
return -1;
}
