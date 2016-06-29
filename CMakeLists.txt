CMAKE_MINIMUM_REQUIRED(VERSION 2.8.12.1)

PROJECT(XiangGuHuaJi)

SET(CMAKE_MODULE_PATH "${PROJECT_SOURCE_DIR}/cmake/Modules")

FIND_PACKAGE(OpenCV REQUIRED)
FIND_PACKAGE(Jsoncpp REQUIRED) # sudo apt-get install libjsoncpp-dev

INCLUDE_DIRECTORIES(
    ${OpenCV_INCLUDE_DIRS}
    ${JSONCPP_INCLUDE_DIR}
    ./XiangGuHuaJi/
)

#LOGIC
SET(LOGIC_SRC
    ./XiangGuHuaJi/game.cpp  
    ./XiangGuHuaJi/map.cpp
    ./XiangGuHuaJi/player.cpp
    ./XiangGuHuaJi/main.cpp 
    ./XiangGuHuaJi/MapParser.cpp
)
ADD_EXECUTABLE(Logic-XGHJ
    ${LOGIC_SRC}
)
TARGET_LINK_LIBRARIES(Logic-XGHJ
    ${OpenCV_LIBS}
    ${JSONCPP_LIBRARY}
)        

#TestAi
ADD_LIBRARY(TestAi SHARED
    ./TestAi/ai.cpp
)    

