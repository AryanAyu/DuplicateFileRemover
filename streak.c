#include <stdio.h>
#include <stdlib.h>

int streak(int n) {
    int streak = 1;
    n += 1;
    int i = 2;
    while(1){
        if (n%i == 0) {
            streak += 1;
            i++;n++;
     }
     else
       break;
   }
   return streak;
}

int main(int argc, char const *argv[]) {
    int a[101][2];
    for (int n=2; n <=100 ;n++) {
        /* code */
        int st = streak(n);
        a[n][0]=n;
        a[n][1]=st;
    }
    for (int i = 2; i <=100; i++) {
        /* code */
        printf("\n%d <-> %d\n", a[i][0],a[i][1]);
    }
}
