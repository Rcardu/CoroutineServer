
set(LIBRARY_OUTPUT_PATH ${LPATH})
aux_source_directory(./ SRC)
add_library(${COROLIB} STATIC ${SRC})

target_include_directories(${COROLIB} PUBLIC ${HAPTH} ${Boost_INCLUDE_DIRS} ${jsoncpp_INCLUDE_DIR})
target_link_directories(${COROLIB} PUBLIC ${boost_LIBRARY_DIRS} ${jsoncpp_LIBRARY_DIR})
target_link_libraries(${COROLIB} PUBLIC ${Boost_LIBRARIES} jsoncpp)
target_compile_definitions(${COROLIB} PUBLIC

  # 使用Clang作为前端编译器时，需要手动开启此宏使用Asio相关协程
  BOOST_ASIO_HAS_CO_AWAIT

  # 使用MSVC作为编译器时，需要手动开启此宏以启用C++20协程库
  BOOST_ASIO_HAS_STD_COROUTINE)

# if(MSVC)
# foreach(flag_var CMAKE_CXX_FLAGS CMAKE_CXX_FLAGS_DEBUG CMAKE_CXX_FLAGS_RELEASE
# CMAKE_CXX_FLAGS_MINSIZEREL CMAKE_CXX_FLAGS_RELWITHDEBINFO)
# if(${flag_var} MATCHES "/MD")
# string(REGEX REPLACE "/MD" "/MT" ${flag_var} "${${flag_var}}")
# endif()
# endforeach()
# endif()
