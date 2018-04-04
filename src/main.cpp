#include "slacking.hpp"

#include <iostream>
#include <fstream>
#include <vector>
#include <streambuf>
#include <string>

int main() {

    std::string token;
    std::ifstream token_file("slack-token.txt");
    std::getline(token_file, token);

    auto& slack = slack::create(token);
    slack.chat.channel = "#temperature-log";

    std::string message;

    std::vector<float> temp_list = {15.3, 15.2, 132.0, 130.1};
    auto now = std::chrono::system_clock::now();
    auto now_c = std::chrono::system_clock::to_time_t(now);

    std::stringstream ss;
    ss.setf(std::ios::fixed, std::ios::floatfield);
    ss.precision(2);
		ss << "`" << std::put_time(std::localtime(&now_c), "%m/%d - %H:%M:%S");

    for (auto const& temp: temp_list) {
        ss << "   " << std::setw(6) << temp;
    }

    ss << " C`";

    message = ss.str();

    slack.chat.postMessage(message);

    return 0;
}
