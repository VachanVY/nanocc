    .globl main
main:
    pushq %rbp
    movq %rsp, %rbp
    subq $24, %rsp
    movl $1, -4(%rbp)
    movl -4(%rbp), %r10d
    movl %r10d, -8(%rbp)
    movl -8(%rbp), %r11d
    imull $2, %r11d
    movl %r11d, -8(%rbp)
    movl -8(%rbp), %r10d
    movl %r10d, -4(%rbp)
    movl -4(%rbp), %r10d
    movl %r10d, -4(%rbp)
    movl -4(%rbp), %r10d
    movl %r10d, -12(%rbp)
    movl $0, -16(%rbp)
    movl -4(%rbp), %r10d
    movl %r10d, -20(%rbp)
    addl $3, -20(%rbp)
    cmpl $0, -4(%rbp)
    movl $0, -24(%rbp)
    sete -24(%rbp)
    movl -4(%rbp), %eax
    movq %rbp, %rsp
    popq %rbp
    ret
    movl $0, %eax
    movq %rbp, %rsp
    popq %rbp
    ret
    .section .note.GNU-stack, "",@progbits
