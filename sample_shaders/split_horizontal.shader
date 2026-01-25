; split_horizontal.shader
LDC, R5, 0                ; R5 = 240 (midpoint Y)
LDC, R6, 1                ; R6 = color A
LDC, R7, 2                ; R7 = color B

SLT, R3, R5, R2           ; R3 = (240 < Y) = bottom half
SLT, R4, R2, R5           ; R4 = (Y < 240) = top half

MULT, R6, R6, R4
MULT, R7, R7, R3
ADD, R0_COLOR_OUT, R6, R7

STORE_PIXEL
END