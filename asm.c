int main(void) {
  int a = 1;

  while (a < 10) {
    a = a + 1;
  }
  
  
  for (int i = 0; i < 10; i = i + 1) {
    a = a + i;
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
        While(
          Binary(<,
            Var(name='a')
            Constant(10)
          )
          Block(
            Expression(
              Assignment(
                Var(name='a')
                Binary(+,
                  Var(name='a')
                  Constant(1)
                )
              )
            )
          )
        )
      )
    )
  )
)
*/