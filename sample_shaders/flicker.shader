    LDC, R7, 0              ;load color from constant 0
    MOV, R0_COLOR_OUT, R7   ;move our color to R0
    STORE_PIXEL             ;store pixels from R0
    END