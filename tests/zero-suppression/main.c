#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <stdint.h>

#define LINE_LENGTH 10000
#define WF_LENGTH 1019
#define PRE 256
#define POST 256

void importWaveform(char * line,  uint16_t * wf);
void printWaveform(uint16_t * wf, uint16_t len);
void printWindow(uint16_t * wf, uint16_t len);
int findRisingEdge(uint16_t * wf, uint16_t len, uint16_t downSampling,
      uint16_t lag, float threshold, float influence,
      uint16_t * output) {

   uint64_t mean = 0;
   int height = 0;
   uint64_t variance = 0;

   // calculate first mean and variance
   for (uint16_t i = 0; i < lag * downSampling; i+= downSampling)
      mean += wf[i];
   mean /= lag;

   for (uint16_t i = 0; i < lag * downSampling; i+= downSampling)
      variance += (wf[i] - mean) * (wf[i] - mean);
   variance /= lag;

   int foundEdge = 0;
   uint16_t edgeStart = 0;
   uint16_t nSamplesInEdge = 0;

   for (uint16_t i = lag * downSampling; i < len; i+= downSampling) {
      uint64_t oldMean = mean;
      height = wf[i] - oldMean;

      if ((height * height > variance * threshold * threshold) && (height >= 0)){
         if (foundEdge == 0) {
            foundEdge = 1;
            nSamplesInEdge = 1;
         }
         else {
            nSamplesInEdge++;
         }
      }

      if (nSamplesInEdge == 2) {
         uint16_t start, stop;
         if (i - downSampling - PRE < 0)
            start = 0;
         else
            start = i - downSampling - PRE;

         if (i - downSampling + POST >= len)
            stop = len;
         else
            stop = i - downSampling + POST;

         memcpy(output, wf + start, (stop - start) * sizeof(uint16_t));
         return (stop - start);

         /* return i - downSampling; */
      }
   }
   return 0;
}

int main() {
    FILE *fp = fopen("../liverpoolPulses.txt", "r");
    if(fp == NULL) {
        perror("Unable to open file!");
        exit(1);
    }

    char line[LINE_LENGTH];
    uint16_t wf[WF_LENGTH];

    fgets(line, sizeof(line), fp);
    importWaveform(line, wf);
    printWaveform(wf, 20);

    uint16_t output[WF_LENGTH];
    int outputLen = findRisingEdge(wf, WF_LENGTH, 4, 8, 4.0, 0., output);

    printWaveform(output, outputLen);

    return 0;
}

void importWaveform(char * line, uint16_t * wf){
    char * pch;
    pch = strtok(line, " ");

    int i = 0;
    while ((pch != NULL) || (i == WF_LENGTH - 1)){
        sscanf(pch, "%hu", &wf[i]);
        i ++;
        pch = strtok(NULL, " ");
    }
}

void printWaveform(uint16_t * wf, uint16_t len){
    for (uint16_t i = 0; i < len; ++i) {
        printf("%d ", wf[i]);
    }
    printf("\n");
}

void printWindow(uint16_t *data, uint16_t len) {
    for (uint16_t i = 0; i < len; ++i) {
        if (data[i] != 0){
            printf("%d: %d\n", i, data[i]);
        }
    }
}

