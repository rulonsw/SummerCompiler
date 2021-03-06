%{
#include <cerrno>
#include <climits>
#include <cstdlib>
#include <string>
#include <cstdlib>
#include <boost/algorithm/string.hpp>
#include <boost/algorithm/string/replace.hpp>
#include "parser.h"
#include "Driver.h"

static yy::location loc;


std::string cleanString (std::string s, bool toChar)
{
    if (toChar) {
        boost::erase_all(s,"\'");
        boost::replace_all(s, "\\n", "\n");
        boost::replace_all(s, "\\r", "\r");
        boost::replace_all(s, "\\b", "\b");
        boost::replace_all(s, "\\t", "\t");
        boost::replace_all(s, "\\f", "\f");
        boost::replace_all(s, "\\", "");
        return s.substr(0,1);
    }
    boost::erase_all(s, "\"");
    return s;

}
%}

%option nounput batch debug noinput

%%

array|ARRAY         {return yy::Parser::make_ARRAYSY(loc);}
begin|BEGIN         {return yy::Parser::make_BEGINSY(loc);}
chr|CHR             {return yy::Parser::make_CHRSY(loc);}
const|CONST         {return yy::Parser::make_CONSTSY(loc);}
downto|DOWNTO       {return yy::Parser::make_DOWNTOSY(loc);}
do|DO               {return yy::Parser::make_DOSY(loc);}
else|ELSE           {return yy::Parser::make_ELSESY(loc);}
elseif|ELSEIF       {return yy::Parser::make_ELSEIFSY(loc);}
end|END             {return yy::Parser::make_ENDSY(loc);}
forward|FORWARD     {return yy::Parser::make_FORWARDSY(loc);}
for|FOR             {return yy::Parser::make_FORSY(loc);}
function|FUNCTION   {return yy::Parser::make_FUNCTIONSY(loc);}
if|IF               {return yy::Parser::make_IFSY(loc);}
of|OF               {return yy::Parser::make_OFSY(loc);}
ord|ORD             {return yy::Parser::make_ORDSY(loc);}
pred|PRED           {return yy::Parser::make_PREDSY(loc);}
procedure|PROCEDURE {return yy::Parser::make_PROCEDURESY(loc);}
read|READ           {return yy::Parser::make_READSY(loc);}
record|RECORD       {return yy::Parser::make_RECORDSY(loc);}
repeat|REPEAT       {return yy::Parser::make_REPEATSY(loc);}
return|RETURN       {return yy::Parser::make_RETURNSY(loc);}
stop|STOP           {return yy::Parser::make_STOPSY(loc);}
succ|SUCC           {return yy::Parser::make_SUCCSY(loc);}
then|THEN           {return yy::Parser::make_THENSY(loc);}
to|TO               {return yy::Parser::make_TOSY(loc);}
type|TYPE           {return yy::Parser::make_TYPESY(loc);}
until|UNTIL         {return yy::Parser::make_UNTILSY(loc);}
var|VAR             {return yy::Parser::make_VARSY(loc);}
ref|REF             {return yy::Parser::make_REFSY(loc);}
while|WHILE         {return yy::Parser::make_WHILESY(loc);}
write|WRITE         {return yy::Parser::make_WRITESY(loc);}

"%"  {return yy::Parser::make_MODSY(loc);}
"&"  {return yy::Parser::make_ANDSY(loc);}
"("  {return yy::Parser::make_LPARENSY(loc);}
")"  {return yy::Parser::make_RPARENSY(loc);}
"*"  {return yy::Parser::make_MULTSY(loc);}
"+"  {return yy::Parser::make_PLUSSY(loc);}
","  {return yy::Parser::make_COMMASY(loc);}
"-"  {return yy::Parser::make_MINUSSY(loc);}
"."  {return yy::Parser::make_DOTSY(loc);}
"/"  {return yy::Parser::make_DIVSY(loc);}
":"  {return yy::Parser::make_COLONSY(loc);}
":=" {return yy::Parser::make_ASSIGNSY(loc);}
";"  {return yy::Parser::make_SCOLONSY(loc);}
"<"  {return yy::Parser::make_LTSY(loc);}
"<=" {return yy::Parser::make_LTESY(loc);}
"<>" {return yy::Parser::make_NEQSY(loc);}
"="  {return yy::Parser::make_EQSY(loc);}
">"  {return yy::Parser::make_GTSY(loc);}
">=" {return yy::Parser::make_GTESY(loc);}
"["  {return yy::Parser::make_LBRACKETSY(loc);}
"]"  {return yy::Parser::make_RBRACKETSY(loc);}
"|"  {return yy::Parser::make_ORSY(loc);}
"~"  {return yy::Parser::make_NOTSY(loc);}
<<EOF>> {return yy::Parser::make_END(loc);}

[a-zA-Z][a-zA-Z0-9_]* {std::string str = cleanString(yytext, false); return yy::Parser::make_IDENTSY(str, loc);}

[0][0-7]+          {int retVal = strtol(yytext, NULL, 8); return yy::Parser::make_INTSY(retVal, loc);}
[0][x][0-9a-fA-F]+ {int retVal = strtol(yytext, NULL, 0); return yy::Parser::make_INTSY(retVal, loc);}
[0-9]+[0-9]*       {int retVal = strtol(yytext, NULL, 10); return yy::Parser::make_INTSY(retVal, loc);}

'\\n'               {std::string str = cleanString(yytext, true); return yy::Parser::make_CHARCONSTSY(str.c_str()[0],loc);}
'\\t'               {std::string str = cleanString(yytext, true); return yy::Parser::make_CHARCONSTSY(str.c_str()[0],loc);}
'\\r'               {std::string str = cleanString(yytext, true); return yy::Parser::make_CHARCONSTSY(str.c_str()[0],loc);}
'\\?.'             {std::string str = cleanString(yytext, true); return yy::Parser::make_CHARCONSTSY(str.c_str()[0], loc);}

\"[a-zA-Z0-9~`!@'#$%^&*()_+=\-\[\]{}\\\/><,.:;| ]*\" {std::string str = cleanString(yytext,false); return yy::Parser::make_STRINGSY(str.c_str(), loc);}

\$.*$ {loc.lines(yyleng); loc.step();}
\n {loc.lines(yyleng); loc.step();}
[ \t]+ {loc.step();}
. { std::cerr << std::string("Unexpected token ")+yytext;}

%%
void Driver::begin_scan() {
    yy_flex_debug = trace_scanning;
    if(!(yyin = fopen(sourceFile.c_str(), "r"))) {
        error("File idString " + sourceFile + " is unable to be opened. strerror: " + strerror(errno));
        throw("FILE READ ERROR");
    }
}

void Driver::end_scan() {
    fclose(yyin);
}