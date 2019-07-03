#include <string>
#include <iostream>

std::string parse_line(const std::string& line) {
    std::string part_msg{};
    size_t n = 0;
    size_t prev_n = n;
    while (true) {
        n = line.find(' ', prev_n);
        if (n == std::string::npos) break;
        const auto numb = static_cast<const float>(std::stod(line.substr(prev_n, n)));
        part_msg.append(reinterpret_cast<const char*>(&numb), 4);
        prev_n = n+1;
    }
    if (!line.empty()) {
        const auto numb = static_cast<const float>(std::stod(line.substr(prev_n, line.size() - 1)));
        part_msg.append(reinterpret_cast<const char *>(&numb), 4);
    }

    return part_msg;
}