cmake_minimum_required(VERSION 3.16)

find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Test)

add_executable(test_track test_track.cpp ../src/model/track.cpp)
target_link_libraries(test_track PRIVATE Qt${QT_VERSION_MAJOR}::Test) 
add_executable(test_playercontroller test_playercontroller.cpp ../src/controller/playercontroller.cpp ../src/model/track.cpp ../src/player/player.cpp)
target_link_libraries(test_playercontroller PRIVATE Qt${QT_VERSION_MAJOR}::Test mpv)
add_executable(test_player test_player.cpp ../src/player/player.cpp)
target_link_libraries(test_player PRIVATE Qt${QT_VERSION_MAJOR}::Test mpv) 

add_test(NAME test_track COMMAND test_track)
add_test(NAME test_playercontroller COMMAND test_playercontroller)
add_test(NAME test_player COMMAND test_player)