#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include <erh.h>

/*--------------------------------------------------------------------
 | MAX_ERRORES: maximo de errores reportados (provoca un error fatal)
 +--------------------------------------------------------------------*/
#define MAX_ERRORES	20

static void CtrlNumeroErrores();

/*--------------------------------------------------------------------
 | totalErrores: cuenta el numero de errores reportados.
 +--------------------------------------------------------------------*/
static int  totalErrores = 0;

/*====================== PROCEDIMIENTO PUBLICO ======================
 * ERH_Sintaxis
 *
 * 	Escribe un texto de error de sintaxis, detectado en una linea
 *	del fichero a compilar.
 * 
 *    Parametros:
 *	numLinea: numero de la linea del error.
 *      formatoErr: formato de impresion (igual al de printf).
 *      ...: parametros del formato de impresion.
 *====================================================================*/
void ERH_Sintaxis(int numLinea, char *formatoErr, ...)
{
        va_list argum;
 
/*--------------------------------------------------------------------
 | Se escribe una linea con: numero de linea y texto de la incidencia
 +--------------------------------------------------------------------*/
va_start(argum, formatoErr);
if (numLinea > 0)
    fprintf(stderr, "[%u]\t", numLinea);
vfprintf(stderr, formatoErr, argum);
fprintf(stderr, "\n");
fflush(stderr);
va_end(argum);
CtrlNumeroErrores();
}

/*====================== PROCEDIMIENTO PUBLICO ======================
 * ERH_NoFatal
 *
 * 	Escribe un texto de error de funcionamiento interno de la
 *	herramienta.
 * 
 *    Parametros:
 *      formatoErr: formato de impresion (igual al de printf).
 *      ...: parametros del formato de impresion.
 *====================================================================*/
void ERH_NoFatal(char *formatoErr, ...)
{
	va_list argum;
 
/*--------------------------------------------------------------------
 | Se escribe una linea con texto de la incidencia
 +--------------------------------------------------------------------*/
va_start(argum, formatoErr);
fprintf(stderr, "Error: ");
vfprintf(stderr, formatoErr, argum);
fprintf(stderr, "\n");
fflush(stderr);
va_end(argum);
CtrlNumeroErrores();
}

/*====================== PROCEDIMIENTO PUBLICO ======================
 * ERH_Fatal
 *
 * 	Escribe un texto de ERROR FATAL para el usuario.
 * 
 *    Parametros:
 *      formatoErr: formato de impresion (igual al de printf).
 *      ...: parametros del formato de impresion.
 *====================================================================*/
void ERH_Fatal(char *formatoErr, ...)
{
	va_list argum;

/*--------------------------------------------------------------------
 | Se escribe el texto de error fatal.
 +--------------------------------------------------------------------*/
va_start(argum, formatoErr);
fprintf(stderr, "\nError fatal:\n    ");
vfprintf(stderr, formatoErr, argum);
fprintf(stderr, "\n\n");
fflush(stderr);
va_end(argum);
exit (-1);
}

/*====================== PROCEDIMIENTO PUBLICO ======================
 * ERH_Fatal
 *
 * 	Escribe un texto de ERROR INTERNO para el usuario.
 * 
 *    Parametros:
 *      formatoErr: formato de impresion (igual al de printf).
 *      ...: parametros del formato de impresion.
 *====================================================================*/
void ERH_Interno(char *formatoErr, ...)
{
	va_list argum;

/*--------------------------------------------------------------------
 | Se escribe el texto de error interno.
 +--------------------------------------------------------------------*/
va_start(argum, formatoErr);
fprintf(stderr, "\nError interno:\n    ");
vfprintf(stderr, formatoErr, argum);
fprintf(stderr, "\n\n");
fflush(stderr);
va_end(argum);
exit (-1);
}

/*====================== PROCEDIMIENTO PUBLICO ======================
 * ERH_SistOper
 *
 * 	Escribe un texto de ERROR de S.O. para el usuario.
 *	Siempre es un error fatal y se aborta la ejecucion.
 * 
 *    Parametros:
 *	funcion: texto de la funcion  de S. Operativo ejecutada
 *      formatoErr: formato de impresion (igual al de printf).
 *      ...: parametros del formato de impresion.
 *====================================================================*/
void ERH_SistOper(char *funcion, char *formatoErr, ...)
{
	va_list argum;

/*--------------------------------------------------------------------
 | Se escribe el texto de error de sistema operativo.
 | Se escribe el error detectado por el S. O. (perror)
 +--------------------------------------------------------------------*/
fprintf(stderr, "\nError S. Operativo:\n");
fprintf(stderr, "    ");
perror(funcion);
fprintf(stderr, "    ");
va_start(argum, formatoErr);
vfprintf(stderr, formatoErr, argum);
va_end(argum);
fprintf(stderr, "\n\n");
fflush(stderr);
}

/*========================= FUNCION PUBLICA =========================
 * ERH_Errores
 *
 * 	Informa del numero de errores informados en un momento
 *	determinado.
 *
 *    Devuelve:
 *	el numero de errores.
 *====================================================================*/
int ERH_Errores(void)
{
return totalErrores;
}

/*========================= FUNCION PUBLICA =========================
 * ERR_SistOper
 *
 * 	Procedimiento de compatibilidad con el tratamiento de errores
 *	"ERR" de la libreria general.
 *====================================================================*/
void ERR_SistOper(char tipo, char *funcionalidad, char *funcApl,
		  char *funcSOp, char *formato, ...)
{
        va_list argum;
	char textoErr[1024];
 
va_start(argum, formato);
vsprintf(textoErr, formato, argum);
ERH_SistOper(funcSOp, textoErr);
va_end(argum);
}

/*========================= FUNCION PUBLICA =========================
 * ERR_Aplicacion
 *
 * 	Procedimiento de compatibilidad con el tratamiento de errores
 *	"ERR" de la libreria general.
 *====================================================================*/
void ERR_Aplicacion(char tipo, char *funcionalidad, char *funcApl,
		    char *formato, ...)
{
        va_list argum;
	char textoErr[1024];
 
va_start(argum, formato);
vsprintf(textoErr, formato, argum);
ERH_NoFatal(textoErr);
va_end(argum);
}

/*====================== PROCEDIMIENTO PRIVADO ======================
 * CtrlNumeroErrores
 * -----------------
 * 	Incrementa el numero de errores reportados y controla el
 *	maximo nivel de errores, tal que si se supera el, se provoca
 *	un error fatal.
 *====================================================================*/
static void CtrlNumeroErrores()
{
totalErrores++;
if (totalErrores > MAX_ERRORES)
    ERH_Fatal ("surpassed max number of errors");
}
