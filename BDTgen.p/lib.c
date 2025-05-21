#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <stdlib.h>
#include <fcntl.h>
#include <string.h>
#include <ctype.h>
#include <tpc.h>
#include <mdi.h>
#include <err.h>
#include <dat.h>
#include <apf.h>
#include <lib.h>

#define FICH_HEADER_LOCAL	"HeaderLocal.str"
#define FICH_FUENTE_LOCAL	"FuenteLocal.str"

#define FICH_HEADER_COMPA	"HeaderCompa.str"
#define FICH_FUENTE_COMPA	"FuenteCompa.str"

#define FICH_FUENTE_CONFI	"FuenteConfi.str"
#define FICH_HEADER_CONFI	"HeaderConfi.str"

#ifdef _LEER_
static char *strHeaderLocal = "";
static char *strFuenteLocal = "";

static char *strHeaderCompa = "";
static char *strFuenteCompa = "";

static char *strHeaderConfi = "";
static char *strFuenteConfi = "";
#else
#include <HeaderLocal.h>
#include <FuenteLocal.h>
#include <HeaderCompa.h>
#include <FuenteCompa.h>
#include <HeaderConfi.h>
#include <FuenteConfi.h>
#endif

#define DIM(A)	(sizeof(A)/sizeof(A[0]))

#define selecCLA_PRI	'p'
#define selecCLA_UNI	'u'
#define selecCLA_MUL	'm'
#define selecCLA_TOD	'*'

#define CODIFICAR	'C'
#define DECODIFICAR	'D'
#define COPIAR		'='
#define COMPARAR	'?'

#define TXT_iniTABLA		"[TABLA>"
#define TXT_finTABLA		"<TABLA]"
#define TXT_iniCAMPO		"[CAMPO>"
#define TXT_finCAMPO		"<CAMPO]"
#define TXT_iniCLAVE		"[CLAVE>"
#define TXT_iniCLVPRIN		"[CLAVE PRINCIPAL>"
#define TXT_iniCLVUNI		"[CLAVE UNICA>"
#define TXT_iniCLVMUL		"[CLAVE NO UNICA>"
#define TXT_finCLAVE		"<CLAVE]"
#define TXT_iniCAMCLV		"[CAMPO CLAVE>"
#define TXT_finCAMCLV		"<CAMPO CLAVE]"
#define TXT_iniSELECCION	"[SELECCION>"
#define TXT_finSELECCION	"<SELECCION]"
#define TXT_iniPRMSELEC		"[PARAMETRO SELECCION>"
#define TXT_finPRMSELEC		"<PARAMETRO SELECCION]"
#define TXT_iniLOCAL		"[BASE LOCAL>"
#define TXT_finLOCAL		"<BASE LOCAL]"
#define TXT_iniCOMPAR		"[BASE COMPARTIDA>"
#define TXT_finCOMPAR		"<BASE COMPARTIDA]"
#define TXT_iniCONTADOR		"[iniCONT>"
#define TXT_incCONTADOR		"[incCONT>"
#define TXT_LONnomBas		"<LON:nomBas>"
#define TXT_LONnomTbl		"<LON:nomTbl>"
#define TXT_LONnomCam		"<LON:nomCam>"
#define TXT_LONnomCla		"<LON:nomCla>"
#define TXT_BASnombre		"<BAS:nombre>"
#define TXT_BASidenti		"[-]"
#define TXT_BASIDENTI		"[+]"
#define TXT_BASnumTbl		"<BAS:numTbl>"
#define TXT_BASbufEle		"<BAS:bufEle>"
#define TXT_TBLnombre		"<TBL:nombre>"
#define TXT_TBLNombre		"<TBL:Nombre>"
#define TXT_TBLNOMBRE		"<TBL:NOMBRE>"
#define TXT_TBLdimMax		"<TBL:dimMax>"
#define TXT_TBLnumCam		"<TBL:numCam>"
#define TXT_TBLnumCla		"<TBL:numCla>"
#define TXT_TBLposEle		"<TBL:posEle>"
#define TXT_TBLlonEle		"<TBL:lonEle>"
#define TXT_CAMnombre		"<CAM:nombre>"
#define TXT_CAMNombre		"<CAM:Nombre>"
#define TXT_CAMNOMBRE		"<CAM:NOMBRE>"
#define TXT_CAMtipo		"<CAM:tipo>"
#define TXT_CAMdimens		"<CAM:dimens>"
#define TXT_CAMtipoC		"<CAM:tipoC>"
#define TXT_CAMposic		"<CAM:posic>"
#define TXT_CAMlongi		"<CAM:longi>"
#define TXT_CAMcodEst		"<CAM:codEst>"
#define TXT_CAMcodCam		"<CAM:codCam>"
#define TXT_CAMdecEst		"<CAM:decEst>"
#define TXT_CAMcopEst		"<CAM:copEst>"
#define TXT_CAMcmpEst		"<CAM:cmpEst>"
#define TXT_CLAnombre		"<CLA:nombre>"
#define TXT_CLANombre		"<CLA:Nombre>"
#define TXT_CLANOMBRE		"<CLA:NOMBRE>"
#define TXT_CLAtipo		"<CLA:tipo>"
#define TXT_CLAnumCCl		"<CLA:numCCl>"
#define TXT_SELnombre		"<SEL:nombre>"
#define TXT_SELNombre		"<SEL:Nombre>"
#define TXT_SELNOMBRE		"<SEL:NOMBRE>"
#define TXT_SELexpres		"<SEL:expres>"
#define TXT_PRSnombre		"<PRS:nombre>"
#define TXT_PRStipoC		"<PRS:tipoC>"
#define TXT_CONTADOR		"<contador>"
#define TXT_BASportable		"<BAS:portable>"

