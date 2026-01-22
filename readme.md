# babyGPU

A very basic and super small attempt at building a GPU simulator (or at least simualtor of some features).

Right now, I'm trying to have a basic simulation of SIMT execution, 32 thread warps, register files per-lane, basic code memory and abiltiy to load a shader into the fake ram of our GPU

## In place
- Logical separation of "hardware", "driver", "application"
- Ability to write, assemble and run basic fragment shaders
- Non-turing complete assembly (missing jumps, etc...)
- Basic ISA, "compiler", and "GPU core"
- Actual display is abstracted to a GLFW + OpengGL 1.1 texture on which we paint our pixel buffer (for simplicity)
- Basic compiler for "assembly-like" shaders, reading from external files (comment stripping, tokenization, error messages)

## Immediate To dos
- add a JMP and JNZ to start making our language more powerful
- Consider floats

