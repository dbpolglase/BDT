#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <malloc.h>
#include <memory.h>
#include <string.h>
#include <err.h>
#include <mdi.h>

/*========================= FUNCION PUBLICA =========================
 * MDI__String
 *	Crea un nuevo string segun la muestra
 *
 *    Parametros:
 *	string: string muestra.
 *	fich:	nombre del fichero de la llamada.
 *	nLin:	numero de la linea en el fichero de la llamada.
 *
 *    Devuelve:
 *	el puntero al nuevo string.
 *====================================================================*/
char *MDI__String(char *string, char *fich, int nLin)
{
	char *pNuevo;

/*--------------------------------------------------------------------
 | Si el string es NULL, la copia es tambien NULL
 +--------------------------------------------------------------------*/
if (string == NULL)
    return NULL;

/*--------------------------------------------------------------------
 | Se duplica el string en memoria dinamica.
 +--------------------------------------------------------------------*/
pNuevo = malloc(strlen(string) + 1);
if (pNuevo == NULL)
    ERR_PorTerminal("strdup fail ([%d] %s)", nLin, fich);

strcpy(pNuevo, string);

return pNuevo;
}

/*========================= FUNCION PUBLICA =========================
 * MDI__EncString
 *	Crea un nuevo string como resultado de encadenar otros dos.
 *	Si el primer string es NULL, se crea solo con el segundo.
 *
 *    Parametros:
 *	string1: string primero a encadenar.
 *	string2: string segundo.
 *	fich:	nombre del fichero de la llamada.
 *	nLin:	numero de la linea en el fichero de la llamada.
 *    Devuelve:
 *	
 *====================================================================*/
char *MDI__EncString(char *string1, char *string2, char *fich, int nLin)
{
	char *nuevo;
	int   dimens;

/*--------------------------------------------------------------------
 | Si un string es NULL, solo se considera el otro.
 +--------------------------------------------------------------------*/
if (string1 == NULL)
    return MDI_String(string2);
if (string2 == NULL)
    return MDI_String(string1);

/*--------------------------------------------------------------------
 | Se crea memoria dinamica para los dos y se encadenan.
 +--------------------------------------------------------------------*/
dimens = strlen(string1) + strlen(string2) + 1;
nuevo  = realloc(string1, dimens);
if (nuevo == NULL)
    ERR_PorTerminal("realloc fail %d bytes ([%d] %s)", dimens, nLin, fich);
strcat(nuevo, string2);

return nuevo;
}

/*====================== PROCEDIMIENTO PUBLICO ======================
 * MDI__Buffer
 *	Crea un buffer de memoria inicializado a NUL.
 *
 *    Parametros:
 *	lBuffer: longitud en bytes del buffer.
 *	fich:	nombre del fichero de la llamada.
 *	nLin:	numero de la linea en el fichero de la llamada.
 *====================================================================*/
void *MDI__Buffer(int lBuffer, char *fich, int nLin)
{
	char *pMem;

pMem = malloc(lBuffer);
if (pMem == NULL)
    ERR_PorTerminal("malloc fail %d bytes ([%d] %s)", lBuffer, nLin, fich);
memset(pMem, 0, lBuffer);

return pMem;
}

/*====================== PROCEDIMIENTO PUBLICO ======================
 * MDI__ElemArray
 *	Dado un array, crea un nuevo elemento de este.
 *
 *    Parametros:
 *	pArray: puntero al array a incrementar.
 *	numElem: numero de lementos del array a incrementar
 *	lonElem: longitud en bytes del elemento del array.
 *	fich:	nombre del fichero de la llamada.
 *	nLin:	numero de la linea en el fichero de la llamada.
 *====================================================================*/