#define TOK_FIN_TXT		1000
#define TOK_iniTABLA		1001
#define TOK_finTABLA		1002
#define TOK_iniCAMPO		1003
#define TOK_finCAMPO		1004
#define TOK_iniCLAVE		1005
#define TOK_iniCLVPRIN		1006
#define TOK_iniCLVUNI		1007
#define TOK_iniCLVMUL		1008
#define TOK_finCLAVE		1009
#define TOK_iniCAMCLV		1010
#define TOK_finCAMCLV		1011
#define TOK_iniSELECCION	1012
#define TOK_finSELECCION	1013
#define TOK_iniPRMSELEC		1014
#define TOK_finPRMSELEC		1015
#define TOK_iniLOCAL		1016
#define TOK_finLOCAL		1017
#define TOK_iniCOMPAR		1018
#define TOK_finCOMPAR		1019
#define TOK_iniCONTADOR		1020
#define TOK_incCONTADOR		1021
#define TOK_LONnomBas		1022
#define TOK_LONnomTbl		1023
#define TOK_LONnomCam		1024
#define TOK_LONnomCla		1025
#define TOK_BASnombre		1026
#define TOK_BASidenti		1027
#define TOK_BASIDENTI		1028
#define TOK_BASnumTbl		1029
#define TOK_BASbufEle		1030
#define TOK_TBLnombre		1031
#define TOK_TBLNombre		1032
#define TOK_TBLNOMBRE		1033
#define TOK_TBLdimMax		1034
#define TOK_TBLnumCam		1035
#define TOK_TBLnumCla		1036
#define TOK_TBLposEle		1037
#define TOK_TBLlonEle		1038
#define TOK_CAMnombre		1039
#define TOK_CAMNombre		1040
#define TOK_CAMNOMBRE		1041
#define TOK_CAMtipo		1042
#define TOK_CAMdimens		1043
#define TOK_CAMtipoC		1044
#define TOK_CAMposic		1045
#define TOK_CAMlongi		1046
#define TOK_CAMcodEst		1047
#define TOK_CAMcodCam		1048
#define TOK_CAMdecEst		1049
#define TOK_CAMcopEst		1050
#define TOK_CAMcmpEst		1051
#define TOK_CLAnombre		1052
#define TOK_CLANombre		1053
#define TOK_CLANOMBRE		1054
#define TOK_CLAtipo		1055
#define TOK_CLAnumCCl		1056
#define TOK_SELnombre		1057
#define TOK_SELNombre		1058
#define TOK_SELNOMBRE		1059
#define TOK_SELexpres		1060
#define TOK_PRSnombre		1061
#define TOK_PRStipoC		1062
#define TOK_CONTADOR		1063
#define TOK_BASportable		1064

/*-------------------------------------------------------------------
 * estructura de para la lista de tokens de la generacion de codigo:
 *	texto: texto asociado al token.
 *	longitud: longitud del texto asociado al token.
 *	token: codigo del token asociado.
 *	stRef: estado de las referencias a los datos a recorrer
 *	    Valores posibles:
 *		A: abierto
 *		-: cerrado
 *		*: no importa.
 *	    Dato asociado a la posicion:
 *		0: TABLA
 *		1: CAMPO
 *		2: CLAVE o CLAVE PRINCIPAL o CLAVE UNICA o CLAVE NO UNICA
 *		3: CAMPO CLAVE
 *		4: SELECCION
 *		5: PARAMETRO SELECCION
 *--------------------------------------------------------------------*/
