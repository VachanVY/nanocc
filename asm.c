int main(void) {
    int sum = 0;
    for (int i = 0; i < 10;) {
        i = i + 1;
        if (i % 2) // if i is odd don't add to sum
            continue;
        sum = sum + i;
    }
    return sum;
}

/*
----------------------------
-------- Parse Tree --------
Program(
  Function(
    name='main'
    body=(
      Block(
        Declaration(
          name='sum'
          Constant(0)
        )
        For(
          Init:
            Declaration(
              name='i'
              Constant(0)
            )
          Condition:
            Binary(<,
              Var(name='i')
              Constant(10)
            )
          ForLoopBody:
            Block(
              Expression(
                Assignment(
                  Var(name='i')
                  Binary(+,
                    Var(name='i')
                    Constant(1)
                  )
                )
              )
              IfElse(
                Binary(%,
                  Var(name='i')
                  Constant(2)
                )
                Continue()
              )
              Expression(
                Assignment(
                  Var(name='sum')
                  Binary(+,
                    Var(name='sum')
                    Var(name='i')
                  )
                )
              )
            )
        )
        Return(
          Var(name='sum')
        )
      )
    )
  )
)
----------------------------
----- Identifier Resolution -----
Program(
  Function(
    name='main'
    body=(
      Block(
        Declaration(
          name='sum.0'
          Constant(0)
        )
        For(
          Init:
            Declaration(
              name='i.1'
              Constant(0)
            )
          Condition:
            Binary(<,
              Var(name='i.1')
              Constant(10)
            )
          ForLoopBody:
            Block(
              Expression(
                Assignment(
                  Var(name='i.1')
                  Binary(+,
                    Var(name='i.1')
                    Constant(1)
                  )
                )
              )
              IfElse(
                Binary(%,
                  Var(name='i.1')
                  Constant(2)
                )
                Continue()
              )
              Expression(
                Assignment(
                  Var(name='sum.0')
                  Binary(+,
                    Var(name='sum.0')
                    Var(name='i.1')
                  )
                )
              )
            )
        )
        Return(
          Var(name='sum.0')
        )
      )
    )
  )
)
---------------------------------
----- Loop Labelling -----
Program(
  Function(
    name='main'
    body=(
      Block(
        Declaration(
          name='sum.0'
          Constant(0)
        )
        For(name='for.0'
          Init:
            Declaration(
              name='i.1'
              Constant(0)
            )
          Condition:
            Binary(<,
              Var(name='i.1')
              Constant(10)
            )
          ForLoopBody:
            Block(
              Expression(
                Assignment(
                  Var(name='i.1')
                  Binary(+,
                    Var(name='i.1')
                    Constant(1)
                  )
                )
              )
              IfElse(
                Binary(%,
                  Var(name='i.1')
                  Constant(2)
                )
                Continue(name='for.0')
              )
              Expression(
                Assignment(
                  Var(name='sum.0')
                  Binary(+,
                    Var(name='sum.0')
                    Var(name='i.1')
                  )
                )
              )
            )
        )
        Return(
          Var(name='sum.0')
        )
      )
    )
  )
)
--------------------------
----------- IR Generation -----------
IRProgram(
  Function(
    name='main'
    instructions=[
      sum.0 = 0
      i.1 = 0
    start_for.0:
      tmp.2 = i.1 < 10
      jump_if_zero tmp.2, break_for.0
      tmp.3 = i.1 + 1
      i.1 = tmp.3
      tmp.4 = i.1 % 2
      jump_if_zero tmp.4, end.1
      jump continue_for.0
    end.1:
      tmp.5 = sum.0 + i.1
      sum.0 = tmp.5
    continue_for.0:
      jump start_for.0
    break_for.0:
      return sum.0
      return 0
    ]
  )
)
-------------------------------------
After resolving pseudo registers: 24
Successfully compiled to executable with result: 0
*/