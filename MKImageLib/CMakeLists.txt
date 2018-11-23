set(lib_src
    src/MKIFileType.cpp
    src/MKIHistogram.cpp
    src/MKIImage.cpp
    src/MKIImageFuncs.cpp
    src/MKIMask.cpp
)

add_library(MKImageLib ${lib_src})
set_target_properties(MKImageLib PROPERTIES
                      CMAKE_CXX_STANDARD 17
                      CXX_STANDARD_REQUIRED ON
                      CMAKE_CXX_EXTENSIONS OFF
)
target_compile_options(MKImageLib PUBLIC
    -std=c++17
    -lstdc++fs
    -pthread
)
target_link_libraries(MKImageLib PRIVATE -lstdc++fs -pthread) 

target_include_directories(MKImageLib PUBLIC src)