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
PACK <PACK_TYPE_IDENTIFIER>
BGN
    [{FIELD TYPE} {FIELD_IDENTIFIER}]+
END
```
Each `PACK` must include an unique identifier and a non-empty list of fields. Fields are name-addressable memory locations, defined as bytes sequences of pre-defined size. Fields can be individually accessed by using the `PACK`'s *base-address* and an offset. The assembler is responsible to statically calculate the location of each field upon usage in the user's code. 

##### Example
``` language assembly
PACK SPRITE
BGN
    BYTE TILE
    BYTE PALETTE
    BYTE X
    BYTE Y
    WORD ATTRIBUTES
END
```

The previous example depicts the declares a `PACK` type called SPRITE, which consists of five fields: TILE, PALETTE, X, Y, and ATTRIBUTES. The first four fields have been declared as `BYTE` which means they will take one byte each, when the `PACK` is allocated. ATTRIBUTE is a `WORD` type which means it takes 2 bytes when the `PACK` is allocated. To access the 

Once declared, a `PACK` type is an *instantiable-entity* that can be used with the `VAR`, `CNST`, `FREE` statements, `.` operand, `AS` type cast CNSTruction, initializer lists and instructions. Instantiated `PACK` are referred to as struct-variables.

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
VAR <PACK_IDENTIFIER> AS <PACK_TYPE_IDENTIFIER> @<ADDRESS_TYPE> [[[.<FIELD_IDENTIFIER> = <VALUE>]+]]*
```
###### `PACK` Instance Deletion
``` language assembly
FREE <PACK_IDENTIFIER>
```
###### `PACK` CNSTant definition with Initializer List
``` language assembly
CNST <PACK_IDENTIFIER> AS <PACK_TYPE_IDENTIFIER> {[.<FIELD_IDENTIFIER> = <VALUE>]+}
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
    VAR     MY_SPRITE IS SPRITE @[GLOBAL_VARIABLES] {.X = H'00, .Y = H'00}
    LD      A, MY_SPRITE.X
    ADD     A, H'04
    LD      MY_SPRITE.X, A 
    FREE    MY_SPRITE
    ...
    LD      A, DEFAULT_PALETTE_INDEX
    LD      {HL AS SPRITE}.PALETTE, A
    ...
```

The previous example instantiates `PACK` SPRITE, with name MY_SPRITE and initializes two of its fields (X and Y). The instantiated struct-variable's X field is then loaded into the accumulator, which gets incremented by `H'04`. The accumulator is then written back to MY_SPRITE's field X. At the end, MY_SPRITE is freed, releasing its memory resources.  The second part of the example loads the accumulator with the CNSTant DEFAULT_PALETTE_INDEX and then *casts* HL to the  `PACK` type SPRITE (more specifically, the assembler will interpret HL as the base address of a SPRITE  `PACK` type and will perform field access arithmetic by using the offsets derivable from `PACK` SPRITE structure).

#### `FUNC`


Functions are an extension of the concept of 'function-labels' in standard assembly. They allow for expressive function calls with the `CALL` instruction and the `WITH` construction. Functions accept input and output argument lists that are automatically managed by the assembler in assembly time. They also allow for static type-check when used with the optinal `AS` keyword (allowed types are: `BOOL`, `CHAR`, `BYTE`, `WORD`, `DWORD` `STR`, their respective array types or any user-defined `PACK` type). The body of a function works exactly as their standard assembly counter-parts. 

##### Example
``` language assembly
FUNC MY_FUNCTION
	IN: ARGUMENT_1 AS BYTE
	IN: ARGUMENT_2
	OUT: RETURN_1
	OUT: RETURN_2 AS WORD
BGN

	; FUNCTION BODY

END
```

In the previous example, function `MY_FUNCTION` is declared accepting two input arguments (`ARGUMENT_1` and `ARGUMENT_2`) and two output argments (`RETURN_1` and `RETURN_2`). Input argument `ARGUMENT_1` has been defined with type `BYTE` while output argument `RETURN_2` has  type `WORD`. The assembler will issue type-mismatch warnings whenever argument types do not match during a function call or when using output arguments. There is no theoritical limit on the number of input and output arguments. 

#### `BGN`
#### `END`
#### `VAR`
#### `BOOL`
#### `CHAR`
#### `BYTE`
#### `WORD`
#### `DWRD`
#### `STR`
#### `AS`
##### `AS` for Type Cast
#### `CNST`
#### `FREE`
#### `IF` 
#### `THEN` 
#### `ELSE`
#### `WITH`
##### `WITH` for Function calls
##### `WITH` for Function return
#### `REPT`
#### `NEXT`
#### `EXIT`
#### `WHEN`
switch-case style selection block.

``` language assembly
    WHEN A
    BGN
        IS 0x01: ; Do SOMETRHING;
                 BRK
        IS 0x02: ; Do SOMETRHING;
                 BRK
        DFLT:    ; Do SOMETRHING (DEFAULT);
                 BRK
    END
```

#### `BRK`
#### `DFLT`
#### `IS`
##### `IS` for Variable Instantiation
##### `IS` with `WHEN` selection block
#### `FOR`
Repeat **for** a fixed amount of iterations.

##### Example
``` language assembly
    REPT FOR 0x100
    BGN
        ... ; Loop body
    END
