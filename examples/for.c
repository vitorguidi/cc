int main() {
    int x = 7;
    int res = 0;

    for (int x = 1; x < 3; x = x + 1)
        for (int x = 10; x < 12; x = x + 1)
            for (int x = 100; res < 5000; )
                for (int x = 1000; ; )
                    if (res < 2000)
                        do
                            if (res < 1500)
                                res = res + x;
                            else
                                break;
                        while (x < 0);
                    else
                        if (x == 1000)
                            for (int x = 5000; x < 5001; x = x + 1)
                                return res + x;
                        else
                            break;

    return x;
}