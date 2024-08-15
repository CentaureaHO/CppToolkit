## 简介
存放个人使用到的一些工具，会逐步将之前的代码整理到此处，  
每个项目目录下包含include，src，CMakeLists.txt与测试源文件四部分，  
include与src分别存放项目的头文件与源文件，  
CMakeLists.txt包含项目与测试程序的参数说明，  
测试源文件给出功能测试代码，同时作为用法示例。

---

## 项目结构  
CppToolKit/  
├─ cmake/  
│   ├─ CMakeLists.txt  
│   └─ bin/  
├─ Common/  
├─ DataStructure/  
├─ Network/  
├─ Parallel/  
├─ build.bat  
└─ build.sh  

其中，Common，DataStructure，Network，Parallel为项目的大致分类，
具体实现方法在文件夹内。

---

## 使用方法
build.bat与build.sh分别为Windows下与Linux下的构建脚本，  
build.bat的信息与功能可能不会及时补充，建议使用build.sh，  
使用build.sh help获取使用说明与完成的功能列表，  
使用build.sh -Dxxx=yyy Target进行编译，  
例如，使用build.sh -DUSE_RAW_POINTER=ON AnyTypeTest编译AnyType的测试程序，并选择使用裸指针存储，  
编译后的程序存储在cmake/bin目录下。  

---

## 已完成功能
### Common

- **AnyType**: 通过多态与动态内存分配，使用一种类型表示任意类型；目前尚未完善。  

### DataStructure

### Network

### Parallel

- **ThreadPool**: 基于PThread实现的线程池，支持任务的异步提交与同步等待。  
