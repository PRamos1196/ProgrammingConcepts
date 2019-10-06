#include <cctype>
#include <map>
using std::map;

#include "tokens.h"

static map<TokenType,string> tokenPrint = {
        { IF, "IF" },
        { THEN, "THEN" },
        { PRINT, "PRINT" },
        { TRUE, "TRUE" },
        { FALSE, "FALSE" },

        { IDENT, "IDENT" },

        { ICONST, "ICONST" },
        { SCONST, "SCONST" },

        { PLUS, "PLUS" },
        { MINUS, "MINUS" },
        { STAR, "STAR" },
        { SLASH, "SLASH" },
        { ASSIGN, "ASSIGN" },
        { EQ, "EQ" },
        { NEQ, "NEQ" },
        { LT, "LT" },
        { LEQ, "LEQ" },
        { GT, "GT" },
        { GEQ, "GEQ" },
        { LOGICAND, "LOGICAND" },
        { LOGICOR, "LOGICOR" },
        { SC, "SC" },

        { ERR, "ERR" },

        { DONE, "DONE" }
};


ostream& operator<<(ostream& out, const Token& tok) {
    TokenType tt = tok.GetTokenType();
    out << tokenPrint[ tt ];
    if( tt == IDENT || tt == ICONST || tt == SCONST || tt == ERR ) {
        out << "(" << tok.GetLexeme() << ")";
    }
    return out;
}


static map<string,TokenType> kwmap = {
        { "if", IF },
        { "then", THEN },
        { "print", PRINT },
        { "true", TRUE },
        { "false", FALSE },
};

Token
id_or_kw(const string& lexeme, int linenum)
{
    TokenType tt = IDENT;

    auto kIt = kwmap.find(lexeme);
    if( kIt != kwmap.end() )
        tt = kIt->second;

    return Token(tt, lexeme, linenum);
}


Token
getNextToken(istream *in, int *linenum)
{
    enum LexState { BEGIN, INID, INSTRING, SAWMINUS, SAWEQ, SAWLT, SAWGT, SAWAND, SAWOR, ININT, INCOMMENT } lexstate = BEGIN;
    string lexeme;
    char ch;

    while(in->get(ch)) {

        if( ch == '\n' ) {
            (*linenum)++;
        }

        switch( lexstate ) {
            case BEGIN:
                if( isspace(ch) )
                    continue;

                lexeme = ch;

                if( isalpha(ch) ) {
                    lexstate = INID;
                }
                else if( ch == '"' ) {
                    lexstate = INSTRING;
                }
                else if( ch == '-' ) {
                    lexstate = SAWMINUS;
                }
                else if( ch == '=' ) {
                    lexstate = SAWEQ;
                }
                else if( ch == '>' ) {
                    lexstate = SAWGT;
                }
                else if( ch == '<' ) {
                    lexstate = SAWLT;
                }
                else if( ch == '&' ) {
                    lexstate = SAWAND;
                }
                else if( ch == '|' ) {
                    lexstate = SAWOR;
                }
                else if( isdigit(ch) ) {
                    lexstate = ININT;
                }
                else if( ch == '#' ) {
                    lexstate = INCOMMENT;
                }
                else {
                    TokenType tt = ERR;
                    switch( ch ) {
                        case '+':
                            tt = PLUS;
                            break;
                        case '*':
                            tt = STAR;
                            break;
                        case '/':
                            tt = SLASH;
                            break;
                        case '(':
                            tt = LPAREN;
                            break;
                        case ')':
                            tt = RPAREN;
                            break;
                        case ';':
                            tt = SC;
                            break;
                    }

                    return Token(tt, lexeme, *linenum);
                }
                break;

            case INID:
                if( isalpha(ch) || isdigit(ch) ) {
                    lexeme += ch;
                }
                else {
                    if( ch == '\n' )
                        (*linenum)--;
                    in->putback(ch);
                    return id_or_kw(lexeme, *linenum);
                }
                break;

            case INSTRING:
                lexeme += ch;
                if( ch == '\n' ) {
                    return Token(ERR, lexeme, *linenum );
                }
                if( ch == '"' ) {
                    lexeme = lexeme.substr(1, lexeme.length()-2);
                    return Token(SCONST, lexeme, *linenum );
                }
                break;

            case SAWMINUS:
                if( !isdigit(ch) ) {
                    if( ch == '\n' )
                        (*linenum)--;
                    in->putback(ch);
                    return Token(MINUS, lexeme, *linenum);
                }
                else {
                    lexeme += ch;
                    lexstate = ININT;
                }
                break;

            case SAWEQ:
                if( ch == '=' )
                    return Token(EQ, lexeme, *linenum);
                else {
                    if( ch == '\n' )
                        (*linenum)--;
                    in->putback(ch);
                    return Token(ASSIGN, lexeme, *linenum);
                }
                break;

            case SAWGT:
                if( ch == '=' )
                    return Token(GEQ, lexeme, *linenum);
                else {
                    if( ch == '\n' )
                        (*linenum)--;
                    in->putback(ch);
                    return Token(GT, lexeme, *linenum);
                }
                break;

            case SAWLT:
                if( ch == '=' )
                    return Token(LEQ, lexeme, *linenum);
                else {
                    if( ch == '\n' )
                        (*linenum)--;
                    in->putback(ch);
                    return Token(LT, lexeme, *linenum);
                }
                break;

            case SAWAND:
                if( ch == '&' )
                    return Token(LOGICAND, lexeme, *linenum);
                else {
                    return Token(ERR, lexeme, *linenum);
                }
                break;

            case SAWOR:
                if( ch == '|' )
                    return Token(LOGICOR, lexeme, *linenum);
                else {
                    return Token(ERR, lexeme, *linenum);
                }
                break;


            case ININT:
                if( isdigit(ch) ) {
                    lexeme += ch;
                }
                else if( isalpha(ch) ) {
                    lexeme += ch;
                    return Token(ERR, lexeme, *linenum);
                }
                else {
                    if( ch == '\n' )
                        (*linenum)--;
                    in->putback(ch);
                    return Token(ICONST, lexeme, *linenum);
                }
                break;

            case INCOMMENT:
                if( ch == '\n' ) {
                    lexstate = BEGIN;
                }
                break;
        }

    }

    if( in->eof() )
        return Token(DONE, "", *linenum);
    return Token(ERR, "some strange I/O error", *linenum);
}


