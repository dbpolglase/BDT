%{
#include <stdio.h>
#include <string.h>
#include <tpc.h>
#include <err.h>
#include <mdi.h>
#include <dat.h>
#include <sca.h>

#define	VALOR_PARAMETRO	1
#define VALOR_STRING	2
#define VALOR_CARACTER	3
#define VALOR_NUMERO	4
#define VALOR_LITERAL	5

#define OPLOG_NOT	1
#define	OPLOG_AND	2
#define	OPLOG_OR	3

#define	OPREL_IGUAL	1
#define	OPREL_DISTINTO	2
#define	OPREL_MAYOR	3
#define	OPREL_MAY_IGU	4
#define	OPREL_MENOR	5
#define	OPREL_MEN_IGU	6
#define	OPREL_HAS	7
#define	OPREL_HASNO	8

static char *ExprCompar(char *, int, char *, int, char *);
static char *ExprLogica(char *, int, char *);
static char *TratarNombre(char *);
static char  TratarAmbito(char *);

/*int yydebug = 1;*/
void yyerror(char *s); 
extern int yylineno;
static RefTABLA	 refTabla;
static RefCLAVE  refClave;
static RefSELECC refSelec;

%}

%union {
	char	  *palabra;
	char	  *string;
	int	   numero;
	char	   caracter;
	char	  *expLog;
	TipoCLAVE  tipCla;
	TipoTPC	   tipCam;
	char	  *linCod;
	char	  *literal;
	struct {
		int   ope;
		char *txt;
		} opeRel;
	struct {
		int   tip;
		char *txt;
		} valRel;
	};

%start  BaseDatos

%token TK_BASE
%token TK_IDENTIFICADOR
%token TK_AMBITO
%token TK_TABLA
%token TK_DIMENSION
%token TK_CLAVE_PRINCIPAL
%token TK_CLAVE_UNICA
%token TK_CLAVE_NO_UNICA
%token TK_CAMPOS
%token TK_SELECCION
%token TK_CODIGO_H
%token TK_CODIGO_C
%token TK_CHAR
%token TK_UNS_CHAR
%token TK_SHORT
%token TK_UNS_SHORT
%token TK_INT
%token TK_UNS_INT
%token TK_LONG
%token TK_UNS_LONG
%token TK_FLOAT
%token TK_DOUBLE
%token TK_OR
%token TK_AND
%token TK_IGUAL
%token TK_DISTINTO
%token TK_MAYOR
%token TK_MAYOR_IGUAL
%token TK_MENOR
%token TK_MENOR_IGUAL
%token TK_HAS
%token TK_HASNO
%token TK_CARACTER
%token TK_STRING
%token TK_LITERAL
%token TK_NUMERO
%token TK_PALABRA

%type <string>   NombreBase
%type <palabra>  IdentifBase
%type <caracter> AmbitoBase

%type <palabra> NombreTabla
%type <numero>  DimensTabla

%type <tipCla>  TipoClaveSecund
%type <tipCam>  TipoCampo
%type <numero>  DimensCampo

%type <expLog>  ExprSelOr
%type <expLog>  ExprSelAnd
%type <expLog>  ExprSelParent
%type <expLog>  ExprRelac
%type <palabra> CampoRelac
%type <opeRel>  OperRelac
%type <valRel>  ValorRelac

%type <palabra> Palabra
%type <string>  String
%type <numero>  Numero

%%

/*--------------------------------------------------------------------
 * 		Definicion de la BASE de DATOS
 * OJO: guardar numero de linea para errores
 *--------------------------------------------------------------------*/
BaseDatos	:  NombreBase IdentifBase AmbitoBase Tablas Codigo
						{ DAT_CreBase($1, $2, $3);    }
		;

NombreBase	:  TK_BASE String		{ $$ = TratarNombre($2);      }
		;

IdentifBase	:  TK_IDENTIFICADOR Palabra	{ $$ = $2;		      }
		;

AmbitoBase	:  TK_AMBITO Palabra		{ $$ = TratarAmbito($2);      }
		;

/*--------------------------------------------------------------------
 * 	    Definicion de las TABLAS de la BASE de DATOS
 *--------------------------------------------------------------------*/
