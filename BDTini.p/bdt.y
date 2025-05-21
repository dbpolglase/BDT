%{
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <tpc.h>
#include <err.h>
#include <mdi.h>
#include <dat.h>
#include <fim.h>
#include <sca.h>

/*--------------------------------------------------------------------
 | OPE_NULA: 	 operacion NULA	(no hacer nada).
 | OPE_INSERTAR: operacion INSERTAR un registro en la base de datos.
 | OPE_ELIMINAR: operacion ELIMINAR un registro de la base de datos.
 | OPE_MODIFICAR: operacion MODIFICAR un registro de la base de datos.
 +--------------------------------------------------------------------*/
#define OPE_NULA	'-'
#define OPE_INSERTAR	'I'
#define OPE_ELIMINAR	'E'
#define OPE_MODIFICAR	'M'

/*--------------------------------------------------------------------
 | EST_VACIO: estado de campo de registro VACIO.
 | EST_LLENO: estado de campo de registro LLENO.
 | EST_ERROR: estado de campo de registro ERROR.
 +--------------------------------------------------------------------*/
#define EST_VACIO	'V'
#define EST_LLENO	'L'
#define EST_ERROR	'E'

/*--------------------------------------------------------------------
 | VAL_CARACTER:    valor de tipo CARACTER.
 | VAL_PALABRA:     valor de tipo PALABRA.
 | VAL_STRING:      valor de tipo STRING.
 | VAL_ARRAY:       valor de tipo ARRAY NUMEROS.
 | VAL_NUM_NATURAL: valor de tipo NATURAL (entero positivo).
 | VAL_NUM_ENTERO:  valor de tipo ENTERO (entero negativo).
 | VAL_NUM_REAL:    calor de tipo REAL.
 +--------------------------------------------------------------------*/
#define VAL_CARACTER	'C'
#define	VAL_PALABRA	'P'
#define VAL_STRING	'S'
#define VAL_ARRAY	'A'
#define VAL_NUM_NATURAL	'N'
#define VAL_NUM_ENTERO	'E'
#define VAL_NUM_REAL	'R'

/*====================================================================
 *		DECLARACION DE FUNCIONES PRIVADAS
 *====================================================================*/
static void  IniciarTablas();
static void  NuevaTabla(char *);
static void  NuevoCampo(char *);
static void  Operacion(char *);
static void  IniRegistroActual();
static void  AsignarValorACampo();

static int   AsignarChar(char *, int);
static int   AsignarUnsChar(char *, int);
static int   AsignarBuffer(char *, int,TipoTPC);
static int   ValorEntero(long *, long, long);
static int   ValorNatural(unsigned long *, unsigned long);

static void  RealizarOperacion();
static void  InsertarElemento();
static void  EliminarElemento();
static void  ModificarElemento();
static char *BuscarElemento();
static char *BuscarPorClave(RefCLAVE);
static int   ClaveCompleta(RefCLAVE);

/*--------------------------------------------------------------------
 | refTabla: referencia a la tabla que se esta tratando.
 | refCampo: referencia al campo de la tabla que se esta tratando.
 +--------------------------------------------------------------------*/
static RefTABLA refTabla = NULL;
static RefCAMPO refCampo = NULL;

/*--------------------------------------------------------------------
 | lonEleAct: longitud (en bytes) del elemento de la tabla actual.
 | bufEleAct: buffer del elemento de la tabla actual.
 | numCamAct: numero de campos del elemento de la tabla actual.
 | indCamAct: indice (orden) del campo que se esta tratando (actual).
 | estCamAct: estado de los campos del elemento actual (VACIO/LLENO).
 +--------------------------------------------------------------------*/
static int   lonEleAct = 0;
static char *bufEleAct = NULL;
static int   numCamAct = 0;
static int   indCamAct = 0;
static char *estCamAct = NULL;

/*--------------------------------------------------------------------
 | operacion: operacion a realizar con el registro de valores leido.
 +--------------------------------------------------------------------*/
static char operacion = OPE_NULA;

/*--------------------------------------------------------------------
 | tipoValor: tipo de valor leido y almacenado en:
 |   caracter: valor de tipo caracter leido.
 |   palabra:  valor de tipo palabra leido (acaba en NUL).
 |   string:   valor de tipo string leido.
 |   lonStr:   longitud en bytes del tipo string leido.
 |   natural:  valor de tipo entero positivo leido.
 |   entero:   valor de tipo entero negativo leido.
 |   real:     valor de tipo real leido.
 +--------------------------------------------------------------------*/
static char	      tipoValor	= '\0';
static char	      caracter	= '\0';
static char	     *palabra	= NULL;
static char	     *string	= NULL;
static int	      lonStr	= 0;
static unsigned long  natural	= 0L;
static long	      entero	= 0L;
static double	      real	= 0.0;

int yydebug = 0;
void yyerror(char *s);
extern int yylineno;
%}

