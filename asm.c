int main(void) {
    return (1 + 1) && (1 - 1);
}

/*
Program(
  Function(
    name='main'
    body=Return(
      Binary(&&,
        Binary(+,
          Constant(1)
          Constant(1)
        )
        Binary(-,
          Constant(1)
          Constant(1)
        )
      )
    )
  )
)
IRProgram(
  Function(
    name='main'
    instructions=[
      tmp.1 = 1 + 1
      jump_if_zero tmp.1, short.0
      tmp.2 = 1 - 1
      jump_if_zero tmp.2, short.0
      tmp.0 = 1
      jump end.1
    short.0:
      tmp.0 = 0
    end.1:
      return tmp.0
    ]
  )
)
After resolving pseudo registers: 12
Successfully compiled to executable with result: 0
*/