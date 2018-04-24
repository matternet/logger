#include <iostream>
#include <string>
#include <boost/filesystem/operations.hpp>
#include <boost/filesystem/fstream.hpp>
#include <chrono>
#include <iomanip>

#include "bitscope-logger.hpp"

extern "C" {
    #include <bitlib.h>
}

#define MY_DEVICES 1 /* open one device only */
#define MY_PROBE_FILE "" /* default probe file if unspecified */

#define MY_DEVICE 0
#define MY_CHANNEL 0
#define MY_MODE BL_MODE_FAST
#define MY_RATE 1000000 /* capture sample rate */
#define MY_SIZE 4 /* number of samples to capture */

void bitscopeLogger() {
    /*
    * Open and select the first channel on the first device.
    */
    printf("\nStarting: Attempting to open %d device%s...\n",MY_DEVICES,MY_DEVICES!=1?"s":"");
    if(!BL_Open(MY_PROBE_FILE,MY_DEVICE)) {
        printf("Failed to find a devices.\n");
        return;
    }
    if(BL_Select(BL_SELECT_DEVICE,MY_DEVICE) != MY_DEVICE) {
        printf("Failed to select device %d.\n",MY_DEVICE);
        return;
    }
    if(BL_Select(BL_SELECT_CHANNEL,MY_CHANNEL) != MY_CHANNEL) {
        printf("Failed to select channel %d.\n",MY_CHANNEL);
        return;
    }

    /*
    * Prepare to capture one channel...
    */
    if ( BL_Mode(MY_MODE) != MY_MODE ) {
        printf("Failed to select mode %d.\n",MY_MODE);
        return;
    }

    BL_Intro(BL_ZERO); /* optional, default BL_ZERO */
    BL_Delay(BL_ZERO); /* optional, default BL_ZERO */
    BL_Rate(MY_RATE); /* optional, default BL_MAX_RATE */
    BL_Size(MY_SIZE); /* optional, default BL_MAX_SIZE */
    BL_Select(BL_SELECT_CHANNEL,MY_CHANNEL); /* choose the channel */
    BL_Trigger(BL_ZERO,BL_TRIG_RISE); /* optional when untriggered */
    BL_Select(BL_SELECT_SOURCE,BL_SOURCE_POD); /* use the POD input */
    BL_Range(BL_Count(BL_COUNT_RANGE)); /* maximum range */
    BL_Offset(BL_ZERO); /* optional, default 0 */
    BL_Enable(true); /* at least one channel must be initialised */

    while (true) {
        auto now = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());
        // std::stringstream out_file_name;
        // out_file_name << "scope_" << std::put_time(std::localtime(&now), "%m-%d_%H-%M-%S") << ".log";

        //boost::filesystem::ofstream log_file(out_file_name.str());

        /*
        * Capture and acquire the data...
        */
        printf("   Trace: %d samples @ %.0fHz = %fs\n",BL_Size(BL_ASK),BL_Rate(BL_ASK), BL_Time(BL_ASK));
        if(BL_Trace(BL_TRACE_FORCED, BL_SYNCHRONOUS)) { /* capture data (without a trigger) */
            int i, n = MY_SIZE;
            double d[n]; /* let's get 5 samples */
            BL_Select(BL_SELECT_CHANNEL,MY_CHANNEL); /* optional if only one channel */
            if(BL_Acquire(n, d) == n) { /* acquire (i.e. dump) the capture data */
                for (i = 0; i < n; i++) {
                    printf(" %f", d[i]);
                }
                printf("\n\n");
            }
        }
        printf("%s\n",BL_Log());

        //log_file.flush();

        //log_file.close();
    }

    BL_Close(); /* call this to release library resources */
}