%union {
	char	      caracter;
	char	     *palabra;
	struct	{
		char *txt;
		int   lon;
		} str;
	unsigned long natural;
	long	      entero;
	double	      real;
	};

%start  DatosTablas

%token TK_TABLA
%token TK_INICIAR
%token TK_SEPARADOR
%token TK_CARACTER
%token TK_PALABRA
%token TK_STRING
%token TK_ARRAY
%token TK_NUM_NATURAL
%token TK_NUM_ENTERO
%token TK_NUM_REAL

%type <palabra>  Palabra

%%

/*--------------------------------------------------------------------
 | 		Definicion de la ESTRUCTURA DE CARGA
 +--------------------------------------------------------------------*/
DatosTablas	:  DatosTablas Tabla
		|
		;

Tabla		:  Iniciar NombreTabla Registros
		;

Iniciar		:  TK_INICIAR			{ IniciarTablas();	}
		|
		;

NombreTabla	:  TK_TABLA Palabra		{ NuevaTabla($2);	}
		;

Registros	:  Registros Operacion Registro { RealizarOperacion();	}
		|
		;

Operacion	:  Palabra			{ Operacion($1);	}
		;

Registro	:  DatosPorOrden
		|  DatosPorCampos
		;

DatosPorOrden	:  '(' ListaValores ')'
		;

ListaValores	:  ListaValores ',' ValorCampo	{ AsignarValorACampo();	}
		|  ValorCampo			{ AsignarValorACampo();	}
		;

DatosPorCampos	:  '[' ListaCampos ']'
		;

ListaCampos	:  ListaCampos ',' UnCampo
		|  UnCampo
		;

UnCampo		:  NombreCampo '=' ValorCampo	{ AsignarValorACampo();	}
		;

NombreCampo	:  Palabra			{ NuevoCampo($1);	}
		;

ValorCampo	:  TK_CARACTER		{ tipoValor = VAL_CARACTER;
					  caracter  = yylval.caracter;	}
		|  Palabra		{ tipoValor = VAL_PALABRA;
					  palabra   = $1;
					  lonStr    = strlen($1) + 1;	}
		|  TK_STRING		{ tipoValor = VAL_STRING;
					  string    = yylval.str.txt;
					  lonStr    = yylval.str.lon;	}
		|  TK_ARRAY		{ tipoValor = VAL_ARRAY;
					  string    = yylval.str.txt;
					  lonStr    = yylval.str.lon;	}
		|  TK_NUM_NATURAL	{ tipoValor = VAL_NUM_NATURAL;
					  natural   = yylval.natural;	}
		|  TK_NUM_ENTERO	{ tipoValor = VAL_NUM_ENTERO;
					  entero    = yylval.entero;	}
		|  TK_NUM_REAL		{ tipoValor = VAL_NUM_REAL;
					  real	    = yylval.real;	}
		;

Palabra		:  TK_PALABRA		{ $$ = yylval.palabra;		}
		;
%%

/*====================== PROCEDIMIENTO PRIVADO ======================
 * NuevaTabla
 *	Analiza y anota la tabla sobre la que se va a trabajar.
 *
 *    Parametros:
 *	nombre: nombre de la tabla.
 *====================================================================*/
static void NuevaTabla(char *nombre)
{
/*--------------------------------------------------------------------
 | Se busca el nombre de la tabla entre las de la base.
 +--------------------------------------------------------------------*/
refTabla = DAT_RecTablas();
while (refTabla) {
    if (strcasecmp(nombre, DAT_NombreTabla(EN_MAYUSC, refTabla)) == 0)
	break;
    refTabla = DAT_SigTabla(refTabla);
    }

/*--------------------------------------------------------------------
 | Si existe la tabla, se dimensionan los buffer de recogida de valores:
 |	buffer del elemento y control de relleno (estado) de campos.
 +--------------------------------------------------------------------*/
if (refTabla) {
    if (bufEleAct) {
	free(bufEleAct);
	free(estCamAct);
	}
    lonEleAct = DAT_BytesElemento(refTabla);
    bufEleAct = MDI_Buffer(lonEleAct);
    numCamAct = DAT_NumeroCampos(refTabla);
    estCamAct = MDI_Buffer(numCamAct);
    IniRegistroActual();
    }
else
    ERR_PorTerminal("%d table \"%s\" unknown", yylineno,nombre);

/*--------------------------------------------------------------------
 | Se libera la memoria dinamica del lexema.
 +--------------------------------------------------------------------*/
free(nombre);
}

/*====================== PROCEDIMIENTO PRIVADO ======================
 * NuevoCampo
 *	Analiza y anota el campo sobre el que realizar la asignacion
 *	de valor.
 *
 *    Parametros:
 *	nombre: nombre del campo.
 *====================================================================*/