static struct {
        char *texto;
	int   longitud;
	int   token;
	char  stRef[6];
	int   quitarRet;
	} lstTokens[] = {
    {TXT_iniTABLA,    sizeof(TXT_iniTABLA)-1,    TOK_iniTABLA,    "------", 1},
    {TXT_finTABLA,    sizeof(TXT_finTABLA)-1,    TOK_finTABLA,    "A-----", 1},
    {TXT_iniCAMPO,    sizeof(TXT_iniCAMPO)-1,    TOK_iniCAMPO,    "A-****", 1},
    {TXT_finCAMPO,    sizeof(TXT_finCAMPO)-1,    TOK_finCAMPO,    "AA****", 1},
    {TXT_iniCLAVE,    sizeof(TXT_iniCLAVE)-1,    TOK_iniCLAVE,    "A*--**", 1},
    {TXT_iniCLVPRIN,  sizeof(TXT_iniCLVPRIN)-1,  TOK_iniCLVPRIN,  "A*--**", 1},
    {TXT_iniCLVUNI,   sizeof(TXT_iniCLVUNI)-1,   TOK_iniCLVUNI,   "A*--**", 1},
    {TXT_iniCLVMUL,   sizeof(TXT_iniCLVMUL)-1,   TOK_iniCLVMUL,   "A*--**", 1},
    {TXT_finCLAVE,    sizeof(TXT_finCLAVE)-1,    TOK_finCLAVE,    "A*A-**", 1},
    {TXT_iniCAMCLV,   sizeof(TXT_iniCAMCLV)-1,   TOK_iniCAMCLV,   "A*A-**", 1},
    {TXT_finCAMCLV,   sizeof(TXT_finCAMCLV)-1,   TOK_finCAMCLV,   "A*AA**", 1},
    {TXT_iniSELECCION,sizeof(TXT_iniSELECCION)-1,TOK_iniSELECCION,"A***--", 1},
    {TXT_finSELECCION,sizeof(TXT_finSELECCION)-1,TOK_finSELECCION,"A***A-", 1},
    {TXT_iniPRMSELEC, sizeof(TXT_iniPRMSELEC)-1, TOK_iniPRMSELEC, "A***A-", 1},
    {TXT_finPRMSELEC, sizeof(TXT_finPRMSELEC)-1, TOK_finPRMSELEC, "A***AA", 1},
    {TXT_iniLOCAL,    sizeof(TXT_iniLOCAL)-1,    TOK_iniLOCAL,    "******", 1},
    {TXT_finLOCAL,    sizeof(TXT_finLOCAL)-1,    TOK_finLOCAL,    "******", 1},
    {TXT_iniCOMPAR,   sizeof(TXT_iniCOMPAR)-1,   TOK_iniCOMPAR,   "******", 1},
    {TXT_finCOMPAR,   sizeof(TXT_finCOMPAR)-1,   TOK_finCOMPAR,   "******", 1},
    {TXT_iniCONTADOR, sizeof(TXT_iniCONTADOR)-1, TOK_iniCONTADOR, "******", 1},
    {TXT_incCONTADOR, sizeof(TXT_incCONTADOR)-1, TOK_incCONTADOR, "******", 1},
    {TXT_LONnomBas,   sizeof(TXT_LONnomBas)-1,   TOK_LONnomBas,   "******", 0},
    {TXT_LONnomTbl,   sizeof(TXT_LONnomTbl)-1,   TOK_LONnomTbl,   "******", 0},
    {TXT_LONnomCam,   sizeof(TXT_LONnomCam)-1,   TOK_LONnomCam,   "******", 0},
    {TXT_LONnomCla,   sizeof(TXT_LONnomCla)-1,   TOK_LONnomCla,   "******", 0},
    {TXT_BASnombre,   sizeof(TXT_BASnombre)-1,   TOK_BASnombre,   "******", 0},
    {TXT_BASidenti,   sizeof(TXT_BASidenti)-1,   TOK_BASidenti,   "******", 0},
    {TXT_BASIDENTI,   sizeof(TXT_BASIDENTI)-1,   TOK_BASIDENTI,   "******", 0},
    {TXT_BASnumTbl,   sizeof(TXT_BASnumTbl)-1,   TOK_BASnumTbl,   "******", 0},
    {TXT_BASbufEle,   sizeof(TXT_BASbufEle)-1,   TOK_BASbufEle,   "******", 0},
    {TXT_TBLnombre,   sizeof(TXT_TBLnombre)-1,   TOK_TBLnombre,   "A*****", 0},
    {TXT_TBLNombre,   sizeof(TXT_TBLNombre)-1,   TOK_TBLNombre,   "A*****", 0},
    {TXT_TBLNOMBRE,   sizeof(TXT_TBLNOMBRE)-1,   TOK_TBLNOMBRE,   "A*****", 0},
    {TXT_TBLdimMax,   sizeof(TXT_TBLdimMax)-1,   TOK_TBLdimMax,   "A*****", 0},
    {TXT_TBLnumCam,   sizeof(TXT_TBLnumCam)-1,   TOK_TBLnumCam,   "A*****", 0},
    {TXT_TBLnumCla,   sizeof(TXT_TBLnumCla)-1,   TOK_TBLnumCla,   "A*****", 0},
    {TXT_TBLposEle,   sizeof(TXT_TBLposEle)-1,   TOK_TBLposEle,   "A*****", 0},
    {TXT_TBLlonEle,   sizeof(TXT_TBLlonEle)-1,   TOK_TBLlonEle,   "A*****", 0},
    {TXT_CAMnombre,   sizeof(TXT_CAMnombre)-1,   TOK_CAMnombre,   "AA****", 0},
    {TXT_CAMNombre,   sizeof(TXT_CAMNombre)-1,   TOK_CAMNombre,   "AA****", 0},
    {TXT_CAMNOMBRE,   sizeof(TXT_CAMNOMBRE)-1,   TOK_CAMNOMBRE,   "AA****", 0},
    {TXT_CAMtipo,     sizeof(TXT_CAMtipo)-1,     TOK_CAMtipo,     "AA****", 0},
    {TXT_CAMdimens,   sizeof(TXT_CAMdimens)-1,   TOK_CAMdimens,   "AA****", 0},
    {TXT_CAMtipoC,    sizeof(TXT_CAMtipoC)-1,    TOK_CAMtipoC,    "AA****", 0},
    {TXT_CAMposic,    sizeof(TXT_CAMposic)-1,    TOK_CAMposic,    "AA****", 0},
    {TXT_CAMlongi,    sizeof(TXT_CAMlongi)-1,    TOK_CAMlongi,    "AA****", 0},
    {TXT_CAMcodEst,   sizeof(TXT_CAMcodEst)-1,   TOK_CAMcodEst,   "AA****", 0},
    {TXT_CAMcodCam,   sizeof(TXT_CAMcodCam)-1,   TOK_CAMcodCam,   "AA****", 0},
    {TXT_CAMdecEst,   sizeof(TXT_CAMdecEst)-1,   TOK_CAMdecEst,   "AA****", 0},
    {TXT_CAMcopEst,   sizeof(TXT_CAMcopEst)-1,   TOK_CAMcopEst,   "AA****", 0},
    {TXT_CAMcmpEst,   sizeof(TXT_CAMcmpEst)-1,   TOK_CAMcmpEst,   "AA****", 0},
    {TXT_CLAnombre,   sizeof(TXT_CLAnombre)-1,   TOK_CLAnombre,   "A*A***", 0},
    {TXT_CLANombre,   sizeof(TXT_CLANombre)-1,   TOK_CLANombre,   "A*A***", 0},
    {TXT_CLANOMBRE,   sizeof(TXT_CLANOMBRE)-1,   TOK_CLANOMBRE,   "A*A***", 0},
    {TXT_CLAtipo,     sizeof(TXT_CLAtipo)-1,     TOK_CLAtipo,     "A*A***", 0},
    {TXT_CLAnumCCl,   sizeof(TXT_CLAnumCCl)-1,   TOK_CLAnumCCl,   "A*A***", 0},
    {TXT_SELnombre,   sizeof(TXT_SELnombre)-1,   TOK_SELnombre,   "A***A*", 0},
    {TXT_SELNombre,   sizeof(TXT_SELNombre)-1,   TOK_SELNombre,   "A***A*", 0},
    {TXT_SELNOMBRE,   sizeof(TXT_SELNOMBRE)-1,   TOK_SELNOMBRE,   "A***A*", 0},
    {TXT_SELexpres,   sizeof(TXT_SELexpres)-1,   TOK_SELexpres,   "A***A*", 0},
    {TXT_PRSnombre,   sizeof(TXT_PRSnombre)-1,   TOK_PRSnombre,   "A***AA", 0},
    {TXT_PRStipoC,    sizeof(TXT_PRStipoC)-1,    TOK_PRStipoC,    "A***AA", 0},
    {TXT_CONTADOR,    sizeof(TXT_CONTADOR)-1,    TOK_CONTADOR,    "******", 0},
    {TXT_BASportable, sizeof(TXT_BASportable)-1, TOK_BASportable, "******", 0}
    };

