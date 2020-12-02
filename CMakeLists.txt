cmake_minimum_required (VERSION 3.8)

project ("video-crop")

add_executable (video-crop "video-crop.cpp")
set(EXECUTABLE_OUTPUT_PATH ${PROJECT_BINARY_DIR}/bin)

set(OpenCV_DIR "C:\\opencv\\build")

find_package(OpenCV REQUIRED)
target_link_libraries(${PROJECT_NAME} ${OpenCV_LIBS})

add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
        "${PROJECT_SOURCE_DIR}/lib"
        $<TARGET_FILE_DIR:${PROJECT_NAME}>)