static void NuevoCampo(char *nombre)
{
/*--------------------------------------------------------------------
 | Si no hay tabla, tampoco hay campo.
 +--------------------------------------------------------------------*/
if (refTabla == NULL) {
    refCampo = NULL;
    return;
    }

/*--------------------------------------------------------------------
 | Se busca el campo entre los definidos para la tabla.
 +--------------------------------------------------------------------*/
indCamAct = 0;
refCampo  = DAT_RecCampos(refTabla);
while (refCampo) {
    if (strcasecmp(nombre, DAT_NombreCampo(EN_MAYUSC, refCampo)) == 0)
	break;
    refCampo = DAT_SigCampo(refCampo);
    indCamAct++;
    }

/*--------------------------------------------------------------------
 | Si no existe el campo, se da un error.
 +--------------------------------------------------------------------*/
if (refCampo == NULL)
    ERR_PorTerminal("%d file \"%s\" of table \"%s\" unknown",
		 yylineno,nombre, DAT_NombreTabla(EN_MAYUSC, refTabla));

/*--------------------------------------------------------------------
 | Se libera la memoria dinamica del lexema.
 +--------------------------------------------------------------------*/
free(nombre);
}


/*====================== PROCEDIMIENTO PRIVADO ======================
 * Operacion
 *	Analiza y anota la operacion definida.
 *
 *    Parametros:
 *	txtOper: 
 *====================================================================*/
static void Operacion(char *txtOper)
{
/*--------------------------------------------------------------------
 | las operaciones admitidas son INSERTAR, ELIMINAR o MODIFICAR.
 +--------------------------------------------------------------------*/
if (strcmp(txtOper, "INSERT") == 0)
    operacion = OPE_INSERTAR;
else if (strcmp(txtOper, "DELETE") == 0)
    operacion = OPE_ELIMINAR;
else if (strcmp(txtOper, "MODIFY") == 0)
    operacion = OPE_MODIFICAR;
else {
    ERR_PorTerminal("%d wrong \"%s\" operation", yylineno,txtOper);
    operacion = OPE_NULA;
    }

/*--------------------------------------------------------------------
 | Se libera la memoria dinamica del lexema.
 +--------------------------------------------------------------------*/
free(txtOper);
}

/*====================== PROCEDIMIENTO PRIVADO ======================
 * IniRegistroActual
 *	
 *
 *====================================================================*/
static void IniRegistroActual()
{
/*--------------------------------------------------------------------
 | Si no hay tabla valida, no se hace nada.
 +--------------------------------------------------------------------*/
if (refTabla == NULL)
    return;

/*--------------------------------------------------------------------
 | Prepara el registro de la tabla actual para cargar los valores de
 | los campos.
 +--------------------------------------------------------------------*/
indCamAct = 0;
refCampo  = DAT_RecCampos(refTabla);
memset(bufEleAct, 0, lonEleAct);
memset(estCamAct, EST_VACIO, numCamAct);
}

/*====================== PROCEDIMIENTO PRIVADO ======================
 * AsignarValorACampo
 *	Realiza la asignacion de campo = valor, previo analisis de
 *	compatibilidad y rango de dichos campo y valor.
 *
 *====================================================================*/
