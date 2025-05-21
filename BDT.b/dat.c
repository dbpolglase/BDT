#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <tpc.h>
#include <mdi.h>
#include <err.h>
#include <dat.h>

#define TAM_ST_TABLA	(sizeof(ST_TABLA))
#define TAM_ST_CAMPO	(sizeof(ST_CAMPO))
#define TAM_ST_CAMCLV	(sizeof(ST_CAMCLV))
#define TAM_ST_CLAVE	(sizeof(ST_CLAVE))
#define TAM_ST_SELECC	(sizeof(ST_SELECC))
#define TAM_ST_PRMSEL	(sizeof(ST_PRMSEL))

#define TXT_ERROR	"**???**"
#define TXT_AMBITO(amb)	(amb == BASE_LOCAL)? "local" :			\
			(amb == BASE_LOCAL_MSB) ? "local portable" :	\
			(amb == BASE_COMPARTIDA) ? "shared" :	\
			(amb == BASE_COMPARTIDA_MSB) ? "shared portable" :	\
			(amb == BASE_CONFIGURACION)? "configuracion" : "**???**"

typedef struct _campo {
	char	*nombre;
	char	*enMayusc;
	char	*enMinusc;
	char	*enMayMin;
	TipoTPC  tipo;
	int	 dimens;
	int	 orden;
	int	 posic;
	int	 bytes;
	struct _campo *sig;
	} ST_CAMPO;

typedef struct _camcla {
	ST_CAMPO *pCampo;
	char	 *nombre;
	char	 *enMayusc;
	char	 *enMinusc;
	char	 *enMayMin;
	struct _camcla *sig;
	} ST_CAMCLV;

typedef struct _clave {
	char	  *nombre;
	char	  *enMayusc;
	char	  *enMinusc;
	char	  *enMayMin;
	TipoCLAVE  tipo;
	ST_CAMCLV *campo;
	int        nCampos;
	struct _clave *sig;
	} ST_CLAVE;

typedef struct _prmsel {
	char *nombre;
	char *enMayusc;
	char *enMinusc;
	char *enMayMin;
	char *tipoC;
	struct _prmsel *sig;
	} ST_PRMSEL;

typedef struct _selec {
	char	  *nombre;
	char	  *enMayusc;
	char	  *enMinusc;
	char	  *enMayMin;
	char	  *condic;
	ST_PRMSEL *param;
	struct _selec *sig;
	} ST_SELECC;

typedef struct _tabla {
	char	 *nombre;
	char	 *enMayusc;
	char	 *enMinusc;
	char	 *enMayMin;
	int	  dimMax;
	ST_CAMPO *campo;
	int	  nCampos;
	ST_CLAVE *clave;
	int	  nClaves;
	ST_SELECC *selec;
	int	  posElem;
	int	  bytesElem;
	struct _tabla *sig;
	} ST_TABLA;

static void AnaLongitud(char *, int, char *);
static void CrearNombres(char *, char **, char **, char **);

extern int	   yylineno;
static char	  *nomBase;
static char	  *ideBaseEnMayusc;
static char	  *ideBaseEnMinusc;
static char	  *ideBaseEnMayMin;
static char	   ambBase;
static ST_TABLA   *tabla	= NULL;
static int	   numTablas	= 0;
static char	  *codigo_h	= NULL;
static char	  *codigo_c	= NULL;

/*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*/

void DAT_CreBase(char *nombre, char *identif, char ambito)
{
AnaLongitud(nombre, LON_NOM_BASE, "base");
AnaLongitud(identif, LON_IDE_BASE, "identificador de la base");
nomBase = nombre;
CrearNombres(identif, &ideBaseEnMayusc, &ideBaseEnMinusc, &ideBaseEnMayMin);
ambBase = ambito;
if (ambBase == BASE_LOCAL_MSB  || ambBase == BASE_COMPARTIDA_MSB || ambBase == BASE_CONFIGURACION)
  TPC_HazPortable(1);
}

char * DAT_NombreBase()
{
return nomBase;
}

char *DAT_IdentificadorBase(int formato)
{
switch(formato) {
    case EN_MAYUSC:
	return ideBaseEnMayusc;
    case EN_MINUSC:
	return ideBaseEnMinusc;
    case EN_MAYMIN:
	return ideBaseEnMayMin;
    }
return TXT_ERROR;
}

char DAT_AmbitoBase()
{
return ambBase;
}

int DAT_NumeroTablas()
{
return numTablas;
}
/*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*/

