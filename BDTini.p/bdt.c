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

#line 192 "bdt.c"

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
  YYSYMBOL_TK_TABLA = 3,                   /* TK_TABLA  */
  YYSYMBOL_TK_INICIAR = 4,                 /* TK_INICIAR  */
  YYSYMBOL_TK_SEPARADOR = 5,               /* TK_SEPARADOR  */
  YYSYMBOL_TK_CARACTER = 6,                /* TK_CARACTER  */
  YYSYMBOL_TK_PALABRA = 7,                 /* TK_PALABRA  */
  YYSYMBOL_TK_STRING = 8,                  /* TK_STRING  */
  YYSYMBOL_TK_ARRAY = 9,                   /* TK_ARRAY  */
  YYSYMBOL_TK_NUM_NATURAL = 10,            /* TK_NUM_NATURAL  */
  YYSYMBOL_TK_NUM_ENTERO = 11,             /* TK_NUM_ENTERO  */
  YYSYMBOL_TK_NUM_REAL = 12,               /* TK_NUM_REAL  */
  YYSYMBOL_13_ = 13,                       /* '('  */
  YYSYMBOL_14_ = 14,                       /* ')'  */
  YYSYMBOL_15_ = 15,                       /* ','  */
  YYSYMBOL_16_ = 16,                       /* '['  */
  YYSYMBOL_17_ = 17,                       /* ']'  */
  YYSYMBOL_18_ = 18,                       /* '='  */
  YYSYMBOL_YYACCEPT = 19,                  /* $accept  */
  YYSYMBOL_DatosTablas = 20,               /* DatosTablas  */
  YYSYMBOL_Tabla = 21,                     /* Tabla  */
  YYSYMBOL_Iniciar = 22,                   /* Iniciar  */
  YYSYMBOL_NombreTabla = 23,               /* NombreTabla  */
  YYSYMBOL_Registros = 24,                 /* Registros  */
  YYSYMBOL_Operacion = 25,                 /* Operacion  */
  YYSYMBOL_Registro = 26,                  /* Registro  */
  YYSYMBOL_DatosPorOrden = 27,             /* DatosPorOrden  */
  YYSYMBOL_ListaValores = 28,              /* ListaValores  */
  YYSYMBOL_DatosPorCampos = 29,            /* DatosPorCampos  */
  YYSYMBOL_ListaCampos = 30,               /* ListaCampos  */
  YYSYMBOL_UnCampo = 31,                   /* UnCampo  */
  YYSYMBOL_NombreCampo = 32,               /* NombreCampo  */
  YYSYMBOL_ValorCampo = 33,                /* ValorCampo  */
  YYSYMBOL_Palabra = 34                    /* Palabra  */
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
#define YYFINAL  2
/* YYLAST -- Last index in YYTABLE.  */
#define YYLAST   27

/* YYNTOKENS -- Number of terminals.  */
#define YYNTOKENS  19
/* YYNNTS -- Number of nonterminals.  */
#define YYNNTS  16
/* YYNRULES -- Number of rules.  */
#define YYNRULES  28
/* YYNSTATES -- Number of states.  */
#define YYNSTATES  39

/* YYMAXUTOK -- Last valid token kind.  */
#define YYMAXUTOK   267


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
      13,    14,     2,     2,    15,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,    18,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,    16,     2,    17,     2,     2,     2,     2,     2,     2,
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
       5,     6,     7,     8,     9,    10,    11,    12
};

#if YYDEBUG
/* YYRLINE[YYN] -- Source line where rule number YYN was defined.  */
static const yytype_uint8 yyrline[] =
{
       0,   154,   154,   155,   158,   161,   162,   165,   168,   169,
     172,   175,   176,   179,   182,   183,   186,   189,   190,   193,
     196,   199,   201,   204,   207,   210,   212,   214,   218
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
  "\"end of file\"", "error", "\"invalid token\"", "TK_TABLA",
  "TK_INICIAR", "TK_SEPARADOR", "TK_CARACTER", "TK_PALABRA", "TK_STRING",
  "TK_ARRAY", "TK_NUM_NATURAL", "TK_NUM_ENTERO", "TK_NUM_REAL", "'('",
  "')'", "','", "'['", "']'", "'='", "$accept", "DatosTablas", "Tabla",
  "Iniciar", "NombreTabla", "Registros", "Operacion", "Registro",
  "DatosPorOrden", "ListaValores", "DatosPorCampos", "ListaCampos",
  "UnCampo", "NombreCampo", "ValorCampo", "Palabra", YY_NULLPTR
};

