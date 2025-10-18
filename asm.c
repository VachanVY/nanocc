int main(void){
    return ~(-42);
}

/*
Program(
  Function(
    name='main'
    body=Return(
      Unary(~,
        Unary(-,
          Constant(42)
        )
      )
    )
  )
)
*/

/*
Program(
  FunctionNode(
    name='main'
    body=Return(
        Constant(2)
      )
)

    .globl main
main:
    mov $42, %eax
    ret

*/