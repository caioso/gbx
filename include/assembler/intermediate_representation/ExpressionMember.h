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
    UnaryPreDecrement,             
    UnaryPostDecrement,             
    UnaryPreIncrement,             
    UnaryPostIncrement,             
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

enum class Surrounder : uint8_t
{
    None,
    Expression,
    Memory
};

struct ExpressionMember
{
    ExpressionType Type;
    Operator OperatorType;
    
    OperandType Operand1Type;
    std::string Operand1;

    OperandType Operand2Type;
    std::string Operand2;

    Surrounder SurrounderType;

    size_t ExpressionID{};
    size_t Depth{};
};

}