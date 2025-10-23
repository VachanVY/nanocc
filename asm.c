int main(void){
    return 1+2*3;
}

/*
Program(
  Function(
    name='main'
    body=Return(
      Unary(~,
        Unary(-,
          Unary(~,
            Unary(-,
              Constant(42)
            )
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
      tmp.0 = - 42
      tmp.1 = ~ tmp.0
      tmp.2 = - tmp.1
      tmp.3 = ~ tmp.2
      return tmp.3
    ]
  )
)
*/