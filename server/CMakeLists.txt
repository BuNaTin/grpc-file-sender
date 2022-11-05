set(ServerProject fs_server)

add_executable(${ServerProject} main.cpp)

target_include_directories(${ServerProject} PRIVATE src)

target_sources(${ServerProject} PRIVATE
    src/FileReciever/Server.cpp
)

target_link_libraries(${ServerProject} PRIVATE ${CORE_LIB})
