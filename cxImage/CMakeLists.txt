project(nnn_cximage)
cmake_minimum_required(VERSION 2.8)

#必要的源文件
aux_source_directory(. SRC_LIST)
aux_source_directory(tools/CxImage SRC_LIST)
aux_source_directory(tools/jpeg SRC_LIST)
aux_source_directory(tools/png SRC_LIST)
#aux_source_directory(tools/tiff SRC_LIST)
aux_source_directory(tools/zlib SRC_LIST)
#aux_source_directory(tools/jasper/base SRC_LIST)
#aux_source_directory(tools/jasper/bmp SRC_LIST)
#aux_source_directory(tools/jasper/jp2 SRC_LIST)
#aux_source_directory(tools/jasper/jpc SRC_LIST)
#aux_source_directory(tools/jasper/jpg SRC_LIST)
#aux_source_directory(tools/jasper/mif SRC_LIST)
#aux_source_directory(tools/jasper/pgx SRC_LIST)
#aux_source_directory(tools/jasper/pnm SRC_LIST)
#aux_source_directory(tools/jasper/ras SRC_LIST)

##添加可执行文件
#add_executable(${PROJECT_NAME} ${SRC_LIST})
##add_library(${PROJECT_NAME} SHARED ${SRC_LIST} )

#查找头文件

include_directories("tools/CxImage")
include_directories("tools/jpeg")
include_directories("tools/png")
include_directories("tools/tiff ")
include_directories("tools/zlib")
include_directories("tools/jasper/include")

aux_source_directory(. SRC_LIST)
add_executable(${PROJECT_NAME} ${SRC_LIST})
