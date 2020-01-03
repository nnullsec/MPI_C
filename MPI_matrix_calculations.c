#include <stdio.h>
#include "mpi.h"

#define RED     "\x1b[31m"
#define CYAN    "\x1b[36m"
#define RESET   "\x1b[0m"

#define N 4


int main(int argc, char** argv){
    int my_rank,p,intsz;
    int arrA[N],arrB[N],arrC[N][N],arrD[N][N];
    int tmparrA[N*N]={0}, tmparrB[N*N]={0}, locA;
    int i,j,sum=0,nduty=1;
    char option='0';

    MPI_Status status;
    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &my_rank);
    MPI_Comm_size(MPI_COMM_WORLD, &p);

    for(;;){

        if(my_rank == 0){
            int cntr0=1,cntr1=1;
            
            if(N%p){// if p does not divide N
                printf("\ninvalid # of processes: returning with -1\n");
                return -1;
            }
            else if(N!=p)
                nduty=N/p;

            for(i=0; i<N; i++){
                for(j=0; j<N; j++){
                    arrC[i][j]=cntr1;
                    arrD[i][j]=cntr1++;
                }
            }
            printf(RED"\n---- Program starts here ----\n"RESET);
            printf(CYAN" 1. C+D\n 2. C*B\n 3. A*B\n Input: "RESET);
            fflush(stdout);
            scanf(" %c",&option);

            if(option=='3'){
                if(N!=p){
                    printf("\nUnfortunatelly this won't work...\n3rd option requires N == p\n\n");
                    return -1;
                }
                printf("\n===========   A   ===========\n");
                for(i=0; i<N; i++,cntr0++){
                    arrA[i]=cntr0; 
                    printf("%3d ",arrA[i]);
                }
            }
            if(option=='2' || option=='3'){
                if(option=='2')
                    if(N!=p){
                        printf("\nUnfortunatelly this won't work...\n2nd option requires N == p\n\n");
                        return -1;
                    }
                printf("\n===========   B   ===========\n");
                for(i=0,cntr0=N+1; i<N; i++,cntr0++){                
                    arrB[i]=cntr0;
                    printf("%3d\n", arrB[i]);
                }
            }
            if(option=='1' || option=='2'){
                printf("\n===========   C   ===========\n");
                for(i=0; i<N; i++){
                    for(j=0; j<N; j++){
                        printf("%3d ",arrC[i][j]);
                        if(j == N-1)
                            printf("\n");
                    }
                }
            }
            if(option=='1'){
                printf("\n===========   D   ===========\n");
                for(i=0; i<N; i++){
                    for(j=0; j<N; j++){
                        printf("%3d ",arrD[i][j]);
                        if(j == N-1)
                            printf("\n");
                    }
                }
            }
        }
MPI_Bcast(&option, 1, MPI_CHAR, 0, MPI_COMM_WORLD);
MPI_Bcast(&nduty, 1, MPI_CHAR, 0, MPI_COMM_WORLD);

/*************************************************
******************* erwthma 1 ********************
*************************************************/
        
        if(option=='1'){
            MPI_Scatter(arrC, N*nduty, MPI_INT, tmparrA, N*nduty, MPI_INT, 0, MPI_COMM_WORLD);
            MPI_Scatter(arrD, N*nduty, MPI_INT, tmparrB, N*nduty, MPI_INT, 0, MPI_COMM_WORLD);

            for(i=0;i<N*nduty;i++)
                tmparrA[i]+=tmparrB[i];
            
            MPI_Gather(tmparrA, N*nduty, MPI_INT, arrD, N*nduty, MPI_INT, 0, MPI_COMM_WORLD);
            
            if(my_rank==0){
                printf("\n===========  C+D  ===========\n");
                for(i=0; i<N; i++){
                    for(j=0; j<N; j++){
                        printf("%3d ",arrD[i][j]);
                    if(j == N-1)
                        printf("\n");
                    }
                }
            }
        }

/*************************************************
******************* erwthma 2 ********************
*************************************************/

        if(option=='2'){
            locA=0;

            MPI_Scatter(arrC, N, MPI_INT, tmparrA, N, MPI_INT, 0, MPI_COMM_WORLD);
            MPI_Bcast(arrB, N, MPI_INT, 0, MPI_COMM_WORLD);
            
            for(i=0; i<N; i++)
                    locA += arrB[i] * tmparrA[i];

            MPI_Gather(&locA, 1, MPI_INT, tmparrB, 1, MPI_INT, 0, MPI_COMM_WORLD);
            
            if (my_rank == 0){
                printf("\n===========  C*B  ============\n");
                for (i=0; i<N; i++){
                    printf("%3d\n", tmparrB[i]);
                }
            }
        }

/*************************************************
******************* erwthma 3 ********************
*************************************************/

        if(option=='3'){
            sum=0;

            MPI_Scatter(arrA, N/p, MPI_INT, tmparrA, N/p, MPI_INT, 0, MPI_COMM_WORLD);
            MPI_Scatter(arrB, N/p, MPI_INT, tmparrB, N/p, MPI_INT, 0, MPI_COMM_WORLD);

            for(i=0;i<N/p;i++)
                tmparrA[i]*=tmparrB[i];
            
            MPI_Reduce(tmparrA, &sum, 1, MPI_INT, MPI_SUM, 0, MPI_COMM_WORLD);

            if(my_rank==0){
                printf("\n===========  A*B  ============\n");
                printf("%3d\n", sum);
            }
        }


/************************************************************/
        if(my_rank==0){
            option=0x41;
            while(option!=0x30 || option!=0x31){
                printf(RED"\n---- Program ends here ----\n"RESET);
                printf(CYAN" Press: 0 to terminate\n Press: 1 to repeat \n Input: "RESET);
                fflush(stdout);
                scanf(" %c",&option);
                printf("\n");
                if(option=='0')  return 0;
                else if(option=='1') break;
            }
        }
    }
    MPI_Finalize();
}
