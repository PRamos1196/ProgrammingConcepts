/*
 * parsetree.h
 */

#ifndef PARSETREE_H_
#define PARSETREE_H_

#include <vector>
#include <map>
#include "value.h"
using std::vector;
using std::map;
static vector<string> idents;

// NodeType represents all possible types
enum NodeType { ERRTYPE, INTTYPE, STRTYPE, BOOLTYPE, IDENTTYPE };

// a "forward declaration" for a class to hold values
class Value;
extern map<string, Value> symbolMap;

class ParseTree {
    int			linenum;
public:

    ParseTree	*left;
    ParseTree	*right;
    ParseTree(int linenum, ParseTree *l = 0, ParseTree *r = 0)
            : linenum(linenum), left(l), right(r) {}

    virtual ~ParseTree() {
        delete left;
        delete right;
    }

    int GetLinenum() const { return linenum; }

    virtual NodeType GetType() const { return ERRTYPE; }
    virtual bool ConstString() const { return false; }
    virtual bool IdentDefined() const { return false; }
    virtual bool BoolDefined() const { return false; }
    virtual string getIDENT() const {return ""; }
    virtual bool getBOOLEAN() const {return false; }
    virtual Value Eval(map<string, Value> &symbolMap) = 0;

    virtual string getLexeme(){
        return 0;
    }
    virtual Value Eval() {
        return Value();
    };

    int LeafCount() const {
        int lc = 0;
        if( left ) lc += left->LeafCount();
        if ( left ) left ->Eval();
        if( right ) lc += right->LeafCount();
        if (right) right->Eval();
        if( left == 0 && right == 0 )
            lc++;
        return lc;
    }

    int StringCount() const{
        int stringc = 0;
        if (left){
            if (left -> GetType () == STRTYPE){
                stringc++;
            }
            stringc += left ->StringCount();
        }
        if (right) {
            if (right->GetType()== STRTYPE){
                stringc++;
            }
            stringc+= right->StringCount();
        }
        return stringc;
    }

    int IdentCount() const {
        int ic = 0;
        if( left) {
            if (left -> GetType ()== IDENTTYPE){
                idents.push_back(left->getLexeme());
                ic++;
            }
            ic+= left ->IdentCount();
        }
        if (right){
            if (right->GetType()== IDENTTYPE){
                idents.push_back(right->getLexeme());
                ic++;
            }
            ic+= right->IdentCount();
        }
        return ic;
    }
    vector<string> myVec(){
        return idents;
    }
};

class StmtList : public ParseTree {

public:
    StmtList(ParseTree *l, ParseTree *r) : ParseTree(0, l, r) {}
    virtual Value Eval(map<string, Value>&symbolMap) {
        left->Eval(symbolMap);
        if(right)
        { right ->Eval(symbolMap); }
        return Value();
    }
};

class IfStatement : public ParseTree {
public:
    IfStatement(int line, ParseTree *ex, ParseTree *stmt) : ParseTree(line, ex, stmt) {}
    virtual Value Eval(map<string, Value> &symbolMap){
        Value lEval = left ->Eval(symbolMap);
        if (lEval.isBoolType())
        { if(lEval.getBoolean()) { return right -> Eval(symbolMap); } }
        else { RunTimeError("Need Boolean Type"); }
        return Value();
    }
};

class Assignment : public ParseTree {
public:
    Assignment(int line, ParseTree *lhs, ParseTree *rhs) : ParseTree(line, lhs, rhs) {}
    virtual Value Eval(map<string, Value> &symbolMap)
    {
        if (left->IdentDefined()) {
            if(!symbolMap.empty() && symbolMap.count(left->getIDENT())) {
                Value reval = right->Eval(symbolMap);
                string lEval = left->getIDENT();
                symbolMap[lEval] = reval;
            } else {
                Value reval = right->Eval(symbolMap);
                string lEval = left->getIDENT();
                symbolMap.insert (std::pair<string, Value>(lEval, reval));
            }
        }
        else { RunTimeError("IDENT Type Expected"); }
        return Value();
    }
};



class PrintStatement : public ParseTree {
public:
    PrintStatement(int line, ParseTree *e) : ParseTree(line, e) {}
    virtual Value Eval(map<string, Value> &symbolMap)
    {
        cout << left->Eval(symbolMap) << '\n';
        return Value();
    }

};

class PlusExpr : public ParseTree {
public:
    PlusExpr(int line, ParseTree *l, ParseTree *r) : ParseTree(line,l,r) {}
    virtual Value Eval (map<string, Value> &symbolMap) { return left -> Eval(symbolMap) + right ->Eval(symbolMap); }
};

class MinusExpr : public ParseTree {
public:
    MinusExpr(int line, ParseTree *l, ParseTree *r) : ParseTree(line,l,r) {}
    virtual Value Eval(map<string, Value> &symbolMap) { return left->Eval(symbolMap) - right->Eval(symbolMap); }
};

