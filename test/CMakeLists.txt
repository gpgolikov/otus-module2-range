project(${PROJECT_NAME}_tests)

list(APPEND ${PROJECT_NAME}_SOURCES
    test_ip_address.cpp
    main.cpp)

add_executable(${PROJECT_NAME} ${${PROJECT_NAME}_SOURCES})

target_link_libraries(${PROJECT_NAME} CONAN_PKG::Catch2 CONAN_PKG::range-v3)

set_target_properties(${PROJECT_NAME} PROPERTIES
    CXX_STANDARD 17
    CXX_STANDARD_REQUIRED ON
    INCLUDE_DIRECTORIES ${CMAKE_SOURCE_DIR}/src
)

if (NOT WIN32)
    set_target_properties(${PROJECT_NAME} PROPERTIES
        COMPILE_OPTIONS "-Wall;-Wextra"
    )
else()
    set_target_properties(${PROJECT_NAME} PROPERTIES
        COMPILE_OPTIONS "-Wpedantic;-Wall;-Wextra"
    )
endif()

enable_testing()

add_test(NAME ${PROJECT_NAME} COMMAND ${PROJECT_NAME})