```

#### `UNTL`
Repeat **until** a condition is met.

``` language assembly
    REPT UNTL MEMORY_REGISTER != 0x01
    BGN
        ... ; Loop body
    END
```

#### `TAG`
Give a _temporary_ identifier to a variable, constant register or memory location.

``` language assembly
    TAG HL, POINTER
```

#### `TRY`
_Throwable_ block initializer (happy flow block).

``` language assembly
    TRY
    BGN
        ... ; Throwable block
    EXPT
    BGN
        ... ; Exception Handling
    END
```
#### `EXPT`
_Exception_ block initializer (unhappy flow block).

``` language assembly
    TRY
    BGN
        ... ; Throwable block
    EXPT
    BGN
        ... ; Exception Handling
    END
```

#### `THRW`
Exception throwing construction

``` language assembly
    ...
    ; Error happened, Abort
    TRHW 0x0A
    ...
```
#### `IN`
#### `OUT`

### Operators
#### Operator Precedence
#### Arithmetic, Logic and Bit-wise Operators
#### `+` Operator
#### `-` Operator
#### `/` Operator
#### `*` Operator
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
#### `.` Operator
### Instructions
#### `NOP`
#### `LD`
#### `INC`
#### `DEC`
#### `RLCA`
#### `ADD`
#### `RRCA`
#### `STOP`
#### `RLA`
#### `JR`
#### `RRA`
#### `DAA`
#### `CPL`
#### `SCF`
#### `CCF`
#### `HALT`
#### `ADC`
#### `SUB`
#### `SBC`
#### `AND`
#### `XOR`
#### `OR`
#### `CP`
#### `RET`
#### `POP`
#### `JP`
#### `PUSH`
#### `CALL`
#### `RST`
#### `EI`
#### `DI`
#### `RLC`
#### `RRC`
#### `RL`
#### `RR`
#### `SLA`
#### `SRA`
#### `SWAP`
#### `SRL`
#### `BIT`
#### `RES`
#### `SET`
### Pseudo Instructions
#### `MOVE`
### Identifiers


### Literals
### Numeric Literals
#### Decimal Numbers
##### `0d` Numric Prefix
#### Hexadecimal Numbers
##### `0x` Numric Prefix
#### Binary Numbers
##### `0b` Numric Prefix
#### Octal Numbers
##### `0o` Numric Prefix

#### Numeric Modifiers
##### `HIGH`
##### `LOW`
##### `[]`

### Boolean Literals
#### `TRUE`
#### `FALSE`
### String and Character literals
#### `''` Character Marker
#### `""` String Marker

### Seperators
#### `()`

#### `{}`
#### `[]`
#### `,`

### Directives
#### `.IFDEF`
Conditional assembly block. This directive instructs the pre-processor to include any code placed the `.IFDEF` ... [`ELSE`|`.END`], given that its condition evaluates true.

##### Example
``` language assembly
    .IFDEF PRE_PROCESSOR_SYMBOL
    .BGN
        ... ; The content of this block *will not* be removed if PRE_PROCESSOR_SYMBOL is 
            ; found defined in any source file or is provided as command-line argument.
    .END
```
#### `.ELSE`
Complementary symbol to `.IFDEF`. The code contained within this block will only be included if the condition in `.IFDEF` does not evaluate true.

##### Example
``` language assembly
    .IFDEF NON_EXISTING_PRE_PROCESSOR_SYMBOL
    .BGN
        ... ; Only included if NON_EXISTING_PRE_PROCESSOR_SYMBOL is defined.
    .ELSE
    .BGN
        ... ; This code *will* be included otherwise.
    .END
```
### `.END`
Block-ending keyword used with `.IFDEF` and `.ELSE`.

#### `.IFNDEF`
Conditional assemly block. The opposite of `.IFDEF`.


##### Example
``` language assembly
    .IFNDEF PRE_PROCESSOR_SYMBOL
    .BGN
        ... ; The content of this block *will* be removed if PRE_PROCESSOR_SYMBOL is 
            ; found defined in any source file or is provided as command-line argument.
    .END
```

#### .DEF
Pre-processor symbol definition directive. `.DEF` only accepts an identifier to fully-define a pre-processing symbol. Identifiers are not required to be unique. Using `.DEF` with an already defined pre-compilation symbol has no effect.

##### Example
``` language assembly
    .DEF PLATFORM_GBX ; Defines a pre-processor symbol with identifier 'PLATFORM_GBX'
```

#### .UNDEF
**Undefine** a previously defined pre-compilation symbol. Using `.UNDEF` with a non-defined pre-processor symbol has no effect.

##### Example
``` language assembly
    .UNDEF PLATFORM_GBX ; Undefine a pre-compulation symbol with identifier 'PLATFORM_GBX'
```


#### `.PRINT`

#### `.USE`
#### `.ORG`
#### `.BREAK`
#### `.BANK`
#### `.CART`
#### `.ADDON`
#### `.ROM`
#### `.RAM`
#### `.SRAM`
#### `.VRAM`
#### `.BANK`
#### `.MACRO`
#### `.TEST`

### Macros

### Other Language Structures
#### Comments
#### Labels

### Basic ABI
#### Function Call Conventions
#### Stack Usage