# ISP_DZ4 (Processor)

Simple processor realisation with assembler and disassembler 

## Build
On linux, you can build using CMake
    
    cmake -S ./ -B out && cmake --build out

## Run
Run CPU, filename is path to code in pseudo-asm
   
    out/executable e <filename>

----
    
Run assembler, input is path to file with code in pseudo-asm, 
output - path to file where bytecodes will be written

    out/executable a <input> <output>

----
    
Run disassembler, input is path to file with bytecode, 
output - path to file where code in pseudo-asm will be written

    out/executable d <input> <output> 

There are example files in assets folder

#Other
Show help
   
    out/executable -h

----

Run unit tests
   
    out/tests
   
## Documentation
You can find documentation at https://arti1208.github.io/ISP_DZ4/html/
