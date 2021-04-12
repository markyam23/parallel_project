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