Tablas		:  Tablas Tabla
		|  Tabla
		;

Tabla		:  NombreTabla DimensTabla
				{ refTabla= DAT_CreTabla($1, $2);	}
		   CamposTabla ClavesTabla SelecTabla
		;

/*--------------------------------------------------------------------
 *     Definicion del Nombre y Dimension maxima de una TABLA
 *--------------------------------------------------------------------*/
NombreTabla	:  TK_TABLA Palabra		{ $$ = $2;		}
		;

DimensTabla	:  TK_DIMENSION Numero		{ $$ = $2;		}
		;

/*--------------------------------------------------------------------
 * 	     Definicion de los CAMPOS de una TABLA
 *--------------------------------------------------------------------*/

CamposTabla	:  TK_CAMPOS ListaCampos
		;

ListaCampos	:  ListaCampos UnCampo
		|  UnCampo
		;

UnCampo		:  TipoCampo Palabra DimensCampo
				{ DAT_CreCampo(refTabla, $2, $1, $3);	}
		;

TipoCampo	:  TK_CHAR			{ $$ = TPC_CHAR;	}
		|  TK_UNS_CHAR			{ $$ = TPC_UNS_CHAR;	}
		|  TK_SHORT			{ $$ = TPC_SHORT;	}
		|  TK_UNS_SHORT			{ $$ = TPC_UNS_SHORT;	}
		|  TK_INT			{ $$ = TPC_INT;		}
		|  TK_UNS_INT			{ $$ = TPC_UNS_INT;	}
		|  TK_LONG			{ $$ = TPC_LONG;	}
		|  TK_UNS_LONG			{ $$ = TPC_UNS_LONG;	}
		|  TK_DOUBLE			{ $$ = TPC_DOUBLE;	}
		|  TK_FLOAT			{ $$ = TPC_FLOAT;	}
		;

DimensCampo	:  '['  Numero ']'		{ $$ = $2;		}
		|				{ $$ = 1;		}
		;

/*--------------------------------------------------------------------
 *    Definicion de las Claves Principal y Secundaria de una TABLA
 *--------------------------------------------------------------------*/
ClavesTabla	:  ClavePrincipal ClavesSecund
		;

ClavePrincipal	:  TK_CLAVE_PRINCIPAL Palabra
			{ refClave = DAT_CreClave(refTabla, $2, CLV_PRINCIPAL);}
		   '=' ListaCamposClave
		;

ClavesSecund	:  ClavesSecund UnaClaveSecund
		|
		;

UnaClaveSecund	:  TipoClaveSecund Palabra
			{ refClave = DAT_CreClave(refTabla, $2, $1);	}
		   '=' ListaCamposClave
		;

TipoClaveSecund	:  TK_CLAVE_UNICA		{ $$ = CLV_UNICA;	}
		|  TK_CLAVE_NO_UNICA		{ $$ = CLV_NO_UNICA;	}
		;

ListaCamposClave:  ListaCamposClave Palabra
			{ DAT_CreCampoClave(refTabla, refClave, $2);	}
		|  Palabra
			{ DAT_CreCampoClave(refTabla, refClave, $1);	}
		;

/*--------------------------------------------------------------------
 *    Definicion de las SELECIONES de la Tabla
 *--------------------------------------------------------------------*/
SelecTabla	: SelecTabla UnaSeleccion
		|
		;

UnaSeleccion	: TK_SELECCION Palabra
			{ refSelec = DAT_CreSeleccion(refTabla, $2);	}
		  '=' ExprSelOr
		  	{ DAT_CreCondicSelecion(refSelec, $5);		}
		;

ExprSelOr	: ExprSelOr TK_OR ExprSelAnd
				{ $$ = ExprLogica($1, OPLOG_OR, $3);	}
		| ExprSelAnd	{ $$ = $1;				}
		;

ExprSelAnd	: ExprSelAnd TK_AND ExprSelParent
				{ $$ = ExprLogica($1, OPLOG_AND, $3);	}
		| ExprSelParent	{ $$ = $1;				}
		;

ExprSelParent	: '(' ExprSelOr ')' { $$ = $2;			}
		| ExprRelac	    { $$ = $1;			}
		;

