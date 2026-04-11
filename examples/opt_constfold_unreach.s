    .globl main
    .text
main:
    pushq %rbp
    movq %rsp, %rbp
    subq $16, %rsp
    movl $1, -4(%rbp)
    movl $0, -8(%rbp)
    movl -8(%rbp), %eax
    movq %rbp, %rsp
    popq %rbp
    ret


    .section .note.GNU-stack, "",@progbits
