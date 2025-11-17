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
