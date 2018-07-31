cmake_minimum_required (VERSION 2.6)
project (test_wireless-cooler)

FIND_PACKAGE( Boost 1.66 COMPONENTS serialization REQUIRED )
INCLUDE_DIRECTORIES( ${Boost_INCLUDE_DIR} )

include_directories(../src)
# include_directories(../../boost_1_66_0)

file(GLOB SOURCES
    *.h
    *.cpp
)


add_executable(test_wireless-cooler ${SOURCES})

include_directories(/usr/local/include)
target_link_libraries(test_wireless-cooler /usr/local/lib/libgtest.a)

TARGET_LINK_LIBRARIES( test_wireless-cooler LINK_PUBLIC ${Boost_LIBRARIES} )