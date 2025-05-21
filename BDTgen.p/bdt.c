/* A Bison parser, made by GNU Bison 3.8.2.  */

/* Bison implementation for Yacc-like parsers in C

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

/* C LALR(1) parser skeleton written by Richard Stallman, by
   simplifying the original so-called "semantic" parser.  */

/* DO NOT RELY ON FEATURES THAT ARE NOT DOCUMENTED in the manual,
   especially those whose name start with YY_ or yy_.  They are
   private implementation details that can be changed or removed.  */

/* All symbols defined below should begin with yy or YY, to avoid
   infringing on user name space.  This should be done even for local
   variables, as they might otherwise be expanded by user macros.
   There are some unavoidable exceptions within include files to
   define necessary library symbols; they are noted "INFRINGES ON
   USER NAME SPACE" below.  */

/* Identify Bison output, and Bison version.  */
#define YYBISON 30802

/* Bison version string.  */
#define YYBISON_VERSION "3.8.2"

/* Skeleton name.  */
#define YYSKELETON_NAME "yacc.c"

/* Pure parsers.  */
#define YYPURE 0

/* Push parsers.  */
#define YYPUSH 0

/* Pull parsers.  */
#define YYPULL 1




/* First part of user prologue.  */
#line 1 "bdt.y"

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


#line 113 "bdt.c"

# ifndef YY_CAST
#  ifdef __cplusplus
#   define YY_CAST(Type, Val) static_cast<Type> (Val)
#   define YY_REINTERPRET_CAST(Type, Val) reinterpret_cast<Type> (Val)
#  else
#   define YY_CAST(Type, Val) ((Type) (Val))
#   define YY_REINTERPRET_CAST(Type, Val) ((Type) (Val))
#  endif
# endif
# ifndef YY_NULLPTR
#  if defined __cplusplus
#   if 201103L <= __cplusplus
#    define YY_NULLPTR nullptr
#   else
#    define YY_NULLPTR 0
#   endif
#  else
#   define YY_NULLPTR ((void*)0)
#  endif
# endif

#include "bdt.h"
/* Symbol kind.  */
enum yysymbol_kind_t
{
  YYSYMBOL_YYEMPTY = -2,
  YYSYMBOL_YYEOF = 0,                      /* "end of file"  */
  YYSYMBOL_YYerror = 1,                    /* error  */
  YYSYMBOL_YYUNDEF = 2,                    /* "invalid token"  */
  YYSYMBOL_TK_BASE = 3,                    /* TK_BASE  */
  YYSYMBOL_TK_IDENTIFICADOR = 4,           /* TK_IDENTIFICADOR  */
  YYSYMBOL_TK_AMBITO = 5,                  /* TK_AMBITO  */
  YYSYMBOL_TK_TABLA = 6,                   /* TK_TABLA  */
  YYSYMBOL_TK_DIMENSION = 7,               /* TK_DIMENSION  */
  YYSYMBOL_TK_CLAVE_PRINCIPAL = 8,         /* TK_CLAVE_PRINCIPAL  */
  YYSYMBOL_TK_CLAVE_UNICA = 9,             /* TK_CLAVE_UNICA  */
  YYSYMBOL_TK_CLAVE_NO_UNICA = 10,         /* TK_CLAVE_NO_UNICA  */
  YYSYMBOL_TK_CAMPOS = 11,                 /* TK_CAMPOS  */
  YYSYMBOL_TK_SELECCION = 12,              /* TK_SELECCION  */
  YYSYMBOL_TK_CODIGO_H = 13,               /* TK_CODIGO_H  */
  YYSYMBOL_TK_CODIGO_C = 14,               /* TK_CODIGO_C  */
  YYSYMBOL_TK_CHAR = 15,                   /* TK_CHAR  */
  YYSYMBOL_TK_UNS_CHAR = 16,               /* TK_UNS_CHAR  */
  YYSYMBOL_TK_SHORT = 17,                  /* TK_SHORT  */
  YYSYMBOL_TK_UNS_SHORT = 18,              /* TK_UNS_SHORT  */
  YYSYMBOL_TK_INT = 19,                    /* TK_INT  */
  YYSYMBOL_TK_UNS_INT = 20,                /* TK_UNS_INT  */
  YYSYMBOL_TK_LONG = 21,                   /* TK_LONG  */
  YYSYMBOL_TK_UNS_LONG = 22,               /* TK_UNS_LONG  */
  YYSYMBOL_TK_FLOAT = 23,                  /* TK_FLOAT  */
  YYSYMBOL_TK_DOUBLE = 24,                 /* TK_DOUBLE  */
  YYSYMBOL_TK_OR = 25,                     /* TK_OR  */
  YYSYMBOL_TK_AND = 26,                    /* TK_AND  */
  YYSYMBOL_TK_IGUAL = 27,                  /* TK_IGUAL  */
  YYSYMBOL_TK_DISTINTO = 28,               /* TK_DISTINTO  */
  YYSYMBOL_TK_MAYOR = 29,                  /* TK_MAYOR  */
  YYSYMBOL_TK_MAYOR_IGUAL = 30,            /* TK_MAYOR_IGUAL  */
  YYSYMBOL_TK_MENOR = 31,                  /* TK_MENOR  */
  YYSYMBOL_TK_MENOR_IGUAL = 32,            /* TK_MENOR_IGUAL  */
  YYSYMBOL_TK_HAS = 33,                    /* TK_HAS  */
  YYSYMBOL_TK_HASNO = 34,                  /* TK_HASNO  */
  YYSYMBOL_TK_CARACTER = 35,               /* TK_CARACTER  */
  YYSYMBOL_TK_STRING = 36,                 /* TK_STRING  */
  YYSYMBOL_TK_LITERAL = 37,                /* TK_LITERAL  */
  YYSYMBOL_TK_NUMERO = 38,                 /* TK_NUMERO  */
  YYSYMBOL_TK_PALABRA = 39,                /* TK_PALABRA  */
  YYSYMBOL_40_ = 40,                       /* '['  */
  YYSYMBOL_41_ = 41,                       /* ']'  */
  YYSYMBOL_42_ = 42,                       /* '='  */
  YYSYMBOL_43_ = 43,                       /* '('  */
  YYSYMBOL_44_ = 44,                       /* ')'  */
  YYSYMBOL_YYACCEPT = 45,                  /* $accept  */
  YYSYMBOL_BaseDatos = 46,                 /* BaseDatos  */
  YYSYMBOL_NombreBase = 47,                /* NombreBase  */
  YYSYMBOL_IdentifBase = 48,               /* IdentifBase  */
  YYSYMBOL_AmbitoBase = 49,                /* AmbitoBase  */
  YYSYMBOL_Tablas = 50,                    /* Tablas  */
  YYSYMBOL_Tabla = 51,                     /* Tabla  */
  YYSYMBOL_52_1 = 52,                      /* $@1  */
  YYSYMBOL_NombreTabla = 53,               /* NombreTabla  */
  YYSYMBOL_DimensTabla = 54,               /* DimensTabla  */
  YYSYMBOL_CamposTabla = 55,               /* CamposTabla  */
  YYSYMBOL_ListaCampos = 56,               /* ListaCampos  */
  YYSYMBOL_UnCampo = 57,                   /* UnCampo  */
  YYSYMBOL_TipoCampo = 58,                 /* TipoCampo  */
  YYSYMBOL_DimensCampo = 59,               /* DimensCampo  */
  YYSYMBOL_ClavesTabla = 60,               /* ClavesTabla  */
  YYSYMBOL_ClavePrincipal = 61,            /* ClavePrincipal  */
  YYSYMBOL_62_2 = 62,                      /* $@2  */
  YYSYMBOL_ClavesSecund = 63,              /* ClavesSecund  */
  YYSYMBOL_UnaClaveSecund = 64,            /* UnaClaveSecund  */
  YYSYMBOL_65_3 = 65,                      /* $@3  */
  YYSYMBOL_TipoClaveSecund = 66,           /* TipoClaveSecund  */
  YYSYMBOL_ListaCamposClave = 67,          /* ListaCamposClave  */
  YYSYMBOL_SelecTabla = 68,                /* SelecTabla  */
  YYSYMBOL_UnaSeleccion = 69,              /* UnaSeleccion  */
  YYSYMBOL_70_4 = 70,                      /* $@4  */
  YYSYMBOL_ExprSelOr = 71,                 /* ExprSelOr  */
  YYSYMBOL_ExprSelAnd = 72,                /* ExprSelAnd  */
  YYSYMBOL_ExprSelParent = 73,             /* ExprSelParent  */
  YYSYMBOL_ExprRelac = 74,                 /* ExprRelac  */
  YYSYMBOL_CampoRelac = 75,                /* CampoRelac  */
  YYSYMBOL_OperRelac = 76,                 /* OperRelac  */
  YYSYMBOL_ValorRelac = 77,                /* ValorRelac  */
  YYSYMBOL_Codigo = 78,                    /* Codigo  */
  YYSYMBOL_Palabra = 79,                   /* Palabra  */
  YYSYMBOL_String = 80,                    /* String  */
  YYSYMBOL_Numero = 81                     /* Numero  */
};
typedef enum yysymbol_kind_t yysymbol_kind_t;




