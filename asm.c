int main(void) {
    return (0 == 0 && 3 == 2 + 1 > 1) + 1;
}

/*
Program(
  Function(
    name='main'
    body=Return(
      Binary(+,
        Binary(&&,
          Binary(==,
            Constant(0)
            Constant(0)
          )
          Binary(==,
            Constant(3)
            Binary(>,
              Binary(+,
                Constant(2)
                Constant(1)
              )
              Constant(1)
            )
          )
        )
        Constant(1)
      )
    )
  )
)
IRProgram(
  Function(
    name='main'
    instructions=[
      tmp.1 = 0 == 0
      jump_if_zero tmp.1, short.0
      tmp.2 = 2 + 1
      tmp.3 = tmp.2 > 1
      tmp.4 = 3 == tmp.3
      jump_if_zero tmp.4, short.0
      tmp.0 = 1
      jump end.1
    short.0:
      tmp.0 = 0
    end.1:
      tmp.5 = tmp.0 + 1
      return tmp.5
    ]
  )
)
After resolving pseudo registers: 24
Successfully compiled to executable with result: 0
*/