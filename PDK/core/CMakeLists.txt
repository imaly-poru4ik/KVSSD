cmake_minimum_required(VERSION 2.8.9)
project (libkvapi C CXX)

option(WITH_EMU "Enable Emulator support" OFF)
option(WITH_KDD "Enable Kernel Driver support" OFF)
option(WITH_SPDK "Enable SPDK Driver support" OFF)

set(CMAKE_C_FLAGS "-MMD -MP -Wall -DLINUX -D_FILE_OFFSET_BITS=64 -fPIC  -march=native")
set(CMAKE_CXX_FLAGS "-O2 -g -std=c++11 -MMD -MP -Wall -DLINUX -D_FILE_OFFSET_BITS=64 -fPIC  -march=native")

set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ./deps)

include_directories (include)
include_directories (${CMAKE_CURRENT_SOURCE_DIR}/src/device_abstract_layer/include)
include_directories (${CMAKE_CURRENT_SOURCE_DIR}/src/api/include/udd)

if(WITH_KDD)

set(kvkdd_CFLAGS "-MMD -MP -Wall -DLINUX -D_FILE_OFFSET_BITS=64 -fPIC  -march=native")
set(kvkdd_CXXFLAGS "-O2 -g -std=c++11 -MMD -MP -Wall -DLINUX -D_FILE_OFFSET_BITS=64 -fPIC  -march=native")

include_directories (${CMAKE_CURRENT_SOURCE_DIR}/src/device_abstract_layer/kernel_driver_adapter/include)

# SOURCE CODE
SET(SOURCES_KDD
    ${CMAKE_CURRENT_SOURCE_DIR}/src/device_abstract_layer/kernel_driver_adapter/src/io_cmd.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/device_abstract_layer/kernel_driver_adapter/src/kv_device.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/device_abstract_layer/kernel_driver_adapter/src/kv_namespace.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/device_abstract_layer/kernel_driver_adapter/src/kvs_adi.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/device_abstract_layer/kernel_driver_adapter/src/queue.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/device_abstract_layer/kernel_driver_adapter/src/kv_linux_kernel.cpp
)

SET(HEADERS_KDD
    ${CMAKE_CURRENT_SOURCE_DIR}/src/device_abstract_layer/include/kvs_adi.h
    ${CMAKE_CURRENT_SOURCE_DIR}/src/device_abstract_layer/include/private/kvs_adi_internal.h
    ${CMAKE_CURRENT_SOURCE_DIR}/src/device_abstract_layer/include/private/io_cmd.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/device_abstract_layer/include/private/kv_device.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/device_abstract_layer/include/private/kv_namespace.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/device_abstract_layer/include/private/kvs_utils.h
    ${CMAKE_CURRENT_SOURCE_DIR}/src/device_abstract_layer/include/private/queue.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/device_abstract_layer/kernel_driver_adapter/include/kv_linux_kernel.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/device_abstract_layer/kernel_driver_adapter/include/linux_nvme_ioctl.h
)


#
# library build rules (shared)
#
add_definitions(-DWITH_KDD)
add_library(kvkdd_static STATIC ${SOURCES_KDD} ${HEADERS_KDD})
add_library(kvkdd SHARED ${SOURCES_KDD} ${HEADERS_KDD})
target_include_directories(kvkdd PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/src/device_abstract_layer/include/private)
target_include_directories(kvkdd_static PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/src/device_abstract_layer/include/private)
set_target_properties(kvkdd PROPERTIES
  LIBRARY_OUTPUT_DIRECTORY ./deps)

set(KVADI_LIBS -L${CMAKE_CURRENT_SOURCE_DIR}/build/deps -Wl,--no-as-needed -lkvkdd_static)

#
# kv api
#
SET(SOURCES_API
  ${SOURCES_API}
  ${CMAKE_CURRENT_SOURCE_DIR}/src/api/src/driver_adapter/kvemuldriver.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/src/api/src/driver_adapter/kvkdd.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/src/api/src/cfrontend.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/src/api/src/kvsdevice.cpp
  )

