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

#include "slacking.hpp"
#include "serial-logger.hpp"
#ifdef WIRINGPI
#include "temperature-logger.hpp"
#endif

int main(int argc, char** argv) {

    // Handle command line arguments -------------------------------------------------------------------------------------------
    bool use_slack = false;
    bool log_serial = false;
    bool log_temperature = false;
    std::string port = "/dev/ttyUSB0";
    unsigned int baud_rate = 115200;

    // Declare the supported options.
    boost::program_options::options_description desc("Allowed options");
    desc.add_options()
        ("help,h", "produce help message")
        ("use-slack,s", boost::program_options::bool_switch(&use_slack)->default_value(false), "send log data over slack")
        ("log-serial,l", boost::program_options::bool_switch(&log_serial)->default_value(false), "log serial data")
        #ifdef WIRINGPI
        ("log-temperature,t", boost::program_options::bool_switch(&log_temperature)->default_value(false), "log temperature data")
        #endif
        ("serial-device,d", boost::program_options::value<std::string>(&port), "serial device to use for data logging (/dev/ttyUSB0)")
        ("baud-rate,b", boost::program_options::value<unsigned int>(&baud_rate), "serial device baud rate (115200)")
    ;

    boost::program_options::variables_map vm;
    boost::program_options::store(boost::program_options::parse_command_line(argc, argv, desc), vm);
    boost::program_options::notify(vm);

    if (vm.count("help")) {
        std::cout << desc << "\n";
        return 1;
    }
    // Done with command line arguments ---------------------------------------------------------------------------------------

    // Get the slack token
    std::string token;
    std::ifstream token_file("slack-token.txt");
    std::getline(token_file, token);

    // Create the slack connection
    slack::Slacking& slack = slack::create(token);
    slack.chat.channel = "#temperature-log";

    std::thread* serial_thread;
    if(log_serial) {
        serial_thread = new std::thread(serialLogger, port, baud_rate);
    }

    #ifdef WIRINGPI
    std::thread* temperature_thread;
    if(log_temperature) {
        temperature_thread = new std::thread(temperatureLogger, slack);
    }
    #endif

    if(log_serial) {
        serial_thread->join();
    }
    #ifdef WIRINGPI
    if(log_temperature) {
        temperature_thread->join();
    }
    #endif

    return 0;
}
