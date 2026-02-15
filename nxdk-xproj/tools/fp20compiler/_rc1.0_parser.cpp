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


/* Substitute the variable and function names.  */
#define yyparse         rc10_parse
#define yylex           rc10_lex
#define yyerror         rc10_error
#define yydebug         rc10_debug
#define yynerrs         rc10_nerrs
#define yylval          rc10_lval
#define yychar          rc10_char

/* First part of user prologue.  */
#line 2 "rc1.0_grammar.y"

void yyerror(const char* s);
int yylex ( void );

#ifdef _WIN32
# include <windows.h>
#endif

#include <stdio.h>
#include <stdlib.h>

#include "rc1.0_combiners.h"
#include "nvparse_errors.h"
#include "nvparse_externs.h"



#line 96 "_rc1.0_parser.cpp"

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

#include "_rc1.0_parser.hpp"
/* Symbol kind.  */
enum yysymbol_kind_t
{
  YYSYMBOL_YYEMPTY = -2,
  YYSYMBOL_YYEOF = 0,                      /* "end of file"  */
  YYSYMBOL_YYerror = 1,                    /* error  */
  YYSYMBOL_YYUNDEF = 2,                    /* "invalid token"  */
  YYSYMBOL_regVariable = 3,                /* regVariable  */
  YYSYMBOL_constVariable = 4,              /* constVariable  */
  YYSYMBOL_color_sum = 5,                  /* color_sum  */
  YYSYMBOL_final_product = 6,              /* final_product  */
  YYSYMBOL_expandString = 7,               /* expandString  */
  YYSYMBOL_halfBiasString = 8,             /* halfBiasString  */
  YYSYMBOL_unsignedString = 9,             /* unsignedString  */
  YYSYMBOL_unsignedInvertString = 10,      /* unsignedInvertString  */
  YYSYMBOL_muxString = 11,                 /* muxString  */
  YYSYMBOL_sumString = 12,                 /* sumString  */
  YYSYMBOL_rgb_portion = 13,               /* rgb_portion  */
  YYSYMBOL_alpha_portion = 14,             /* alpha_portion  */
  YYSYMBOL_openParen = 15,                 /* openParen  */
  YYSYMBOL_closeParen = 16,                /* closeParen  */
  YYSYMBOL_openBracket = 17,               /* openBracket  */
  YYSYMBOL_closeBracket = 18,              /* closeBracket  */
  YYSYMBOL_semicolon = 19,                 /* semicolon  */
  YYSYMBOL_comma = 20,                     /* comma  */
  YYSYMBOL_dot = 21,                       /* dot  */
  YYSYMBOL_times = 22,                     /* times  */
  YYSYMBOL_minus = 23,                     /* minus  */
  YYSYMBOL_equals = 24,                    /* equals  */
  YYSYMBOL_plus = 25,                      /* plus  */
  YYSYMBOL_bias_by_negative_one_half_scale_by_two = 26, /* bias_by_negative_one_half_scale_by_two  */
  YYSYMBOL_bias_by_negative_one_half = 27, /* bias_by_negative_one_half  */
  YYSYMBOL_scale_by_one_half = 28,         /* scale_by_one_half  */
  YYSYMBOL_scale_by_two = 29,              /* scale_by_two  */
  YYSYMBOL_scale_by_four = 30,             /* scale_by_four  */
  YYSYMBOL_clamp_color_sum = 31,           /* clamp_color_sum  */
  YYSYMBOL_lerp = 32,                      /* lerp  */
  YYSYMBOL_fragment_rgb = 33,              /* fragment_rgb  */
  YYSYMBOL_fragment_alpha = 34,            /* fragment_alpha  */
  YYSYMBOL_floatValue = 35,                /* floatValue  */
  YYSYMBOL_YYACCEPT = 36,                  /* $accept  */
  YYSYMBOL_WholeEnchilada = 37,            /* WholeEnchilada  */
  YYSYMBOL_Combiners = 38,                 /* Combiners  */
  YYSYMBOL_ConstColor = 39,                /* ConstColor  */
  YYSYMBOL_GeneralCombiners = 40,          /* GeneralCombiners  */
  YYSYMBOL_GeneralCombiner = 41,           /* GeneralCombiner  */
  YYSYMBOL_GeneralPortion = 42,            /* GeneralPortion  */
  YYSYMBOL_PortionDesignator = 43,         /* PortionDesignator  */
  YYSYMBOL_GeneralMappedRegister = 44,     /* GeneralMappedRegister  */
  YYSYMBOL_GeneralFunction = 45,           /* GeneralFunction  */
  YYSYMBOL_Dot = 46,                       /* Dot  */
  YYSYMBOL_Mul = 47,                       /* Mul  */
  YYSYMBOL_Mux = 48,                       /* Mux  */
  YYSYMBOL_Sum = 49,                       /* Sum  */
  YYSYMBOL_BiasScale = 50,                 /* BiasScale  */
  YYSYMBOL_FinalMappedRegister = 51,       /* FinalMappedRegister  */
  YYSYMBOL_FinalCombiner = 52,             /* FinalCombiner  */
  YYSYMBOL_ClampColorSum = 53,             /* ClampColorSum  */
  YYSYMBOL_FinalProduct = 54,              /* FinalProduct  */
  YYSYMBOL_FinalRgbFunction = 55,          /* FinalRgbFunction  */
  YYSYMBOL_FinalAlphaFunction = 56,        /* FinalAlphaFunction  */
  YYSYMBOL_Register = 57                   /* Register  */
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
typedef yytype_uint8 yy_state_t;

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
#define YYFINAL  27
/* YYLAST -- Last index in YYTABLE.  */
#define YYLAST   258

/* YYNTOKENS -- Number of terminals.  */
#define YYNTOKENS  36
/* YYNNTS -- Number of nonterminals.  */
#define YYNNTS  22
/* YYNRULES -- Number of rules.  */
#define YYNRULES  88
/* YYNSTATES -- Number of states.  */
#define YYNSTATES  220

/* YYMAXUTOK -- Last valid token kind.  */
#define YYMAXUTOK   290


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
      35
};

#if YYDEBUG
/* YYRLINE[YYN] -- Source line where rule number YYN was defined.  */
static const yytype_int16 yyrline[] =
{
       0,    72,    72,    79,    85,    91,    97,   105,   113,   123,
     132,   137,   145,   151,   157,   163,   169,   175,   183,   189,
     199,   203,   209,   215,   221,   227,   233,   239,   245,   251,
     259,   265,   271,   277,   283,   289,   295,   301,   309,   317,
     323,   335,   343,   351,   355,   359,   363,   367,   373,   379,
     385,   391,   397,   403,   409,   415,   421,   429,   435,   441,
     447,   453,   460,   466,   472,   478,   484,   491,   499,   507,
     515,   523,   532,   540,   548,   556,   564,   574,   580,   588,
     594,   600,   610,   622,   632,   646,   660,   668,   672
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
  "\"end of file\"", "error", "\"invalid token\"", "regVariable",
  "constVariable", "color_sum", "final_product", "expandString",
  "halfBiasString", "unsignedString", "unsignedInvertString", "muxString",
  "sumString", "rgb_portion", "alpha_portion", "openParen", "closeParen",
  "openBracket", "closeBracket", "semicolon", "comma", "dot", "times",
  "minus", "equals", "plus", "bias_by_negative_one_half_scale_by_two",
  "bias_by_negative_one_half", "scale_by_one_half", "scale_by_two",
  "scale_by_four", "clamp_color_sum", "lerp", "fragment_rgb",
  "fragment_alpha", "floatValue", "$accept", "WholeEnchilada", "Combiners",
  "ConstColor", "GeneralCombiners", "GeneralCombiner", "GeneralPortion",
  "PortionDesignator", "GeneralMappedRegister", "GeneralFunction", "Dot",
  "Mul", "Mux", "Sum", "BiasScale", "FinalMappedRegister", "FinalCombiner",
  "ClampColorSum", "FinalProduct", "FinalRgbFunction",
  "FinalAlphaFunction", "Register", YY_NULLPTR
};

