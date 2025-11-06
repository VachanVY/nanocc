int main(void) {
  int a = 1;
  int b = 2;
  if (a) {
    int a = 1;
    return a;
  }
  else 
    if (b) {
      return b;
    } else {
      return 3;
    }
}

/*
Program(
  Function(
    name='main'
    body=(
      Block(
        Declaration(
          name='a'
          Constant(1)
        )
        Declaration(
          name='b'
          Constant(2)
        )
        IfElse(
          Var(name='a')
          Block(
            Declaration(
              name='a'
              Constant(1)
            )
            Return(
              Var(name='a')
            )
          )
          IfElse(
            Var(name='b')
            Block(
              Return(
                Var(name='b')
              )
            )
            Block(
              Return(
                Constant(3)
              )
            )
          )
        )
      )
    )
  )
)
Program(
  Function(
    name='main'
    body=(
      Block(
        Declaration(
          name='a.0'
          Constant(1)
        )
        Declaration(
          name='b.1'
          Constant(2)
        )
        IfElse(
          Var(name='a.0')
          Block(
            Declaration(
              name='a.2'
              Constant(1)
            )
            Return(
              Var(name='a.2')
            )
          )
          IfElse(
            Var(name='b.1')
            Block(
              Return(
                Var(name='b.1')
              )
            )
            Block(
              Return(
                Constant(3)
              )
            )
          )
        )
      )
    )
  )
)
IRProgram(
  Function(
    name='main'
    instructions=[
      a.0 = 1
      b.1 = 2
      jump_if_zero a.0, else.0
      a.2 = 1
      return a.2
      jump end.1
    else.0:
      jump_if_zero b.1, else.2
      return b.1
      jump end.3
    else.2:
      return 3
    end.3:
    end.1:
      return 0
    ]
  )
)
After resolving pseudo registers: 12
Successfully compiled to executable with result: 0
*/