#include "shader_compiler.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

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

static int parse_value(const char *s, uint16_t *out) {
    char *end;
    unsigned long v = strtoul(s, &end, 0);
    if (end == s) return 0;
    if (*end != '\0') return 0;
    if (v > 0xFFFF) return 0;

    *out = (uint16_t)v;
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

// TO DO: parse and compile in one pass so we get better error messages?

InstructionTextList read_shader_text(const char *filename) {
    int linecount = 0;
    InstructionTextList out = {0};

    FILE *fp = fopen(filename, "r");
    if (!fp) return out;

    size_t cap = 0;
    char line[1024];

    while (fgets(line, sizeof line, fp)) {
        linecount++;

        char original[1024];
        strncpy(original, line, sizeof original - 1);
        original[sizeof original - 1] = '\0';

        // Strip comment (semicolon starts comment)
        char *semi = strchr(line, ';');
        if (semi) *semi = '\0';

        // Skip blank/whitespace-only lines (after comment stripped)
        if (is_blank_line(line)) continue;

        // Tokenize
        char *opcode_tok = strtok(line, ",");
        char *reg_tok    = strtok(NULL, ",");
        char *value_tok  = strtok(NULL, ",");
        char *extra_tok  = strtok(NULL, ",");

        if (extra_tok) {
            printf("Shader Compilation Error line %d: too many operands: %s", linecount, original);
            free(out.data);
            out.data = NULL;
            out.len = 0;
            break;
        }

        if (!opcode_tok || !reg_tok || !value_tok) {
            printf("Shader Compilation Error line %d: expected 'OPCODE, REG_DEST, VALUE': %s", linecount, original);
            free(out.data);
            out.data = NULL;
            out.len = 0;
            break;
        }

        // Grow buffer
        if (out.len == cap) {
            size_t new_cap = cap ? cap * 2 : 16;
            void *p = realloc(out.data, new_cap * sizeof *out.data);
            if (!p) {
                printf("Shader Compilation Error line %d: out of memory\n", linecount);
                free(out.data);
                out.data = NULL;
                out.len = 0;
                break;
            }
            out.data = p;
            cap = new_cap;
        }

        InstructionText *dst = &out.data[out.len];

        if (!copy_token(dst->opcode, opcode_tok)) {
            printf("Shader Compilation Error line %d: invalid opcode token: %s", linecount, original);
            free(out.data); out.data = NULL; out.len = 0; break;
        }
        if (!copy_token(dst->reg_dest, reg_tok)) {
            printf("Shader Compilation Error line %d: invalid reg token: %s", linecount, original);
            free(out.data); out.data = NULL; out.len = 0; break;
        }
        if (!copy_token(dst->value, value_tok)) {
            printf("Shader Compilation Error line %d: invalid value token: %s", linecount, original);
            free(out.data); out.data = NULL; out.len = 0; break;
        }

        out.len++;
    }

    fclose(fp);
    return out;
}

struct Instruction* compile_shader(InstructionTextList list, int *prog_size) {
    *prog_size = list.len;
    uint8_t reg_dest;
    uint8_t opcode;
    uint16_t value;
    struct Instruction *shader = malloc(*prog_size * sizeof(struct Instruction));

    for (size_t i = 0; i < *prog_size; i++) {
        uint8_t opcode;
        uint8_t reg_dest;
        uint16_t value;

        if(!parse_opcode(list.data[i].opcode, &opcode)) {
            printf("Error: Invalid opcode: '%s'\n", list.data[i].opcode);
            return NULL;
        }
        if (opcode == OP_STORE_PIXEL || opcode == OP_END) {
            shader[i].opcode = opcode;
            shader[i].reg_dest = 0;
            shader[i].value = 0;
        } else {
            if(!parse_reg(list.data[i].reg_dest, &reg_dest)) {
                printf("Error: Invalid register: '%s'\n", list.data[i].reg_dest);
                return NULL;
            }
            if(opcode == OP_MOV_LOW_IMM16 || opcode == OP_MOV_HIGH_IMM16) {
                if(!parse_value(list.data[i].value, &value)) {
                    printf("Error: Invalid value: '%s'\n", list.data[i].value);
                    return NULL;
                }
            } else {
                uint8_t src_reg;
                if(!parse_reg(list.data[i].value, &src_reg)) {
                    printf("Error: Invalid register: '%s'\n", list.data[i].value);
                    return NULL;
                }
                value = (uint16_t)src_reg;
            }
            shader[i].opcode = opcode;
            shader[i].reg_dest = reg_dest;
            shader[i].value = value;
        }

    }

    free(list.data);
    return shader;
}
