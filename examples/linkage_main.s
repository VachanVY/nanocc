    .globl main
    .text
main:
    pushq %rbp
    movq %rsp, %rbp
    subq $48, %rsp
    call init_sys@PLT
    movl %eax, -4(%rbp)
    call init_sys@PLT
    movl %eax, -8(%rbp)
    call init_sys@PLT
    movl %eax, -12(%rbp)
    movl $1, -16(%rbp)
  start_for.0:
    movl num_times(%rip), %r10d
    cmpl %r10d, -16(%rbp)
    movl $0, -20(%rbp)
    setle -20(%rbp)
    cmpl $0, -20(%rbp)
    je break_for.0
    movl -16(%rbp), %edi
    call factorial@PLT
    movl %eax, -24(%rbp)
    movl -24(%rbp), %r10d
    movl %r10d, -28(%rbp)
    movl -16(%rbp), %edi
    call putint@PLT
    movl %eax, -32(%rbp)
    movl $32, %edi
    call putchar@PLT
    movl %eax, -36(%rbp)
    movl -28(%rbp), %edi
    call putint@PLT
    movl %eax, -40(%rbp)
    movl $10, %edi
    call putchar@PLT
    movl %eax, -44(%rbp)
  continue_for.0:
    movl -16(%rbp), %r10d
    movl %r10d, -48(%rbp)
    addl $1, -48(%rbp)
    movl -48(%rbp), %r10d
    movl %r10d, -16(%rbp)
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