static void IniGeneracion(char *, char *, char *);
static void GenerarFichero();
static void FinGeneracion();

static void IniTabla();
static void FinTabla();
static void IniCampo();
static void FinCampo();
static void IniClave(char);
static void FinClave();
static void IniCampoClave();
static void FinCampoClave();
static void IniSeleccion();
static void FinSeleccion();
static void IniParamSelec();
static void FinParamSelec();
static void DefTipoCampo();
static void FuncionCampo(char, int);
static int  BuffMaxElemento();
static int  SigToken(int);
static void IgnoraCodigo(int);
static void ObtPosicLect(char **, int *);
static void ResPosicLect(char *, int);
static void AnalizaReferencias(char *, char *);
static void ErrorInterno(char *, char *);

static char	 *TBLdesde  = NULL;
static RefTABLA   TBLref    = NULL;
static int	  TBLnLin   = 0;

static char	 *CAMdesde  = NULL;
static RefCAMPO   CAMref    = NULL;
static int	  CAMnLin   = 0;

static char	 *CLAdesde  = NULL;
static RefCLAVE   CLAref    = NULL;
static TipoCLAVE  CLAtipo   = CLV_PRINCIPAL;
static int	  CLAselTot = 1;
static int	  CLAnLin   = 0;

static char	 *CCLdesde  = NULL;
static RefCAMCLV  CCLref    = NULL;
static int	  CCLnLin   = 0;

static char	 *SELdesde  = NULL;
static RefSELECC  SELref    = NULL;
static int	  SELnLin   = 0;

static char	 *PRSdesde  = NULL;
static RefPRMSEL  PRSref    = NULL;
static int	  PRSnLin   = 0;
static int	  contador  = 0;

static char *fichOriginal = "**???**";
static int   nLinFichOrig = 1;
static FILE *fp		  = NULL;
static char *pTexto	  = NULL;

void LIB_GenerarHeader()
{
	char nomHeader[LON_IDE_BASE + 3];

sprintf(nomHeader, "%s.h", DAT_IdentificadorBase(EN_MINUSC));
switch(DAT_AmbitoBase()) {
    case BASE_LOCAL:
    case BASE_LOCAL_MSB:
	IniGeneracion(nomHeader, FICH_HEADER_LOCAL, strHeaderLocal);
	break;
    case BASE_COMPARTIDA:
    case BASE_COMPARTIDA_MSB:
	IniGeneracion(nomHeader,  FICH_HEADER_COMPA, strHeaderCompa);
	break;
    case BASE_CONFIGURACION:
	IniGeneracion(nomHeader,  FICH_HEADER_CONFI, strHeaderConfi);
	break;
    }

fprintf(fp, "#ifndef __%s_H\n", DAT_IdentificadorBase(EN_MAYUSC));
fprintf(fp, "#define __%s_H\n\n", DAT_IdentificadorBase(EN_MAYUSC));
fprintf(fp, "#ifdef __cplusplus\n");
fprintf(fp, "extern \"C\" {\n");
fprintf(fp, "#endif\n\n");

if (DAT_Codigo_h())
    fprintf(fp, "%s\n", DAT_Codigo_h());

GenerarFichero();

fprintf(fp, "\n#ifdef __cplusplus\n");
fprintf(fp, "}\n");
fprintf(fp, "#endif\n");
fprintf(fp, "#endif /* __%s_H */\n", DAT_IdentificadorBase(EN_MAYUSC));

FinGeneracion();
}

