int num_times = 10;
int factorial(int n) {
    int ret;
    if (n <= 1) {
        ret = 1;
    } else {
        ret = n * factorial(n - 1);
    }
    return ret;
}