#ifdef short
# undef short
#endif

/* On compilers that do not define __PTRDIFF_MAX__ etc., make sure
   <limits.h> and (if available) <stdint.h> are included
   so that the code can choose integer types of a good width.  */

#ifndef __PTRDIFF_MAX__
# include <limits.h> /* INFRINGES ON USER NAME SPACE */
# if defined __STDC_VERSION__ && 199901 <= __STDC_VERSION__
#  include <stdint.h> /* INFRINGES ON USER NAME SPACE */
#  define YY_STDINT_H
# endif
#endif

/* Narrow types that promote to a signed type and that can represent a
   signed or unsigned integer of at least N bits.  In tables they can
   save space and decrease cache pressure.  Promoting to a signed type
   helps avoid bugs in integer arithmetic.  */

#ifdef __INT_LEAST8_MAX__
typedef __INT_LEAST8_TYPE__ yytype_int8;
#elif defined YY_STDINT_H
typedef int_least8_t yytype_int8;
#else
typedef signed char yytype_int8;
#endif

#ifdef __INT_LEAST16_MAX__
typedef __INT_LEAST16_TYPE__ yytype_int16;
#elif defined YY_STDINT_H
typedef int_least16_t yytype_int16;
#else
typedef short yytype_int16;
#endif

/* Work around bug in HP-UX 11.23, which defines these macros
   incorrectly for preprocessor constants.  This workaround can likely
   be removed in 2023, as HPE has promised support for HP-UX 11.23
   (aka HP-UX 11i v2) only through the end of 2022; see Table 2 of
   <https://h20195.www2.hpe.com/V2/getpdf.aspx/4AA4-7673ENW.pdf>.  */
#ifdef __hpux
# undef UINT_LEAST8_MAX
# undef UINT_LEAST16_MAX
# define UINT_LEAST8_MAX 255
# define UINT_LEAST16_MAX 65535
#endif

#if defined __UINT_LEAST8_MAX__ && __UINT_LEAST8_MAX__ <= __INT_MAX__
typedef __UINT_LEAST8_TYPE__ yytype_uint8;
#elif (!defined __UINT_LEAST8_MAX__ && defined YY_STDINT_H \
       && UINT_LEAST8_MAX <= INT_MAX)
typedef uint_least8_t yytype_uint8;
#elif !defined __UINT_LEAST8_MAX__ && UCHAR_MAX <= INT_MAX
typedef unsigned char yytype_uint8;
#else
typedef short yytype_uint8;
#endif

#if defined __UINT_LEAST16_MAX__ && __UINT_LEAST16_MAX__ <= __INT_MAX__
typedef __UINT_LEAST16_TYPE__ yytype_uint16;
#elif (!defined __UINT_LEAST16_MAX__ && defined YY_STDINT_H \
       && UINT_LEAST16_MAX <= INT_MAX)
typedef uint_least16_t yytype_uint16;
#elif !defined __UINT_LEAST16_MAX__ && USHRT_MAX <= INT_MAX
typedef unsigned short yytype_uint16;
#else
typedef int yytype_uint16;
#endif

#ifndef YYPTRDIFF_T
# if defined __PTRDIFF_TYPE__ && defined __PTRDIFF_MAX__
#  define YYPTRDIFF_T __PTRDIFF_TYPE__
#  define YYPTRDIFF_MAXIMUM __PTRDIFF_MAX__
# elif defined PTRDIFF_MAX
#  ifndef ptrdiff_t
#   include <stddef.h> /* INFRINGES ON USER NAME SPACE */
#  endif
#  define YYPTRDIFF_T ptrdiff_t
#  define YYPTRDIFF_MAXIMUM PTRDIFF_MAX
# else
#  define YYPTRDIFF_T long
#  define YYPTRDIFF_MAXIMUM LONG_MAX
# endif
#endif

#ifndef YYSIZE_T
# ifdef __SIZE_TYPE__
#  define YYSIZE_T __SIZE_TYPE__
# elif defined size_t
#  define YYSIZE_T size_t
# elif defined __STDC_VERSION__ && 199901 <= __STDC_VERSION__
#  include <stddef.h> /* INFRINGES ON USER NAME SPACE */
#  define YYSIZE_T size_t
# else
#  define YYSIZE_T unsigned
# endif
#endif

#define YYSIZE_MAXIMUM                                  \
  YY_CAST (YYPTRDIFF_T,                                 \
           (YYPTRDIFF_MAXIMUM < YY_CAST (YYSIZE_T, -1)  \
            ? YYPTRDIFF_MAXIMUM                         \
            : YY_CAST (YYSIZE_T, -1)))

#define YYSIZEOF(X) YY_CAST (YYPTRDIFF_T, sizeof (X))


/* Stored state numbers (used for stacks). */
typedef yytype_int8 yy_state_t;

/* State numbers in computations.  */
typedef int yy_state_fast_t;

#ifndef YY_
# if defined YYENABLE_NLS && YYENABLE_NLS
#  if ENABLE_NLS
#   include <libintl.h> /* INFRINGES ON USER NAME SPACE */
#   define YY_(Msgid) dgettext ("bison-runtime", Msgid)
#  endif
# endif
# ifndef YY_
#  define YY_(Msgid) Msgid
# endif
#endif


#ifndef YY_ATTRIBUTE_PURE
# if defined __GNUC__ && 2 < __GNUC__ + (96 <= __GNUC_MINOR__)
#  define YY_ATTRIBUTE_PURE __attribute__ ((__pure__))
# else
#  define YY_ATTRIBUTE_PURE
# endif
#endif

#ifndef YY_ATTRIBUTE_UNUSED
# if defined __GNUC__ && 2 < __GNUC__ + (7 <= __GNUC_MINOR__)
#  define YY_ATTRIBUTE_UNUSED __attribute__ ((__unused__))
# else
#  define YY_ATTRIBUTE_UNUSED
# endif
#endif

/* Suppress unused-variable warnings by "using" E.  */
#if ! defined lint || defined __GNUC__
# define YY_USE(E) ((void) (E))
#else
# define YY_USE(E) /* empty */
#endif

/* Suppress an incorrect diagnostic about yylval being uninitialized.  */
#if defined __GNUC__ && ! defined __ICC && 406 <= __GNUC__ * 100 + __GNUC_MINOR__
# if __GNUC__ * 100 + __GNUC_MINOR__ < 407
#  define YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN                           \
    _Pragma ("GCC diagnostic push")                                     \
    _Pragma ("GCC diagnostic ignored \"-Wuninitialized\"")
# else
#  define YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN                           \
    _Pragma ("GCC diagnostic push")                                     \
    _Pragma ("GCC diagnostic ignored \"-Wuninitialized\"")              \
    _Pragma ("GCC diagnostic ignored \"-Wmaybe-uninitialized\"")
# endif
# define YY_IGNORE_MAYBE_UNINITIALIZED_END      \
    _Pragma ("GCC diagnostic pop")
#else
# define YY_INITIAL_VALUE(Value) Value
#endif
#ifndef YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
# define YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
# define YY_IGNORE_MAYBE_UNINITIALIZED_END
#endif
#ifndef YY_INITIAL_VALUE
# define YY_INITIAL_VALUE(Value) /* Nothing. */
#endif

#if defined __cplusplus && defined __GNUC__ && ! defined __ICC && 6 <= __GNUC__
# define YY_IGNORE_USELESS_CAST_BEGIN                          \
    _Pragma ("GCC diagnostic push")                            \
    _Pragma ("GCC diagnostic ignored \"-Wuseless-cast\"")
# define YY_IGNORE_USELESS_CAST_END            \
    _Pragma ("GCC diagnostic pop")
#endif
#ifndef YY_IGNORE_USELESS_CAST_BEGIN
# define YY_IGNORE_USELESS_CAST_BEGIN
# define YY_IGNORE_USELESS_CAST_END
#endif


