    .globl main
main:
    pushq %rbp
    movq %rsp, %rbp
    subq $16, %rsp
    movl $42, -4(%rbp)
    negl -4(%rbp)
    movl -4(%rbp), %r10d
    movl %r10d, -8(%rbp)
    notl -8(%rbp)
    movl -8(%rbp), %r10d
    movl %r10d, -12(%rbp)
    negl -12(%rbp)
    movl -12(%rbp), %r10d
    movl %r10d, -16(%rbp)
    notl -16(%rbp)
    movl -16(%rbp), %eax
    movq %rbp, %rsp
    popq %rbp
    ret
    .section .note.GNU-stack, "",@progbits