static const char *
yysymbol_name (yysymbol_kind_t yysymbol)
{
  return yytname[yysymbol];
}
#endif

#define YYPACT_NINF (-17)

#define yypact_value_is_default(Yyn) \
  ((Yyn) == YYPACT_NINF)

#define YYTABLE_NINF (-1)

#define yytable_value_is_error(Yyn) \
  0

/* YYPACT[STATE-NUM] -- Index in YYTABLE of the portion describing
   STATE-NUM.  */
static const yytype_int8 yypact[] =
{
     -17,     1,   -17,   -17,   -17,    -1,     0,   -17,   -17,   -17,
       0,   -10,   -17,     3,     0,   -17,   -17,   -17,   -17,   -17,
     -17,   -17,   -17,   -17,     9,   -17,   -17,     5,   -17,     7,
     -17,   -17,     3,     0,   -17,     3,   -17,   -17,   -17
};

/* YYDEFACT[STATE-NUM] -- Default reduction number in state STATE-NUM.
   Performed when YYTABLE does not specify something else to do.  Zero
   means the default is an error.  */
static const yytype_int8 yydefact[] =
{
       3,     6,     1,     5,     2,     0,     0,     9,    28,     7,
       4,     0,    10,     0,     0,     8,    11,    12,    21,    23,
      24,    25,    26,    27,     0,    15,    22,     0,    18,     0,
      20,    13,     0,     0,    16,     0,    14,    17,    19
};

/* YYPGOTO[NTERM-NUM].  */
static const yytype_int8 yypgoto[] =
{
     -17,   -17,   -17,   -17,   -17,   -17,   -17,   -17,   -17,   -17,
     -17,   -17,   -15,   -17,   -16,    -6
};

/* YYDEFGOTO[NTERM-NUM].  */
static const yytype_int8 yydefgoto[] =
{
       0,     1,     4,     5,     7,    10,    11,    15,    16,    24,
      17,    27,    28,    29,    25,    26
};

/* YYTABLE[YYPACT[STATE-NUM]] -- What to do in state STATE-NUM.  If
   positive, shift that token.  If negative, reduce the rule whose
   number is the opposite.  If YYTABLE_NINF, syntax error.  */
static const yytype_int8 yytable[] =
{
       9,     2,     6,    13,    12,     3,    14,     8,    30,    18,
       8,    19,    20,    21,    22,    23,    36,     0,    37,    38,
      33,     0,    34,    31,    32,    35,     0,    30
};

static const yytype_int8 yycheck[] =
{
       6,     0,     3,    13,    10,     4,    16,     7,    14,     6,
       7,     8,     9,    10,    11,    12,    32,    -1,    33,    35,
      15,    -1,    17,    14,    15,    18,    -1,    33
};

/* YYSTOS[STATE-NUM] -- The symbol kind of the accessing symbol of
   state STATE-NUM.  */
static const yytype_int8 yystos[] =
{
       0,    20,     0,     4,    21,    22,     3,    23,     7,    34,
      24,    25,    34,    13,    16,    26,    27,    29,     6,     8,
       9,    10,    11,    12,    28,    33,    34,    30,    31,    32,
      34,    14,    15,    15,    17,    18,    33,    31,    33
};

/* YYR1[RULE-NUM] -- Symbol kind of the left-hand side of rule RULE-NUM.  */
static const yytype_int8 yyr1[] =
{
       0,    19,    20,    20,    21,    22,    22,    23,    24,    24,
      25,    26,    26,    27,    28,    28,    29,    30,    30,    31,
      32,    33,    33,    33,    33,    33,    33,    33,    34
};

