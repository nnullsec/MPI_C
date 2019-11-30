#include <stdio.h>
#include "mpi.h"

void main(int argc, char** argv){
    int my_rank;
    int k,res,finres,num_of_elements,p,n;
    int source,target;
    int tag1=1, tag2=2;
    int min_l,max_l,var,j;
    float average;
    int arrayA[100];
    float D[100];

    char option;
    unsigned short int counter = 0;


    MPI_Status status;
    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &my_rank);
    MPI_Comm_size(MPI_COMM_WORLD, &p);


/*********************************************************************
******************************           *****************************
**********************************************************************/


    if(my_rank == 0){
        printf("\nGive the amount of numbers (up to 100): ");
        scanf("%d", &n);
        //n=8; //debug
        printf("Give %d numbers: ", n);

        for (j=0; j<n; j++)
            scanf("%d", &arrayA[j]);

        for (target = 1; target < p; target++){
            MPI_Send(&n, 1, MPI_INT, target, tag1, MPI_COMM_WORLD);

            num_of_elements = n/p;
            k = num_of_elements;
        }
        for (target = 1; target < p; target++){
            MPI_Send(&arrayA[k], num_of_elements, MPI_INT, target, tag2, MPI_COMM_WORLD);
            k += num_of_elements;
        }
    }
    else{
        MPI_Recv(&n, 1, MPI_INT, 0, tag1, MPI_COMM_WORLD, &status);
        num_of_elements = n/p;
        MPI_Recv(&arrayA[0], num_of_elements, MPI_INT, 0, tag2, MPI_COMM_WORLD, &status);
    }

    for(k=0; k<num_of_elements; k++)
        res = res + (arrayA[k]);

    if (my_rank == 0)
        printf("\n\t** Erwthma 1 **\n");

    if (my_rank != 0)
        MPI_Send(&res, 1, MPI_INT, 0, tag1, MPI_COMM_WORLD);
    else{
        finres = res;
        printf(" proccess %d result: %d\n", my_rank, res);

        for (source = 1; source < p; source++){
            MPI_Recv(&res, 1, MPI_INT, source, tag1, MPI_COMM_WORLD, &status);
            finres = finres + res;
            printf(" proccess %d result: %d\n", source, res);
       }

        printf("\tSum: %d \n",finres);
        average = finres / n;
        printf("\tAverage: %d/%d = %.1f\n\n",finres,n, (float)finres / n);
    }

/*********************************************************************
******************************           *****************************
**********************************************************************/

    if (my_rank==0){
        for(target = 1; target < p; target++)
            MPI_Send(&average, 1, MPI_INT, target, tag1, MPI_COMM_WORLD);
    }
    else
        MPI_Recv(&average,1,MPI_INT,0,tag1,MPI_COMM_WORLD, &status);

    for (k=0; k<num_of_elements; k++)
        res = res + ((arrayA[k]-average)*(arrayA[k]-average));

    if (my_rank == 0)
        printf("\n\t** Erwthma 2 **\n");

    if (my_rank != 0)
        MPI_Send(&res, 1, MPI_INT, 0, tag1, MPI_COMM_WORLD);
    else{
        finres = res;
        printf(" proccess %d result: %d\n", my_rank, res);

        for (source = 1; source < p; source++){
           MPI_Recv(&res, 1, MPI_INT, source, tag1, MPI_COMM_WORLD, &status);
           finres = finres + res;
           printf(" proccess %d result: %d\n", source, res);
        }

        printf("\tdespresion: %d/%d = %.1f\n\n",finres,n, (float)finres / n);
    }

/*********************************************************************
******************************           *****************************
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

        for (k=0; k<num_of_elements; k++){
            D[k] = ((arrayA[k] - min_l) / (float)(max_l - min_l)) * 100;
        }
        MPI_Send(D, num_of_elements, MPI_FLOAT, 0, tag1, MPI_COMM_WORLD);
    }
    if (my_rank == 0)
        printf("\n\t** Erwthma 3 **\n");

    if (my_rank == 0){
        for (k=0; k<num_of_elements; k++)
            D[k] = ((arrayA[k] - min_l) / (float)(max_l - min_l)) * 100;
        for (source = 1; source < p; source++)
            MPI_Recv(D+source*num_of_elements, num_of_elements, MPI_FLOAT, source, tag1, MPI_COMM_WORLD, &status);
        for (k=0; k<n; k++)
            printf("d[%d]: %3.1f%%\n", k, D[k]);
    }

    MPI_Finalize();
}
