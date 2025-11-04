int main(void) {
  int a = 0;
  int b = a ? 1 : 0;
  if (a)
      return 1;
  else
      return 2;
}

/*
    .globl main
main:
    pushq %rbp
    movq %rsp, %rbp
    subq $12, %rsp
    movl $0, -4(%rbp) # a = 0
    cmpl $0, -4(%rbp) # if (a==0)
    je else_branch.0  # then jump to else
    movl $1, -8(%rbp) # if (a != 0): b = 1
    jmp end.1         # jump to end
  else_branch.0:      
    movl $0, -8(%rbp)
  end.1:
    movl -8(%rbp), %r10d
    movl %r10d, -12(%rbp)
    movl $0, %eax
    movq %rbp, %rsp
    popq %rbp
    ret
    .section .note.GNU-stack, "",@progbits

*/
/*
Program(
  Function(
    name='main'
    body=(
      Declaration(
        name='b')
      Declaration(
        name='a'
        Constant(1)
      )
      Expression(
        Assignment(
          Var(name='a')
          Binary(*,
            Var(name='a')
            Constant(2)
          )
        )
      )
      Expression(
        Assignment(
          Var(name='b')
          Assignment(
            Var(name='a')
            Var(name='a')
          )
        )
      )
      Declaration(
        name='c'
        Constant(0)
      )
      Expression(
        Binary(+,
          Var(name='a')
          Constant(3)
        )
      )
      Expression(
        Unary(!
          Var(name='a')
        )
      )
      Return(
        Constant(0)
      )
    )
  )
)
Program(
  Function(
    name='main'
    body=(
      Declaration(
        name='b.0')
      Declaration(
        name='a.1'
        Constant(1)
      )
      Expression(
        Assignment(
          Var(name='a.1')
          Binary(*,
            Var(name='a.1')
            Constant(2)
          )
        )
      )
      Expression(
        Assignment(
          Var(name='b.0')
          Assignment(
            Var(name='a.1')
            Var(name='a.1')
          )
        )
      )
      Declaration(
        name='c.2'
        Constant(0)
      )
      Expression(
        Binary(+,
          Var(name='a.1')
          Constant(3)
        )
      )
      Expression(
        Unary(!
          Var(name='a.1')
        )
      )
      Return(
        Constant(0)
      )
    )
  )
)
*/