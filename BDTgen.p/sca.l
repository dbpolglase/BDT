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

%{
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <limits.h>

#include <tpc.h>
#include <mdi.h>
#include <err.h>
#include <ppr.h>
#include <dat.h>	/* para la union YYSTYPE en tokens.h */
#include <sca.h>
#include <bdt.h>

static void LineaDeCodigo();
static void Caracter();
static void String();
static void Literal();
static void Numero();
static void Palabra();
%}

%%

Base{ESP}*:			return TK_BASE;
Identifier{ESP}*:		return TK_IDENTIFICADOR;
Scope{ESP}*:			return TK_AMBITO;
Table{ESP}*:			return TK_TABLA;
Dimension{ESP}*:		return TK_DIMENSION;
First{ESP}+Key{ESP}*:	        return TK_CLAVE_PRINCIPAL;
Uniq{ESP}+Key{ESP}*:		return TK_CLAVE_UNICA;
Not{ESP}+Uniq{ESP}+Key{ESP}*:	return TK_CLAVE_NO_UNICA;
Fields{ESP}*:			return TK_CAMPOS;
Search{ESP}*:		        return TK_SELECCION;
^h>.*$				{ LineaDeCodigo(); return TK_CODIGO_H; }
^c>.*$				{ LineaDeCodigo(); return TK_CODIGO_C; }
char				return TK_CHAR;
unsigned{ESP}+char		return TK_UNS_CHAR;
short{ESP}+int			return TK_SHORT;
unsigned{ESP}+short{ESP}+int	return TK_UNS_SHORT;
int				return TK_INT;
unsigned{ESP}+int		return TK_UNS_INT;
long				return TK_LONG;
unsigned{ESP}+long		return TK_UNS_LONG;
double				return TK_DOUBLE;
float				return TK_FLOAT;

OR				return TK_OR;
AND				return TK_AND;

==				return TK_IGUAL;
!=				return TK_DISTINTO;
\>				return TK_MAYOR;
\>=				return TK_MAYOR_IGUAL;
\<				return TK_MENOR;
\<=				return TK_MENOR_IGUAL;
HAS				return TK_HAS;
HASNO				return TK_HASNO;

\'.*\'				{ Caracter(); return TK_CARACTER;	}
\".*\"				{ String();   return TK_STRING;		}
\<{LET}{LNU}*\>			{ Literal();  return TK_LITERAL;	}
{DIG}+				{ Numero();   return TK_NUMERO;		}
{LET}{LNU}*			{ Palabra();  return TK_PALABRA;	}

{ESP}+				;
\n				;
.				return yytext[0];

%%

#define ES_OCTAL(ch)	((ch >= '0') && (ch <= '7'))

static char *fichPreproc;

/*====================== PROCEDIMIENTO PUBLICO ======================
 * SCA_Inicializar
 * 	Inicializa el analisis lexico de un fichero de definicion
 * 	de la Configuracion del Sistema
 *    Parametros:
 *      fichero: path del fichero a analizar.
 *====================================================================*/
void SCA_Inicializar(char *fichero)
{
/*--------------------------------------------------------------------
 | Se preprocesa el fichero
 +--------------------------------------------------------------------*/
fichPreproc = PPR_PreprocFichero(fichero);
if (fichPreproc == NULL)
    exit(-1);

/*--------------------------------------------------------------------
 | Se abre el fichero para lectura, asignando el "file pointer" al
 | usado por lex.
 +--------------------------------------------------------------------*/
yyin = fopen (fichPreproc, "r");
if (yyin == NULL) {
    unlink(fichPreproc);
    ERR_PorTerminal("fopen error (read) prepro file  \"%s\"", fichPreproc);
    }
}

/*====================== PROCEDIMIENTO PUBLICO ======================
 * SCA_Finalizar
 * 	Finaliza el analisis lexico del fichero de definicion
 * 	de la Configuracion del Sistema (cierra el fichero)
 *====================================================================*/
void SCA_Finalizar(void)
{
fclose(yyin);
unlink(fichPreproc);
}

/*====================== PROCEDIMIENTO PRIVADO ======================
 * LineaDeCodigo
 * 	Compone una linea de codigo (h o c) quitando la cabecera
 *	("h>" o "c>") y adjuntando un salto de linea.
 *	El texto lo toma de yytext y la linea la deja en yylval
 *====================================================================*/