include_directories (${CMAKE_CURRENT_SOURCE_DIR}/src/api/include)
include_directories (${CMAKE_CURRENT_SOURCE_DIR}/src/api/include/private)
set(KVAPI_CXXFLAGS "-O2 -std=c++11 -MMD -MP -Wall -D_FILE_OFFSET_BITS=64 -fPIC -fpermissive -march=native")
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${KVAPI_CFLAGS}")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${KVAPI_CXXFLAGS}")
set(KVAPI_LIBS ${KVAPI_LIBS} -pthread -Wl,--no-as-needed -fPIC -lnuma -lrt ${KVADI_LIBS} -march=native)


add_library(kvapi SHARED ${SOURCES_API} ${HEADERS_API})
target_include_directories(kvapi PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/src/api/include/private)
target_link_libraries (kvapi ${KVAPI_LIBS} kvkdd_static)

add_executable(sample_code_async ${CMAKE_CURRENT_SOURCE_DIR}/sample_code/test_async.cpp ${SOURCES_API} ${HEADERS_API})
target_link_libraries(sample_code_async ${KVAPI_LIBS})
add_dependencies(sample_code_async kvapi)

add_executable(sample_code_sync ${CMAKE_CURRENT_SOURCE_DIR}/sample_code/test_sync.cpp ${SOURCES_API} ${HEADERS_API})
target_link_libraries(sample_code_sync ${KVAPI_LIBS})
add_dependencies(sample_code_sync kvapi)


elseif(WITH_EMU)

set(kvemul_CFLAGS "-MMD -MP -Wall -DLINUX -D_FILE_OFFSET_BITS=64 -fPIC  -march=native")
set(kvemul_CXXFLAGS "-O2 -g -std=c++11 -MMD -MP -Wall -DLINUX -D_FILE_OFFSET_BITS=64 -fPIC  -march=native")
 
# SOURCE CODE
SET(SOURCES_EMU
    ${CMAKE_CURRENT_SOURCE_DIR}/src/device_abstract_layer/emulator/src/io_cmd.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/device_abstract_layer/emulator/src/kv_config.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/device_abstract_layer/emulator/src/kv_device.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/device_abstract_layer/emulator/src/kv_namespace.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/device_abstract_layer/emulator/src/kv_emulator.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/device_abstract_layer/emulator/src/kvs_adi.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/device_abstract_layer/emulator/src/thread_pool.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/device_abstract_layer/emulator/src/queue.cpp
)

SET(HEADERS_EMU
    ${CMAKE_CURRENT_SOURCE_DIR}/src/device_abstract_layer/include/kvs_adi.h
    ${CMAKE_CURRENT_SOURCE_DIR}/src/device_abstract_layer/include/private/kvs_adi_internal.h
    ${CMAKE_CURRENT_SOURCE_DIR}/src/device_abstract_layer/include/private/io_cmd.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/device_abstract_layer/include/private/history.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/device_abstract_layer/include/private/kv_config.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/device_abstract_layer/include/private/kv_device.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/device_abstract_layer/include/private/kv_namespace.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/device_abstract_layer/include/private/kv_emulator.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/device_abstract_layer/include/private/kvs_utils.h
    ${CMAKE_CURRENT_SOURCE_DIR}/src/device_abstract_layer/include/private/queue.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/device_abstract_layer/include/private/thread_pool.hpp
)

#
# emu library build rules (shared)
#
add_definitions(-DWITH_EMU)
add_library(kvemul_static STATIC ${SOURCES_EMU} ${HEADERS_EMU})
add_library(kvemul SHARED ${SOURCES_EMU} ${HEADERS_EMU})
target_include_directories(kvemul PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/src/device_abstract_layer/include/private)
target_include_directories(kvemul_static PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/src/device_abstract_layer/include/private) 
set_target_properties(kvemul PROPERTIES
  LIBRARY_OUTPUT_DIRECTORY ./deps)

set(KVADI_LIBS -L${CMAKE_CURRENT_SOURCE_DIR}/build/deps -Wl,--no-as-needed -lkvemul_static)


#
# kv api
#
SET(SOURCES_API
  ${SOURCES_API}
  ${CMAKE_CURRENT_SOURCE_DIR}/src/api/src/driver_adapter/kvemuldriver.cpp 
  ${CMAKE_CURRENT_SOURCE_DIR}/src/api/src/driver_adapter/kvkdd.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/src/api/src/cfrontend.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/src/api/src/kvsdevice.cpp
  )

