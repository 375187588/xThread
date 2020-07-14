#检查cmake的版本 至少是2.8.3版本
CMAKE_MINIMUM_REQUIRED(VERSION 2.8.3)

#输出消息
MESSAGE("---------------------C++线程unique_lock等范例----------------")

#工程名字
SET(entryName "ulocklinit")
#放在同级目录
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_DEBUG ${CMAKE_CURRENT_SOURCE_DIR}/../../bin) 
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_RELEASE ${CMAKE_CURRENT_SOURCE_DIR}/../../bin)

MESSAGE("--------------------正在生成" ${entryName} "-----------------")
#PROJECT(${entryName})

#c++11支持
SET(CMAKE_CXX_FLAGS "-std=c++11" )
ADD_DEFINITIONS(-std=c++11)
add_compile_options(-std=c++11)

AUX_SOURCE_DIRECTORY(. REG_SRC)


MESSAGE(${CMAKE_CURRENT_SOURCE_DIR})	
	   
ADD_EXECUTABLE(${entryName} ${REG_SRC})

#ADD_CUSTOM_COMMAND(TARGET Strategy POST_BUILD 
#COMMAND ${CMAKE_COMMAND} -E del /F /S /Q ${CMAKE_CURRENT_SOURCE_DIR}/Debug 
#COMMAND ${CMAKE_COMMAND} -E del /F /S /Q ${CMAKE_CURRENT_SOURCE_DIR}/${entryName}.dir 
#COMMAND ${CMAKE_COMMAND} -E del /F /S /Q ${CMAKE_CURRENT_SOURCE_DIR}/CMakeFiles)

#ADD_CUSTOM_COMMAND(TARGET ${entryName} ${REG_SRC} PRE_BUILD 
#COMMAND echo "开始删除生成的文件和目录" 
#COMMAND ${CMAKE_COMMAND} -E del /F /S /Q ${CMAKE_CURRENT_SOURCE_DIR}/Debug
#COMMAND ${CMAKE_COMMAND} -E del /F /S /Q ${CMAKE_CURRENT_SOURCE_DIR}/${entryName}.dir
#COMMAND ${CMAKE_COMMAND} -E del /F /S /Q ${CMAKE_CURRENT_SOURCE_DIR}/CMakeFiles
#COMMENT "结束删除生成的文件和目录")

MESSAGE("执行命令结束")


