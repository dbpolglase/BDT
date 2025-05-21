#include <assert.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <fcntl.h>
#include <string.h>
#include <tpc.h>
#include <mdi.h>
#include <err.h>
#include <dat.h>
#include <fim.h>

#define LON_DAT_BASE	(LON_NOM_BASE + LON_IDE_BASE		\
			+ TPC_TAM_COD_CHAR + TPC_TAM_COD_INT)
#define LON_DAT_TABLA	(LON_NOM_TABLA + (TPC_TAM_COD_INT * 3)	\
			+ TPC_TAM_COD_LONG)
#define LON_DAT_CAMPO	(LON_NOM_CAMPO + TPC_TAM_COD_CHAR + TPC_TAM_COD_INT)
#define LON_DAT_CLAVE	(LON_NOM_CLAVE + TPC_TAM_COD_CHAR + TPC_TAM_COD_INT)
#define LON_DAT_CAMCLV	(LON_NOM_CAMPO)

static int CrearNombre(char **, int, char *);
static char *pMemBase	= NULL;
static int   lonMemBase	= 0;

/*========================= FUNCION PUBLICA =========================
 * FIM_GenerarBase
 *	Crea el fichero imagen de la base de datos vacio, acorde con la
 *	definicion de tablas y campos.
 *====================================================================*/
void FIM_GenerarBase()
{
	char	 *pCabec;
	char	 *pElems;
	int	  lonCab;
	int	  lonEle;
	int	  i;
	RefTABLA  refTabla;
	RefCAMPO  refCampo;
	RefCLAVE  refClave;
	RefCAMCLV refCamClv;
	char	  caracter;
	int	  entero;
	char nomImagen[LON_IDE_BASE + 9];

/*--------------------------------------------------------------------
 * Dimension de la cabecera del fichero: descripcion de la base.
 *--------------------------------------------------------------------*/
lonCab	= LON_DAT_BASE;
lonCab += LON_DAT_TABLA * DAT_NumeroTablas();
refTabla = DAT_RecTablas();
while(refTabla) {
    lonCab += LON_DAT_CAMPO * DAT_NumeroCampos(refTabla);
    lonCab += LON_DAT_CLAVE * DAT_NumeroClaves(refTabla);

    refClave = DAT_RecClaves(refTabla);
    while(refClave) {
	lonCab += LON_DAT_CAMCLV * DAT_NumeroCamposClave(refClave);
        refClave = DAT_SigClave(refClave);
	}
    refTabla = DAT_SigTabla(refTabla);
    }

lonEle = 0;
refTabla = DAT_RecTablas();
while(refTabla) {
    DAT_ActPosicElementos(refTabla, (int) (lonCab + lonEle));
    lonEle += DAT_DimMaxTabla(refTabla) * DAT_BytesElemento(refTabla);
    refTabla = DAT_SigTabla(refTabla);
    }

lonMemBase = lonCab + lonEle;

/*--------------------------------------------------------------------
 * Se pide memoria dinamica para la cabecera.
 --------------------------------------------------------------------*/
pMemBase = MDI_Buffer(lonMemBase);
memset(pMemBase, 0, lonMemBase);
pCabec = pMemBase;
pElems = pCabec + lonCab;

/*--------------------------------------------------------------------
 * Se escribe el nombre de la base y el numero de tablas.
 --------------------------------------------------------------------*/
strncpy(pCabec, DAT_NombreBase(), LON_NOM_BASE);
pCabec  += LON_NOM_BASE;
strncpy(pCabec, DAT_IdentificadorBase(EN_MAYUSC), LON_IDE_BASE);
pCabec  += LON_IDE_BASE;
caracter = DAT_AmbitoBase();
pCabec  += TPC_CodChar(&caracter, 1, pCabec);
entero   = DAT_NumeroTablas();
pCabec  += TPC_CodInt(&entero, 1, pCabec);

refTabla = DAT_RecTablas();
while(refTabla) {
    strncpy(pCabec, DAT_NombreTabla(EN_MAYUSC, refTabla), LON_NOM_TABLA);
    pCabec  += LON_NOM_TABLA;

    entero   = DAT_DimMaxTabla(refTabla);
    pCabec  += TPC_CodInt(&entero, 1, pCabec);

    entero   = DAT_NumeroCampos(refTabla);
    pCabec  += TPC_CodInt(&entero, 1, pCabec);

    entero   = DAT_NumeroClaves(refTabla);
    pCabec  += TPC_CodInt(&entero, 1, pCabec);

    entero  = DAT_PosicElementos(refTabla);
    pCabec  += TPC_CodInt(&entero, 1, pCabec);

    refCampo = DAT_RecCampos(refTabla);
    while(refCampo) {
	strncpy(pCabec, DAT_NombreCampo(EN_MAYUSC, refCampo), LON_NOM_CAMPO);
	pCabec += LON_NOM_CAMPO;

	caracter = (char) DAT_TipoCampo(refCampo);
	pCabec  += TPC_CodChar(&caracter, 1, pCabec);

	entero   = DAT_DimensCampo(refCampo);
        pCabec  += TPC_CodInt(&entero, 1, pCabec);

	refCampo = DAT_SigCampo(refCampo);
	}

    refClave = DAT_RecClaves(refTabla);
    while(refClave) {
	strncpy(pCabec, DAT_NombreClave(EN_MAYUSC, refClave), LON_NOM_CLAVE);
	pCabec += LON_NOM_CLAVE;

	caracter = DAT_TipoClave(refClave);
	pCabec  += TPC_CodChar(&caracter, 1, pCabec);

	entero   = DAT_NumeroCamposClave(refClave);
        pCabec  += TPC_CodInt(&entero, 1, pCabec);

	refCamClv = DAT_RecCamposClave(refClave);
	while(refCamClv) {
	    refCampo = DAT_RefCampoClave(refCamClv);
	    strncpy(pCabec, DAT_NombreCampo(EN_MAYUSC,refCampo), LON_NOM_CAMPO);
	    pCabec += LON_NOM_CAMPO;
	    refCamClv = DAT_SigCampoClave(refCamClv);
	    }
	refClave = DAT_SigClave(refClave);
	}

    refTabla = DAT_SigTabla(refTabla);
    }

refTabla = DAT_RecTablas();
while(refTabla) {
    for ( i = 0; i < DAT_DimMaxTabla(refTabla); i++) {
	*pElems = ELEM_LIBRE;
	pElems += DAT_BytesElemento(refTabla);
	}
    refTabla = DAT_SigTabla(refTabla);
    }

/*--------------------------------------------------------------------
 * Se compone el nombre del fichero imagen y se salva en este.
 *--------------------------------------------------------------------*/
sprintf(nomImagen, "%s.ibd", DAT_IdentificadorBase(EN_MAYUSC));

FIM_EscribirBase(nomImagen);
free(pMemBase);
}

