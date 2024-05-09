cmake_minimum_required(VERSION 3.15)
project(Client_t)

set(EXECUTABLE_OUTPUT_PATH ${EPATH})

add_executable(alclient TestClient.cc)
target_include_directories(alclient PRIVATE ${HAPTH})
target_link_directories(alclient PRIVATE ${LPATH})
target_link_libraries(alclient PRIVATE ${COROLIB})
