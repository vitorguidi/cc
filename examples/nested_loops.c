int main() {
    int total = 0;
    int i = 0;

    while (i < 5)
        for (int j = 0; j < 3; j = j + 1)
            for (int k = 0; k < 2; )
                for (int l = 0; ; l = l + 1)
                    for (int m = 0; ; )
                        do
                            if (i + j > 5)
                                if (total > 10)
                                    break;
                                else
                                    continue;
                            else if (i == 2)
                                for (int n = 0; n < 1; )
                                    if (total == 5)
                                        return total;
                                    else
                                        total = total + (n = n + 1);
                            else
                                total = total + 1;
                        while (k = k + 1);
    
    return total;
}