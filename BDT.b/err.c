#include <stdio.h>
#include <errno.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <time.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdarg.h>

#include <err.h>
#include <tpc.h>
#include <mem_din.h>

/*--------------------------------------------------------------------
 | ST_NORMAL: estado normal de un bloque funcional
 | ST_ALARMA: estado de alarma de un bloque funcional
 +--------------------------------------------------------------------*/
#define ST_NORMAL	'N'
#define ST_ALARMA	'A'

#define CNF_LON_APLICACION 50
#define CNF_LON_PROCESO	   50
/*--------------------------------------------------------------------
 | PERIODO: periodo de tiempo para las estadisticas de errores
 | UMBRAL_DEF: numero umbral de errores por defecto para las estadisticas
 +--------------------------------------------------------------------*/
#define PERIODO		1L
#define UMBRAL_DEF	10

/*--------------------------------------------------------------------
 | NUM_BLK_MD: numero de bloques funcionales para la gestion de m. dinam.
 | TAM_BLK_FUNC: tamano en bytes de la struct BLK_FUNC
 +--------------------------------------------------------------------*/
#define NUM_BLK_MD	10
#define TAM_BLK_FUNC	(sizeof(BLK_FUNC))

/*--------------------------------------------------------------------
 | FICH_ERR: Fichero para dejar los errores.
 | FIFO_ERROR: path de la fifo para enviar los errores a GESTION
 +--------------------------------------------------------------------*/
#define FICH_ERROR      "errores.txt"
/**********
 * NOTA berpi 12/03/1999:
 *      Puesta ruta absoluta para ser inmune al arranque desde
 *      directorios distintos.
#define FIFO_ERROR      "./ppERROR"
 **********/
#define FIFO_ERROR      "/usr/tmp/ppERROR"

/*--------------------------------------------------------------------
 | TAM_REG_ERROR: tamano del error aplanado para su envio y recepcion.
 +--------------------------------------------------------------------*/
#define TAM_REG_ERROR	((CNF_LON_APLICACION	* TPC_TAM_COD_CHAR) + \
			 (CNF_LON_PROCESO	* TPC_TAM_COD_CHAR) + \
			 (ERR_LON_BLK_FUNC	* TPC_TAM_COD_CHAR) + \
			 (ERR_LON_FUNCION	* TPC_TAM_COD_CHAR) + \
			 (TPC_TAM_COD_LONG	+ TPC_TAM_COD_CHAR) + \
			 (ERR_LON_TEXTO		* TPC_TAM_COD_CHAR))
 
/*--------------------------------------------------------------------
 | LON_TXT_ALARMA: longitud del texto de una alarma.
 | POS_MAX_ALARMA: posicion maxima en el fichero circular de alarmas.
 +--------------------------------------------------------------------*/
#define LON_TXT_ALARMA	ERR_LON_TEXTO
#define TAM_REG_ALARMA	((CNF_LON_APLICACION	* TPC_TAM_COD_CHAR) + \
			 (CNF_LON_PROCESO	* TPC_TAM_COD_CHAR) + \
			 (ERR_LON_BLK_FUNC	* TPC_TAM_COD_CHAR) + \
			 (TPC_TAM_COD_LONG	+ TPC_TAM_COD_CHAR) + \
			 (LON_TXT_ALARMA	* TPC_TAM_COD_CHAR))

/*----------------------------------------------------------------
 * struct BLK_FUNC
 *	nombre: nombre del bloque funcional
 *	estado: estado en que se encuentra (ST_NORMAL, ST_ALARMA).
 *	cuenta: contador de errores del bloque funcional.
 *	umbral: umbral de errores que se considera no normal.
 *	tiempo: tiempo de inicio de cuenta de errores.
 *----------------------------------------------------------------*/
typedef struct {
    char  nombre[ERR_LON_BLK_FUNC];
    char  estado;
    int   cuenta;
    int   umbral;
    float media;
    time_t  tiempo;
    } BLK_FUNC;

/*====================================================================
 *		DECLARACION DE FUNCIONES PRIVADAS
 *====================================================================*/
static void  InicInterna(char *, char *);
static void  TratarError(char, char *, char *, char *);
static void  EnviarError(char, char *, char *);
static void  EnviarAlarma(char *, char, int, time_t);
static int   EscrEnFifo(char *, int);
static int   ErrorAFichero(char, char *, time_t, char *);
static int   AlarmaAFichero(char, char *, time_t, char *);
static void  IniBlkFuncional(int, char);
static int   BusBlkFuncional(char *);
static char *TxtFecha(time_t);
static void  CopiaTexto(char *, int, char *);

/*--------------------------------------------------------------------
 | aplicacion: nombre de la aplicacion a gestionar.
 | proceso: nombre de la proceso a gestionar.
 +--------------------------------------------------------------------*/
static char aplicacion[CNF_LON_APLICACION + 1]	= "**???**";
static char proceso[CNF_LON_PROCESO + 1]	= "**???**";

/*--------------------------------------------------------------------
 | lstBlkFunc: lista de funciones definidas en la aplicacion.
 | numBlkFunc: numero total de bloques funcionales.
 | maxBlkFunc: maximo de bloques funcionales en mem. dinamica en cada
 |	       momento (maxBlkFunc - numBlkFunc = registros libres).
 +--------------------------------------------------------------------*/
