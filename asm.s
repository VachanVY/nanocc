    .globl main
main:
    pushq %rbp
    movq %rsp, %rbp
    subq $12, %rsp
    movl $1, -4(%rbp) # -4(%rbp) = 1
    addl $1, -4(%rbp) # -4(%rbp) = 1+1 = 2
    cmpl $0, -4(%rbp) # compare -4(%rbp) and 0
    je short.0        # if equal, jump to short.0
    movl $1, -8(%rbp) # -8(%rbp) = 1
    subl $1, -8(%rbp) # -8(%rbp) = 1-1 = 0
    cmpl $0, -8(%rbp) # compare -8(%rbp) and 0
    je short.0        # if equal, jump to short.0
    movl $1, -12(%rbp) # else move 1 to -12(%rbp)
    jmp end.1         # jump to end.1
  short.0:
    movl $0, -12(%rbp) # move 0 to -12(%rbp)
  end.1:
    movl -12(%rbp), %eax # move -12(%rbp) to %eax
    movq %rbp, %rsp
    popq %rbp
    ret
    .section .note.GNU-stack, "",@progbits
