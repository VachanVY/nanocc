    .globl main
main:
    pushq %rbp
    movq %rsp, %rbp
    subq $12, %rsp
    movl $0, -4(%rbp)
    cmpl $0, -4(%rbp)
    je else_branch.0
    movl $1, -8(%rbp)
    jmp end.1
  else_branch.0:
    movl $0, -8(%rbp)
  end.1:
    movl -8(%rbp), %r10d
    movl %r10d, -12(%rbp)
    cmpl $0, -4(%rbp)
    je else.2
    movl $1, %eax
    movq %rbp, %rsp
    popq %rbp
    ret
    jmp end.3
  else.2:
    movl $2, %eax
    movq %rbp, %rsp
    popq %rbp
    ret
  end.3:
    movl $0, %eax
    movq %rbp, %rsp
    popq %rbp
    ret
    .section .note.GNU-stack, "",@progbits
