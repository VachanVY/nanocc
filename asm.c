int main(void) {
    return (10 && 0) + (0 && 4) + (0 && 0);
}

/*
Program(
  Function(
    name='main'
    body=Return(
      Binary(+,
        Binary(+,
          Binary(&&,
            Constant(10)
            Constant(0)
          )
          Binary(&&,
            Constant(0)
            Constant(4)
          )
        )
        Binary(&&,
          Constant(0)
          Constant(0)
        )
      )
    )
  )
)
*/