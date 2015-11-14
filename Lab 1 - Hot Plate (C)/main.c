//
//  main.c
//  Lab 1 - Hot Plate (C)
//
//  Created by Kyle on 11/10/15.
//  Copyright © 2015 Kyle Pontius. All rights reserved.
//

#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#define MAX_ARRAY_SIZE 16384

float** arr1;
float** arr2;
float*** tempArray; // Holder for swapping arrays.

/*
    Organization:
    ARRAY[ROW][COLUMN]
*/

//double When()
//{
//    struct timeval tp;
//    gettimeofday(&tp, NULL);
//    return ((double) tp.tv_sec + (double) tp.tv_usec * 1e-6);
//}

void writeCSV(float** arr) {
    FILE *fp = fopen("hotplateOutput.csv", "w+");
    
    for (int i = 0; i < MAX_ARRAY_SIZE; i++) {
        fprintf(fp, "%f", arr[i][0]);
        
        for (int j = 1; j < MAX_ARRAY_SIZE; j++) {
            fprintf(fp, ", %f",arr[i][j]);
        }
        
        fprintf(fp, "\n");
    }
    
    fclose(fp);
}

void setupArrays() {
    for (int i = 0; i < MAX_ARRAY_SIZE; i++) {
        for (int j = 0; j < MAX_ARRAY_SIZE; j++) {
            arr1[i][j] = 50; // All other cells
        }
        
        arr1[0][i] = 0;                     // Top row
        arr2[0][i] = 0;
        arr1[i][0] = 0;                     // Left side
        arr2[0][i] = 0;
        arr1[i][MAX_ARRAY_SIZE - 1] = 0;    // Right side
        arr2[i][MAX_ARRAY_SIZE - 1] = 0;
        arr1[MAX_ARRAY_SIZE - 1][i] = 100;  // Bottom row
        arr2[MAX_ARRAY_SIZE - 1][i] = 100;
    }
    
    for (int i = 0; i <= 330; i++) {
        arr1[400][i] = 100;
        arr2[400][i] = 100;
    }
    
    arr1[200][500] = 100;
    arr2[200][500] = 100;
}

void allocateArray(float*** array) {
    *array = (float **) malloc(MAX_ARRAY_SIZE * (sizeof(float *)));
    
    for (int i = 0; i < MAX_ARRAY_SIZE; i++) {
        (*array)[i] = (float *) malloc(MAX_ARRAY_SIZE * (sizeof(float)));
    }
}

int arrayIsFinished() {
    int isFinished = 1;
    float difference = 0;
    
    // Avoid iterating over borders.
    for (int i = 1; i < MAX_ARRAY_SIZE - 1; i++) {
        for (int j = 1; j < MAX_ARRAY_SIZE - 1; j++) {
            // Avoid static cells, save computations.
            if (i == 400 && j <= 330) {
                continue;
            } else if (i == 200 && j == 500) {
                continue;
            }
            
            difference = fabsf(arr2[i][j] - ((arr2[i + 1][j] + arr2[i - 1][j] + arr2[i][j + 1] + arr2[i][j - 1]) / 4));
            
            if (difference > 0.1) {
//                printf("\nDifference: %f", difference);
//                printf("\nValue: %f", arr2[i][j]);
                isFinished = 0;
                break;
            }
        }
        
        if (isFinished == 0) {
//            printf("\nRow: %d", i);
            break;
        }
    }
    
//    printf("\nDifference: %f", difference);
    return isFinished;
}

double calculateSteadyState() {
    int isFinished = 0;
    int iterations = 0;
    
    while (isFinished == 0) {
        iterations++;
        
        // Avoid iterating over borders.
        for (int i = 1; i < MAX_ARRAY_SIZE - 1; i++) {
            for (int j = 1; j < MAX_ARRAY_SIZE - 1; j++) {
                // Avoid static cells.
                if (i == 400 && j <= 330) {
                    continue;
                } else if (i == 200 && j == 500) {
                    continue;
                }
                
                arr2[i][j] = (arr1[i + 1][j] + arr1[i - 1][j] + arr1[i][j + 1] + arr1[i][j - 1] + (4 * arr1[i][j])) / 8;
            }
        }
        
        isFinished = arrayIsFinished();
        
        tempArray = &arr1;
        arr1 = arr2;
        arr2 = *tempArray;
        
        printf("\nIterations: %d", iterations);
    }
    
    // TODO: FIX ME!
    return 0;
}

int main(int argc, const char * argv[]) {
    allocateArray(&arr1);
    allocateArray(&arr2);
    setupArrays();
    
    calculateSteadyState();
    
//    writeCSV(arr2);
}
