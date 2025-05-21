#ifndef __DAT_H
#define __DAT_H
#ifdef __cplusplus
extern "C" {
#endif

#define LON_NOM_BASE		32
#define LON_IDE_BASE		6
#define LON_NOM_TABLA		16
#define LON_NOM_CAMPO		20
#define LON_NOM_CLAVE		16

#define BASE_LOCAL		'l'
#define BASE_LOCAL_MSB		'm'
#define BASE_COMPARTIDA		'c'
#define BASE_COMPARTIDA_MSB	'd'
#define BASE_CONFIGURACION	'C'

#define EN_MAYUSC		1
#define EN_MINUSC		2
#define EN_MAYMIN		3

#ifdef __unix__
typedef struct ST_TABLA	  *RefTABLA;
typedef struct ST_CAMPO   *RefCAMPO;
typedef struct ST_CLAVE   *RefCLAVE;
typedef struct ST_CAMCLV  *RefCAMCLV;
typedef struct ST_SELECC  *RefSELECC;
typedef struct ST_PRMSEL  *RefPRMSEL;
#else
typedef void  *RefTABLA;
typedef void  *RefCAMPO;
typedef void  *RefCLAVE;
typedef void  *RefCAMCLV;
typedef void  *RefSELECC;
typedef void  *RefPRMSEL;
#endif

typedef enum {	CLV_PRINCIPAL = 'P',
		CLV_UNICA     = 'U',
		CLV_NO_UNICA  = 'M'
		} TipoCLAVE; /* OJO: UNICA/NO UNICA */

/*====================================================================
 * Funciones de la BASE
 *====================================================================*/
void	   DAT_CreBase(char *nombre, char *identif, char ambito);
char	  *DAT_NombreBase();
char	  *DAT_IdentificadorBase(int formato);
char	   DAT_AmbitoBase();
int	   DAT_NumeroTablas();

void	   DAT_VisDefiniciones(void);

/*====================================================================
 * Funciones de las TABLAS de la BASE
 *====================================================================*/
RefTABLA   DAT_CreTabla(char *nombre, int dimMax);
void	   DAT_ActPosicElementos(RefTABLA refTabla, int posicion);

RefTABLA   DAT_RecTablas(void);
RefTABLA   DAT_SigTabla(RefTABLA refTabla);

char      *DAT_NombreTabla(int formato, RefTABLA refTabla);
int	   DAT_DimMaxTabla(RefTABLA refTabla);
int	   DAT_NumeroCampos(RefTABLA refTabla);
int	   DAT_NumeroClaves(RefTABLA refTabla);

int	   DAT_PosicElementos(RefTABLA refTabla);
int	   DAT_BytesElemento(RefTABLA refTabla);

/*====================================================================
 * Funciones de los CAMPOS de cada TABLA
 *====================================================================*/
void	   DAT_CreCampo(RefTABLA refTabla, char *nombre,
			TipoTPC tipo, int dimens);

RefCAMPO   DAT_CampoPorNombre(RefTABLA refTabla, char *nombre);
RefCAMPO   DAT_RecCampos(RefTABLA refTabla);
RefCAMPO   DAT_SigCampo(RefCAMPO refCampo);

char	  *DAT_NombreCampo(int formato, RefCAMPO refCampo);
TipoTPC	   DAT_TipoCampo(RefCAMPO refCampo);
int	   DAT_DimensCampo(RefCAMPO refCampo);

int	   DAT_OrdenCampo(RefCAMPO refCampo);
int        DAT_BytesCampo(RefCAMPO refCampo);
int        DAT_PosicCampo(RefCAMPO refCampo);

/*====================================================================
 * Funciones de las CLAVES de cada TABLA
 *====================================================================*/
RefCLAVE   DAT_CreClave(RefTABLA refTabla, char *nombre, TipoCLAVE tipo);

RefCLAVE   DAT_RecClaves(RefTABLA refTabla);
RefCLAVE   DAT_SigClave(RefCLAVE refClave);

char	  *DAT_NombreClave(int formato, RefCLAVE refClave);
TipoCLAVE  DAT_TipoClave(RefCLAVE refClave);
int	   DAT_NumeroCamposClave(RefCLAVE refClave);

/*====================================================================
 * Funciones de los CAMPOS de cada CLAVE
 *====================================================================*/
void	   DAT_CreCampoClave(RefTABLA refTabla, RefCLAVE refClave,
			  char *nombre);

RefCAMCLV  DAT_RecCamposClave(RefCLAVE refClave);
RefCAMCLV  DAT_SigCampoClave(RefCAMCLV refCamClv);
RefCAMPO   DAT_RefCampoClave(RefCAMCLV refCamClv);

/*====================================================================
 * Funciones de las SELECCION de cada TABLA
 *====================================================================*/
RefSELECC  DAT_CreSeleccion(RefTABLA refTabla, char *nombre);
void       DAT_CreCondicSelecion(RefSELECC refSelec, char *condic);

RefSELECC  DAT_RecSelecciones(RefTABLA refTabla);
RefSELECC  DAT_SigSeleccion(RefSELECC refSelec);

char	  *DAT_NombreSeleccion(int formato, RefSELECC refSelec);
char	  *DAT_CondicSeleccion(RefSELECC refSele);

/*====================================================================
 * Funciones de los PARAMETROS de cada SELECCION
 *====================================================================*/
void	   DAT_CreParamSelec(RefSELECC refSelec, char *nombre, char *tipoC);

RefPRMSEL  DAT_RecParamSelec(RefSELECC refSelec);
RefPRMSEL  DAT_SigParamSelec(RefPRMSEL refPrmSel);
char	  *DAT_NombreParamSelec(int formato, RefPRMSEL refPrmSel);
char	  *DAT_TipoParamSelec(RefPRMSEL refPrmSel);

/*====================================================================
 * Funciones de codigo header e implementacion
 *====================================================================*/
void	  DAT_Linea_h(char *linea);
void	  DAT_Linea_c(char *linea);
char	 *DAT_Codigo_h(void);
char	 *DAT_Codigo_c(void);

#ifdef __cplusplus
}
#endif
#endif /* __DAT_H */
