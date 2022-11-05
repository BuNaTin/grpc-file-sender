set(ServerProject fs_server)

add_executable(${ServerProject} main.cpp)

target_link_libraries(${ServerProject} PRIVATE ${ProtoGen})
