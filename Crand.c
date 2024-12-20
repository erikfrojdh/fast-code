#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/time.h>
#include <stdint.h>
#include <time.h>
int comp(const void* a, const void* b) {
 
      // If a is smaller, positive value will be returned
    return (*(unsigned*)a - *(unsigned*)b);
}


int main(int argc, char* argv[]){

    size_t len = strtoul(argv[1], NULL, 0);
    // size_t len = 20;
    unsigned long long sum = 0;
    unsigned int* vec = (unsigned int*)malloc(len * sizeof(unsigned));
 
        srand(time(NULL));
        for (int i=0; i < len; i++){
            vec[i] = rand()%100;
            // printf("%d\n", vec[i]);
        }
 
        if (argc > 2){
            qsort(vec, len, sizeof(unsigned), comp);
            printf("Sorted\n");
        }
 
        struct timeval tv1, tv2;
 
        gettimeofday(&tv1,NULL);
        for (int i=0; i < len; i++)
                if (vec[i] < 50)
                        sum=sum+vec[i];
        gettimeofday(&tv2,NULL);

        // for (int i=0; i < len; i++){
        //     printf("%d\n", vec[i]);
        // }
 
        uint64_t diff = tv2.tv_sec*(uint64_t)1000000+tv2.tv_usec - (tv1.tv_sec*(uint64_t)1000000+tv1.tv_usec);
 
        printf("Sum: %d, Time diff: %d µs \n", sum, diff);
}
