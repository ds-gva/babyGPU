; test_jmp.shader - Jump over an instruction

JMP, 2                    ; 1: Skip next instruction, jump to index 3
LDC, R0_COLOR_OUT, 0      ; 0: Load color from constant[0]
LDC, R0_COLOR_OUT, 1      ; 2: Load color from constant[1] - SHOULD BE SKIPPED
STORE_PIXEL               ; 3: Store pixel
END                       ; 4: Done