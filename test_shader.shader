;   LOW: 0xGGRR  HIGH: 0xFFBB

; Color (opaque red)
OP_MOV_LOW_IMM16,  R7, 0x00FF
OP_MOV_HIGH_IMM16, R7, 0xFF00

; X bounds: left=270, right=370
OP_MOV_LOW_IMM16,  R4, 270
OP_MOV_LOW_IMM16,  R5, 370

; insideX_left = (270 < X) -> R3
OP_MOV_LOW_IMM16,  R3, 0
OP_ADD_REG,        R3, R4
OP_SLT_REG,        R3, R1

; insideX_right = (X < 370) -> R6
OP_MOV_LOW_IMM16,  R6, 0
OP_ADD_REG,        R6, R1
OP_SLT_REG,        R6, R5

; insideX = insideX_left * insideX_right -> R3
OP_MULT_REG,       R3, R6

; Y bounds: top=190, bottom=290  (reuse R4/R5)
OP_MOV_LOW_IMM16,  R4, 190
OP_MOV_LOW_IMM16,  R5, 290

; insideY_top = (190 < Y) -> R6
OP_MOV_LOW_IMM16,  R6, 0
OP_ADD_REG,        R6, R4
OP_SLT_REG,        R6, R2

; insideY_bottom = (Y < 290) -> R0
OP_MOV_LOW_IMM16,  R0_COLOR_OUT, 0
OP_ADD_REG,        R0_COLOR_OUT, R2
OP_SLT_REG,        R0_COLOR_OUT, R5

; insideY = insideY_top * insideY_bottom -> R6
OP_MULT_REG,       R6, R0_COLOR_OUT

; insideBox = insideX * insideY -> R3
OP_MULT_REG,       R3, R6

; Output color = Color * insideBox
OP_MOV_LOW_IMM16,  R0_COLOR_OUT, 0
OP_ADD_REG,        R0_COLOR_OUT, R7
OP_MULT_REG,       R0_COLOR_OUT, R3

OP_STORE_PIXEL,    0, 0
OP_END,            0, 0