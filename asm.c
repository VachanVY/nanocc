int main(void) {
    return (10 + 1) && (3 * 4);
}

/*
Program(
  Function(
    name='main'
    body=Return(
      Binary(&&,
        Binary(+,
          Constant(10)
          Constant(1)
        )
        Binary(*,
          Constant(3)
          Constant(4)
        )
      )
    )
  )
)
*/

/*
IRProgram(
  Function(
    name='main'
    instructions=[
      tmp.1 = 10 + 1
      jump_if_zero tmp.1, short.0
      tmp.2 = 3 * 4
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
*/