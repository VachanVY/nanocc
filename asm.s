    .globl main
main:
    pushq %rbp
    movq %rsp, %rbp
    subq $16, %rsp
    movl $2, -4(%rbp)
    subq $8, %rsp
    movl -4(%rbp), %edi
    call fib
    addq $8, %rsp
    movl %eax, -8(%rbp)
    movl -8(%rbp), %eax
    movq %rbp, %rsp
    popq %rbp
    ret

    movl $0, %eax
    movq %rbp, %rsp
    popq %rbp
    ret

    .globl fib
fib:
    pushq %rbp
    movq %rsp, %rbp
    subq $48, %rsp
    movl %edi, -4(%rbp)
    cmpl $0, -4(%rbp)
    movl $0, -8(%rbp)
    sete -8(%rbp)
    cmpl $0, -8(%rbp)
    jne short.1
    cmpl $1, -4(%rbp)
    movl $0, -12(%rbp)
    sete -12(%rbp)
    cmpl $0, -12(%rbp)
    jne short.1
    movl $0, -16(%rbp)
    jmp end.2
  short.1:
    movl $1, -16(%rbp)
  end.2:
    cmpl $0, -16(%rbp)
    je else.0
    movl -4(%rbp), %eax
    movq %rbp, %rsp
    popq %rbp
    ret

    jmp end.3
  else.0:
    movl -4(%rbp), %r10d
    movl %r10d, -20(%rbp)
    subl $1, -20(%rbp)
    subq $8, %rsp
    movl -20(%rbp), %edi
    call fib
    addq $8, %rsp
    movl %eax, -24(%rbp)
    movl -4(%rbp), %r10d
    movl %r10d, -28(%rbp)
    subl $2, -28(%rbp)
    subq $8, %rsp
    movl -28(%rbp), %edi
    call fib
    addq $8, %rsp
    movl %eax, -32(%rbp)
    movl -24(%rbp), %r10d
    movl %r10d, -36(%rbp)
    movl -32(%rbp), %r10d
    addl %r10d, -36(%rbp)
    movl -36(%rbp), %eax
    movq %rbp, %rsp
    popq %rbp
    ret

  end.3:
    movl $0, %eax
    movq %rbp, %rsp
    popq %rbp
    ret

    .section .note.GNU-stack, "",@progbits
