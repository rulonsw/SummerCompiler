/*
*   The following code is largely inspired by
*   the calc++ example on GNU's website
*   available here: https://www.gnu.org/software/bison/manual/bison.html#A-Complete-C_002b_002b-Example
*/

%skeleton "lalr1.cc" /* -*- C++ -*- */
%require "3.0.4"
%defines
%define parser_class_name {Parser}

%define api.token.constructor
%define api.value.type variant
%define parse.assert

%code requires
{
#include <string>
#include "its_complicated/components/Expression.h"
#include "its_complicated/MetaCoder.h"
class Driver;
}

// The parsing context.
%param { Driver& driver }

%locations
%initial-action
{
  // Initialize the initial location.
  @$.begin.filename = @$.end.filename = &driver.sourceFile;
};

%define parse.trace
%define parse.error verbose

%code
{
#include "Driver.h"
}


%token ARRAYSY
%token ASSIGNSY
%token BEGINSY
%token CHRSY
%token COLONSY
%token COMMASY
%token CONSTSY
%token DIVSY
%token DOSY
%token DOTSY
%token DOWNTOSY
%token ELSEIFSY
%token ELSESY
%token ENDSY
%token EQSY
%token FORSY
%token FORWARDSY
%token FUNCTIONSY
%token GTESY
%token GTSY
%token <std::string> IDENTSY
%token IFSY
%token INTSY
%token LBRACKETSY
%token LPARENSY
%token LTESY
%token LTSY
%token MINUSSY
%token MODSY
%token MULTSY
%token NOTSY
%token OFSY
%token ORDSY
%token PLUSSY
%token PREDSY
%token PROCEDURESY
%token RBRACKETSY
%token READSY
%token RECORDSY
%token REFSY
%token REPEATSY
%token RETURNSY
%token RPARENSY
%token SCOLONSY
%token STOPSY
%token <std::string> STRINGSY
%token SUCCSY
%token THENSY
%token TOSY
%token TYPESY
%token UNTILSY
%token VARSY
%token WHILESY
%token WRITESY

%token END  0  "end of file"

%left ANDSY ORSY
%right NOTSY
%nonassoc EQSY LTESY GTESY GTSY LTSY NEQSY
%left PLUSSY MINUSSY
%left DIVSY MODSY MULTSY
%right UMINUSSY

%token <char> CHARCONSTSY
%type <int> Arguments
%type <int> ArrayType
%type <int> Assignment
%type <int> Block
%type <int> Body
%type <int> ElseClause
%type <int> ElseIfHead
%type <int> ElseIfList
%type <std::shared_ptr<RSWCOMP::Expression>> Expression
%type <RSWCOMP::Function> FSignature
%type <int> FieldDecl
%type <int> FieldDecls
%type <int> ForHead
%type <int> ForStatement
%type <std::pair<std::vector<std::string>, std::vector<RSWCOMP::Type>>> FormalParameter
%type <std::pair<std::vector<std::string>, std::vector<RSWCOMP::Type>>> FormalParameters
%type <std::vector<std::string> > PFIdentList
%type <int> FunctionCall
%type <int> INTSY
%type <int> IdentList
%type <int> OptVar
%type <int> IfHead
%type <int> IfStatement
%type <std::shared_ptr<RSWCOMP::LValue>> LValue
%type <int> OptArguments
%type <RSWCOMP::FunctionSignature> OptFormalParameters
%type <RSWCOMP::Function> PSignature
%type <int> ProcedureCall
%type <int> ReadArgs
%type <int> ReadStatement
%type <int> RecordType
%type <int> RepeatStatement
%type <int> ReturnStatement
%type <RSWCOMP::Type> SimpleType
%type <int> Statement
%type <int> StatementList
%type <int> StopStatement
%type <int> ThenPart
%type <int> ToHead
%type <RSWCOMP::Type> Type
%type <std::shared_ptr<RSWCOMP::Expression>> WhileHead
%type <std::shared_ptr<RSWCOMP::Expression>> WhileStatement
%type <int> WriteArgs
%type <int> WriteStatement
%type <std::string> idString
%type <std::string> stringString

%%
Program : ProgramHead Block DOTSY {RSWCOMP::MainBlock();}
				;

ProgramHead : OptConstDecls OptTypeDecls OptVarDecls PFDecls
            ;
OptConstDecls : CONSTSY ConstDecls
							|
							;

ConstDecls : ConstDecls ConstDecl
					 | ConstDecl {RSWCOMP::ConstBlock();}
					 ;

ConstDecl : IDENTSY EQSY Expression SCOLONSY {RSWCOMP::declareConst($1, $3);}
					;

PFDecls : PFDecls ProcedureDecl
        | PFDecls FunctionDecl
        |
        ;

