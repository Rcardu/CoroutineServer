
set(EXECUTABLE_OUTPUT_PATH ${EPATH})

add_executable(cocrexe alpha.cc)
target_include_directories(cocrexe PRIVATE ${HAPTH})
target_link_directories(cocrexe PRIVATE ${LPATH})
target_link_libraries(cocrexe PRIVATE ${COROLIB})
target_compile_definitions(cocrexe PRIVATE)

add_subdirectory(clinet)
