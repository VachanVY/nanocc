#ifndef NANOCC_TARGET_X86_X86TARGETINFO_HPP
#define NANOCC_TARGET_X86_X86TARGETINFO_HPP

/*
use 64 bit registers in push instructions.
use 8 bit registers in conditional set instructions.
use 32 bit registers in all other instructions.
*/

#include <string>

//    64   32   8 bit registers
#define REG_FAMILIES                                                                               \
    X(rax, eax, al)                                                                                \
    X(rdx, edx, dl)                                                                                \
    X(rcx, ecx, cl)                                                                                \
    X(rdi, edi, dil)                                                                               \
    X(rsi, esi, sil)                                                                               \
    X(r8, r8d, r8b)                                                                                \
    X(r9, r9d, r9b)                                                                                \
    X(r10, r10d, r10b)                                                                             \
    X(r11, r11d, r11b)

enum class Reg {
#define X(r64, r32, r8) r64, r32, r8,
    REG_FAMILIES
#undef X
    rsp,
    rbp
};

enum class RegClass {
    RC_64,
    RC_32,
    RC_8,
};

static const std::string regToString[] = {
#define X(r64, r32, r8) "%" #r64, "%" #r32, "%" #r8,
    REG_FAMILIES
#undef X
    "%rsp",
    "%rbp"
};

inline std::string getRegString(Reg reg) {
    // register in string form
    return regToString[static_cast<int>(reg)];
}

#endif // NANOCC_TARGET_X86_X86TARGETINFO_HPP
