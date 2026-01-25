; In current implementation: R1 holds X coordinate (provided by hardware)

; Constants to be loaded
LDC, R3, 0           ; R3 becomes 256 for everyone
LDC, R4, 1           ; R4 becomes 0xFF000000 (Full Alpha)
LDC, R5, 2           ; R5 holds 640 (screen width)

; doing (X*255) / Width ; this ensures R6 is always between 0 and 255 for our screen width
MULTI, R6, R1, 255  ; R6 = X * 255
DIV, R6, R6, R5  ; R6 Divided by width (R5)

MULT, R7, R6, R3     ; R7 = NormalizedGreen (R6) * 256          

; 4. Combine them
ADD, R0_COLOR_OUT, R4, R7 ; Result: Alpha | Green

STORE_PIXEL

END