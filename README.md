# grpc-file-sender
Client for sending big files throw grpc

Client would upload files chunk by chunk in different rpc calls, until the remain size (total file size). Server will count size of streams recieved and answer smth like "Continue", when it is waiting for the next package and "Done" when all data is recieved. Chunk size is 32Kb, like tcp packages of big files that have 32Kb or 64Kb size.

## Run 

Istall build utils (`scripts/install-build-utils.sh`) and run `scripts/build.sh`.
Server binary will be in `build/server/fs_server` and client - `build/client/fs_client`

If you want to send some data to server - call client with param `send` or `send -h` to get more info

## Test

After installing test-utils run `scripts/run-test`, which starts a pack of basic tests and assertions
