#include <FileReciever/Server.h>

#include <fstream>
#include <grpcpp/grpcpp.h>
#include <sender.grpc.pb.h>

#include <Logger.h>

namespace FileReciever {

class SenderServiceImpl final : public SenderService::Service {
    grpc::Status
    StartUploadFile(grpc::ServerContext *,
                    const FileInfo *data,
                    UploadFileResponse *response) override {
        if (m_remaining_size != 0 && !m_current_file.empty()) {
            LOGI("Server is busy loading other file");
            return {grpc::StatusCode::ABORTED,
                    "Server is busy loading other file"};
        }
        LOGI("Open {} file to recieve, bytes size {}",
             data->name(),
             data->remain_size());
        m_current_file = data->name();
        m_remaining_size = data->remain_size();
        m_out = std::ofstream(data->name(),
                              std::ios::binary | std::ios::trunc);

        response->set_status("Preparing to upload done");
        return grpc::Status::OK;
    }
    grpc::Status UploadChunk(grpc::ServerContext *,
                             const FileData *data,
                             UploadFileResponse *response) override {
        if (m_current_file.empty()) {
            return {grpc::StatusCode::FAILED_PRECONDITION,
                    "Server had not opened file"};
        }
        m_remaining_size -= data->chunk().size();
        m_out.write(data->chunk().data(), data->chunk().size());

        if (m_remaining_size <= 0) {
            CloseNStop();
            response->set_status("Done");
        } else {
            response->set_status("Continue");
        }
        return grpc::Status::OK;
    }

    grpc::Status AbortLoading(grpc::ServerContext *,
                              const Empty *,
                              UploadFileResponse *response) override {
        CloseNStop();
        response->set_status("Break file uploading done");
        return grpc::Status::OK;
    }

    // in-class logic
private:
    void CloseNStop() {
        m_out.close();
        m_current_file.clear();
        m_remaining_size = 0;
    }

private:
    std::ofstream m_out;
    std::string m_current_file;
    long long m_remaining_size = 0;
};

void RunServer(const std::string &address, u16 port) {
    const std::string server_address(address + ':' +
                                     std::to_string(port));
    SenderServiceImpl service;

    grpc::EnableDefaultHealthCheckService(true);
    grpc::ServerBuilder builder;
    builder.AddListeningPort(server_address,
                             grpc::InsecureServerCredentials());
    builder.RegisterService(&service);
    std::unique_ptr<grpc::Server> server(builder.BuildAndStart());
    LOGI("Server listening on {}", server_address);

    server->Wait();
    LOGI("Work done");
}

} // namespace FileReciever