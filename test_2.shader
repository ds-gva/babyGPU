; Load constants from constant buffer
LDC, R7, 0          ; R7 = color (red)
LDC, R4, 1          ; R4 = 270 (left X)
LDC, R5, 2          ; R5 = 370 (right X)

; insideX_left = (270 < X)
SLT, R3, R4, R1     ; R3 = (R4 < R1) = (270 < X) ? 1 : 0

; insideX_right = (X < 370)
SLT, R6, R1, R5     ; R6 = (R1 < R5) = (X < 370) ? 1 : 0

; insideX = insideX_left AND insideX_right
MULT, R3, R3, R6    ; R3 = R3 * R6

; Load Y bounds
LDC, R4, 3          ; R4 = 190 (top Y)
LDC, R5, 4          ; R5 = 290 (bottom Y)

; insideY_top = (190 < Y)
SLT, R6, R4, R2     ; R6 = (R4 < R2) = (190 < Y) ? 1 : 0

; insideY_bottom = (Y < 290)
SLT, R8, R2, R5     ; R8 = (R2 < R5) = (Y < 290) ? 1 : 0

; insideY = insideY_top AND insideY_bottom
MULT, R6, R6, R8    ; R6 = R6 * R8

; insideBox = insideX AND insideY
MULT, R3, R3, R6    ; R3 = insideX * insideY (0 or 1)

; Output = color * insideBox
MULT, R0_COLOR_OUT, R7, R3

STORE_PIXEL
END