static BLK_FUNC *lstBlkFunc = NULL;
static int numBlkFunc = 0;
static int maxBlkFunc = 0;

/*--------------------------------------------------------------------
 | fdFIFO: descriptor de la fifo para la comunicacion de/con GESTION.
 | soyGestion: indicacion de si la aplicacion es GESTION.
 | funcErrorDeGestion: funcion de tratamiento de error de GESTION.
 | funcAlarmaDeGestion: funcion de tratamiento de alarma de GESTION.
 +--------------------------------------------------------------------*/
static int fdFIFO = -1;
static int soyGestion = 0;
static void (*funcErrorDeGestion)(char, char *, char *, char *, int, char *);
static void (*funcAlarmaDeGestion)(char, char *, char *, char *, int, char *);

static int nroErroresTerminal = 0;/* Solo para errores por terminal */
/*====================== PROCEDIMIENTO PUBLICO ======================
 * ERR_Inicializar
 *	Inicializa el modulo para el tratamiento de errores.
 *
 *    Parametros:
 *	nomAplic: nombre de la aplicacion.
 *	nomProc: nombre de la proceso.
 *====================================================================*/
void ERR_Inicializar(char *nomAplic, char *nomProc)
{
/*--------------------------------------------------------------------
 | Inicializacion como aplicacion del modulo (identificacion y registros)
 +--------------------------------------------------------------------*/
soyGestion = 0;
InicInterna(nomAplic, nomProc);
}

/*====================== PROCEDIMIENTO PUBLICO ======================
 * ERR_IniGestor
 *      Inicializa el tratamiento de errores de Gestion.
 *
 *    Parametros:
 *      nomAplic: nombre de la aplicacion.
 *      nomProc: nombre de la proceso.
 *      funcError: puntero a la funcion de tratamiento de error.
 *      funcAlarma: puntero a la funcion de tratamiento de Alarma.
 *====================================================================*/
int ERR_IniGestor(char *nomAplic, char *nomProc,
       	void (*funcError)(char, char *, char *, char *, int, char *),
       	void (*funcAlarma)(char, char *, char *, char *, int, char *))
{
/*--------------------------------------------------------------------
 | Inicializacion como Gestion del modulo:
 |	- Se inicializa como cualquier otra aplicacion.
 |	- Se pone a 1 el indicador de gestion.
 |	- Se guardan las funciones de tratamiento de error y alarma.
 |	- Se crea la FIFO de errores y alarmas y se abre para lectura.
 +--------------------------------------------------------------------*/
InicInterna(nomAplic, nomProc);
soyGestion	    = 1;
funcErrorDeGestion  = funcError;
funcAlarmaDeGestion = funcAlarma;
unlink(FIFO_ERROR);
if (mkfifo(FIFO_ERROR, 0666) == -1) {
    ERR_PorConsola("fail to creat FIFO for errors");
    exit(-1);
    }
fdFIFO = open(FIFO_ERROR, O_RDWR);
if (fdFIFO == -1) {
    ERR_PorConsola("fail to open FIFO for errors ");
    exit(-1);
    }

return fdFIFO;
}

/*====================== PROCEDIMIENTO PUBLICO ======================
 * ERR_Finalizar
 *	Finaliza el tratamiento de errores (libera todos los recursos)
 *
 *====================================================================*/
void ERR_Finalizar()
{
/*--------------------------------------------------------------------
 | Libera la memoria dinamica de los bloques funcionales.
 +--------------------------------------------------------------------*/
if (soyGestion && (fdFIFO != -1))
    close(fdFIFO);
if (lstBlkFunc)
    free(lstBlkFunc);
numBlkFunc = 0;
maxBlkFunc = 0;
}
 
/*====================== PROCEDIMIENTO PUBLICO ======================
 * ERR_Reiniciar
 *	Ponen los bloques funcionales en estado inicial.
 *
 *    Parametros:
 *	pRespuesta: puntero del mensaje de respuesta.
 *	pLongitud:  longitud del mensaje anterior.
 *====================================================================*/
int ERR_Reiniciar(char **pRespuesta, int *pLongitud)
{
	int ind;

/*--------------------------------------------------------------------
 | Se ponen todas las funciones en estado NORMAL.
 +--------------------------------------------------------------------*/
for (ind = 0; ind < numBlkFunc; ind++)
    IniBlkFuncional(ind, ST_NORMAL);
*pRespuesta = "reiniciado el estado de las funciones";
*pLongitud  = strlen(*pRespuesta);
  return 0;
}

/*====================== PROCEDIMIENTO PUBLICO ======================
 * ERR_Estado
 *	Informa a Gestion del estado de los bloques funcionales.
 *
 *    Parametros:
 *	pRespuesta: puntero del mensaje de respuesta.
 *	pLongitud:  longitud del mensaje anterior.
 *====================================================================*/