static void AsignarValorACampo()
{
	char		  *pCampo;
	TipoTPC		   tipoCampo;
	int		   result;
	short int	   shortConSigno;
	unsigned short int shortSinSigno;
	int		   intConSigno;
	unsigned int	   intSinSigno;
	long		   longConSigno;
	unsigned long	   longSinSigno;
        float		   flotante;

/*--------------------------------------------------------------------
 | Si no hay tabla valida o campo valido, no se hace nada.
 +--------------------------------------------------------------------*/
if ((refTabla == NULL) || (refCampo == NULL))
    return;

/*--------------------------------------------------------------------
 | Se comprueba que el campo a asignar este vacio.
 +--------------------------------------------------------------------*/
if (estCamAct[indCamAct] != EST_VACIO) {
    ERR_PorTerminal("%d field \"%s\" duplicate in the register",
		yylineno,DAT_NombreCampo(EN_MAYUSC, refCampo));
    return;
    }

/*--------------------------------------------------------------------
 | Se obtienen los datos del campo necesarios para su asignacion.
 +--------------------------------------------------------------------*/
tipoCampo = DAT_TipoCampo(refCampo);
pCampo	  = bufEleAct + DAT_PosicCampo(refCampo);

/*--------------------------------------------------------------------
 | Se llama a la funcion correspondiente al tipo de campo.
 +--------------------------------------------------------------------*/
/*--------------------------------------------------------------------
 | Si la dimension es > 1, es un array de caracteres.
 +--------------------------------------------------------------------*/
if ( DAT_DimensCampo(refCampo) > 1)
    result =  AsignarBuffer(pCampo, DAT_DimensCampo(refCampo), tipoCampo);
else
{
  switch(tipoCampo) {
    case TPC_CHAR:
    	result = AsignarChar(pCampo,1);
	break;
    case TPC_UNS_CHAR:
    	result = AsignarUnsChar(pCampo,1);
	break;

    case TPC_SHORT:
    	result = ValorEntero(&longConSigno, TPC_MIN_SHORT, TPC_MAX_SHORT);
	shortConSigno = longConSigno;
    	TPC_CodShort(&shortConSigno, 1, pCampo);
	break;

    case TPC_UNS_SHORT:
    	result = ValorNatural(&longSinSigno, TPC_MAX_UNS_SHORT);
	shortSinSigno = longSinSigno;
    	TPC_CodUnsShort(&shortSinSigno, 1, pCampo);
	break;

    case TPC_INT:
    	result = ValorEntero(&longConSigno, TPC_MIN_INT, TPC_MAX_INT);
	intConSigno = longConSigno;
    	TPC_CodInt(&intConSigno, 1, pCampo);
	break;
    case TPC_UNS_INT:
    	result = ValorNatural(&longSinSigno, TPC_MAX_UNS_INT);
	intSinSigno = longSinSigno;
    	TPC_CodUnsInt(&intSinSigno, 1, pCampo);
	break;

    case TPC_LONG:
    	result = ValorEntero(&longConSigno, TPC_MIN_LONG, TPC_MAX_LONG);
    	TPC_CodLong(&longConSigno, 1, pCampo);
	break;
    case TPC_UNS_LONG:
    	result = ValorNatural(&longSinSigno, TPC_MAX_UNS_LONG);
    	TPC_CodUnsLong(&longSinSigno, 1, pCampo);
	break;
    case TPC_FLOAT:
    	result = 0;
        flotante = (float) real;
	TPC_CodFloat(&flotante, 1, pCampo);
	break;
    case TPC_DOUBLE:
    	result = 0;
	TPC_CodDouble(&real, 1, pCampo);
	break;
    default:
	break;
    }
   }

/*--------------------------------------------------------------------
 | Se marca el estado del campo tratado segun el resultado.
 +--------------------------------------------------------------------*/
if (result == -1)
    estCamAct[indCamAct] = EST_ERROR;
else
    estCamAct[indCamAct] = EST_LLENO;

/*--------------------------------------------------------------------
 | Se avanzan las referencias al estado actual (para relleno por lista).
 +--------------------------------------------------------------------*/
indCamAct++;
refCampo = DAT_SigCampo(refCampo);
}

/*========================= FUNCION PRIVADA =========================
 * AsignarChar
 *	Realiza el analisis de compatibilidad del valor con el tipo
 *	"char", asi como que el rango del valor este entre los limites
 *	admitidos para este.
 *
 *    Parametros:
 *	pCampo: puntero al campo a codificar el valor.
 *	dimension: dimension del campo.
 *    Devuelve:
 *	-1: si hay error de tipo o rango.
 *	0: si tipo y rango es correcto.
 *====================================================================*/
static int AsignarChar(char *pCampo, int dimension)
{
	long valor;
	int  result;

/*--------------------------------------------------------------------
 | Los valores admitidos son: caracter y numero entre TPC_MIN_CHAR y
 | TPC_MAX_CHAR
 +--------------------------------------------------------------------*/
if (tipoValor == VAL_CARACTER) {
    TPC_CodChar(&caracter, 1, pCampo);
    result = 0;
    }
else {
    result = ValorEntero(&valor, TPC_MIN_CHAR, TPC_MAX_CHAR);
    TPC_CodChar((char *) &valor, 1, pCampo);
    }

return result;
}

/*========================= FUNCION PRIVADA =========================
 * AsignarUnsChar
 *	Realiza el analisis de compatibilidad del valor con el tipo
 *	"unsigned char", asi como que el rango del valor este entre
 *	los limites admitidos para este.
 *
 *    Parametros:
 *	pCampo: puntero al campo a codificar el valor.
 *	dimension: dimension del campo.
 *    Devuelve:
 *	-1: si hay error de tipo o rango.
 *	0: si tipo y rango es correcto.
 *====================================================================*/
static int AsignarUnsChar(char *pCampo, int dimension)
{
	unsigned long valor;
	int result;
 
/*--------------------------------------------------------------------
 | Los valores admitidos son: caracter y numero hasta TPC_MAX_UNS_CHAR
 +--------------------------------------------------------------------*/
if (tipoValor == VAL_CARACTER) {
    TPC_CodChar(&caracter, 1, pCampo);
    result = 0;
    }
else {
    result = ValorNatural(&valor, TPC_MAX_UNS_CHAR);
    TPC_CodUnsChar((unsigned char *) &valor, 1, pCampo);
    }

return result;
}

