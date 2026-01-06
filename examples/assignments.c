int main() {
    int a = 2 % 10 + 4;
    int b = 10;
    int c = -200;
    int w; int z; int t; int v;

    w = z = t = v = (a = b) + (c = 5);

    int e = ~-a & !!b | (c ^ v);

    (a = 1) || (b = 2) && (v = z = 3) | 10 >> 2 < 5;

    int f = a || (w = w + 1);

    int g = (v > a) == (e < b) != (f >= c);

    ; ; ;

    (a = 2 % 3 + 4) > (b = v = 2 + 10 & 10);

    return (a + b + c + w + z + t + v + e + f + g) % 127;
}