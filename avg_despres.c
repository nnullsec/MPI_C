#include <stdio.h>
#include "mpi.h"

#define n 20 

int main(int argc, char** argv){
    int my_rank;
    int k,res = 0,num_of_elements, p;
    int source,target;
    int tag1=1, tag2=2, tag3=3;
    int min_l,max_l,var,j;
    float average;
    int arrayA[100];
    float D[100];

    int rem,div;
    int elem_arr[50];

    char option='1';

    MPI_Status status;
    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &my_rank);
    MPI_Comm_size(MPI_COMM_WORLD, &p);



    for(;;){
        if(my_rank == 0){
            printf("\n---- Program starts here ----\n");
            printf(" 1. Average\n 2. Despersion \n 3. Custom D vector\n Intput: ");
            fflush(stdout);
            scanf(" %c",&option);

            for (j=0; j<n; j++)
                arrayA[j] = j+1;


            rem = n % p;
            div = n / p;
            for (j=0; j<p; j++){
                if (j<rem)
                    elem_arr[j] = div + 1;
                else
                    elem_arr[j] = div;
            }

            for (target = 1, k = *elem_arr; target < p; target++){
                MPI_Send(&elem_arr[target], 1, MPI_INT, target, tag1, MPI_COMM_WORLD);
                MPI_Send(&arrayA[k], elem_arr[target], MPI_INT, target, tag2, MPI_COMM_WORLD);
                MPI_Send(&option, 1, MPI_CHAR, target, tag3, MPI_COMM_WORLD);
                k += elem_arr[target];
            }

        }
        else{
            MPI_Recv(elem_arr, 1, MPI_INT, 0, tag1, MPI_COMM_WORLD, &status);
            MPI_Recv(arrayA, *elem_arr, MPI_INT, 0, tag2, MPI_COMM_WORLD, &status);
            MPI_Recv(&option, 1, MPI_CHAR, 0, tag3, MPI_COMM_WORLD, &status);
        }



        /*********************************************************************
        ****************************** Erwthma 1 *****************************
        **********************************************************************/

        if(option=='1' || option=='2'){

            if (my_rank == 0 && option=='1')
                printf("\n\n\t** Erwthma 1 **\n");

            res=0;
            for(k=0; k<*elem_arr; k++)
                res += arrayA[k];

            if (my_rank != 0)
                MPI_Send(&res, 1, MPI_INT, 0, tag1, MPI_COMM_WORLD);
            else{
                if(option=='1')
                    printf(" proccess %d result1: %d\n", my_rank, res);

                for (source = 1, k = res; source < p; source++){
                    MPI_Recv(&res, 1, MPI_INT, source, tag1, MPI_COMM_WORLD, &status);

                    if(option=='1')
                        printf(" proccess %d result: %d\n", source, res);

                    k += res;
                }

                res = k;

                if(option=='1')
                    printf("\tSum: %d \n",res);

                average = (float)res / n;

                if(option=='1')
                    printf("\tAverage: %d/%d = %.1f\n\n",res,n, (float)res / n);
            }
        }

        /*********************************************************************
        ****************************** Erwthma 2 *****************************
        **********************************************************************/

        if(option=='2'){

            if (my_rank == 0){
                for(target = 1; target < p; target++)
                    MPI_Send(&average, 1, MPI_INT, target, tag1, MPI_COMM_WORLD);
            }
            else
                MPI_Recv(&average,1,MPI_INT,0,tag1,MPI_COMM_WORLD, &status);

            for (res=0, k=0; k<*elem_arr; k++)
                res += ((arrayA[k]-average)*(arrayA[k]-average));

            if (my_rank == 0)
                printf("\n\t** Erwthma 2 **\n");

            if (my_rank != 0)
                MPI_Send(&res, 1, MPI_INT, 0, tag1, MPI_COMM_WORLD);
            else{
                printf(" proccess %d result: %d\n", my_rank, res);

                k = res;
                for (source = 1; source < p; source++){
                   MPI_Recv(&res, 1, MPI_INT, source, tag1, MPI_COMM_WORLD, &status);
                   printf(" proccess %d result: %d\n", source, res);
                   k += res;
                }
                res = k;

                printf("\tdespresion: %d/%d = %.1f\n\n",res,n, (float)res / n);
            }
        }

        /*********************************************************************
        ****************************** Erwthma 3 *****************************
        **********************************************************************/

        if(option=='3'){

            if (my_rank == 0){
                max_l = -9999;
                min_l = 9999;

                for (k=0; k<n; k++){
                    if (arrayA[k] > max_l)
                        max_l = arrayA[k];
                    if (arrayA[k] < min_l)
                        min_l = arrayA[k];
                }

                for(target = 1; target < p; target++){
                    MPI_Send(&min_l, 1, MPI_INT, target, tag1, MPI_COMM_WORLD);
                    MPI_Send(&max_l, 1, MPI_INT, target, tag2, MPI_COMM_WORLD);
                }
            }

            if (my_rank!=0){
                MPI_Recv(&min_l, 1, MPI_INT, 0, tag1, MPI_COMM_WORLD, &status);
                MPI_Recv(&max_l, 1, MPI_INT, 0, tag2, MPI_COMM_WORLD, &status);

                for (k=0; k<*elem_arr; k++)
                    D[k] = ((arrayA[k] - min_l) / (float)(max_l - min_l)) * 100;

                MPI_Send(D, *elem_arr, MPI_FLOAT, 0, tag1, MPI_COMM_WORLD);
            }

            if (my_rank == 0)
                printf("\n\t** Erwthma 3 **\n");

            if (my_rank == 0){
                for (k=0; k<*elem_arr; k++)
                    D[k] = ((arrayA[k] - min_l) / (float)(max_l - min_l)) * 100;

                for (source = 1, k=elem_arr[0]; source < p; source++){
                    MPI_Recv(D+k, elem_arr[source], MPI_FLOAT, source, tag1, MPI_COMM_WORLD, &status);
                    k+=elem_arr[source];
                }

                for (k=0; k<n; k++)
                    printf("d[%d]:%6d -> %5.2f%%\n", k,arrayA[k], D[k]);
            }
        }


        /**************************************************************************/
        if(my_rank==0){
            while(1){
                printf("\n---- Program ends here ----\n");
                printf(" Press: 0 to terminate\n Press: 1 to repeat \n Input: ");
                fflush(stdout);
                scanf(" %c",&option);
                printf("\n\n");
                if(option=='0')  return 0;
                else if(option=='1') break;
            }
        }
    }

    MPI_Finalize();
}