void LIB_GenerarFuente()
{
	char nomFuente[LON_IDE_BASE + 3];

sprintf(nomFuente, "%s.c", DAT_IdentificadorBase(EN_MINUSC));
switch(DAT_AmbitoBase()) {
    case BASE_LOCAL:
    case BASE_LOCAL_MSB:
	IniGeneracion(nomFuente,  FICH_FUENTE_LOCAL, strFuenteLocal);
	break;
    case BASE_COMPARTIDA:
    case BASE_COMPARTIDA_MSB:
	IniGeneracion(nomFuente,  FICH_FUENTE_COMPA, strFuenteCompa);
	break;
    case BASE_CONFIGURACION:
	IniGeneracion(nomFuente,  FICH_FUENTE_CONFI, strFuenteConfi);
	break;
    }

if (DAT_Codigo_c())
    fprintf(fp, "%s\n", DAT_Codigo_c());

GenerarFichero();

FinGeneracion();
}

static void IniGeneracion(char *fichGen, char *fichOri, char *txtOri)
{
fp = fopen(fichGen, "w");
if (fp == NULL)
{
    ERR_PorTerminal("fopen(escritura) fail fichero \"%s\"", fichGen);
    exit(1);
}

fichOriginal = fichOri;
pTexto	     = txtOri;
nLinFichOrig = 2;

#ifdef _LEER_
	{

		int	    fd;
		struct stat buf;
		int	    i;
		char path[512];

	sprintf(path, "%s/%s", APF_DirName(fichOri),fichOri);
	if ((fd = open (path, 0)) == -1)
        {
	    ERR_PorTerminal("open fichero \"%s\" fail", path);
            exit(1);
        }

	if (fstat(fd, &buf) == -1)
        {
	    ERR_PorTerminal("fstat fichero \"%s\" fail", path);
            exit(1);
        }

	if ((pTexto = (char *)malloc((unsigned)buf.st_size + 1)) == NULL)
        {
	    ERR_PorTerminal("malloc \"%d\" fail", buf.st_size);
            exit(1);
        }

	i = read (fd, pTexto, (unsigned)buf.st_size);
	if (i != (int)buf.st_size)
        {
	    ERR_PorTerminal("fail read fichero \"%s\" (tiene %d, leo %d)", path,
			(int)buf.st_size, i);
            exit(1);
        }
	close(fd);
	*(pTexto + buf.st_size) = '\0';
	}
#endif
}

static void FinGeneracion()
{
fclose(fp);
}