/*========================= FUNCION PRIVADA =========================
 * AsignarBuffer
 *	Realiza el analisis de compatibilidad de valores con el array
 *	de caracteres (con o sin signo), asi como que la longitud del
 *	valor no sea mayor que la dimension del array (segun parametro).
 *	Si hay error se pone todo el array a NUL.
 *
 *    Parametros:
 *	pCampo: puntero al campo a codificar el valor.
 *	dimension: dimension maxima del array de char (con o son signo).
 *    Devuelve:
 *	-1: si hay error de tipo o dimension.
 *	0: si tipo y dimension es correcto.
 *====================================================================*/
static int AsignarBuffer(char *pCampo, int dimension, TipoTPC tipoCampo)
{
   char *pBuffer;
   int   result;
   short int	   shortConSigno;
   unsigned short int shortSinSigno;
   int		   intConSigno;
   unsigned int	   intSinSigno;
   long		   longConSigno;
   unsigned long   longSinSigno;
   double          doble;
   float           flotante;

   switch(tipoValor) {
       case VAL_PALABRA:
	if (tipoCampo != TPC_CHAR && tipoCampo != TPC_UNS_CHAR)
        {
           ERR_PorTerminal(
    		 "%d value field erroneous \"%s\" (%d)",
		   yylineno,DAT_NombreCampo(EN_MAYUSC, refCampo), dimension);
           return -1;
        }
        pBuffer = MDI_Buffer(dimension);
       	memcpy(pBuffer, palabra, lonStr);
	TPC_CodChar(pBuffer, dimension, pCampo);
	free(palabra);
        free(pBuffer);
       	result = 0;
	break;
       case VAL_STRING:
	if (tipoCampo != TPC_CHAR && tipoCampo != TPC_UNS_CHAR)
        {
           ERR_PorTerminal("%d value field erroneous \"%s\" (%d)",
		   yylineno,DAT_NombreCampo(EN_MAYUSC, refCampo), dimension);
           return -1;
        }
        pBuffer = MDI_Buffer(dimension);
	memcpy(pBuffer, string, lonStr);
	TPC_CodChar(pBuffer, dimension, pCampo);
	free(palabra);
        free(pBuffer);
       	result = 0;
	break;
    case VAL_ARRAY:
         char *token = strtok(string, ",");
       	 result = 0;
         if(token != NULL){
           while(token != NULL){
             switch(tipoCampo) {
               case TPC_SHORT:
	         shortConSigno = atoi(token);
    	         pCampo+=TPC_CodShort(&shortConSigno, 1, pCampo);
	         break;
               case TPC_UNS_SHORT:
	         shortSinSigno = atol(token);
    	         pCampo+=TPC_CodUnsShort(&shortSinSigno, 1, pCampo);
	         break;
               case TPC_INT:
	         intConSigno = atoi(token);
    	         pCampo+=TPC_CodInt(&intConSigno, 1, pCampo);
	         break;
               case TPC_UNS_INT:
	         intSinSigno = atol(token);
    	         pCampo+=TPC_CodUnsInt(&intSinSigno, 1, pCampo);
                 break;
               case TPC_LONG:
	         longConSigno = atol(token);
    	         pCampo+=TPC_CodLong(&longConSigno, 1, pCampo);
	         break;
               case TPC_UNS_LONG:
	         longSinSigno = strtoull(token,NULL,0);
    	         pCampo+=TPC_CodUnsLong(&longSinSigno, 1, pCampo);
	         break;
               case TPC_FLOAT:
	         flotante = atof(token);
    	         pCampo+=TPC_CodFloat(&flotante, 1, pCampo);
	         break;
               case TPC_DOUBLE:
	         doble = atof(token);
    	         pCampo+=TPC_CodDouble(&doble, 1, pCampo);
	         break;
               default:
	         ERR_PorTerminal("%d value asigned to field \"%s\" wrong",
		    yylineno,DAT_NombreCampo(EN_MAYUSC, refCampo));
	         result = -1;
              }
              token = strtok(NULL, ",");
            }
         }
         break;
       default:
	 ERR_PorTerminal("%d value asigned to field \"%s\" wrong",
		    yylineno,DAT_NombreCampo(EN_MAYUSC, refCampo));
	 result = -1;
    }
return result;
}

/*========================= FUNCION PRIVADA =========================
 * ValorEntero
 *	Realiza el analisis de compatibilidad del valor con el tipo
 *	entero, asi como que el rango del valor este entre los limites
 *	definidos para este (segun parametros).
 *	Si hay error el valor asignado es 0.
 *
 *    Parametros:
 *	pValor: puntero para el valor.
 *	limNegativo: limite negativo.
 *	limPositivo: limite positivo.
 *    Devuelve:
 *	-1: si hay error de tipo o rango.
 *	0: si tipo y rango es correcto.
 *====================================================================*/
