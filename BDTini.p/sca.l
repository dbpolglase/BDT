%option yylineno
%option noyywrap
%option noinput
%option nounput

divert(0)dnl M4
changequote(,)dnl M4

ESP		[\t ]
LET		[a-zA-Z]
DIG		[0-9]
LNU		[a-zA-Z0-9_]
SEP		[=]

%{
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <limits.h>

#include <mdi.h>
#include <err.h>
#include <ppr.h>
#include <sca.h>
#include <bdt.h>

static void  Caracter();
static void  Palabra();
static void  String();
static void  Array();
static char *CalcularCaracter(char *, char *);
static void  NumNatural();
static void  NumEntero();
static void  NumReal();

%}

%%

Tabla{ESP}*:			return TK_TABLA;
RESET{ESP}*TABLES		return TK_INICIAR;
\'.*\'				{ Caracter();	    return TK_CARACTER;	   }
{LET}{LNU}*			{ Palabra();	    return TK_PALABRA;	   }
\"[^"]+\"			{ String();	    return TK_STRING;	   }
\{[^}]+\}			{ Array();	    return TK_ARRAY;	   }
{DIG}+				{ NumNatural();	    return TK_NUM_NATURAL; }
-{DIG}+				{ NumEntero();	    return TK_NUM_ENTERO;  }
{DIG}*\.{DIG}+			{ NumReal();	    return TK_NUM_REAL;	   }
{SEP}{2,}			return TK_SEPARADOR;

{ESP}+				;
\n				;
.				return yytext[0];

%%

#define ES_OCTAL(ch) 	((ch >= '0') && (ch <= '7'))

static char *fichPreproc;

/*====================== PROCEDIMIENTO PUBLICO ======================
 * SCA_Inicializar
 * -----------------
 *    FUNCION:
 * 	Inicializa el analisis lexico de un fichero de definicion
 * 	de la Configuracion del Sistema
 *    PARAMETROS:
 *      fichero: path del fichero a analizar.
 *====================================================================*/
void SCA_Inicializar(char *fichero)
{
/*--------------------------------------------------------------------
 * Se preprocesa el fichero
 *--------------------------------------------------------------------*/
fichPreproc = PPR_PreprocFichero(fichero);
if (fichPreproc == NULL)
    exit(-1);

/*--------------------------------------------------------------------
 * Se abre el fichero para lectura, asignando el "file pointer" al
 * usado por lex.
 *--------------------------------------------------------------------*/
yyin = fopen (fichPreproc, "r");
if (yyin == NULL) {
    unlink(fichPreproc);
    ERR_PorTerminal("fail fopen(read) file preproc. \"%s\"", fichPreproc);
    }
}

/*====================== PROCEDIMIENTO PUBLICO ======================
 * SCA_Finalizar
 * -----------------
 *    FUNCION:
 * 	Finaliza el analisis lexico del fichero de definicion
 * 	de la Configuracion del Sistema (cierra el fichero)
 *====================================================================*/
void SCA_Finalizar(void)
{
fclose(yyin);
unlink(fichPreproc);
}

/*====================== PROCEDIMIENTO PRIVADO ======================
 * Caracter
 *	Extrae el valor del caracter del lexema, transformando los
 *	caracteres especiales.
 *====================================================================*/
static void Caracter()
{
	char *pFin;

/*--------------------------------------------------------------------
 | Calcula el primer caracter despues de la comilla.
 | Para que no haya mas de un caracter definido, el ultimo caracter
 | debe ser otra comilla.
 +--------------------------------------------------------------------*/
pFin = CalcularCaracter((char *) yytext + 1, &yylval.caracter);
if (*pFin != '\'')
   ERR_PorTerminal("%d character \"%s\" too long", yylineno,yytext);
}

/*====================== PROCEDIMIENTO PRIVADO ======================
 * String
 *	Extrae un string del lexema, tal que le quita las dobles
 *	comillas, transforma los caracteres especiales y calcula la
 *	longitud final del string.
 *====================================================================*/
static void String()
{
	char *pTexto;
	char *pString;
	int   lonStr;

/*--------------------------------------------------------------------
 | Transforma el texto sobre el mismo buffer (es siempre mas pequeno)
 | quitando la primera y la ultima doble comilla.
 +--------------------------------------------------------------------*/
pTexto  = (char *) yytext + 1;
pString = (char *) yytext;
lonStr	= 0;
while(*pTexto != '"') {
    pTexto = CalcularCaracter(pTexto, pString);
    pString++;
    lonStr++;
    }
*pString++ = '\0';
lonStr++;

/*--------------------------------------------------------------------
 | Se copia el buffer transformado en memoria dinamica, segun la
 | longitud que se ha calculado.
 +--------------------------------------------------------------------*/
yylval.str.lon = lonStr; 
yylval.str.txt = MDI_Buffer(lonStr);
memcpy(yylval.str.txt, (char *) yytext, lonStr);
}

/*====================== PROCEDIMIENTO PRIVADO ======================
 * Array
 *      Extrae un array del lexema, tal que le quita las dobles
 *      comillas, transforma los caracteres especiales y calcula la
 *      longitud final del string.
 *====================================================================*/