RefTABLA DAT_CreTabla(char *nombre, int dimMax)
{
	ST_TABLA  *pNueva;
	ST_TABLA **pEnlace;

AnaLongitud(nombre, LON_NOM_TABLA, "tabla");

pEnlace = &tabla;
while(*pEnlace != NULL) {
    if (strcmp((*pEnlace)->nombre, nombre) == 0)
        ERR_PorTerminal("%d table: \"%s\" duplicate", yylineno,nombre);
    pEnlace = &((*pEnlace)->sig);
    }

pNueva		  = (ST_TABLA *) MDI_Buffer(TAM_ST_TABLA);
pNueva->nombre	  = nombre;
pNueva->dimMax	  = dimMax;
pNueva->campo	  = NULL;
pNueva->nCampos	  = 0;
pNueva->clave	  = NULL;
pNueva->nClaves	  = 0;
pNueva->selec	  = NULL;
pNueva->bytesElem = 1;
pNueva->sig	  = NULL;
*pEnlace	  = pNueva;
CrearNombres(nombre, &pNueva->enMayusc, &pNueva->enMinusc, &pNueva->enMayMin);
numTablas++;

return (RefTABLA) pNueva;
}

void DAT_ActPosicElementos(RefTABLA refTabla, int posicion)
{
((ST_TABLA *)refTabla)->posElem = posicion;
}

int DAT_PosicElementos(RefTABLA refTabla)
{
return ((ST_TABLA *)refTabla)->posElem;
}

RefTABLA DAT_RecTablas(void)
{
return (RefTABLA) tabla;
}

RefTABLA DAT_SigTabla(RefTABLA refTabla)
{
return (RefTABLA) (((ST_TABLA *)refTabla)->sig);
}

char *DAT_NombreTabla(int formato, RefTABLA refTabla)
{
switch(formato) {
    case EN_MAYUSC:
	return ((ST_TABLA *)refTabla)->enMayusc;
    case EN_MINUSC:
	return ((ST_TABLA *)refTabla)->enMinusc;
    case EN_MAYMIN:
	return ((ST_TABLA *)refTabla)->enMayMin;
    }
return TXT_ERROR;
}

int DAT_NumeroCampos(RefTABLA refTabla)
{
return ((ST_TABLA *)refTabla)->nCampos;
}

int DAT_NumeroClaves(RefTABLA refTabla)
{
return ((ST_TABLA *)refTabla)->nClaves;
}

int DAT_DimMaxTabla(RefTABLA refTabla)
{
return ((ST_TABLA *)refTabla)->dimMax;
}

int DAT_BytesElemento(RefTABLA refTabla)
{
return ((ST_TABLA *)refTabla)->bytesElem;
}


/*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*/

void DAT_CreCampo(RefTABLA refTabla, char *nombre, TipoTPC tipo, int dimens)
{
	ST_CAMPO  *pNuevo;
	ST_CAMPO **pEnlace;
	int     bytesCampo;

AnaLongitud(nombre, LON_NOM_CAMPO, "campo");
/*  Lo quitamos a ver q. pasa
if ((dimens > 1) && (tipo != TPC_CHAR) && (tipo != TPC_UNS_CHAR))
    ERR_PorTerminal(" %d the type \"%s\" of field \"%s\" don't admits array",
    		 yylineno,TPC_Nombre(tipo), nombre);
*/

pEnlace = &((ST_TABLA *)refTabla)->campo;
while(*pEnlace != NULL) {
    if (strcmp((*pEnlace)->nombre, nombre) == 0)
        ERR_PorTerminal("%d file: \"%s\" duplicate", yylineno,nombre);
    pEnlace = &((*pEnlace)->sig);
    }

bytesCampo	= TPC_TamanoCodif(tipo, dimens);
pNuevo		= (ST_CAMPO *) MDI_Buffer(TAM_ST_CAMPO);
pNuevo->nombre	= nombre;
pNuevo->tipo	= tipo;
pNuevo->dimens	= dimens;
pNuevo->orden	= ((ST_TABLA *)refTabla)->nCampos;
pNuevo->posic	= ((ST_TABLA *)refTabla)->bytesElem;
pNuevo->bytes	= bytesCampo;
pNuevo->sig	= NULL;
*pEnlace	= pNuevo;
CrearNombres(nombre, &pNuevo->enMayusc, &pNuevo->enMinusc, &pNuevo->enMayMin);
((ST_TABLA *)refTabla)->bytesElem += bytesCampo;
((ST_TABLA *)refTabla)->nCampos++;
}