/* YYR2[RULE-NUM] -- Number of symbols on the right-hand side of rule RULE-NUM.  */
static const yytype_int8 yyr2[] =
{
       0,     2,     2,     0,     3,     1,     0,     2,     3,     0,
       1,     1,     1,     3,     3,     1,     3,     3,     1,     3,
       1,     1,     1,     1,     1,     1,     1,     1,     1
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
  case 5: /* Iniciar: TK_INICIAR  */
#line 161 "bdt.y"
                                                { IniciarTablas();	}
#line 1226 "bdt.c"
    break;

  case 7: /* NombreTabla: TK_TABLA Palabra  */
#line 165 "bdt.y"
                                                { NuevaTabla((yyvsp[0].palabra));	}
#line 1232 "bdt.c"
    break;

  case 8: /* Registros: Registros Operacion Registro  */
#line 168 "bdt.y"
                                                { RealizarOperacion();	}
#line 1238 "bdt.c"
    break;

  case 10: /* Operacion: Palabra  */
#line 172 "bdt.y"
                                                { Operacion((yyvsp[0].palabra));	}
#line 1244 "bdt.c"
    break;

  case 14: /* ListaValores: ListaValores ',' ValorCampo  */
#line 182 "bdt.y"
                                                { AsignarValorACampo();	}
#line 1250 "bdt.c"
    break;

  case 15: /* ListaValores: ValorCampo  */
#line 183 "bdt.y"
                                                { AsignarValorACampo();	}
#line 1256 "bdt.c"
    break;

  case 19: /* UnCampo: NombreCampo '=' ValorCampo  */
#line 193 "bdt.y"
                                                { AsignarValorACampo();	}
#line 1262 "bdt.c"
    break;

  case 20: /* NombreCampo: Palabra  */
#line 196 "bdt.y"
                                                { NuevoCampo((yyvsp[0].palabra));	}
#line 1268 "bdt.c"
    break;

  case 21: /* ValorCampo: TK_CARACTER  */
#line 199 "bdt.y"
                                        { tipoValor = VAL_CARACTER;
					  caracter  = yylval.caracter;	}
#line 1275 "bdt.c"
    break;

  case 22: /* ValorCampo: Palabra  */
#line 201 "bdt.y"
                                        { tipoValor = VAL_PALABRA;
					  palabra   = (yyvsp[0].palabra);
					  lonStr    = strlen((yyvsp[0].palabra)) + 1;	}
#line 1283 "bdt.c"
    break;

  case 23: /* ValorCampo: TK_STRING  */
#line 204 "bdt.y"
                                        { tipoValor = VAL_STRING;
					  string    = yylval.str.txt;
					  lonStr    = yylval.str.lon;	}
#line 1291 "bdt.c"
    break;

  case 24: /* ValorCampo: TK_ARRAY  */
#line 207 "bdt.y"
                                        { tipoValor = VAL_ARRAY;
					  string    = yylval.str.txt;
					  lonStr    = yylval.str.lon;	}
#line 1299 "bdt.c"
    break;

  case 25: /* ValorCampo: TK_NUM_NATURAL  */
#line 210 "bdt.y"
                                        { tipoValor = VAL_NUM_NATURAL;
					  natural   = yylval.natural;	}
#line 1306 "bdt.c"
    break;

  case 26: /* ValorCampo: TK_NUM_ENTERO  */
#line 212 "bdt.y"
                                        { tipoValor = VAL_NUM_ENTERO;
					  entero    = yylval.entero;	}
#line 1313 "bdt.c"
    break;

  case 27: /* ValorCampo: TK_NUM_REAL  */
#line 214 "bdt.y"
                                        { tipoValor = VAL_NUM_REAL;
					  real	    = yylval.real;	}
#line 1320 "bdt.c"
    break;

  case 28: /* Palabra: TK_PALABRA  */
#line 218 "bdt.y"
                                        { (yyval.palabra) = yylval.palabra;		}
#line 1326 "bdt.c"
    break;


#line 1330 "bdt.c"

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

#line 220 "bdt.y"


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