static const char *
yysymbol_name (yysymbol_kind_t yysymbol)
{
  return yytname[yysymbol];
}
#endif

#define YYPACT_NINF (-63)

#define yypact_value_is_default(Yyn) \
  ((Yyn) == YYPACT_NINF)

#define YYTABLE_NINF (-1)

#define yytable_value_is_error(Yyn) \
  0

/* YYPACT[STATE-NUM] -- Index in YYTABLE of the portion describing
   STATE-NUM.  */
static const yytype_int16 yypact[] =
{
      48,   -13,    -8,    53,    16,    11,    14,    73,   -63,    48,
      94,   -63,   -63,    43,   147,    44,    47,    70,   144,   -63,
     -63,    53,    -5,    74,    78,    83,   144,   -63,    94,    94,
     -63,   -63,   -63,    17,    44,    47,    17,    44,    47,   -63,
     -63,    64,   -63,   -63,   -63,   -63,    97,   111,   107,   -63,
      58,   103,   -63,   117,    80,   132,   142,    21,   139,    94,
     -63,   -63,    44,    47,   -63,   -63,    44,    47,   -63,   -63,
     146,   167,   171,   144,   151,   -63,   149,   -63,   116,    80,
      80,   138,   -63,   144,   -63,   144,    92,   -63,   -63,   -63,
     -63,   -63,   -63,   133,   170,   173,   174,   175,   176,   177,
     160,   -63,   169,   -63,   -63,   179,   180,   181,   182,   183,
     184,   -63,   -63,   -63,    80,   129,   185,    28,   186,   187,
     188,   -63,   -63,   -63,   -63,   -63,   -63,   -63,   -63,   191,
     193,   194,   195,   196,   -63,   -63,   -63,   189,   199,   200,
     201,   202,   152,   163,   -63,   144,   -63,   144,   144,   -63,
     164,   203,   204,   205,   206,   207,    98,    80,    80,    80,
      80,   212,   213,   -63,   -63,   129,   129,   198,   210,   211,
     214,   -63,   -63,   -63,   -63,   -63,   215,   217,   219,   220,
     221,   222,    80,    80,   223,   224,   144,   -63,   144,   165,
     225,   228,   -63,   -63,   -63,   -63,   229,   230,   -63,   -63,
     231,   232,   233,   234,   235,   -63,   -63,    50,   144,   236,
     -63,   -63,   -63,   144,   240,   -63,   238,   239,   -63,   -63
};

/* YYDEFACT[STATE-NUM] -- Default reduction number in state STATE-NUM.
   Performed when YYTABLE does not specify something else to do.  Zero
   means the default is an error.  */
static const yytype_int8 yydefact[] =
{
       0,     0,     0,     0,     0,     0,     0,     0,     2,     0,
       0,    11,     8,     0,     0,    67,    72,     0,     0,    20,
      21,     0,     0,     0,     0,     0,     0,     1,     0,     0,
       6,    10,     5,     0,    68,    73,     0,    69,    74,    62,
      57,     0,    88,    87,    51,    54,     0,     0,     0,    48,
       0,     0,    15,     0,     0,     0,     0,     0,     0,     0,
       7,     3,    70,    75,    63,    58,    71,    76,    64,    59,
       0,     0,     0,     0,     0,    16,     0,    12,     0,    33,
      37,     0,    77,     0,    84,     0,     0,    86,     4,    65,
      60,    66,    61,     0,     0,     0,     0,     0,     0,     0,
       0,    17,     0,    13,    19,     0,     0,     0,     0,     0,
       0,    30,    31,    32,    34,     0,     0,     0,     0,     0,
       0,    52,    55,    49,    53,    56,    50,    78,    14,     0,
       0,     0,     0,     0,    18,    35,    36,     0,     0,     0,
       0,     0,     0,     0,    22,     0,    82,     0,     0,    85,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,    23,    40,     0,     0,     0,     0,     0,
       0,    43,    44,    45,    46,    47,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,    83,     0,     0,
       0,     0,    24,    26,    28,    29,     0,     0,    38,    39,
       0,     0,     0,     0,     0,    25,    27,     0,     0,     0,
      41,    42,    81,     0,     0,     9,     0,     0,    79,    80
};

/* YYPGOTO[NTERM-NUM].  */
static const yytype_int16 yypgoto[] =
{
     -63,   -63,   -63,     9,    -2,     0,   -18,   -63,   -62,   -63,
      39,    61,   -63,   -63,   -63,   -25,    -4,   190,   208,     1,
       8,   -52
};

/* YYDEFGOTO[NTERM-NUM].  */
static const yytype_uint8 yydefgoto[] =
{
       0,     7,     8,     9,    10,    11,    22,    23,   143,    78,
      79,    80,   135,   136,   110,    48,    12,    13,    14,    15,
      16,    49
};

/* YYTABLE[YYPACT[STATE-NUM]] -- What to do in state STATE-NUM.  If
   positive, shift that token.  If negative, reduce the rule whose
   number is the opposite.  If YYTABLE_NINF, syntax error.  */
static const yytype_uint8 yytable[] =
{
      57,    58,    81,    51,    53,    30,    32,    29,    19,    20,
      31,    17,    21,    52,    34,    37,    18,    40,    28,    96,
      99,    35,    38,    39,    60,    61,    59,    81,    81,    31,
      50,    24,    74,    76,    62,    25,    65,    66,    26,    69,
      84,    63,    64,    85,    67,    68,    86,   146,   100,     2,
       5,     6,     1,   147,     2,    88,   102,     1,   116,    31,
     117,   119,   137,   144,    90,     3,    19,    20,    92,   212,
      89,    19,    20,    27,    91,   213,     5,     6,     6,     4,
       5,     5,     6,    42,    43,    41,    42,    43,    44,    45,
     163,    54,    46,    47,    55,    42,    43,    44,    45,    70,
       2,    46,    47,   184,   185,   178,   179,   180,   181,   176,
     177,     3,    71,   144,   144,    56,    19,    20,   111,   113,
     167,    75,   168,   169,   118,     4,    72,     5,     6,    73,
     196,   197,    42,    43,   104,    77,   138,   139,   140,   141,
     112,   114,   105,   106,   107,   108,   109,    42,    43,    44,
      45,    82,   142,    46,    47,    42,    43,    83,    87,   161,
     162,   200,   115,   201,    19,    20,    93,   103,   120,   101,
      42,    43,    94,    95,    42,    43,    97,    98,     4,   127,
       5,     6,   164,   214,   165,   166,   121,   128,   216,   122,
     123,   124,   125,   126,   129,   130,   131,   132,   133,   170,
     202,   148,   134,     0,    36,   145,   149,   151,   150,   152,
     153,   154,   155,   156,   157,   158,   159,   160,   186,     0,
       0,    33,   171,   172,   173,   174,   175,   182,   183,   187,
     190,   188,   191,     0,   189,   192,   193,   194,   195,     0,
       0,   203,   198,   199,   204,   205,   206,   207,     0,   209,
       0,     0,   208,   210,   211,   215,   217,   218,   219
};

