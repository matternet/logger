#include <iostream>
#include <string>
#include <boost/filesystem/operations.hpp>
#include <boost/filesystem/fstream.hpp>
#include <boost/asio.hpp>
#include <regex>
#include <chrono>
#include <iomanip>

#include "serial-logger.hpp"

void serialLogger(std::string port, unsigned int baud_rate) {
    boost::asio::io_service io;
    boost::asio::serial_port serial(io, port);
    serial.set_option(boost::asio::serial_port_base::baud_rate(baud_rate));

    char c;
    while (true) {
        auto now = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());
        std::stringstream out_file_name;
        out_file_name << "serial_" << std::put_time(std::localtime(&now), "%m-%d_%H-%M-%S") << ".log";

        boost::filesystem::ofstream log_file(out_file_name.str());
        std::regex new_log("LANDED");

        std::string line;
        while(!regex_match(line, new_log)) {
            boost::asio::read(serial, boost::asio::buffer(&c, 1));

            if(c == '\n' || c == '\r') {
                if(!line.empty()) {
                    now = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());
                    log_file << std::put_time(std::localtime(&now), "%m/%d - %H:%M:%S -- ") << line << std::endl;
                    log_file.flush();
                    line.clear();
                }
            }
            else {
                line += c;
            }
        }

        log_file.close();
    }
}
