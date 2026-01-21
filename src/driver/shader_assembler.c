#include "shader_assembler.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>


#define MAX_TOKS 4;

// Use this to convert opcodes to their enum values
static const OpcodeEntry kOpcodes[] = {
    {"MOV_LOW_IMM8",  MOV_LOW_IMM8},
    {"MOV_HIGH_IMM8", MOV_HIGH_IMM8},
    {"LDC",            LDC},
    {"LDI",            LDI},
    {"MOV",            MOV},

    {"ADD",            ADD},
    {"ADDI",           ADDI},
    {"MULT",           MULT},
    {"MULTI",          MULTI},
    {"DIV",            DIV},
    {"DIVI",           DIVI},

    {"SLT",            SLT},
    {"STORE_PIXEL",    STORE_PIXEL},
    {"END",            END},

};
// Use this to convert registers to their enum values
static const RegEntry kRegs[] = {
    {"R0_COLOR_OUT", R0_COLOR_OUT},
    {"R1",           R1},
    {"R2",           R2},
    {"R3",           R3},
    {"R4",           R4},
    {"R5",           R5},
    {"R6",           R6},
    {"R7",           R7},
    {"R8",           R8},
};


static char* trim(char *s) {
    if (!s) return s;

    // skip leading whitespace
    while (*s && isspace((unsigned char)*s)) s++;

    // if all spaces, return empty string
    if (*s == '\0') return s;

    // find end
    char *end = s + strlen(s) - 1;
    while (end > s && isspace((unsigned char)*end)) end--;

    // write new terminator
    end[1] = '\0';
    return s;
}

static int contains_whitespace(const char *s) {
    if (!s) return 0;
    for (; *s; s++) {
        if (isspace((unsigned char)*s)) return 1;
    }
    return 0;
}

static int parse_reg(const char *s, uint8_t *out) {
    for (size_t i = 0; i < sizeof kRegs / sizeof kRegs[0]; i++) {
        if (strcmp(s, kRegs[i].name) == 0) {
            *out = kRegs[i].reg;
            return 1;
        }
    }
    return 0;
}

static int parse_opcode(const char *s, uint8_t *out) {
    for (size_t i = 0; i < sizeof kOpcodes / sizeof kOpcodes[0]; i++) {
        if (strcmp(s, kOpcodes[i].name) == 0) {
            *out = kOpcodes[i].opcode;
            return 1;
        }
    }
    return 0; // unknown opcode
}

static int parse_imm8(const char *s, uint8_t *out) {
    char *end;
    unsigned long v = strtoul(s, &end, 0);
    if (end == s) return 0;      // no digits
    if (*end != '\0') return 0;  // trailing junk
    if (v > 255) return 0;       // out of range
    *out = (uint8_t)v;
    return 1;
}

static int is_blank_line(const char *s) {
    for (; *s; s++) {
        if (!isspace((unsigned char)*s)) return 0;
    }
    return 1;
}

static int copy_token(char dst[64], char *tok) {
    tok = trim(tok);
    if (!tok || tok[0] == '\0') return 0;
    if (contains_whitespace(tok)) return 0;
    strncpy(dst, tok, 63);
    dst[63] = '\0';
    return 1;
}

InstructionTextList read_shader_text(const char *filename) {
    InstructionTextList out = {0};
    FILE *fp = fopen(filename, "r");
    if (!fp) return out;

    size_t cap = 0;
    char line[1024];
    int linecount = 0;

    while (fgets(line, sizeof line, fp)) {
        linecount++;

        // Strip comment
        char *semi = strchr(line, ';');
        if (semi) *semi = '\0';

        // Skip blank lines
        if (is_blank_line(line)) continue;

        // Grow buffer if needed
        if (out.len == cap) {
            size_t new_cap = cap ? cap * 2 : 16;
            void *p = realloc(out.data, new_cap * sizeof *out.data);
            if (!p) { free(out.data); out.data = NULL; out.len = 0; return out; }
            out.data = p;
            cap = new_cap;
        }

        InstructionText *inst = &out.data[out.len];
        inst->count = 0;
        inst->line_num = linecount;

        // Tokenize by comma (up to 4 tokens)
        char *t = strtok(line, ",");
        while (t != NULL && inst->count < 4) {
            t = trim(t);
            strncpy(inst->tok[inst->count], t, 63);
            inst->tok[inst->count][63] = '\0';
            inst->count++;
            t = strtok(NULL, ",");
        }

        // If there's a 5th token, that's an error (too many operands)
        if (t != NULL) {
            printf("Shader Error line %d: too many tokens\n", linecount);
            free(out.data); out.data = NULL; out.len = 0;
            fclose(fp);
            return out;
        }

        out.len++;
    }

    fclose(fp);
    return out;
}

struct Instruction* assemble_shader(InstructionTextList list, int *prog_size) {
   *prog_size = (int)list.len;