static const yytype_int16 yycheck[] =
{
      25,    26,    54,    21,    22,     9,    10,     9,    13,    14,
      10,    24,     3,    18,    13,    14,    24,    16,     9,    71,
      72,    13,    14,    15,    28,    29,    28,    79,    80,    29,
      21,    15,    50,    51,    33,    24,    35,    36,    24,    38,
      19,    33,    34,    22,    36,    37,    25,    19,    73,     6,
      33,    34,     4,    25,     6,    59,    74,     4,    83,    59,
      85,    86,   114,   115,    63,    17,    13,    14,    67,    19,
      62,    13,    14,     0,    66,    25,    33,    34,    34,    31,
      33,    33,    34,     3,     4,    15,     3,     4,     5,     6,
     142,    17,     9,    10,    16,     3,     4,     5,     6,    35,
       6,     9,    10,   165,   166,   157,   158,   159,   160,    11,
      12,    17,    15,   165,   166,    32,    13,    14,    79,    80,
     145,    18,   147,   148,    32,    31,    15,    33,    34,    22,
     182,   183,     3,     4,    18,    18,     7,     8,     9,    10,
      79,    80,    26,    27,    28,    29,    30,     3,     4,     5,
       6,    19,    23,     9,    10,     3,     4,    15,    19,     7,
       8,   186,    24,   188,    13,    14,    20,    18,    35,    18,
       3,     4,     5,     6,     3,     4,     5,     6,    31,    19,
      33,    34,    19,   208,    21,    22,    16,    18,   213,    16,
      16,    16,    16,    16,    15,    15,    15,    15,    15,    35,
      35,    15,    18,    -1,    14,    20,    19,    16,    20,    16,
      16,    16,    16,    24,    15,    15,    15,    15,    20,    -1,
      -1,    13,    19,    19,    19,    19,    19,    15,    15,    19,
      15,    20,    15,    -1,    20,    16,    16,    16,    16,    -1,
      -1,    16,    19,    19,    16,    16,    16,    16,    -1,    16,
      -1,    -1,    20,    19,    19,    19,    16,    19,    19
};

/* YYSTOS[STATE-NUM] -- The symbol kind of the accessing symbol of
   state STATE-NUM.  */
static const yytype_int8 yystos[] =
{
       0,     4,     6,    17,    31,    33,    34,    37,    38,    39,
      40,    41,    52,    53,    54,    55,    56,    24,    24,    13,
      14,    39,    42,    43,    15,    24,    24,     0,    39,    40,
      52,    41,    52,    54,    55,    56,    53,    55,    56,    56,
      55,    15,     3,     4,     5,     6,     9,    10,    51,    57,
      39,    42,    18,    42,    17,    16,    32,    51,    51,    40,
      52,    52,    55,    56,    56,    55,    55,    56,    56,    55,
      35,    15,    15,    22,    42,    18,    42,    18,    45,    46,
      47,    57,    19,    15,    19,    22,    25,    19,    52,    56,
      55,    56,    55,    20,     5,     6,    57,     5,     6,    57,
      51,    18,    42,    18,    18,    26,    27,    28,    29,    30,
      50,    46,    47,    46,    47,    24,    51,    51,    32,    51,
      35,    16,    16,    16,    16,    16,    16,    19,    18,    15,
      15,    15,    15,    15,    18,    48,    49,    57,     7,     8,
       9,    10,    23,    44,    57,    20,    19,    25,    15,    19,
      20,    16,    16,    16,    16,    16,    24,    15,    15,    15,
      15,     7,     8,    57,    19,    21,    22,    51,    51,    51,
      35,    19,    19,    19,    19,    19,    11,    12,    57,    57,
      57,    57,    15,    15,    44,    44,    20,    19,    20,    20,
      15,    15,    16,    16,    16,    16,    57,    57,    19,    19,
      51,    51,    35,    16,    16,    16,    16,    16,    20,    16,
      19,    19,    19,    25,    51,    19,    51,    16,    19,    19
};

/* YYR1[RULE-NUM] -- Symbol kind of the left-hand side of rule RULE-NUM.  */
static const yytype_int8 yyr1[] =
{
       0,    36,    37,    38,    38,    38,    38,    38,    38,    39,
      40,    40,    41,    41,    41,    41,    41,    41,    42,    42,
      43,    43,    44,    44,    44,    44,    44,    44,    44,    44,
      45,    45,    45,    45,    45,    45,    45,    45,    46,    47,
      47,    48,    49,    50,    50,    50,    50,    50,    51,    51,
      51,    51,    51,    51,    51,    51,    51,    52,    52,    52,
      52,    52,    52,    52,    52,    52,    52,    52,    52,    52,
      52,    52,    52,    52,    52,    52,    52,    53,    54,    55,
      55,    55,    55,    55,    55,    55,    56,    57,    57
};

