    .globl factorial
    .text
factorial:
    pushq %rbp
    movq %rsp, %rbp
    subq $32, %rsp
    movl %edi, -4(%rbp)
    cmpl $1, -4(%rbp)
    movl $0, -8(%rbp)
    setle -8(%rbp)
    cmpl $0, -8(%rbp)
    je else.0
    movl $1, -12(%rbp)
    jmp end.1
  else.0:
    movl -4(%rbp), %r10d
    movl %r10d, -16(%rbp)
    subl $1, -16(%rbp)
    movl -16(%rbp), %edi
    call factorial
    movl %eax, -20(%rbp)
    movl -4(%rbp), %r10d
    movl %r10d, -24(%rbp)
    movl -24(%rbp), %r11d
    imull -20(%rbp), %r11d
    movl %r11d, -24(%rbp)
    movl -24(%rbp), %r10d
    movl %r10d, -12(%rbp)
  end.1:
    movl -12(%rbp), %eax
    movq %rbp, %rsp
    popq %rbp
    ret

    movl $0, %eax
    movq %rbp, %rsp
    popq %rbp
    ret

    .globl num_times
    .data
    .align 4
num_times:
    .long 10

    .section .note.GNU-stack, "",@progbits
