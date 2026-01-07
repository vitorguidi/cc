int main() {
    int a = 5;
    int b = 10;
    int c = 2;
    int result = 100;

    if (a + b > 10)
        if (b / a < 1)
            if (c == 2)
                result = 1;
            else
                result = 2;
        else
            result = a * b + c;
    
    return (result > 50) ? (b = 0) ? (result = 1) : (result = result + 8) : (result = 0);
}