static void Array()
{
        char *pTexto;
        char *pString;
        int   lonStr;

/*--------------------------------------------------------------------
 | Transforma el texto sobre el mismo buffer (es siempre mas pequeno)
 | quitando la primera y la ultima doble comilla.
 +--------------------------------------------------------------------*/
pTexto  = (char *) yytext + 1;
pString = (char *) yytext;
lonStr  = 0;
while(*pTexto != '}') {
    pTexto = CalcularCaracter(pTexto, pString);
    pString++;
    lonStr++;
    }
*pString++ = '\0';
lonStr++;

/*--------------------------------------------------------------------
 | Se copia el buffer transformado en memoria dinamica, segun la
 | longitud que se ha calculado.
 +--------------------------------------------------------------------*/
yylval.str.lon = lonStr;
yylval.str.txt = MDI_Buffer(lonStr);
memcpy(yylval.str.txt, (char *) yytext, lonStr);
}


/*========================= FUNCION PUBLICA =========================
 * CalcularCaracter
 *	Realiza la conversion de una tira de texto en el caracter
 *	correspondiente.
 *
 *    Parametros:
 *	pTexto: punt. al texto a transformar.
 *	pCaracter: punt. para dejar el caracter calculado.
 *    Devuelve:
 *	el puntero a la posicion del caracter siguiente.
 *====================================================================*/
static char *CalcularCaracter(char *pTexto, char *pCaracter)
{
	int  i;

/*--------------------------------------------------------------------
 | Si no hay back-slash, el caracter es el del texto, tal cual.
 +--------------------------------------------------------------------*/
if (*pTexto != '\\') {
    *pCaracter = *pTexto++;
    return pTexto;
    }

/*--------------------------------------------------------------------
 | Si hay back-slash el caracter puede ser:
 |	- Especial: \a, \b, \f, \n, \r, \t, \v, se transforma.
 |	- Octal: de 1 a 3 digitos octales, se calcula su valor.
 |	- Otros: se toma tal cual.
 +--------------------------------------------------------------------*/
pTexto++;
switch(*pTexto) {
    case 'a':
	*pCaracter = '\a';
	break;
    case 'b':
	*pCaracter = '\b';
	break;
    case 'f':
	*pCaracter = '\f';
	break;
    case 'n':
	*pCaracter = '\n';
	break;
    case 'r':
	*pCaracter = '\r';
	break;
    case 't':
	*pCaracter = '\t';
	break;
    case 'v':
	*pCaracter = '\v';
	break;
    default:
	if (ES_OCTAL(*pTexto)) {
	    *pCaracter = *pTexto - '0';
	    pTexto++;
	    for (i = 0; i < 2; i++) {
		if (! ES_OCTAL(*pTexto))
		    break;
		*pCaracter *= 8;
		*pCaracter +=  *pTexto - '0';
		pTexto++;
		}
	    pTexto--;
	    }
	else
	    *pCaracter = *pTexto;
	break;
    }

/*--------------------------------------------------------------------
 | Se avanza el puntero y se devuelve.
 +--------------------------------------------------------------------*/
pTexto++;
return pTexto;
}

/*====================== PROCEDIMIENTO PRIVADO ======================
 * NumNatural
 *	Calcula el numero entero positivo asociado al lexema.
 *====================================================================*/
static void NumNatural()
{
/*--------------------------------------------------------------------
 | Se calcula el valor controlando los errores de rango.
 +--------------------------------------------------------------------*/
errno = 0;
yylval.natural = strtoul((char *) yytext, (char **) NULL, 10);
if (errno == ERANGE)
    ERR_PorTerminal("%d value of \"%s\" out of range", yylineno,yytext);
}

/*====================== PROCEDIMIENTO PRIVADO ======================
 * NumEntero
 *	Calcula el numero entero negativo asociado al lexema.
 *====================================================================*/
static void NumEntero()
{
/*--------------------------------------------------------------------
 | Se calcula el valor controlando los errores de rango.
 +--------------------------------------------------------------------*/
errno = 0;
yylval.entero = strtol((char *) yytext, (char **) NULL, 10);
if (errno == ERANGE)
    ERR_PorTerminal("%d value of \"%s\" out of range", yylineno,yytext);
}

/*====================== PROCEDIMIENTO PRIVADO ======================
 * NumReal
 *	Calcula el numero real asociado al lexema.
 *====================================================================*/
static void NumReal()
{
/*--------------------------------------------------------------------
 | Se calcula el valor controlando los errores de rango.
 +--------------------------------------------------------------------*/
errno = 0;
yylval.real = strtod((char *) yytext, (char **) NULL);
if (errno == ERANGE)
    ERR_PorTerminal("%d value of \"%s\" out of range", yylineno,yytext);
}

/*====================== PROCEDIMIENTO PRIVADO ======================
 * Palabra
 *	Recoge el lexema asociado a la palabra.
 *====================================================================*/
static void Palabra()
{
yylval.palabra = MDI_String((char *) yytext);
}
