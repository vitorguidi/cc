
int fibonacci(int i) {
    if (i<=1) return i;
    return fibonacci(i-1) + fibonacci(i-2);
}

int main() {
    int sum = 0;
    for(int i=0;i<8;i=i+1) {
        sum = sum + fibonacci(i-1) + fibonacci(i-2);
    }
    return sum;
}