#ifndef TEMPERATURE_LOGGER_HPP_
#define TEMPERATURE_LOGGER_HPP_

#define PCF8591_ADDRESS 0x48
#define PCF8591_BASE 64
#define PCF8591_A0 PCF8591_BASE + 0
#define PCF8591_A1 PCF8591_BASE + 1
#define PCF8591_A2 PCF8591_BASE + 2
#define PCF8591_A3 PCF8591_BASE + 3

void analogLogger(std::string port, unsigned int baud_rate);
float isl94212_ExternalTemperatureRegisterReadingConversionTo_degC(uint8_t val_u8);

#endif
