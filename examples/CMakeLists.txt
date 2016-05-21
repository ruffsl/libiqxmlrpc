cmake_minimum_required(VERSION 2.8)
project(Libiqxmlrpc_examples)

include(FindBoost)
include(FindLibXml2)
include(FindOpenSSL)
include(CheckFunctionExists)

find_package(Boost REQUIRED COMPONENTS date_time thread system)
include_directories(${Boost_INCLUDE_DIR} ${XML2_INCLUDE_DIRS} ${LIBXML2_INCLUDE_DIR} ${OPENSSL_INCLUDE_DIR} ${PROJECT_BINARY_DIR}/libiqxmlrpc)
# find_package(Libiqxmlrpc REQUIRED)

include_directories(include)

# add the executable
add_executable(client
    src/client.cc)
add_executable(server
    src/server.cc)

add_executable(sclient
    src/sclient.cc)
add_executable(sserver
    src/sserver.cc
    include/helper.hpp)

set(example_libs
  iqxmlrpc
  ${Boost_LIBRARIES}
  ${XML2_LIBRARIES} # obsolete
  ${LIBXML2_LIBRARIES}
  ${OPENSSL_LIBRARIES}
)

target_link_libraries(client
${example_libs})
target_link_libraries(server
${example_libs})

target_link_libraries(sclient
${example_libs})
target_link_libraries(sserver
${example_libs})
