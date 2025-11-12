// 0, 1, 1, 2, 3, 5, 8, ...
int fib(int n) {
  if (n == 0 || n == 1) {
    return n;
  } else {
    return fib(n - 1) + fib(n - 2);
  }
}

int main(void) {
  int n = 6;
  return fib(n);
}

/*
-------- Parse Tree --------
Program(
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
)
----------------------------
*/