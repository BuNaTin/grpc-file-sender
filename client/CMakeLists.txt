set(ClientProject client_project)

add_executable(${ClientProject} main.cpp)

target_include_directories(${ClientProject} PRIVATE src)

target_sources(${ClientProject} PRIVATE
    src/FileSender/Client.cpp
)

target_link_libraries(${ClientProject} PRIVATE ${CORE_LIB})