/*========================= FUNCION PUBLICA =========================
 * FIM_LeerBase
 *	Lee el fichero imagen de la base de datos y recoge la
 *	definicion de tablas sobre el modulo "dat".
 *
 *    Parametros:
 *	fichero: nombre del fichero imagen de donde leer la base.
 *    Devuelve:
 *	el puntero a memoria dinamica con la base.
 *====================================================================*/
char *FIM_LeerBase(char *fichero)
{
	char	 *pActual;
	RefTABLA  refTabla;
	RefCLAVE  refClave;
	char	 *nomBase;
	char	 *ideBase;
	char	  ambito;
	int	  numTablas;
	char	 *nomTabla;
	int	  dimMaxima;
	int	  numCampos;
	int	  numClaves;
	int	  posElems;
	char	 *nomCampo;
	char	  tipCampo;
	int	  dimCampo;
	char	 *nomClave;
	char	  tipClave;
	int	  numCamClv;
	char	 *nomCamClv;
	int	  tbl, cam, cla, ccl;

MDI_FicheroDatos(fichero, &pMemBase, &lonMemBase);
pActual  = pMemBase;
pActual += CrearNombre(&nomBase, LON_NOM_BASE, pActual);
pActual += CrearNombre(&ideBase, LON_IDE_BASE, pActual);
pActual += TPC_DecChar(&ambito, 1, pActual);
if (ambito == BASE_LOCAL_MSB || ambito == BASE_COMPARTIDA_MSB || ambito==BASE_CONFIGURACION)
{
   TPC_HazPortable(1);
}
pActual += TPC_DecInt(&numTablas, 1, pActual);


DAT_CreBase(nomBase, ideBase, ambito);

for (tbl = 0; tbl < numTablas; tbl++) {
    pActual += CrearNombre(&nomTabla, LON_NOM_TABLA, pActual);
    pActual += TPC_DecInt(&dimMaxima, 1, pActual);
    pActual += TPC_DecInt(&numCampos, 1, pActual);
    pActual += TPC_DecInt(&numClaves, 1, pActual);
    pActual += TPC_DecInt(&posElems, 1, pActual);
    refTabla = DAT_CreTabla(nomTabla, dimMaxima);
    DAT_ActPosicElementos(refTabla, posElems);

    for (cam = 0; cam < numCampos; cam++) {
	pActual += CrearNombre(&nomCampo, LON_NOM_CAMPO, pActual);
	pActual += TPC_DecChar(&tipCampo, 1, pActual);
        pActual += TPC_DecInt(&dimCampo, 1, pActual);
	DAT_CreCampo(refTabla, nomCampo, (TipoTPC)tipCampo, dimCampo);
	}

    for (cla = 0; cla < numClaves; cla++) {
	pActual += CrearNombre(&nomClave, LON_NOM_CLAVE, pActual);
	pActual += TPC_DecChar(&tipClave, 1, pActual);
        pActual += TPC_DecInt(&numCamClv, 1, pActual);
	refClave = DAT_CreClave(refTabla, nomClave, tipClave);

	for (ccl = 0; ccl < numCamClv; ccl++) {
	    pActual += CrearNombre(&nomCamClv, LON_NOM_CAMPO, pActual);
	    DAT_CreCampoClave(refTabla, refClave, nomCamClv);
	    }
	}
    }
return pMemBase;
}