static void GenerarFichero()
{
	int token;

token = SigToken(1);
while(token != TOK_FIN_TXT) {
    switch(token) {
	case TOK_iniLOCAL:
	    if (DAT_AmbitoBase() != BASE_LOCAL)
		IgnoraCodigo(TOK_finLOCAL);
	    break;
	case TOK_iniCOMPAR:
	    if (DAT_AmbitoBase() != BASE_COMPARTIDA)
		IgnoraCodigo(TOK_finCOMPAR);
	    break;
	case TOK_finLOCAL:
	case TOK_finCOMPAR:
	    break;
	case TOK_iniTABLA:
	    IniTabla();
	    break;
	case TOK_finTABLA:
	    FinTabla();
	    break;
	case TOK_iniCAMPO:
	    IniCampo();
	    break;
	case TOK_finCAMPO:
	    FinCampo();
	    break;
	case TOK_iniCLAVE:
	    IniClave(selecCLA_TOD);
	    break;
	case TOK_iniCLVPRIN:
	    IniClave(selecCLA_PRI);
	    break;
	case TOK_iniCLVUNI:
	    IniClave(selecCLA_UNI);
	    break;
	case TOK_iniCLVMUL:
	    IniClave(selecCLA_MUL);
	    break;
	case TOK_finCLAVE:
	    FinClave();
	    break;
	case TOK_iniCAMCLV:
	    IniCampoClave();
	    break;
	case TOK_finCAMCLV:
	    FinCampoClave();
	    break;
	case TOK_iniSELECCION:
	    IniSeleccion();
	    break;
	case TOK_finSELECCION:
	    FinSeleccion();
	    break;
	case TOK_iniPRMSELEC:
	    IniParamSelec();
	    break;
	case TOK_finPRMSELEC:
	    FinParamSelec();
	    break;

	case TOK_LONnomBas:
	    fprintf(fp, "%d", LON_NOM_BASE);
	    break;
	case TOK_LONnomTbl:
	    fprintf(fp, "%d", LON_NOM_TABLA);
	    break;
	case TOK_LONnomCam:
	    fprintf(fp, "%d", LON_NOM_CAMPO);
	    break;
	case TOK_LONnomCla:
	    fprintf(fp, "%d", LON_NOM_CLAVE);
	    break;
	case TOK_BASnombre:
	    fprintf(fp, "%s",DAT_NombreBase());
	    break;
	case TOK_BASidenti:
	    fprintf(fp, "%s",DAT_IdentificadorBase(EN_MINUSC));
	    break;
	case TOK_BASIDENTI:
	    fprintf(fp, "%s",DAT_IdentificadorBase(EN_MAYUSC));
	    break;
	case TOK_BASnumTbl:
	    fprintf(fp, "%d", DAT_NumeroTablas());
	    break;
	case TOK_BASbufEle:
	    fprintf(fp, "%d", BuffMaxElemento());
	    break;

	case TOK_TBLnombre:
	    fprintf(fp, "%s",DAT_NombreTabla(EN_MINUSC, TBLref));
	    break;
	case TOK_TBLNombre:
	    fprintf(fp, "%s",DAT_NombreTabla(EN_MAYMIN, TBLref));
	    break;
	case TOK_TBLNOMBRE:
	    fprintf(fp, "%s",DAT_NombreTabla(EN_MAYUSC, TBLref));
	    break;
	case TOK_TBLdimMax:
	    fprintf(fp, "%d", DAT_DimMaxTabla(TBLref));
	    break;
	case TOK_TBLposEle:
	    fprintf(fp, "%ldL", (long)DAT_PosicElementos(TBLref));
	    break;
	case TOK_TBLlonEle:
	    fprintf(fp, "%d", DAT_BytesElemento(TBLref));
	    break;
	case TOK_TBLnumCam:
	    fprintf(fp, "%d", DAT_NumeroCampos(TBLref));
	    break;
	case TOK_TBLnumCla:
	    fprintf(fp, "%d", DAT_NumeroClaves(TBLref));
	    break;

	case TOK_CAMnombre:
	    fprintf(fp, "%s",DAT_NombreCampo(EN_MINUSC, CAMref));
	    break;
	case TOK_CAMNombre:
	    fprintf(fp, "%s",DAT_NombreCampo(EN_MAYMIN, CAMref));
	    break;
	case TOK_CAMNOMBRE:
	    fprintf(fp, "%s",DAT_NombreCampo(EN_MAYUSC, CAMref));
	    break;
	case TOK_CAMtipo:
	    fprintf(fp, "'%c'", (char) DAT_TipoCampo(CAMref));
	    break;
	case TOK_CAMdimens:
	    fprintf(fp, "%d", DAT_DimensCampo(CAMref));
	    break;
	case TOK_CAMtipoC:
	    DefTipoCampo();
	    break;
	case TOK_CAMposic:
	    fprintf(fp, "%d", DAT_PosicCampo(CAMref));
	    break;
	case TOK_CAMlongi:
	    fprintf(fp, "%d", DAT_BytesCampo(CAMref));
	    break;
	case TOK_CAMcodEst:
	    FuncionCampo(CODIFICAR, 1);
	    break;
	case TOK_CAMcodCam:
	    FuncionCampo(CODIFICAR, 0);
	    break;
	case TOK_CAMdecEst:
	    FuncionCampo(DECODIFICAR, 1);
	    break;
	case TOK_CAMcopEst:
	    FuncionCampo(COPIAR, 1);
	    break;
	case TOK_CAMcmpEst:
	    FuncionCampo(COMPARAR, 1);
	    break;

	case TOK_CLAnombre:
	    fprintf(fp, "%s",DAT_NombreClave(EN_MINUSC, CLAref));
	    break;
	case TOK_CLANombre:
	    fprintf(fp, "%s",DAT_NombreClave(EN_MAYMIN, CLAref));
	    break;
	case TOK_CLANOMBRE:
	    fprintf(fp, "%s",DAT_NombreClave(EN_MAYUSC, CLAref));
	    break;
	case TOK_CLAtipo:
	    fprintf(fp, "'%c'", (char) DAT_TipoClave(CLAref));
	    break;
	case TOK_CLAnumCCl:
	    fprintf(fp, "%d", DAT_NumeroCamposClave(CLAref));
	    break;

	case TOK_SELnombre:
	    fprintf(fp, "%s",DAT_NombreSeleccion(EN_MINUSC, SELref));
	    break;
	case TOK_SELNombre:
	    fprintf(fp, "%s",DAT_NombreSeleccion(EN_MAYMIN, SELref));
	    break;
	case TOK_SELNOMBRE:
	    fprintf(fp, "%s",DAT_NombreSeleccion(EN_MAYUSC, SELref));
	    break;
	case TOK_SELexpres:
	    fprintf(fp, "%s",DAT_CondicSeleccion(SELref));
	    break;
	case TOK_PRSnombre:
	    fprintf(fp, "%s",DAT_NombreParamSelec(EN_MINUSC, PRSref));
	    break;
	case TOK_PRStipoC:
	    fprintf(fp, "%s",DAT_TipoParamSelec(PRSref));
	    break;

	case TOK_iniCONTADOR:
	    contador = 0;
	    break;
	case TOK_incCONTADOR:
	    contador++;
	    break;
	case TOK_CONTADOR:
	    fprintf(fp, "%d", contador);
	    break;

	case TOK_FIN_TXT:
	    return;
	case TOK_BASportable:
            switch(DAT_AmbitoBase()) {
             case BASE_LOCAL:
             case BASE_COMPARTIDA:
	      fprintf(fp, "0");
	      break;
	    case BASE_LOCAL_MSB:
    	    case BASE_COMPARTIDA_MSB:
            case BASE_CONFIGURACION:
	      fprintf(fp, "1");
	      break;
            }
	    break;

	default:
	    putc(token, fp);
	    break;
	}
    token = SigToken(1);
    }
}