ExprRelac	: CampoRelac OperRelac ValorRelac
			{ $$ = ExprCompar($1, $2.ope, $2.txt, $3.tip, $3.txt);}
		;

CampoRelac	: Palabra		{ $$ = $1;		}
		;

OperRelac	: TK_IGUAL	 { $$.ope = OPREL_IGUAL;    $$.txt = "=="; }
		| TK_DISTINTO	 { $$.ope = OPREL_DISTINTO; $$.txt = "=="; }
		| TK_MAYOR	 { $$.ope = OPREL_MAYOR;    $$.txt = ">";  }
		| TK_MAYOR_IGUAL { $$.ope = OPREL_MAY_IGU;  $$.txt = ">="; }
		| TK_MENOR	 { $$.ope = OPREL_MENOR;    $$.txt = "<";  }
		| TK_MENOR_IGUAL { $$.ope = OPREL_MEN_IGU;  $$.txt = "<="; }
		| TK_HAS	 { $$.ope = OPREL_HAS;      $$.txt = "";   }
		| TK_HASNO	 { $$.ope = OPREL_HASNO;    $$.txt = "";   }
		;

ValorRelac	: Palabra
			{ $$.tip = VALOR_PARAMETRO;
			  $$.txt = $1;				}
		| TK_CARACTER
			{ $$.tip = VALOR_CARACTER;
			  $$.txt = yylval.string;		}
		| String
			{ $$.tip = VALOR_STRING;
			  $$.txt = $1;				}
		| TK_NUMERO
			{ $$.tip = VALOR_NUMERO;
			  $$.txt = yylval.string;		}
		| TK_LITERAL
			{ $$.tip = VALOR_LITERAL;
			  $$.txt = yylval.string;		}
		;

/*--------------------------------------------------------------------
 *    Definicion de codigo fuente a incluir (header e implementacion)
 *--------------------------------------------------------------------*/

Codigo		:  Codigo TK_CODIGO_H	{ DAT_Linea_h(yylval.linCod); }
		|  Codigo TK_CODIGO_C	{ DAT_Linea_c(yylval.linCod); }
		|
		;

/*--------------------------------------------------------------------
 *    Definicion de uso general
 *--------------------------------------------------------------------*/
Palabra		:  TK_PALABRA			{ $$ = yylval.palabra;	}
		;

String		:  TK_STRING			{ $$ = yylval.string;	}
		;

Numero		:  TK_NUMERO			{ $$ = yylval.numero;	}
		;
%%

static char *TratarNombre(char *nombre)
{
	int lonNom;
printf("Nombre :%s",nombre);

/*--------------------------------------------------------------------
 * Se eliminan las dos dos caracteres " y " que acotan el string
 *--------------------------------------------------------------------*/
lonNom = strlen(nombre) - 2;
memmove(nombre, nombre + 1, lonNom);
nombre[lonNom] = '\0';
return nombre;
}

static char TratarAmbito(char *texto)
{
if (strcmp(texto, "local") == 0)
    return BASE_LOCAL;
if (strcmp(texto, "localMSB") == 0)
    return BASE_LOCAL_MSB;
if (strcmp(texto, "shared") == 0)
    return BASE_COMPARTIDA;
if (strcmp(texto, "sharedMSB") == 0)
    return BASE_COMPARTIDA_MSB;
if (strcmp(texto, "configuracion") == 0)
    return BASE_CONFIGURACION;

ERR_PorTerminal("%d ambito \"%s\"", yylineno,texto);
return BASE_LOCAL;
}