RefCAMPO  DAT_CampoPorNombre(RefTABLA refTabla, char *nombre)
{
	ST_CAMPO *pCampo;

pCampo = ((ST_TABLA *)refTabla)->campo;
while(pCampo != NULL) {
    if (strcasecmp((pCampo)->nombre, nombre) == 0)
        break;
    pCampo = pCampo->sig;
    }
return (RefCAMPO) pCampo;
}


RefCAMPO DAT_RecCampos(RefTABLA refTabla)
{
return (RefCAMPO) ((ST_TABLA *)refTabla)->campo;
}

RefCAMPO DAT_SigCampo(RefCAMPO refCampo)
{
return (RefCAMPO) ((ST_CAMPO *)refCampo)->sig;
}

char *DAT_NombreCampo(int formato, RefCAMPO refCampo)
{
switch(formato) {
    case EN_MAYUSC:
	return ((ST_CAMPO *)refCampo)->enMayusc;
    case EN_MINUSC:
	return ((ST_CAMPO *)refCampo)->enMinusc;
    case EN_MAYMIN:
	return ((ST_CAMPO *)refCampo)->enMayMin;
    }
return TXT_ERROR;
}

TipoTPC DAT_TipoCampo(RefCAMPO refCampo)
{
return ((ST_CAMPO *)refCampo)->tipo;
}

int DAT_DimensCampo(RefCAMPO refCampo)
{
return ((ST_CAMPO *)refCampo)->dimens;
}

int DAT_OrdenCampo(RefCAMPO refCampo)
{
return ((ST_CAMPO *)refCampo)->orden;
}

int DAT_PosicCampo(RefCAMPO refCampo)
{
return ((ST_CAMPO *)refCampo)->posic;
}

int DAT_BytesCampo(RefCAMPO refCampo)
{
return ((ST_CAMPO *)refCampo)->bytes;
}

/*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*/

RefCLAVE DAT_CreClave(RefTABLA refTabla, char *nombre, TipoCLAVE tipo)
{
	ST_CLAVE *pNueva;
	ST_CLAVE **pEnlace;

AnaLongitud(nombre, LON_NOM_CLAVE, "clave");

pEnlace = &((ST_TABLA *)refTabla)->clave;
while(*pEnlace != NULL) {
    if (strcmp((*pEnlace)->nombre, nombre) == 0)
        ERR_PorTerminal("%d key: \"%s\" name duplicate", yylineno,nombre);
    pEnlace = &((*pEnlace)->sig);
    }

pNueva		= (ST_CLAVE *) MDI_Buffer(TAM_ST_CLAVE);
pNueva->nombre	= nombre;
pNueva->tipo	= tipo;
pNueva->campo	= NULL;
pNueva->nCampos	= 0;
pNueva->sig	= NULL;
*pEnlace	= pNueva;
CrearNombres(nombre, &pNueva->enMayusc, &pNueva->enMinusc, &pNueva->enMayMin);
((ST_TABLA *)refTabla)->nClaves++;

return (RefCLAVE) pNueva;
}

RefCLAVE DAT_RecClaves(RefTABLA refTabla)
{
return (RefCLAVE) ((ST_TABLA *)refTabla)->clave;
}

RefCLAVE DAT_SigClave(RefCLAVE refClave)
{
return (RefCLAVE) ((ST_CLAVE *)refClave)->sig;
}

char *DAT_NombreClave(int formato, RefCLAVE refClave)
{
switch(formato) {
    case EN_MAYUSC:
	return ((ST_CLAVE *)refClave)->enMayusc;
    case EN_MINUSC:
	return ((ST_CLAVE *)refClave)->enMinusc;
    case EN_MAYMIN:
	return ((ST_CLAVE *)refClave)->enMayMin;
    }
return TXT_ERROR;
}

TipoCLAVE DAT_TipoClave(RefCLAVE refClave)
{
return ((ST_CLAVE *)refClave)->tipo;
}

int DAT_NumeroCamposClave(RefCLAVE refClave)
{
return ((ST_CLAVE *)refClave)->nCampos;
}

