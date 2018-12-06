set(SOURCE
    "main.cpp"
    "utils.cpp"
    "log.cpp"
    ${CMAKE_CURRENT_SOURCE_DIR}/routes/common.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/routes/post.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/models/common.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/models/post.cpp
)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

add_executable(lal-seabolt
    ${SOURCE}
)

set(THREADS_PREFER_PTHREAD_FLAG ON)
find_package(Threads REQUIRED)

find_library(BOLT seabolt17 PATHS "${PROJECT_SOURCE_DIR}/lib64")

include_directories(
    "${PROJECT_SOURCE_DIR}/include"
    "${PROJECT_SOURCE_DIR}/include/seabolt17"
)
target_link_libraries(
    ${PROJECT_NAME}
    lal
    uuid
    Threads::Threads
    ssl
    crypto
    ${BOLT}
)