#define YY_ASSERT(E) ((void) (0 && (E)))

#if !defined yyoverflow

/* The parser invokes alloca or malloc; define the necessary symbols.  */

# ifdef YYSTACK_USE_ALLOCA
#  if YYSTACK_USE_ALLOCA
#   ifdef __GNUC__
#    define YYSTACK_ALLOC __builtin_alloca
#   elif defined __BUILTIN_VA_ARG_INCR
#    include <alloca.h> /* INFRINGES ON USER NAME SPACE */
#   elif defined _AIX
#    define YYSTACK_ALLOC __alloca
#   elif defined _MSC_VER
#    include <malloc.h> /* INFRINGES ON USER NAME SPACE */
#    define alloca _alloca
#   else
#    define YYSTACK_ALLOC alloca
#    if ! defined _ALLOCA_H && ! defined EXIT_SUCCESS
#     include <stdlib.h> /* INFRINGES ON USER NAME SPACE */
      /* Use EXIT_SUCCESS as a witness for stdlib.h.  */
#     ifndef EXIT_SUCCESS
#      define EXIT_SUCCESS 0
#     endif
#    endif
#   endif
#  endif
# endif

# ifdef YYSTACK_ALLOC
   /* Pacify GCC's 'empty if-body' warning.  */
#  define YYSTACK_FREE(Ptr) do { /* empty */; } while (0)
#  ifndef YYSTACK_ALLOC_MAXIMUM
    /* The OS might guarantee only one guard page at the bottom of the stack,
       and a page size can be as small as 4096 bytes.  So we cannot safely
       invoke alloca (N) if N exceeds 4096.  Use a slightly smaller number
       to allow for a few compiler-allocated temporary stack slots.  */
#   define YYSTACK_ALLOC_MAXIMUM 4032 /* reasonable circa 2006 */
#  endif
# else
#  define YYSTACK_ALLOC YYMALLOC
#  define YYSTACK_FREE YYFREE
#  ifndef YYSTACK_ALLOC_MAXIMUM
#   define YYSTACK_ALLOC_MAXIMUM YYSIZE_MAXIMUM
#  endif
#  if (defined __cplusplus && ! defined EXIT_SUCCESS \
       && ! ((defined YYMALLOC || defined malloc) \
             && (defined YYFREE || defined free)))
#   include <stdlib.h> /* INFRINGES ON USER NAME SPACE */
#   ifndef EXIT_SUCCESS
#    define EXIT_SUCCESS 0
#   endif
#  endif
#  ifndef YYMALLOC
#   define YYMALLOC malloc
#   if ! defined malloc && ! defined EXIT_SUCCESS
void *malloc (YYSIZE_T); /* INFRINGES ON USER NAME SPACE */
#   endif
#  endif
#  ifndef YYFREE
#   define YYFREE free
#   if ! defined free && ! defined EXIT_SUCCESS
void free (void *); /* INFRINGES ON USER NAME SPACE */
#   endif
#  endif
# endif
#endif /* !defined yyoverflow */

#if (! defined yyoverflow \
     && (! defined __cplusplus \
         || (defined YYSTYPE_IS_TRIVIAL && YYSTYPE_IS_TRIVIAL)))

/* A type that is properly aligned for any stack member.  */
union yyalloc
{
  yy_state_t yyss_alloc;
  YYSTYPE yyvs_alloc;
};

/* The size of the maximum gap between one aligned stack and the next.  */
# define YYSTACK_GAP_MAXIMUM (YYSIZEOF (union yyalloc) - 1)

/* The size of an array large to enough to hold all stacks, each with
   N elements.  */
# define YYSTACK_BYTES(N) \
     ((N) * (YYSIZEOF (yy_state_t) + YYSIZEOF (YYSTYPE)) \
      + YYSTACK_GAP_MAXIMUM)

# define YYCOPY_NEEDED 1

/* Relocate STACK from its old location to the new one.  The
   local variables YYSIZE and YYSTACKSIZE give the old and new number of
   elements in the stack, and YYPTR gives the new location of the
   stack.  Advance YYPTR to a properly aligned location for the next
   stack.  */
# define YYSTACK_RELOCATE(Stack_alloc, Stack)                           \
    do                                                                  \
      {                                                                 \
        YYPTRDIFF_T yynewbytes;                                         \
        YYCOPY (&yyptr->Stack_alloc, Stack, yysize);                    \
        Stack = &yyptr->Stack_alloc;                                    \
        yynewbytes = yystacksize * YYSIZEOF (*Stack) + YYSTACK_GAP_MAXIMUM; \
        yyptr += yynewbytes / YYSIZEOF (*yyptr);                        \
      }                                                                 \
    while (0)

#endif

#if defined YYCOPY_NEEDED && YYCOPY_NEEDED
/* Copy COUNT objects from SRC to DST.  The source and destination do
   not overlap.  */
# ifndef YYCOPY
#  if defined __GNUC__ && 1 < __GNUC__
#   define YYCOPY(Dst, Src, Count) \
      __builtin_memcpy (Dst, Src, YY_CAST (YYSIZE_T, (Count)) * sizeof (*(Src)))
#  else
#   define YYCOPY(Dst, Src, Count)              \
      do                                        \
        {                                       \
          YYPTRDIFF_T yyi;                      \
          for (yyi = 0; yyi < (Count); yyi++)   \
            (Dst)[yyi] = (Src)[yyi];            \
        }                                       \
      while (0)
#  endif
# endif
#endif /* !YYCOPY_NEEDED */

/* YYFINAL -- State number of the termination state.  */
#define YYFINAL  6
/* YYLAST -- Last index in YYTABLE.  */
#define YYLAST   84

/* YYNTOKENS -- Number of terminals.  */
#define YYNTOKENS  45
/* YYNNTS -- Number of nonterminals.  */
#define YYNNTS  37
/* YYNRULES -- Number of rules.  */
#define YYNRULES  69
/* YYNSTATES -- Number of states.  */
#define YYNSTATES  101

/* YYMAXUTOK -- Last valid token kind.  */
#define YYMAXUTOK   294


/* YYTRANSLATE(TOKEN-NUM) -- Symbol number corresponding to TOKEN-NUM
   as returned by yylex, with out-of-bounds checking.  */
#define YYTRANSLATE(YYX)                                \
  (0 <= (YYX) && (YYX) <= YYMAXUTOK                     \
   ? YY_CAST (yysymbol_kind_t, yytranslate[YYX])        \
   : YYSYMBOL_YYUNDEF)

/* YYTRANSLATE[TOKEN-NUM] -- Symbol number corresponding to TOKEN-NUM
   as returned by yylex.  */
static const yytype_int8 yytranslate[] =
{
       0,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
      43,    44,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,    42,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,    40,     2,    41,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     1,     2,     3,     4,
       5,     6,     7,     8,     9,    10,    11,    12,    13,    14,
      15,    16,    17,    18,    19,    20,    21,    22,    23,    24,
      25,    26,    27,    28,    29,    30,    31,    32,    33,    34,
      35,    36,    37,    38,    39
};

#if YYDEBUG
/* YYRLINE[YYN] -- Source line where rule number YYN was defined.  */
static const yytype_int16 yyrline[] =
{
       0,   132,   132,   136,   139,   142,   148,   149,   153,   152,
     160,   163,   170,   173,   174,   177,   181,   182,   183,   184,
     185,   186,   187,   188,   189,   190,   193,   194,   200,   204,
     203,   208,   209,   213,   212,   217,   218,   221,   223,   230,
     231,   235,   234,   240,   242,   245,   247,   250,   251,   254,
     258,   261,   262,   263,   264,   265,   266,   267,   268,   271,
     274,   277,   280,   283,   292,   293,   294,   300,   303,   306
};
#endif

/** Accessing symbol of state STATE.  */
#define YY_ACCESSING_SYMBOL(State) YY_CAST (yysymbol_kind_t, yystos[State])

#if YYDEBUG || 0
/* The user-facing name of the symbol whose (internal) number is
   YYSYMBOL.  No bounds checking.  */
static const char *yysymbol_name (yysymbol_kind_t yysymbol) YY_ATTRIBUTE_UNUSED;

/* YYTNAME[SYMBOL-NUM] -- String name of the symbol SYMBOL-NUM.
   First, the terminals, then, starting at YYNTOKENS, nonterminals.  */
