#include <FileSender/Client.hpp>

#include <Logger.h>
#include <arpa/inet.h>
#include <filesystem>
#include <fstream>
#include <grpcpp/grpcpp.h>
#include <sender.grpc.pb.h>

namespace {

inline bool sendFileName(SenderService::Stub *stub,
                         const std::string &filename,
                         UploadFileResponse &resp) noexcept {
    grpc::ClientContext context;
    FileInfo info;
    info.set_name(filename);
    info.set_remain_size(std::filesystem::file_size(filename));
    grpc::Status status = stub->StartUploadFile(&context, info, &resp);
    return status.ok();
}

inline bool validateIp(const std::string &address) {
    struct sockaddr_in sa;
    int result = inet_pton(AF_INET, address.c_str(), &(sa.sin_addr));
    return result > 0;
}

} // namespace

namespace FileSender {

class ClientImpl final : public Client {
public:
    bool sendFile(const std::string &filename) noexcept override;
    bool sendAbort() noexcept override;

    // constuctors
public:
    ClientImpl(const Client::Settings &settings);
    ~ClientImpl() = default;

private:
    std::unique_ptr<SenderService::Stub> m_stub;
};

std::unique_ptr<Client>
Client::create(const Client::Settings &settings) {
    if(!validateIp(settings.address)) {
        LOGE("Wrong ip {} to client", settings.address);
        return nullptr;
    }

    return std::make_unique<ClientImpl>(settings);
}

ClientImpl::ClientImpl(const Client::Settings &settings) {
    const std::string &bind_address =
            settings.address + ':' + std::to_string(settings.port);
    const auto channel = grpc::CreateChannel(
            bind_address, grpc::InsecureChannelCredentials());
    m_stub = SenderService::NewStub(channel);
    if (!m_stub) {
        LOGE("Error while creating stub, terminate");
        std::terminate();
    }
    LOGI("Create client working to {}", bind_address);
}

bool ClientImpl::sendFile(const std::string &filename) noexcept {
    UploadFileResponse response;
    if (!std::filesystem::exists(filename)) {
        LOGE("Couldnot open {}", filename);
        return false;
    }
    bool should_end = !sendFileName(m_stub.get(), filename, response);
    if (!should_end) {
        LOGI("Send filename {}, response: {}",
             filename,
             response.status());
    } else {
        LOGE("Couldnot send filename to server");
    }

    const std::size_t size = 32 * 1024;
    std::array<char, size> data;
    std::ifstream read_from(filename);
    grpc::Status status;
    while (!should_end) {
        read_from.read(data.begin(), size);
        std::size_t bytes_read = read_from.gcount();
        if (bytes_read == 0) {
            should_end = true;
            break;
        }
        char *end = data.begin() + bytes_read;
        grpc::ClientContext context;
        FileData payload;
        payload.set_chunk({data.begin(), end});
        LOGI("Send {} bytes", bytes_read);
        status = m_stub->UploadChunk(&context, payload, &response);
        should_end = !status.ok();
    }
    LOGI("Send done");

    return status.ok();
}

bool ClientImpl::sendAbort() noexcept {
    grpc::ClientContext context;
    UploadFileResponse resp;
    Empty empty;
    auto status = m_stub->AbortLoading(&context, empty, &resp);
    return status.ok();
}

} // namespace FileSender