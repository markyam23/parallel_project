#include <iostream>
#include<time.h>

using namespace std;

void merge(int *arr, int left, int merge, int right)
{
	int leftSub = merge - left + 1;
	int rightSub = right - merge;
	int leftArr[leftSub], rightArr[rightSub];
	for (int i = 0; i < leftSub; i++)
		leftArr[i] = arr[left + i];
	for (int j = 0; j < rightSub; j++)
		rightArr[j] = arr[merge + 1 + j];
	// Initial index of first subarray
	int leftIdx = 0;
	// Initial index of second subarray
	int rightIdx = 0;
	// Initial index of merged subarray
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
	if(leftIdx>=rightIdx){
		return;
	}
	int m = leftIdx+ (rightIdx-leftIdx)/2;
	mergeSort(arr,leftIdx,m);
	mergeSort(arr,m+1,rightIdx);
	merge(arr,leftIdx,m,rightIdx);
}

void printArray(int *arr, int size){
	for (int i = 0; i < size; i++)
		cout << arr[i] << " ";
}
int randNum(int min, int max){
    int num = min + (rand() % (max - min + 1));
    return num;
}

int main(){
    auto start = chrono::high_resolution_clock::now();
    srand(time(NULL));
    int arrSize = 2000000;
	int *arr = (int*)malloc(arrSize * sizeof(int));
    for(int i = 0; i <arrSize; i++){
        arr[i]=randNum(0, arrSize);
    }

	cout << "Initial Array: " << endl;
	printArray(arr, arrSize);

	mergeSort(arr, 0, arrSize - 1);

	cout << endl << "Sorted array: "<< endl;
	printArray(arr, arrSize);
    auto stop = chrono::high_resolution_clock::now(); 
    auto duration = chrono::duration_cast<std::chrono::seconds>(stop - start);    
    cout << endl << "Seconds to run: "<< duration.count()<< endl; 
	return 0;
}