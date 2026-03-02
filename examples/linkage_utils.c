int putchar(int c);

int putint(int x) {
    if (x < 0) {
        putchar(45);  // ASCII '-'
        x = -x;
    }

    if (x >= 10)
        putint(x / 10);

    putchar(48 + (x % 10));  // ASCII '0' = 48
}

int hello_world(void) {
    // "Hello, World!" using only integers
    putchar(72);    putchar(101);    putchar(108);    
    putchar(108);    putchar(111);    putchar(44);
    putchar(32);    putchar(87);    putchar(111);    
    putchar(114);    putchar(108);    putchar(100);
    putchar(33);    putchar(10);
}

int init_sys(void) {
    static int initialized = 0;

    if (initialized == 1)
        return 0;

    hello_world();

    putchar(73);   // 'I'
    putchar(78);   // 'N'
    putchar(73);   // 'I'
    putchar(84);   // 'T'
    putchar(10);   // '\n'

    initialized = 1;
    return 1;
}
