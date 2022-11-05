# grpc-file-sender
Client for sending big files throw grpc

Client would upload files chunk by chunk in different rpc calls, until the remain size (total file size). Server will count size of streams recieved and answer smth like "Continue", when it is waiting for the next package and "Done" when all data is recieved
