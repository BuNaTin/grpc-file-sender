cmake_minimum_required(VERSION 3.16)

project(grpc_file_sender)
set(Project grpc_file_sender)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# compilers flags
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -Werror -pedantic")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wno-unused-parameter -Wno-unused-function -Wno-error=unused-variable")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wno-terminate -Wno-error=maybe-uninitialized -Wno-error=literal-suffix")

set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -Wall -Wextra -Werror -pedantic -Wnull-dereference")
set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -Wduplicated-cond -Wduplicated-branches")
set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -Wlogical-op -Wnon-virtual-dtor -O3")

# install conan for cmake
if(NOT EXISTS "${CMAKE_BINARY_DIR}/conan.cmake")
  message(STATUS "Downloading conan.cmake from https://github.com/conan-io/cmake-conan")
  file(DOWNLOAD "https://raw.githubusercontent.com/conan-io/cmake-conan/0.18.1/conan.cmake"
                "${CMAKE_BINARY_DIR}/conan.cmake"
                TLS_VERIFY ON)
endif()

# setup dependencies
include(${CMAKE_BINARY_DIR}/conan.cmake)
conan_cmake_autodetect(settings)
conan_cmake_run(CONANFILE conanfile.txt BASIC_SETUP CMAKE_TARGETS NO_OUTPUT_DIRS)
include(${CMAKE_BINARY_DIR}/conan_paths.cmake)

# grpc create & build in static lib
set(ProtoGen protos_generated_lib)

configure_file(.clang-format .clang-format COPYONLY)

set(GRPC_DIR ${CMAKE_CURRENT_BINARY_DIR}/generated)
file(MAKE_DIRECTORY ${GRPC_DIR})

message("Proto dir: ${GRPC_DIR}")

set(protos_file sender.proto)
set(proto_srcs "${GRPC_DIR}/sender.pb.cc")
set(proto_hdrs "${GRPC_DIR}/sender.pb.h")
set(grpc_srcs "${GRPC_DIR}/sender.grpc.pb.cc")
set(grpc_hdrs "${GRPC_DIR}/sender.grpc.pb.h")
add_custom_command(
    OUTPUT "${proto_srcs}" "${proto_hdrs}" "${grpc_srcs}" "${grpc_hdrs}"
    COMMAND ./build_tools/protoc
    ARGS --grpc_out "${GRPC_DIR}"
         --cpp_out "${GRPC_DIR}"
         -I "../protos"
         --plugin=protoc-gen-grpc="build_tools/grpc_cpp_plugin"
         ${protos_file}
    DEPENDS ${CMAKE_CURRENT_BINARY_DIR}/../protos/${protos_file}
    COMMENT "Invoking protoc to generate gRPC C++ files."
)

add_library(${ProtoGen}
    STATIC 
    ${proto_srcs}
    ${grpc_srcs}
)
target_include_directories(${ProtoGen}
    PUBLIC 
    ${GRPC_DIR}
)
target_link_libraries(${ProtoGen}
    PUBLIC 
    CONAN_PKG::grpc
)

# Executables
# empty yet
