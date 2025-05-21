#ifndef __ERR_H
#define __ERR_H
#ifdef __cplusplus
extern "C" {
#endif

#define ERR_FUN_COMU		"Comunicaciones"
#define ERR_FUN_GEN		"Servicios Generales"
#define ERR_FUN_CNF		"Configuracion"

/*--------------------------------------------------------------------
 | ERR_LON_BLK_FUNC: long. maxima del nombre del bloque funcional.
 | ERR_LON_FUNCION:  long. maxima del nombre de funcion.
 | ERR_LON_TEXTO:    long. maxima del texto del error.
 +--------------------------------------------------------------------*/
#define ERR_LON_BLK_FUNC	32
#define ERR_LON_FUNCION         32
#define ERR_LON_TEXTO           150

/*--------------------------------------------------------------------
 | ERR_FATAL:	 identificador de gravedad de error fatal.
 | ERR_NO_FATAL: identificador de gravedad de error no fatal.
 +--------------------------------------------------------------------*/
#define	ERR_FATAL	'F'
#define	ERR_NO_FATAL	'N'

void ERR_Inicializar(char *aplicacion, char *proceso);
void ERR_Finalizar();
void ERR_BloqueFuncional(char *nombre, int umbral);
void ERR_Aplicacion(char gravedad, char *funcionalidad,
		    char *funcApl, char *formato, ...);
void ERR_SistOper(char gravedad, char *funcionalidad,
		  char *funcApl, char *funcSOper, char *formato, ...);

/*========= INI PRIVADO ==========*/
/*--------------------------------------------------------------------
 | REG_ERROR: identificador de registro de error.
 | REG_ALARMA: identificador de registro de alarma.
 +--------------------------------------------------------------------*/
#define REG_ERROR       'E'
#define REG_ALARMA      'A'

#define ERR_ACTI_ALR    'A'
#define ERR_PERM_ALR    'P'
#define ERR_CESE_ALR    'C'

#define TXT_GRAVEDAD(g)		(g == ERR_NO_FATAL)? "no fatal" :	\
				(g == ERR_FATAL)? "fatal" : "**???**"
#define TXT_TIPO_ALARMA(t)      (t == ERR_ACTI_ALR) ? "activacion" :   \
				(t == ERR_PERM_ALR) ? "permanencia" : "cese"

void ERR_PorConsola(char *formato, ...);
void ERR_PorTerminal(char *formato, ...);
int ERR_HayErroresTerminal();

int ERR_IniGestor(char *nomAplic, char *nomProc,
		void (*funcError)(char, char *, char *, char *, int, char *),
		void (*funcAlarma)(char, char *, char *, char *, int, char *));


int ERR_LeerTipo(char *pTipo);
int ERR_LeerError(char *pGravedad, char *pAplicacion, char *pProceso,
		char *pFuncion, int *pFecha, char *pTexto);
int ERR_LeerAlarma(char *pTipo, char *pAplicacion, char *pProceso,
		char *pBlkFuncional, int *pFecha, char *pTexto);

int ERR_Reiniciar();
char * ERR_Estado();
/*========= FIN PRIVADO ==========*/

#ifdef __cplusplus
}
#endif
#endif /* __ERR_H */
