# nanocc

<img width="964" height="371" alt="image" src="https://github.com/user-attachments/assets/d9d512e6-b41d-40e3-b140-81b943754c77" />

<!-- <img width="1101" height="288" alt="image" src="https://github.com/user-attachments/assets/40781b3b-ce70-44c4-9f00-296b379433de" /> -->

## Build nanocc

```python
git clone --recurse-submodules https://github.com/VachanVY/nanocc.git

chmod +x buildcc.sh

# build the compiler
./buildcc.sh compiler
## or to rebuild codebase
./buildcc.sh rebuild

# build tools/test_nanocc.cpp for the testing
./buildcc.sh test nanocc_codegen
## or rebuild it
./buildcc.sh rebuild_test nanocc_codegen

# run tests
./nanocc_tests/test_compiler ./build/tools/nanocc_codegen --chapter 10 -v

# clang format
find . -type f \( -name "*.cpp" -o -name "*.cc" -o -name "*.c" -o -name "*.hpp" -o -name "*.hh" -o -name "*.h" \) -exec clang-format -i {} +
```

## nanocc codebase

nanocc codebase tree directory structure (LLVM style)

```
setopt nullglob && tree -a -L 4 -I '.git' --noreport include lib tools --dirsfirst && echo "" && ls -1 *.cpp *.c *.sh *.md CMakeLists.txt LICENSE nanocc 2>/dev/null | sort
```

```
include
└── nanocc
    ├── AST
    │   └── AST.hpp
    ├── Codegen
    │   ├── ASM.hpp
    │   └── IRToPseudoAsmPass.hpp
    ├── IR
    │   ├── BasicBlock.hpp
    │   ├── IRDump.hpp
    │   └── IR.hpp
    ├── Lexer
    │   └── Lexer.hpp
    ├── Parser
    │   └── Parser.hpp
    ├── Sema
    │   └── Sema.hpp
    ├── Target
    │   └── X86
    │       ├── X86TargetEmitter.hpp
    │       └── X86TargetInfo.hpp
    ├── Transforms
    │   ├── ConstantFolding.hpp
    │   ├── CopyPropagation.hpp
    │   ├── DeadStoreElimination.hpp
    │   ├── PassManager.hpp
    │   └── SimplifyCFG.hpp
    └── Utils
        ├── tokens.def
        ├── Tokens.hpp
        └── Utils.hpp
lib
├── Codegen
│   ├── CMakeLists.txt
│   ├── IRToPseudoAsmHelper.hpp
│   └── IRToPseudoAsmPass.cpp
├── IR
│   ├── BasicBlock.cpp
│   ├── CMakeLists.txt
│   ├── IR.cpp
│   ├── IRDump.cpp
│   ├── IRGen.cpp
│   └── IRHelper.hpp
├── Lexer
│   ├── CAPI
│   │   ├── lexer.c
│   │   ├── lexer.h
│   │   ├── lexer_internal.h
│   │   ├── regex.c
│   │   └── regex.h
│   ├── CMakeLists.txt
│   └── Lexer.cpp
├── Parser
│   ├── CMakeLists.txt
│   └── Parser.cpp
├── Sema
│   ├── CMakeLists.txt
│   ├── Sema.cpp
│   ├── SemaDecl.cpp
│   ├── SemaHelper.hpp
│   ├── SemaLabel.cpp
│   └── SemaType.cpp
├── Target
│   ├── X86
│   │   ├── CMakeLists.txt
│   │   ├── X86InstrFixup.cpp
│   │   ├── X86PseudoLowering.cpp
│   │   └── X86TargetEmitter.cpp
│   └── CMakeLists.txt
├── Transforms
│   ├── CMakeLists.txt
│   ├── ConstantFolding.cpp
│   ├── CopyPropagation.cpp
│   ├── DeadStoreElimination.cpp
│   ├── PassManager.cpp
│   └── SimplifyCFG.cpp
├── Utils
│   ├── CMakeLists.txt
│   └── Utils.cpp
└── CMakeLists.txt
tools
├── test
│   ├── README.txt
│   ├── TestCommon.hpp
│   ├── TestIR.cpp
│   ├── TestLexer.cpp
│   ├── TestParser.cpp
│   ├── TestSema.cpp
│   └── TestX86AsmGen.cpp
├── CMakeLists.txt
└── NanoCC.cpp

buildcc.sh
CMakeLists.txt
LICENSE
nanocc
README.md
```

