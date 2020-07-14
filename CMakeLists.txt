#检查cmake的版本 至少是2.8.3版本
CMAKE_MINIMUM_REQUIRED(VERSION 2.8.3)

#输出消息
MESSAGE("---------------------C++11线程－－－－－－－－--------------------------")

#工程名字
SET(objname "xthread")
SET(entryName "xthread")
#放在同级目录
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_DEBUG ${CMAKE_CURRENT_SOURCE_DIR}/bin) 
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_RELEASE ${CMAKE_CURRENT_SOURCE_DIR}/bin)

set(PATH ${PROJECT_SOURCE_DIR})

MESSAGE("--------------------正在生成" ${objname} "------------------------------")
PROJECT(${objname})

#c++11支持
SET(CMAKE_CXX_FLAGS "-std=c++11" )
ADD_DEFINITIONS(-std=c++11)
add_compile_options(-std=c++11)

#条件变量
ADD_SUBDIRECTORY(condvar)
#死锁
ADD_SUBDIRECTORY(deadlock)
#future promise async
ADD_SUBDIRECTORY(fpasync)
#线程管理
ADD_SUBDIRECTORY(manager)
#packaged_task
ADD_SUBDIRECTORY(packagedtask)
#线程总览
ADD_SUBDIRECTORY(pandect)
#时间控制
ADD_SUBDIRECTORY(timecontr)
#用户可调用对象
ADD_SUBDIRECTORY(uadjustfun)
#unique_lock
ADD_SUBDIRECTORY(ulocklinit)
#结束提示
MESSAGE("执行命令结束")


