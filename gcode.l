%option prefix="xx"
%option noyywrap
%option c++


%{
/*
//#include <FlexLexer.h>
// An example of using the flex C++ scanner class.

// compile:
//  flex svg_data.l
//  g++  lex.xx.cc
*/
#include <iostream>

%}

COMMENT      "(".*")"
G            "G"[0-9]+
X            "X"{NUMBER}
Y            "Y"{NUMBER}
Z            "Z"{NUMBER}
M            "M"[0-9]+
R            "R"{NUMBER}
I            "I"{NUMBER}
J            "J"{NUMBER}
F            "F"{NUMBER}


DEF_REEL1    [-+]?[0-9]+([Ee][+-]?[0-9]+)?
DEF_REEL2    [-+]?[0-9]+"."[0-9]*([Ee][+-]?[0-9]+)?
DEF_REEL3    [-+]?[0-9]*"."[0-9]+([Ee][+-]?[0-9]+)?
NUMBER       {DEF_REEL1}|{DEF_REEL2}|{DEF_REEL3}



%%
[ \t\n]        ;
{COMMENT}      ;
{G}         {return 1;}
{X}         {return 2;}
{Y}         {return 3;}
{Z}         {return 4;}
{M}         {return 5;}
{R}         {return 6;}
{I}         {return 7;}
{J}         {return 8;}
{F}         {return 9;}

%%


