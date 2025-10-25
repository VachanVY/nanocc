    .globl main
main:
    pushq %rbp
    movq %rsp, %rbp
    subq $16, %rsp
    movl $2, -4(%rbp)
    movl -4(%rbp), %r11d
    imull $3, %r11d
    movl %r11d, -4(%rbp)
    movl -4(%rbp), %eax
    cdq
    movl $4, %r10d
    idivl %r10d
    movl %eax, -8(%rbp)
    movl -8(%rbp), %r10d
    movl %r10d, -12(%rbp)
    movl -12(%rbp), %r11d
    imull $5, %r11d
    movl %r11d, -12(%rbp)
    movl $1, -16(%rbp)
    movl -12(%rbp), %r10d
    addl %r10d, -16(%rbp)
    movl -16(%rbp), %eax
    movq %rbp, %rsp
    popq %rbp
    ret
    .section .note.GNU-stack, "",@progbits
