#include "slacking.hpp"

#include <iostream>
#include <fstream>
#include <vector>
#include <streambuf>
#include <string>
#include <cmath>
#include <thread>

#include <wiringPi.h>
#include <pcf8591.h>

#define PCF8591_ADDRESS 0x48
#define PCF8591_BASE 64
#define PCF8591_A0 PCF8591_BASE + 0
#define PCF8591_A1 PCF8591_BASE + 1
#define PCF8591_A2 PCF8591_BASE + 2
#define PCF8591_A3 PCF8591_BASE + 3

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

int main() {

    // Get the slack token
    std::string token;
    std::ifstream token_file("slack-token.txt");
    std::getline(token_file, token);

    // Create the slack connection
    auto& slack = slack::create(token);
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

    std::string message;

    while (true) {
        auto now = std::chrono::system_clock::now();
        auto now_c = std::chrono::system_clock::to_time_t(now);

        std::stringstream ss;
        ss.setf(std::ios::fixed, std::ios::floatfield);
        ss.precision(2);
    		ss << "`" << std::put_time(std::localtime(&now_c), "%m/%d - %H:%M:%S");

        int i = 0;
        for (auto const& analog_input : analog_input_number) {
            ss << "   " << std::setw(7) << isl94212_ExternalTemperatureRegisterReadingConversionTo_degC(analogRead(analog_input)) << " C";
            i++;
        }

        ss << "`";

        message = ss.str();

        slack.chat.postMessage(message);

        std::this_thread::sleep_for(std::chrono::seconds(1));
      }

    return 0;
}