ProcedureDecl : PSignature SCOLONSY FORWARDSY SCOLONSY {$1.isProcedure = true; $1.isFwdDeclaration = true;}
              | PSignature SCOLONSY Body SCOLONSY {$1.isProcedure = true;}
				    	;

PSignature : PROCEDURESY IDENTSY LPARENSY OptFormalParameters RPARENSY {$4.name = $2; $$ = RSWCOMP::Function($4, RSWCOMP::Type());}
           ;

FunctionDecl : FSignature SCOLONSY FORWARDSY SCOLONSY {$1.isFwdDeclaration= true; $1.isProcedure = false;}
						 | FSignature SCOLONSY Body SCOLONSY {$1.isFwdDeclaration = false; $1.isProcedure = false;}
						 ;

FSignature : FUNCTIONSY IDENTSY LPARENSY OptFormalParameters RPARENSY COLONSY Type[rt] {$4.name = $2; $$ = RSWCOMP::Function($4, $rt);}
           ;

OptFormalParameters : FormalParameters {
                            RSWCOMP::FunctionSignature fs($1.first, $1.second);
                            $$ = fs;
                        }
                    | {RSWCOMP::FunctionSignature fs;
                            $$ = fs;
                      }
                    ;

FormalParameters : FormalParameters SCOLONSY FormalParameter {
                        for(int i = 0; i < $3.first.size(); i++) {
                            $$.first.push_back($3.first.at(i));
                            $$.second.push_back($3.second.at(i));
                        }
                        $$ = $1;
                    }
                 | FormalParameter {
                        $$ = $1;
                    }
                 ;
//TODO: FormalParameter type needs to be changed to a std::pair
FormalParameter : OptVar PFIdentList COLONSY Type {
                        for(auto i : $2) {
                            $$.first.push_back(i);
                            $$.second.push_back($4);
                        }
                    }
                ;

PFIdentList : PFIdentList COMMASY IDENTSY {$$.push_back($3); $$ = $1;}
            | IDENTSY {$$.push_back($1);}


OptVar : VARSY {}
       | REFSY {}
       | {}
       ;


Body : OptConstDecls OptTypeDecls OptVarDecls Block {}
     ;

Block : BEGINSY StatementList ENDSY {RSWCOMP::WriteABlock();}
      ;

StatementList : StatementList SCOLONSY Statement {}
              | Statement {}
              ;

OptTypeDecls : TYPESY TypeDecls
             |
             ;

TypeDecls    : TypeDecls TypeDecl
             | TypeDecl
             ;

TypeDecl : IDENTSY EQSY Type SCOLONSY {}
         ;

Type : SimpleType {$$ = $1;}
     | RecordType {}
     | ArrayType {}
     ;

SimpleType : IDENTSY {$$ = RSWCOMP::SearchForSimple($1);}
           ;

RecordType : RECORDSY FieldDecls ENDSY {}
           ;

FieldDecls : FieldDecls FieldDecl {}
           | {}
           ;

FieldDecl : IdentList COLONSY Type SCOLONSY {}
          ;

IdentList : IdentList COMMASY IDENTSY {
                //TODO: reform MakeId with scope sensitivity
                RSWCOMP::MakeId($3);
            }
          | IDENTSY {RSWCOMP::MakeId($1);}
          ;

ArrayType : ARRAYSY LBRACKETSY Expression COLONSY Expression RBRACKETSY OFSY Type {}
          ;

OptVarDecls : VARSY VarDecls
            |
            ;

VarDecls    : VarDecls VarDecl
            | VarDecl
            ;

VarDecl : IdentList COLONSY Type SCOLONSY {RSWCOMP::MakeVar($3);}
        ;

Statement : Assignment {$$ = $1;}
          | IfStatement {}
          | WhileStatement {}
          | RepeatStatement {}
          | ForStatement {}
          | StopStatement {$$ = $1;}
          | ReturnStatement {}
          | ReadStatement {$$ = $1;}
          | WriteStatement {$$ = $1;}
          | ProcedureCall {}
          | {$$ = -1;}
          ;

Assignment : LValue ASSIGNSY Expression {RSWCOMP::Assign($1,$3);}
           ;

IfStatement : IFSY Expression {RSWCOMP::ProcIfStmt($2);} ThenPart ElseIfList ElseClause ENDSY {RSWCOMP::FinishIfStmt();}
            ;

ThenPart : THENSY StatementList {}
         ;

ElseIfList : ElseIfList ElseIfHead ThenPart {}
           |{}
           ;

ElseIfHead : ELSEIFSY {RSWCOMP::PrepElseIfStmt();} Expression {RSWCOMP::ProcElseIfStmt($3);}
           ;

ElseClause : ELSESY {RSWCOMP::ProcElseStmt();} StatementList {}
           | {RSWCOMP::ProcElseStmt();}
           ;