static void IgnoraCodigo(int tokenHasta)
{
	int tk;

do  {
    tk = SigToken(0);
    if (tk == TOK_FIN_TXT) {
	pTexto--;
	return;
	}
    } while(tk != tokenHasta);
}

static void IniTabla()
{
TBLref = DAT_RecTablas();
ObtPosicLect(&TBLdesde, &TBLnLin);
}

static void FinTabla()
{
TBLref = DAT_SigTabla(TBLref);
if (TBLref != NULL)
    ResPosicLect(TBLdesde, TBLnLin);
}

static void IniCampo()
{
CAMref = DAT_RecCampos(TBLref);
ObtPosicLect(&CAMdesde, &CAMnLin);
}

static void FinCampo()
{
CAMref = DAT_SigCampo(CAMref);
if (CAMref != NULL)
    ResPosicLect(CAMdesde, CAMnLin);
}

static void IniClave(char claveSelec)
{
CLAref	  = DAT_RecClaves(TBLref);
CLAselTot = 0;
ObtPosicLect(&CLAdesde, &CLAnLin);
switch(claveSelec) {
    case selecCLA_PRI:
	CLAtipo = CLV_PRINCIPAL;
	break;
    case selecCLA_UNI:
	CLAtipo = CLV_UNICA;
	break;
    case selecCLA_MUL:
	CLAtipo = CLV_NO_UNICA;
	break;
    case selecCLA_TOD:
	CLAselTot = 1;
	return;
    }

while (CLAref != NULL) {
    if (DAT_TipoClave(CLAref) == CLAtipo)
	break;
    CLAref = DAT_SigClave(CLAref);
    }

if (CLAref == NULL)
    IgnoraCodigo(TOK_finCLAVE);
}

static void FinClave()
{
CLAref = DAT_SigClave(CLAref);
if (! CLAselTot) {
    while (CLAref != NULL) {
	if (DAT_TipoClave(CLAref) == CLAtipo)
	    break;
	CLAref = DAT_SigClave(CLAref);
	}
    }

if (CLAref != NULL)
    ResPosicLect(CLAdesde, CLAnLin);
}

static void IniCampoClave()
{
CCLref = DAT_RecCamposClave(CLAref);
CAMref = DAT_RefCampoClave(CCLref);
ObtPosicLect(&CCLdesde, &CCLnLin);
}

static void FinCampoClave()
{
CCLref = DAT_SigCampoClave(CCLref);
if (CCLref != NULL) {
    CAMref = DAT_RefCampoClave(CCLref);
    ResPosicLect(CCLdesde, CCLnLin);
    }
else
    CAMref = NULL;
}

static void IniSeleccion()
{
SELref = DAT_RecSelecciones(TBLref);
if (SELref == NULL)
    IgnoraCodigo(TOK_finSELECCION);
else
    ObtPosicLect(&SELdesde, &SELnLin);
}

static void FinSeleccion()
{
SELref = DAT_SigSeleccion(SELref);
if (SELref != NULL)
    ResPosicLect(SELdesde, SELnLin);
}

static void IniParamSelec()
{
PRSref = DAT_RecParamSelec(SELref);
if (PRSref == NULL)
    IgnoraCodigo(TOK_finPRMSELEC);
else
    ObtPosicLect(&PRSdesde, &PRSnLin);
}

static void FinParamSelec()
{
PRSref = DAT_SigParamSelec(PRSref);
if (PRSref != NULL)
    ResPosicLect(PRSdesde, PRSnLin);
}



static void DefTipoCampo()
{
	char	*nombre;
	TipoTPC  tipo;
	int	 dimens;

nombre = DAT_NombreCampo(EN_MINUSC, CAMref);
tipo   = DAT_TipoCampo(CAMref);
dimens = DAT_DimensCampo(CAMref);
fprintf(fp, "%-14s %s", TPC_Nombre(tipo), nombre);
if (dimens > 1)
    fprintf(fp, "[%d]", dimens);
}

static void FuncionCampo(char accion, int conStruct)
{
	char    *nomTbl;
	char    *nomCam;
	char     nomDat[LON_NOM_TABLA + LON_NOM_CAMPO + 2];
	char     nomDat2[LON_NOM_TABLA + LON_NOM_CAMPO + 4];
	TipoTPC  tipCam;
	int      dimCam;
	int      posCam;

 
nomTbl = DAT_NombreTabla(EN_MAYMIN, TBLref),
nomCam = DAT_NombreCampo(EN_MINUSC, CAMref);
tipCam = DAT_TipoCampo(CAMref);
dimCam = DAT_DimensCampo(CAMref);
posCam = DAT_PosicCampo(CAMref);

if (conStruct) {
    sprintf(nomDat, "p%s->%s", nomTbl, nomCam);
    sprintf(nomDat2, "p%s2->%s", nomTbl, nomCam);
    }
else {
    strcpy(nomDat, nomCam);
    sprintf(nomDat2, "%s2", nomCam);
    }

switch(accion) {
    case CODIFICAR:
	fprintf(fp, "%s(%s%s, %d, elem + %d)", TPC_FuncCodif(tipCam),
		(dimCam > 1) ? "" : "&",
		nomDat, dimCam, posCam);
	break;
    case DECODIFICAR:
	fprintf(fp, "%s(%s%s, %d, elem + %d)", TPC_FuncDecod(tipCam),
		(dimCam > 1) ? "" : "&",
		nomDat, dimCam, posCam);
	break;
/******************************************************************/
/* OJO esto hay que hacerlo en la libreria TPC una vez modificada */
/* hay que decidir si los CHAR[n] se tratan siempre como string   */
/******************************************************************/
    case COPIAR:
	if (dimCam > 1)
	    if (tipCam == TPC_CHAR)
		fprintf(fp, "strcpy(%s, %s)", nomDat, nomCam);
	    else
		fprintf(fp, "memcpy(%s, %s, %d)", nomDat, nomCam, dimCam);
	else
	    fprintf(fp, "%s = %s", nomDat, nomCam);
	break;
    case COMPARAR:
	if (dimCam > 1)
	    if (tipCam == TPC_CHAR)
		fprintf(fp, "strcmp(%s, %s)", nomDat, nomDat2);
	    else
		fprintf(fp, "memcmp(%s, %s, %d)", nomDat, nomDat2, dimCam);
	else
	    fprintf(fp, "CMP_VAL(%s, %s)", nomDat, nomDat2);
	break;
    }
}