void DAT_CreCampoClave(RefTABLA refTabla, RefCLAVE refClave, char *nombre)
{
	ST_CAMCLV *pNuevo;
	ST_CAMCLV **pEnlace;
	ST_CAMPO  *pCampo;

pCampo = ((ST_TABLA *)refTabla)->campo;
while (pCampo != NULL) {
    if (strcmp(pCampo->nombre, nombre) == 0)
    	break;
    pCampo = pCampo->sig;
    }

if (pCampo == NULL) {
    ERR_PorTerminal("%d field: \"%s\" not declare int table", yylineno,nombre);
    return;
    }

pEnlace = &((ST_CLAVE *)refClave)->campo;
while(*pEnlace != NULL) {
    if (strcmp((*pEnlace)->nombre, nombre) == 0)
        ERR_PorTerminal( "field: \"%s\" name duplicate", yylineno,nombre);
    pEnlace = &((*pEnlace)->sig);
    }

pNuevo		= (ST_CAMCLV *) MDI_Buffer(TAM_ST_CAMCLV);
pNuevo->pCampo	= pCampo;
pNuevo->nombre	= MDI_String(nombre);
pNuevo->sig	= NULL;
*pEnlace	= pNuevo;
    
((ST_CLAVE *)refClave)->nCampos++;
}

RefCAMCLV DAT_RecCamposClave(RefCLAVE refClave)
{
return (RefCAMCLV) ((ST_CLAVE *)refClave)->campo;
}

RefCAMCLV DAT_SigCampoClave(RefCAMCLV refCamClv)
{
return (RefCAMCLV) ((ST_CAMCLV *)refCamClv)->sig;
}

RefCAMPO DAT_RefCampoClave(RefCAMCLV refCamClv)
{
return (RefCAMPO) ((ST_CAMCLV *)refCamClv)->pCampo;
}

/*====================================================================
 * Funciones de las SELECCION de cada TABLA
 *====================================================================*/
RefSELECC DAT_CreSeleccion(RefTABLA refTabla, char *nombre)
{
	ST_SELECC *pNueva;
	ST_SELECC **pEnlace;

pEnlace = &((ST_TABLA *)refTabla)->selec;
while(*pEnlace != NULL) {
    if (strcmp((*pEnlace)->nombre, nombre) == 0)
        ERR_PorTerminal("%d select: \"%s\" name duplicate", yylineno,nombre);
    pEnlace = &((*pEnlace)->sig);
    }

pNueva		= (ST_SELECC *) MDI_Buffer(TAM_ST_SELECC);
pNueva->nombre	= nombre;
pNueva->condic	= "";
pNueva->param	= NULL;
pNueva->sig	= NULL;
*pEnlace	= pNueva;
CrearNombres(nombre, &pNueva->enMayusc, &pNueva->enMinusc, &pNueva->enMayMin);

return (RefSELECC) pNueva;
}

void DAT_CreCondicSelecion(RefSELECC refSelec, char *condic)
{
((ST_SELECC *)refSelec)->condic = condic;
}
 
RefSELECC DAT_RecSelecciones(RefTABLA refTabla)
{
return (RefSELECC) ((ST_TABLA *)refTabla)->selec;
}

RefSELECC DAT_SigSeleccion(RefSELECC refSelec)
{
return (RefSELECC) ((ST_SELECC *)refSelec)->sig;
}
 
char *DAT_NombreSeleccion(int formato, RefSELECC refSelec)
{
switch(formato) {
    case EN_MAYUSC:
	return ((ST_SELECC *)refSelec)->enMayusc;
    case EN_MINUSC:
	return ((ST_SELECC *)refSelec)->enMinusc;
    case EN_MAYMIN:
	return ((ST_SELECC *)refSelec)->enMayMin;
    }
return TXT_ERROR;
}

char *DAT_CondicSeleccion(RefSELECC refSelec)
{
return ((ST_SELECC *)refSelec)->condic;
}
 
/*====================================================================
 * Funciones de los PARAMETROS de cada SELECCION
 *====================================================================*/
void DAT_CreParamSelec(RefSELECC refSelec, char *nombre, char *tipoC)
{
	ST_PRMSEL *pNuevo;
	ST_PRMSEL **pEnlace;

pEnlace = &((ST_SELECC *)refSelec)->param;
while(*pEnlace != NULL) {
    if (strcmp((*pEnlace)->nombre, nombre) == 0) {
	if (strcmp((*pEnlace)->tipoC, tipoC) == 0)
	    return;
	else
	    ERR_PorTerminal( "parameter: \"%s\" type redefined", yylineno,nombre);
	}
    pEnlace = &((*pEnlace)->sig);
    }

pNuevo		= (ST_PRMSEL*) MDI_Buffer(TAM_ST_PRMSEL);
pNuevo->nombre	= nombre;
pNuevo->tipoC	= tipoC;
pNuevo->sig	= NULL;
*pEnlace	= pNuevo;
CrearNombres(nombre, &pNuevo->enMayusc, &pNuevo->enMinusc, &pNuevo->enMayMin);
}

RefPRMSEL DAT_RecParamSelec(RefSELECC refSelec)
{
return (RefPRMSEL) ((ST_SELECC *)refSelec)->param;
}

