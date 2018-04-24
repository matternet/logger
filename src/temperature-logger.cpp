#include <iostream>
#include <fstream>
#include <vector>
#include <streambuf>
#include <string>
#include <cmath>
#include <thread>
#include <boost/program_options.hpp>
#include <boost/filesystem/operations.hpp>
#include <boost/filesystem/fstream.hpp>

#include <wiringPi.h>
#include <pcf8591.h>
#include "slacking.hpp"

#include "temperature-logger.hpp"

/* These temperature probes use a NTCLE413E2103F102L.
 * R25-VALUE = 10khom (103)
 * R25 tolerance = 1% (F)
 * Special B25/85 value: L (low): 3000 ≤ B25/85 < 3500
 *
 * Specifically the designation is: NTCLE413-428 10K 1 % B3435 K
 *
 * For more detail see: www.vishay.com/thermistors/ntc-curve-list/
 *
 * In the circuit, the NTC is the lower part of a voltage divider with the upper part at 40.2kohms.
 *
 * f = adc_val/adc_max = RT / (40200 + RT)
 * where RT is the resistance of the RTC thermistor.
 * rearranging yields:
 * RT = f*40200 / (1-f)
 *
 * Temperature = 3425 / (11.521 + log( RT/10000 ))
 */

float isl94212_ExternalTemperatureRegisterReadingConversionTo_degC(uint8_t val_u8) {
    int32_t val_i32;
    float val_float, f, RT, externalTemp_degK_float, externalTemp_degC_float;
    val_i32 = (int32_t)(val_u8 & 0x3FFF);
    val_float = (float)val_i32;
    f = val_float / (float)0x3fff;
    RT = f*40200.0 / (1.0 - f);
    externalTemp_degK_float = 3425.0 / (11.521 + log(RT/10000.0));
    externalTemp_degC_float = externalTemp_degK_float - 273.15;
    return externalTemp_degC_float;
}

void serialLogger(slack::Slacking slack) {

    slack.chat.channel = "#temperature-log";

    // Connect to the pcf8591
    wiringPiSetup();
    pcf8591Setup(PCF8591_BASE, PCF8591_ADDRESS);

    // Define the analog inputs
    std::vector<uint8_t> analog_input_number = {
      PCF8591_A0,
      PCF8591_A1,
      PCF8591_A2,
      PCF8591_A3
    };

    auto now = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());
    std::stringstream out_file_name;
    out_file_name << "temperature_" << std::put_time(std::localtime(&now), "%m-%d_%H-%M-%S") << ".log";
    std::ofstream output_file;
    output_file.open((out_file_name.str()), std::ofstream::out | std::ofstream::app);

    while (true) {
        now = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());

        std::stringstream ss;
        ss.precision(2);
    	ss << "`" << std::put_time(std::localtime(&now), "%m/%d - %H:%M:%S");

        #ifdef WIRINGPI
        int i = 0;
        for (auto const& analog_input : analog_input_number) {
            ss << "   " << std::setw(7) << isl94212_ExternalTemperatureRegisterReadingConversionTo_degC(analogRead(analog_input)) << " C";
            i++;
        }
        #endif

        ss << "`";

        if (use_slack) {
            slack.chat.postMessage(ss.str());
        }

        ss << "\n";
        output_file << ss.str();
        output_file.flush();

        std::this_thread::sleep_for(std::chrono::seconds(1));
      }

    return 0;
}