static char *ExprCompar(char *nomCampo, int tipOpeRel, char *txtOpeRel,
					int tipValor,  char *txtValor)
{
	RefCAMPO refCampo;
	int	 dimCampo;
	TipoTPC	 tipCampo;
	char 	*nomTabla;
	char	 txtCampo[LON_NOM_TABLA+ LON_NOM_CAMPO + 4];
	char	*txtTipoC;
	char	 buffAux[256];

/*--------------------------------------------------------------------
 * Busqueda de los datos del campo: tipo y dimension
 *--------------------------------------------------------------------*/
refCampo = DAT_CampoPorNombre(refTabla, nomCampo);
if (refCampo == NULL) {
    ERR_PorTerminal("%d field \"%s\" not defined on table ", yylineno,nomCampo);
    return "";
    }

tipCampo = DAT_TipoCampo(refCampo);
dimCampo = DAT_DimensCampo(refCampo);

/*--------------------------------------------------------------------
 * los operadores HAS y HASNO solo son aplicables campos tipo CHAR[n]
 *--------------------------------------------------------------------*/
if (((dimCampo == 1) || (tipCampo != TPC_CHAR)) &&
    ((tipOpeRel == OPREL_HAS) || (tipOpeRel == OPREL_HASNO))) {
    ERR_PorTerminal("%d field \"%s\" operator not apply", yylineno,nomCampo);
    return "";
    }

/*--------------------------------------------------------------------
 * Valor PARAMETRO es compatible pues es el mismo tipo del campo
 * Valor LITERAL (supuesto #define) se acepta (no hay informacion)
 * Valor CARACTER y NUMERO son compatible para "C", el rango del tipo
 * lo analizara el compilador en cada caso (en relacion con el campo).
 * Valor STRING solo es compatible con TPC_CHAR[n]
 *--------------------------------------------------------------------*/
if ((tipValor == VALOR_STRING) &&
    ((tipCampo != TPC_CHAR) || (dimCampo == 1))) {
    ERR_PorTerminal("%d value \"%s\" of operator not apply", yylineno,nomCampo);
    return "";
    }

/*--------------------------------------------------------------------
 * Si el valor comparado es PARAMETRO se da de alta con su nombre
 * y su tipo (el modulo "defs" desecha los duplicados por nombre).
 *--------------------------------------------------------------------*/
if (tipValor == VALOR_PARAMETRO) {
    if (dimCampo > 1)
	sprintf(buffAux, "%s *%s", TPC_Nombre(tipCampo), txtValor);
    else
	sprintf(buffAux, "%s %s", TPC_Nombre(tipCampo), txtValor);
    txtTipoC = MDI_String(buffAux);
    DAT_CreParamSelec(refSelec, txtValor, txtTipoC);
    }

nomTabla = DAT_NombreTabla(EN_MAYMIN, refTabla);
nomCampo = DAT_NombreCampo(EN_MINUSC, refCampo);
if (nomTabla == NULL) {
    ERR_PorTerminal("table name NULL");
    return "";
    }
sprintf(txtCampo, "p%s->%s", nomTabla, nomCampo);

if ((tipCampo == TPC_CHAR) && (dimCampo > 1)) {
    switch(tipOpeRel) {
	case OPREL_HAS: 
	    sprintf(buffAux, "strstr(%s, %s)", txtCampo, txtValor);
	    break;
	case OPREL_HASNO: 
	    sprintf(buffAux, "!strstr(%s, %s)", txtCampo, txtValor);
	    break;
	default:
	    sprintf(buffAux, "(strcmp(%s, %s) %s 0)",
			txtCampo, txtValor, txtOpeRel);
	    break;
	}
    }

else if ((tipCampo == TPC_UNS_CHAR) && (dimCampo > 1))
    sprintf(buffAux, "(memcmp(%s, %s, %d) %s 0)",
    		txtCampo, txtValor, dimCampo, txtOpeRel);
else
    sprintf(buffAux, "(%s %s %s)", txtCampo, txtOpeRel, txtValor);
return MDI_String(buffAux);
}


char *ExprLogica(char *operando1, int operLog, char *operando2)
{
	int lonExpr;
	char *txtExpr;

lonExpr = strlen(operando1) + strlen(operando2) + 6;
switch(operLog) {
    case OPLOG_AND:
	txtExpr = MDI_Buffer(lonExpr);
	sprintf(txtExpr, "%s &&\n\t%s", operando1, operando2);
	break;
    case OPLOG_OR:
	txtExpr = MDI_Buffer(lonExpr);
	sprintf(txtExpr, "%s ||\n\t%s", operando1, operando2);
	break;
    }
free(operando1);
if (operando2)
    free(operando2);

return txtExpr;
}

void yyerror(char *txtErr)
{
if (strcmp(txtErr, "syntax error") == 0)
    ERR_PorTerminal("%d syntax error ",yylineno);
else
    ERR_PorTerminal("%d %s",yylineno, txtErr);
}