/*========================= FUNCION PUBLICA =========================
 * FIM_ObtenerBase
 *	Obtiene la direccion de memoria en que se ha leido la base.
 *
 *    Devuelve:
 *	el puntero a memoria dinamica con la base.
 *====================================================================*/
char *FIM_ObtenerBase()
{
return pMemBase;
}

/*========================= FUNCION PUBLICA =========================
 * FIM_EscribirBase
 *	Guarda el nuevo contenido de fichero imagen de la base de
 *	datos. Si no existe el fichero, crea uno nuevo.
 *
 *    Parametros:
 *	fichero: nombre del fichero imagen de donde escribir.
 *====================================================================*/
void FIM_EscribirBase(char *fichero)
{
	int fd;

fd = creat(fichero, 0666);
if (fd == -1)
    ERR_SistOper(ERR_FATAL,"BDT","FIM_EscribirBase","creat", "image file \"%s\"", fichero);

if (write(fd, pMemBase, lonMemBase) != lonMemBase)
    ERR_SistOper(ERR_FATAL,"BDT","FIM_EscribirBase","write", "data of the base in the file \"%s\"", fichero);

if (close(fd) == -1)
    ERR_SistOper(ERR_FATAL,"BDT","FIM_EscribirBase","close", "file \"%s\"", fichero);
}

/*========================= FUNCION PRIVADA =========================
 * CrearNombre
 *	Crea un nombre en memoria dinamica con una longitud maxima.
 *
 *    Parametros:
 *	pNomBase:  puntero a dejar el nombre.
 *	lonMaxNom: longitud maxima del nombre (sin contar el NUL).
 *	pBuffer:   buffer de donde sacar el nombre.
 *    Devuelve:
 *	la longitud maxima el nombre.
 *====================================================================*/
static int CrearNombre(char **pNomBase, int lonMaxNom, char *pBuffer)
{
	char *bufMaximo;

/*--------------------------------------------------------------------
 | Se crea un buffer maximo y se copia el texto del nombre mas el NUL.
 +--------------------------------------------------------------------*/
bufMaximo = MDI_Buffer(lonMaxNom + 1);
strncpy(bufMaximo, pBuffer, lonMaxNom);
bufMaximo[lonMaxNom] = '\0';

/*--------------------------------------------------------------------
 | Se crea el nombre con la longitud exacta y se libera el maximo.
 +--------------------------------------------------------------------*/
*pNomBase = MDI_String(bufMaximo);
free(bufMaximo);

return lonMaxNom;
}
