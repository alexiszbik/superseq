cmake_minimum_required(VERSION 3.20)
project(ProtoSeq VERSION 0.1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

include(FetchContent)

FetchContent_Declare(
    rtmidi
    GIT_REPOSITORY https://github.com/thestk/rtmidi.git
    GIT_TAG 6.0.0
)

set(RTMIDI_BUILD_TESTING OFF CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(rtmidi)

add_executable(ProtoSeq
    src/main.cpp
    src/MidiClock.cpp
    src/Sequence.cpp
    src/SequenceTrack.cpp
    src/VirtualMidiSender.cpp
)

target_include_directories(ProtoSeq PRIVATE src)
target_link_libraries(ProtoSeq PRIVATE rtmidi)

if(APPLE)
    target_link_libraries(ProtoSeq PRIVATE "-framework CoreMIDI" "-framework CoreFoundation")
endif()
