#香菇滑鸡 主逻辑

## 概述

- 服务器
    - 游戏逻辑 **Game**
    - 地图参数 **Map**
    - 提供与Ai的接口 **Player**
- 用户
    - 参赛**Ai**

## 文件一览

- 服务器    
    - main.cpp 加载Map和Player,启动Game
    - game.h/game.cpp 游戏逻辑Game
    - map.h/map.cpp 地图参数Map
    - player.h/player.cpp 提供与Ai的接口(装填Info)
    - definition.h 所有类型的定义
    - converter.hpp 进行vector和Mat的转化
    - info.hpp 存储Ai用的全部数据Info
- 用户
    - ai.h/ai.cpp 参赛Ai
    - definition.h 所有类型的定义
    - info.hpp 存储Ai用的全部数据Info

## cv::Mat 与 vector< vector< T > >

- **服务器**使用 cv::Mat，利用OpenCV辅助进行游戏逻辑。
- **参赛Ai**使用 vector<vector< T > > 类型。 


