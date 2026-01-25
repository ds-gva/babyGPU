    LDC, R0_COLOR_OUT, 0      ; Load red from constant[0] (assume its set to red)
    JMP, skip_green           ; Jump over the green color
    LDC, R0_COLOR_OUT, 1      ; Load green from constant[1] (assume its set to green)- SHOULD BE SKIPPED 
skip_green:
    STORE_PIXEL
    END