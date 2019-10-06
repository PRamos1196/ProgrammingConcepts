/*
 * value.h
 */

#ifndef VALUE_H_
#define VALUE_H_

#include <string>
#include <iostream>
using namespace std;
extern void RunTimeError ( string );


// object holds boolean, integer, or string, and remembers which it holds
class Value {
    bool	bval;
    int		ival;
    string	sval;
    enum VT { isBool, isInt, isString, isTypeError } type;

public:
    Value() : bval(false), ival(0), type(isTypeError) {}
    Value(bool bval) : bval(bval), ival(0), type(isBool) {}
    Value(int ival) : bval(false), ival(ival), type(isInt) {}
    Value(string sval) : bval(false), ival(0), sval(sval), type(isString) {}

    // in the case of an error, I use the value to hold the error message
    Value(string sval, bool isError) : bval(false), ival(0), sval(sval), type(isTypeError) {}

    bool isBoolType() const { return type == VT::isBool; }
    bool isIntType() const { return type == VT::isInt; }
    bool isStringType() const { return type == VT::isString; }
    bool isError() const { return type == VT::isTypeError; }
    bool hasMessage() const { return isError() && sval.size() > 0; }
    bool isTrue() const { return isBoolType() && bval; }
    bool getBoolean() const {
        if( !isBoolType() ) { throw "Not boolean valued"; }
        return bval;
    }

    int getInteger() const {
        if( !isIntType() )
            throw "Not integer valued";
        return ival;
    }

    string getString() const {
        if( !isStringType() )
            throw "Not string valued";
        return sval;
    }

    string getMessage() const {
        if( !hasMessage() )
            throw "No message";
        return sval;
    }

    friend ostream& operator<<(ostream& out, const Value& v) {
        if( v.type == VT::isBool ) out << (v.bval ? "True" : "False");
        else if( v.type == VT::isInt ) out << v.ival;
        else if( v.type == VT::isString ) out << v.sval;
        else if( v.sval.size() > 0 ) out << "RUNTIME ERROR " << v.sval;
        else out << "TYPE ERROR";
        return out;
    }
    Value operator+(const Value& v){
        if (type == isInt && v.type == isInt) { return Value(ival + v.ival); }
        if(type == isString && v.type == isString) { return Value(sval + v.sval); }
        RunTimeError("Cant add these two guys");
        return Value();
    }
    Value operator-(const Value& v){
        if(type == isInt && v.type ==isInt) { return Value(ival - v.ival); }
        RunTimeError("Cant minus these two guys");
        return Value();
    }
    Value operator*(const Value& v){
        if (type == isInt && v.type == isInt) { return Value(ival * v.ival); }
        if (type == isInt && v.type == isString) {
            if(getInteger() >= 0)
            {
                string a;
                for( int i=0; i < ival; ++i) { a = a + v.sval; }
                return a;
            }
            RunTimeError("String times negative number cant be done");
        }
        if (type == isString && v.type == isInt) {
            if(v.getInteger() >=0) {
                string a;
                for(int i=0; i <v.ival; ++i) { a = a+ sval; }
                return a;
            }
            RunTimeError("String times negative number cant be done");
        }
        if (type== isInt && v.type == isBool) {
            if (ival == -1) {
                bool ans = !v.bval;
                return (ans);
            }
            RunTimeError("Cant do this stmt");
        }
        RunTimeError("Cant timmes these two");
        return Value();
    }
    Value operator/(const Value& v) {
        if(type== isInt && v.type == isInt) {
            if(v.ival != 0) { return Value( ival / v.ival ); }
            RunTimeError("Cant divide by 0 thats undefined");
        }
        RunTimeError("Cant divide these chief");
        return Value();
    }
    Value operator<(const Value& v) {
        if (type == isInt && v.type == isInt) { return Value(ival < v.ival); }
        if (type == isString && v.type == isString) { return Value(sval < v.sval); }
        RunTimeError("smth happened with this <");
        return Value();
    }
    Value operator<=(const Value& v) {
        if (type == isInt && v.type == isInt) { return Value(ival <= v.ival); }
        if (type == isString && v.type == isString) { return Value(sval <= v.sval); }
        RunTimeError("smth happened with this <=");
        return Value();
    }
    Value operator>(const Value& v) {
        if (type == isInt && v.type == isInt) { return Value(ival > v.ival); }
        if (type == isString && v.type == isString) { return Value(sval > v.sval); }
        RunTimeError("smth happened with this >");
        return Value();
    }
    Value operator>=(const Value& v) {
        if (type == isInt && v.type == isInt) { return Value(ival >= v.ival); }
        if (type == isString && v.type == isString) { return Value(sval >= v.sval); }
        RunTimeError("smth happened with this >=");
        return Value();
    }
    Value operator==(const Value& v) {
        if (type == isInt && v.type == isInt) { return Value(ival == v.ival); }
        if (type == isString && v.type == isString) { return Value(sval == v.sval); }
        if (type == isBool && v.type == isBool) { return Value(bval == v.bval); }
        RunTimeError("smth happened with ==");
        return Value();
    }

    Value operator!=(const Value& v) {
        if (type == isInt && v.type == isInt) { return Value(ival != v.ival); }
        if (type == isString && v.type == isString) { return Value(sval != v.sval); }
        if (type == isBool && v.type == isBool) { return Value(bval != v.bval); }
        RunTimeError("smth happened with !=");
        return Value();
    }
};

#endif /* VALUE_H_ */