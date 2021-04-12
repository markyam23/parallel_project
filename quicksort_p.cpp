#include <iostream>
#include <mpi.h>
#include <unistd.h>
#include <stdlib.h>
#include <vector>
#include <cmath>

#define MCW MPI_COMM_WORLD

using namespace std;

void cube(int *data, int m){
    int rank, size;
    int dest;
    unsigned int mask=1;
    MPI_Comm_rank(MCW, &rank);
    MPI_Comm_size(MCW, &size);

    dest = rank^(mask<<m);

    MPI_Send(data,1,MPI_INT,dest,0,MCW);
    MPI_Recv(data,1,MPI_INT,MPI_ANY_SOURCE,0,MCW,MPI_STATUS_IGNORE);

    return;
}

void printArray(int arr[], int size, int start){
    for (int i = start; i < size; i++){
        cout<<arr[i]<<" ";
    }
}

void swap(int* swap1, int* swap2){
    int temp = *swap1;
    *swap1 = *swap2;
    *swap2 = temp;
}
 
int partition (int arr[], int low, int high){
    int index = low - 1;
    int pivot = arr[high];
 
    for (int j = low; j <= high - 1; j++){ // perform partitioning
        if (arr[j] <= pivot){
            swap(&arr[++index], &arr[j]); //swap locations if value is less than pivot
        }
    }
    swap(&arr[high], &arr[++index]);
    return index; // return the mid index for sorting
}
 
void quickSort(int arr[], int low, int high){
    if (low > high){
        return; //base case
    }
    int mid = partition(arr, low, high); // get the middle value result from partition

    quickSort(arr, low, mid - 1); //sort bottom half recursively
    quickSort(arr, mid + 1, high); //sort top half recursively
}

int main(int argc, char **argv){

    int numbers[] = {7,4,2,8,1,6,3,5,9,10,14,12,11,13};

    int rank, size;
    int data;
    int pivot;
    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MCW, &rank);
    MPI_Comm_size(MCW, &size);
    
    int length = sizeof(numbers)/sizeof(numbers[0]);
    
    // perform the initial algorithm log2(size) times
    for (int k = 0; k < log2(size); ++k) {
        // divide the list among each process
        int partitionSize = (length/size)/(k+1);
        int start = partitionSize * rank;
        int end = partitionSize * (rank+1) - 1;

        //set pivot
        if (rank > size/2){
            pivot = length/2 + k*(length/4);
        } else{
            pivot = length/2 - k*(length/4);
        }
        
        
        // each process divides its list into 2 (smaller than pivot and greater than pivot)
        int bigArr[partitionSize];
        int smallArr[partitionSize];
        for (int i = 0; i < partitionSize; ++i) {
            smallArr[i] = 0;
            bigArr[i] = 0;
            if(numbers[i+start] <= pivot){
                smallArr[i] = numbers[i+start];
            } else{
                bigArr[i] = numbers[i+start];
            }
        }
        
        //decide who to send to using hypercube
        data = rank;
        cube(&data,k);
        MPI_Barrier(MCW);
        // lower process sends its high list to partner high list and vice versa
        if (rank < data){
            MPI_Send(bigArr,partitionSize,MPI_INT,data,0,MCW);
            MPI_Recv(bigArr,partitionSize,MPI_INT,MPI_ANY_SOURCE,0,MCW,MPI_STATUS_IGNORE);
        } else{
            MPI_Send(smallArr,partitionSize,MPI_INT,data,0,MCW);
            MPI_Recv(smallArr,partitionSize,MPI_INT,MPI_ANY_SOURCE,0,MCW,MPI_STATUS_IGNORE);
            cout<<endl;
        }
        
        // set new numbers list
        int m = sizeof(bigArr)/sizeof(bigArr[0]);
        int n = sizeof(smallArr)/sizeof(smallArr[0]);

        int numbers[m + n];
        copy(bigArr, bigArr + m, numbers);
        copy(smallArr, smallArr + n, numbers + m);
        
        if(k == log2(size)-1){
            MPI_Send(numbers,partitionSize*2,MPI_INT,0,0,MCW); //send results to process 0
        }
    }
    //gather at rank 0 and sequential quicksort on in order divided lists
    if(rank == 0){
        for (int i = 0; i < size; ++i) {
            MPI_Recv(numbers,length,MPI_INT,MPI_ANY_SOURCE,0,MCW,MPI_STATUS_IGNORE);
            quickSort(numbers, 0, length-1);
            printArray(numbers,length, length/2); // offset to account for padding 0s
        }
    }

    MPI_Finalize();

    return 0;
}