static const char *const yytname[] =
{
  "\"end of file\"", "error", "\"invalid token\"", "TK_BASE",
  "TK_IDENTIFICADOR", "TK_AMBITO", "TK_TABLA", "TK_DIMENSION",
  "TK_CLAVE_PRINCIPAL", "TK_CLAVE_UNICA", "TK_CLAVE_NO_UNICA", "TK_CAMPOS",
  "TK_SELECCION", "TK_CODIGO_H", "TK_CODIGO_C", "TK_CHAR", "TK_UNS_CHAR",
  "TK_SHORT", "TK_UNS_SHORT", "TK_INT", "TK_UNS_INT", "TK_LONG",
  "TK_UNS_LONG", "TK_FLOAT", "TK_DOUBLE", "TK_OR", "TK_AND", "TK_IGUAL",
  "TK_DISTINTO", "TK_MAYOR", "TK_MAYOR_IGUAL", "TK_MENOR",
  "TK_MENOR_IGUAL", "TK_HAS", "TK_HASNO", "TK_CARACTER", "TK_STRING",
  "TK_LITERAL", "TK_NUMERO", "TK_PALABRA", "'['", "']'", "'='", "'('",
  "')'", "$accept", "BaseDatos", "NombreBase", "IdentifBase", "AmbitoBase",
  "Tablas", "Tabla", "$@1", "NombreTabla", "DimensTabla", "CamposTabla",
  "ListaCampos", "UnCampo", "TipoCampo", "DimensCampo", "ClavesTabla",
  "ClavePrincipal", "$@2", "ClavesSecund", "UnaClaveSecund", "$@3",
  "TipoClaveSecund", "ListaCamposClave", "SelecTabla", "UnaSeleccion",
  "$@4", "ExprSelOr", "ExprSelAnd", "ExprSelParent", "ExprRelac",
  "CampoRelac", "OperRelac", "ValorRelac", "Codigo", "Palabra", "String",
  "Numero", YY_NULLPTR
};

static const char *
yysymbol_name (yysymbol_kind_t yysymbol)
{
  return yytname[yysymbol];
}
#endif

#define YYPACT_NINF (-38)

#define yypact_value_is_default(Yyn) \
  ((Yyn) == YYPACT_NINF)

#define YYTABLE_NINF (-1)

#define yytable_value_is_error(Yyn) \
  0

/* YYPACT[STATE-NUM] -- Index in YYTABLE of the portion describing
   STATE-NUM.  */
static const yytype_int8 yypact[] =
{
       0,   -31,     8,    15,   -38,   -38,   -38,   -18,    29,   -38,
     -38,   -18,    36,   -38,   -18,    36,   -38,    37,   -38,   -38,
      17,     5,   -38,   -38,   -38,   -38,   -38,    34,    -6,    38,
     -38,   -38,   -38,   -38,   -38,   -38,   -38,   -38,   -38,   -38,
      -6,   -38,   -18,   -18,   -38,   -38,   -38,     9,   -38,    39,
      23,     5,   -38,     6,   -18,   -38,   -38,   -38,   -38,   -18,
      12,   -18,   -38,   -38,   -38,   -18,   -38,    13,    14,   -38,
     -37,   -18,   -37,    25,    31,   -38,   -38,    -5,   -38,   -18,
     -24,   -37,   -37,   -38,   -38,   -38,   -38,   -38,   -38,   -38,
     -38,     2,   -38,    31,   -38,   -38,   -38,   -38,   -38,   -38,
     -38
};

/* YYDEFACT[STATE-NUM] -- Default reduction number in state STATE-NUM.
   Performed when YYTABLE does not specify something else to do.  Zero
   means the default is an error.  */
static const yytype_int8 yydefact[] =
{
       0,     0,     0,     0,    68,     3,     1,     0,     0,    67,
       4,     0,     0,     5,     0,    66,     7,     0,    10,     6,
       2,     0,     8,    64,    65,    69,    11,     0,     0,     0,
      16,    17,    18,    19,    20,    21,    22,    23,    25,    24,
      12,    14,     0,     0,    40,    32,    13,    27,    29,     9,
      28,     0,    15,     0,     0,    39,    35,    36,    31,     0,
       0,     0,    41,    33,    26,    30,    38,     0,     0,    37,
       0,     0,     0,    42,    44,    46,    48,     0,    50,    34,
       0,     0,     0,    51,    52,    53,    54,    55,    56,    57,
      58,     0,    47,    43,    45,    60,    63,    62,    49,    59,
      61
};

/* YYPGOTO[NTERM-NUM].  */
static const yytype_int8 yypgoto[] =
{
     -38,   -38,   -38,   -38,   -38,   -38,    44,   -38,   -38,   -38,
     -38,   -38,    20,   -38,   -38,   -38,   -38,   -38,   -38,   -38,
     -38,   -38,   -10,   -38,   -38,   -38,    -9,   -19,   -17,   -38,
     -38,   -38,   -38,   -38,    -7,   -25,    16
};

/* YYDEFGOTO[NTERM-NUM].  */
static const yytype_int8 yydefgoto[] =
{
       0,     2,     3,     8,    12,    15,    16,    27,    17,    22,
      29,    40,    41,    42,    52,    44,    45,    53,    50,    58,
      68,    59,    65,    49,    55,    67,    73,    74,    75,    76,
      77,    91,    98,    20,    78,     5,    26
};

/* YYTABLE[YYPACT[STATE-NUM]] -- What to do in state STATE-NUM.  If
   positive, shift that token.  If negative, reduce the rule whose
   number is the opposite.  If YYTABLE_NINF, syntax error.  */
static const yytype_int8 yytable[] =
{
      10,    81,     9,     1,    13,     4,    72,    18,     6,    30,
      31,    32,    33,    34,    35,    36,    37,    38,    39,     7,
      92,     9,    83,    84,    85,    86,    87,    88,    89,    90,
      23,    24,    56,    57,    11,    47,    48,    95,     4,    96,
      97,     9,    14,    25,    21,    28,    43,    62,    61,    51,
      81,    54,    63,    64,    66,    70,    71,    82,    69,    19,
      46,    79,    93,    80,    66,    94,   100,    60,     0,     0,
       0,     0,    69,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,    99
};

static const yytype_int8 yycheck[] =
{
       7,    25,    39,     3,    11,    36,    43,    14,     0,    15,
      16,    17,    18,    19,    20,    21,    22,    23,    24,     4,
      44,    39,    27,    28,    29,    30,    31,    32,    33,    34,
      13,    14,     9,    10,     5,    42,    43,    35,    36,    37,
      38,    39,     6,    38,     7,    11,     8,    54,    42,    40,
      25,    12,    59,    41,    61,    42,    42,    26,    65,    15,
      40,    71,    81,    72,    71,    82,    91,    51,    -1,    -1,
      -1,    -1,    79,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    91
};

/* YYSTOS[STATE-NUM] -- The symbol kind of the accessing symbol of
   state STATE-NUM.  */
static const yytype_int8 yystos[] =
{
       0,     3,    46,    47,    36,    80,     0,     4,    48,    39,
      79,     5,    49,    79,     6,    50,    51,    53,    79,    51,
      78,     7,    54,    13,    14,    38,    81,    52,    11,    55,
      15,    16,    17,    18,    19,    20,    21,    22,    23,    24,
      56,    57,    58,     8,    60,    61,    57,    79,    79,    68,
      63,    40,    59,    62,    12,    69,     9,    10,    64,    66,
      81,    42,    79,    79,    41,    67,    79,    70,    65,    79,
      42,    42,    43,    71,    72,    73,    74,    75,    79,    67,
      71,    25,    26,    27,    28,    29,    30,    31,    32,    33,
      34,    76,    44,    72,    73,    35,    37,    38,    77,    79,
      80
};

/* YYR1[RULE-NUM] -- Symbol kind of the left-hand side of rule RULE-NUM.  */
static const yytype_int8 yyr1[] =
{
       0,    45,    46,    47,    48,    49,    50,    50,    52,    51,
      53,    54,    55,    56,    56,    57,    58,    58,    58,    58,
      58,    58,    58,    58,    58,    58,    59,    59,    60,    62,
      61,    63,    63,    65,    64,    66,    66,    67,    67,    68,
      68,    70,    69,    71,    71,    72,    72,    73,    73,    74,
      75,    76,    76,    76,    76,    76,    76,    76,    76,    77,
      77,    77,    77,    77,    78,    78,    78,    79,    80,    81
};