char * ERR_Estado()
{
	int    ind;
	time_t tmpAct;
	char  *pMen;
	static char *bufMen = NULL;
	static int   lonBuf = 0;

/*--------------------------------------------------------------------
 | Si no hay buffer, se crea el primer bloque del buffer-respuesta
 +--------------------------------------------------------------------*/
if (lonBuf == 0) {
    lonBuf = 1024;
    bufMen = malloc(lonBuf);
    }

/*--------------------------------------------------------------------
 | Si no hay bloques funcionales, se informa de tal.
 +--------------------------------------------------------------------*/
if (numBlkFunc == 0) {
    strcpy(bufMen,"No hay funciones definidas");
    return bufMen;
    }


/*--------------------------------------------------------------------
 | Si hay bloques funcionales, se lista el estado de estos.
 +--------------------------------------------------------------------*/
pMen  = bufMen;
pMen += sprintf(pMen, "\tbloque funcional estado cuenta umbral periodo\n");
pMen += sprintf(pMen, "\t---------------- ------ ------ ------ -------\n");
time(&tmpAct);
for (ind = 0; ind < numBlkFunc; ind++) {
    if ((pMen - bufMen) < 80) {
	lonBuf += 1024;
	bufMen = realloc(bufMen, lonBuf);
	}
    pMen += sprintf (pMen, "\t%-*s %s  %4d   %4d    %ld\n",
		ERR_LON_BLK_FUNC, lstBlkFunc[ind].nombre,
		(lstBlkFunc[ind].estado == ST_NORMAL) ? "NORMAL" : "ALARMA",
		lstBlkFunc[ind].cuenta,
		lstBlkFunc[ind].umbral,
		tmpAct - lstBlkFunc[ind].tiempo);
    }
  return bufMen;
}
 
/*====================== PROCEDIMIENTO PRIVADO ======================
 * InicInterna
 *	Recoge los datos de identificacion de la aplicacion y los
 *	guarda. Crea un bloque de registros de bloque funcional
 *	Inicializa la invocacion de las funciones de las ordenes.
 *
 *    Parametros:
 *	nomAplic: nombre de la aplicacion.
 *	nomProc: nombre de la proceso.
 *====================================================================*/
static void InicInterna(char *nomAplic, char *nomProc)
{
	static int inicializado = 0;
	int tamMemoria;

/*--------------------------------------------------------------------
 | Si el modulo ya esta inicializado, no hay que hacer nada.
 +--------------------------------------------------------------------*/
if (inicializado)
    return;

/*--------------------------------------------------------------------
 | Copia los nombres de aplicacion, unidad funcional y proceso.
 +--------------------------------------------------------------------*/
CopiaTexto(nomAplic, CNF_LON_APLICACION, aplicacion);
CopiaTexto(nomProc,  CNF_LON_PROCESO,	 proceso);

/*--------------------------------------------------------------------
 | Pide el primer lote de registros de bloques funcionales en m. dinam.
 +--------------------------------------------------------------------*/
maxBlkFunc = NUM_BLK_MD;
tamMemoria = maxBlkFunc * TAM_BLK_FUNC;
lstBlkFunc = (BLK_FUNC *) MALLOC(tamMemoria, "InicInterna", "struct BLK_FUNC");
inicializado = 1;
}

/*====================== PROCEDIMIENTO PUBLICO ======================
 * ERR_BloqueFuncional
 *	Declara un nuevo bloque funcional de la aplicacion para su
 *	tratamiento en estadisticas de errores.
 *
 *    Parametros:
 *	nombre: nombre del bloque funcional.
 *	umbral: umbral de errores por periodo (frecuencia) considerado
 *		como critico.
 *====================================================================*/
void ERR_BloqueFuncional(char *nombre, int umbral)
{
	int tamMemoria;

/*--------------------------------------------------------------------
 | Si no quedan registros libres (en el bloque), se pide un nuevo bloque
 +--------------------------------------------------------------------*/
if (numBlkFunc >= maxBlkFunc) {
    maxBlkFunc += NUM_BLK_MD;
    tamMemoria  = maxBlkFunc * TAM_BLK_FUNC;
    lstBlkFunc = (BLK_FUNC *) REALLOC((char *)lstBlkFunc, tamMemoria,
    				      "ERR_BloqueFuncional", "struct BLK_FUNC");
    }

/*--------------------------------------------------------------------
 | Se inicializa el nuevo bloque funcional.
 |	- se copia su nombre.
 |	- se copia el umbral.
 |	- se calcula la media.
 |	- se inicializa la cuenta, estado y tiempo.
 | Se incrementa el numero de bloques funcionales.
 +--------------------------------------------------------------------*/
CopiaTexto(nombre, ERR_LON_BLK_FUNC, lstBlkFunc[numBlkFunc].nombre);
lstBlkFunc[numBlkFunc].umbral = umbral;
lstBlkFunc[numBlkFunc].media  = (float) (umbral / PERIODO);
IniBlkFuncional(numBlkFunc, ST_NORMAL);
numBlkFunc++;
}
    
/*====================== PROCEDIMIENTO PUBLICO ======================
 * ERR_Aplicacion
 *	Reporta y trata un error de Aplicacion.
 *
 *    Parametros:
 *	gravedad: gravedad del error (ERR_FATAL, ERR_NO_FATAL).
 *	blkFuncional: nombre del bloque funcional que se asocia al error.
 *	funcApl:  nombre de la funcion C en que se produce el error.
 *	formato:  formato (tipo sprintf) del texto del error.
 *	...:      parametros del formato.
 *====================================================================*/
