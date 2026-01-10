int main() {
    int val = 5;
    int x = 1;
    int result = 0;

    for (int x=0; ; ) { 
        int x = 10; 
        val = val - 1; 

        if (val < 3) break; 

        {
            int val = 100; 
            int i = 0;
            
            while (i < 2) {
                i = i+1;
                if (x > 5) {
                    int x = 50; 
                    result = result + (x / val); 
                }
                
                for (int j = 0; ; ) {
                    int val = 10; 
                    result = result + (val + j);
                    j=j+1;
                    if (j >= 2) break;
                }
                
                x = x - 2; 
            }
        }
        
        result = result + x; 
    }

    return val + result;
}