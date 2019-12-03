//edit lines 40-51 to enter production mode
#include <stdio.h>
#include "mpi.h"

void main(int argc, char** argv){
    int my_rank;
    int k,res = 0,num_of_elements, p, n;
    int source,target, count;
    int tag1=1, tag2=2;
    int min_l,max_l,var,j;
    float average;
    int arrayA[100];
    float D[100];

    int rem,div;
    int elem_arr[50];

    char option;

    MPI_Status status;
    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &my_rank);
    MPI_Comm_size(MPI_COMM_WORLD, &p);


/*********************************************************************
****************************** Erwthma 1 *****************************
**********************************************************************/
    option = '1';
    count = 0;


    while (option == '1') {

        if(my_rank == 0){

            if (count++){
                for (j=0; j<53; j++)
                    printf("=");
                //printf("\n\t\t * No. of runs: %d *\n",count);
            }


            //printf("\n Give amount of numbers (up to 100): ");
            //scanf("%d", &n);
            n=50; //dev
            //printf(" Insert %d numbers: ", n);

            for (j=0; j<n; j++)
                arrayA[j] = j+1; //dev
                //scanf("%d", &arrayA[j]);


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
                k += elem_arr[target];
            }

        }
        else{
            MPI_Recv(elem_arr, 1, MPI_INT, 0, tag1, MPI_COMM_WORLD, &status);
            MPI_Recv(arrayA, *elem_arr, MPI_INT, 0, tag2, MPI_COMM_WORLD, &status);

        }

        if (my_rank == 0)
            printf("\n\t** Erwthma 1 **\n");

        for(k=0; k<*elem_arr; k++){
            res += arrayA[k];
        }

        if (my_rank != 0)
            MPI_Send(&res, 1, MPI_INT, 0, tag1, MPI_COMM_WORLD);
        else{
            printf(" proccess %d result: %d\n", my_rank, res);

            for (source = 1, k = res; source < p; source++){
                MPI_Recv(&res, 1, MPI_INT, source, tag1, MPI_COMM_WORLD, &status);
                printf(" proccess %d result: %d\n", source, res);
                k += res;
            }
            res = k;
            printf("\tSum: %d \n",res);
            average = res / n;
            printf("\tAverage: %d/%d = %.1f\n\n",res,n, (float)res / n);
        }

    /*********************************************************************
    ****************************** Erwthma 2 *****************************
    **********************************************************************/

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

    /*********************************************************************
    ****************************** Erwthma 3 *****************************
    **********************************************************************/


        if (my_rank == 0){
            max_l = arrayA[0];
            min_l = arrayA[0];

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
                printf("d[%d]: %3d -> %4.2f%%\n", k,arrayA[k], D[k]);
        }

        if (my_rank == 0) {
            printf("\n\n\t\t1. Continue\n\t\t2. Kill\n\t\tPlease enter your option: ");
            scanf(" %c", &option);
            printf("\n\n");
            for (target = 1; target < p; target++)
                MPI_Send(&option, 1, MPI_INT, target, tag1, MPI_COMM_WORLD);
        }
        else
            MPI_Recv(&option, 1, MPI_INT, 0, tag1, MPI_COMM_WORLD, &status);
    }

    MPI_Finalize();
}
