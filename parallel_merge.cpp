#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <mpi.h>
#define MCW MPI_COMM_WORLD

using namespace std;

void printArray(int *arr, int size){
	for (int i = 0; i < size; i++) {
		cout << arr[i] << " ";
    }
    cout<<endl;
}


void merge(int *arr, int left, int merge, int right){
    //calculate the sizes for the left and right sub arrays
	int leftSub = merge - left + 1;
	int rightSub = right - merge;
    //initialize sub arrays
	int leftArr[leftSub], rightArr[rightSub];
	for (int i = 0; i < leftSub; i++)
		leftArr[i] = arr[left + i];
	for (int j = 0; j < rightSub; j++)
		rightArr[j] = arr[merge + 1 + j];
	// create an index for the left subarray
	int leftIdx = 0;
	// create an index for the right subarray
	int rightIdx = 0;
	// create index for the array that will get the merges
	int mergeIdx = left;

	while (leftIdx < leftSub && rightIdx < rightSub) {
		if (leftArr[leftIdx] <= rightArr[rightIdx]) {
			arr[mergeIdx] = leftArr[leftIdx];
			leftIdx++;
		}
		else {
			arr[mergeIdx] = rightArr[rightIdx];
			rightIdx++;
		}
		mergeIdx++;
	}

	//Copies the rest of items in the left array
	while (leftIdx < leftSub) {
		arr[mergeIdx] = leftArr[leftIdx];
		leftIdx++;
		mergeIdx++;
	}
	//Copies the rest of items in the right array
	while (rightIdx < rightSub) {
		arr[mergeIdx] = rightArr[rightIdx];
		rightIdx++;
		mergeIdx++;
	}
}
void mergeSort(int *arr,int leftIdx,int rightIdx){
	if(leftIdx >= rightIdx){
		return;
	}
	int m = leftIdx + (rightIdx - leftIdx)/2;
	mergeSort(arr,leftIdx,m);
	mergeSort(arr, m+1,rightIdx);
	merge(arr,leftIdx, m, rightIdx);
}

int randNum(int min, int max){
    int num = min + (rand() % (max - min + 1));
    return num;
}
int main(int argc, char **argv){
    auto start = chrono::high_resolution_clock::now();

    int rank, size;
    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MCW, &rank); 
    MPI_Comm_size(MCW, &size); 
    srand(time(NULL));

    int arrSize = 2000000;
    //allocate memory and fill the initial array
	int *startArr = (int*)malloc(arrSize * sizeof(int)); 
    for(int i = 0; i <arrSize; i++){
        startArr[i] = randNum(0, arrSize);
    }
    if(rank == 0){
    cout<<"Unsorted Array:"<<endl;
    // printArray(startArr, arrSize);
    }
    int divSize = arrSize/size;
    int *subArr = (int*)malloc(divSize * sizeof(int));
    //Sends sub arrays to each process
	MPI_Scatter(startArr, divSize, MPI_INT, subArr, divSize, MPI_INT, 0, MCW);
    mergeSort(subArr, 0, (divSize-1));    
    int *sortedArr = NULL;
    //we only want rank 0 to actually have the memory allocated for the sorted array
    if(rank == 0){
        sortedArr = (int*)malloc(arrSize * sizeof(int));
    }
    MPI_Gather(subArr, divSize, MPI_INT, sortedArr, divSize, MPI_INT, 0, MCW);
    //gotta call the merge sort one last time on the entire list to make sure sub lists get put into correct places
    if(rank==0){
        mergeSort(sortedArr, 0, (arrSize-1));
        cout<< "Sorted Array:" << endl;
        printArray(sortedArr, arrSize);
        free(sortedArr);
    }
    free(subArr);
    free(startArr);
    MPI_Finalize();
    auto stop = chrono::high_resolution_clock::now(); 
        if(!rank){
            auto duration = chrono::duration_cast<std::chrono::seconds>(stop - start);    
            cout<< "Seconds to run: "<< duration.count()<< endl; 
        }
    return 0;
}

