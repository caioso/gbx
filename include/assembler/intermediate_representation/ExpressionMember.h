#pragma once

#include <string>

namespace gbxasm::intermediate_representation
{

enum class ExpressionType
{
    Binary,
    Unary,
    ResolveOperand
};

enum class Operator
{
    UnaryLogicNegation,        
    UnaryBitwiseNegation,      
    UnaryNegative,             
    UnaryPositive,             
    UnaryImmediate,             
    BinaryAddition,            
    BinarySubtraction,         
    BinaryMultiplication,      
    BinaryDivision,            
    BinaryBitwiseAnd,          
    BinaryBitwiseOr,           
    BinaryBitwiseXor,          
    BinaryLeftShift,           
    BinaryRightShift,          
    BinaryEquality,             
    BinaryDiffertence,          
    BinaryLogicAnd,             
    BinaryLogicOr,              
    BinaryGreaterThan,          
    BinaryLessThan,             
    BinaryGreaterThanOrEqualTo, 
    BinaryLessThanOrEqualTo,    
    BinaryThreeWay,             
    BinaryAssignment,
    NoOperator
};

enum class OperandType
{
    Identifier,
    PackIdentifier, 
    NumericLiteral,
    CharLiteral,
    StringLiteral,
    BooleanLiteral,
    Expression,
    NoOperand
};

struct ExpressionMember
{
    size_t ExpressionID{};

    ExpressionType Type;
    Operator OperatorType;
    
    OperandType Operand1Type;
    std::string Operand1;

    OperandType Operand2Type;
    std::string Operand2;

    size_t Depth{};
};

}