int main(void){
    return 1+2*3/4*5;
}

/*
Program(
  Function(
    name='main'
    body=Return(
      Binary(+,
        Constant(1)
        Binary(*,
          Binary(/,
            Binary(*,
              Constant(2)
              Constant(3)
            )
            Constant(4)
          )
          Constant(5)
        )
      )
    )
  )
)
)*/

/*
IRProgram(
  Function(
    name='main'
    instructions=[
      tmp.0 = 2 * 3         = 6
      tmp.1 = tmp.0 / 4     = 6/4 = 1
      tmp.2 = tmp.1 * 5     = 5
      tmp.3 = 1 + tmp.2     = 6
      return tmp.3
    ]
  )
) // since we only support integers for now the output would be 6
*/

/*
After resolving pseudo registers: 16
*/