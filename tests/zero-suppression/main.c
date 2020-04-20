#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#define LINE_LENGTH 10000
#define WF_LENGTH 1019

void importWaveform(char * line, int * wf);
void printWaveform(int * wf, int len);
float stddev(int data[], int len);
float mean(int data[], int len);
void printWindow(int * data, int len);
int signalWindow(int * input, int inputLength, int * output, int outputMinLength,
                 int lag, float threshold, float influence);


int main() {
    FILE *fp = fopen("../liverpoolPulses.txt", "r");
    if(fp == NULL) {
        perror("Unable to open file!");
        exit(1);
    }

    char line[LINE_LENGTH];
    int wf[WF_LENGTH];

    fgets(line, sizeof(line), fp);
    importWaveform(line, wf);
    printWaveform(wf, 20);

    int output[WF_LENGTH];
    int len = WF_LENGTH;
    int outputLen = signalWindow(wf, len, output, 20, 100, 5., 0.01);

    printWindow(output, outputLen);
    return 0;
}



float mean(int data[], int len) {
    float sum = 0.0;

    int i;
    for(i=0; i<len; ++i)
        sum += data[i];

    return sum/len;
}

float stddev(int data[], int len) {
    float the_mean = mean(data, len);
    float standardDeviation = 0.0;

    int i;
    for(i=0; i<len; ++i) {
        standardDeviation += pow(data[i] - the_mean, 2);
    }

    return sqrt(standardDeviation/len);
}

void importWaveform(char * line, int * wf){
    char * pch;
    pch = strtok(line, " ");

    int i = 0;
    while ((pch != NULL) || (i == WF_LENGTH - 1)){
        sscanf(pch, "%d", &wf[i]);
        i ++;
        pch = strtok(NULL, " ");
    }
}

void printWaveform(int * wf, int len){
    for (int i = 0; i < len; ++i) {
        printf("%d ", wf[i]);
    }
    printf("\n");
}

void printWindow(int *data, int len) {
    for (int i = 0; i < len; ++i) {
        if (data[i] != 0){
            printf("%d: %d\n", i, data[i]);
        }
    }
}

int signalWindow(int *input, int inputLength, int *output, int outputMinLength,
        int lag, float threshold, float influence) {

//    int tempOutput[WF_LENGTH];
    memset(output, 0, sizeof(int) * inputLength);
    int filteredData[inputLength];
    memcpy(filteredData, input, sizeof(int) * inputLength);

    float avgFilter[inputLength];
    float stdFilter[inputLength];

    avgFilter[lag - 1] = mean(input, lag);
    stdFilter[lag - 1] = stddev(input, lag);

    int totStart = -1; // start of the time over threshold
    int outputLength = 0;

    for (int i = lag; i < inputLength; i++) {
        if (fabsf(input[i] - avgFilter[i - 1]) > threshold * stdFilter[i - 1]) {
            outputLength ++;
            if (totStart == -1)
                totStart = i; // mark the first sample over threshold

            if (i - totStart == outputLength) { // check for consecutive samples
            }
            else { // if not, reset counters
                totStart = i;
                outputLength = 0;
            }

//            if (input[i] > avgFilter[i - 1]) {
//                tempOutput[i] = 1;
//            } else {
//                tempOutput[i] = -1;
//            }
            filteredData[i] = influence * input[i] + (1 - influence) * filteredData[i - 1];
        } else {
//            tempOutput[i] = 0;
        }

        avgFilter[i] = mean(filteredData + i - lag, lag);
        stdFilter[i] = stddev(filteredData + i - lag, lag);
    }

    if (outputLength >= outputMinLength){ // only export if longer than minimum length, add lag window as presamples
        outputLength += lag;
        memcpy(output, input + totStart - lag, sizeof(int) * outputLength);
    }
    else {
        outputLength = 0;
    }

    return outputLength;
}
