CMAKE_MINIMUM_REQUIRED(VERSION 2.8.7)

PROJECT(XiangGuHuaJi)

SET(CMAKE_MODULE_PATH "${PROJECT_SOURCE_DIR}/cmake/Modules")

#FIND_PACKAGE(Jsoncpp REQUIRED) # sudo apt-get install libjsoncpp-dev

INCLUDE_DIRECTORIES(
    ./XiangGuHuaJi/
#  ${JSONCPP_INCLUDE_DIR}
    ./XiangGuHuaJi/json/
)

#LOGIC
SET(LOGIC_SRC
    ./XiangGuHuaJi/game.cpp  
    ./XiangGuHuaJi/map.cpp
    ./XiangGuHuaJi/player.cpp
    ./XiangGuHuaJi/main.cpp
    ./XiangGuHuaJi/controller.cpp 
    ./thirdparty/src/json/json_reader.cpp
    ./thirdparty/src/json/json_writer.cpp
    ./thirdparty/src/json/json_value.cpp
    ./thirdparty/src/json/json_valueiterator.inl
)
ADD_EXECUTABLE(Logic-XGHJ
    ${LOGIC_SRC}
)
    
#TARGET_LINK_LIBRARIES(Logic-XGHJ
#    ${JSONCPP_LIBRARY}
#)     
    
#TestAi
ADD_LIBRARY(TestAi SHARED
    ./TestAi/ai.cpp
)    