RefPRMSEL DAT_SigParamSelec(RefPRMSEL refPrmSel)
{
return (RefPRMSEL) ((ST_PRMSEL *)refPrmSel)->sig;
}

char *DAT_NombreParamSelec(int formato, RefPRMSEL refPrmSel)
{
switch(formato) {
    case EN_MAYUSC:
	return ((ST_PRMSEL *)refPrmSel)->enMayusc;
    case EN_MINUSC:
	return ((ST_PRMSEL *)refPrmSel)->enMinusc;
    case EN_MAYMIN:
	return ((ST_PRMSEL *)refPrmSel)->enMayMin;
    }
return TXT_ERROR;
}

char *DAT_TipoParamSelec(RefPRMSEL refPrmSel)
{
return ((ST_PRMSEL *)refPrmSel)->tipoC;
}






void DAT_Linea_h(char *linea)
{
codigo_h = MDI_EncString(codigo_h, linea);
}

void DAT_Linea_c(char *linea)
{
codigo_c = MDI_EncString(codigo_c, linea);
}

char *DAT_Codigo_h(void)
{
return codigo_h;
}

char *DAT_Codigo_c(void)
{
return codigo_c;
}


void DAT_VisDefiniciones(void)
{
	RefTABLA   refTbl;
	RefCAMPO   refCam;
	RefCLAVE   refCla;
	RefCAMCLV  refCCl;
	RefSELECC  refSel;
	int	   dimens;
	char      *txtTipo;

printf("Base %s \"%s\" (%s)\n", TXT_AMBITO(ambBase), nomBase, ideBaseEnMayusc);
refTbl = DAT_RecTablas();
while (refTbl != NULL) {

    printf("  Table %s (max: %d elementos)\n",
			DAT_NombreTabla(EN_MAYUSC, refTbl),
			DAT_DimMaxTabla(refTbl));
    printf("    Fields:\n");
    refCam = DAT_RecCampos(refTbl);
    while (refCam != NULL) {
	txtTipo = TPC_Nombre(DAT_TipoCampo(refCam));
	dimens	= DAT_DimensCampo(refCam);
	if (dimens > 1)
	    printf("        %s %s[%d]\n",
			txtTipo, DAT_NombreCampo(EN_MAYUSC, refCam), dimens);
	else
	    printf("        %s %s\n",
			txtTipo, DAT_NombreCampo(EN_MAYUSC, refCam));
	refCam = DAT_SigCampo(refCam);
	}

    refCla = DAT_RecClaves(refTbl);
    while (refCla != NULL) {
	switch(DAT_TipoClave(refCla)) {
	    case CLV_PRINCIPAL:
		txtTipo = "First";
		break;
	    case CLV_UNICA:
		txtTipo = "Uniq";
		break;
	    case CLV_NO_UNICA:
		txtTipo = "not uniq";
		break;
	    default:
		txtTipo = "?tipo?";
		break;
	    }
	printf("    Key (%s) %s =",
		txtTipo, DAT_NombreClave(EN_MAYUSC, refCla));
	refCCl = DAT_RecCamposClave(refCla);
	while (refCCl != NULL) {	/* OJO uso del puntero */
	    printf(" %s", ((ST_CAMCLV *)refCCl)->nombre);
	    refCCl = DAT_SigCampoClave(refCCl);
	    }
	printf("\n");
	refCla = DAT_SigClave(refCla);
	}

    refSel = DAT_RecSelecciones(refTbl);
    while (refSel != NULL) {
	printf("    Select... %s\n",
		DAT_NombreSeleccion(EN_MAYUSC, refSel));
	refSel = DAT_SigSeleccion(refSel);
	}
    refTbl = DAT_SigTabla(refTbl);
    }
}

static void AnaLongitud(char *texto, int longitud, char *nombre)
{
if (strlen(texto) > longitud)
    ERR_PorTerminal("%d %s: \"%s\" (max %d caract.)",
    			yylineno,nombre, texto, longitud);
}

static void CrearNombres(char *nombre, char **pMayusc,
                         char **pMinusc, char **pMayMin)
{
        char *pAux;
 
*pMayusc = MDI_String(nombre);
pAux = *pMayusc;
while(*pAux) {
    *pAux = toupper(*pAux);
    pAux++;
}
 
*pMinusc = MDI_String(nombre);
pAux = *pMinusc;
while(*pAux) {
    *pAux = tolower(*pAux);
    pAux ++;
}
 
*pMayMin = MDI_String(*pMinusc);
**pMayMin = toupper(**pMayMin);
}
