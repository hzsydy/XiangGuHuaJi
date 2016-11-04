# 2016年自动化系香菇滑稽大赛 工程文件介绍

- WorldEditor：地图编辑器。
  - 资瓷使用xls导入和手点两种方式画地图
  - 以后可以改造成界面

- TestAi：样例AI格式
  - 用于测试功能实现，并且也就是发给小朋友的模板

- XiangGuHuaJi：主逻辑
  - definition.h：定义了需要用到的各种常量
  - map.h：载入和解析地图
  - ai.h：AI的头文件
  - player.h：调用AI的类


  - game.h：模型，也就是游戏本体
  - debug.hpp：本人（Du）手撸的一个小调试工具，可以格式化的打印二维向量到命令行（本人对此非常自豪）

- HuaJiCmd：命令行

- HuaJiClient：联网版