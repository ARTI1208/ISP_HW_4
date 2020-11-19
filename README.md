# ISP_DZ4 (Processor)

Simple processor realisation with assembler and disassembler. Under construction

## Build
On linux, you can build using CMake
    
    cmake -S ./ -B out && cmake --build out

## Run
Run CPU, filename is path to code in pseudo-asm
   
    out/cpu <filename>

or
   
    out/executable e <filename>

----
    
Run assembler, input is path to file with code in pseudo-asm, 
output - path to file where bytecodes will be written
   
    out/asm <input> <output>

or
   
    out/executable a <input> <output>

----
    
Run disassembler, input is path to file with bytecode, 
output - path to file where code in pseudo-asm will be written
   
    out/disasm <input> <output>

or
   
    out/executable d <input> <output> 

There are example files in assets folder

#Available commands

| Mnemonic  | Op code (hex) | Description  |
| --------- |:-------------:| ------------ |
| add       | 00            | Pops two last values from stack and pushes their sum |
| sub       | 01            | Pops two last values from stack and pushes their subtraction result (first popped - subtrahend, second - minuend) |
| mul       | 02            | Pops two last values from stack and pushes their multiplication result |
| div       | 03            | Pops two last values from stack and pushes their division result (first popped - divisor, second - dividend) |
| sqrt      | 04            | Pops last value from stack and pushes it's square root |
| in        | 05            | Reads double value from console and pushes to stack |
| out       | 06            | Pops last value from stack and writes to console |
| pop       | 07            | Pops last value from stack |
| push      | 08            | Pushes double value to stack |
| pop       | 09            | Pops last value from stack and writes to given register (ax, bx, cx or dx) |
| push      | 0a            | Pushes value of given register (ax, bx, cx or dx) to stack |
| jmp       | 0b            | Unconditionally jumps to the label |
| ifl       | 0c            | Pops two last values from stack and jumps to given label if second popped value is less then the first one |
| ifle      | 0d            | Pops two last values from stack and jumps to given label if second popped value is less then or equal to the first one |
| ifg       | 0e            | Pops two last values from stack and jumps to given label if second popped value is greater then the first one |
| ifge      | 0f            | Pops two last values from stack and jumps to given label if second popped value is greater then or equal to the first one |
| ife       | 10            | Pops two last values from stack and jumps to given label if second popped value is equal to the first one |
| ifne      | 11            | Pops two last values from stack and jumps to given label if second popped value is not equal to the first one |
| call      | 12            | Saves current position in call stack and jumps to given label |
| ret       | 13            | Returns to the last saved position in call stack, just after the last executed call command |
| hlt       | 14            | Stops the execution |

#Other
Show help
   
    out/executable -h

----

Run unit tests
   
    out/tests
   
## Documentation
You can find documentation at https://arti1208.github.io/ISP_DZ4/html/
