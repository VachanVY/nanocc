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
*/