static int ValorEntero(long *pValor, long limNegativo, long limPositivo)
{
/*--------------------------------------------------------------------
 | Si el valor es un numero natural (positivo), su valor debe estar
 | entre 0 y el limite positivo. Si es entero (negativo) su valor debe
 | estar entre el limite positivo y el negativo.
 | El resto de tipos no son admitidos como compatibles con valor entero.
 +--------------------------------------------------------------------*/
*pValor = 0L;
switch(tipoValor) {
    case VAL_NUM_NATURAL:
	if (natural <= limPositivo) {
	    *pValor = (long) natural;
	    return 0;
	    }
	break;

    case VAL_NUM_ENTERO:
	if ((entero >= limNegativo) && (entero <= limPositivo)) {
	    *pValor = entero;
	    return 0;
	    }
	break;

    default:
	ERR_PorTerminal("%d value asigned to field \"%s\" wrong",
		yylineno,DAT_NombreCampo(EN_MAYUSC, refCampo));
	return -1;
    }

ERR_PorTerminal("%d value asigned to field \"%s\" out of range ",
		yylineno,DAT_NombreCampo(EN_MAYUSC, refCampo));
return -1;
}

/*========================= FUNCION PRIVADA =========================
 * ValorNatural
 *	Realiza el analisis de compatibilidad del valor con el tipo
 *	unsigned, asi como que el rango del valor este en el limite
 *	definido para este (segun parametro).
 *	Si hay error el valor asignado es 0.
 *
 *    Parametros:
 *	pValor: puntero para el valor.
 *	limite: limite del numero.
 *    Devuelve:
 *	-1: si hay error de tipo o rango.
 *	0: si tipo y rango es correcto.
 *====================================================================*/
static int ValorNatural(unsigned long *pValor, unsigned long limite)
{
/*--------------------------------------------------------------------
 | Si el valor es un numero natural (positivo), su valor debe ser menor
 | o igual que el limite maximo.
 | El resto de tipos no son admitidos como compatibles con valor entero.
 +--------------------------------------------------------------------*/
*pValor = 0U;
switch(tipoValor) {
    case VAL_NUM_NATURAL:
	if (natural <= limite) {
	    *pValor = natural;
	    return 0;
	    }
	break;

    default:
	ERR_PorTerminal("%d value asigned to field \"%s\" wrong ",
		yylineno,DAT_NombreCampo(EN_MAYUSC, refCampo));
	return -1;
    }

ERR_PorTerminal("%d value asigned  to field \"%s\" out  of range",
		yylineno,DAT_NombreCampo(EN_MAYUSC, refCampo));
return -1;
}

/*====================== PROCEDIMIENTO PRIVADO ======================
 * RealizarOperacion
 *	Ejecuta la operacion definida sobre el registro leido.
 *====================================================================*/
static void RealizarOperacion()
{
/*--------------------------------------------------------------------
 | Se llama a la funcion que corresponde con la operacion.
 +--------------------------------------------------------------------*/
switch(operacion) {
    case OPE_INSERTAR:
	InsertarElemento();
	break;

    case OPE_ELIMINAR:
	EliminarElemento();
	break;

    case OPE_MODIFICAR:
	ModificarElemento();
	break;
    }

/*--------------------------------------------------------------------
 | Se inicializa todos los datos asociados al registro a tratar.
 +--------------------------------------------------------------------*/
IniRegistroActual();
}

/*====================== PROCEDIMIENTO PRIVADO ======================
 * IniciarTablas
 *	Realiza el inicio de las tablas de la base como vacias.
 *====================================================================*/
static void IniciarTablas()
{
	char *pElemento;
	int   numElem;
	int   lonElem;
	
/*--------------------------------------------------------------------
 | Se recorre cada una de las tablas y se ponen todos sus elementos
 | como LIBRES.
 +--------------------------------------------------------------------*/
refTabla = DAT_RecTablas();
while(refTabla) {
    numElem   = DAT_DimMaxTabla(refTabla);
    pElemento = FIM_ObtenerBase() + DAT_PosicElementos(refTabla);
    lonElem   = DAT_BytesElemento(refTabla);
    while(numElem) {
	*pElemento = ELEM_LIBRE;
	numElem--;
	pElemento += lonElem;
	}
    refTabla = DAT_SigTabla(refTabla);
    }
}

/*====================== PROCEDIMIENTO PRIVADO ======================
 * InsertarElemento
 *	Realiza la operacion de insercion del registro leido.
 *====================================================================*/
