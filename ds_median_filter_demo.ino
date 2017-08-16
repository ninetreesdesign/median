/*
  median_filter.ino
  a 'running' short-interval median to remove "salt&pepper noise" outlier data
  finds the median of 3 or 5 integers or floats
  Uses explicit steps for sorting for simplicity and speed
  
  references
  http://pages.ripco.net/~jgamble/nw.html for sorting algorithm
  function pointers http://www.dummies.com/programming/cpp/how-to-pass-pointers-to-functions-in-c/
  earlier implementation started by David Cary
   
  2016-11-07: more typical data stream and two sizes for median calculation: David Smith
  2017-05-31: add two add'l "prev" values to operate the median5(), used sprintf() to streamline printing
              Implemented on the Teensyduino 3.x, which supports float printing. 
              For other arduinos, convert to a string first per below. 

              from: https://stackoverflow.com/questions/27651012/arduino-sprintf-float-not-formatting#27652012
              // convert it to a C-Style string and then use sprintf, eg:
              char str_temp[6];
              dtostrf(temp, 4, 2, str_temp);
              sprintf(temperature,"%s F", str_temp);

  14.85  15.84 11.88 14.85 --------------*
  15.84  11.88 16.83 15.84 ---------------*
  11.88  16.83 20.79 16.83 ----------------*
  16.83  20.79 13.86 16.83 ----------------*
  20.79  13.86 22.77 20.79 --------------------*
  13.86  22.77  8.91 13.86 -------------*

*/
#include <TextGraph.h>  // to provide an output graph  http://engineeringnotes.blogspot.com/2013/09/graphing-in-arduino-serial-monitor.html

// instances
TextGraph TG(1);  // create an instance of TextGraph named "TG"

// globals
double fval_new   = 0;
double fval_prev1 = 0;
double fval_prev2 = 0;
double fval_prev3 = 0;
double fval_prev4 = 0;
char str_buf[99];

//! setup ------------------------------------------------------------------------------
void setup() {
  Serial.begin(9600);
  while (!Serial && (millis() < 8000)) { } // continues if the monitor window is never opened

  //Serial.println("prev2 \tprev1 \tnew \t\tmedian:");     // print output header line  med3
  Serial.println("prev4 \t prev3 \tprev2 \tprev1 \tnew \t\tmedian:");     // print output header line  med5
}

//! main loop ------------------------------------------------------------------------------
void loop() {
  // generate random data with outliers to observe median filtering
  // drop val_prevN value and shift in latest value
  double fmedian = 0;
  fval_prev4 = fval_prev3;
  fval_prev3 = fval_prev2;
  fval_prev2 = fval_prev1;
  fval_prev1 = fval_new;
  fval_new = 1.9 * (10 + random(-2, 8) + (random(0, 7) == 0) * (random(0, 28))); // small variation plus occasional spikes

  fmedian = median5(fval_prev4, fval_prev3, fval_prev2, fval_prev1, fval_new);
  sprintf(str_buf, "%5.1f  %5.1f  %5.1f  %5.1f  %5.1f    %5.1f", fval_prev4, fval_prev3, fval_prev2, fval_prev1, fval_new, fmedian);
  Serial.print(str_buf);
  TG.barGraph(fmedian);       // dotGraph function, simplest form with line only

  delay(200);
}

//! --- calculate the median from 5 adjacent points
double median5( double a0, double a1, double a2, double a3, double a4 ) {
  /* Network for N=5, using Bose-Nelson Algorithm.
    SWAP(0, 1); SWAP(3, 4); SWAP(2, 4);
    SWAP(2, 3); SWAP(0, 3); SWAP(0, 2);
    SWAP(1, 4); SWAP(1, 3); SWAP(1, 2);
  */
  swap(&a0, &a1); // 0,1
  swap(&a3, &a4);
  swap(&a2, &a4);
  swap(&a2, &a3); // 2,3
  swap(&a0, &a3);
  swap(&a0, &a2);
  swap(&a1, &a4); // 1,4
  swap(&a1, &a3);
  swap(&a1, &a2);

  return a2; // median value
}
int median5( int a0, int a1, int a2, int a3, int a4 ) {
  /* Network for N=5, using Bose-Nelson Algorithm.
    SWAP(0, 1); SWAP(3, 4); SWAP(2, 4);
    SWAP(2, 3); SWAP(0, 3); SWAP(0, 2);
    SWAP(1, 4); SWAP(1, 3); SWAP(1, 2);
  */
  swap(&a0, &a1); // 0,1
  swap(&a3, &a4);
  swap(&a2, &a4);
  swap(&a2, &a3); // 2,3
  swap(&a0, &a3);
  swap(&a0, &a2);
  swap(&a1, &a4); // 1,4
  swap(&a1, &a3);
  swap(&a1, &a2);

  return a2; // median value
}

// calculate the median from 3 adjacent points
double median3( double a0, double a1, double a2 ) {
  /* SWAP(1, 2); SWAP(0, 2); SWAP(0, 1); */
  swap(&a1, &a2); // swaps values  if arg1 > arg2
  swap(&a0, &a2);
  swap(&a0, &a1);

  return a1; // median value
}
int median3( int a0, int a1, int a2 ) {
  /* SWAP(1, 2); SWAP(0, 2); SWAP(0, 1); */
  swap(&a1, &a2); // swaps values  if arg1 > arg2
  swap(&a0, &a2);
  swap(&a0, &a1);

  return a1; // median value
}

//! --- swaps values (float/double) of j and k if j > k
void swap(double *j,  double *k) {
  double x = *j;
  double y = *k;
  if (*j > *k) {
    *k = x;
    *j = y;
  }
}

//! --- swaps values (int) of j and k if j > k
void swap(int *j,  int *k) {
  double x = *j;
  double y = *k;
  if (*j > *k) {
    *k = x;
    *j = y;
  }
}


