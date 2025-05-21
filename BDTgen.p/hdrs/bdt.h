/* A Bison parser, made by GNU Bison 3.8.2.  */

/* Bison interface for Yacc-like parsers in C

   Copyright (C) 1984, 1989-1990, 2000-2015, 2018-2021 Free Software Foundation,
   Inc.

   This program is free software: you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation, either version 3 of the License, or
   (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program.  If not, see <https://www.gnu.org/licenses/>.  */

/* As a special exception, you may create a larger work that contains
   part or all of the Bison parser skeleton and distribute that work
   under terms of your choice, so long as that work isn't itself a
   parser generator using the skeleton or a modified version thereof
   as a parser skeleton.  Alternatively, if you modify or redistribute
   the parser skeleton itself, you may (at your option) remove this
   special exception, which will cause the skeleton and the resulting
   Bison output files to be licensed under the GNU General Public
   License without this special exception.

   This special exception was added by the Free Software Foundation in
   version 2.2 of Bison.  */

/* DO NOT RELY ON FEATURES THAT ARE NOT DOCUMENTED in the manual,
   especially those whose name start with YY_ or yy_.  They are
   private implementation details that can be changed or removed.  */

#ifndef YY_YY_BDT_H_INCLUDED
# define YY_YY_BDT_H_INCLUDED
/* Debug traces.  */
#ifndef YYDEBUG
# define YYDEBUG 0
#endif
#if YYDEBUG
extern int yydebug;
#endif

/* Token kinds.  */
#ifndef YYTOKENTYPE
# define YYTOKENTYPE
  enum yytokentype
  {
    YYEMPTY = -2,
    YYEOF = 0,                     /* "end of file"  */
    YYerror = 256,                 /* error  */
    YYUNDEF = 257,                 /* "invalid token"  */
    TK_BASE = 258,                 /* TK_BASE  */
    TK_IDENTIFICADOR = 259,        /* TK_IDENTIFICADOR  */
    TK_AMBITO = 260,               /* TK_AMBITO  */
    TK_TABLA = 261,                /* TK_TABLA  */
    TK_DIMENSION = 262,            /* TK_DIMENSION  */
    TK_CLAVE_PRINCIPAL = 263,      /* TK_CLAVE_PRINCIPAL  */
    TK_CLAVE_UNICA = 264,          /* TK_CLAVE_UNICA  */
    TK_CLAVE_NO_UNICA = 265,       /* TK_CLAVE_NO_UNICA  */
    TK_CAMPOS = 266,               /* TK_CAMPOS  */
    TK_SELECCION = 267,            /* TK_SELECCION  */
    TK_CODIGO_H = 268,             /* TK_CODIGO_H  */
    TK_CODIGO_C = 269,             /* TK_CODIGO_C  */
    TK_CHAR = 270,                 /* TK_CHAR  */
    TK_UNS_CHAR = 271,             /* TK_UNS_CHAR  */
    TK_SHORT = 272,                /* TK_SHORT  */
    TK_UNS_SHORT = 273,            /* TK_UNS_SHORT  */
    TK_INT = 274,                  /* TK_INT  */
    TK_UNS_INT = 275,              /* TK_UNS_INT  */
    TK_LONG = 276,                 /* TK_LONG  */
    TK_UNS_LONG = 277,             /* TK_UNS_LONG  */
    TK_FLOAT = 278,                /* TK_FLOAT  */
    TK_DOUBLE = 279,               /* TK_DOUBLE  */
    TK_OR = 280,                   /* TK_OR  */
    TK_AND = 281,                  /* TK_AND  */
    TK_IGUAL = 282,                /* TK_IGUAL  */
    TK_DISTINTO = 283,             /* TK_DISTINTO  */
    TK_MAYOR = 284,                /* TK_MAYOR  */
    TK_MAYOR_IGUAL = 285,          /* TK_MAYOR_IGUAL  */
    TK_MENOR = 286,                /* TK_MENOR  */
    TK_MENOR_IGUAL = 287,          /* TK_MENOR_IGUAL  */
    TK_HAS = 288,                  /* TK_HAS  */
    TK_HASNO = 289,                /* TK_HASNO  */
    TK_CARACTER = 290,             /* TK_CARACTER  */
    TK_STRING = 291,               /* TK_STRING  */
    TK_LITERAL = 292,              /* TK_LITERAL  */
    TK_NUMERO = 293,               /* TK_NUMERO  */
    TK_PALABRA = 294               /* TK_PALABRA  */
  };
  typedef enum yytokentype yytoken_kind_t;
#endif
/* Token kinds.  */
#define YYEMPTY -2
#define YYEOF 0
#define YYerror 256
#define YYUNDEF 257
#define TK_BASE 258
#define TK_IDENTIFICADOR 259
#define TK_AMBITO 260
#define TK_TABLA 261
#define TK_DIMENSION 262
#define TK_CLAVE_PRINCIPAL 263
#define TK_CLAVE_UNICA 264
#define TK_CLAVE_NO_UNICA 265
#define TK_CAMPOS 266
#define TK_SELECCION 267
#define TK_CODIGO_H 268
#define TK_CODIGO_C 269
#define TK_CHAR 270
#define TK_UNS_CHAR 271
#define TK_SHORT 272
#define TK_UNS_SHORT 273
#define TK_INT 274
#define TK_UNS_INT 275
#define TK_LONG 276
#define TK_UNS_LONG 277
#define TK_FLOAT 278
#define TK_DOUBLE 279
#define TK_OR 280
#define TK_AND 281
#define TK_IGUAL 282
#define TK_DISTINTO 283
#define TK_MAYOR 284
#define TK_MAYOR_IGUAL 285
#define TK_MENOR 286
#define TK_MENOR_IGUAL 287
#define TK_HAS 288
#define TK_HASNO 289
#define TK_CARACTER 290
#define TK_STRING 291
#define TK_LITERAL 292
#define TK_NUMERO 293
#define TK_PALABRA 294

/* Value type.  */
#if ! defined YYSTYPE && ! defined YYSTYPE_IS_DECLARED
union YYSTYPE
{
#line 43 "bdt.y"

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
	

#line 166 "bdt.h"

};
typedef union YYSTYPE YYSTYPE;
# define YYSTYPE_IS_TRIVIAL 1
# define YYSTYPE_IS_DECLARED 1
#endif


extern YYSTYPE yylval;


int yyparse (void);


#endif /* !YY_YY_BDT_H_INCLUDED  */
