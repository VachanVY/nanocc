# nanocc

<img width="964" height="371" alt="image" src="https://github.com/user-attachments/assets/d9d512e6-b41d-40e3-b140-81b943754c77" />

<!-- <img width="1101" height="288" alt="image" src="https://github.com/user-attachments/assets/40781b3b-ce70-44c4-9f00-296b379433de" /> -->

## nanocc progress
```c
int putchar(int c);

 // return type, only int is supported as of now
int putint(int x) {
    if (x < 0) {
        putchar(45);  // ASCII '-'
        x = -x;
    }

    if (x >= 10)
        putint(x / 10);

    putchar(48 + (x % 10));  // ASCII '0' = 48
    return 0;
}

int factorial(int n) {
    int ret;
    if (n <= 1) {
        ret = 1;
    } else {
        ret = n * factorial(n - 1);
    }
    return ret;
}

int main(void) {
    // "Hello, World!" using only integers
    putchar(72);    putchar(101);    putchar(108);    
    putchar(108);    putchar(111);    putchar(44);
    putchar(32);    putchar(87);    putchar(111);    
    putchar(114);    putchar(108);    putchar(100);
    putchar(33);    putchar(10);

    for (int i = 1; i <= 5; i = i + 1) {
        int fact = factorial(i);
        putint(i); /*space*/ putchar(32); 
        putint(fact); /*newline*/ putchar(10);
    }
    return 0;
}
```

<details>
  <summary>Abstract Syntax Tree</summary>

```
Program(
  Function(
    name='putchar'
    Parameters(
      name='c.0'
    )
  )
  Function(
    name='putint'
    Parameters(
      name='x.1'
    )
    body=(
      Block(
        IfElse(
          Binary(<,
            Var(name='x.1')
            Constant(0)
          )
          Block(
            Expression(
              FunctionCall(
                name='putchar'
                args(
                  Constant(45)
                )
              )
            )
            Expression(
              Assignment(
                Var(name='x.1')
                Unary(-
                  Var(name='x.1')
                )
              )
            )
          )
        )
        IfElse(
          Binary(>=,
            Var(name='x.1')
            Constant(10)
          )
          Expression(
            FunctionCall(
              name='putint'
              args(
                Binary(/,
                  Var(name='x.1')
                  Constant(10)
                )
              )
            )
          )
        )
        Expression(
          FunctionCall(
            name='putchar'
            args(
              Binary(+,
                Constant(48)
                Binary(%,
                  Var(name='x.1')
                  Constant(10)
                )
              )
            )
          )
        )
        Return(
          Constant(0)
        )
      )
    )
  )
  Function(
    name='factorial'
    Parameters(
      name='n.2'
    )
    body=(
      Block(
        Declaration(
          name='ret.3')
        IfElse(
          Binary(<=,
            Var(name='n.2')
            Constant(1)
          )
          Block(
            Expression(
              Assignment(
                Var(name='ret.3')
                Constant(1)
              )
            )
          )
          Block(
            Expression(
              Assignment(
                Var(name='ret.3')
                Binary(*,
                  Var(name='n.2')
                  FunctionCall(
                    name='factorial'
                    args(
                      Binary(-,
                        Var(name='n.2')
                        Constant(1)
                      )
                    )
                  )
                )
              )
            )
          )
        )
        Return(
          Var(name='ret.3')
        )
      )
    )
  )
  Function(
    name='main'
    Parameters()
    body=(
      Block(
        Expression(
          FunctionCall(
            name='putchar'
            args(
              Constant(72)
            )
          )
        )
        Expression(
          FunctionCall(
            name='putchar'
            args(
              Constant(101)
            )
          )
        )
        Expression(
          FunctionCall(
            name='putchar'
            args(
              Constant(108)
            )
          )
        )
        Expression(
          FunctionCall(
            name='putchar'
            args(
              Constant(108)
            )
          )
        )
        Expression(
          FunctionCall(
            name='putchar'
            args(
              Constant(111)
            )
          )
        )
        Expression(
          FunctionCall(
            name='putchar'
            args(
              Constant(44)
            )
          )
        )
        Expression(
          FunctionCall(
            name='putchar'
            args(
              Constant(32)
            )
          )
        )
        Expression(
          FunctionCall(
            name='putchar'
            args(
              Constant(87)
            )
          )
        )
        Expression(
          FunctionCall(
            name='putchar'
            args(
              Constant(111)
            )
          )
        )
        Expression(
          FunctionCall(
            name='putchar'
            args(
              Constant(114)
            )
          )
        )
        Expression(
          FunctionCall(
            name='putchar'
            args(
              Constant(108)
            )
          )
        )
        Expression(
          FunctionCall(
            name='putchar'
            args(
              Constant(100)
            )
          )
        )
        Expression(
          FunctionCall(
            name='putchar'
            args(
              Constant(33)
            )
          )
        )
        Expression(
          FunctionCall(
            name='putchar'
            args(
              Constant(10)
            )
          )
        )
        For(name='for.0'
          Init:
            Declaration(
              name='i.4'
              Constant(1)
            )
          Condition:
            Binary(<=,
              Var(name='i.4')
              Constant(5)
            )
          Post:
            Assignment(
              Var(name='i.4')
              Binary(+,
                Var(name='i.4')
                Constant(1)
              )
            )
          ForLoopBody:
            Block(
              Declaration(
                name='fact.5'
                FunctionCall(
                  name='factorial'
                  args(
                    Var(name='i.4')
                  )
                )
              )
              Expression(
                FunctionCall(
                  name='putint'
                  args(
                    Var(name='i.4')
                  )
                )
              )
              Expression(
                FunctionCall(
                  name='putchar'
                  args(
                    Constant(32)
                  )
                )
              )
              Expression(
                FunctionCall(
                  name='putint'
                  args(
                    Var(name='fact.5')
                  )
                )
              )
              Expression(
                FunctionCall(
                  name='putchar'
                  args(
                    Constant(10)
                  )
                )
              )
            )
        )
        Return(
          Constant(0)
        )
      )
    )
  )
)
```
</details>