static void LineaDeCodigo()
{
/*--------------------------------------------------------------------
 | Se eliminan dos caracteres correspondientes a "h>" o "c>"
 | Se le encadena un salto de linea.
 +--------------------------------------------------------------------*/
memmove(yytext, yytext + 2, yyleng - 1);
strcat((char *)yytext, "\n");
yylval.linCod = (char *)yytext;
}


/*========================= FUNCION PUBLICA =========================
 * Caracter
 *	Realiza la conversion de una tira de texto en el caracter
 *	correspondiente.
 *	El texto lo toma de yytext y el caracter lo deja en yylval
 *====================================================================*/
static void Caracter()
{
	int  i;
	char *pTexto;

/*--------------------------------------------------------------------
 | Se salta la primera comilla.
 +--------------------------------------------------------------------*/
pTexto = (char *) yytext + 1;

/*--------------------------------------------------------------------
 | Si hay back-slash el caracter puede ser:
 |	- Especial: \a, \b, \f, \n, \r, \t, \v, se transforma.
 |	- Octal: de 1 a 3 digitos octales, se calcula su valor.
 |	- Otros: se toma tal cual.
 +--------------------------------------------------------------------*/
if (*pTexto == '\\') {
    pTexto++;
    switch(*pTexto) {
	case 'a':
	    yylval.caracter = '\a';
	    break;
	case 'b':
	    yylval.caracter = '\b';
	    break;
	case 'f':
	    yylval.caracter = '\f';
	    break;
	case 'n':
	    yylval.caracter = '\n';
	    break;
	case 'r':
	    yylval.caracter = '\r';
	    break;
	case 't':
	    yylval.caracter = '\t';
	    break;
	case 'v':
	    yylval.caracter = '\v';
	    break;
	default:
	    if (ES_OCTAL(*pTexto)) {
		yylval.caracter = *pTexto - '0';
		pTexto++;
		for (i = 0; i < 2; i++) {
		    if (! ES_OCTAL(*pTexto))
			break;
		    yylval.caracter *= 8;
		    yylval.caracter +=  *pTexto - '0';
		    pTexto++;
		    }
		pTexto--;
		}
	    else
		yylval.caracter = *pTexto;
	    break;
	}
    }
else
    yylval.caracter = *pTexto;

/*--------------------------------------------------------------------
 | Se comprueba que se ha llegado al final del texto una comilla
 +--------------------------------------------------------------------*/
pTexto++;
if (*pTexto != '\'') {
    ERR_PorTerminal("%d character %s wrong definition", yylineno,yytext);
    yylval.caracter = '\0';
    }
}

/*========================= FUNCION PUBLICA =========================
 * String
 *	Toma el texto de yytext y deja el string en yylval
 *====================================================================*/
static void String()
{
/*--------------------------------------------------------------------
 | Se hace una copia en memoria dinamica
 +--------------------------------------------------------------------*/
yylval.string = MDI_String((char *) yytext);
}


/*========================= FUNCION PUBLICA =========================
 * Caracter
 *	Realiza la conversion de una tira de texto en el literal
 *	correspondiente.
 *	El texto lo toma de yytext y el literal lo deja en yylval
 *====================================================================*/
static void Literal()
{
/*--------------------------------------------------------------------
 | Se eliminan las dos dos caracteres < y > que acotan el texto y se
 | copia sobre un buffer de memoria dinamica
 +--------------------------------------------------------------------*/
yylval.literal = MDI_Buffer(yyleng - 1);
memcpy(yylval.literal, yytext + 1, yyleng - 2);
yylval.literal[yyleng - 1] = '\0';
}

/*========================= FUNCION PUBLICA =========================
 * Numero
 *	Realiza la conversion de una tira de texto en el numero entero
 *	correspondiente.
 *	El texto lo toma de yytext y el numero lo deja en yylval
 *====================================================================*/
static void Numero()
{
	long valor;

/*--------------------------------------------------------------------
 | Se transforma la tira de texto, comprobando que no haya overflow.
 +--------------------------------------------------------------------*/
errno = 0;
valor = strtol((char *) yytext, (char **) NULL, 10);
if ((errno == ERANGE) || (valor > (long)INT_MAX))
    ERR_PorTerminal("%d value of \"%s\" out of range ", yylineno,yytext);

yylval.numero = (int)valor;
}

/*========================= FUNCION PUBLICA =========================
 * Palabra
 *	Toma el texto de yytext y deja la palabre en yylval
 *====================================================================*/
static void Palabra()
{
yylval.palabra = MDI_String((char *) yytext);
}
