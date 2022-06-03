#include <codegen.h>
#include <panic.h>
#include <symbol.h>
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>


// 2022 Ian Moffett


#define MAX_REGS 4


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
            reg_bmp |= (1 << reg);
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
    fprintf(out, "\tadd %s, %s\n", regs[r1], regs[r2]);
    cg_free_reg(r2);
    return r1;
}


static REG_COUNTER cg_mul(REG_COUNTER r1, REG_COUNTER r2) {
    fprintf(out, "\timul %s, %s\n", regs[r1], regs[r2]);
    cg_free_reg(r2);
    return r1;
}

static REG_COUNTER cg_sub(REG_COUNTER r1, REG_COUNTER r2) {
    fprintf(out, "\tsub %s, %s\n", regs[r1], regs[r2]);
    cg_free_reg(r2);
    return r1;
}


// Divide first register by second 
// and return register that contains result.
static REG_COUNTER cg_div(REG_COUNTER r1, REG_COUNTER r2) {
    fprintf(out, "\tmov rax, %s\n", regs[r1]);
    fprintf(out, "\tcqo\n");
    fprintf(out, "\tidiv %s\n", regs[r2]);
    fprintf(out, "\tmov %s, rax\n", regs[r1]);
    cg_free_reg(r2);
    return r1;
}

static REG_COUNTER cg_load(uint64_t value) {
    REG_COUNTER reg = alloc_reg();
    fprintf(out, "\tmov %s, %ld\n", regs[reg], value);
    return reg;
}


static void cg_prologue(void) {
  fputs(
        "default rel\n"
        "global main\n"
        "extern printf\n\n"
        "section .text\n"
        "print_int:\n"
        "\t.fmtint: db \"%d\", 0xA, 0x0\n"
        "\tpush rbp\n"
        "\tmov rbp, rsp\n"
        "\tsub rsp, 16\n"
        "\tmov rsi, rbx\n"                  // RBX is used to hold int we want to print.
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

void codegen_printint(REG_COUNTER reg) {
    fprintf(out, "\tmov rbx, %s\n", regs[reg]);
    fprintf(out, "\tcall print_int\n");
    cg_freeall_regs();
}


// Store a registers value into a var.
static REG_COUNTER cg_stor_glob(REG_COUNTER r, char* sym) {
    fprintf(out, "\tmov [%s], %s\n", sym, regs[r]);
    return r;
}


// Load a variables value into a register.
static REG_COUNTER cg_load_glob(const char* sym) {
    // Allocate a register for storing variable value.
    REG_COUNTER reg_alloc = alloc_reg();

    // Generate some assembly.
    fprintf(out, "\tmov %s, [%s]\n", regs[reg_alloc], sym);

    // Return allocated register.
    return reg_alloc;
}


static REG_COUNTER cgequal(REG_COUNTER r1, REG_COUNTER r2) {
    fprintf(out, "\tcmp %s, %s\n", regs[r1], regs[r2]);
    fprintf(out, "\tsete al\n");
    fprintf(out, "\tmovzx %s, al\n", regs[r1]);
    fprintf(out, "\tand %s, 0xFF\n", regs[r1]);
    cg_free_reg(r2);
    return r1;
}


static REG_COUNTER cgnotequal(REG_COUNTER r1, REG_COUNTER r2) {
    fprintf(out, "\tcmp %s, %s\n", regs[r1], regs[r2]);
    fprintf(out, "\tsetne al\n");
    fprintf(out, "\tmovzx %s, al\n", regs[r1]);
    fprintf(out, "\tand %s, 0xFF\n", regs[r1]);
    cg_free_reg(r2);
    return r1;
}


static REG_COUNTER cglessthan(REG_COUNTER r1, REG_COUNTER r2) {
    fprintf(out, "\tcmp %s, %s\n", regs[r1], regs[r2]);
    fprintf(out, "\tsetl al\n");
    fprintf(out, "\tmovzx %s, al\n", regs[r1]);
    fprintf(out, "\tand %s, 0xFF\n", regs[r1]);
    cg_free_reg(r2);
    return r1;
}

static REG_COUNTER cggreaterthan(REG_COUNTER r1, REG_COUNTER r2) {
    fprintf(out, "\tcmp %s, %s\n", regs[r1], regs[r2]);
    fprintf(out, "\tsetg al\n");
    fprintf(out, "\tmovzx %s, al\n", regs[r1]);
    fprintf(out, "\tand %s, 0xFF\n", regs[r1]);
    cg_free_reg(r2);
    return r1;
}


static REG_COUNTER cglessequal(REG_COUNTER r1, REG_COUNTER r2) {
    fprintf(out, "\tcmp %s, %s\n", regs[r1], regs[r2]);
    fprintf(out, "\tsetle al\n");
    fprintf(out, "\tmovzx %s, al\n", regs[r1]);
    fprintf(out, "\tand %s, 0xFF\n", regs[r1]);
    cg_free_reg(r2);
    return r1;
}


static REG_COUNTER cggreaterqual(REG_COUNTER r1, REG_COUNTER r2) {
    fprintf(out, "\tcmp %s, %s\n", regs[r1], regs[r2]);
    fprintf(out, "\tsetge al\n");
    fprintf(out, "\tmovzx %s, al\n", regs[r1]);
    fprintf(out, "\tand %s, 0xFF\n", regs[r1]);
    cg_free_reg(r2);
    return r1;
}




int32_t interpret_ast(struct ASTNode* node, REG_COUNTER reg) {
    int64_t leftreg, rightreg;

    // Get left and right sub-tree vaues.
    if (node->left)
        leftreg = interpret_ast(node->left, -1);

    if (node->right)
        rightreg = interpret_ast(node->right, leftreg);

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
        case A_LVIDENT:
            return cg_stor_glob(reg, gsym[node->id].name);
        case A_IDENT:
            return cg_load_glob(gsym[node->id].name);
        case A_ASSIGN:
            return rightreg;
        case A_EQ:
            return cgequal(leftreg, rightreg);
        case A_NE:
            return cgnotequal(leftreg, rightreg);
        case A_LT:
            return cglessthan(leftreg, rightreg);
        case A_GT:
            return cggreaterthan(leftreg, rightreg);
        case A_LE:
            return cglessequal(leftreg, rightreg);
        case A_GE:
            return cggreaterqual(leftreg, rightreg);
        default:
            printf("Unknown AST operator caught in %s().\n", __func__);
            panic();
    }

    return -1;
}


// Generate global symbol.
// TODO: Update this as new types are a thing.
void codegen_genglobsym(const char* sym, INTEGER_TYPE type) {
    fprintf(out, "\t%s_start: jmp %s_end\n", sym, sym);
    fprintf(out, "\tsection .data\n");
    switch (type) {
        case U8:
            fprintf(out, "\t%s: db 0\n", sym);
            break;
    }

    fprintf(out, "\tsection .text\n");
    fprintf(out, "\t%s_end:\n", sym);
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
