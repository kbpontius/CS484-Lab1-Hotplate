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
#include <sys/time.h>

#define MAX_ARRAY_SIZE 8192

float** currentArray;
float** previousArray;

/*
    Organization:
    ARRAY[ROW][COLUMN]
*/

double When()
{
    struct timeval tp;
    gettimeofday(&tp, NULL);
    return ((double) tp.tv_sec + (double) tp.tv_usec * 1e-6);
}

void swapArrays() {
    float** tempArray = currentArray;
    currentArray = previousArray;
    previousArray = tempArray;
}

int isFixedCell(int i, int j) {
    if (i == 400 && (j >= 0 && j <= 330)) {
        return 1;
    }
    
    if (i == 200 && j == 500) {
        return 1;
    }
        
    return 0;
}

float getDifference(int i, int j) {
    float middle = currentArray[i][j];
    
    float up = currentArray[i - 1][j];
    float down = currentArray[i + 1][j];
    float left = currentArray[i][j - 1];
    float right = currentArray[i][j + 1];
    
    return fabs(middle - ((down + up + right + left) / 4));
}

float calculateNewCell(int i, int j) {
    float middle = previousArray[i][j];
    
    float up = previousArray[i - 1][j];
    float down = previousArray[i + 1][j];
    float left = previousArray[i][j - 1];
    float right = previousArray[i][j + 1];
    
    return (down + up + right + left + (middle * 4)) / 8;
}

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
    #pragma omp parallel for schedule(auto)
    for (int i = 0; i < MAX_ARRAY_SIZE; i++) {
        for (int j = 0; j < MAX_ARRAY_SIZE; j++) {
            currentArray[i][j] = 50;                // All other cells
            previousArray[i][j] = 50;
        }
        
        currentArray[0][i] = 0;                     // Top row
        previousArray[0][i] = 0;
        
        currentArray[i][0] = 0;                     // Left side
        previousArray[i][0] = 0;
        
        currentArray[i][MAX_ARRAY_SIZE - 1] = 0;    // Right side
        previousArray[i][MAX_ARRAY_SIZE - 1] = 0;
        
        currentArray[MAX_ARRAY_SIZE - 1][i] = 100;  // Bottom row
        previousArray[MAX_ARRAY_SIZE - 1][i] = 100;
    }
    
    for (int i = 0; i <= 330; i++) {
        currentArray[400][i] = 100;
        previousArray[400][i] = 100;
    }
    
    currentArray[200][500] = 100;
    previousArray[200][500] = 100;
}

void allocateArray(float*** array) {
    *array = (float **) malloc(MAX_ARRAY_SIZE * (sizeof(float *)));
    
    #pragma omp parallel for schedule(auto)
    for (int i = 0; i < MAX_ARRAY_SIZE; i++) {
        (*array)[i] = (float *) malloc(MAX_ARRAY_SIZE * (sizeof(float)));
    }
}

int arrayIsFinished() {
    float difference = 0;
    float isFinished = 1;
    
    #pragma omp parallel for schedule(auto)
    // Avoid iterating over borders.
    for (int i = 1; i < MAX_ARRAY_SIZE - 1; i++) {
        if (isFinished == 1) {
            for (int j = 1; j < MAX_ARRAY_SIZE - 1; j++) {
                // Avoid static cells, save computations.
                if (isFixedCell(i, j) == 0) {
                    difference = getDifference(i, j);
                    
                    if (difference >= 0.1) {
//                        printf("\nDifference: %f", difference);
//                        printf("\nRow: %d || Col: %d", i, j);
                        isFinished = 0;
                        break;
                    }
                }
            }
        }
    }
    
    return isFinished;
}

int calculateSteadyState() {
    int isFinished = 0;
    int iterations = 0;
    
    while (isFinished == 0) {
        iterations++;
        swapArrays();
        
        #pragma omp parallel for schedule(auto)
        // Avoid iterating over borders.
        for (int i = 1; i < MAX_ARRAY_SIZE - 1; i++) {
            #pragma omp parallel for
            for (int j = 1; j < MAX_ARRAY_SIZE - 1; j++) {
                // Avoid static cells.
                if (isFixedCell(i, j) == 0) {
                    currentArray[i][j] = calculateNewCell(i, j);
                }
            }
        }
        
        isFinished = arrayIsFinished();
    }
    
    return iterations;
}

int countCellsOverThreshold() {
    int count = 0;
    
    // Avoid iterating over borders.
    for (int i = 1; i < MAX_ARRAY_SIZE - 1; i++) {
        for (int j = 1; j < MAX_ARRAY_SIZE - 1; j++) {
            if (currentArray[i][j] > 50) {
                count++;
            }
        }
    }
    
    return count;
}

int main(int argc, const char * argv[]) {
    double startTime = When();
    allocateArray(&currentArray);
    allocateArray(&previousArray);
    setupArrays();
    
    int iterations = calculateSteadyState();
    int result = countCellsOverThreshold();
    
    double endTime = When();
    
    printf("Iterations: %d", iterations);
    printf("\nCells 50+: %d\n", result);
    printf("\nTotal Execution Time: %f\n", endTime - startTime);
//    writeCSV(currentArray);
}
