# GAME BOY X Debugger Message Protocol

This document describes the messaging protocol of the GAME BOY X platform. The debugging flow is controlled by message exchanged between a **Debug Server** and **Debug Client**. The server does not spontanously starts communication with the client. A client is always initializing a transaction were one or more request-response pairs are exchanged. 

Requests are divided into four groups: **Control Requests**, **Status Requests**, **Data Requests** and **Error Requests**. The following sections describe each request falling in each group.

## Control Requests

#### Breakpoint [Conditional-] (Create/Remove)
#### Watchpoint (Create/Remove)
#### Step Over
#### Step Into
#### Step Out
#### Run Until
#### Run
#### Continue
#### Stop
#### Reset


## Status Requests
#### Interrupts Status 
#### Peripherals Status
#### Framerate Status
#### Read Log


## Data Requests
#### Read/Write Register
#### Read/Write Status Flag
#### Read/Write Memory


## Error Requests
#### Get Last Error