void ERR_Aplicacion(char gravedad, char *blkFuncional,
		    char *funcApl, char *formato, ...)
{
	char	texto[ERR_LON_TEXTO * 2];
        va_list	params;

/*--------------------------------------------------------------------
 | Se protege ante punteros NULL en blkFuncional.
 +--------------------------------------------------------------------*/
if (blkFuncional == NULL)
    blkFuncional = "NULL";

/*--------------------------------------------------------------------
 | Si mira si el formato es NULL.
 | Se formatea el texto (formato sprintf) segun el formato y parametros.
 +--------------------------------------------------------------------*/
va_start(params, formato);
if (formato == NULL)
    formato = "NULL";
vsprintf(texto, formato, params);
va_end(params);
TratarError(gravedad, blkFuncional, funcApl, texto);
}

/*====================== PROCEDIMIENTO PUBLICO ======================
 * ERR_SistOper
 *	Reporta y trata un error de Sistema Operativo.
 *
 *    Parametros:
 *	gravedad:  gravedad del error (ERR_FATAL, ERR_NO_FATAL).
 *	blkFuncional: nombre del bloque funcional que se asocia al error.
 *	funcApl:   nombre de la funcion C en que se produce el error.
 *	funcSOper: nombre de la funcion del Sistema Operativo llamada.
 *	formato:   formato (tipo sprintf) del texto del error.
 *	...:       parametros del formato.
 *====================================================================*/
void ERR_SistOper(char gravedad, char *blkFuncional, char *funcApl,
		  char *funcSOper, char *formato, ...)
{
	char	texto[ERR_LON_TEXTO * 2];
        va_list	params;

/*	extern int   errno;*/

/*--------------------------------------------------------------------
 | Se protege ante punteros NULL en blkFuncional.
 +--------------------------------------------------------------------*/
if (blkFuncional == NULL)
    blkFuncional = "NULL";

/*--------------------------------------------------------------------
 | Si mira si el formato es NULL.
 | Primero se formatea el texto del sistema operativo (con la funcion).
 | Se formatea el texto (formato sprintf) segun el formato y parametros.
 +--------------------------------------------------------------------*/
va_start(params, formato);
if (formato == NULL)
    formato = "NULL";

sprintf(texto, "%s: %s\n", funcSOper, strerror(errno));
vsprintf(texto + strlen(texto), formato, params);
va_end(params);

TratarError(gravedad, blkFuncional, funcApl, texto);
}

/*====================== PROCEDIMIENTO PUBLICO ======================
 * ERR_PorConsola
 *	Escribe un mensaje de aviso por la consola del procesador.
 *	este es ultimo nivel de error.
 *    Parametros:
 *	formato:  formato (tipo sprintf) del texto del error.
 *	...:      parametros del formato.
 *====================================================================*/
void ERR_PorTerminal(char *formato, ...)
{
   va_list	 params;

   nroErroresTerminal ++;
/*--------------------------------------------------------------------
 | Se abre el dispositivo de la consola del procesador.
 +--------------------------------------------------------------------*/
/*--------------------------------------------------------------------
 | Se escribe el mensaje con la identificacion de la aplicacion y se
 | cierra la consola.
 +--------------------------------------------------------------------*/
if (formato == NULL)
    formato = "formato del mensaje = NULL";
printf("============================================================\n");

if (formato != NULL) {
    va_start(params, formato);
    vprintf(formato, params);
    va_end(params);
    }
else
    printf("ERROR: formato = NULL");

printf("\n============================================================\n");
}

/*====================== PROCEDIMIENTO PUBLICO ======================
 * ERR_HayErrores
 *	Escribe un mensaje de aviso por la consola del procesador.
 *	este es ultimo nivel de error.
 *    Parametros:
 *	formato:  formato (tipo sprintf) del texto del error.
 *	...:      parametros del formato.
 *====================================================================*/
int ERR_HayErroresTerminal()
{
   return nroErroresTerminal;
}

/*====================== PROCEDIMIENTO PUBLICO ======================
 * ERR_PorConsola
 *	Escribe un mensaje de aviso por la consola del procesador.
 *	este es ultimo nivel de error.
 *    Parametros:
 *	formato:  formato (tipo sprintf) del texto del error.
 *	...:      parametros del formato.
 *====================================================================*/
void ERR_PorConsola(char *formato, ...)
{
	FILE	*fp;
        va_list	 params;

/*--------------------------------------------------------------------
 | Se abre el dispositivo de la consola del procesador.
 +--------------------------------------------------------------------*/
fp = fopen("/dev/console", "w");
if (fp == NULL)
    return;

/*--------------------------------------------------------------------
 | Se escribe el mensaje con la identificacion de la aplicacion y se
 | cierra la consola.
 +--------------------------------------------------------------------*/
if (formato == NULL)
    formato = "formato del mensaje = NULL";
fprintf(fp, "============================================================\n");
fprintf(fp, "Aplicacion: %s, Proceso: %s\n\t", aplicacion, proceso);

if (formato != NULL) {
    va_start(params, formato);
    vfprintf(fp, formato, params);
    va_end(params);
    }
else
    fprintf(fp, "ERROR: formato = NULL");

fprintf(fp, "\n============================================================\n");
fclose(fp);
}

/*========================= FUNCION PUBLICA =========================
 * ERR_LeerTipo
 *	Lee el tipo de registro que hay en la fifo de errores.
 *
 *    Parametros:
 *	pTipo: puntero al tipo de registro.
 *    Devuelve:
 *	 0: si se lee el tipo correctamente.
 *	-1: si hay error en la lectura.
 *====================================================================*/