    struct Instruction *shader = malloc(*prog_size * sizeof(struct Instruction));
    if (!shader) return NULL;

    for (size_t i = 0; i < *prog_size; i++) {
        InstructionText *t = &list.data[i];
        struct Instruction *inst = &shader[i];

        inst->opcode = 0;
        inst->dst = 0;
        inst->src0 = 0;
        inst->src1_or_imm8 = 0;

        if (t->count < 1) {
            printf("Error line %d: empty instruction\n", t->line_num);
            free(shader); return NULL;
        }

        uint8_t opcode;
        if (!parse_opcode(t->tok[0], &opcode)) {
            printf("Error line %d: unknown opcode '%s'\n", t->line_num, t->tok[0]);
            free(shader); return NULL;
        }
        inst->opcode = opcode;

        switch (opcode) {

            case END:
            case STORE_PIXEL:
                if (t->count != 1) {
                    printf("Error line %d: %s takes no operands\n", t->line_num, t->tok[0]);
                    free(shader); return NULL;
                }
                break;

            case MOV:
                // MOV dst, src0
                if (t->count != 3) {
                    printf("Error line %d: MOV requires 2 operands (dst, src)\n", t->line_num);
                    free(shader); return NULL;
                }
                if (!parse_reg(t->tok[1], &inst->dst))  { printf("Error line %d: invalid dst '%s'\n", t->line_num, t->tok[1]); free(shader); return NULL; }
                if (!parse_reg(t->tok[2], &inst->src0)) { printf("Error line %d: invalid src '%s'\n", t->line_num, t->tok[2]); free(shader); return NULL; }
                break;

            case MOV_LOW_IMM8:
            case MOV_HIGH_IMM8:
            case LDC:
                // dst, imm8
                if (t->count != 3) {
                    printf("Error line %d: %s requires 2 operands (dst, imm)\n", t->line_num, t->tok[0]);
                    free(shader); return NULL;
                }
                if (!parse_reg(t->tok[1], &inst->dst)){
                     printf("Error line %d: invalid dst '%s'\n", t->line_num, t->tok[1]);
                     free(shader); return NULL;
                    }
                if (!parse_imm8(t->tok[2], &inst->src1_or_imm8)){
                    printf("Error line %d: invalid imm '%s'\n", t->line_num, t->tok[2]);
                    free(shader); return NULL;
                }

                break;
            case LDI:
                // dst, imm8
                if (t->count != 3) {
                    printf("Error line %d: %s requires 2 operands (dst, imm)\n", t->line_num, t->tok[0]);
                    free(shader); return NULL;
                }
                if (!parse_reg(t->tok[1], &inst->dst)) {
                    printf("Error line %d: invalid dst '%s'\n", t->line_num, t->tok[1]);
                    free(shader);
                    return NULL;
                }
                if (!parse_imm8(t->tok[2], &inst->src1_or_imm8)){ 
                    printf("Error line %d: invalid imm '%s'\n", t->line_num, t->tok[2]);
                    free(shader);
                    return NULL; 
                }
                break;

            case ADD:
            case MULT:
            case DIV:
            case SLT:
                // dst, src0, src1 (all regs)
                if (t->count != 4) {
                    printf("Error line %d: %s requires 3 operands (dst, src0, src1)\n", t->line_num, t->tok[0]);
                    free(shader); return NULL;
                }
                if (!parse_reg(t->tok[1], &inst->dst))            { printf("Error line %d: invalid dst\n",  t->line_num); free(shader); return NULL; }
                if (!parse_reg(t->tok[2], &inst->src0))           { printf("Error line %d: invalid src0\n", t->line_num); free(shader); return NULL; }
                if (!parse_reg(t->tok[3], &inst->src1_or_imm8))   { printf("Error line %d: invalid src1\n", t->line_num); free(shader); return NULL; }
                break;

            case ADDI:
            case MULTI:
            case DIVI:
                // dst, src0, imm8
                if (t->count != 4) {
                    printf("Error line %d: %s requires 3 operands (dst, src0, imm)\n", t->line_num, t->tok[0]);
                    free(shader); return NULL;
                }
                if (!parse_reg(t->tok[1], &inst->dst))           { printf("Error line %d: invalid dst\n",  t->line_num); free(shader); return NULL; }
                if (!parse_reg(t->tok[2], &inst->src0))          { printf("Error line %d: invalid src0\n", t->line_num); free(shader); return NULL; }
                if (!parse_imm8(t->tok[3], &inst->src1_or_imm8)) { printf("Error line %d: invalid imm\n",  t->line_num); free(shader); return NULL; }
                break;

            default:
                printf("Error line %d: unhandled opcode %d\n", t->line_num, opcode);
                free(shader); return NULL;
        }
    }
    return shader;
}