## nanocc progress

```c
// examples/linkage_fact.c
int num_times = 10;
int factorial(int n) {
    int ret;
    if (n <= 1) {
        ret = 1;
    } else {
        ret = n * factorial(n - 1);
    }
    return ret;
}
```

<details>
  <summary>assembly output</summary>
 
```asm
      .globl factorial
    .text
factorial:
    pushq %rbp
    movq %rsp, %rbp
    subq $32, %rsp
    movl %edi, -4(%rbp)
    cmpl $1, -4(%rbp)
    movl $0, -8(%rbp)
    setle -8(%rbp)
    cmpl $0, -8(%rbp)
    je else.0
    movl $1, -12(%rbp)
    jmp end.1
  else.0:
    movl -4(%rbp), %r10d
    movl %r10d, -16(%rbp)
    subl $1, -16(%rbp)
    movl -16(%rbp), %edi
    call factorial
    movl %eax, -20(%rbp)
    movl -4(%rbp), %r10d
    movl %r10d, -24(%rbp)
    movl -24(%rbp), %r11d
    imull -20(%rbp), %r11d
    movl %r11d, -24(%rbp)
    movl -24(%rbp), %r10d
    movl %r10d, -12(%rbp)
  end.1:
    movl -12(%rbp), %eax
    movq %rbp, %rsp
    popq %rbp
    ret

    movl $0, %eax
    movq %rbp, %rsp
    popq %rbp
    ret

    .globl num_times
    .data
    .align 4

num_times:
.long 10

    .section .note.GNU-stack, "",@progbits

````

</details>

```c
// examples/linkage_utils.c
int putchar(int c);

int putint(int x) {
    if (x < 0) {
        putchar(45);  // ASCII '-'
        x = -x;
    }

    if (x >= 10)
        putint(x / 10);

    putchar(48 + (x % 10));  // ASCII '0' = 48
}

int hello_world(void) {
    // "Hello, World!" using only integers
    putchar(72);    putchar(101);    putchar(108);
    putchar(108);    putchar(111);    putchar(44);
    putchar(32);    putchar(87);    putchar(111);
    putchar(114);    putchar(108);    putchar(100);
    putchar(33);    putchar(10);
}

int init_sys(void) {
    static int initialized = 0;

    if (initialized == 1)
        return 0;

    hello_world();

    putchar(73);   // 'I'
    putchar(78);   // 'N'
    putchar(73);   // 'I'
    putchar(84);   // 'T'
    putchar(10);   // '\n'

    initialized = 1;
    return 1;
}
````

<details>
  <summary>assembly output</summary>
 
