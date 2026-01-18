# babyGPU

A very basic and super small attempt at building a GPU simulator (or at least simualtor of some features).

Right now, I'm trying to have a basic simulation of SIMT execution, 32 thread warps, register files per-lane, basic code memory and abiltiy to load a shader into the fake ram of our GPU

## In place
- Basic ISA, "compiler", and "GPU core"
- Actual display is abstracted to a GLFW + OpengGL 1.1 texture on which we paint our pixel buffer (for simplicity)
- Basic compiler for "assembly-like" shaders, reading from external files (comment stripping, tokenization, error messages)
- All using uint - given there's no fp its definitely not realistic

## Immediate To dos
- add a OP_JMP and OP_JNZ to start making our language more powerful
- Add uniforms
- Consider floats

