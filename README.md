# GAME BOY X DEBUGGER MESSAGE DEFINITION
This document describes the message exchange protocol used in the GAME BOY X Platform. Debug server and clients must implement the *required* aspects of this protocol in order to be able to fully implement GAME BOY X Debugging functionality.

## Contents
  - [Conventions](#conventions)
  - [DebugServer](#debugserver)
    - [ClientConnectedMessage](#clientconnectedmessage)
    - [DebugServerStatusMessage](#debugserverstatusmessage)
  - [DebugClient](#debugclient)
  - 

## Conventions
1. Bytes are stored in the message body in `Little-Endian` format.

## DebugServer
### ClientConnectedMessage
`ClientConnectedMessage` informs the server that a client has joined. This message requires no response.
  
#### _Request_ Message Format

Byte _(0-based)_|  Value _(16-bit)_ | Description
--- | --- | ---
0x0000 | `0xFFFF` | *Request* Message ID
     
#### _Response_ Message Format

No response required.


### DebugServerStatusMessage
`DebugServerStatusMessage` informs the current Debug Server status. The Debug Server can be in one out of three possible states:
  
1. `Halted`: The server is currently not running. In other words, there are no machine cycles or peripherals executing.
2. `Running`: The server is currently executing. Code execution can be stopped by issuing a `Stop`, `Reset` or `Break` command.
3. `Error`: An internal error has been issued by the server's core and execution is halted. The client can take actions to resume from the error condition.

#### _Request_ Message Format

Byte _(0-based)_|  Value _(16-bit)_ | Description
--- | --- | ---
0x0000 | `0xFFFE` | *Request* Message ID
     
#### _Response_ Message Format

Byte _(0-based)_ |  Value _(16-bit)_ | Description
--- | --- | ---
0x0000 | `0xFFFE` | *Response* Message ID
0x0002 | `enum::ServerState` | Debug server state

Where `enum::ServerState` assumes values:
Value | Description
--- | --- 
0x0001 | `Halted`
0x0002 | `Running`
0x0002 | `Error`

## DebugClient