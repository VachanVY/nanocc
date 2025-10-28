# x64 assembly
* `movl`: `l` for longwords -> 32 bit
* `movq`: `q` for quadwords -> 64 bit
* `%rsp`: stack pointer
    <img width="1600" height="471" alt="image" src="https://github.com/user-attachments/assets/6a0a2b72-4da7-4ba4-9dcb-763b0d179c3f" />
* `%rbp`: base pointer to caller stack Frame
    <img width="1600" height="522" alt="image" src="https://github.com/user-attachments/assets/2bbf2663-12de-4a76-934d-734e889ae425" />

    By convention, the value stored in the %RBP register is the base address (highest address) of the function's stack frame.
    Primary Content of %RBP is the Base address of the current stack frame.

* <img width="1035" height="1586" alt="image" src="https://github.com/user-attachments/assets/5c420ab9-5e0b-4fb2-ab26-e0f8e7a80382" />

* ```asm
        .globl main
    main:
        ;
        pushq %rbp
        movq %rsp, %rbp
        subq $8, %rsp
        ;
        movl $42, -4(%rbp)
        negl -4(%rbp)
  
        movl -4(%rbp), %r10d
        movl %r10d, -8(%rbp)
  
        notl -4(%rbp)
        ;
        movl -4(%rbp), %eax
        movq %rbp, %rsp
        popq %rbp
        ret
        .section .note.GNU-stack, "",@progbits
    ```
