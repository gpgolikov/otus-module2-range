list(APPEND ${PROJECT_NAME}_SOURCES
    main.cpp)

add_executable(${PROJECT_NAME} ${${PROJECT_NAME}_SOURCES})

target_link_libraries(${PROJECT_NAME} CONAN_PKG::range-v3)

set_target_properties(${PROJECT_NAME} PROPERTIES
    CXX_STANDARD 17
    CXX_STANDARD_REQUIRED ON
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

install(TARGETS ${PROJECT_NAME} RUNTIME DESTINATION bin)