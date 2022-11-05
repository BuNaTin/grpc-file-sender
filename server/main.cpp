#include <FileReciever/Server.h>

#include <Logger.h>
#include <argparse/argparse.hpp>
#include <types.h>

#define ARG_ADDRESS "--address"
#define ARG_ADDRESS_SHORT "-a"

#define ARG_PORT "--port"
#define ARG_PORT_SHORT "-p"

#define ARG_HELP "--help"

void buildArgs(argparse::ArgumentParser &parser) {
    parser.add_argument(ARG_ADDRESS, ARG_ADDRESS_SHORT)
            .help("where should server bind on")
            .default_value(std::string("0.0.0.0"));
    parser.add_argument(ARG_PORT, ARG_PORT_SHORT)
            .help("server bind port")
            .default_value(9090)
            .scan<'i', int>();
}

int main(int argc, char *argv[]) {
    checkBytes();
    argparse::ArgumentParser arg_parser("File reciever server");
    buildArgs(arg_parser);
    try {
        arg_parser.parse_args(argc, argv);
    } catch (std::runtime_error &err) {
        LOGE("{} while parsing arguments", err.what());
        return 1;
    } catch (std::logic_error &err) {
        LOGE("{} in logic while parsing arguments", err.what());
        return 1;
    }

    if(arg_parser.is_used(ARG_HELP)) {
        std::cout << arg_parser.help().str() << std::endl;
        return 0;
    }

    std::string bind_address = arg_parser.get<std::string>(ARG_ADDRESS);
    u16 bind_port = arg_parser.get<int>(ARG_PORT);
    FileReciever::RunServer(bind_address, bind_port);

    return 0;
}