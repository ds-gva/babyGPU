# babyGPU

A very basic and super small attempt at building a GPU simulator (or at least simualtor of some features).

Right now, I'm trying to have a basic simulation of SIMT execution, 32 thread warps, register files per-lane, basic code memory and abiltiy to load a shader into the fake ram of our GPU

## In place
- Logical separation of "hardware", "driver", "application"
- Ability to write, assemble and run basic fragment shaders
- Non-turing complete assembly (missing jumps, etc...)
- Basic ISA, "compiler", and "GPU core"
- Actual display is abstracted to a GLFW + OpengGL 1.1 texture on which we paint our pixel buffer (for simplicity)
- Basic assembler for "assembly-like" shaders, reading from external files (comment stripping, tokenization, error messages)

## ISA Operations

### Load
**MOV_LOW_IMM8** - *Load value into bottom 8 bits (preserve top)* - MOV_LOW_IMM8, DST, IMM8
**MOV_HIGH_IMM8** - *Load value into top 8 bits (preserves bottom)* - MOV_HIGH_IMM8, DST, IMM8
**LDC** - *Load constant into destination register, constant is defined in application* - LDC, DST, IMM8
**LDI** - *Load immediate value into destination* - LDI, DST, IMM8

### Move
MOV

### Arithmetics
ADD
ADDI
MULT
MULTI
DIV
DIVI

### Logic
SLT

### Branching
JMP

### Store
STORE_PIXEL

### End
END

## Immediate To dos
- Change assembler to add labels allowing better use of jumps
- Add JNZ
- Add masking for conditional execution across threads
- Consider floats


