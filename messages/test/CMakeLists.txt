project(tests)

find_package(Qt5Test REQUIRED)
find_package(BerkeleyDB REQUIRED)
find_package(persistent-storage REQUIRED)
find_package(cryptopp REQUIRED)

SET(CMAKE_AUTOMOC ON)
SET(CMAKE_INCLUDE_CURRENT_DIR ON)
SET(CMAKE_CXX_STANDARD 17)
SET(CMAKE_CXX_STANDARD_REQUIRED ON)

SET(MY_TEST_LIBS messages Qt5::Test messages-fakes encryption  utils  pthread xg persistent-storage cryptopp-static plugins-support ${Boost_SYSTEM_LIBRARY})

ENABLE_TESTING()



add_subdirectory(messages-fakes)



add_subdirectory( test_cases/persistent_storage )
add_subdirectory( test_cases/messages )
add_subdirectory( test_cases/watchers )
add_subdirectory( test_cases/utils )
