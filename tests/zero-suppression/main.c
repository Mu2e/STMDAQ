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
// find rising edge function:
// - inputs:
//    - waveform: unsigned 16-bit integer
//    - length of waveform: unsigned 16-bit integer
//    - windowLen: moving window width, should be a power of 2 to make division
//    easier
//    - threshold: float, multiple of standard deviation 
// - output: unsigned 16-bit integer, zero-suppressed waveform
// - return: length of the output
uint16_t findRisingEdge(uint16_t * wf, uint16_t len,
      uint16_t windowLen, float threshold, uint16_t * output);

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
    int outputLen = findRisingEdge(wf, WF_LENGTH, 128, 4.0, output);
    printf("outputLen %d\n", outputLen);

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

uint16_t findRisingEdge(uint16_t * wf, uint16_t len,
      uint16_t windowLen, float threshold, uint16_t * output) {

   // have to be signed numbers so that the arithmetic below works correctly
   int64_t ped = 0;
   int64_t var = 0;

   // calculate first ped and var
   for (uint16_t i = 0; i < windowLen; i++)
      ped += wf[i];
   ped /= windowLen;

   for (uint16_t i = 0; i < windowLen; i++)
      var += (wf[i] - ped) * (wf[i] - ped);
   var /= windowLen;
   printf("ped %llu, var %llu\n", ped, var);

   for (uint16_t i = windowLen; i < len; i++) {
      int64_t oldped = ped;
      int height = wf[i] - oldped;

      // only find positive edge
      if ((height * height > var * threshold * threshold) && (height >= 0)){
         // check next 2 samples for positive gradient
         if (i+2 >= len) { // just return 0 if at the end of data
            return 0;
         }
         else {
            if ((wf[i+2] > wf[i+1]) && (wf[i+1] > wf[i])) { // this is it!
               uint16_t firstSample, lastSample;
               if (i - PRE < 0)
                  firstSample = 0;
               else
                  firstSample = i - PRE;

               if (i + POST >= len)
                  lastSample = len;
               else
                  lastSample = i + POST;

               memcpy(output, wf + firstSample, (lastSample - firstSample) * sizeof(uint16_t));
               return (lastSample - firstSample);
            }
         }
      }

      ped += (wf[i] - wf[i - windowLen]) / windowLen;
      var += (wf[i] - wf[i - windowLen]) * (wf[i] + wf[i - windowLen] - ped - oldped) / windowLen;
      /* printf("sample %d: y %d, ped %lld, var %lld\n", i, wf[i], ped, var); */
   }
   return 0;
}