int ERR_LeerTipo(char *pTipo)
{
/*--------------------------------------------------------------------
 | Lee un caracter correspondiente al tipo y si hay error se da un
 | aviso por la consola del procesador.
 +--------------------------------------------------------------------*/
if (read(fdFIFO, pTipo, 1) != 1) {
    ERR_PorConsola("fail to read type in ERROR FIFO");
    return -1;
    }
return 0;
}

/*========================= FUNCION PUBLICA =========================
 * ERR_LeerError
 *	
 *
 *    Parametros:
 *	pGravedad: punt a la gravedad del error.
 *	pAplicacion: punt al nombre de la aplicacion del error.
 *	pProceso: punt al nombre del proceso del error.
 *	pFuncion: punt al nombre de la funcion del error.
 *	pFecha: punt a la fecha del error.
 *	pTexto: punt al texto del error.
 *    Devuelve:
 *	 0: si se lee el registro de error correctamente.
 *	-1: si hay error en la lectura.
 *====================================================================*/
int ERR_LeerError(char *pGravedad, char *pAplicacion, char *pProceso,
		  char *pFuncion, int *pFecha, char *pTexto)
{
	char registroErr[TAM_REG_ERROR - 1];
	char *pReg;

/*--------------------------------------------------------------------
 | Lee un registro de error y si hay error se da un aviso por la
 | consola del procesador.
 +--------------------------------------------------------------------*/
if (read(fdFIFO, registroErr, (TAM_REG_ERROR - 1)) != (TAM_REG_ERROR - 1)) {
    ERR_PorConsola("fail to read ERROR in ERROR FIFO");
    return -1;
    }

/*--------------------------------------------------------------------
 | Se decodifica el registro del error.
 +--------------------------------------------------------------------*/
pReg  = registroErr;
pReg += TPC_DecChar(pGravedad,	    1,			   pReg);
pReg += TPC_DecChar(pAplicacion,    CNF_LON_APLICACION,	   pReg);
pReg += TPC_DecChar(pProceso,	    CNF_LON_PROCESO,	   pReg);
pReg += TPC_DecChar(pFuncion,	    ERR_LON_FUNCION,	   pReg);
pReg += TPC_DecInt((int *)pFecha,	    1,		   pReg);
pReg += TPC_DecChar(pTexto,	    ERR_LON_TEXTO,	   pReg);

return 0;
}

/*========================= FUNCION PUBLICA =========================
 * ERR_LeerAlarma
 *	
 *
 *    Parametros:
 *	pTipo: punt. al tipo de alarma.
 *	pAplicacion: punt al nombre de la aplicacion de la alarma.
 *	pProceso: punt al nombre del proceso de la alarma.
 *	pBlkFuncional: punt al nombre del bloque funcional de la alarma.
 *	pFecha: punt a la fecha de la alarma.
 *	pTexto: punt al texto de la alarma.
 *    Devuelve:
 *	 0: si se lee el registro de error correctamente.
 *	-1: si hay error en la lectura.
 *====================================================================*/
int ERR_LeerAlarma(char *pTipo, char *pAplicacion, char *pProceso,
		   char *pBlkFuncional, int *pFecha, char *pTexto)
{
	char  registroAlr[TAM_REG_ALARMA - 1];
	char *pReg;

/*--------------------------------------------------------------------
 | Lee un registro de alarma y si hay error se da un aviso por la
 | consola del procesador.
 +--------------------------------------------------------------------*/
if (read(fdFIFO, registroAlr, (TAM_REG_ALARMA - 1)) != (TAM_REG_ALARMA - 1)) {
    ERR_PorConsola("fail to read ALARM from ERROR FIFO");
    return -1;
    }

/*--------------------------------------------------------------------
 | Se decodifica el registro de la alarma.
 +--------------------------------------------------------------------*/
pReg  = registroAlr;
pReg += TPC_DecChar(pTipo,	   1,			pReg);
pReg += TPC_DecChar(pAplicacion,   CNF_LON_APLICACION,	pReg);
pReg += TPC_DecChar(pProceso,	   CNF_LON_PROCESO,	pReg);
pReg += TPC_DecChar(pBlkFuncional, ERR_LON_BLK_FUNC,	pReg);
pReg += TPC_DecInt((int *)pFecha,	   1,		pReg);
pReg += TPC_DecChar(pTexto,	   ERR_LON_TEXTO,	pReg);

return 0;
}

/*====================== PROCEDIMIENTO PUBLICO ======================
 * TratarError
 *	Realiza el tratamiento del error segun la gravedad y analiza
 *	el estado de la estadistica del bloque funcional asociado al error.
 *
 *    Parametros:
 *	gravedad: gravedad del error (ERR_FATAL, ERR_NO_FATAL).
 *	blkFuncional: nombre del bloque funcional que se asocia al error.
 *	funcApl:  nombre de la funcion C en que se produce el error.
 *	texto:	  texto del error.
 *====================================================================*/
