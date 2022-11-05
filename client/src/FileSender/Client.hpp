#pragma once

#include <memory>
#include <string>
#include <types.h>

namespace FileSender {

class Client {
public:
    virtual bool sendFile(const std::string &filename) noexcept = 0;
    virtual bool sendAbort() noexcept = 0;

    // constructors
public:
    struct Settings {
        std::string address;
        u16 port;
    };

    static std::unique_ptr<Client> create(const Settings &settings);

    virtual ~Client(){}
};

} // namespace FileSender