// defined in linkage_utils.c
int init_sys(void);

int putchar(int c);
int putint(int x);

// defined in linkage_fact.c
extern int num_times;
int factorial(int n);

int main(void){
    init_sys();
    init_sys(); // WON'T PRINT "INIT" AGAIN
    init_sys(); // WON'T PRINT "INIT" AGAIN

    for (int i = 1; i <= num_times; i = i + 1) {
        int fact = factorial(i);
        putint(i); /*space*/ putchar(32); 
        putint(fact); /*newline*/ putchar(10);
    }
    return 0;
}