```asm
    .globl putint
    .text
putint:
    pushq %rbp
    movq %rsp, %rbp
    subq $48, %rsp
    movl %edi, -4(%rbp)
    cmpl $0, -4(%rbp)
    movl $0, -8(%rbp)
    setl -8(%rbp)
    cmpl $0, -8(%rbp)
    je end.0
    movl $45, %edi
    call putchar@PLT
    movl %eax, -12(%rbp)
    movl -4(%rbp), %r10d
    movl %r10d, -16(%rbp)
    negl -16(%rbp)
    movl -16(%rbp), %r10d
    movl %r10d, -4(%rbp)
  end.0:
    cmpl $10, -4(%rbp)
    movl $0, -20(%rbp)
    setge -20(%rbp)
    cmpl $0, -20(%rbp)
    je end.1
    movl -4(%rbp), %eax
    cdq
    movl $10, %r10d
    idivl %r10d
    movl %eax, -24(%rbp)
    movl -24(%rbp), %edi
    call putint
    movl %eax, -28(%rbp)
  end.1:
    movl -4(%rbp), %eax
    cdq
    movl $10, %r10d
    idivl %r10d
    movl %edx, -32(%rbp)
    movl $48, -36(%rbp)
    movl -32(%rbp), %r10d
    addl %r10d, -36(%rbp)
    movl -36(%rbp), %edi
    call putchar@PLT
    movl %eax, -40(%rbp)
    movl $0, %eax
    movq %rbp, %rsp
    popq %rbp
    ret

    .globl hello_world
    .text

hello_world:
pushq %rbp
movq %rsp, %rbp
subq $64, %rsp
movl $72, %edi
call putchar@PLT
movl %eax, -4(%rbp)
movl $101, %edi
call putchar@PLT
movl %eax, -8(%rbp)
movl $108, %edi
call putchar@PLT
movl %eax, -12(%rbp)
movl $108, %edi
call putchar@PLT
movl %eax, -16(%rbp)
movl $111, %edi
call putchar@PLT
movl %eax, -20(%rbp)
movl $44, %edi
call putchar@PLT
movl %eax, -24(%rbp)
movl $32, %edi
call putchar@PLT
movl %eax, -28(%rbp)
movl $87, %edi
call putchar@PLT
movl %eax, -32(%rbp)
movl $111, %edi
call putchar@PLT
movl %eax, -36(%rbp)
movl $114, %edi
call putchar@PLT
movl %eax, -40(%rbp)
movl $108, %edi
call putchar@PLT
movl %eax, -44(%rbp)
movl $100, %edi
call putchar@PLT
movl %eax, -48(%rbp)
movl $33, %edi
call putchar@PLT
movl %eax, -52(%rbp)
movl $10, %edi
call putchar@PLT
movl %eax, -56(%rbp)
movl $0, %eax
movq %rbp, %rsp
popq %rbp
ret

    .globl init_sys
    .text

init_sys:
pushq %rbp
movq %rsp, %rbp
subq $32, %rsp
cmpl $1, initialized.2(%rip)
movl $0, -4(%rbp)
sete -4(%rbp)
cmpl $0, -4(%rbp)
je end.2
movl $0, %eax
movq %rbp, %rsp
popq %rbp
ret

end.2:
call hello_world
movl %eax, -8(%rbp)
movl $73, %edi
call putchar@PLT
movl %eax, -12(%rbp)
movl $78, %edi
call putchar@PLT
movl %eax, -16(%rbp)
movl $73, %edi
call putchar@PLT
movl %eax, -20(%rbp)
movl $84, %edi
call putchar@PLT
movl %eax, -24(%rbp)
movl $10, %edi
call putchar@PLT
movl %eax, -28(%rbp)
movl $1, initialized.2(%rip)
movl $1, %eax
movq %rbp, %rsp
popq %rbp
ret

    movl $0, %eax
    movq %rbp, %rsp
    popq %rbp
    ret

    .bss
    .align 4

initialized.2:
.zero 4

    .section .note.GNU-stack, "",@progbits

````

</details>

```c
// defined in linkage_utils.c
int init_sys(void);

int putchar(int c);
int putint(int x);

// defined in linkage_fact.c
extern int num_times;
int factorial(int n);

int main(void){
    init_sys();
    init_sys(); // WON'T PRINT "INIT" AGAIN
    init_sys(); // WON'T PRINT "INIT" AGAIN

    for (int i = 1; i <= num_times; i = i + 1) {
        int fact = factorial(i);
        putint(i); /*space*/ putchar(32);
        putint(fact); /*newline*/ putchar(10);
    }
    return 0;
}
````

<details>
  <summary>assembly output</summary>
 
