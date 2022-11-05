import pytest
import os
import subprocess

def run_server(address: str, port: str) -> int :
    command: list = ["./client/fs_client", "-a", address, "-p", port]
    result = subprocess.run(command, capture_output=True, text=True)
    return result.returncode

def run_client(address: str, port: str, commands) -> int :
    command: list = ["./client/fs_client", "-a", address, "-p", port]
    if(len(commands) != 0):
        command += ["send"]
        command += commands
    result = subprocess.run(command, capture_output=True, text=True)
    print(result)
    return result.returncode

@pytest.mark.parametrize("ip, port, code", [
    ("127.0.0.1", "9090", 0),
    ("124.3434.435.345", "9090", 1),
    ("9.5.6.7","12423", 0)
])
def test_client(ip: str, port: str, code: int):
    if(code == 0):
        assert(run_client(ip, port, []) == 0)
    else:
        assert(run_client(ip, port, []) != 0)


@pytest.mark.parametrize("ip, port, commands, code", [
    ("127.0.0.1", "9090", ["-a"], 1),
    ("127.0.0.1", "9090", ["d"] , 1),
    ("127.0.0.1", "9090", ["-f", "../conanfile.txt"], 0),
    ("127.0.0.1", "9090", ["-f", "server/fs_server"], 0),
    ("127.0.0.1", "9090", ["-f", "conanfile"], 1)
])
def test_sending(ip: str, port: str, commands, code: int):
    run_server(ip, port)
    if(code == 0):
        assert(run_client(ip, port, commands) == 0)
    else:
        assert(run_client(ip, port, commands) != 0)

#run_client("127.0.0.1", "9090", ["-f", "Fff"])