void MDI__ElemArray(void **pArray, int *numElem, int lonElem,
		   char *fich, int nLin)
{
/*--------------------------------------------------------------------
 | Si es el primer elemento del array, se crea, si no se redimensiona.
 +--------------------------------------------------------------------*/
(*numElem)++;
if (*numElem == 1)
    *pArray = malloc(lonElem);
else
    *pArray = realloc(*pArray, *numElem * lonElem);
if (*pArray == NULL)
    ERR_PorTerminal("malloc/realloc fail, element of array ([%d] %s)", nLin, fich);
}

/*====================== PROCEDIMIENTO PUBLICO ======================
 * MDI__Fichero
 *	Este procedimiento se debe llamar con las macros:
 *		MDI_FicheroTexto
 *		MDI_FicheroDatos
 *	Lee el contenido de un fichero y lo deja en memoria dinamica.
 *	Si el puntero a la longitus es NULL, se asume que el fichero es
 *	de texto y se le pone a este un NUL adicional, caso contrario se 
 *	toma como un fichero de datos y se actualiza la longitud.
 *
 *    Parametros:
 *	fichDat:   path del fichero a tratar.
 *	pMemoria:  punt. para asignar la memoria.
 *	pLongitud: punt. para asignar la longitud en bytes de la memoria.
 *	fich:	   nombre del fichero de la llamada.
 *	nLin:	   numero de la linea en el fichero de la llamada.
 *====================================================================*/
void MDI__Fichero(char *fichDat, char **pMemoria,
		  int *pLongitud, char *fich, int nLin)
{
        int         fd;
        struct stat buf;
        int         resultLect;
	int	    nBytes;
 
/*--------------------------------------------------------------------
 | Si el fichero es NULL no se lee nada.
 +--------------------------------------------------------------------*/
if (fichDat == NULL)
    ERR_PorTerminal("name of file \"NULL\"");
 
/*--------------------------------------------------------------------
 | Se comprueba que existe el fichero y que se puede leer.
 +--------------------------------------------------------------------*/
if (access(fichDat, F_OK) != 0)
    ERR_PorTerminal("file \"%s\" don't exists", fichDat);
if (access(fichDat, R_OK) != 0)
    ERR_PorTerminal("not read access to file \"%s\" ", fichDat);

/*--------------------------------------------------------------------
 | Se abre el fichero a preprocesar.
 +--------------------------------------------------------------------*/
fd = open(fichDat, O_RDONLY);
if (fd == -1)
    ERR_PorTerminal("can't open file \"%s\" ([%d] %s)", fichDat, nLin, fich);
 
/*--------------------------------------------------------------------
 | Se obtiene la longitud en bytes del fichero a preprocesar.
 +--------------------------------------------------------------------*/
if (fstat(fd, &buf) == -1)
    ERR_PorTerminal("error fstat file \"%s\" ([%d] %s)", fichDat, nLin, fich);
nBytes = buf.st_size;
 
/*--------------------------------------------------------------------
 | Se obtiene memoria dinamica para el fichero a preprocesar:
 |	- Si es de datos, tambien se actualiza la longitud.
 |	- Si es para texto, se pide el un byte de mas para el NUL.
 +--------------------------------------------------------------------*/
if (pLongitud) {
    *pMemoria = MDI_Buffer(nBytes);
    *pLongitud = nBytes;
    }
else {
    *pMemoria = MDI_Buffer(nBytes + 1);
    *(*pMemoria + nBytes) = '\0';
    }
 
/*--------------------------------------------------------------------
 | Se lee el fichero a preprocesar sobre la memoria dinamica.
 +--------------------------------------------------------------------*/
if (nBytes > 0) {
    resultLect = read(fd, *pMemoria, (unsigned)nBytes);
    if (resultLect != nBytes)
	ERR_PorTerminal("can't read file \"%s\" (leidos %d bytes) ([%d] %s)",
    			fichDat, resultLect, nLin, fich);
    }

/*--------------------------------------------------------------------
 | Se cierra el fichero leido.
 +--------------------------------------------------------------------*/
(void) close(fd);
}