<details>
<summary>Generated Intermeddiate Representation and Assembly</summary>

 ```asm
# ----------- IR Generation -----------
# Function[
#  name='putint'
#  parameters=[
#    'x.1'
#  ]
#   instructions=[
#     tmp.6 = x.1 < 0
#     jump_if_false tmp.6, end.1
#     tmp.7 = putchar(45)
#     tmp.8 = - x.1
#     x.1 = tmp.8
#   end.1:
#     tmp.9 = x.1 >= 10
#     jump_if_false tmp.9, end.2
#     tmp.10 = x.1 / 10
#     tmp.11 = putint(tmp.10)
#   end.2:
#     tmp.12 = x.1 % 10
#     tmp.13 = 48 + tmp.12
#     tmp.14 = putchar(tmp.13)
#     return 0
#     return 0
#   ]
# ]
# Function[
#   name='factorial'
#   parameters=[
#     'n.2'
#   ]
#   instructions=[
#     tmp.15 = n.2 <= 1
#     jump_if_false tmp.15, else.3
#     ret.3 = 1
#     jump end.4
#   else.3:
#     tmp.16 = n.2 - 1
#     tmp.17 = factorial(tmp.16)
#     tmp.18 = n.2 * tmp.17
#     ret.3 = tmp.18
#   end.4:
#     return ret.3
#     return 0
#   ]
# ]
# Function[
#   name='main'
#   parameters=[]
#   instructions=[
#     tmp.19 = putchar(72)
#     tmp.20 = putchar(101)
#     tmp.21 = putchar(108)
#     tmp.22 = putchar(108)
#     tmp.23 = putchar(111)
#     tmp.24 = putchar(44)
#     tmp.25 = putchar(32)
#     tmp.26 = putchar(87)
#     tmp.27 = putchar(111)
#     tmp.28 = putchar(114)
#     tmp.29 = putchar(108)
#     tmp.30 = putchar(100)
#     tmp.31 = putchar(33)
#     tmp.32 = putchar(10)
#     i.4 = 1
#   start_for.0:
#     tmp.33 = i.4 <= 5
#     jump_if_false tmp.33, break_for.0
#     tmp.34 = factorial(i.4)
#     fact.5 = tmp.34
#     tmp.35 = putint(i.4)
#     tmp.36 = putchar(32)
#     tmp.37 = putint(fact.5)
#     tmp.38 = putchar(10)
#   continue_for.0:
#     tmp.39 = i.4 + 1
#     i.4 = tmp.39
#     jump start_for.0
#   break_for.0:
#     return 0
#     return 0
#   ]
# ]
# -------------------------------------


     .globl putint
putint:
    pushq %rbp
    movq %rsp, %rbp
    subq $48, %rsp
    movl %edi, -4(%rbp)
    cmpl $0, -4(%rbp)
    movl $0, -8(%rbp)
    setl -8(%rbp)
    cmpl $0, -8(%rbp)
    je end.1
    movl $45, %edi
    call putchar@PLT
    movl %eax, -12(%rbp)
    movl -4(%rbp), %r10d
    movl %r10d, -16(%rbp)
    negl -16(%rbp)
    movl -16(%rbp), %r10d
    movl %r10d, -4(%rbp)
  end.1:
    cmpl $10, -4(%rbp)
    movl $0, -20(%rbp)
    setge -20(%rbp)
    cmpl $0, -20(%rbp)
    je end.2
    movl -4(%rbp), %eax
    cdq
    movl $10, %r10d
    idivl %r10d
    movl %eax, -24(%rbp)
    movl -24(%rbp), %edi
    call putint
    movl %eax, -28(%rbp)
  end.2:
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

    movl $0, %eax
    movq %rbp, %rsp
    popq %rbp
    ret

    .globl factorial
factorial:
    pushq %rbp
    movq %rsp, %rbp
    subq $32, %rsp
    movl %edi, -4(%rbp)
    cmpl $1, -4(%rbp)
    movl $0, -8(%rbp)
    setle -8(%rbp)
    cmpl $0, -8(%rbp)
    je else.3
    movl $1, -12(%rbp)
    jmp end.4
  else.3:
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
  end.4:
    movl -12(%rbp), %eax
    movq %rbp, %rsp
    popq %rbp
    ret

    movl $0, %eax
    movq %rbp, %rsp
    popq %rbp
    ret

    .globl main
main:
    pushq %rbp
    movq %rsp, %rbp
    subq $96, %rsp
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
    movl $1, -60(%rbp)
  start_for.0:
    cmpl $5, -60(%rbp)
    movl $0, -64(%rbp)
    setle -64(%rbp)
    cmpl $0, -64(%rbp)
    je break_for.0
    movl -60(%rbp), %edi
    call factorial
    movl %eax, -68(%rbp)
    movl -68(%rbp), %r10d
    movl %r10d, -72(%rbp)
    movl -60(%rbp), %edi
    call putint
    movl %eax, -76(%rbp)
    movl $32, %edi
    call putchar@PLT
    movl %eax, -80(%rbp)
    movl -72(%rbp), %edi
    call putint
    movl %eax, -84(%rbp)
    movl $10, %edi
    call putchar@PLT
    movl %eax, -88(%rbp)
  continue_for.0:
    movl -60(%rbp), %r10d
    movl %r10d, -92(%rbp)
    addl $1, -92(%rbp)
    movl -92(%rbp), %r10d
    movl %r10d, -60(%rbp)
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
```

</details>


```bash
g++ -std=c++23 -Iinclude lexer.cpp parser.cpp checker.cpp irgen.cpp asmgen.cpp codegen.cpp test.cpp -o nanocc.out
./nanocc.out asm.c # compiles to assembly: asm.s
gcc asm.s; ./a.out # gcc assembler .s => executable
```

Output:
```
Hello, World!
1 1
2 2
3 6
4 24
5 120
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
```

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
```
</details>