static void TratarError(char gravedad, char *blkFuncional,
			char *funcApl, char *texto)
{
	int     blkFun;
	time_t  periodo;
	float   media;
 
/*--------------------------------------------------------------------
 | Si el error es FATAL, se envia y se suicida el proceso.
 +--------------------------------------------------------------------*/
if (gravedad == ERR_FATAL) {
    EnviarError(gravedad, funcApl, texto);
    ERR_Finalizar();
    kill(getpid(), SIGTERM);
    }

/*--------------------------------------------------------------------
 | Se busca el bloque funcional del error, por el nombre.
 | Si no existe, se crea.
 +--------------------------------------------------------------------*/
blkFun = BusBlkFuncional(blkFuncional);
if (blkFun == -1) {
    ERR_BloqueFuncional(blkFuncional, UMBRAL_DEF);
    blkFun = BusBlkFuncional(blkFuncional);
    }

/*--------------------------------------------------------------------
 | Se incrementa la cuenta de errores y se calcula el periodo de tiempo
 | transcurrido desde el inicio de cuenta.
 | Se analiza el estado del bloque funcional.
 +--------------------------------------------------------------------*/
lstBlkFunc[blkFun].cuenta++;
periodo = time(NULL) - lstBlkFunc[blkFun].tiempo;
switch(lstBlkFunc[blkFun].estado) {
    /*----------------------------------------------------------------
     | Estado NORMAL:
     | 	- Se envia el error al gestor de errores.
     |	- Si se cumple el periodo, se inicia una nueva cuenta.
     |	- Si no, puede ser que se sobrepase el umbral en cuyo caso,
     |    el bloque funcional pasa a estado de ALARMA.
     +----------------------------------------------------------------*/
    case ST_NORMAL:
	EnviarError(gravedad, funcApl, texto);
	if (periodo >= PERIODO)
	    IniBlkFuncional(blkFun, ST_NORMAL);
	else if (lstBlkFunc[blkFun].cuenta >= lstBlkFunc[blkFun].umbral) {
	    EnviarAlarma(lstBlkFunc[blkFun].nombre, ERR_ACTI_ALR,
	    		 lstBlkFunc[blkFun].cuenta, periodo);
	    IniBlkFuncional(blkFun, ST_ALARMA);
	    }
	break;

    /*----------------------------------------------------------------
     | Estado ALARMA:
     | 	- Se envia el error al gestor de errores.
     |	- Si se cumple el periodo, se inicia una nueva cuenta.
     |	- Si no, puede ser que se sobrepase el umbral ??????
     +----------------------------------------------------------------*/
    case ST_ALARMA:
	if (periodo > PERIODO) {
	    media = (float) (lstBlkFunc[blkFun].cuenta / periodo);
	    if (media >= lstBlkFunc[blkFun].media) {
		EnviarAlarma(lstBlkFunc[blkFun].nombre, ERR_PERM_ALR,
			     lstBlkFunc[blkFun].cuenta, periodo);
		IniBlkFuncional(blkFun, ST_ALARMA);
		}
	    else {
		EnviarAlarma(lstBlkFunc[blkFun].nombre, ERR_CESE_ALR,
			     lstBlkFunc[blkFun].cuenta, periodo);
		EnviarError(gravedad, funcApl, texto);
		IniBlkFuncional(blkFun, ST_NORMAL);
		}
	    }
	break;
    }
}

/*====================== PROCEDIMIENTO PRIVADO ======================
 * EnviarError
 *	Realiza el envio del error al gestor de errores.
 *
 *    Parametros:
 *	gravedad: gravedad del error.
 *	funcApl:  nombre de la funcion C en que se produce el error.
 *	textoErr: texto del error.
 *====================================================================*/
static void EnviarError(char gravedad, char *funcion, char *textoErr)
{
	time_t fecha;
	char registroErr[TAM_REG_ERROR];
	char *pReg;
	int aux;

time(&fecha);
/*--------------------------------------------------------------------
 | Si la aplicacion es gestion de errores se invoca su funcion.
 +--------------------------------------------------------------------*/
if (soyGestion) {
    (*funcErrorDeGestion)(gravedad, aplicacion, proceso,
			  funcion, (int)fecha, textoErr);
    return;
    }

/*--------------------------------------------------------------------
 | Se codifica el registro del error.
 +--------------------------------------------------------------------*/
pReg  = registroErr;
*pReg++ = REG_ERROR;
pReg += TPC_CodChar(&gravedad,	   1,			  pReg);
pReg += TPC_CodChar(aplicacion,	   CNF_LON_APLICACION,	  pReg);
pReg += TPC_CodChar(proceso,	   CNF_LON_PROCESO,	  pReg);
pReg += TPC_CodChar(funcion,	   ERR_LON_FUNCION,	  pReg);
aux = (int)fecha;
pReg += TPC_CodInt((int *)&aux,	   1,		  pReg);
pReg += TPC_CodChar(textoErr,	   ERR_LON_TEXTO,	  pReg);

/*--------------------------------------------------------------------
 | Se envia el error a gestion y si hay problemas, se escribe en el
 | fichero y se avisa por la consola de que no se puede enviar un error
 | a gestion. Si hay problemas tambien con el fichero, se avisa tambien
 |  de esto por la consola.
 +--------------------------------------------------------------------*/
if (EscrEnFifo(registroErr, TAM_REG_ERROR) == -1) {
    ERR_PorConsola("can't send ERROR to management");
    if (ErrorAFichero(gravedad, funcion, fecha, textoErr) == -1)
	ERR_PorConsola("can't write ERROR in the file");
    }
}

/*====================== PROCEDIMIENTO PRIVADO ======================
 * EnviarAlarma
 *	Realiza el tratamiento del estado de alarma de un bloque
 *	funcional.
 *
 *    Parametros:
 *	blkFuncional: nombre del bloque funcional.
 *	tipo: tipo de alarma generada (ERR_ACTI_ALR/ERR_PERM_ALR/ERR_CESE_ALR).
 *	numErrores: numero de errores en el periodo.
 *	periodo: periodo de la estadistica.
 *====================================================================*/
