# babyGPU

<img src="./header.png" />

babyGPU is a work in progress (and very early stage) educational GPU instructions simulator built in C. At the moment the hardware-side is simulated using C-code (rather than building from first principles and gates), so the main learning aspect is around using  assembly-like instructions to manipulate the GPU memory and write to buffer. Right now, I'm trying to have a basic simulation of SIMT execution, 32 thread warps, register files per-lane, basic code memory and abiltiy to load a shader into the fake ram of our GPU.

Runs only on windows. Have not spent any time on the build side of things. All disclaimers apply.

## In place
- Logical separation of "hardware", "driver", "application"
- Ability to write, assemble and run basic fragment shaders ; uncdotional jumps and labels are in place
- Non-turing complete assembly (missing conditional jumps, etc...)
- Basic ISA, "compiler", and "GPU core"
- Actual display is abstracted to a GLFW + OpengGL 1.1 texture on which we paint our pixel buffer (for simplicity)
- Basic assembler for "assembly-like" shaders, reading from external files (comment stripping, tokenization, error messages)

## Register
At the moment babyGPU only has 10 registers (but the plan is to add support for more flexible number of registers going forward)
- **R0_COLOR_OUT**: this is a special register where the screen buffer is read (i.e., colors per pixel)
- **R1**: this is initalized with scaled x coordinates (640x480 fixed res)
- **R2**: this is initalized with scaled y coordinates
- **R3-R9**: these are free registers

## ISA Operations

### Load
**MOV_LOW_IMM8** - *Load value into bottom 8 bits (preserve top)* - MOV_LOW_IMM8, DST, IMM8  
**MOV_HIGH_IMM8** - *Load value into top 8 bits (preserves bottom)* - MOV_HIGH_IMM8, DST, IMM8  
**LDC** - *Load constant into destination register, constant is defined in application* - LDC, DST, IMM8  
**LDI** - *Load immediate value into destination* - LDI, DST, IMM8  

### Move
**MOV** - *Move value from one register to another* MOV, DST; SRC0

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

## Sample programs
1. Flicker shader
2. Red Square
3. Horizontal color-split
4. Green gradient
(more to come...)

## How to build
Check out quick_build_and_launch.bat (or the tasks.json for vscode) ; haven't put much thought into it up until now, so its basically just working on my local setup.

## Immediate To dos
- Add JNZ
- Add masking for conditional execution across threads
- Consider floats