WhileStatement : WHILESY {RSWCOMP::PrepWhileStmt();} Expression {RSWCOMP::ProcWhileStmt($3);} DOSY StatementList ENDSY {RSWCOMP::FinishWhileStmt();}
               ;

RepeatStatement : REPEATSY {RSWCOMP::PrepRepeatStmt();} StatementList UNTILSY Expression {RSWCOMP::ProcRepeatStmt($5);}

ForStatement : ForHead ToHead DOSY StatementList ENDSY{RSWCOMP::PrepForStmt(true);}
             | ForHead DownToHead DOSY StatementList ENDSY{RSWCOMP::PrepForStmt(false);}
             ;

ForHead : FORSY IDENTSY ASSIGNSY Expression {RSWCOMP::ProcForStmt($2, $4);}
        ;

ToHead : TOSY Expression {RSWCOMP::ProcToHead($2);}
       ;

DownToHead  : DOWNTOSY Expression {RSWCOMP::ProcDownToHead($2);}
            ;
StopStatement : STOPSY {RSWCOMP::Stop();}
              ;

ReturnStatement : RETURNSY Expression {}
                | RETURNSY {}
                ;


ReadStatement : READSY LPARENSY ReadArgs RPARENSY {$$ = $3;}
              ;

ReadArgs : ReadArgs COMMASY LValue {RSWCOMP::ReadValue($3);}
         | LValue                  {RSWCOMP::ReadValue($1);}
         ;

WriteStatement : WRITESY LPARENSY WriteArgs RPARENSY {$$ = $3;}
               ;

WriteArgs : WriteArgs COMMASY Expression {RSWCOMP::WriteExpr($3);}
          | Expression                   {RSWCOMP::WriteExpr($1);}
          ;

ProcedureCall : IDENTSY LPARENSY OptArguments RPARENSY {}
              ;
OptArguments : Arguments {}
             |           {}
             ;
Arguments : Arguments COMMASY Expression {}
          | Expression                   {}
          ;

Expression : CHARCONSTSY                         {$$ = RSWCOMP::CharExpr($1);}
           | CHRSY LPARENSY Expression RPARENSY  {$$ = RSWCOMP::ChrExpr($3);}
           | Expression ANDSY Expression         {$$ = RSWCOMP::AndExpr($1,$3);}
           | Expression DIVSY Expression         {$$ = RSWCOMP::DivExpr($1,$3);}
           | Expression EQSY Expression          {$$ = RSWCOMP::EqExpr($1,$3);}
           | Expression GTESY Expression         {$$ = RSWCOMP::GteExpr($1,$3);}
           | Expression GTSY Expression          {$$ = RSWCOMP::GtExpr($1,$3);}
           | Expression LTESY Expression         {$$ = RSWCOMP::LteExpr($1,$3);}
           | Expression LTSY Expression          {$$ = RSWCOMP::LtExpr($1,$3);}
           | Expression MINUSSY Expression       {$$ = RSWCOMP::SubExpr($1,$3);}
           | Expression MODSY Expression         {$$ = RSWCOMP::ModExpr($1,$3);}
           | Expression MULTSY Expression        {$$ = RSWCOMP::MultExpr($1,$3);}
           | Expression NEQSY Expression         {$$ = RSWCOMP::NeqExpr($1,$3);}
           | Expression ORSY Expression          {$$ = RSWCOMP::OrExpr($1,$3);}
           | Expression PLUSSY Expression        {$$ = RSWCOMP::AddExpr($1,$3);}
           | FunctionCall                        {}
           | INTSY                               {$$ = RSWCOMP::IntExpr($1);}
           | LPARENSY Expression RPARENSY        {}
           | LValue                              {$$ = RSWCOMP::ExprFromLV($1);}
           | MINUSSY Expression %prec UMINUSSY   {$$ = RSWCOMP::UnMinusExpr($2);}
           | NOTSY Expression                    {$$ = RSWCOMP::NotExpr($2);}
           | ORDSY LPARENSY Expression RPARENSY  {$$ = RSWCOMP::OrdExpr($3);}
           | PREDSY LPARENSY Expression RPARENSY {$$ = RSWCOMP::PredExpr($3);}
           | STRINGSY                            {$$ = RSWCOMP::StringExpr($1);}
           | SUCCSY LPARENSY Expression RPARENSY {$$ = RSWCOMP::SuccExpr($3);}
           ;

FunctionCall : IDENTSY LPARENSY OptArguments RPARENSY {}
             ;

LValue : LValue DOTSY IDENTSY {$$ = nullptr;}
       | LValue LBRACKETSY Expression RBRACKETSY {$$ = nullptr;}
       | IDENTSY {$$ = RSWCOMP::loadId($1);}
       ;
%%
void yy::Parser::error (const location_type& l,
                          const std::string& m)
{
  driver.error (l, m);
}
