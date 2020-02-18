#pragma once

#include <exception>
#include <string>

class TypeError : public std::exception {
private:
    std::string m_message = "Type error, expected: ";
public:
    TypeError(std::string expectedType, std::string val) {
        m_message += expectedType + " but value is: " + val;
    }

    const char *what() const noexcept override {
        return m_message.c_str();
    }
};