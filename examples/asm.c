/*
prints:
Hello, World!
1 1
2 2
3 6
4 24
5 120
*/
int putchar(int c);

 // return type, only int is supported as of now
int putint(int x) {
    if (x < 0) {
        putchar(45);  // ASCII '-'
        x = -x;
    }

    if (x >= 10)
        putint(x / 10);

    putchar(48 + (x % 10));  // ASCII '0' = 48
    return 0;
}

int factorial(int n) {
    int ret;
    if (n <= 1) {
        ret = 1;
    } else {
        ret = n * factorial(n - 1);
    }
    return ret;
}

int main(void) {
    // "Hello, World!" using only integers
    putchar(72);    putchar(101);    putchar(108);    
    putchar(108);    putchar(111);    putchar(44);
    putchar(32);    putchar(87);    putchar(111);    
    putchar(114);    putchar(108);    putchar(100);
    putchar(33);    putchar(10);

    for (int i = 1; i <= 5; i = i + 1) {
        int fact = factorial(i);
        putint(i); /*space*/ putchar(32); 
        putint(fact); /*newline*/ putchar(10);
    }
    return 0;
}


/*
----- Lexical Analysis -----
| 0 | int | int |
| 1 | identifier | putchar |
| 2 | ( | ( |
| 3 | int | int |
| 4 | identifier | c |
| 5 | ) | ) |
| 6 | ; | ; |
| 7 | int | int |
| 8 | identifier | putint |
| 9 | ( | ( |
| 10 | int | int |
| 11 | identifier | x |
| 12 | ) | ) |
| 13 | { | { |
| 14 | if | if |
| 15 | ( | ( |
| 16 | identifier | x |
| 17 | < | < |
| 18 | constant | 0 |
| 19 | ) | ) |
| 20 | { | { |
| 21 | identifier | putchar |
| 22 | ( | ( |
| 23 | constant | 45 |
| 24 | ) | ) |
| 25 | ; | ; |
| 26 | identifier | x |
| 27 | = | = |
| 28 | - | - |
| 29 | identifier | x |
| 30 | ; | ; |
| 31 | } | } |
| 32 | if | if |
| 33 | ( | ( |
| 34 | identifier | x |
| 35 | >= | >= |
| 36 | constant | 10 |
| 37 | ) | ) |
| 38 | identifier | putint |
| 39 | ( | ( |
| 40 | identifier | x |
| 41 | / | / |
| 42 | constant | 10 |
| 43 | ) | ) |
| 44 | ; | ; |
| 45 | identifier | putchar |
| 46 | ( | ( |
| 47 | constant | 48 |
| 48 | + | + |
| 49 | ( | ( |
| 50 | identifier | x |
| 51 | % | % |
| 52 | constant | 10 |
| 53 | ) | ) |
| 54 | ) | ) |
| 55 | ; | ; |
| 56 | return | return |
| 57 | constant | 0 |
| 58 | ; | ; |
| 59 | } | } |
| 60 | int | int |
| 61 | identifier | factorial |
| 62 | ( | ( |
| 63 | int | int |
| 64 | identifier | n |
| 65 | ) | ) |
| 66 | { | { |
| 67 | int | int |
| 68 | identifier | ret |
| 69 | ; | ; |
| 70 | if | if |
| 71 | ( | ( |
| 72 | identifier | n |
| 73 | <= | <= |
| 74 | constant | 1 |
| 75 | ) | ) |
| 76 | { | { |
| 77 | identifier | ret |
| 78 | = | = |
| 79 | constant | 1 |
| 80 | ; | ; |
| 81 | } | } |
| 82 | else | else |
| 83 | { | { |
| 84 | identifier | ret |
| 85 | = | = |
| 86 | identifier | n |
| 87 | * | * |
| 88 | identifier | factorial |
| 89 | ( | ( |
| 90 | identifier | n |
| 91 | - | - |
| 92 | constant | 1 |
| 93 | ) | ) |
| 94 | ; | ; |
| 95 | } | } |
| 96 | return | return |
| 97 | identifier | ret |
| 98 | ; | ; |
| 99 | } | } |
| 100 | int | int |
| 101 | identifier | main |
| 102 | ( | ( |
| 103 | void | void |
| 104 | ) | ) |
| 105 | { | { |
| 106 | identifier | putchar |
| 107 | ( | ( |
| 108 | constant | 72 |
| 109 | ) | ) |
| 110 | ; | ; |
| 111 | identifier | putchar |
| 112 | ( | ( |
| 113 | constant | 101 |
| 114 | ) | ) |
| 115 | ; | ; |
| 116 | identifier | putchar |
| 117 | ( | ( |
| 118 | constant | 108 |
| 119 | ) | ) |
| 120 | ; | ; |
| 121 | identifier | putchar |
| 122 | ( | ( |
| 123 | constant | 108 |
| 124 | ) | ) |
| 125 | ; | ; |
| 126 | identifier | putchar |
| 127 | ( | ( |
| 128 | constant | 111 |
| 129 | ) | ) |
| 130 | ; | ; |
| 131 | identifier | putchar |
| 132 | ( | ( |
| 133 | constant | 44 |
| 134 | ) | ) |
| 135 | ; | ; |
| 136 | identifier | putchar |
| 137 | ( | ( |
| 138 | constant | 32 |
| 139 | ) | ) |
| 140 | ; | ; |
| 141 | identifier | putchar |
| 142 | ( | ( |
| 143 | constant | 87 |
| 144 | ) | ) |
| 145 | ; | ; |
| 146 | identifier | putchar |
| 147 | ( | ( |
| 148 | constant | 111 |
| 149 | ) | ) |
| 150 | ; | ; |
| 151 | identifier | putchar |
| 152 | ( | ( |
| 153 | constant | 114 |
| 154 | ) | ) |
| 155 | ; | ; |
| 156 | identifier | putchar |
| 157 | ( | ( |
| 158 | constant | 108 |
| 159 | ) | ) |
| 160 | ; | ; |
| 161 | identifier | putchar |
| 162 | ( | ( |
| 163 | constant | 100 |
| 164 | ) | ) |
| 165 | ; | ; |
| 166 | identifier | putchar |
| 167 | ( | ( |
| 168 | constant | 33 |
| 169 | ) | ) |
| 170 | ; | ; |
| 171 | identifier | putchar |
| 172 | ( | ( |
| 173 | constant | 10 |
| 174 | ) | ) |
| 175 | ; | ; |
| 176 | for | for |
| 177 | ( | ( |
| 178 | int | int |
| 179 | identifier | i |
| 180 | = | = |
| 181 | constant | 1 |
| 182 | ; | ; |
| 183 | identifier | i |
| 184 | <= | <= |
| 185 | constant | 5 |
| 186 | ; | ; |
| 187 | identifier | i |
| 188 | = | = |
| 189 | identifier | i |
| 190 | + | + |
| 191 | constant | 1 |
| 192 | ) | ) |
| 193 | { | { |
| 194 | int | int |
| 195 | identifier | fact |
| 196 | = | = |
| 197 | identifier | factorial |
| 198 | ( | ( |
| 199 | identifier | i |
| 200 | ) | ) |
| 201 | ; | ; |
| 202 | identifier | putint |
| 203 | ( | ( |
| 204 | identifier | i |
| 205 | ) | ) |
| 206 | ; | ; |
| 207 | identifier | putchar |
| 208 | ( | ( |
| 209 | constant | 32 |
| 210 | ) | ) |
| 211 | ; | ; |
| 212 | identifier | putint |
| 213 | ( | ( |
| 214 | identifier | fact |
| 215 | ) | ) |
| 216 | ; | ; |
| 217 | identifier | putchar |
| 218 | ( | ( |
| 219 | constant | 10 |
| 220 | ) | ) |
| 221 | ; | ; |
| 222 | } | } |
| 223 | return | return |
| 224 | constant | 0 |
| 225 | ; | ; |
| 226 | } | } |
----------------------------
-------- Parse Tree --------
Program(
  Function(
    name='putchar'
    Parameters(
      name='c'
    )
  )
  Function(
    name='putint'
    Parameters(
      name='x'
    )
    body=(
      Block(
        IfElse(
          Binary(<,
            Var(name='x')
            Constant(0)
          )
          Block(
            Expression(
              FunctionCall(
                name='putchar'
                args(
                  Constant(45)
                )
              )
            )
            Expression(
              Assignment(
                Var(name='x')
                Unary(-
                  Var(name='x')
                )
              )
            )
          )
        )
        IfElse(
          Binary(>=,
            Var(name='x')
            Constant(10)
          )
          Expression(
            FunctionCall(
              name='putint'
              args(
                Binary(/,
                  Var(name='x')
                  Constant(10)
                )
              )
            )
          )
        )
        Expression(
          FunctionCall(
            name='putchar'
            args(
              Binary(+,
                Constant(48)
                Binary(%,
                  Var(name='x')
                  Constant(10)
                )
              )
            )
          )
        )
        Return(
          Constant(0)
        )
      )
    )
  )
  Function(
    name='factorial'
    Parameters(
      name='n'
    )
    body=(
      Block(
        Declaration(
          name='ret')
        IfElse(
          Binary(<=,
            Var(name='n')
            Constant(1)
          )
          Block(
            Expression(
              Assignment(
                Var(name='ret')
                Constant(1)
              )
            )
          )
          Block(
            Expression(
              Assignment(
                Var(name='ret')
                Binary(*,
                  Var(name='n')
                  FunctionCall(
                    name='factorial'
                    args(
                      Binary(-,
                        Var(name='n')
                        Constant(1)
                      )
                    )
                  )
                )
              )
            )
          )
        )
        Return(
          Var(name='ret')
        )
      )
    )
  )
  Function(
    name='main'
    Parameters()
    body=(
      Block(
        Expression(
          FunctionCall(
            name='putchar'
            args(
              Constant(72)
            )
          )
        )
        Expression(
          FunctionCall(
            name='putchar'
            args(
              Constant(101)
            )
          )
        )
        Expression(
          FunctionCall(
            name='putchar'
            args(
              Constant(108)
            )
          )
        )
        Expression(
          FunctionCall(
            name='putchar'
            args(
              Constant(108)
            )
          )
        )
        Expression(
          FunctionCall(
            name='putchar'
            args(
              Constant(111)
            )
          )
        )
        Expression(
          FunctionCall(
            name='putchar'
            args(
              Constant(44)
            )
          )
        )
        Expression(
          FunctionCall(
            name='putchar'
            args(
              Constant(32)
            )
          )
        )
        Expression(
          FunctionCall(
            name='putchar'
            args(
              Constant(87)
            )
          )
        )
        Expression(
          FunctionCall(
            name='putchar'
            args(
              Constant(111)
            )
          )
        )
        Expression(
          FunctionCall(
            name='putchar'
            args(
              Constant(114)
            )
          )
        )
        Expression(
          FunctionCall(
            name='putchar'
            args(
              Constant(108)
            )
          )
        )
        Expression(
          FunctionCall(
            name='putchar'
            args(
              Constant(100)
            )
          )
        )
        Expression(
          FunctionCall(
            name='putchar'
            args(
              Constant(33)
            )
          )
        )
        Expression(
          FunctionCall(
            name='putchar'
            args(
              Constant(10)
            )
          )
        )
        For(
          Init:
            Declaration(
              name='i'
              Constant(1)
            )
          Condition:
            Binary(<=,
              Var(name='i')
              Constant(5)
            )
          Post:
            Assignment(
              Var(name='i')
              Binary(+,
                Var(name='i')
                Constant(1)
              )
            )
          ForLoopBody:
            Block(
              Declaration(
                name='fact'
                FunctionCall(
                  name='factorial'
                  args(
                    Var(name='i')
                  )
                )
              )
              Expression(
                FunctionCall(
                  name='putint'
                  args(
                    Var(name='i')
                  )
                )
              )
              Expression(
                FunctionCall(
                  name='putchar'
                  args(
                    Constant(32)
                  )
                )
              )
              Expression(
                FunctionCall(
                  name='putint'
                  args(
                    Var(name='fact')
                  )
                )
              )
              Expression(
                FunctionCall(
                  name='putchar'
                  args(
                    Constant(10)
                  )
                )
              )
            )
        )
        Return(
          Constant(0)
        )
      )
    )
  )
)
----------------------------
----- Identifier Resolution -----
Program(
  Function(
    name='putchar'
    Parameters(
      name='c.0'
    )
  )
  Function(
    name='putint'
    Parameters(
      name='x.1'
    )
    body=(
      Block(
        IfElse(
          Binary(<,
            Var(name='x.1')
            Constant(0)
          )
          Block(
            Expression(
              FunctionCall(
                name='putchar'
                args(
                  Constant(45)
                )
              )
            )
            Expression(
              Assignment(
                Var(name='x.1')
                Unary(-
                  Var(name='x.1')
                )
              )
            )
          )
        )
        IfElse(
          Binary(>=,
            Var(name='x.1')
            Constant(10)
          )
          Expression(
            FunctionCall(
              name='putint'
              args(
                Binary(/,
                  Var(name='x.1')
                  Constant(10)
                )
              )
            )
          )
        )
        Expression(
          FunctionCall(
            name='putchar'
            args(
              Binary(+,
                Constant(48)
                Binary(%,
                  Var(name='x.1')
                  Constant(10)
                )
              )
            )
          )
        )
        Return(
          Constant(0)
        )
      )
    )
  )
  Function(
    name='factorial'
    Parameters(
      name='n.2'
    )
    body=(
      Block(
        Declaration(
          name='ret.3')
        IfElse(
          Binary(<=,
            Var(name='n.2')
            Constant(1)
          )
          Block(
            Expression(
              Assignment(
                Var(name='ret.3')
                Constant(1)
              )
            )
          )
          Block(
            Expression(
              Assignment(
                Var(name='ret.3')
                Binary(*,
                  Var(name='n.2')
                  FunctionCall(
                    name='factorial'
                    args(
                      Binary(-,
                        Var(name='n.2')
                        Constant(1)
                      )
                    )
                  )
                )
              )
            )
          )
        )
        Return(
          Var(name='ret.3')
        )
      )
    )
  )
  Function(
    name='main'
    Parameters()
    body=(
      Block(
        Expression(
          FunctionCall(
            name='putchar'
            args(
              Constant(72)
            )
          )
        )
        Expression(
          FunctionCall(
            name='putchar'
            args(
              Constant(101)
            )
          )
        )
        Expression(
          FunctionCall(
            name='putchar'
            args(
              Constant(108)
            )
          )
        )
        Expression(
          FunctionCall(
            name='putchar'
            args(
              Constant(108)
            )
          )
        )
        Expression(
          FunctionCall(
            name='putchar'
            args(
              Constant(111)
            )
          )
        )
        Expression(
          FunctionCall(
            name='putchar'
            args(
              Constant(44)
            )
          )
        )
        Expression(
          FunctionCall(
            name='putchar'
            args(
              Constant(32)
            )
          )
        )
        Expression(
          FunctionCall(
            name='putchar'
            args(
              Constant(87)
            )
          )
        )
        Expression(
          FunctionCall(
            name='putchar'
            args(
              Constant(111)
            )
          )
        )
        Expression(
          FunctionCall(
            name='putchar'
            args(
              Constant(114)
            )
          )
        )
        Expression(
          FunctionCall(
            name='putchar'
            args(
              Constant(108)
            )
          )
        )
        Expression(
          FunctionCall(
            name='putchar'
            args(
              Constant(100)
            )
          )
        )
        Expression(
          FunctionCall(
            name='putchar'
            args(
              Constant(33)
            )
          )
        )
        Expression(
          FunctionCall(
            name='putchar'
            args(
              Constant(10)
            )
          )
        )
        For(
          Init:
            Declaration(
              name='i.4'
              Constant(1)
            )
          Condition:
            Binary(<=,
              Var(name='i.4')
              Constant(5)
            )
          Post:
            Assignment(
              Var(name='i.4')
              Binary(+,
                Var(name='i.4')
                Constant(1)
              )
            )
          ForLoopBody:
            Block(
              Declaration(
                name='fact.5'
                FunctionCall(
                  name='factorial'
                  args(
                    Var(name='i.4')
                  )
                )
              )
              Expression(
                FunctionCall(
                  name='putint'
                  args(
                    Var(name='i.4')
                  )
                )
              )
              Expression(
                FunctionCall(
                  name='putchar'
                  args(
                    Constant(32)
                  )
                )
              )
              Expression(
                FunctionCall(
                  name='putint'
                  args(
                    Var(name='fact.5')
                  )
                )
              )
              Expression(
                FunctionCall(
                  name='putchar'
                  args(
                    Constant(10)
                  )
                )
              )
            )
        )
        Return(
          Constant(0)
        )
      )
    )
  )
)
---------------------------------
----- Type Checking -----
Program(
  Function(
    name='putchar'
    Parameters(
      name='c.0'
    )
  )
  Function(
    name='putint'
    Parameters(
      name='x.1'
    )
    body=(
      Block(
        IfElse(
          Binary(<,
            Var(name='x.1')
            Constant(0)
          )
          Block(
            Expression(
              FunctionCall(
                name='putchar'
                args(
                  Constant(45)
                )
              )
            )
            Expression(
              Assignment(
                Var(name='x.1')
                Unary(-
                  Var(name='x.1')
                )
              )
            )
          )
        )
        IfElse(
          Binary(>=,
            Var(name='x.1')
            Constant(10)
          )
          Expression(
            FunctionCall(
              name='putint'
              args(
                Binary(/,
                  Var(name='x.1')
                  Constant(10)
                )
              )
            )
          )
        )
        Expression(
          FunctionCall(
            name='putchar'
            args(
              Binary(+,
                Constant(48)
                Binary(%,
                  Var(name='x.1')
                  Constant(10)
                )
              )
            )
          )
        )
        Return(
          Constant(0)
        )
      )
    )
  )
  Function(
    name='factorial'
    Parameters(
      name='n.2'
    )
    body=(
      Block(
        Declaration(
          name='ret.3')
        IfElse(
          Binary(<=,
            Var(name='n.2')
            Constant(1)
          )
          Block(
            Expression(
              Assignment(
                Var(name='ret.3')
                Constant(1)
              )
            )
          )
          Block(
            Expression(
              Assignment(
                Var(name='ret.3')
                Binary(*,
                  Var(name='n.2')
                  FunctionCall(
                    name='factorial'
                    args(
                      Binary(-,
                        Var(name='n.2')
                        Constant(1)
                      )
                    )
                  )
                )
              )
            )
          )
        )
        Return(
          Var(name='ret.3')
        )
      )
    )
  )
  Function(
    name='main'
    Parameters()
    body=(
      Block(
        Expression(
          FunctionCall(
            name='putchar'
            args(
              Constant(72)
            )
          )
        )
        Expression(
          FunctionCall(
            name='putchar'
            args(
              Constant(101)
            )
          )
        )
        Expression(
          FunctionCall(
            name='putchar'
            args(
              Constant(108)
            )
          )
        )
        Expression(
          FunctionCall(
            name='putchar'
            args(
              Constant(108)
            )
          )
        )
        Expression(
          FunctionCall(
            name='putchar'
            args(
              Constant(111)
            )
          )
        )
        Expression(
          FunctionCall(
            name='putchar'
            args(
              Constant(44)
            )
          )
        )
        Expression(
          FunctionCall(
            name='putchar'
            args(
              Constant(32)
            )
          )
        )
        Expression(
          FunctionCall(
            name='putchar'
            args(
              Constant(87)
            )
          )
        )
        Expression(
          FunctionCall(
            name='putchar'
            args(
              Constant(111)
            )
          )
        )
        Expression(
          FunctionCall(
            name='putchar'
            args(
              Constant(114)
            )
          )
        )
        Expression(
          FunctionCall(
            name='putchar'
            args(
              Constant(108)
            )
          )
        )
        Expression(
          FunctionCall(
            name='putchar'
            args(
              Constant(100)
            )
          )
        )
        Expression(
          FunctionCall(
            name='putchar'
            args(
              Constant(33)
            )
          )
        )
        Expression(
          FunctionCall(
            name='putchar'
            args(
              Constant(10)
            )
          )
        )
        For(
          Init:
            Declaration(
              name='i.4'
              Constant(1)
            )
          Condition:
            Binary(<=,
              Var(name='i.4')
              Constant(5)
            )
          Post:
            Assignment(
              Var(name='i.4')
              Binary(+,
                Var(name='i.4')
                Constant(1)
              )
            )
          ForLoopBody:
            Block(
              Declaration(
                name='fact.5'
                FunctionCall(
                  name='factorial'
                  args(
                    Var(name='i.4')
                  )
                )
              )
              Expression(
                FunctionCall(
                  name='putint'
                  args(
                    Var(name='i.4')
                  )
                )
              )
              Expression(
                FunctionCall(
                  name='putchar'
                  args(
                    Constant(32)
                  )
                )
              )
              Expression(
                FunctionCall(
                  name='putint'
                  args(
                    Var(name='fact.5')
                  )
                )
              )
              Expression(
                FunctionCall(
                  name='putchar'
                  args(
                    Constant(10)
                  )
                )
              )
            )
        )
        Return(
          Constant(0)
        )
      )
    )
  )
)
-------------------------
----- Loop Labelling -----
Program(
  Function(
    name='putchar'
    Parameters(
      name='c.0'
    )
  )
  Function(
    name='putint'
    Parameters(
      name='x.1'
    )
    body=(
      Block(
        IfElse(
          Binary(<,
            Var(name='x.1')
            Constant(0)
          )
          Block(
            Expression(
              FunctionCall(
                name='putchar'
                args(
                  Constant(45)
                )
              )
            )
            Expression(
              Assignment(
                Var(name='x.1')
                Unary(-
                  Var(name='x.1')
                )
              )
            )
          )
        )
        IfElse(
          Binary(>=,
            Var(name='x.1')
            Constant(10)
          )
          Expression(
            FunctionCall(
              name='putint'
              args(
                Binary(/,
                  Var(name='x.1')
                  Constant(10)
                )
              )
            )
          )
        )
        Expression(
          FunctionCall(
            name='putchar'
            args(
              Binary(+,
                Constant(48)
                Binary(%,
                  Var(name='x.1')
                  Constant(10)
                )
              )
            )
          )
        )
        Return(
          Constant(0)
        )
      )
    )
  )
  Function(
    name='factorial'
    Parameters(
      name='n.2'
    )
    body=(
      Block(
        Declaration(
          name='ret.3')
        IfElse(
          Binary(<=,
            Var(name='n.2')
            Constant(1)
          )
          Block(
            Expression(
              Assignment(
                Var(name='ret.3')
                Constant(1)
              )
            )
          )
          Block(
            Expression(
              Assignment(
                Var(name='ret.3')
                Binary(*,
                  Var(name='n.2')
                  FunctionCall(
                    name='factorial'
                    args(
                      Binary(-,
                        Var(name='n.2')
                        Constant(1)
                      )
                    )
                  )
                )
              )
            )
          )
        )
        Return(
          Var(name='ret.3')
        )
      )
    )
  )
  Function(
    name='main'
    Parameters()
    body=(
      Block(
        Expression(
          FunctionCall(
            name='putchar'
            args(
              Constant(72)
            )
          )
        )
        Expression(
          FunctionCall(
            name='putchar'
            args(
              Constant(101)
            )
          )
        )
        Expression(
          FunctionCall(
            name='putchar'
            args(
              Constant(108)
            )
          )
        )
        Expression(
          FunctionCall(
            name='putchar'
            args(
              Constant(108)
            )
          )
        )
        Expression(
          FunctionCall(
            name='putchar'
            args(
              Constant(111)
            )
          )
        )
        Expression(
          FunctionCall(
            name='putchar'
            args(
              Constant(44)
            )
          )
        )
        Expression(
          FunctionCall(
            name='putchar'
            args(
              Constant(32)
            )
          )
        )
        Expression(
          FunctionCall(
            name='putchar'
            args(
              Constant(87)
            )
          )
        )
        Expression(
          FunctionCall(
            name='putchar'
            args(
              Constant(111)
            )
          )
        )
        Expression(
          FunctionCall(
            name='putchar'
            args(
              Constant(114)
            )
          )
        )
        Expression(
          FunctionCall(
            name='putchar'
            args(
              Constant(108)
            )
          )
        )
        Expression(
          FunctionCall(
            name='putchar'
            args(
              Constant(100)
            )
          )
        )
        Expression(
          FunctionCall(
            name='putchar'
            args(
              Constant(33)
            )
          )
        )
        Expression(
          FunctionCall(
            name='putchar'
            args(
              Constant(10)
            )
          )
        )
        For(name='for.0'
          Init:
            Declaration(
              name='i.4'
              Constant(1)
            )
          Condition:
            Binary(<=,
              Var(name='i.4')
              Constant(5)
            )
          Post:
            Assignment(
              Var(name='i.4')
              Binary(+,
                Var(name='i.4')
                Constant(1)
              )
            )
          ForLoopBody:
            Block(
              Declaration(
                name='fact.5'
                FunctionCall(
                  name='factorial'
                  args(
                    Var(name='i.4')
                  )
                )
              )
              Expression(
                FunctionCall(
                  name='putint'
                  args(
                    Var(name='i.4')
                  )
                )
              )
              Expression(
                FunctionCall(
                  name='putchar'
                  args(
                    Constant(32)
                  )
                )
              )
              Expression(
                FunctionCall(
                  name='putint'
                  args(
                    Var(name='fact.5')
                  )
                )
              )
              Expression(
                FunctionCall(
                  name='putchar'
                  args(
                    Constant(10)
                  )
                )
              )
            )
        )
        Return(
          Constant(0)
        )
      )
    )
  )
)
--------------------------
----------- IR Generation -----------
Function[
  name='putint'
  parameters=[
    'x.1'
  ]
  instructions=[
    tmp.6 = x.1 < 0
    jump_if_false tmp.6, end.1
    tmp.7 = putchar(45)
    tmp.8 = - x.1
    x.1 = tmp.8
  end.1:
    tmp.9 = x.1 >= 10
    jump_if_false tmp.9, end.2
    tmp.10 = x.1 / 10
    tmp.11 = putint(tmp.10)
  end.2:
    tmp.12 = x.1 % 10
    tmp.13 = 48 + tmp.12
    tmp.14 = putchar(tmp.13)
    return 0
    return 0
  ]
]
Function[
  name='factorial'
  parameters=[
    'n.2'
  ]
  instructions=[
    tmp.15 = n.2 <= 1
    jump_if_false tmp.15, else.3
    ret.3 = 1
    jump end.4
  else.3:
    tmp.16 = n.2 - 1
    tmp.17 = factorial(tmp.16)
    tmp.18 = n.2 * tmp.17
    ret.3 = tmp.18
  end.4:
    return ret.3
    return 0
  ]
]
Function[
  name='main'
  parameters=[]
  instructions=[
    tmp.19 = putchar(72)
    tmp.20 = putchar(101)
    tmp.21 = putchar(108)
    tmp.22 = putchar(108)
    tmp.23 = putchar(111)
    tmp.24 = putchar(44)
    tmp.25 = putchar(32)
    tmp.26 = putchar(87)
    tmp.27 = putchar(111)
    tmp.28 = putchar(114)
    tmp.29 = putchar(108)
    tmp.30 = putchar(100)
    tmp.31 = putchar(33)
    tmp.32 = putchar(10)
    i.4 = 1
  start_for.0:
    tmp.33 = i.4 <= 5
    jump_if_false tmp.33, break_for.0
    tmp.34 = factorial(i.4)
    fact.5 = tmp.34
    tmp.35 = putint(i.4)
    tmp.36 = putchar(32)
    tmp.37 = putint(fact.5)
    tmp.38 = putchar(10)
  continue_for.0:
    tmp.39 = i.4 + 1
    i.4 = tmp.39
    jump start_for.0
  break_for.0:
    return 0
    return 0
  ]
]
-------------------------------------
Function putint: Stack Size = 40
Function factorial: Stack Size = 24
Function main: Stack Size = 92
*/