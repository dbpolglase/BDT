#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#include <err.h>

/*====================== PROCEDIMIENTO PUBLICO ======================
 * _MALLOC
 *	Recubre la funcion "malloc" poniendole control de error.
 *
 *    Parametros:
 *	tamano: numero de bytes del buffer.
 *	nomFunc: nombre de la funcion llamante.
 *	txtPara: texto que indica el uso de la memoria.
 *	fich:	nombre del fichero de la llamada.
 *	nLin:	numero de la linea en el fichero de la llamada.
 *====================================================================*/
void *_MALLOC(int tamano, char *nomFunc, char *txtPara, char *fich, int nLin)
{
	void *pMem;

   pMem = malloc(tamano);
   if (pMem == NULL)
    ERR_SistOper( ERR_FATAL,ERR_FUN_GEN, nomFunc, "malloc",
    		"%d bytes para %s ([%d] %s)", tamano, txtPara, nLin, fich);

   memset( pMem, '\0', tamano );


return pMem;
}

/*====================== PROCEDIMIENTO PUBLICO ======================
 * _REALLOC
 *	Recubre la funcion "malloc" poniendole control de error.
 *
 *    Parametros:
 *	punt: muntero a la memoria dinamica a redimensionar.
 *	tamano: numero de bytes del buffer.
 *	nomFunc: nombre de la funcion llamante.
 *	txtPara: texto que indica el uso de la memoria.
 *	fich:	nombre del fichero de la llamada.
 *	nLin:	numero de la linea en el fichero de la llamada.
 *====================================================================*/
void *_REALLOC(char *punt, int tamano, char *nomFunc,
		char *txtPara, char *fich, int nLin)
{
	void *pMem;

   pMem = realloc(punt, tamano);
   if (pMem == NULL)
    ERR_SistOper( ERR_FATAL,ERR_FUN_GEN, nomFunc, "realloc",
    		"%d bytes para %s ([%d] %s)", tamano, txtPara, nLin, fich);

return pMem;
}

/*====================== PROCEDIMIENTO PUBLICO ======================
 * _ELEM_ARRAY
 *	Dado un array, crea un nuevo elemento de este.
 *
 *    Parametros:
 *	pArray: puntero al array a incrementar.
 *	pNumElem: puntero al numero de lementos del array a incrementar
 *	lonElem: longitud en bytes del elemento del array.
 *	nomFunc: nombre de la funcion llamante.
 *	txtPara: texto que indica el uso de la memoria.
 *	fich:	nombre del fichero de la llamada.
 *	nLin:	numero de la linea en el fichero de la llamada.
 *====================================================================*/
void _ELEM_ARRAY(void **pArray, int *pNumElem, int lonElem,
		 char *nomFunc, char *txtPara, char *fich, int nLin)
{
/*--------------------------------------------------------------------
 | Si es el primer elemento del array, se crea, si no se redimensiona.
 +--------------------------------------------------------------------*/
(*pNumElem)++;
if (*pNumElem == 1)
  {
    *pArray = malloc(lonElem);
    memset( *pArray, '\0', lonElem );
  }

else
    *pArray = realloc((void *)*pArray, (*pNumElem * lonElem));
if (*pArray == NULL)
    ERR_SistOper(ERR_FATAL, ERR_FUN_GEN, nomFunc, "malloc/realloc",
    		 "%d elementos de array de %s ([%d] %s)",
		 *pNumElem, txtPara, nLin, fich);
}
