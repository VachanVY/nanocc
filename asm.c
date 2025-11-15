// 0, 1, 1, 2, 3, 5, 8, ...
int fib(int a);

int main(void) {
  int n = 6;
  return fib(n);
}

int fib(int n) {
  if (n == 0 || n == 1) {
    return n;
  } else {
    return fib(n - 1) + fib(n - 2);
  }
}

/*
----------------------------
-------- Parse Tree --------
Program(
  Function(
    name='fib'
    Parameters(
      name='a'
    )
  )
  Function(
    name='main'
    Parameters()
    body=(
      Block(
        Declaration(
          name='n'
          Constant(6)
        )
        Return(
          FunctionCall(
            name='fib'
            args(
              Var(name='n')
            )
          )
        )
      )
    )
  )
  Function(
    name='fib'
    Parameters(
      name='n'
    )
    body=(
      Block(
        IfElse(
          Binary(||,
            Binary(==,
              Var(name='n')
              Constant(0)
            )
            Binary(==,
              Var(name='n')
              Constant(1)
            )
          )
          Block(
            Return(
              Var(name='n')
            )
          )
          Block(
            Return(
              Binary(+,
                FunctionCall(
                  name='fib'
                  args(
                    Binary(-,
                      Var(name='n')
                      Constant(1)
                    )
                  )
                )
                FunctionCall(
                  name='fib'
                  args(
                    Binary(-,
                      Var(name='n')
                      Constant(2)
                    )
                  )
                )
              )
            )
          )
        )
      )
    )
  )
)
----------------------------
----- Identifier Resolution -----
Program(
  Function(
    name='fib'
    Parameters(
      name='a.0'
    )
  )
  Function(
    name='main'
    Parameters()
    body=(
      Block(
        Declaration(
          name='n.1'
          Constant(6)
        )
        Return(
          FunctionCall(
            name='fib'
            args(
              Var(name='n.1')
            )
          )
        )
      )
    )
  )
  Function(
    name='fib'
    Parameters(
      name='n.2'
    )
    body=(
      Block(
        IfElse(
          Binary(||,
            Binary(==,
              Var(name='n.2')
              Constant(0)
            )
            Binary(==,
              Var(name='n.2')
              Constant(1)
            )
          )
          Block(
            Return(
              Var(name='n.2')
            )
          )
          Block(
            Return(
              Binary(+,
                FunctionCall(
                  name='fib'
                  args(
                    Binary(-,
                      Var(name='n.2')
                      Constant(1)
                    )
                  )
                )
                FunctionCall(
                  name='fib'
                  args(
                    Binary(-,
                      Var(name='n.2')
                      Constant(2)
                    )
                  )
                )
              )
            )
          )
        )
      )
    )
  )
)
---------------------------------
----- Type Checking -----
Program(
  Function(
    name='fib'
    Parameters(
      name='a.0'
    )
  )
  Function(
    name='main'
    Parameters()
    body=(
      Block(
        Declaration(
          name='n.1'
          Constant(6)
        )
        Return(
          FunctionCall(
            name='fib'
            args(
              Var(name='n.1')
            )
          )
        )
      )
    )
  )
  Function(
    name='fib'
    Parameters(
      name='n.2'
    )
    body=(
      Block(
        IfElse(
          Binary(||,
            Binary(==,
              Var(name='n.2')
              Constant(0)
            )
            Binary(==,
              Var(name='n.2')
              Constant(1)
            )
          )
          Block(
            Return(
              Var(name='n.2')
            )
          )
          Block(
            Return(
              Binary(+,
                FunctionCall(
                  name='fib'
                  args(
                    Binary(-,
                      Var(name='n.2')
                      Constant(1)
                    )
                  )
                )
                FunctionCall(
                  name='fib'
                  args(
                    Binary(-,
                      Var(name='n.2')
                      Constant(2)
                    )
                  )
                )
              )
            )
          )
        )
      )
    )
  )
)
-------------------------
----- Loop Labelling -----
Program(
  Function(
    name='fib'
    Parameters(
      name='a.0'
    )
  )
  Function(
    name='main'
    Parameters()
    body=(
      Block(
        Declaration(
          name='n.1'
          Constant(6)
        )
        Return(
          FunctionCall(
            name='fib'
            args(
              Var(name='n.1')
            )
          )
        )
      )
    )
  )
  Function(
    name='fib'
    Parameters(
      name='n.2'
    )
    body=(
      Block(
        IfElse(
          Binary(||,
            Binary(==,
              Var(name='n.2')
              Constant(0)
            )
            Binary(==,
              Var(name='n.2')
              Constant(1)
            )
          )
          Block(
            Return(
              Var(name='n.2')
            )
          )
          Block(
            Return(
              Binary(+,
                FunctionCall(
                  name='fib'
                  args(
                    Binary(-,
                      Var(name='n.2')
                      Constant(1)
                    )
                  )
                )
                FunctionCall(
                  name='fib'
                  args(
                    Binary(-,
                      Var(name='n.2')
                      Constant(2)
                    )
                  )
                )
              )
            )
          )
        )
      )
    )
  )
)
--------------------------
----- IR Generation -----
Function[
  name='main'
  instructions=[
    n.1 = 6
    tmp.3 = fib(n.1)
    return tmp.3
    return 0
  ]
]
Function[
  name='fib'
  instructions=[
    tmp.5 = n.2 == 0
    jump_if_not_zero tmp.5, short.1
    tmp.6 = n.2 == 1
    jump_if_not_zero tmp.6, short.1
    tmp.4 = 0
    jump end.2
  short.1:
    tmp.4 = 1
  end.2:
    jump_if_zero tmp.4, else.0
    return n.2
    jump end.3
  else.0:
    tmp.7 = n.2 - 1
    tmp.8 = fib(tmp.7)
    tmp.9 = n.2 - 2
    tmp.10 = fib(tmp.9)
    tmp.11 = tmp.8 + tmp.10
    return tmp.11
  end.3:
    return 0
  ]
]
-------------------------
*/