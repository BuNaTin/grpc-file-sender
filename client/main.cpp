#include <FileSender/Client.hpp>

#include <Logger.h>
#include <argparse/argparse.hpp>
#include <iostream>
#include <types.h>

#define ARG_ADDRESS "--address"
#define ARG_ADDRESS_SHORT "-a"

#define ARG_PORT "--port"
#define ARG_PORT_SHORT "-p"

#define ARG_SEND_ABORT "--abort"
#define ARG_SEND_ABORT_SHORT "-a"

#define ARG_SEND_FILE "--file"
#define ARG_SEND_FILE_SHORT "-f"

#define ARG_HELP "--help"

void buildArgs(argparse::ArgumentParser &parser,
               argparse::ArgumentParser &send) {
    parser.add_argument(ARG_ADDRESS, ARG_ADDRESS_SHORT)
            .help("where should client connect to")
            .default_value(std::string("127.0.0.1"));
    parser.add_argument(ARG_PORT, ARG_PORT_SHORT)
            .help("server port")
            .default_value(9090)
            .scan<'i', int>();

    send.add_description("Client based on grpc commands");
    send.add_argument(ARG_SEND_ABORT, ARG_SEND_ABORT_SHORT)
            .help("Send abort command to server to cancel file "
                  "uploading")
            .default_value(false)
            .implicit_value(true);
    send.add_argument(ARG_SEND_FILE, ARG_SEND_FILE_SHORT)
            .help("Send file to server")
            .required();

    parser.add_subparser(send);
}

int main(int argc, char *argv[]) {
    checkBytes(); // assertion for types size

    argparse::ArgumentParser arg_parser("File sender client");
    argparse::ArgumentParser send("send");
    buildArgs(arg_parser, send);
    try {
        arg_parser.parse_args(argc, argv);
    } catch (std::runtime_error &err) {
        LOGE("{} at parsing arguments", err.what());
        return 1;
    } catch (std::logic_error &err) {
        LOGE("{} in logic while parsing arguments", err.what());
        return 1;
    }

    if (arg_parser.is_used(ARG_HELP)) {
        std::cout << arg_parser.help().str() << std::endl;
        return 0;
    }

    FileSender::Client::Settings cl_settings;
    cl_settings.address = arg_parser.get<std::string>(ARG_ADDRESS);
    cl_settings.port = arg_parser.get<int>(ARG_PORT);
    auto client = FileSender::Client::create(cl_settings);
    if(!client) {
        LOGE("Could not create client");
        return 1;
    }

    if (send.is_used(ARG_SEND_ABORT)) {
        client->sendAbort();
    }
    if (send.is_used(ARG_SEND_FILE)) {
        std::string filename = send.get<std::string>(ARG_SEND_FILE);
        client->sendFile(filename);
    }

    return 0;
}
