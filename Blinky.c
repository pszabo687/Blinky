/*---------------------------------------------------------------------------
 * Copyright (c) 2020 Arm Limited (or its affiliates). All rights reserved.
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Licensed under the Apache License, Version 2.0 (the License); you may
 * not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an AS IS BASIS, WITHOUT
 * WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 *      Name:    Blinky.c
 *      Purpose: RTX example program
 *
 *---------------------------------------------------------------------------*/

#include "main.h"
#include <stdio.h>
#include "cmsis_os2.h"
#include<limits.h>

static osThreadId_t tid_thrLED, tid_fibonacciCalcThread,tid_fibPrintThread; 
static osSemaphoreId_t sid_fibReadable, sid_fibWriteable;        

static int fib_vect[2] = {0,1};

// Calculator can modify the data when the reader (printer) is done
__NO_RETURN static void thrFibCalc( void *arg ) {
    for(;;)
    {
        int new_value = fib_vect[0]+fib_vect[1];
        if( new_value > INT_MAX/2) // if( new_value > 50 )
          osThreadExit();
        if( osOK == osSemaphoreAcquire(sid_fibWriteable, 1000U)){
            fib_vect[0]=fib_vect[1];
            fib_vect[1]=new_value;
            osSemaphoreRelease( sid_fibReadable );
        } else {
            // error handling and exit
            printf( "Error acquiring writeable semaphore\n");
            osThreadExit();
        }
    }
}

// Reader (printer) can print when the writer (calculator) is done
__NO_RETURN static void thrFibPrint( void *arg) {
    for(;;)
    {
        if( osOK == osSemaphoreAcquire(sid_fibReadable, 1000U))
        {
            printf("%d\r\n", fib_vect[1]);
            osSemaphoreRelease(sid_fibWriteable);
        } else {
            // error handling and exit
            osThreadExit();
        }
    }
}

/*---------------------------------------------------------------------------
  thrLED: blink LED
 *---------------------------------------------------------------------------*/
__NO_RETURN static void thrLED (void *arg) {
//  uint32_t active_flag = 0U;

  (void)arg;

  for (;;) {
//    if (osThreadFlagsWait(1U, osFlagsWaitAny, 0U) == 1U) {
//      active_flag ^= 1U;
//    }

//    if (active_flag == 1U) {
////      vioSetSignal(vioLED0, vioLEDoff);         // Switch LED0 off
////      vioSetSignal(vioLED1, vioLEDon);          // Switch LED1 on
//      osDelay(100U);                            // Delay 100 ms
////      vioSetSignal(vioLED0, vioLEDon);          // Switch LED0 on
////      vioSetSignal(vioLED1, vioLEDoff);         // Switch LED1 off
//      osDelay(100U);                            // Delay 100 ms
//    }
//    else {
			printf("LED On\r\n");
      osDelay(500U);                            // Delay 500 ms
			printf("LED Off\r\n");
      osDelay(500U);                            // Delay 500 ms
//    }
  }
}

/*---------------------------------------------------------------------------
 * Application main thread
 *---------------------------------------------------------------------------*/
static void app_main (void *argument) {
  (void)argument;

  // Wasn't sure I should remove this or not, pullutes the debuggin output, so I did
//  tid_thrLED = osThreadNew(thrLED, NULL, NULL);         // Create LED thread
//  if (tid_thrLED == NULL) { /* add error handling */ }

  // Semaphores are initalized to allow the reader in first.
  sid_fibReadable = osSemaphoreNew(1U,1U, NULL);
  if( sid_fibReadable == NULL ) {/* handle it in some way */}

  sid_fibWriteable = osSemaphoreNew(1U, 0U, NULL);
  if( sid_fibWriteable == NULL ) {/* handle it in some way */}

  // Reader and writer threads started
  tid_fibonacciCalcThread = osThreadNew( thrFibCalc, NULL, NULL);
  if( tid_fibonacciCalcThread == NULL ) { /* call the same error hanlder as abolve */}
  tid_fibPrintThread = osThreadNew( thrFibPrint, NULL, NULL);
  if( tid_fibPrintThread == NULL ) { /* call the same error hanlder as abolve */}
  osThreadExit();
}

/*---------------------------------------------------------------------------
 * Application initialization
 *---------------------------------------------------------------------------*/
void app_initialize (void) {
  osThreadNew(app_main, NULL, NULL);
}
