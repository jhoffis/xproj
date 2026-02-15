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

#ifndef YY_RC10_RC1_0_PARSER_HPP_INCLUDED
# define YY_RC10_RC1_0_PARSER_HPP_INCLUDED
/* Debug traces.  */
#ifndef YYDEBUG
# define YYDEBUG 0
#endif
#if YYDEBUG
extern int rc10_debug;
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
    regVariable = 258,             /* regVariable  */
    constVariable = 259,           /* constVariable  */
    color_sum = 260,               /* color_sum  */
    final_product = 261,           /* final_product  */
    expandString = 262,            /* expandString  */
    halfBiasString = 263,          /* halfBiasString  */
    unsignedString = 264,          /* unsignedString  */
    unsignedInvertString = 265,    /* unsignedInvertString  */
    muxString = 266,               /* muxString  */
    sumString = 267,               /* sumString  */
    rgb_portion = 268,             /* rgb_portion  */
    alpha_portion = 269,           /* alpha_portion  */
    openParen = 270,               /* openParen  */
    closeParen = 271,              /* closeParen  */
    openBracket = 272,             /* openBracket  */
    closeBracket = 273,            /* closeBracket  */
    semicolon = 274,               /* semicolon  */
    comma = 275,                   /* comma  */
    dot = 276,                     /* dot  */
    times = 277,                   /* times  */
    minus = 278,                   /* minus  */
    equals = 279,                  /* equals  */
    plus = 280,                    /* plus  */
    bias_by_negative_one_half_scale_by_two = 281, /* bias_by_negative_one_half_scale_by_two  */
    bias_by_negative_one_half = 282, /* bias_by_negative_one_half  */
    scale_by_one_half = 283,       /* scale_by_one_half  */
    scale_by_two = 284,            /* scale_by_two  */
    scale_by_four = 285,           /* scale_by_four  */
    clamp_color_sum = 286,         /* clamp_color_sum  */
    lerp = 287,                    /* lerp  */
    fragment_rgb = 288,            /* fragment_rgb  */
    fragment_alpha = 289,          /* fragment_alpha  */
    floatValue = 290               /* floatValue  */
  };
  typedef enum yytokentype yytoken_kind_t;
#endif

/* Value type.  */
#if ! defined YYSTYPE && ! defined YYSTYPE_IS_DECLARED
union YYSTYPE
{
#line 19 "rc1.0_grammar.y"

  int ival;
  float fval;
  RegisterEnum registerEnum;
  BiasScaleEnum biasScaleEnum;
  MappedRegisterStruct mappedRegisterStruct;
  ConstColorStruct constColorStruct;
  GeneralPortionStruct generalPortionStruct;
  GeneralFunctionStruct generalFunctionStruct;
  OpStruct opStruct;
  GeneralCombinerStruct generalCombinerStruct;
  GeneralCombinersStruct generalCombinersStruct;
  FinalProductStruct finalProductStruct;
  FinalRgbFunctionStruct finalRgbFunctionStruct;
  FinalAlphaFunctionStruct finalAlphaFunctionStruct;
  FinalCombinerStruct finalCombinerStruct;
  CombinersStruct combinersStruct;

#line 118 "_rc1.0_parser.hpp"

};
typedef union YYSTYPE YYSTYPE;
# define YYSTYPE_IS_TRIVIAL 1
# define YYSTYPE_IS_DECLARED 1
#endif


extern YYSTYPE rc10_lval;


int rc10_parse (void);


#endif /* !YY_RC10_RC1_0_PARSER_HPP_INCLUDED  */