static void EnviarAlarma(char *blkFuncional, char tipo,
			 int numErrores, time_t periodo)
{
	time_t fecha;
	char txtAlarma[ERR_LON_TEXTO];
	char registroAlr[TAM_REG_ALARMA];
	char *pReg;
   	int aux;

/*--------------------------------------------------------------------
 | Se formatea el texto con la estadistica de errores.
 +--------------------------------------------------------------------*/
sprintf(txtAlarma, "%d errores en %ld segundos", numErrores, periodo);

time(&fecha);
/*--------------------------------------------------------------------
 | Si la aplicacion es gestion de errores se invoca su funcion.
 +--------------------------------------------------------------------*/
if (soyGestion) {
    (*funcAlarmaDeGestion)(tipo, aplicacion, proceso,
			   blkFuncional, (int)fecha, txtAlarma);
    return;
    }

/*--------------------------------------------------------------------
 | Se codifica el registro del error.
 +--------------------------------------------------------------------*/
pReg  = registroAlr;
*pReg++ = REG_ALARMA;
pReg += TPC_CodChar(&tipo,	  1,			pReg);
pReg += TPC_CodChar(aplicacion,	  CNF_LON_APLICACION,	pReg);
pReg += TPC_CodChar(proceso,	  CNF_LON_PROCESO,	pReg);
pReg += TPC_CodChar(blkFuncional, ERR_LON_BLK_FUNC,	pReg);
aux = (int)fecha;
pReg += TPC_CodInt((int *)&aux,	  1,		pReg);
pReg += TPC_CodChar(txtAlarma,	  ERR_LON_TEXTO,	pReg);

/*--------------------------------------------------------------------
 | Se envia la alarma a gestion y si hay problemas, se escribe en el
 | fichero y se avisa por la consola de que no se puede enviar una alarma
 | a gestion. Si hay problemas tambien con el fichero, se avisa tambien
 | de esto por la consola.
 +--------------------------------------------------------------------*/
if (EscrEnFifo(registroAlr, TAM_REG_ALARMA) == -1) {
    ERR_PorConsola("can't send ALARM to management");
    if (AlarmaAFichero(tipo, blkFuncional, fecha, txtAlarma) == -1)
	ERR_PorConsola("can't write ALARM in the file");
    }
}

/*========================= FUNCION PRIVADA =========================
 * EscrEnFifo
 *	Escribe un registro (ERROR/ALARMA) en la fifo de errores.
 *
 *    Parametros:
 *	registro: punt. al registro a escribir.
 *	longitud: longitud del registro a escribir.
 *    Devuelve:
 *	 0: si el registro se escribe correctamente.
 *	-1: si hay error de escritura.
 *====================================================================*/
static int EscrEnFifo(char *registro, int longitud)
{
/*--------------------------------------------------------------------
 | Se abre la FIFO de errores.
 +--------------------------------------------------------------------*/
fdFIFO = open(FIFO_ERROR, O_WRONLY | O_NDELAY);
if (fdFIFO == -1)
    return -1;

/*--------------------------------------------------------------------
 | Se escribe el registro en la fifo y se cierra.
 +--------------------------------------------------------------------*/
if (write(fdFIFO, registro, longitud) != longitud) {
    close(fdFIFO);
    return -1;
    }
close(fdFIFO);
return 0;
}

/*========================= FUNCION PRIVADA =========================
 * ErrorAFichero
 *	Escribe un error en el fichero de errores formateado en ASCII.
 *
 *    Parametros:
 *	gravedad: gravedad del error (ERR_FATAL, ERR_NO_FATAL).
 *	funcion:  nombre de la funcion C en que se produce el error.
 *	fecha: fecha en formato unix (long)
 *	texto: texto del error.
 *    Devuelve:
 *	 0: si se escribe correctamente.
 *	-1: si hay error de escritura.
 *====================================================================*/
static int ErrorAFichero(char gravedad, char *funcion, time_t fecha, char *texto)
{
	FILE *fp;

/*--------------------------------------------------------------------
 | Se abre el fichero de errores para escritura.
 +--------------------------------------------------------------------*/
fp = fopen(FICH_ERROR, "a");
if (fp == NULL)
    return -1;

/*--------------------------------------------------------------------
 | Se escribe el error en formato ASCII y se cierra el fichero.
 +--------------------------------------------------------------------*/
fprintf(fp, "---------------------------------------\n");
fprintf(fp, "Error %s\n", TXT_GRAVEDAD(gravedad));
fprintf(fp, "---------------------------------------\n");
fprintf(fp, "Aplicacion\t%s\n", aplicacion);
fprintf(fp, "Proceso\t\t%s\n", proceso);
fprintf(fp, "Funcion\t\t%s\n", funcion); 
fprintf(fp, "Fecha\t\t%s\n", TxtFecha(fecha));
fprintf(fp, "%s\n", texto);
fclose(fp);
return 0;
}

