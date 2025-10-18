int main(void){
    return 42;
}

/*
Program(
  FunctionNode(
    name='main'
    body=Return(
        Int(2)
      )
)

    .globl main
main:
    mov $42, %eax
    ret

*/