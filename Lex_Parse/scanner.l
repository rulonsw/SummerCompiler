%{
#include <cstring>
#include <cstdlib>
#include "parser.hpp"

static yy::location loc;
%}

%option nounput noyywrap batch debug noinput

%%

array|ARRAY         {return yy::Parser::token::ARRAYSY;}
begin|BEGIN         {return yy::Parser::token::BEGINSY;}
chr|CHR             {return yy::Parser::token::CHRSY;}
const|CONST         {return yy::Parser::token::CONSTSY;}
downto|DOWNTO       {return yy::Parser::token::DOWNTOSY;}
do|DO               {return yy::Parser::token::DOSY;}
else|ELSE           {return yy::Parser::token::ELSESY;}
elseif|ELSEIF       {return yy::Parser::token::ELSEIFSY;}
end|END             {return yy::Parser::token::ENDSY;}
forward|FORWARD     {return yy::Parser::token::FORWARDSY;}
for|FOR             {return yy::Parser::token::FORSY;}
function|FUNCTION   {return yy::Parser::token::FUNCTIONSY;}
if|IF               {return yy::Parser::token::IFSY;}
of|OF               {return yy::Parser::token::OFSY;}
ord|ORD             {return yy::Parser::token::ORDSY;}
pred|PRED           {return yy::Parser::token::PREDSY;}
procedure|PROCEDURE {return yy::Parser::token::PROCEDURESY;}
read|READ           {return yy::Parser::token::READSY;}
record|RECORD       {return yy::Parser::token::RECORDSY;}
repeat|REPEAT       {return yy::Parser::token::REPEATSY;}
return|RETURN       {return yy::Parser::token::RETURNSY;}
stop|STOP           {return yy::Parser::token::STOPSY;}
succ|SUCC           {return yy::Parser::token::SUCCSY;}
then|THEN           {return yy::Parser::token::THENSY;}
to|TO               {return yy::Parser::token::TOSY;}
type|TYPE           {return yy::Parser::token::TYPESY;}
until|UNTIL         {return yy::Parser::token::UNTILSY;}
var|VAR             {return yy::Parser::token::VARSY;}
ref|REF             {return yy::Parser::token::REFSY;}
while|WHILE         {return yy::Parser::token::WHILESY;}
write|WRITE         {return yy::Parser::token::WRITESY;}

"%"  {return yy::Parser::token::MODSY;}
"&"  {return yy::Parser::token::ANDSY;}
"("  {return yy::Parser::token::LPARENSY;}
")"  {return yy::Parser::token::RPARENSY;}
"*"  {return yy::Parser::token::MULTSY;}
"+"  {return yy::Parser::token::PLUSSY;}
","  {return yy::Parser::token::COMMASY;}
"-"  {return yy::Parser::token::MINUSSY;}
"."  {return yy::Parser::token::DOTSY;}
"/"  {return yy::Parser::token::DIVSY;}
":"  {return yy::Parser::token::COLONSY;}
":=" {return yy::Parser::token::ASSIGNSY;}
";"  {return yy::Parser::token::SCOLONSY;}
"<"  {return yy::Parser::token::LTSY;}
"<=" {return yy::Parser::token::LTESY;}
"<>" {return yy::Parser::token::NEQSY;}
"="  {return yy::Parser::token::EQSY;}
">"  {return yy::Parser::token::GTSY;}
">=" {return yy::Parser::token::GTESY;}
"["  {return yy::Parser::token::LBRACKETSY;}
"]"  {return yy::Parser::token::RBRACKETSY;}
"|"  {return yy::Parser::token::ORSY;}
"~"  {return yy::Parser::token::NOTSY;}

[a-zA-Z][a-zA-Z0-9_]* {return yy::Parser::token::IDENTSY;}

[0][0-7]+          {return yy::Parser::token::INTSY;}
[0][x][0-9a-fA-F]+ {return yy::Parser::token::INTSY;}
[0-9]+[0-9]*       {return yy::Parser::token::INTSY;}

'\\n'               {return yy::Parser::token::CHARCONSTSY;}
'\\t'               {return yy::Parser::token::CHARCONSTSY;}
'\\r'               {return yy::Parser::token::CHARCONSTSY;}
'\\?.'             {return yy::Parser::token::CHARCONSTSY;}

\"[a-zA-Z0-9~`!@'#$%^&*()_+=\-\[\]{}\\\/><,.:;| ]*\" {return yy::Parser::token::STRINGSY;}

\$.*$ {loc.lines(yyleng); loc.step();}
\n {loc.lines(yyleng); loc.step();}
[ \t]+ {loc.step();}
. { std::cerr << std::string("Unexpected token ")+yytext;}

%%
