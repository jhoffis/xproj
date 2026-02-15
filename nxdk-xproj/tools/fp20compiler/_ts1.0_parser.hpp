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

#ifndef YY_TS10_TS1_0_PARSER_HPP_INCLUDED
# define YY_TS10_TS1_0_PARSER_HPP_INCLUDED
/* Debug traces.  */
#ifndef YYDEBUG
# define YYDEBUG 0
#endif
#if YYDEBUG
extern int ts10_debug;
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
    floatValue = 258,              /* floatValue  */
    gequal = 259,                  /* gequal  */
    less = 260,                    /* less  */
    texVariable = 261,             /* texVariable  */
    expandString = 262,            /* expandString  */
    openParen = 263,               /* openParen  */
    closeParen = 264,              /* closeParen  */
    semicolon = 265,               /* semicolon  */
    comma = 266,                   /* comma  */
    nop = 267,                     /* nop  */
    texture_1d = 268,              /* texture_1d  */
    texture_2d = 269,              /* texture_2d  */
    texture_rectangle = 270,       /* texture_rectangle  */
    texture_3d = 271,              /* texture_3d  */
    texture_cube_map = 272,        /* texture_cube_map  */
    cull_fragment = 273,           /* cull_fragment  */
    pass_through = 274,            /* pass_through  */
    offset_2d_scale = 275,         /* offset_2d_scale  */
    offset_2d = 276,               /* offset_2d  */
    offset_rectangle_scale = 277,  /* offset_rectangle_scale  */
    offset_rectangle = 278,        /* offset_rectangle  */
    dependent_ar = 279,            /* dependent_ar  */
    dependent_gb = 280,            /* dependent_gb  */
    dot_product_2d_1of2 = 281,     /* dot_product_2d_1of2  */
    dot_product_2d_2of2 = 282,     /* dot_product_2d_2of2  */
    dot_product_rectangle_1of2 = 283, /* dot_product_rectangle_1of2  */
    dot_product_rectangle_2of2 = 284, /* dot_product_rectangle_2of2  */
    dot_product_depth_replace_1of2 = 285, /* dot_product_depth_replace_1of2  */
    dot_product_depth_replace_2of2 = 286, /* dot_product_depth_replace_2of2  */
    dot_product_3d_1of3 = 287,     /* dot_product_3d_1of3  */
    dot_product_3d_2of3 = 288,     /* dot_product_3d_2of3  */
    dot_product_3d_3of3 = 289,     /* dot_product_3d_3of3  */
    dot_product_cube_map_1of3 = 290, /* dot_product_cube_map_1of3  */
    dot_product_cube_map_2of3 = 291, /* dot_product_cube_map_2of3  */
    dot_product_cube_map_3of3 = 292, /* dot_product_cube_map_3of3  */
    dot_product_reflect_cube_map_eye_from_qs_1of3 = 293, /* dot_product_reflect_cube_map_eye_from_qs_1of3  */
    dot_product_reflect_cube_map_eye_from_qs_2of3 = 294, /* dot_product_reflect_cube_map_eye_from_qs_2of3  */
    dot_product_reflect_cube_map_eye_from_qs_3of3 = 295, /* dot_product_reflect_cube_map_eye_from_qs_3of3  */
    dot_product_reflect_cube_map_const_eye_1of3 = 296, /* dot_product_reflect_cube_map_const_eye_1of3  */
    dot_product_reflect_cube_map_const_eye_2of3 = 297, /* dot_product_reflect_cube_map_const_eye_2of3  */
    dot_product_reflect_cube_map_const_eye_3of3 = 298, /* dot_product_reflect_cube_map_const_eye_3of3  */
    dot_product_cube_map_and_reflect_cube_map_eye_from_qs_1of3 = 299, /* dot_product_cube_map_and_reflect_cube_map_eye_from_qs_1of3  */
    dot_product_cube_map_and_reflect_cube_map_eye_from_qs_2of3 = 300, /* dot_product_cube_map_and_reflect_cube_map_eye_from_qs_2of3  */
    dot_product_cube_map_and_reflect_cube_map_eye_from_qs_3of3 = 301, /* dot_product_cube_map_and_reflect_cube_map_eye_from_qs_3of3  */
    dot_product_cube_map_and_reflect_cube_map_const_eye_1of3 = 302, /* dot_product_cube_map_and_reflect_cube_map_const_eye_1of3  */
    dot_product_cube_map_and_reflect_cube_map_const_eye_2of3 = 303, /* dot_product_cube_map_and_reflect_cube_map_const_eye_2of3  */
    dot_product_cube_map_and_reflect_cube_map_const_eye_3of3 = 304 /* dot_product_cube_map_and_reflect_cube_map_const_eye_3of3  */
  };
  typedef enum yytokentype yytoken_kind_t;
#endif

/* Value type.  */
#if ! defined YYSTYPE && ! defined YYSTYPE_IS_DECLARED
union YYSTYPE
{
#line 17 "ts1.0_grammar.y"

  float fval;
  InstPtr inst;
  InstListPtr instList;
  MappedVariablePtr variable;

#line 120 "_ts1.0_parser.hpp"

};
typedef union YYSTYPE YYSTYPE;
# define YYSTYPE_IS_TRIVIAL 1
# define YYSTYPE_IS_DECLARED 1
#endif


extern YYSTYPE ts10_lval;


int ts10_parse (void);


#endif /* !YY_TS10_TS1_0_PARSER_HPP_INCLUDED  */