static int BuffMaxElemento()
{
	static int bufMaxEle = 0;

if (bufMaxEle == 0) {
	RefTABLA  refTabla;

    refTabla = DAT_RecTablas();
    while(refTabla) {
	if (bufMaxEle < DAT_BytesElemento(refTabla))
	    bufMaxEle = DAT_BytesElemento(refTabla);
	refTabla = DAT_SigTabla(refTabla);
	}
    }

return bufMaxEle;
}

static int SigToken(int analizarRefer)
{
	int i;
	int tk;

if (*pTexto == '\0')
    return TOK_FIN_TXT;

if ((*pTexto == '<') || (*pTexto == '[')) {
    for (i = 0; i < DIM(lstTokens); i++)
	if (strncmp(pTexto, lstTokens[i].texto, lstTokens[i].longitud) == 0) {
	    if (analizarRefer)
		AnalizaReferencias(lstTokens[i].stRef, lstTokens[i].texto);
	    pTexto += lstTokens[i].longitud;
	    if ((lstTokens[i].quitarRet) && (*pTexto == '\n')) {
		pTexto++;
		nLinFichOrig++;
		}
	    return lstTokens[i].token;
            }

    if ((*pTexto == '<') && (*(pTexto + 4) == ':')) {
		char *pFinTk;
		int lonTk;

	pFinTk = strchr(pTexto, '>');
	if (pFinTk == NULL)
	    lonTk = 13;
	else {
	    lonTk = pFinTk - pTexto + 1;
	    lonTk = (lonTk > 13) ? 13 : lonTk;
	    }
	fprintf(stderr, "[%d] %s\t\"%.*s\" posible token erroneo\n",
			nLinFichOrig, fichOriginal, lonTk, pTexto);
	}
    }

tk = (*pTexto &0x00FF);
pTexto++;
if (tk == '\n')
    nLinFichOrig++;

return tk;
}

static void ObtPosicLect(char **pTxt, int *pNLin)
{
*pTxt  = pTexto;
*pNLin = nLinFichOrig;
}

static void ResPosicLect(char *pTxt, int nLin)
{
pTexto = pTxt;
nLinFichOrig = nLin;
}

static void AnalizaReferencias(char *estado, char *txtToken)
{
if ((estado[0] == 'A') && (TBLref == NULL))
    ErrorInterno("bucle de TABLA no abierto", txtToken);

if ((estado[0] == '-') && (TBLref != NULL))
    ErrorInterno("bucle de TABLA abierto", txtToken);

if ((estado[1] == 'A') && (CAMref == NULL))
    ErrorInterno("bucle de CAMPO no abierto", txtToken);

if ((estado[1] == '-') && (CAMref != NULL))
    ErrorInterno("bucle de CAMPO abierto", txtToken);

if ((estado[2] == 'A') && (CLAref == NULL))
    ErrorInterno("bucle de CLAVE no abierto", txtToken);

if ((estado[2] == '-') && (CLAref != NULL))
    ErrorInterno("bucle de CLAVE abierto", txtToken);

if ((estado[3] == 'A') && (CCLref == NULL))
    ErrorInterno("bucle de CAMPO CLAVE no abierto", txtToken);

if ((estado[3] == '-') && (CCLref != NULL))
    ErrorInterno("bucle de CAMPO CLAVE abierto", txtToken);

if ((estado[4] == 'A') && (SELref == NULL))
    ErrorInterno("bucle de SELECCION no abierto", txtToken);

if ((estado[4] == '-') && (SELref != NULL))
    ErrorInterno("bucle de SELECCION abierto", txtToken);

if ((estado[5] == 'A') && (PRSref == NULL))
    ErrorInterno("bucle de PARAMETRO SELECCION no abierto", txtToken);

if ((estado[5] == '-') && (PRSref != NULL))
    ErrorInterno("bucle de PARAMETRO SELECCION abierto", txtToken);
}

static void ErrorInterno(char *txtErr, char *txtToken)
{
ERR_PorTerminal("[%d] %s: %s (token actual \"%s\")",
		nLinFichOrig, fichOriginal, txtErr, txtToken);
}