class TimesExpr : public ParseTree {
public:
    TimesExpr(int line, ParseTree *l, ParseTree *r) : ParseTree(line,l,r) {}
    virtual Value Eval(map<string, Value> &symbolMap) { return left->Eval(symbolMap) * right->Eval(symbolMap); }
};

class DivideExpr : public ParseTree {
public:
    DivideExpr(int line, ParseTree *l, ParseTree *r) : ParseTree(line,l,r) {}
    virtual Value Eval(map<string, Value> &symbolMap) { return left->Eval(symbolMap) / right->Eval(symbolMap); }
};

class LogicAndExpr : public ParseTree {
public:
    LogicAndExpr(int line, ParseTree *l, ParseTree *r) : ParseTree(line,l,r) {}
    virtual Value Eval(map<string, Value> &symbolMap) {
        Value lEval = left->Eval(symbolMap);
        Value rEval = right->Eval(symbolMap);
        if (lEval.isBoolType() && rEval.isBoolType()){ return lEval.isTrue() && rEval.isTrue(); }
        else { RunTimeError("BOOL Type expected"); }
        return Value();
    }
};

class LogicOrExpr : public ParseTree {
public:
    LogicOrExpr(int line, ParseTree *l, ParseTree *r) : ParseTree(line,l,r) {}
    virtual Value Eval(map<string, Value> &symbolMap)
    {
        Value rEval = right->Eval(symbolMap);
        Value lEval = left->Eval(symbolMap);
        if (lEval.isBoolType() || rEval.isBoolType()) { return lEval.isTrue() || rEval.isTrue(); }
        else { RunTimeError("BOOL Type Expected"); }
        return Value();
    }
};

class EqExpr : public ParseTree {
public:
    EqExpr(int line, ParseTree *l, ParseTree *r) : ParseTree(line,l,r) {}
    virtual Value Eval(map<string, Value> &symbolMap)
    {
        return left->Eval(symbolMap) == right->Eval(symbolMap);
    }
};

class NEqExpr : public ParseTree {
public:
    NEqExpr(int line, ParseTree *l, ParseTree *r) : ParseTree(line,l,r) {}
    virtual Value Eval(map<string, Value> &symbolMap){ return left->Eval(symbolMap) != right->Eval(symbolMap); }
};

class LtExpr : public ParseTree {
public:
    LtExpr(int line, ParseTree *l, ParseTree *r) : ParseTree(line,l,r) {}
    virtual Value Eval(map<string, Value> &symbolMap) { return left->Eval(symbolMap) < right->Eval(symbolMap); }
};



class LEqExpr : public ParseTree {
public:
    LEqExpr(int line, ParseTree *l, ParseTree *r) : ParseTree(line,l,r) {}
    virtual Value Eval(map<string, Value> &symbolMap){ return left->Eval(symbolMap) <= right->Eval(symbolMap); }
};

class GtExpr : public ParseTree {
public:
    GtExpr(int line, ParseTree *l, ParseTree *r) : ParseTree(line,l,r) {}
    virtual Value Eval(map<string, Value> &symbolMap){ return left->Eval(symbolMap) > right->Eval(symbolMap); }
};

class GEqExpr : public ParseTree {
public:
    GEqExpr(int line, ParseTree *l, ParseTree *r) : ParseTree(line,l,r) {}
    virtual Value Eval(map<string, Value> &symbolMap) { return left->Eval(symbolMap) >= right->Eval(symbolMap); }
};

class IConst : public ParseTree {
    int val;

public:
    IConst(int l, int i) : ParseTree(l), val(i) {}
    IConst(Token& t) : ParseTree(t.GetLinenum()) { val = stoi(t.GetLexeme()); }
    NodeType GetType() const { return INTTYPE; }
    virtual Value Eval(map<string, Value> &symbolMap){ return Value(val); }
};

class BoolConst : public ParseTree {
    bool val;

public:
    BoolConst(Token& t, bool val) : ParseTree(t.GetLinenum()), val(val) {}

    NodeType GetType() const { return BOOLTYPE; }
    bool BoolDefined() const { return true; }
    bool getBOOLEAN() const {return val; }
    virtual Value Eval(map<string, Value> &symbolMap){ return Value(val); }
};

class SConst : public ParseTree {
    string val;

public:
    SConst(Token& t) : ParseTree(t.GetLinenum()) { val = t.GetLexeme(); }
    NodeType GetType() const { return STRTYPE; }
    bool ConstString() const { return true; }
    virtual Value Eval(map<string, Value> &symbolMap) { return Value(val);}
};

class Ident : public ParseTree {
    string id;
    NodeType GetType() const { return IDENTTYPE; }

public:
    Ident(Token& t) : ParseTree(t.GetLinenum()), id(t.GetLexeme()) {}
    string ident = id;
    string getLexeme() { return ident; };
    bool IdentDefined() const { return true; }
    string getIDENT() const { return id; }

    virtual Value Eval(map<string, Value> &symbolMap)
    {
        if (!symbolMap.empty() && symbolMap.count(id)) { return symbolMap[id]; }
        else { RunTimeError(""); }
        return Value();
    }
};

#endif /* PARSETREE_H_ */