```asm
    .globl main
    .text
main:
    pushq %rbp
    movq %rsp, %rbp
    subq $48, %rsp
    call init_sys@PLT
    movl %eax, -4(%rbp)
    call init_sys@PLT
    movl %eax, -8(%rbp)
    call init_sys@PLT
    movl %eax, -12(%rbp)
    movl $1, -16(%rbp)
  start_for.0:
    movl num_times(%rip), %r10d
    cmpl %r10d, -16(%rbp)
    movl $0, -20(%rbp)
    setle -20(%rbp)
    cmpl $0, -20(%rbp)
    je break_for.0
    movl -16(%rbp), %edi
    call factorial@PLT
    movl %eax, -24(%rbp)
    movl -24(%rbp), %r10d
    movl %r10d, -28(%rbp)
    movl -16(%rbp), %edi
    call putint@PLT
    movl %eax, -32(%rbp)
    movl $32, %edi
    call putchar@PLT
    movl %eax, -36(%rbp)
    movl -28(%rbp), %edi
    call putint@PLT
    movl %eax, -40(%rbp)
    movl $10, %edi
    call putchar@PLT
    movl %eax, -44(%rbp)
  continue_for.0:
    movl -16(%rbp), %r10d
    movl %r10d, -48(%rbp)
    addl $1, -48(%rbp)
    movl -48(%rbp), %r10d
    movl %r10d, -16(%rbp)
    jmp start_for.0
  break_for.0:
    movl $0, %eax
    movq %rbp, %rsp
    popq %rbp
    ret

    movl $0, %eax
    movq %rbp, %rsp
    popq %rbp
    ret


    .section .note.GNU-stack, "",@progbits

````

</details>


```bash
chmod +x nanocc
# our compiler compiles to assembly: asm.s
# gcc assembler asm.s => executable
./nanocc examples/linkage_fact.c examples/linkage_main.c examples/linkage_utils.c -o main
./main
````

Output:

```
Hello, World!
INIT
1 1
2 2
3 6
4 24
5 120
6 720
7 5040
8 40320
9 362880
10 3628800
```

<details>
  <summary>Notes/Trash</summary>
 
```c
int foo(int a, int b, int c, int d, int e, int f, int g, int h) {
    return a + h;
}

int caller(int arg) {
return arg + foo(1, 2, 3, 4, 5, 6, 7, 8);
}

````

```asm
    .globl foo
foo:
    pushq %rbp
    movq %rsp, %rbp
    subq $48, %rsp
    movl %edi, -4(%rbp) # 4-bit registers, so gap is 4 between successive elements in stack
    movl %esi, -8(%rbp)
    movl %edx, -12(%rbp)
    movl %ecx, -16(%rbp)
    movl %r8d, -20(%rbp)
    movl %r9d, -24(%rbp)
    movl 16(%rbp), %r10d # pushq uses 8 bytes # stack args
    movl %r10d, -28(%rbp)
    movl 24(%rbp), %r10d # 24 - 16 = 8 # stack args
    movl %r10d, -32(%rbp)
    movl -4(%rbp), %r10d
    movl %r10d, -36(%rbp)
    movl -32(%rbp), %r10d
    addl %r10d, -36(%rbp)
    movl -36(%rbp), %eax
    movq %rbp, %rsp
    popq %rbp
    ret

    movl $0, %eax
    movq %rbp, %rsp
    popq %rbp
    ret

    .globl caller
caller:
    pushq %rbp
    movq %rsp, %rbp
    subq $16, %rsp
    movl %edi, -4(%rbp)
    movl $1, %edi
    movl $2, %esi
    movl $3, %edx
    movl $4, %ecx
    movl $5, %r8d
    movl $6, %r9d
    pushq $8       # pushq takes 64-bit values
    pushq $7       # that's why "gap" between `7` and `8` will 8 bytes in memory
    call foo
    addq $16, %rsp
    movl %eax, -8(%rbp)
    movl -4(%rbp), %r10d
    movl %r10d, -12(%rbp)
    movl -8(%rbp), %r10d
    addl %r10d, -12(%rbp)
    movl -12(%rbp), %eax
    movq %rbp, %rsp
    popq %rbp
    ret

    movl $0, %eax
    movq %rbp, %rsp
    popq %rbp
    ret

    .section .note.GNU-stack, "",@progbits
````

</details>