static void InsertarElemento()
{
	RefCLAVE  refClave;
	char	 *pElemento;
	int	  numElems;
	int	  lonElem;

/*--------------------------------------------------------------------
 | Primero se comprueba que los valores asociados a claves no unicas
 | (principal y unicas), no existan en otro elemento de la base.
 +--------------------------------------------------------------------*/
refClave = DAT_RecClaves(refTabla);
while (refClave) {
    if (! ClaveCompleta(refClave))
	return;
    if ((DAT_TipoClave(refClave) != CLV_NO_UNICA) &&
	(BuscarPorClave(refClave) != NULL)) {
	ERR_PorTerminal("%d value of key \"%s\" duplicate",
			     yylineno,DAT_NombreClave(EN_MAYUSC, refClave));
	return;
	}
    refClave = DAT_SigClave(refClave);
    }

/*--------------------------------------------------------------------
 | Se recorre la memoria donde estan los elementos (codificados) de la
 | tabla para buscar el primero que este vacio.
 +--------------------------------------------------------------------*/
pElemento = FIM_ObtenerBase() + DAT_PosicElementos(refTabla);
numElems  = DAT_DimMaxTabla(refTabla);
lonElem   = DAT_BytesElemento(refTabla);
while(numElems) {
    if (*pElemento == ELEM_LIBRE)
	break;
    numElems--;
    pElemento += lonElem;
    }

/*--------------------------------------------------------------------
 | Si la tabla esta llena se da un error.
 +--------------------------------------------------------------------*/
if (numElems == 0) {
    ERR_PorTerminal("%d table \"%s\" full (max %d)",
    		 yylineno,DAT_NombreTabla(EN_MAYUSC, refTabla),
		 DAT_DimMaxTabla(refTabla));
    return;
    }

/*--------------------------------------------------------------------
 | Si hay elemento libre, se copia y se marca como ocupado.
 +--------------------------------------------------------------------*/
memcpy(pElemento, bufEleAct, lonEleAct);
*pElemento = ELEM_OCUPADO;
}

/*====================== PROCEDIMIENTO PRIVADO ======================
 * EliminarElemento
 *	Realiza la operacion de eliminacion del registro leido.
 *====================================================================*/
static void EliminarElemento()
{
	char	 *pElemento;

/*--------------------------------------------------------------------
 | Si no existe la clave principal, se da un error.
 | Si existe, se pone a NUL y se marca como libre.
 +--------------------------------------------------------------------*/
pElemento = BuscarElemento(bufEleAct);
if (pElemento == NULL)
{
    return;
}
memset(pElemento, 0, lonEleAct);
*pElemento = ELEM_LIBRE;
}

/*====================== PROCEDIMIENTO PRIVADO ======================
 * ModificarElemento
 *	Realiza la operacion de modificacion segun registro leido.
 *====================================================================*/
static void ModificarElemento()
{
	RefCLAVE  refClave;
	char	 *pPrincipal;
	char	 *pElemento;

/*--------------------------------------------------------------------
 | Si no existe el elemento segun la clave principal, se da un error.
 +--------------------------------------------------------------------*/
pPrincipal = BuscarElemento(bufEleAct);
if (pPrincipal == NULL)
    return;

/*--------------------------------------------------------------------
 | Para las claves unicas, se mira que los campos correspondientes
 | tengan valor asignado. Si existe dicho valor, debe ser en el mismo
 | elemento que la clave principal.
 +--------------------------------------------------------------------*/
refClave = DAT_RecClaves(refTabla);
while (refClave) {
    if (! ClaveCompleta(refClave))
	return;
    if (DAT_TipoClave(refClave) == CLV_UNICA) {
	pElemento = BuscarPorClave(refClave);
	if (pElemento && (pElemento != pPrincipal)) {
	    ERR_PorTerminal("%d value of key \"%s\" exist",
			 yylineno,DAT_NombreClave(EN_MAYUSC, refClave));
	    return;
	    }
	}
    refClave  = DAT_SigClave(refClave);
    }

/*--------------------------------------------------------------------
 | Si el registro es correcto, se copia y se marca como ocupado.
 +--------------------------------------------------------------------*/
memcpy(pPrincipal, bufEleAct, lonEleAct);
*pPrincipal = ELEM_OCUPADO;
}

/*========================= FUNCION PRIVADA =========================
 * BuscarElemento
 *	Busca el elemento leido (codificado) segun la clave principal.
 *
 *    Devuelve:
 *	el puntero al elemento en la base (en memoria), si existe.
 *	NULL: si el elemento no existe.
 *====================================================================*/
static char *BuscarElemento()
{
	RefCLAVE  refClave;
	char	 *pElemento;

/*--------------------------------------------------------------------
 | Se busca la clave principal, tal que el registro (codificado) debe
 | tener los campos de de dicha clave rellenos.
 | Se busca el elemento segun dicha clave.
 +--------------------------------------------------------------------*/
refClave  = DAT_RecClaves(refTabla);
pElemento = NULL;
while (refClave) {
    if (DAT_TipoClave(refClave) == CLV_PRINCIPAL) {
	if (! ClaveCompleta(refClave))
	    return NULL;
	pElemento = BuscarPorClave(refClave);
	break;
	}
    refClave = DAT_SigClave(refClave);
    }

/*--------------------------------------------------------------------
 | Si no existe se da un error.
 +--------------------------------------------------------------------*/
if (pElemento == NULL)
    ERR_PorTerminal("%d element with primary key dont exist ",yylineno);
return pElemento;
}

