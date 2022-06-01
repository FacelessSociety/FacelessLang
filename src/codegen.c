#include <codegen.h>
#include <panic.h>
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>


// 2022 Ian Moffett


#define MAX_REGS 4
typedef uint8_t REG_COUNTER;


// Out file.
static FILE* out = NULL;

// Set if taken, clear if free.
static uint8_t reg_bmp = 0x0;

// Registers.
static char* regs[MAX_REGS]= {"r8", "r9", "r10", "r11"};

static inline void cg_freeall_regs(void) {
    reg_bmp = 0x0;
}


static int8_t alloc_reg(void) {
    for (REG_COUNTER reg = 0; reg < MAX_REGS; ++reg) {
        if (!(reg_bmp & (1 << reg))) {
            return reg;
        }
    }

    return -1;
}


static void cg_free_reg(REG_COUNTER reg_id) {
    if (reg_id >= MAX_REGS) return;
    reg_bmp &= ~(1 << reg_id);
}


// Add two registers together and return register
// id with result.
static REG_COUNTER cg_add(REG_COUNTER r1, REG_COUNTER r2) {
    fprintf(out, "\tadd\t%s, %s\n", regs[r1], regs[r2]);
    cg_free_reg(r2);
    return r1;
}


static REG_COUNTER cg_mul(REG_COUNTER r1, REG_COUNTER r2) {
    fprintf(out, "\timul\t%s, %s\n", regs[r1], regs[r2]);
    cg_free_reg(r2);
    return r1;
}

static REG_COUNTER cg_sub(REG_COUNTER r1, REG_COUNTER r2) {
    fprintf(out, "\tsub\t%s, %s\n", regs[r1], regs[r2]);
    cg_free_reg(r2);
    return r1;
}


static REG_COUNTER cg_div(REG_COUNTER r1, REG_COUNTER r2) {
    fprintf(out, "\tidiv\t%s, %s\n", regs[r1], regs[r2]);
    cg_free_reg(r2);
    return r1;
}

static REG_COUNTER cg_load(uint64_t value) {
    REG_COUNTER reg = alloc_reg();
    fprintf(out, "\tmov\t%s, %ld\n", regs[reg], value);
    return reg;
}


static void cg_prologue(void) {
  fputs(
        "default rel\n"
        "global main\n"
        "extern printf\n"
        "section .text\n"
        "print_int:\n"
        "\t.fmtint: db \"%d\", 0xA, 0x0\n"
        "\tpush rbp\n"
        "\tmov rbp, rsp\n"
        "\tsub rsp, 16\n"
        "\tmov esi, ebx\n"                  // EBX is used to hold int we want to print.
        "\tlea rdi, [rel .fmtint]\n"
        "\txor eax, eax\n"
        "\tcall printf\n"
        "\tleave\n"
        "\tretq\n"
        "\nmain:\n",
    out);
}

static void cg_epilogue(void) {
  fputs(
    "\tmov eax, 1\n"
	"\tmov ebx, 0\n"
    "\tint 0x80",
    out);
}


int32_t interpret_ast(struct ASTNode* node) {
    int64_t leftreg, rightreg;

    // Get left and right sub-tree vaues.
    if (node->left)
        leftreg = interpret_ast(node->left);

    if (node->right)
        rightreg = interpret_ast(node->right);

    switch (node->op) {
        case A_ADD:
            return cg_add(leftreg, rightreg);
        case A_SUB:
            return cg_sub(leftreg, rightreg);
        case A_MUL:
            return cg_mul(leftreg, rightreg);
        case A_DIV:
            return cg_div(leftreg, rightreg);
        case A_INTLIT:
            return (cg_load(node->val_int));
        default:
            printf("Unknown AST operator caught in %s().\n", __func__);
            panic();
    }

    return -1;
}


void codegen_end(void) {
    extern uint8_t asm_only;

    if (out == NULL) return;
    cg_epilogue();
    fclose(out);

    if (!(asm_only)) {
        system("nasm -felf64 /tmp/fcomp_out.s -o ./out.o");
        system("gcc -fno-pie -m64 -no-pie out.o");

        // Remove junk.
        remove("/tmp/fcomp_out.s");
        remove("out.o");
    } else {
        system("mv /tmp/fcomp_out.s ./");
    }
}

void codegen_init(void) {
    out = fopen("/tmp/fcomp_out.s", "w");
    cg_prologue();
}