/*========================= FUNCION PRIVADA =========================
 * AlarmaAFichero
 *	Escribe una alarma en el fichero de errores formateada en ASCII.
 *
 *    Parametros:
 *	tipo: tipo de alarma (ERR_ACTI_ALR/ERR_PERM_ALR/ERR_CESE_ALR).
 *	blkFuncional: nombre del bloque funcional que se asocia a la alarma.
 *	funcion:  nombre de la funcion C en que se produce e la alarma.
 *	fecha: fecha en formato unix (long)
 *	texto: texto de la alarma.
 *    Devuelve:
 *	 0: si se escribe correctamente.
 *	-1: si hay error de escritura.
 *====================================================================*/
static int AlarmaAFichero(char tipo, char *blkFuncional, 
			  time_t fecha, char *texto)
{
	FILE *fp;

/*--------------------------------------------------------------------
 | Se abre el fichero de errores para escritura.
 +--------------------------------------------------------------------*/
fp = fopen(FICH_ERROR, "a");
if (fp == NULL)
    return -1;

/*--------------------------------------------------------------------
 | Se escribe e la alarma en formato ASCII y se cierra el fichero.
 +--------------------------------------------------------------------*/
fprintf(fp, "---------------------------------------\n");
fprintf(fp, "Alarma de %s\n", TXT_TIPO_ALARMA(tipo));
fprintf(fp, "---------------------------------------\n");
fprintf(fp, "Aplicacion\t\t%s\n", aplicacion);
fprintf(fp, "Proceso\t\t\t%s\n", proceso);
fprintf(fp, "Bloque Funcional\t%s\n", blkFuncional); 
fprintf(fp, "Fecha\t\t\t%s\n", TxtFecha(fecha));
fprintf(fp, "%s\n", texto);
fclose(fp);
return 0;
}

/*====================== PROCEDIMIENTO PRIVADO ======================
 * IniBlkFuncional
 *	Inicializa el estado, cuenta e inicio de periodo (tiempo) de
 *	un bloque funcional
 *
 *    Parametros:
 *	ind: indice del bloque funcional en la lista.
 *	estado: estado a inicializar.
 *====================================================================*/
static void IniBlkFuncional(int ind, char estado)
{
/*--------------------------------------------------------------------
 | Se pone la cuenta a 0, tiempo al que haya en ese instante y estado
 | al especificado en la llamada.
 +--------------------------------------------------------------------*/
lstBlkFunc[ind].cuenta = 0;
lstBlkFunc[ind].estado = estado;
lstBlkFunc[ind].tiempo = time(NULL);
}

/*========================= FUNCION PRIVADA =========================
 * BusBlkFuncional
 *	Busca un bloque funcional en la lista, por el nombre.
 *
 *    Parametros:
 *	nombre: nombre del bloque funcional.
 *    Devuelve:
 *	el indice a la lista, si existe.
 *	-1, si no existe.
 *====================================================================*/
static int BusBlkFuncional(char *nombre)
{
	int i;

/*--------------------------------------------------------------------
 | Se recorre la lista y si lo encuentra devuelve el indice, si no -1.
 +--------------------------------------------------------------------*/
for (i = 0; i < numBlkFunc; i++)
    /**********
     * berpi 12/04/1999:
     *       Se busca el nombre con strncmp() por si acaso el nombre
     *       del bloque funcional es demasiado largo.
    if (strcmp(nombre, lstBlkFunc[i].nombre) == 0)
     **********/
    if (strncmp(nombre, lstBlkFunc[i].nombre, ERR_LON_BLK_FUNC-1) == 0)
	return i;
return -1;
}

/*========================= FUNCION PRIVADA =========================
 * TxtFecha
 *	Formatea la fecha en ASCII en castellano. Esta funcion solo
 *	no puede ser llamada simultaneamente.
 *
 *    Parametros:
 *	seg: segundos de la fecha.
 *    Devuelve:
 *	El texto de la fecha ASCII.
 *====================================================================*/
static char *TxtFecha(time_t seg)
{
	struct tm *stFecha;
	static char txtFecha[40];

/*--------------------------------------------------------------------
 | Si la fecha es -1 es una fecha erronea.
 +--------------------------------------------------------------------*/
if (seg == -1L)
    return "**???**";

/*--------------------------------------------------------------------
 | Se obtiene la struct de la fecha desglosada y se formatea en texto.
 +--------------------------------------------------------------------*/
stFecha = localtime(&seg);
sprintf(txtFecha, "%.2d/%.2d/%.4d %.2d:%.2d:%.2d",
		stFecha->tm_mday, stFecha->tm_mon + 1, stFecha->tm_year + 1900,
		stFecha->tm_hour, stFecha->tm_min, stFecha->tm_sec);
return txtFecha;
}

/*====================== PROCEDIMIENTO PRIVADO ======================
 * CopiaTexto
 *	Copia un string hasta una longitud maxima (acabando en NULL)
 *	esta funcion se hace pues strncpy no asegura el NUL al final.
 *
 *    Parametros:
 *	txtOrigen:  texto a copiar.
 *	lonMax:     longitud maxima a copiar.
 *	txtDestino: texto donde copiar.
 *====================================================================*/
static void CopiaTexto(char *txtOrigen, int lonMax, char *txtDestino)
{
/*--------------------------------------------------------------------
 | Se copia con strncpy y se pone NUL al final para asegurarlo.
 +--------------------------------------------------------------------*/
memset(txtDestino, 0, lonMax);
strncpy(txtDestino, txtOrigen, lonMax);
txtDestino[lonMax - 1] = '\0';
}

