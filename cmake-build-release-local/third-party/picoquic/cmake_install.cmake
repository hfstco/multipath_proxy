# Install script for directory: /tmp/tmp.K74dkG82Wf/third-party/picoquic

# Set the install prefix
if(NOT DEFINED CMAKE_INSTALL_PREFIX)
  set(CMAKE_INSTALL_PREFIX "/usr/local")
endif()
string(REGEX REPLACE "/$" "" CMAKE_INSTALL_PREFIX "${CMAKE_INSTALL_PREFIX}")

# Set the install configuration name.
if(NOT DEFINED CMAKE_INSTALL_CONFIG_NAME)
  if(BUILD_TYPE)
    string(REGEX REPLACE "^[^A-Za-z0-9_]+" ""
           CMAKE_INSTALL_CONFIG_NAME "${BUILD_TYPE}")
  else()
    set(CMAKE_INSTALL_CONFIG_NAME "Release")
  endif()
  message(STATUS "Install configuration: \"${CMAKE_INSTALL_CONFIG_NAME}\"")
endif()

# Set the component getting installed.
if(NOT CMAKE_INSTALL_COMPONENT)
  if(COMPONENT)
    message(STATUS "Install component: \"${COMPONENT}\"")
    set(CMAKE_INSTALL_COMPONENT "${COMPONENT}")
  else()
    set(CMAKE_INSTALL_COMPONENT)
  endif()
endif()

# Install shared libraries without execute permission?
if(NOT DEFINED CMAKE_INSTALL_SO_NO_EXE)
  set(CMAKE_INSTALL_SO_NO_EXE "1")
endif()

# Is this installation the result of a crosscompile?
if(NOT DEFINED CMAKE_CROSSCOMPILING)
  set(CMAKE_CROSSCOMPILING "FALSE")
endif()

# Set default install directory permissions.
if(NOT DEFINED CMAKE_OBJDUMP)
  set(CMAKE_OBJDUMP "/usr/bin/objdump")
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xUnspecifiedx" OR NOT CMAKE_INSTALL_COMPONENT)
  if(EXISTS "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/bin/picoquicdemo" AND
     NOT IS_SYMLINK "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/bin/picoquicdemo")
    file(RPATH_CHECK
         FILE "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/bin/picoquicdemo"
         RPATH "")
  endif()
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/bin" TYPE EXECUTABLE FILES "/tmp/tmp.K74dkG82Wf/cmake-build-release-local/third-party/picoquic/picoquicdemo")
  if(EXISTS "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/bin/picoquicdemo" AND
     NOT IS_SYMLINK "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/bin/picoquicdemo")
    file(RPATH_CHANGE
         FILE "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/bin/picoquicdemo"
         OLD_RPATH "/usr/lib/x86_64-linux-gnu/libssl.so:/usr/lib/x86_64-linux-gnu/libcrypto.so:"
         NEW_RPATH "")
    if(CMAKE_INSTALL_DO_STRIP)
      execute_process(COMMAND "/usr/bin/strip" "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/bin/picoquicdemo")
    endif()
  endif()
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xUnspecifiedx" OR NOT CMAKE_INSTALL_COMPONENT)
  if(EXISTS "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/bin/picolog_t" AND
     NOT IS_SYMLINK "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/bin/picolog_t")
    file(RPATH_CHECK
         FILE "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/bin/picolog_t"
         RPATH "")
  endif()
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/bin" TYPE EXECUTABLE FILES "/tmp/tmp.K74dkG82Wf/cmake-build-release-local/third-party/picoquic/picolog_t")
  if(EXISTS "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/bin/picolog_t" AND
     NOT IS_SYMLINK "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/bin/picolog_t")
    file(RPATH_CHANGE
         FILE "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/bin/picolog_t"
         OLD_RPATH "/usr/lib/x86_64-linux-gnu/libssl.so:/usr/lib/x86_64-linux-gnu/libcrypto.so:"
         NEW_RPATH "")
    if(CMAKE_INSTALL_DO_STRIP)
      execute_process(COMMAND "/usr/bin/strip" "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/bin/picolog_t")
    endif()
  endif()
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xUnspecifiedx" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/lib" TYPE STATIC_LIBRARY FILES "/tmp/tmp.K74dkG82Wf/cmake-build-release-local/third-party/picoquic/libpicoquic-core.a")
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xUnspecifiedx" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/lib" TYPE STATIC_LIBRARY FILES "/tmp/tmp.K74dkG82Wf/cmake-build-release-local/third-party/picoquic/libpicoquic-log.a")
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xUnspecifiedx" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/lib" TYPE STATIC_LIBRARY FILES "/tmp/tmp.K74dkG82Wf/cmake-build-release-local/third-party/picoquic/libpicohttp-core.a")
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xUnspecifiedx" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/include" TYPE FILE FILES
    "/tmp/tmp.K74dkG82Wf/third-party/picoquic/picoquic/picoquic.h"
    "/tmp/tmp.K74dkG82Wf/third-party/picoquic/picoquic/picosocks.h"
    "/tmp/tmp.K74dkG82Wf/third-party/picoquic/picoquic/picoquic_utils.h"
    "/tmp/tmp.K74dkG82Wf/third-party/picoquic/picoquic/picoquic_packet_loop.h"
    "/tmp/tmp.K74dkG82Wf/third-party/picoquic/picoquic/picoquic_unified_log.h"
    "/tmp/tmp.K74dkG82Wf/third-party/picoquic/picoquic/picoquic_logger.h"
    "/tmp/tmp.K74dkG82Wf/third-party/picoquic/picoquic/picoquic_binlog.h"
    "/tmp/tmp.K74dkG82Wf/third-party/picoquic/picoquic/picoquic_config.h"
    "/tmp/tmp.K74dkG82Wf/third-party/picoquic/picoquic/picoquic_lb.h"
    "/tmp/tmp.K74dkG82Wf/third-party/picoquic/loglib/autoqlog.h"
    "/tmp/tmp.K74dkG82Wf/third-party/picoquic/picohttp/h3zero.h"
    "/tmp/tmp.K74dkG82Wf/third-party/picoquic/picohttp/h3zero_common.h"
    "/tmp/tmp.K74dkG82Wf/third-party/picoquic/picohttp/h3zero_uri.h"
    "/tmp/tmp.K74dkG82Wf/third-party/picoquic/picohttp/democlient.h"
    "/tmp/tmp.K74dkG82Wf/third-party/picoquic/picohttp/demoserver.h"
    "/tmp/tmp.K74dkG82Wf/third-party/picoquic/picohttp/pico_webtransport.h"
    "/tmp/tmp.K74dkG82Wf/third-party/picoquic/picohttp/wt_baton.h"
    )
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for each subdirectory.
  include("/tmp/tmp.K74dkG82Wf/cmake-build-release-local/_deps/picotls-build/cmake_install.cmake")

endif()

