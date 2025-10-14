# SunnyLandDemo

## 版权声明

本项目中的代码和资源均遵循 `CC0` 许可证。

本项目是[《SunnyLand》](https://github.com/WispSnow/SunnyLand)项目的学习代码`（其实就是抄了一遍啦）`，感谢原作者的分享和贡献。`（大大真的好厉害请收下我的膝盖!）`

原作者在Bilibili上也有相关视频教程，欢迎关注和观看：

- 原作者B站主页：[ZiyuGameDev](https://space.bilibili.com/3546810402474894)
- 原作者B站视频：[SunnyLand](https://www.bilibili.com/video/BV1u7NizLEBa)
- 原作者GitHub：[WispSnow/SunnyLand](https://github.com/WispSnow/SunnyLand)

## 项目介绍

SunnyLandDemo是一个基于SDL库的2D平台游戏项目，使用C++语言编写。项目包含一个简单的平台游戏场景，玩家角色可以通过键盘控制移动和跳跃，游戏场景中包含障碍物和敌人。

## 功能特点

- 简单的平台游戏场景
- 玩家角色控制
- 障碍物和敌人
- 键盘控制

## 使用方法

- 项目中的资源文件和代码文件已经整理好，请按照`编译方法`和`运行方法`进行编译和运行。

### 环境要求

- 操作系统：Windows
- 编译器：GCC / G++ / Clang
- 编译环境：VSCode/Visual Studio + CMake + vcpkg包管理器

### 编译方法

1. 克隆项目到本地
2. 确保你的vcpkg工具链已经安装并配置好以下库：
   - SDL3
   - SDL3_image
   - SDL3_ttf
   - glm
   - spdlog
   - nlohmann_json
3. 游戏在`CMakeLists.txt`中直接设置了vcpkg的路径为编写者的本地路径`（嘻嘻，其实是我懒）`，如果你使用路径和游戏的不一样，请自行修改`CMakeLists.txt`中的`set(VCPKG_ROOT_DIR "你的vcpkg路径")`。

## 联系方式

如有任何问题或建议，请不要联系我。`（因为我也不懂）`
