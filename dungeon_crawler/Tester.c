#include <stdio.h>
#include <stdlib.h>
#include <time.h>  
  

int main(int argc, char const *argv[])
{
    int randomgetal16 = srand() % 17;  // 0 t/m 16
    int bin = 0, rem = 0, place = 1;
    char binary[9];
    int temp = bin;
    int i = 0;

    while (randomgetal16)
    {
        rem = randomgetal16 % 2;
        randomgetal16 = randomgetal16 / 2 ;
        bin = bin + (rem * place);
        place = place * 10;
        
    }
    
    while (temp > 0) {
    binary[9] = temp % 10;  // Store last digit
    temp /= 10;               // Remove last digit
    } 
    
    // Print in reverse (original order)
    printf("Binary digits: ");
    for (int j = i - 1; j >= 0; j--) {
        printf("%d ", binary[j]);
    }

    return 0;
}
  
  