include_directories (${CMAKE_CURRENT_SOURCE_DIR}/src/api/include)
include_directories (${CMAKE_CURRENT_SOURCE_DIR}/src/api/include/private)
set(KVAPI_CXXFLAGS "-O2 -std=c++11 -MMD -MP -Wall -D_FILE_OFFSET_BITS=64 -fPIC -fpermissive -march=native")
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${KVAPI_CFLAGS}")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${KVAPI_CXXFLAGS}")
set(KVAPI_LIBS ${KVAPI_LIBS} -pthread -Wl,--no-as-needed -fPIC -lnuma -lrt ${KVADI_LIBS} -march=native)


add_library(kvapi SHARED ${SOURCES_API} ${HEADERS_API})
target_include_directories(kvapi PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/src/api/include/private)
target_link_libraries (kvapi ${KVAPI_LIBS} kvemul_static)

add_executable(sample_code_async ${CMAKE_CURRENT_SOURCE_DIR}/sample_code/test_async.cpp ${SOURCES_API} ${HEADERS_API})
target_link_libraries(sample_code_async ${KVAPI_LIBS})
add_dependencies(sample_code_async kvapi)

add_executable(sample_code_sync ${CMAKE_CURRENT_SOURCE_DIR}/sample_code/test_sync.cpp ${SOURCES_API} ${HEADERS_API})
target_link_libraries(sample_code_sync ${KVAPI_LIBS})
add_dependencies(sample_code_sync kvapi)

elseif(WITH_SPDK)
  include_directories (${CMAKE_CURRENT_SOURCE_DIR}/src/api/include)
  include_directories (${CMAKE_CURRENT_SOURCE_DIR}/src/api/include/private)
  
  set(KVKUDD_LIBS -L${CMAKE_CURRENT_SOURCE_DIR}/lib -Wl,--no-as-needed -lkvnvmedd -ldl)
  SET(SOURCES_API
    ${SOURCES_API}
    ${CMAKE_CURRENT_SOURCE_DIR}/src/api/src/uddenv.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/api/src/driver_adapter/kvudd.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/api/src/cfrontend.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/api/src/kvsdevice.cpp
    )
  set(KVAPI_LIBS ${KVAPI_LIBS} ${KVKUDD_LIBS} -lrt)
  add_definitions(-DWITH_SPDK)
  message("SPDK is enabled")

  set(KVAPI_CXXFLAGS "-O2 -std=c++11 -MMD -MP -Wall -D_FILE_OFFSET_BITS=64 -fPIC -fpermissive -march=native")
  set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${KVAPI_CFLAGS}")
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${KVAPI_CXXFLAGS}")
  set(KVAPI_LIBS ${KVAPI_LIBS} -pthread -Wl,--no-as-needed -fPIC -lnuma -lrt ${KVADI_LIBS} -march=native)

  add_library(kvapi SHARED ${SOURCES_API} ${HEADERS_API})
  target_include_directories(kvapi PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/src/api/include/private)
  target_link_libraries (kvapi ${KVAPI_LIBS})

  add_executable(sample_code_async ${CMAKE_CURRENT_SOURCE_DIR}/sample_code/test_async.cpp ${SOURCES_API} ${HEADERS_API})
  target_link_libraries(sample_code_async ${KVAPI_LIBS})
  add_dependencies(sample_code_async kvapi)

  add_executable(sample_code_sync ${CMAKE_CURRENT_SOURCE_DIR}/sample_code/test_sync.cpp ${SOURCES_API} ${HEADERS_API})
  target_link_libraries(sample_code_sync ${KVAPI_LIBS})
  add_dependencies(sample_code_sync kvapi)
  
else()
  message( FATAL_ERROR "Please specify device driver type for compilation." )

endif()


if (0)
add_executable(sample_code_sync ${CMAKE_CURRENT_SOURCE_DIR}/sample_code/test_sync.cpp ${SOURCES_API} ${HEADERS_API})
target_link_libraries(sample_code_sync ${KVAPI_LIBS})
add_dependencies(sample_code_sync kvapi)
endif()