/* YYR2[RULE-NUM] -- Number of symbols on the right-hand side of rule RULE-NUM.  */
static const yytype_int8 yyr2[] =
{
       0,     2,     5,     2,     2,     2,     2,     1,     0,     6,
       2,     2,     2,     2,     1,     3,     1,     1,     1,     1,
       1,     1,     1,     1,     1,     1,     3,     0,     2,     0,
       5,     2,     0,     0,     5,     1,     1,     2,     1,     2,
       0,     0,     5,     3,     1,     3,     1,     3,     1,     3,
       1,     1,     1,     1,     1,     1,     1,     1,     1,     1,
       1,     1,     1,     1,     2,     2,     0,     1,     1,     1
};


enum { YYENOMEM = -2 };

#define yyerrok         (yyerrstatus = 0)
#define yyclearin       (yychar = YYEMPTY)

#define YYACCEPT        goto yyacceptlab
#define YYABORT         goto yyabortlab
#define YYERROR         goto yyerrorlab
#define YYNOMEM         goto yyexhaustedlab


#define YYRECOVERING()  (!!yyerrstatus)

#define YYBACKUP(Token, Value)                                    \
  do                                                              \
    if (yychar == YYEMPTY)                                        \
      {                                                           \
        yychar = (Token);                                         \
        yylval = (Value);                                         \
        YYPOPSTACK (yylen);                                       \
        yystate = *yyssp;                                         \
        goto yybackup;                                            \
      }                                                           \
    else                                                          \
      {                                                           \
        yyerror (YY_("syntax error: cannot back up")); \
        YYERROR;                                                  \
      }                                                           \
  while (0)

/* Backward compatibility with an undocumented macro.
   Use YYerror or YYUNDEF. */
#define YYERRCODE YYUNDEF


/* Enable debugging if requested.  */
#if YYDEBUG

# ifndef YYFPRINTF
#  include <stdio.h> /* INFRINGES ON USER NAME SPACE */
#  define YYFPRINTF fprintf
# endif

# define YYDPRINTF(Args)                        \
do {                                            \
  if (yydebug)                                  \
    YYFPRINTF Args;                             \
} while (0)




# define YY_SYMBOL_PRINT(Title, Kind, Value, Location)                    \
do {                                                                      \
  if (yydebug)                                                            \
    {                                                                     \
      YYFPRINTF (stderr, "%s ", Title);                                   \
      yy_symbol_print (stderr,                                            \
                  Kind, Value); \
      YYFPRINTF (stderr, "\n");                                           \
    }                                                                     \
} while (0)


/*-----------------------------------.
| Print this symbol's value on YYO.  |
`-----------------------------------*/

static void
yy_symbol_value_print (FILE *yyo,
                       yysymbol_kind_t yykind, YYSTYPE const * const yyvaluep)
{
  FILE *yyoutput = yyo;
  YY_USE (yyoutput);
  if (!yyvaluep)
    return;
  YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
  YY_USE (yykind);
  YY_IGNORE_MAYBE_UNINITIALIZED_END
}


/*---------------------------.
| Print this symbol on YYO.  |
`---------------------------*/

static void
yy_symbol_print (FILE *yyo,
                 yysymbol_kind_t yykind, YYSTYPE const * const yyvaluep)
{
  YYFPRINTF (yyo, "%s %s (",
             yykind < YYNTOKENS ? "token" : "nterm", yysymbol_name (yykind));

  yy_symbol_value_print (yyo, yykind, yyvaluep);
  YYFPRINTF (yyo, ")");
}

/*------------------------------------------------------------------.
| yy_stack_print -- Print the state stack from its BOTTOM up to its |
| TOP (included).                                                   |
`------------------------------------------------------------------*/

static void
yy_stack_print (yy_state_t *yybottom, yy_state_t *yytop)
{
  YYFPRINTF (stderr, "Stack now");
  for (; yybottom <= yytop; yybottom++)
    {
      int yybot = *yybottom;
      YYFPRINTF (stderr, " %d", yybot);
    }
  YYFPRINTF (stderr, "\n");
}

# define YY_STACK_PRINT(Bottom, Top)                            \
do {                                                            \
  if (yydebug)                                                  \
    yy_stack_print ((Bottom), (Top));                           \
} while (0)


/*------------------------------------------------.
| Report that the YYRULE is going to be reduced.  |
`------------------------------------------------*/

static void
yy_reduce_print (yy_state_t *yyssp, YYSTYPE *yyvsp,
                 int yyrule)
{
  int yylno = yyrline[yyrule];
  int yynrhs = yyr2[yyrule];
  int yyi;
  YYFPRINTF (stderr, "Reducing stack by rule %d (line %d):\n",
             yyrule - 1, yylno);
  /* The symbols being reduced.  */
  for (yyi = 0; yyi < yynrhs; yyi++)
    {
      YYFPRINTF (stderr, "   $%d = ", yyi + 1);
      yy_symbol_print (stderr,
                       YY_ACCESSING_SYMBOL (+yyssp[yyi + 1 - yynrhs]),
                       &yyvsp[(yyi + 1) - (yynrhs)]);
      YYFPRINTF (stderr, "\n");
    }
}

# define YY_REDUCE_PRINT(Rule)          \
do {                                    \
  if (yydebug)                          \
    yy_reduce_print (yyssp, yyvsp, Rule); \
} while (0)

/* Nonzero means print parse trace.  It is left uninitialized so that
   multiple parsers can coexist.  */
int yydebug;
#else /* !YYDEBUG */
# define YYDPRINTF(Args) ((void) 0)
# define YY_SYMBOL_PRINT(Title, Kind, Value, Location)
# define YY_STACK_PRINT(Bottom, Top)
# define YY_REDUCE_PRINT(Rule)
#endif /* !YYDEBUG */


/* YYINITDEPTH -- initial size of the parser's stacks.  */
#ifndef YYINITDEPTH
# define YYINITDEPTH 200
#endif

/* YYMAXDEPTH -- maximum size the stacks can grow to (effective only
   if the built-in stack extension method is used).

   Do not make this value too large; the results are undefined if
   YYSTACK_ALLOC_MAXIMUM < YYSTACK_BYTES (YYMAXDEPTH)
   evaluated with infinite-precision integer arithmetic.  */

#ifndef YYMAXDEPTH
# define YYMAXDEPTH 10000
#endif






/*-----------------------------------------------.
| Release the memory associated to this symbol.  |
`-----------------------------------------------*/

static void
yydestruct (const char *yymsg,
            yysymbol_kind_t yykind, YYSTYPE *yyvaluep)
{
  YY_USE (yyvaluep);
  if (!yymsg)
    yymsg = "Deleting";
  YY_SYMBOL_PRINT (yymsg, yykind, yyvaluep, yylocationp);

  YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
  YY_USE (yykind);
  YY_IGNORE_MAYBE_UNINITIALIZED_END
}


/* Lookahead token kind.  */
int yychar;

/* The semantic value of the lookahead symbol.  */
YYSTYPE yylval;
/* Number of syntax errors so far.  */
int yynerrs;




/*----------.
| yyparse.  |
`----------*/

