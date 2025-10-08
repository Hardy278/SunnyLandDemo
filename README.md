# SunnyLandDemo

## 项目介绍

SunnyLandDemo是一个基于SDL库的2D平台游戏项目，使用C++语言编写。项目包含一个简单的平台游戏场景，玩家角色可以通过键盘控制移动和跳跃，游戏场景中包含障碍物和敌人。

## 功能特点

- 简单的平台游戏场景
- 玩家角色控制
- 障碍物和敌人
- 键盘控制

## 使用方法

- 打包的版本我会另外再发，请自行下载
- 项目中的资源文件和代码文件已经整理好，请按照`编译方法`和`运行方法`进行编译和运行。

### 环境要求

- 操作系统：Windows / Linux / MacOS
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

### 运行方法

1. 在VSCode或者Visual Studio中打开项目文件夹
2. 使用CMake生成项目
3. 编译并运行项目

## 联系方式

如有任何问题或建议，请通过以下方式联系我：

- QQ私我即可，我是抄的轮子请不要80我呜呜呜

## 版权声明

本项目中的代码和资源均遵循 `CC0` 许可证。请确保在使用本项目中的代码和资源时遵守相应的许可证条款。

虽然是`CC0`许可证，但是我还是要提一嘴原作者，本项目是基于[《SunnyLand》](https://github.com/WispSnow/SunnyLand)项目进行修改和扩展的，感谢原作者的分享和贡献。`（大大真的好厉害请收下我的膝盖!）`

原作者在Bilibili上也有相关视频教程，欢迎关注和观看：

- 原作者B站主页：[ZiyuGameDev](https://space.bilibili.com/3546810402474894)
- 原作者B站视频：[SunnyLand](https://www.bilibili.com/video/BV1u7NizLEBa)