/*========================= FUNCION PRIVADA =========================
 * BuscarPorClave
 *	Busca el elemento leido (codificado) segun una determinada clave.
 *
 *    Parametros:
 *	refClave: referencia a la clave por la que buscar.
 *    Devuelve:
 *	el puntero al elemento en la base (en memoria), si existe.
 *	NULL: si la clave no existe.
 *====================================================================*/
static char *BuscarPorClave(RefCLAVE refClave)
{
	RefCAMCLV refCamClv;
	RefCAMPO  refCampo;
	char	  *pElemento;
	int	   numElems;
	int	   lonElem;
	int	   posCampo;
	int	   lonCampo;
	int	   igual;

/*--------------------------------------------------------------------
 | Se inicializa el puntero al primer elemento de la tabla.
 | Se obtienen el numero de elementos de la tabla y la longitud de estos.
 +--------------------------------------------------------------------*/
pElemento = FIM_ObtenerBase() + DAT_PosicElementos(refTabla);
numElems  = DAT_DimMaxTabla(refTabla);
lonElem   = DAT_BytesElemento(refTabla);

/*--------------------------------------------------------------------
 | Se recorre la tabla y si cada uno de los campos de la clave son
 | iguales (memcmp) se da por encontrado el elemento, pues se supone
 | que la clave es unica y por tanto tambien el elemento es unico.
 +--------------------------------------------------------------------*/
while(numElems) {
    if (*pElemento == ELEM_OCUPADO) {
	refCamClv = DAT_RecCamposClave(refClave);
	igual	  = 1;
	while (refCamClv) {
	    refCampo = DAT_RefCampoClave(refCamClv);
	    posCampo = DAT_PosicCampo(refCampo);
	    lonCampo = DAT_BytesCampo(refCampo);
	    if (memcmp(pElemento + posCampo, bufEleAct + posCampo, lonCampo)) {
		igual = 0;
		break;
		}

	    refCamClv = DAT_SigCampoClave(refCamClv);
	    }
	if (igual)
	    return pElemento;
        }
    numElems--;
    pElemento += lonElem;
    }

/*--------------------------------------------------------------------
 | Si no hay elemento con los mismos campos de la clave se devuelve NULL.
 +--------------------------------------------------------------------*/
return NULL;
}

/*========================= FUNCION PRIVADA =========================
 * ClaveCompleta
 *	Analiza si los campos de una determinada clave contienen valor.
 *
 *    Parametros:
 *	refClave: referencia de la clave a analizar.
 *    Devuelve:
 *	1: si todos los campos estan rellenos con un valor.
 *	0: si algun campo no contiene valor.
 *====================================================================*/
static int ClaveCompleta(RefCLAVE refClave)
{
	RefCAMCLV refCamClv;
	RefCAMPO  refCampo;
	int	  ordCampo;
	int	  result;

/*--------------------------------------------------------------------
 | Se recorren los campos de la clave tal que:
 |	- El campo en error no se considera relleno. No se emite error
 |	  pues ya se ha dado en la asignacion.
 |	- El campo vacio provoca la emision de un error.
 | Con cualquier campo en que se de alguno de los casos anteriores se
 | considera que la clave no esta rellena (incompleta).
 +--------------------------------------------------------------------*/
refCamClv = DAT_RecCamposClave(refClave);
result	  = 1;
while (refCamClv) {
    refCampo = DAT_RefCampoClave(refCamClv);
    ordCampo = DAT_OrdenCampo(refCampo);
    if (estCamAct[ordCampo] == EST_ERROR)
	result = 0;
    else if (estCamAct[ordCampo] == EST_VACIO) {
	ERR_PorTerminal("%d field \"%s\" of key \"%s\" don't assigned",
			yylineno,DAT_NombreCampo(EN_MAYUSC, refCampo),
			DAT_NombreClave(EN_MAYUSC, refClave));
	result = 0;
	}
    refCamClv = DAT_SigCampoClave(refCamClv);
    }
return result;
}
/*====================== PROCEDIMIENTO PUBLICO ======================
 * yyerror
 *	Captura lo errores de yacc y lo transforma en errores propios.
 *
 *    Parametros:
 *	txtErr: texto de error de yacc.
 *====================================================================*/
void yyerror(char *txtErr)
{
/*--------------------------------------------------------------------
 | Si el texto es "syntax error", se traduce.
 +--------------------------------------------------------------------*/
if (strcmp(txtErr, "syntax error") == 0)
    ERR_PorTerminal("%d syntax error ",yylineno);
else
    ERR_PorTerminal("%d %s ",yylineno, txtErr);
}
