# GAME BOY X EXPRESSSIVE ASSEMBLY LANGUAGE

**GAME BOY X EXPRESSIVE ASSEMBLY LANGUAGE** (gbXpressive *ASM*) is a assembly programming language compatible with Nintendo's GAME BOY, GAME BOY Color, Super GAME BOY and the GAME BOY X video game platforms. 

The language is syntactically compatible with Z80's assembly (along with GAME BOY X extended mode instructions) and can be translated into executable code for any Z80-compliant hardware platform.

gbXpressive *ASM* is designed to provide *mid-level* programming language features (such as C-like statements, identifiers, among others) while leveraging the *low-level* capabilities of traditional assembly languages.

This is a *live-document* describing the language standards and features. The first draft version of this document has been created by @caioso, at 01-22-2021.

## Contents

- [Introduction](#introduction)
    - [Conventions](#conventions)
    - [Revisions](#revisions)

- [Features](#features)
  - [Statements](#statements)
    - [`PACK`](#pack)

## Introduction
### Conventions
### Revisions

## Features

### Registers
### Register Pairs
### Flags Register
### Special Registers
### Variables
### Functions
#### Function Arguments
#### Function Returns
#### Abortable Functions
### Statements

gbXpressive *ASM* attempts to improve traditional assembly languages readability by introducing several *high-level* programming language statements. Statements *may or may not* lead to code generation and are designed to both improve the overall code's readability as well as to simplify commonly recurring tasks implemented by programmers. 

Most statements introduce their own ***keywords*** and syntactic and semantic rules, which are described in this chapter.

#### PACK

`PACK` declares a data structure type composed of one or more *identifiable* fields. Fundamentally, a `PACK` has two properties: an initial **address** and a **length**. A `PACK` is designed to simplify memory management in a syntactic level.

##### Syntax
``` language assembly
PACK <PACK_TYPE_IDENTIFIER>:
    [{FIELD TYPE} {FIELD_IDENTIFIER}]+
END
```
Each `PACK` must include an unique identifier and a non-empty list of fields. Fields are name-addressable memory locations, defined as bytes sequences of pre-defined size. Fields can be individually accessed by using the `PACK`'s *base-address* and an offset. The assembler is responsible to statically calculate the location of each field upon usage in the user's code. 

##### Example
``` language assembly
PACK SPRITE:
    BYTE TILE
    BYTE PALETTE
    BYTE X
    BYTE Y
    WORD ATTRIBUTES
END
```

The previous example depicts the declares a `PACK` type called SPRITE, which consists of five fields: TILE, PALETTE, X, Y, and ATTRIBUTES. The first four fields have been declared as `BYTE` which means they will take one byte each, when the `PACK` is allocated. ATTRIBUTE is a `WORD` type which means it takes 2 bytes when the `PACK` is allocated. To access the 

Once declared, a `PACK` type is an *instantiable-entity* that can be used with the `DECL`, `CONST`, `FREE` statements, `.` operand, `AS` type cast construction, initializer lists and instructions. Instantiated `PACK` are referred to as struct-variables.

##### Syntax
###### Field Access
``` language assembly
<PACK_IDENTIFIER>.<FIELD_IDENTIFIER>
```
###### Field Access in `PACK` Array
``` language assembly
<PACK_IDENTIFIER>[<INDEX>].<FIELD_IDENTIFIER>
```
###### `PACK` Instantiation with Initializer List
``` language assembly
DECL <PACK_IDENTIFIER> AS <PACK_TYPE_IDENTIFIER> @<ADDRESS_TYPE> [[[.<FIELD_IDENTIFIER>:<VALUE>]+]]*
```
###### `PACK` Instance Deletion
``` language assembly
FREE <PACK_IDENTIFIER>
```
###### `PACK` Constant definition with Initializer List
``` language assembly
CONST <PACK_IDENTIFIER> AS <PACK_TYPE_IDENTIFIER> {[.<FIELD_IDENTIFIER>:<VALUE>]+}
```

###### `PACK` Usage with Instruction
``` language assembly
OPCODE [<OPERAND>, ]* <PACK_IDENTIFIER>.<FIELD_IDENTIFIER> [, <OPERAND>]*
```
###### `PACK` in Type Cast
``` language assembly
[OPCODE|STATEMENT|DIRECTIVE] ... {[REGISTER_PAIR] AS <PACK_IDENTIFIER>.<FIELD_IDENTIFIER>} ...
```

Note that, the instantiation of a `PACK` with initializer lists **with an struct-variable**, incur in extra code generation. Field accesses however, are resolved in assembly time without the need of extra code generation. When using `PACK` fields as an instruction operand, data size must be taken into account. When using the `PACK` type in an Type Cast, extra code will be generated to correct the address of the register pairs.

##### Example
``` language assembly
    ...
    DECL    MY_SPRITE AS SPRITE @[GLOBAL_VARIABLES] {.X:H'00, .Y:H'00}
    LD      A, MY_SPRITE.X
    ADD     A, H'04
    LD      MY_SPRITE.X, A 
    FREE    MY_SPRITE
    ...
    LD      A, DEFAULT_PALETTE_INDEX
    LD      {HL AS SPRITE}.PALETTE, A
    ...
```

The previous example instantiates `PACK` SPRITE, with name MY_SPRITE and initializes two of its fields (X and Y). The instantiated struct-variable's X field is then loaded into the accumulator, which gets incremented by `H'04`. The accumulator is then written back to MY_SPRITE's field X. At the end, MY_SPRITE is freed, releasing its memory resources.  The second part of the example loads the accumulator with the constant DEFAULT_PALETTE_INDEX and then *casts* HL to the  `PACK` type SPRITE (more specifically, the assembler will interpret HL as the base address of a SPRITE  `PACK` type and will perform field access arithmetic by using the offsets derivable from `PACK` SPRITE structure).

#### `FUNC`
#### `END`
#### `DECL`
#### `BOOL`
#### `CHAR`
#### `BYTE`
#### `WORD`
#### `DWORD`
#### `STRING`
#### `AS`
##### `AS` for Variable Instantiation
##### `AS` for Type Cast
#### `CONST`
#### `FREE`
#### `IF`, `THEN`, `ELSE`
#### `CALL`
#### `REPEAT`
#### `TIMES`
#### `WHEN`
#### `IS`
#### `WHILE`
#### `ALIAS`
#### `UNALIAS`
#### `TRY`
#### `SOLVE`
#### `ABORT`
#### `TEST`

### Operators
#### Operator Precedence
#### Arithmetic, Logic and Bit-wise Operators
#### `+` Operator
#### `-` Operator
#### `/` Operator
#### `*` Operator
#### `+` Operator
#### `&` Operator
#### `|` Operator
#### `~` Operator
#### `^` Operator
#### `<<` Operator
#### `>>` Operator
#### `==` Operator
#### `!=` Operator
#### `&&` Operator
#### `||` Operator
#### `>` Operator
#### `<` Operator
#### `>=` Operator
#### `<=` Operator
#### `<=>` Operator
#### `!` Operator
#### Special Operators
#### `@` Operator
#### `:` Operator
#### `""` String constant Marker
### Instructions

### Pseudo Instructions
#### `STORE`
#### `LOAD`
#### `SWAP`
#### `MOVE`
### Identifiers
#### Hierarchy

### Numeric Representation
#### Decimal Numbers
##### `0d` Numric Prefix
#### Hexadecimal Numbers
##### `0x` Numric Prefix
#### Binary Numbers
##### `0b` Numric Prefix
#### Octal Numbers
##### `0o` Numric Prefix
#### ASCII Encoded Bytes
##### `''` Character Marker
#### Numeric Modifiers
##### `HIGH`, `H`
##### `LOW`, `L`
##### `BIT`, `B`
#### Boolean Constants `TRUE` and `FALSE`

### Directives
#### `.USE`
#### `.ORG`
#### `.PRINT`
#### `.BREAK`
#### `.BANK`
#### `.CART`
#### `.ADDON`
#### `.ASSERT`
#### `.FAIL`
#### `.PASS`
#### `.ROM`
#### `.RAM`
#### `.SRAM`
#### `.VRAM`
#### `.BANK`

### Macros

### Other Language Structures
#### Comments
#### Labels
#### Initializer Lists `{}`
#### Array Index Subscript `[]`

### Basic ABI
#### Function Call Conventions
#### Stack Usage