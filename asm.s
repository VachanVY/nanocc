    .globl main
main:
    pushq %rbp
    movq %rsp, %rbp
    subq $12, %rsp
    movl $1, -4(%rbp)
    movl $2, -8(%rbp)
    cmpl $0, -4(%rbp)
    je else.0
    movl $1, -12(%rbp)
    movl -12(%rbp), %eax
    movq %rbp, %rsp
    popq %rbp
    ret
    jmp end.1
  else.0:
    cmpl $0, -8(%rbp)
    je else.2
    movl -8(%rbp), %eax
    movq %rbp, %rsp
    popq %rbp
    ret
    jmp end.3
  else.2:
    movl $3, %eax
    movq %rbp, %rsp
    popq %rbp
    ret
  end.3:
  end.1:
    movl $0, %eax
    movq %rbp, %rsp
    popq %rbp
    ret
    .section .note.GNU-stack, "",@progbits
