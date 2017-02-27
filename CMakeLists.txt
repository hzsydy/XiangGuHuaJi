CMAKE_MINIMUM_REQUIRED(VERSION 2.8)

PROJECT(XiangGuHuaJi)

INCLUDE_DIRECTORIES(
    ./XiangGuHuaJi/
    ./HuaJiCmd/
)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} --std=c++11")

# SRC
SET(LOGIC_SRC
    ./XiangGuHuaJi/game.cpp
    ./XiangGuHuaJi/player.cpp
    ./XiangGuHuaJi/map.cpp
)

SET(CMD_SRC
    ./HuaJiCmd/controller.cpp
    ./HuaJiCmd/main.cpp
)

# HuaJiCmd
ADD_EXECUTABLE(HuaJiCmd
    ${CMD_SRC}
    ${LOGIC_SRC}
)

TARGET_LINK_LIBRARIES(HuaJiCmd
    ${CMAKE_DL_LIBS}
)


# TestAi
ADD_LIBRARY(TestAi
    SHARED
    ./TestAi/ai.cpp
)