int
yyparse (void)
{
    yy_state_fast_t yystate = 0;
    /* Number of tokens to shift before error messages enabled.  */
    int yyerrstatus = 0;

    /* Refer to the stacks through separate pointers, to allow yyoverflow
       to reallocate them elsewhere.  */

    /* Their size.  */
    YYPTRDIFF_T yystacksize = YYINITDEPTH;

    /* The state stack: array, bottom, top.  */
    yy_state_t yyssa[YYINITDEPTH];
    yy_state_t *yyss = yyssa;
    yy_state_t *yyssp = yyss;

    /* The semantic value stack: array, bottom, top.  */
    YYSTYPE yyvsa[YYINITDEPTH];
    YYSTYPE *yyvs = yyvsa;
    YYSTYPE *yyvsp = yyvs;

  int yyn;
  /* The return value of yyparse.  */
  int yyresult;
  /* Lookahead symbol kind.  */
  yysymbol_kind_t yytoken = YYSYMBOL_YYEMPTY;
  /* The variables used to return semantic value and location from the
     action routines.  */
  YYSTYPE yyval;



#define YYPOPSTACK(N)   (yyvsp -= (N), yyssp -= (N))

  /* The number of symbols on the RHS of the reduced rule.
     Keep to zero when no symbol should be popped.  */
  int yylen = 0;

  YYDPRINTF ((stderr, "Starting parse\n"));

  yychar = YYEMPTY; /* Cause a token to be read.  */

  goto yysetstate;


/*------------------------------------------------------------.
| yynewstate -- push a new state, which is found in yystate.  |
`------------------------------------------------------------*/
yynewstate:
  /* In all cases, when you get here, the value and location stacks
     have just been pushed.  So pushing a state here evens the stacks.  */
  yyssp++;


/*--------------------------------------------------------------------.
| yysetstate -- set current state (the top of the stack) to yystate.  |
`--------------------------------------------------------------------*/
yysetstate:
  YYDPRINTF ((stderr, "Entering state %d\n", yystate));
  YY_ASSERT (0 <= yystate && yystate < YYNSTATES);
  YY_IGNORE_USELESS_CAST_BEGIN
  *yyssp = YY_CAST (yy_state_t, yystate);
  YY_IGNORE_USELESS_CAST_END
  YY_STACK_PRINT (yyss, yyssp);

  if (yyss + yystacksize - 1 <= yyssp)
#if !defined yyoverflow && !defined YYSTACK_RELOCATE
    YYNOMEM;
#else
    {
      /* Get the current used size of the three stacks, in elements.  */
      YYPTRDIFF_T yysize = yyssp - yyss + 1;

# if defined yyoverflow
      {
        /* Give user a chance to reallocate the stack.  Use copies of
           these so that the &'s don't force the real ones into
           memory.  */
        yy_state_t *yyss1 = yyss;
        YYSTYPE *yyvs1 = yyvs;

        /* Each stack pointer address is followed by the size of the
           data in use in that stack, in bytes.  This used to be a
           conditional around just the two extra args, but that might
           be undefined if yyoverflow is a macro.  */
        yyoverflow (YY_("memory exhausted"),
                    &yyss1, yysize * YYSIZEOF (*yyssp),
                    &yyvs1, yysize * YYSIZEOF (*yyvsp),
                    &yystacksize);
        yyss = yyss1;
        yyvs = yyvs1;
      }
# else /* defined YYSTACK_RELOCATE */
      /* Extend the stack our own way.  */
      if (YYMAXDEPTH <= yystacksize)
        YYNOMEM;
      yystacksize *= 2;
      if (YYMAXDEPTH < yystacksize)
        yystacksize = YYMAXDEPTH;

      {
        yy_state_t *yyss1 = yyss;
        union yyalloc *yyptr =
          YY_CAST (union yyalloc *,
                   YYSTACK_ALLOC (YY_CAST (YYSIZE_T, YYSTACK_BYTES (yystacksize))));
        if (! yyptr)
          YYNOMEM;
        YYSTACK_RELOCATE (yyss_alloc, yyss);
        YYSTACK_RELOCATE (yyvs_alloc, yyvs);
#  undef YYSTACK_RELOCATE
        if (yyss1 != yyssa)
          YYSTACK_FREE (yyss1);
      }
# endif

      yyssp = yyss + yysize - 1;
      yyvsp = yyvs + yysize - 1;

      YY_IGNORE_USELESS_CAST_BEGIN
      YYDPRINTF ((stderr, "Stack size increased to %ld\n",
                  YY_CAST (long, yystacksize)));
      YY_IGNORE_USELESS_CAST_END

      if (yyss + yystacksize - 1 <= yyssp)
        YYABORT;
    }
#endif /* !defined yyoverflow && !defined YYSTACK_RELOCATE */


  if (yystate == YYFINAL)
    YYACCEPT;

  goto yybackup;


/*-----------.
| yybackup.  |
`-----------*/
yybackup:
  /* Do appropriate processing given the current state.  Read a
     lookahead token if we need one and don't already have one.  */

  /* First try to decide what to do without reference to lookahead token.  */
  yyn = yypact[yystate];
  if (yypact_value_is_default (yyn))
    goto yydefault;

  /* Not known => get a lookahead token if don't already have one.  */

  /* YYCHAR is either empty, or end-of-input, or a valid lookahead.  */
  if (yychar == YYEMPTY)
    {
      YYDPRINTF ((stderr, "Reading a token\n"));
      yychar = yylex ();
    }

  if (yychar <= YYEOF)
    {
      yychar = YYEOF;
      yytoken = YYSYMBOL_YYEOF;
      YYDPRINTF ((stderr, "Now at end of input.\n"));
    }
  else if (yychar == YYerror)
    {
      /* The scanner already issued an error message, process directly
         to error recovery.  But do not keep the error token as
         lookahead, it is too special and may lead us to an endless
         loop in error recovery. */
      yychar = YYUNDEF;
      yytoken = YYSYMBOL_YYerror;
      goto yyerrlab1;
    }
  else
    {
      yytoken = YYTRANSLATE (yychar);
      YY_SYMBOL_PRINT ("Next token is", yytoken, &yylval, &yylloc);
    }

  /* If the proper action on seeing token YYTOKEN is to reduce or to
     detect an error, take that action.  */
  yyn += yytoken;
  if (yyn < 0 || YYLAST < yyn || yycheck[yyn] != yytoken)
    goto yydefault;
  yyn = yytable[yyn];
  if (yyn <= 0)
    {
      if (yytable_value_is_error (yyn))
        goto yyerrlab;
      yyn = -yyn;
      goto yyreduce;
    }

  /* Count tokens shifted since error; after three, turn off error
     status.  */
  if (yyerrstatus)
    yyerrstatus--;

  /* Shift the lookahead token.  */
  YY_SYMBOL_PRINT ("Shifting", yytoken, &yylval, &yylloc);
  yystate = yyn;
  YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
  *++yyvsp = yylval;
  YY_IGNORE_MAYBE_UNINITIALIZED_END

  /* Discard the shifted token.  */
  yychar = YYEMPTY;
  goto yynewstate;


/*-----------------------------------------------------------.
| yydefault -- do the default action for the current state.  |
`-----------------------------------------------------------*/
yydefault:
  yyn = yydefact[yystate];
  if (yyn == 0)
    goto yyerrlab;
  goto yyreduce;


/*-----------------------------.
| yyreduce -- do a reduction.  |
`-----------------------------*/
yyreduce:
  /* yyn is the number of a rule to reduce with.  */
  yylen = yyr2[yyn];

  /* If YYLEN is nonzero, implement the default value of the action:
     '$$ = $1'.

     Otherwise, the following line sets YYVAL to garbage.
     This behavior is undocumented and Bison
     users should not rely upon it.  Assigning to YYVAL
     unconditionally makes the parser a bit smaller, and it avoids a
     GCC warning that YYVAL may be used uninitialized.  */
  yyval = yyvsp[1-yylen];


  YY_REDUCE_PRINT (yyn);
  switch (yyn)
    {
  case 2: /* BaseDatos: NombreBase IdentifBase AmbitoBase Tablas Codigo  */
#line 133 "bdt.y"
                                                { DAT_CreBase((yyvsp[-4].string), (yyvsp[-3].palabra), (yyvsp[-2].caracter));    }
#line 1256 "bdt.c"
    break;

  case 3: /* NombreBase: TK_BASE String  */
#line 136 "bdt.y"
                                                { (yyval.string) = TratarNombre((yyvsp[0].string));      }
#line 1262 "bdt.c"
    break;

  case 4: /* IdentifBase: TK_IDENTIFICADOR Palabra  */
#line 139 "bdt.y"
                                                { (yyval.palabra) = (yyvsp[0].palabra);		      }
#line 1268 "bdt.c"
    break;

  case 5: /* AmbitoBase: TK_AMBITO Palabra  */
#line 142 "bdt.y"
                                                { (yyval.caracter) = TratarAmbito((yyvsp[0].palabra));      }
#line 1274 "bdt.c"
    break;

  case 8: /* $@1: %empty  */
#line 153 "bdt.y"
                                { refTabla= DAT_CreTabla((yyvsp[-1].palabra), (yyvsp[0].numero));	}
#line 1280 "bdt.c"
    break;

  case 10: /* NombreTabla: TK_TABLA Palabra  */
#line 160 "bdt.y"
                                                { (yyval.palabra) = (yyvsp[0].palabra);		}
#line 1286 "bdt.c"
    break;

  case 11: /* DimensTabla: TK_DIMENSION Numero  */
#line 163 "bdt.y"
                                                { (yyval.numero) = (yyvsp[0].numero);		}
#line 1292 "bdt.c"
    break;

  case 15: /* UnCampo: TipoCampo Palabra DimensCampo  */
#line 178 "bdt.y"
                                { DAT_CreCampo(refTabla, (yyvsp[-1].palabra), (yyvsp[-2].tipCam), (yyvsp[0].numero));	}
#line 1298 "bdt.c"
    break;

  case 16: /* TipoCampo: TK_CHAR  */
#line 181 "bdt.y"
                                                { (yyval.tipCam) = TPC_CHAR;	}
#line 1304 "bdt.c"
    break;

  case 17: /* TipoCampo: TK_UNS_CHAR  */
#line 182 "bdt.y"
                                                { (yyval.tipCam) = TPC_UNS_CHAR;	}
#line 1310 "bdt.c"
    break;

  case 18: /* TipoCampo: TK_SHORT  */
#line 183 "bdt.y"
                                                { (yyval.tipCam) = TPC_SHORT;	}
#line 1316 "bdt.c"
    break;

  case 19: /* TipoCampo: TK_UNS_SHORT  */
#line 184 "bdt.y"
                                                { (yyval.tipCam) = TPC_UNS_SHORT;	}
#line 1322 "bdt.c"
    break;

  case 20: /* TipoCampo: TK_INT  */
#line 185 "bdt.y"
                                                { (yyval.tipCam) = TPC_INT;		}
#line 1328 "bdt.c"
    break;

  case 21: /* TipoCampo: TK_UNS_INT  */
#line 186 "bdt.y"
                                                { (yyval.tipCam) = TPC_UNS_INT;	}
#line 1334 "bdt.c"
    break;

  case 22: /* TipoCampo: TK_LONG  */
#line 187 "bdt.y"
                                                { (yyval.tipCam) = TPC_LONG;	}
#line 1340 "bdt.c"
    break;

  case 23: /* TipoCampo: TK_UNS_LONG  */
#line 188 "bdt.y"
                                                { (yyval.tipCam) = TPC_UNS_LONG;	}
#line 1346 "bdt.c"
    break;

  case 24: /* TipoCampo: TK_DOUBLE  */
#line 189 "bdt.y"
                                                { (yyval.tipCam) = TPC_DOUBLE;	}
#line 1352 "bdt.c"
    break;

  case 25: /* TipoCampo: TK_FLOAT  */
#line 190 "bdt.y"
                                                { (yyval.tipCam) = TPC_FLOAT;	}
#line 1358 "bdt.c"
    break;

  case 26: /* DimensCampo: '[' Numero ']'  */
#line 193 "bdt.y"
                                                { (yyval.numero) = (yyvsp[-1].numero);		}
#line 1364 "bdt.c"
    break;

  case 27: /* DimensCampo: %empty  */
#line 194 "bdt.y"
                                                { (yyval.numero) = 1;		}
#line 1370 "bdt.c"
    break;

  case 29: /* $@2: %empty  */
#line 204 "bdt.y"
                        { refClave = DAT_CreClave(refTabla, (yyvsp[0].palabra), CLV_PRINCIPAL);}
#line 1376 "bdt.c"
    break;

  case 33: /* $@3: %empty  */
#line 213 "bdt.y"
                        { refClave = DAT_CreClave(refTabla, (yyvsp[0].palabra), (yyvsp[-1].tipCla));	}
#line 1382 "bdt.c"
    break;

  case 35: /* TipoClaveSecund: TK_CLAVE_UNICA  */
#line 217 "bdt.y"
                                                { (yyval.tipCla) = CLV_UNICA;	}
#line 1388 "bdt.c"
    break;

  case 36: /* TipoClaveSecund: TK_CLAVE_NO_UNICA  */
#line 218 "bdt.y"
                                                { (yyval.tipCla) = CLV_NO_UNICA;	}
#line 1394 "bdt.c"
    break;

  case 37: /* ListaCamposClave: ListaCamposClave Palabra  */
#line 222 "bdt.y"
                        { DAT_CreCampoClave(refTabla, refClave, (yyvsp[0].palabra));	}
#line 1400 "bdt.c"
    break;

  case 38: /* ListaCamposClave: Palabra  */
#line 224 "bdt.y"
                        { DAT_CreCampoClave(refTabla, refClave, (yyvsp[0].palabra));	}
#line 1406 "bdt.c"
    break;

  case 41: /* $@4: %empty  */
#line 235 "bdt.y"
                        { refSelec = DAT_CreSeleccion(refTabla, (yyvsp[0].palabra));	}
#line 1412 "bdt.c"
    break;

  case 42: /* UnaSeleccion: TK_SELECCION Palabra $@4 '=' ExprSelOr  */
#line 237 "bdt.y"
                        { DAT_CreCondicSelecion(refSelec, (yyvsp[0].expLog));		}
#line 1418 "bdt.c"
    break;

  case 43: /* ExprSelOr: ExprSelOr TK_OR ExprSelAnd  */
#line 241 "bdt.y"
                                { (yyval.expLog) = ExprLogica((yyvsp[-2].expLog), OPLOG_OR, (yyvsp[0].expLog));	}
#line 1424 "bdt.c"
    break;

  case 44: /* ExprSelOr: ExprSelAnd  */
#line 242 "bdt.y"
                                { (yyval.expLog) = (yyvsp[0].expLog);				}
#line 1430 "bdt.c"
    break;

  case 45: /* ExprSelAnd: ExprSelAnd TK_AND ExprSelParent  */
#line 246 "bdt.y"
                                { (yyval.expLog) = ExprLogica((yyvsp[-2].expLog), OPLOG_AND, (yyvsp[0].expLog));	}
#line 1436 "bdt.c"
    break;

  case 46: /* ExprSelAnd: ExprSelParent  */
#line 247 "bdt.y"
                                { (yyval.expLog) = (yyvsp[0].expLog);				}
#line 1442 "bdt.c"
    break;

  case 47: /* ExprSelParent: '(' ExprSelOr ')'  */
#line 250 "bdt.y"
                                    { (yyval.expLog) = (yyvsp[-1].expLog);			}
#line 1448 "bdt.c"
    break;

  case 48: /* ExprSelParent: ExprRelac  */
#line 251 "bdt.y"
                                    { (yyval.expLog) = (yyvsp[0].expLog);			}
#line 1454 "bdt.c"
    break;

  case 49: /* ExprRelac: CampoRelac OperRelac ValorRelac  */
#line 255 "bdt.y"
                        { (yyval.expLog) = ExprCompar((yyvsp[-2].palabra), (yyvsp[-1].opeRel).ope, (yyvsp[-1].opeRel).txt, (yyvsp[0].valRel).tip, (yyvsp[0].valRel).txt);}
#line 1460 "bdt.c"
    break;

  case 50: /* CampoRelac: Palabra  */
#line 258 "bdt.y"
                                        { (yyval.palabra) = (yyvsp[0].palabra);		}
#line 1466 "bdt.c"
    break;

  case 51: /* OperRelac: TK_IGUAL  */
#line 261 "bdt.y"
                                 { (yyval.opeRel).ope = OPREL_IGUAL;    (yyval.opeRel).txt = "=="; }
#line 1472 "bdt.c"
    break;

  case 52: /* OperRelac: TK_DISTINTO  */
#line 262 "bdt.y"
                                 { (yyval.opeRel).ope = OPREL_DISTINTO; (yyval.opeRel).txt = "=="; }
#line 1478 "bdt.c"
    break;

  case 53: /* OperRelac: TK_MAYOR  */
#line 263 "bdt.y"
                                 { (yyval.opeRel).ope = OPREL_MAYOR;    (yyval.opeRel).txt = ">";  }
#line 1484 "bdt.c"
    break;

  case 54: /* OperRelac: TK_MAYOR_IGUAL  */
#line 264 "bdt.y"
                                 { (yyval.opeRel).ope = OPREL_MAY_IGU;  (yyval.opeRel).txt = ">="; }
#line 1490 "bdt.c"
    break;

  case 55: /* OperRelac: TK_MENOR  */
#line 265 "bdt.y"
                                 { (yyval.opeRel).ope = OPREL_MENOR;    (yyval.opeRel).txt = "<";  }
#line 1496 "bdt.c"
    break;

  case 56: /* OperRelac: TK_MENOR_IGUAL  */
#line 266 "bdt.y"
                                 { (yyval.opeRel).ope = OPREL_MEN_IGU;  (yyval.opeRel).txt = "<="; }
#line 1502 "bdt.c"
    break;

  case 57: /* OperRelac: TK_HAS  */
#line 267 "bdt.y"
                                 { (yyval.opeRel).ope = OPREL_HAS;      (yyval.opeRel).txt = "";   }
#line 1508 "bdt.c"
    break;

  case 58: /* OperRelac: TK_HASNO  */
#line 268 "bdt.y"
                                 { (yyval.opeRel).ope = OPREL_HASNO;    (yyval.opeRel).txt = "";   }
#line 1514 "bdt.c"
    break;

  case 59: /* ValorRelac: Palabra  */
#line 272 "bdt.y"
                        { (yyval.valRel).tip = VALOR_PARAMETRO;
			  (yyval.valRel).txt = (yyvsp[0].palabra);				}
#line 1521 "bdt.c"
    break;

  case 60: /* ValorRelac: TK_CARACTER  */
#line 275 "bdt.y"
                        { (yyval.valRel).tip = VALOR_CARACTER;
			  (yyval.valRel).txt = yylval.string;		}
#line 1528 "bdt.c"
    break;

  case 61: /* ValorRelac: String  */
#line 278 "bdt.y"
                        { (yyval.valRel).tip = VALOR_STRING;
			  (yyval.valRel).txt = (yyvsp[0].string);				}
#line 1535 "bdt.c"
    break;

  case 62: /* ValorRelac: TK_NUMERO  */
#line 281 "bdt.y"
                        { (yyval.valRel).tip = VALOR_NUMERO;
			  (yyval.valRel).txt = yylval.string;		}
#line 1542 "bdt.c"
    break;

  case 63: /* ValorRelac: TK_LITERAL  */
#line 284 "bdt.y"
                        { (yyval.valRel).tip = VALOR_LITERAL;
			  (yyval.valRel).txt = yylval.string;		}
#line 1549 "bdt.c"
    break;

  case 64: /* Codigo: Codigo TK_CODIGO_H  */
#line 292 "bdt.y"
                                        { DAT_Linea_h(yylval.linCod); }
#line 1555 "bdt.c"
    break;

  case 65: /* Codigo: Codigo TK_CODIGO_C  */
#line 293 "bdt.y"
                                        { DAT_Linea_c(yylval.linCod); }
#line 1561 "bdt.c"
    break;

  case 67: /* Palabra: TK_PALABRA  */
#line 300 "bdt.y"
                                                { (yyval.palabra) = yylval.palabra;	}
#line 1567 "bdt.c"
    break;

  case 68: /* String: TK_STRING  */
#line 303 "bdt.y"
                                                { (yyval.string) = yylval.string;	}
#line 1573 "bdt.c"
    break;

  case 69: /* Numero: TK_NUMERO  */
#line 306 "bdt.y"
                                                { (yyval.numero) = yylval.numero;	}
#line 1579 "bdt.c"
    break;


#line 1583 "bdt.c"

      default: break;
    }
  /* User semantic actions sometimes alter yychar, and that requires
     that yytoken be updated with the new translation.  We take the
     approach of translating immediately before every use of yytoken.
     One alternative is translating here after every semantic action,
     but that translation would be missed if the semantic action invokes
     YYABORT, YYACCEPT, or YYERROR immediately after altering yychar or
     if it invokes YYBACKUP.  In the case of YYABORT or YYACCEPT, an
     incorrect destructor might then be invoked immediately.  In the
     case of YYERROR or YYBACKUP, subsequent parser actions might lead
     to an incorrect destructor call or verbose syntax error message
     before the lookahead is translated.  */
  YY_SYMBOL_PRINT ("-> $$ =", YY_CAST (yysymbol_kind_t, yyr1[yyn]), &yyval, &yyloc);

  YYPOPSTACK (yylen);
  yylen = 0;

  *++yyvsp = yyval;

  /* Now 'shift' the result of the reduction.  Determine what state
     that goes to, based on the state we popped back to and the rule
     number reduced by.  */
  {
    const int yylhs = yyr1[yyn] - YYNTOKENS;
    const int yyi = yypgoto[yylhs] + *yyssp;
    yystate = (0 <= yyi && yyi <= YYLAST && yycheck[yyi] == *yyssp
               ? yytable[yyi]
               : yydefgoto[yylhs]);
  }

  goto yynewstate;


/*--------------------------------------.
| yyerrlab -- here on detecting error.  |
`--------------------------------------*/
yyerrlab:
  /* Make sure we have latest lookahead translation.  See comments at
     user semantic actions for why this is necessary.  */
  yytoken = yychar == YYEMPTY ? YYSYMBOL_YYEMPTY : YYTRANSLATE (yychar);
  /* If not already recovering from an error, report this error.  */
  if (!yyerrstatus)
    {
      ++yynerrs;
      yyerror (YY_("syntax error"));
    }

  if (yyerrstatus == 3)
    {
      /* If just tried and failed to reuse lookahead token after an
         error, discard it.  */

      if (yychar <= YYEOF)
        {
          /* Return failure if at end of input.  */
          if (yychar == YYEOF)
            YYABORT;
        }
      else
        {
          yydestruct ("Error: discarding",
                      yytoken, &yylval);
          yychar = YYEMPTY;
        }
    }

  /* Else will try to reuse lookahead token after shifting the error
     token.  */
  goto yyerrlab1;


/*---------------------------------------------------.
| yyerrorlab -- error raised explicitly by YYERROR.  |
`---------------------------------------------------*/
yyerrorlab:
  /* Pacify compilers when the user code never invokes YYERROR and the
     label yyerrorlab therefore never appears in user code.  */
  if (0)
    YYERROR;
  ++yynerrs;

  /* Do not reclaim the symbols of the rule whose action triggered
     this YYERROR.  */
  YYPOPSTACK (yylen);
  yylen = 0;
  YY_STACK_PRINT (yyss, yyssp);
  yystate = *yyssp;
  goto yyerrlab1;


/*-------------------------------------------------------------.
| yyerrlab1 -- common code for both syntax error and YYERROR.  |
`-------------------------------------------------------------*/
yyerrlab1:
  yyerrstatus = 3;      /* Each real token shifted decrements this.  */

  /* Pop stack until we find a state that shifts the error token.  */
  for (;;)
    {
      yyn = yypact[yystate];
      if (!yypact_value_is_default (yyn))
        {
          yyn += YYSYMBOL_YYerror;
          if (0 <= yyn && yyn <= YYLAST && yycheck[yyn] == YYSYMBOL_YYerror)
            {
              yyn = yytable[yyn];
              if (0 < yyn)
                break;
            }
        }

      /* Pop the current state because it cannot handle the error token.  */
      if (yyssp == yyss)
        YYABORT;


      yydestruct ("Error: popping",
                  YY_ACCESSING_SYMBOL (yystate), yyvsp);
      YYPOPSTACK (1);
      yystate = *yyssp;
      YY_STACK_PRINT (yyss, yyssp);
    }

  YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
  *++yyvsp = yylval;
  YY_IGNORE_MAYBE_UNINITIALIZED_END


  /* Shift the error token.  */
  YY_SYMBOL_PRINT ("Shifting", YY_ACCESSING_SYMBOL (yyn), yyvsp, yylsp);

  yystate = yyn;
  goto yynewstate;


/*-------------------------------------.
| yyacceptlab -- YYACCEPT comes here.  |
`-------------------------------------*/
yyacceptlab:
  yyresult = 0;
  goto yyreturnlab;


/*-----------------------------------.
| yyabortlab -- YYABORT comes here.  |
`-----------------------------------*/
yyabortlab:
  yyresult = 1;
  goto yyreturnlab;


/*-----------------------------------------------------------.
| yyexhaustedlab -- YYNOMEM (memory exhaustion) comes here.  |
`-----------------------------------------------------------*/
yyexhaustedlab:
  yyerror (YY_("memory exhausted"));
  yyresult = 2;
  goto yyreturnlab;


/*----------------------------------------------------------.
| yyreturnlab -- parsing is finished, clean up and return.  |
`----------------------------------------------------------*/
yyreturnlab:
  if (yychar != YYEMPTY)
    {
      /* Make sure we have latest lookahead translation.  See comments at
         user semantic actions for why this is necessary.  */
      yytoken = YYTRANSLATE (yychar);
      yydestruct ("Cleanup: discarding lookahead",
                  yytoken, &yylval);
    }
  /* Do not reclaim the symbols of the rule whose action triggered
     this YYABORT or YYACCEPT.  */
  YYPOPSTACK (yylen);
  YY_STACK_PRINT (yyss, yyssp);
  while (yyssp != yyss)
    {
      yydestruct ("Cleanup: popping",
                  YY_ACCESSING_SYMBOL (+*yyssp), yyvsp);
      YYPOPSTACK (1);
    }
#ifndef yyoverflow
  if (yyss != yyssa)
    YYSTACK_FREE (yyss);
#endif

  return yyresult;
}

#line 308 "bdt.y"


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