/* YYR2[RULE-NUM] -- Number of symbols on the right-hand side of rule RULE-NUM.  */
static const yytype_int8 yyr2[] =
{
       0,     2,     1,     3,     4,     2,     2,     3,     1,    12,
       2,     1,     4,     5,     6,     3,     4,     5,     5,     4,
       1,     1,     1,     2,     4,     5,     4,     5,     4,     4,
       2,     2,     2,     1,     2,     3,     3,     1,     6,     6,
       4,     6,     6,     4,     4,     4,     4,     4,     1,     4,
       4,     1,     4,     4,     1,     4,     4,     2,     3,     3,
       4,     4,     2,     3,     3,     4,     4,     1,     2,     2,
       3,     3,     1,     2,     2,     3,     3,     4,     6,    13,
      13,    11,     6,     8,     4,     6,     4,     1,     1
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
  case 2: /* WholeEnchilada: Combiners  */
#line 73 "rc1.0_grammar.y"
                {
			(yyvsp[0].combinersStruct).Validate();
			(yyvsp[0].combinersStruct).Invoke();
		}
#line 1286 "_rc1.0_parser.cpp"
    break;

  case 3: /* Combiners: ConstColor GeneralCombiners FinalCombiner  */
#line 80 "rc1.0_grammar.y"
                {
			CombinersStruct combinersStruct;
			combinersStruct.Init((yyvsp[-1].generalCombinersStruct), (yyvsp[0].finalCombinerStruct), (yyvsp[-2].constColorStruct));
			(yyval.combinersStruct) = combinersStruct;
		}
#line 1296 "_rc1.0_parser.cpp"
    break;

  case 4: /* Combiners: ConstColor ConstColor GeneralCombiners FinalCombiner  */
#line 86 "rc1.0_grammar.y"
                {
			CombinersStruct combinersStruct;
			combinersStruct.Init((yyvsp[-1].generalCombinersStruct), (yyvsp[0].finalCombinerStruct), (yyvsp[-3].constColorStruct), (yyvsp[-2].constColorStruct));
			(yyval.combinersStruct) = combinersStruct;
		}
#line 1306 "_rc1.0_parser.cpp"
    break;

  case 5: /* Combiners: GeneralCombiners FinalCombiner  */
#line 92 "rc1.0_grammar.y"
                {
			CombinersStruct combinersStruct;
			combinersStruct.Init((yyvsp[-1].generalCombinersStruct), (yyvsp[0].finalCombinerStruct));
			(yyval.combinersStruct) = combinersStruct;
		}
#line 1316 "_rc1.0_parser.cpp"
    break;

  case 6: /* Combiners: ConstColor FinalCombiner  */
#line 98 "rc1.0_grammar.y"
                {
			GeneralCombinersStruct generalCombinersStruct;
			generalCombinersStruct.Init();
			CombinersStruct combinersStruct;
			combinersStruct.Init(generalCombinersStruct, (yyvsp[0].finalCombinerStruct), (yyvsp[-1].constColorStruct));
			(yyval.combinersStruct) = combinersStruct;
		}
#line 1328 "_rc1.0_parser.cpp"
    break;

  case 7: /* Combiners: ConstColor ConstColor FinalCombiner  */
#line 106 "rc1.0_grammar.y"
                {
			GeneralCombinersStruct generalCombinersStruct;
			generalCombinersStruct.Init();
			CombinersStruct combinersStruct;
			combinersStruct.Init(generalCombinersStruct, (yyvsp[0].finalCombinerStruct), (yyvsp[-2].constColorStruct), (yyvsp[-1].constColorStruct));
			(yyval.combinersStruct) = combinersStruct;
		}
#line 1340 "_rc1.0_parser.cpp"
    break;

  case 8: /* Combiners: FinalCombiner  */
#line 114 "rc1.0_grammar.y"
                {
			GeneralCombinersStruct generalCombinersStruct;
			generalCombinersStruct.Init();
			CombinersStruct combinersStruct;
			combinersStruct.Init(generalCombinersStruct, (yyvsp[0].finalCombinerStruct));
			(yyval.combinersStruct) = combinersStruct;
		}
#line 1352 "_rc1.0_parser.cpp"
    break;

  case 9: /* ConstColor: constVariable equals openParen floatValue comma floatValue comma floatValue comma floatValue closeParen semicolon  */
#line 124 "rc1.0_grammar.y"
                {
			ConstColorStruct constColorStruct;
			constColorStruct.Init((yyvsp[-11].registerEnum), (yyvsp[-8].fval), (yyvsp[-6].fval), (yyvsp[-4].fval), (yyvsp[-2].fval));
			(yyval.constColorStruct) = constColorStruct;
		}
#line 1362 "_rc1.0_parser.cpp"
    break;

  case 10: /* GeneralCombiners: GeneralCombiners GeneralCombiner  */
#line 133 "rc1.0_grammar.y"
                {
			(yyvsp[-1].generalCombinersStruct) += (yyvsp[0].generalCombinerStruct);
			(yyval.generalCombinersStruct) = (yyvsp[-1].generalCombinersStruct);
		}
#line 1371 "_rc1.0_parser.cpp"
    break;

  case 11: /* GeneralCombiners: GeneralCombiner  */
#line 138 "rc1.0_grammar.y"
                {
			GeneralCombinersStruct generalCombinersStruct;
			generalCombinersStruct.Init((yyvsp[0].generalCombinerStruct));
			(yyval.generalCombinersStruct) = generalCombinersStruct;
		}
#line 1381 "_rc1.0_parser.cpp"
    break;

  case 12: /* GeneralCombiner: openBracket GeneralPortion GeneralPortion closeBracket  */
#line 146 "rc1.0_grammar.y"
                {
			GeneralCombinerStruct generalCombinerStruct;
			generalCombinerStruct.Init((yyvsp[-2].generalPortionStruct), (yyvsp[-1].generalPortionStruct));
			(yyval.generalCombinerStruct) = generalCombinerStruct;
		}
#line 1391 "_rc1.0_parser.cpp"
    break;

  case 13: /* GeneralCombiner: openBracket ConstColor GeneralPortion GeneralPortion closeBracket  */
#line 152 "rc1.0_grammar.y"
                {
			GeneralCombinerStruct generalCombinerStruct;
			generalCombinerStruct.Init((yyvsp[-2].generalPortionStruct), (yyvsp[-1].generalPortionStruct), (yyvsp[-3].constColorStruct));
			(yyval.generalCombinerStruct) = generalCombinerStruct;
		}
#line 1401 "_rc1.0_parser.cpp"
    break;

  case 14: /* GeneralCombiner: openBracket ConstColor ConstColor GeneralPortion GeneralPortion closeBracket  */
#line 158 "rc1.0_grammar.y"
                {
			GeneralCombinerStruct generalCombinerStruct;
			generalCombinerStruct.Init((yyvsp[-2].generalPortionStruct), (yyvsp[-1].generalPortionStruct), (yyvsp[-4].constColorStruct), (yyvsp[-3].constColorStruct));
			(yyval.generalCombinerStruct) = generalCombinerStruct;
		}
#line 1411 "_rc1.0_parser.cpp"
    break;

  case 15: /* GeneralCombiner: openBracket GeneralPortion closeBracket  */
#line 164 "rc1.0_grammar.y"
                {
			GeneralCombinerStruct generalCombinerStruct;
			generalCombinerStruct.Init((yyvsp[-1].generalPortionStruct));
			(yyval.generalCombinerStruct) = generalCombinerStruct;
		}
#line 1421 "_rc1.0_parser.cpp"
    break;

  case 16: /* GeneralCombiner: openBracket ConstColor GeneralPortion closeBracket  */
#line 170 "rc1.0_grammar.y"
                {
			GeneralCombinerStruct generalCombinerStruct;
			generalCombinerStruct.Init((yyvsp[-1].generalPortionStruct), (yyvsp[-2].constColorStruct));
			(yyval.generalCombinerStruct) = generalCombinerStruct;
		}
#line 1431 "_rc1.0_parser.cpp"
    break;

  case 17: /* GeneralCombiner: openBracket ConstColor ConstColor GeneralPortion closeBracket  */
#line 176 "rc1.0_grammar.y"
                {
			GeneralCombinerStruct generalCombinerStruct;
			generalCombinerStruct.Init((yyvsp[-1].generalPortionStruct), (yyvsp[-3].constColorStruct), (yyvsp[-2].constColorStruct));
			(yyval.generalCombinerStruct) = generalCombinerStruct;
		}
#line 1441 "_rc1.0_parser.cpp"
    break;

  case 18: /* GeneralPortion: PortionDesignator openBracket GeneralFunction BiasScale closeBracket  */
#line 184 "rc1.0_grammar.y"
                {
			GeneralPortionStruct generalPortionStruct;
			generalPortionStruct.Init((yyvsp[-4].ival), (yyvsp[-2].generalFunctionStruct), (yyvsp[-1].biasScaleEnum));
			(yyval.generalPortionStruct) = generalPortionStruct;
		}
#line 1451 "_rc1.0_parser.cpp"
    break;

  case 19: /* GeneralPortion: PortionDesignator openBracket GeneralFunction closeBracket  */
#line 190 "rc1.0_grammar.y"
                {
			BiasScaleEnum noScale;
			noScale.word = RCP_SCALE_BY_ONE;
			GeneralPortionStruct generalPortionStruct;
			generalPortionStruct.Init((yyvsp[-3].ival), (yyvsp[-1].generalFunctionStruct), noScale);
			(yyval.generalPortionStruct) = generalPortionStruct;
		}
#line 1463 "_rc1.0_parser.cpp"
    break;

  case 20: /* PortionDesignator: rgb_portion  */
#line 200 "rc1.0_grammar.y"
                {
			(yyval.ival) = (yyvsp[0].ival);
		}
#line 1471 "_rc1.0_parser.cpp"
    break;

  case 21: /* PortionDesignator: alpha_portion  */
#line 204 "rc1.0_grammar.y"
                {
			(yyval.ival) = (yyvsp[0].ival);
		}
#line 1479 "_rc1.0_parser.cpp"
    break;

  case 22: /* GeneralMappedRegister: Register  */
#line 210 "rc1.0_grammar.y"
                {
			MappedRegisterStruct reg;
			reg.Init((yyvsp[0].registerEnum), MAP_SIGNED_IDENTITY);
			(yyval.mappedRegisterStruct) = reg;
		}
#line 1489 "_rc1.0_parser.cpp"
    break;

  case 23: /* GeneralMappedRegister: minus Register  */
#line 216 "rc1.0_grammar.y"
                {
			MappedRegisterStruct reg;
			reg.Init((yyvsp[0].registerEnum), MAP_SIGNED_NEGATE);
			(yyval.mappedRegisterStruct) = reg;
		}
#line 1499 "_rc1.0_parser.cpp"
    break;

  case 24: /* GeneralMappedRegister: expandString openParen Register closeParen  */
#line 222 "rc1.0_grammar.y"
                {
			MappedRegisterStruct reg;
			reg.Init((yyvsp[-1].registerEnum), MAP_EXPAND_NORMAL);
			(yyval.mappedRegisterStruct) = reg;
		}
#line 1509 "_rc1.0_parser.cpp"
    break;

  case 25: /* GeneralMappedRegister: minus expandString openParen Register closeParen  */
#line 228 "rc1.0_grammar.y"
                {
			MappedRegisterStruct reg;
			reg.Init((yyvsp[-1].registerEnum), MAP_EXPAND_NEGATE);
			(yyval.mappedRegisterStruct) = reg;
		}
#line 1519 "_rc1.0_parser.cpp"
    break;

  case 26: /* GeneralMappedRegister: halfBiasString openParen Register closeParen  */
#line 234 "rc1.0_grammar.y"
                {
			MappedRegisterStruct reg;
			reg.Init((yyvsp[-1].registerEnum), MAP_HALF_BIAS_NORMAL);
			(yyval.mappedRegisterStruct) = reg;
		}
#line 1529 "_rc1.0_parser.cpp"
    break;

  case 27: /* GeneralMappedRegister: minus halfBiasString openParen Register closeParen  */
#line 240 "rc1.0_grammar.y"
                {
			MappedRegisterStruct reg;
			reg.Init((yyvsp[-1].registerEnum), MAP_HALF_BIAS_NEGATE);
			(yyval.mappedRegisterStruct) = reg;
		}
#line 1539 "_rc1.0_parser.cpp"
    break;

  case 28: /* GeneralMappedRegister: unsignedString openParen Register closeParen  */
#line 246 "rc1.0_grammar.y"
                {
			MappedRegisterStruct reg;
			reg.Init((yyvsp[-1].registerEnum), MAP_UNSIGNED_IDENTITY);
			(yyval.mappedRegisterStruct) = reg;
		}
#line 1549 "_rc1.0_parser.cpp"
    break;

  case 29: /* GeneralMappedRegister: unsignedInvertString openParen Register closeParen  */
#line 252 "rc1.0_grammar.y"
                {
			MappedRegisterStruct reg;
			reg.Init((yyvsp[-1].registerEnum), MAP_UNSIGNED_INVERT);
			(yyval.mappedRegisterStruct) = reg;
		}
#line 1559 "_rc1.0_parser.cpp"
    break;

  case 30: /* GeneralFunction: Dot Dot  */
#line 260 "rc1.0_grammar.y"
                {
			GeneralFunctionStruct generalFunction;
			generalFunction.Init((yyvsp[-1].opStruct), (yyvsp[0].opStruct));
			(yyval.generalFunctionStruct) = generalFunction;
		}
#line 1569 "_rc1.0_parser.cpp"
    break;

  case 31: /* GeneralFunction: Dot Mul  */
#line 266 "rc1.0_grammar.y"
                {
			GeneralFunctionStruct generalFunction;
			generalFunction.Init((yyvsp[-1].opStruct), (yyvsp[0].opStruct));
			(yyval.generalFunctionStruct) = generalFunction;
		}
#line 1579 "_rc1.0_parser.cpp"
    break;

  case 32: /* GeneralFunction: Mul Dot  */
#line 272 "rc1.0_grammar.y"
                {
			GeneralFunctionStruct generalFunction;
			generalFunction.Init((yyvsp[-1].opStruct), (yyvsp[0].opStruct));
			(yyval.generalFunctionStruct) = generalFunction;
		}
#line 1589 "_rc1.0_parser.cpp"
    break;

  case 33: /* GeneralFunction: Dot  */
#line 278 "rc1.0_grammar.y"
                {
			GeneralFunctionStruct generalFunction;
			generalFunction.Init((yyvsp[0].opStruct));
			(yyval.generalFunctionStruct) = generalFunction;
		}
#line 1599 "_rc1.0_parser.cpp"
    break;

  case 34: /* GeneralFunction: Mul Mul  */
#line 284 "rc1.0_grammar.y"
                {
			GeneralFunctionStruct generalFunction;
			generalFunction.Init((yyvsp[-1].opStruct), (yyvsp[0].opStruct));
			(yyval.generalFunctionStruct) = generalFunction;
		}
#line 1609 "_rc1.0_parser.cpp"
    break;

  case 35: /* GeneralFunction: Mul Mul Mux  */
#line 290 "rc1.0_grammar.y"
                {
			GeneralFunctionStruct generalFunction;
			generalFunction.Init((yyvsp[-2].opStruct), (yyvsp[-1].opStruct), (yyvsp[0].opStruct));
			(yyval.generalFunctionStruct) = generalFunction;
		}
#line 1619 "_rc1.0_parser.cpp"
    break;

  case 36: /* GeneralFunction: Mul Mul Sum  */
#line 296 "rc1.0_grammar.y"
                {
			GeneralFunctionStruct generalFunction;
			generalFunction.Init((yyvsp[-2].opStruct), (yyvsp[-1].opStruct), (yyvsp[0].opStruct));
			(yyval.generalFunctionStruct) = generalFunction;
		}
#line 1629 "_rc1.0_parser.cpp"
    break;

  case 37: /* GeneralFunction: Mul  */
#line 302 "rc1.0_grammar.y"
                {
			GeneralFunctionStruct generalFunction;
			generalFunction.Init((yyvsp[0].opStruct));
			(yyval.generalFunctionStruct) = generalFunction;
		}
#line 1639 "_rc1.0_parser.cpp"
    break;

  case 38: /* Dot: Register equals GeneralMappedRegister dot GeneralMappedRegister semicolon  */
#line 310 "rc1.0_grammar.y"
                {
			OpStruct dotFunction;
			dotFunction.Init(RCP_DOT, (yyvsp[-5].registerEnum), (yyvsp[-3].mappedRegisterStruct), (yyvsp[-1].mappedRegisterStruct));
			(yyval.opStruct) = dotFunction;
		}
#line 1649 "_rc1.0_parser.cpp"
    break;

  case 39: /* Mul: Register equals GeneralMappedRegister times GeneralMappedRegister semicolon  */
#line 318 "rc1.0_grammar.y"
                {
			OpStruct mulFunction;
			mulFunction.Init(RCP_MUL, (yyvsp[-5].registerEnum), (yyvsp[-3].mappedRegisterStruct), (yyvsp[-1].mappedRegisterStruct));
			(yyval.opStruct) = mulFunction;
		}
#line 1659 "_rc1.0_parser.cpp"
    break;

  case 40: /* Mul: Register equals GeneralMappedRegister semicolon  */
#line 324 "rc1.0_grammar.y"
                {
			RegisterEnum zero;
			zero.word = RCP_ZERO;
			MappedRegisterStruct one;
			one.Init(zero, MAP_UNSIGNED_INVERT);
			OpStruct mulFunction;
			mulFunction.Init(RCP_MUL, (yyvsp[-3].registerEnum), (yyvsp[-1].mappedRegisterStruct), one);
			(yyval.opStruct) = mulFunction;
		}
#line 1673 "_rc1.0_parser.cpp"
    break;

  case 41: /* Mux: Register equals muxString openParen closeParen semicolon  */
#line 336 "rc1.0_grammar.y"
                {
			OpStruct muxFunction;
			muxFunction.Init(RCP_MUX, (yyvsp[-5].registerEnum));
			(yyval.opStruct) = muxFunction;
		}
#line 1683 "_rc1.0_parser.cpp"
    break;

  case 42: /* Sum: Register equals sumString openParen closeParen semicolon  */
#line 344 "rc1.0_grammar.y"
                {
			OpStruct sumFunction;
			sumFunction.Init(RCP_SUM, (yyvsp[-5].registerEnum));
			(yyval.opStruct) = sumFunction;
		}
#line 1693 "_rc1.0_parser.cpp"
    break;

  case 43: /* BiasScale: bias_by_negative_one_half_scale_by_two openParen closeParen semicolon  */
#line 352 "rc1.0_grammar.y"
                {
			(yyval.biasScaleEnum) = (yyvsp[-3].biasScaleEnum);
		}
#line 1701 "_rc1.0_parser.cpp"
    break;

  case 44: /* BiasScale: bias_by_negative_one_half openParen closeParen semicolon  */
#line 356 "rc1.0_grammar.y"
                {
			(yyval.biasScaleEnum) = (yyvsp[-3].biasScaleEnum);
		}
#line 1709 "_rc1.0_parser.cpp"
    break;

  case 45: /* BiasScale: scale_by_one_half openParen closeParen semicolon  */
#line 360 "rc1.0_grammar.y"
                {
			(yyval.biasScaleEnum) = (yyvsp[-3].biasScaleEnum);
		}
#line 1717 "_rc1.0_parser.cpp"
    break;

  case 46: /* BiasScale: scale_by_two openParen closeParen semicolon  */
#line 364 "rc1.0_grammar.y"
                {
			(yyval.biasScaleEnum) = (yyvsp[-3].biasScaleEnum);
		}
#line 1725 "_rc1.0_parser.cpp"
    break;

  case 47: /* BiasScale: scale_by_four openParen closeParen semicolon  */
#line 368 "rc1.0_grammar.y"
                {
			(yyval.biasScaleEnum) = (yyvsp[-3].biasScaleEnum);
		}
#line 1733 "_rc1.0_parser.cpp"
    break;

  case 48: /* FinalMappedRegister: Register  */
#line 374 "rc1.0_grammar.y"
                {
			MappedRegisterStruct reg;
			reg.Init((yyvsp[0].registerEnum), MAP_UNSIGNED_IDENTITY);
			(yyval.mappedRegisterStruct) = reg;
		}
#line 1743 "_rc1.0_parser.cpp"
    break;

  case 49: /* FinalMappedRegister: unsignedString openParen Register closeParen  */
#line 380 "rc1.0_grammar.y"
                {
			MappedRegisterStruct reg;
			reg.Init((yyvsp[-1].registerEnum), MAP_UNSIGNED_IDENTITY);
			(yyval.mappedRegisterStruct) = reg;
		}
#line 1753 "_rc1.0_parser.cpp"
    break;

  case 50: /* FinalMappedRegister: unsignedInvertString openParen Register closeParen  */
#line 386 "rc1.0_grammar.y"
                {
			MappedRegisterStruct reg;
			reg.Init((yyvsp[-1].registerEnum), MAP_UNSIGNED_INVERT);
			(yyval.mappedRegisterStruct) = reg;
		}
#line 1763 "_rc1.0_parser.cpp"
    break;

  case 51: /* FinalMappedRegister: color_sum  */
#line 392 "rc1.0_grammar.y"
                {
			MappedRegisterStruct reg;
			reg.Init((yyvsp[0].registerEnum));
			(yyval.mappedRegisterStruct) = reg;
		}
#line 1773 "_rc1.0_parser.cpp"
    break;

  case 52: /* FinalMappedRegister: unsignedString openParen color_sum closeParen  */
#line 398 "rc1.0_grammar.y"
                {
			MappedRegisterStruct reg;
			reg.Init((yyvsp[-1].registerEnum), MAP_UNSIGNED_IDENTITY);
			(yyval.mappedRegisterStruct) = reg;
		}
#line 1783 "_rc1.0_parser.cpp"
    break;

  case 53: /* FinalMappedRegister: unsignedInvertString openParen color_sum closeParen  */
#line 404 "rc1.0_grammar.y"
                {
			MappedRegisterStruct reg;
			reg.Init((yyvsp[-1].registerEnum), MAP_UNSIGNED_INVERT);
			(yyval.mappedRegisterStruct) = reg;
		}
#line 1793 "_rc1.0_parser.cpp"
    break;

  case 54: /* FinalMappedRegister: final_product  */
#line 410 "rc1.0_grammar.y"
                {
			MappedRegisterStruct reg;
			reg.Init((yyvsp[0].registerEnum));
			(yyval.mappedRegisterStruct) = reg;
		}
#line 1803 "_rc1.0_parser.cpp"
    break;

  case 55: /* FinalMappedRegister: unsignedString openParen final_product closeParen  */
#line 416 "rc1.0_grammar.y"
                {
			MappedRegisterStruct reg;
			reg.Init((yyvsp[-1].registerEnum), MAP_UNSIGNED_IDENTITY);
			(yyval.mappedRegisterStruct) = reg;
		}
#line 1813 "_rc1.0_parser.cpp"
    break;

  case 56: /* FinalMappedRegister: unsignedInvertString openParen final_product closeParen  */
#line 422 "rc1.0_grammar.y"
                {
			MappedRegisterStruct reg;
			reg.Init((yyvsp[-1].registerEnum), MAP_UNSIGNED_INVERT);
			(yyval.mappedRegisterStruct) = reg;
		}
#line 1823 "_rc1.0_parser.cpp"
    break;

  case 57: /* FinalCombiner: FinalAlphaFunction FinalRgbFunction  */
#line 430 "rc1.0_grammar.y"
                {
			FinalCombinerStruct finalCombinerStruct;
			finalCombinerStruct.Init((yyvsp[0].finalRgbFunctionStruct), (yyvsp[-1].finalAlphaFunctionStruct), false);
			(yyval.finalCombinerStruct) = finalCombinerStruct;
		}
#line 1833 "_rc1.0_parser.cpp"
    break;

  case 58: /* FinalCombiner: ClampColorSum FinalAlphaFunction FinalRgbFunction  */
#line 436 "rc1.0_grammar.y"
                {
			FinalCombinerStruct finalCombinerStruct;
			finalCombinerStruct.Init((yyvsp[0].finalRgbFunctionStruct), (yyvsp[-1].finalAlphaFunctionStruct), true);
			(yyval.finalCombinerStruct) = finalCombinerStruct;
		}
#line 1843 "_rc1.0_parser.cpp"
    break;

  case 59: /* FinalCombiner: FinalProduct FinalAlphaFunction FinalRgbFunction  */
#line 442 "rc1.0_grammar.y"
                {
			FinalCombinerStruct finalCombinerStruct;
			finalCombinerStruct.Init((yyvsp[0].finalRgbFunctionStruct), (yyvsp[-1].finalAlphaFunctionStruct), false, (yyvsp[-2].finalProductStruct));
			(yyval.finalCombinerStruct) = finalCombinerStruct;
		}
#line 1853 "_rc1.0_parser.cpp"
    break;

  case 60: /* FinalCombiner: ClampColorSum FinalProduct FinalAlphaFunction FinalRgbFunction  */
#line 448 "rc1.0_grammar.y"
                {
			FinalCombinerStruct finalCombinerStruct;
			finalCombinerStruct.Init((yyvsp[0].finalRgbFunctionStruct), (yyvsp[-1].finalAlphaFunctionStruct), true, (yyvsp[-2].finalProductStruct));
			(yyval.finalCombinerStruct) = finalCombinerStruct;
		}
#line 1863 "_rc1.0_parser.cpp"
    break;

  case 61: /* FinalCombiner: FinalProduct ClampColorSum FinalAlphaFunction FinalRgbFunction  */
#line 454 "rc1.0_grammar.y"
                {
			FinalCombinerStruct finalCombinerStruct;
			finalCombinerStruct.Init((yyvsp[0].finalRgbFunctionStruct), (yyvsp[-1].finalAlphaFunctionStruct), true, (yyvsp[-3].finalProductStruct));
			(yyval.finalCombinerStruct) = finalCombinerStruct;
		}
#line 1873 "_rc1.0_parser.cpp"
    break;

  case 62: /* FinalCombiner: FinalRgbFunction FinalAlphaFunction  */
#line 461 "rc1.0_grammar.y"
                {
			FinalCombinerStruct finalCombinerStruct;
			finalCombinerStruct.Init((yyvsp[-1].finalRgbFunctionStruct), (yyvsp[0].finalAlphaFunctionStruct), false);
			(yyval.finalCombinerStruct) = finalCombinerStruct;
		}
#line 1883 "_rc1.0_parser.cpp"
    break;

  case 63: /* FinalCombiner: ClampColorSum FinalRgbFunction FinalAlphaFunction  */
#line 467 "rc1.0_grammar.y"
                {
			FinalCombinerStruct finalCombinerStruct;
			finalCombinerStruct.Init((yyvsp[-1].finalRgbFunctionStruct), (yyvsp[0].finalAlphaFunctionStruct), true);
			(yyval.finalCombinerStruct) = finalCombinerStruct;
		}
#line 1893 "_rc1.0_parser.cpp"
    break;

  case 64: /* FinalCombiner: FinalProduct FinalRgbFunction FinalAlphaFunction  */
#line 473 "rc1.0_grammar.y"
                {
			FinalCombinerStruct finalCombinerStruct;
			finalCombinerStruct.Init((yyvsp[-1].finalRgbFunctionStruct), (yyvsp[0].finalAlphaFunctionStruct), false, (yyvsp[-2].finalProductStruct));
			(yyval.finalCombinerStruct) = finalCombinerStruct;
		}
#line 1903 "_rc1.0_parser.cpp"
    break;

  case 65: /* FinalCombiner: ClampColorSum FinalProduct FinalRgbFunction FinalAlphaFunction  */
#line 479 "rc1.0_grammar.y"
                {
			FinalCombinerStruct finalCombinerStruct;
			finalCombinerStruct.Init((yyvsp[-1].finalRgbFunctionStruct), (yyvsp[0].finalAlphaFunctionStruct), true, (yyvsp[-2].finalProductStruct));
			(yyval.finalCombinerStruct) = finalCombinerStruct;
		}
#line 1913 "_rc1.0_parser.cpp"
    break;

  case 66: /* FinalCombiner: FinalProduct ClampColorSum FinalRgbFunction FinalAlphaFunction  */
#line 485 "rc1.0_grammar.y"
                {
			FinalCombinerStruct finalCombinerStruct;
			finalCombinerStruct.Init((yyvsp[-1].finalRgbFunctionStruct), (yyvsp[0].finalAlphaFunctionStruct), true, (yyvsp[-3].finalProductStruct));
			(yyval.finalCombinerStruct) = finalCombinerStruct;
		}
#line 1923 "_rc1.0_parser.cpp"
    break;

  case 67: /* FinalCombiner: FinalRgbFunction  */
#line 492 "rc1.0_grammar.y"
                {
			FinalCombinerStruct finalCombinerStruct;
			FinalAlphaFunctionStruct finalAlphaFunctionStruct;
			finalAlphaFunctionStruct.ZeroOut();
			finalCombinerStruct.Init((yyvsp[0].finalRgbFunctionStruct), finalAlphaFunctionStruct, false);
			(yyval.finalCombinerStruct) = finalCombinerStruct;
		}
#line 1935 "_rc1.0_parser.cpp"
    break;

  case 68: /* FinalCombiner: ClampColorSum FinalRgbFunction  */
#line 500 "rc1.0_grammar.y"
                {
			FinalCombinerStruct finalCombinerStruct;
			FinalAlphaFunctionStruct finalAlphaFunctionStruct;
			finalAlphaFunctionStruct.ZeroOut();
			finalCombinerStruct.Init((yyvsp[0].finalRgbFunctionStruct), finalAlphaFunctionStruct, true);
			(yyval.finalCombinerStruct) = finalCombinerStruct;
		}
#line 1947 "_rc1.0_parser.cpp"
    break;

  case 69: /* FinalCombiner: FinalProduct FinalRgbFunction  */
#line 508 "rc1.0_grammar.y"
                {
			FinalCombinerStruct finalCombinerStruct;
			FinalAlphaFunctionStruct finalAlphaFunctionStruct;
			finalAlphaFunctionStruct.ZeroOut();
			finalCombinerStruct.Init((yyvsp[0].finalRgbFunctionStruct), finalAlphaFunctionStruct, false, (yyvsp[-1].finalProductStruct));
			(yyval.finalCombinerStruct) = finalCombinerStruct;
		}
#line 1959 "_rc1.0_parser.cpp"
    break;

  case 70: /* FinalCombiner: ClampColorSum FinalProduct FinalRgbFunction  */
#line 516 "rc1.0_grammar.y"
                {
			FinalCombinerStruct finalCombinerStruct;
			FinalAlphaFunctionStruct finalAlphaFunctionStruct;
			finalAlphaFunctionStruct.ZeroOut();
			finalCombinerStruct.Init((yyvsp[0].finalRgbFunctionStruct), finalAlphaFunctionStruct, true, (yyvsp[-1].finalProductStruct));
			(yyval.finalCombinerStruct) = finalCombinerStruct;
		}
#line 1971 "_rc1.0_parser.cpp"
    break;

  case 71: /* FinalCombiner: FinalProduct ClampColorSum FinalRgbFunction  */
#line 524 "rc1.0_grammar.y"
                {
			FinalCombinerStruct finalCombinerStruct;
			FinalAlphaFunctionStruct finalAlphaFunctionStruct;
			finalAlphaFunctionStruct.ZeroOut();
			finalCombinerStruct.Init((yyvsp[0].finalRgbFunctionStruct), finalAlphaFunctionStruct, true, (yyvsp[-2].finalProductStruct));
			(yyval.finalCombinerStruct) = finalCombinerStruct;
		}
#line 1983 "_rc1.0_parser.cpp"
    break;

  case 72: /* FinalCombiner: FinalAlphaFunction  */
#line 533 "rc1.0_grammar.y"
                {
			FinalCombinerStruct finalCombinerStruct;
			FinalRgbFunctionStruct finalRgbFunctionStruct;
			finalRgbFunctionStruct.ZeroOut();
			finalCombinerStruct.Init(finalRgbFunctionStruct, (yyvsp[0].finalAlphaFunctionStruct), false);
			(yyval.finalCombinerStruct) = finalCombinerStruct;
		}
#line 1995 "_rc1.0_parser.cpp"
    break;

  case 73: /* FinalCombiner: ClampColorSum FinalAlphaFunction  */
#line 541 "rc1.0_grammar.y"
                {
			FinalCombinerStruct finalCombinerStruct;
			FinalRgbFunctionStruct finalRgbFunctionStruct;
			finalRgbFunctionStruct.ZeroOut();
			finalCombinerStruct.Init(finalRgbFunctionStruct, (yyvsp[0].finalAlphaFunctionStruct), true);
			(yyval.finalCombinerStruct) = finalCombinerStruct;
		}
#line 2007 "_rc1.0_parser.cpp"
    break;

  case 74: /* FinalCombiner: FinalProduct FinalAlphaFunction  */
#line 549 "rc1.0_grammar.y"
                {
			FinalCombinerStruct finalCombinerStruct;
			FinalRgbFunctionStruct finalRgbFunctionStruct;
			finalRgbFunctionStruct.ZeroOut();
			finalCombinerStruct.Init(finalRgbFunctionStruct, (yyvsp[0].finalAlphaFunctionStruct), false, (yyvsp[-1].finalProductStruct));
			(yyval.finalCombinerStruct) = finalCombinerStruct;
		}
#line 2019 "_rc1.0_parser.cpp"
    break;

  case 75: /* FinalCombiner: ClampColorSum FinalProduct FinalAlphaFunction  */
#line 557 "rc1.0_grammar.y"
                {
			FinalCombinerStruct finalCombinerStruct;
			FinalRgbFunctionStruct finalRgbFunctionStruct;
			finalRgbFunctionStruct.ZeroOut();
			finalCombinerStruct.Init(finalRgbFunctionStruct, (yyvsp[0].finalAlphaFunctionStruct), true, (yyvsp[-1].finalProductStruct));
			(yyval.finalCombinerStruct) = finalCombinerStruct;
		}
#line 2031 "_rc1.0_parser.cpp"
    break;

  case 76: /* FinalCombiner: FinalProduct ClampColorSum FinalAlphaFunction  */
#line 565 "rc1.0_grammar.y"
                {
			FinalCombinerStruct finalCombinerStruct;
			FinalRgbFunctionStruct finalRgbFunctionStruct;
			finalRgbFunctionStruct.ZeroOut();
			finalCombinerStruct.Init(finalRgbFunctionStruct, (yyvsp[0].finalAlphaFunctionStruct), true, (yyvsp[-2].finalProductStruct));
			(yyval.finalCombinerStruct) = finalCombinerStruct;
		}
#line 2043 "_rc1.0_parser.cpp"
    break;

  case 77: /* ClampColorSum: clamp_color_sum openParen closeParen semicolon  */
#line 575 "rc1.0_grammar.y"
                {
			(yyval.ival) = (yyvsp[-3].ival);
		}
#line 2051 "_rc1.0_parser.cpp"
    break;

  case 78: /* FinalProduct: final_product equals FinalMappedRegister times FinalMappedRegister semicolon  */
#line 581 "rc1.0_grammar.y"
                {
			FinalProductStruct finalProductStruct;
			finalProductStruct.Init((yyvsp[-3].mappedRegisterStruct), (yyvsp[-1].mappedRegisterStruct));
			(yyval.finalProductStruct) = finalProductStruct;
		}
#line 2061 "_rc1.0_parser.cpp"
    break;

  case 79: /* FinalRgbFunction: fragment_rgb equals lerp openParen FinalMappedRegister comma FinalMappedRegister comma FinalMappedRegister closeParen plus FinalMappedRegister semicolon  */
#line 589 "rc1.0_grammar.y"
                {
			FinalRgbFunctionStruct finalRgbFunctionStruct;
			finalRgbFunctionStruct.Init((yyvsp[-8].mappedRegisterStruct), (yyvsp[-6].mappedRegisterStruct), (yyvsp[-4].mappedRegisterStruct), (yyvsp[-1].mappedRegisterStruct));
			(yyval.finalRgbFunctionStruct) = finalRgbFunctionStruct;
		}
#line 2071 "_rc1.0_parser.cpp"
    break;

  case 80: /* FinalRgbFunction: fragment_rgb equals FinalMappedRegister plus lerp openParen FinalMappedRegister comma FinalMappedRegister comma FinalMappedRegister closeParen semicolon  */
#line 595 "rc1.0_grammar.y"
                {
			FinalRgbFunctionStruct finalRgbFunctionStruct;
			finalRgbFunctionStruct.Init((yyvsp[-6].mappedRegisterStruct), (yyvsp[-4].mappedRegisterStruct), (yyvsp[-2].mappedRegisterStruct), (yyvsp[-10].mappedRegisterStruct));
			(yyval.finalRgbFunctionStruct) = finalRgbFunctionStruct;
		}
#line 2081 "_rc1.0_parser.cpp"
    break;

  case 81: /* FinalRgbFunction: fragment_rgb equals lerp openParen FinalMappedRegister comma FinalMappedRegister comma FinalMappedRegister closeParen semicolon  */
#line 601 "rc1.0_grammar.y"
                {
			RegisterEnum zero;
			zero.word = RCP_ZERO;
			MappedRegisterStruct reg;
			reg.Init(zero, MAP_UNSIGNED_IDENTITY);
			FinalRgbFunctionStruct finalRgbFunctionStruct;
			finalRgbFunctionStruct.Init((yyvsp[-6].mappedRegisterStruct), (yyvsp[-4].mappedRegisterStruct), (yyvsp[-2].mappedRegisterStruct), reg);
			(yyval.finalRgbFunctionStruct) = finalRgbFunctionStruct;
		}
#line 2095 "_rc1.0_parser.cpp"
    break;

  case 82: /* FinalRgbFunction: fragment_rgb equals FinalMappedRegister times FinalMappedRegister semicolon  */
#line 611 "rc1.0_grammar.y"
                {
			RegisterEnum zero;
			zero.word = RCP_ZERO;
			MappedRegisterStruct reg1;
			reg1.Init(zero, MAP_UNSIGNED_IDENTITY);
			MappedRegisterStruct reg2;
			reg2.Init(zero, MAP_UNSIGNED_IDENTITY);
			FinalRgbFunctionStruct finalRgbFunctionStruct;
			finalRgbFunctionStruct.Init((yyvsp[-3].mappedRegisterStruct), (yyvsp[-1].mappedRegisterStruct), reg1, reg2);
			(yyval.finalRgbFunctionStruct) = finalRgbFunctionStruct;
		}
#line 2111 "_rc1.0_parser.cpp"
    break;

  case 83: /* FinalRgbFunction: fragment_rgb equals FinalMappedRegister times FinalMappedRegister plus FinalMappedRegister semicolon  */
#line 623 "rc1.0_grammar.y"
                {
			RegisterEnum zero;
			zero.word = RCP_ZERO;
			MappedRegisterStruct reg1;
			reg1.Init(zero, MAP_UNSIGNED_IDENTITY);
			FinalRgbFunctionStruct finalRgbFunctionStruct;
			finalRgbFunctionStruct.Init((yyvsp[-5].mappedRegisterStruct), (yyvsp[-3].mappedRegisterStruct), reg1, (yyvsp[-1].mappedRegisterStruct));
			(yyval.finalRgbFunctionStruct) = finalRgbFunctionStruct;
		}
#line 2125 "_rc1.0_parser.cpp"
    break;

  case 84: /* FinalRgbFunction: fragment_rgb equals FinalMappedRegister semicolon  */
#line 633 "rc1.0_grammar.y"
                {
			RegisterEnum zero;
			zero.word = RCP_ZERO;
			MappedRegisterStruct reg1;
			reg1.Init(zero, MAP_UNSIGNED_IDENTITY);
			MappedRegisterStruct reg2;
			reg2.Init(zero, MAP_UNSIGNED_IDENTITY);
			MappedRegisterStruct reg3;
			reg3.Init(zero, MAP_UNSIGNED_IDENTITY);
			FinalRgbFunctionStruct finalRgbFunctionStruct;
			finalRgbFunctionStruct.Init(reg1, reg2, reg3, (yyvsp[-1].mappedRegisterStruct));
			(yyval.finalRgbFunctionStruct) = finalRgbFunctionStruct;
		}
#line 2143 "_rc1.0_parser.cpp"
    break;

  case 85: /* FinalRgbFunction: fragment_rgb equals FinalMappedRegister plus FinalMappedRegister semicolon  */
#line 647 "rc1.0_grammar.y"
                {
			RegisterEnum zero;
			zero.word = RCP_ZERO;
			MappedRegisterStruct reg2;
			reg2.Init(zero, MAP_UNSIGNED_INVERT);
			MappedRegisterStruct reg3;
			reg3.Init(zero, MAP_UNSIGNED_IDENTITY);
			FinalRgbFunctionStruct finalRgbFunctionStruct;
			finalRgbFunctionStruct.Init((yyvsp[-3].mappedRegisterStruct), reg2, reg3, (yyvsp[-1].mappedRegisterStruct));
			(yyval.finalRgbFunctionStruct) = finalRgbFunctionStruct;
		}
#line 2159 "_rc1.0_parser.cpp"
    break;

  case 86: /* FinalAlphaFunction: fragment_alpha equals FinalMappedRegister semicolon  */
#line 661 "rc1.0_grammar.y"
                {
			FinalAlphaFunctionStruct finalAlphaFunctionStruct;
			finalAlphaFunctionStruct.Init((yyvsp[-1].mappedRegisterStruct));
			(yyval.finalAlphaFunctionStruct) = finalAlphaFunctionStruct;
		}
#line 2169 "_rc1.0_parser.cpp"
    break;

  case 87: /* Register: constVariable  */
#line 669 "rc1.0_grammar.y"
                {
			(yyval.registerEnum) = (yyvsp[0].registerEnum);
		}
#line 2177 "_rc1.0_parser.cpp"
    break;

  case 88: /* Register: regVariable  */
#line 673 "rc1.0_grammar.y"
                {
			(yyval.registerEnum) = (yyvsp[0].registerEnum);
		}
#line 2185 "_rc1.0_parser.cpp"
    break;


#line 2189 "_rc1.0_parser.cpp"

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

#line 678 "rc1.0_grammar.y"

void yyerror(const char* s)
{
     errors.set("syntax error", line_number);
}
