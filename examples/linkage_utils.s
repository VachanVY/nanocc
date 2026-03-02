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
