cmake_minimum_required (VERSION 2.6)
project (test_wireless-cooler)

include_directories(../src)

file(GLOB SOURCES
    *.h
    *.cpp
)


add_executable(test_wireless-cooler ${SOURCES})

include_directories(/usr/local/include)
target_link_libraries(test_wireless-cooler /usr/local/lib/libgtest.a)

