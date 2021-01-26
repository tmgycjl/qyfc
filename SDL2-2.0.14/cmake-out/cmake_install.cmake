# Install script for directory: E:/GitHub/qyfc/SDL2-2.0.14

# Set the install prefix
if(NOT DEFINED CMAKE_INSTALL_PREFIX)
  set(CMAKE_INSTALL_PREFIX "C:/Program Files (x86)/SDL2")
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

# Is this installation the result of a crosscompile?
if(NOT DEFINED CMAKE_CROSSCOMPILING)
  set(CMAKE_CROSSCOMPILING "FALSE")
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xUnspecifiedx" OR NOT CMAKE_INSTALL_COMPONENT)
  if("${CMAKE_INSTALL_CONFIG_NAME}" MATCHES "^([Dd][Ee][Bb][Uu][Gg])$")
    file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/lib" TYPE STATIC_LIBRARY FILES "E:/GitHub/qyfc/SDL2-2.0.14/cmake-out/Debug/SDL2-staticd.lib")
  elseif("${CMAKE_INSTALL_CONFIG_NAME}" MATCHES "^([Rr][Ee][Ll][Ee][Aa][Ss][Ee])$")
    file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/lib" TYPE STATIC_LIBRARY FILES "E:/GitHub/qyfc/SDL2-2.0.14/cmake-out/Release/SDL2-static.lib")
  elseif("${CMAKE_INSTALL_CONFIG_NAME}" MATCHES "^([Mm][Ii][Nn][Ss][Ii][Zz][Ee][Rr][Ee][Ll])$")
    file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/lib" TYPE STATIC_LIBRARY FILES "E:/GitHub/qyfc/SDL2-2.0.14/cmake-out/MinSizeRel/SDL2-static.lib")
  elseif("${CMAKE_INSTALL_CONFIG_NAME}" MATCHES "^([Rr][Ee][Ll][Ww][Ii][Tt][Hh][Dd][Ee][Bb][Ii][Nn][Ff][Oo])$")
    file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/lib" TYPE STATIC_LIBRARY FILES "E:/GitHub/qyfc/SDL2-2.0.14/cmake-out/RelWithDebInfo/SDL2-static.lib")
  endif()
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xUnspecifiedx" OR NOT CMAKE_INSTALL_COMPONENT)
  if("${CMAKE_INSTALL_CONFIG_NAME}" MATCHES "^([Dd][Ee][Bb][Uu][Gg])$")
    file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/lib" TYPE STATIC_LIBRARY OPTIONAL FILES "E:/GitHub/qyfc/SDL2-2.0.14/cmake-out/Debug/SDL2d.lib")
  elseif("${CMAKE_INSTALL_CONFIG_NAME}" MATCHES "^([Rr][Ee][Ll][Ee][Aa][Ss][Ee])$")
    file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/lib" TYPE STATIC_LIBRARY OPTIONAL FILES "E:/GitHub/qyfc/SDL2-2.0.14/cmake-out/Release/SDL2.lib")
  elseif("${CMAKE_INSTALL_CONFIG_NAME}" MATCHES "^([Mm][Ii][Nn][Ss][Ii][Zz][Ee][Rr][Ee][Ll])$")
    file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/lib" TYPE STATIC_LIBRARY OPTIONAL FILES "E:/GitHub/qyfc/SDL2-2.0.14/cmake-out/MinSizeRel/SDL2.lib")
  elseif("${CMAKE_INSTALL_CONFIG_NAME}" MATCHES "^([Rr][Ee][Ll][Ww][Ii][Tt][Hh][Dd][Ee][Bb][Ii][Nn][Ff][Oo])$")
    file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/lib" TYPE STATIC_LIBRARY OPTIONAL FILES "E:/GitHub/qyfc/SDL2-2.0.14/cmake-out/RelWithDebInfo/SDL2.lib")
  endif()
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xUnspecifiedx" OR NOT CMAKE_INSTALL_COMPONENT)
  if("${CMAKE_INSTALL_CONFIG_NAME}" MATCHES "^([Dd][Ee][Bb][Uu][Gg])$")
    file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/bin" TYPE SHARED_LIBRARY FILES "E:/GitHub/qyfc/SDL2-2.0.14/cmake-out/Debug/SDL2d.dll")
  elseif("${CMAKE_INSTALL_CONFIG_NAME}" MATCHES "^([Rr][Ee][Ll][Ee][Aa][Ss][Ee])$")
    file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/bin" TYPE SHARED_LIBRARY FILES "E:/GitHub/qyfc/SDL2-2.0.14/cmake-out/Release/SDL2.dll")
  elseif("${CMAKE_INSTALL_CONFIG_NAME}" MATCHES "^([Mm][Ii][Nn][Ss][Ii][Zz][Ee][Rr][Ee][Ll])$")
    file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/bin" TYPE SHARED_LIBRARY FILES "E:/GitHub/qyfc/SDL2-2.0.14/cmake-out/MinSizeRel/SDL2.dll")
  elseif("${CMAKE_INSTALL_CONFIG_NAME}" MATCHES "^([Rr][Ee][Ll][Ww][Ii][Tt][Hh][Dd][Ee][Bb][Ii][Nn][Ff][Oo])$")
    file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/bin" TYPE SHARED_LIBRARY FILES "E:/GitHub/qyfc/SDL2-2.0.14/cmake-out/RelWithDebInfo/SDL2.dll")
  endif()
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xUnspecifiedx" OR NOT CMAKE_INSTALL_COMPONENT)
  if("${CMAKE_INSTALL_CONFIG_NAME}" MATCHES "^([Dd][Ee][Bb][Uu][Gg])$")
    file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/lib" TYPE STATIC_LIBRARY FILES "E:/GitHub/qyfc/SDL2-2.0.14/cmake-out/Debug/SDL2maind.lib")
  elseif("${CMAKE_INSTALL_CONFIG_NAME}" MATCHES "^([Rr][Ee][Ll][Ee][Aa][Ss][Ee])$")
    file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/lib" TYPE STATIC_LIBRARY FILES "E:/GitHub/qyfc/SDL2-2.0.14/cmake-out/Release/SDL2main.lib")
  elseif("${CMAKE_INSTALL_CONFIG_NAME}" MATCHES "^([Mm][Ii][Nn][Ss][Ii][Zz][Ee][Rr][Ee][Ll])$")
    file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/lib" TYPE STATIC_LIBRARY FILES "E:/GitHub/qyfc/SDL2-2.0.14/cmake-out/MinSizeRel/SDL2main.lib")
  elseif("${CMAKE_INSTALL_CONFIG_NAME}" MATCHES "^([Rr][Ee][Ll][Ww][Ii][Tt][Hh][Dd][Ee][Bb][Ii][Nn][Ff][Oo])$")
    file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/lib" TYPE STATIC_LIBRARY FILES "E:/GitHub/qyfc/SDL2-2.0.14/cmake-out/RelWithDebInfo/SDL2main.lib")
  endif()
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xUnspecifiedx" OR NOT CMAKE_INSTALL_COMPONENT)
  if(EXISTS "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/cmake/SDL2Targets.cmake")
    file(DIFFERENT EXPORT_FILE_CHANGED FILES
         "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/cmake/SDL2Targets.cmake"
         "E:/GitHub/qyfc/SDL2-2.0.14/cmake-out/CMakeFiles/Export/cmake/SDL2Targets.cmake")
    if(EXPORT_FILE_CHANGED)
      file(GLOB OLD_CONFIG_FILES "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/cmake/SDL2Targets-*.cmake")
      if(OLD_CONFIG_FILES)
        message(STATUS "Old export file \"$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/cmake/SDL2Targets.cmake\" will be replaced.  Removing files [${OLD_CONFIG_FILES}].")
        file(REMOVE ${OLD_CONFIG_FILES})
      endif()
    endif()
  endif()
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/cmake" TYPE FILE FILES "E:/GitHub/qyfc/SDL2-2.0.14/cmake-out/CMakeFiles/Export/cmake/SDL2Targets.cmake")
  if("${CMAKE_INSTALL_CONFIG_NAME}" MATCHES "^([Dd][Ee][Bb][Uu][Gg])$")
    file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/cmake" TYPE FILE FILES "E:/GitHub/qyfc/SDL2-2.0.14/cmake-out/CMakeFiles/Export/cmake/SDL2Targets-debug.cmake")
  endif()
  if("${CMAKE_INSTALL_CONFIG_NAME}" MATCHES "^([Mm][Ii][Nn][Ss][Ii][Zz][Ee][Rr][Ee][Ll])$")
    file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/cmake" TYPE FILE FILES "E:/GitHub/qyfc/SDL2-2.0.14/cmake-out/CMakeFiles/Export/cmake/SDL2Targets-minsizerel.cmake")
  endif()
  if("${CMAKE_INSTALL_CONFIG_NAME}" MATCHES "^([Rr][Ee][Ll][Ww][Ii][Tt][Hh][Dd][Ee][Bb][Ii][Nn][Ff][Oo])$")
    file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/cmake" TYPE FILE FILES "E:/GitHub/qyfc/SDL2-2.0.14/cmake-out/CMakeFiles/Export/cmake/SDL2Targets-relwithdebinfo.cmake")
  endif()
  if("${CMAKE_INSTALL_CONFIG_NAME}" MATCHES "^([Rr][Ee][Ll][Ee][Aa][Ss][Ee])$")
    file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/cmake" TYPE FILE FILES "E:/GitHub/qyfc/SDL2-2.0.14/cmake-out/CMakeFiles/Export/cmake/SDL2Targets-release.cmake")
  endif()
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xDevelx" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/cmake" TYPE FILE FILES
    "E:/GitHub/qyfc/SDL2-2.0.14/SDL2Config.cmake"
    "E:/GitHub/qyfc/SDL2-2.0.14/cmake-out/SDL2ConfigVersion.cmake"
    )
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xUnspecifiedx" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/include/SDL2" TYPE FILE FILES
    "E:/GitHub/qyfc/SDL2-2.0.14/include/SDL.h"
    "E:/GitHub/qyfc/SDL2-2.0.14/include/SDL_assert.h"
    "E:/GitHub/qyfc/SDL2-2.0.14/include/SDL_atomic.h"
    "E:/GitHub/qyfc/SDL2-2.0.14/include/SDL_audio.h"
    "E:/GitHub/qyfc/SDL2-2.0.14/include/SDL_bits.h"
    "E:/GitHub/qyfc/SDL2-2.0.14/include/SDL_blendmode.h"
    "E:/GitHub/qyfc/SDL2-2.0.14/include/SDL_clipboard.h"
    "E:/GitHub/qyfc/SDL2-2.0.14/include/SDL_config_android.h"
    "E:/GitHub/qyfc/SDL2-2.0.14/include/SDL_config_iphoneos.h"
    "E:/GitHub/qyfc/SDL2-2.0.14/include/SDL_config_macosx.h"
    "E:/GitHub/qyfc/SDL2-2.0.14/include/SDL_config_minimal.h"
    "E:/GitHub/qyfc/SDL2-2.0.14/include/SDL_config_os2.h"
    "E:/GitHub/qyfc/SDL2-2.0.14/include/SDL_config_pandora.h"
    "E:/GitHub/qyfc/SDL2-2.0.14/include/SDL_config_psp.h"
    "E:/GitHub/qyfc/SDL2-2.0.14/include/SDL_config_windows.h"
    "E:/GitHub/qyfc/SDL2-2.0.14/include/SDL_config_winrt.h"
    "E:/GitHub/qyfc/SDL2-2.0.14/include/SDL_config_wiz.h"
    "E:/GitHub/qyfc/SDL2-2.0.14/include/SDL_copying.h"
    "E:/GitHub/qyfc/SDL2-2.0.14/include/SDL_cpuinfo.h"
    "E:/GitHub/qyfc/SDL2-2.0.14/include/SDL_egl.h"
    "E:/GitHub/qyfc/SDL2-2.0.14/include/SDL_endian.h"
    "E:/GitHub/qyfc/SDL2-2.0.14/include/SDL_error.h"
    "E:/GitHub/qyfc/SDL2-2.0.14/include/SDL_events.h"
    "E:/GitHub/qyfc/SDL2-2.0.14/include/SDL_filesystem.h"
    "E:/GitHub/qyfc/SDL2-2.0.14/include/SDL_gamecontroller.h"
    "E:/GitHub/qyfc/SDL2-2.0.14/include/SDL_gesture.h"
    "E:/GitHub/qyfc/SDL2-2.0.14/include/SDL_haptic.h"
    "E:/GitHub/qyfc/SDL2-2.0.14/include/SDL_hints.h"
    "E:/GitHub/qyfc/SDL2-2.0.14/include/SDL_joystick.h"
    "E:/GitHub/qyfc/SDL2-2.0.14/include/SDL_keyboard.h"
    "E:/GitHub/qyfc/SDL2-2.0.14/include/SDL_keycode.h"
    "E:/GitHub/qyfc/SDL2-2.0.14/include/SDL_loadso.h"
    "E:/GitHub/qyfc/SDL2-2.0.14/include/SDL_locale.h"
    "E:/GitHub/qyfc/SDL2-2.0.14/include/SDL_log.h"
    "E:/GitHub/qyfc/SDL2-2.0.14/include/SDL_main.h"
    "E:/GitHub/qyfc/SDL2-2.0.14/include/SDL_messagebox.h"
    "E:/GitHub/qyfc/SDL2-2.0.14/include/SDL_metal.h"
    "E:/GitHub/qyfc/SDL2-2.0.14/include/SDL_misc.h"
    "E:/GitHub/qyfc/SDL2-2.0.14/include/SDL_mouse.h"
    "E:/GitHub/qyfc/SDL2-2.0.14/include/SDL_mutex.h"
    "E:/GitHub/qyfc/SDL2-2.0.14/include/SDL_name.h"
    "E:/GitHub/qyfc/SDL2-2.0.14/include/SDL_opengl.h"
    "E:/GitHub/qyfc/SDL2-2.0.14/include/SDL_opengl_glext.h"
    "E:/GitHub/qyfc/SDL2-2.0.14/include/SDL_opengles.h"
    "E:/GitHub/qyfc/SDL2-2.0.14/include/SDL_opengles2.h"
    "E:/GitHub/qyfc/SDL2-2.0.14/include/SDL_opengles2_gl2.h"
    "E:/GitHub/qyfc/SDL2-2.0.14/include/SDL_opengles2_gl2ext.h"
    "E:/GitHub/qyfc/SDL2-2.0.14/include/SDL_opengles2_gl2platform.h"
    "E:/GitHub/qyfc/SDL2-2.0.14/include/SDL_opengles2_khrplatform.h"
    "E:/GitHub/qyfc/SDL2-2.0.14/include/SDL_pixels.h"
    "E:/GitHub/qyfc/SDL2-2.0.14/include/SDL_platform.h"
    "E:/GitHub/qyfc/SDL2-2.0.14/include/SDL_power.h"
    "E:/GitHub/qyfc/SDL2-2.0.14/include/SDL_quit.h"
    "E:/GitHub/qyfc/SDL2-2.0.14/include/SDL_rect.h"
    "E:/GitHub/qyfc/SDL2-2.0.14/include/SDL_render.h"
    "E:/GitHub/qyfc/SDL2-2.0.14/include/SDL_revision.h"
    "E:/GitHub/qyfc/SDL2-2.0.14/include/SDL_rwops.h"
    "E:/GitHub/qyfc/SDL2-2.0.14/include/SDL_scancode.h"
    "E:/GitHub/qyfc/SDL2-2.0.14/include/SDL_sensor.h"
    "E:/GitHub/qyfc/SDL2-2.0.14/include/SDL_shape.h"
    "E:/GitHub/qyfc/SDL2-2.0.14/include/SDL_stdinc.h"
    "E:/GitHub/qyfc/SDL2-2.0.14/include/SDL_surface.h"
    "E:/GitHub/qyfc/SDL2-2.0.14/include/SDL_system.h"
    "E:/GitHub/qyfc/SDL2-2.0.14/include/SDL_syswm.h"
    "E:/GitHub/qyfc/SDL2-2.0.14/include/SDL_test.h"
    "E:/GitHub/qyfc/SDL2-2.0.14/include/SDL_test_assert.h"
    "E:/GitHub/qyfc/SDL2-2.0.14/include/SDL_test_common.h"
    "E:/GitHub/qyfc/SDL2-2.0.14/include/SDL_test_compare.h"
    "E:/GitHub/qyfc/SDL2-2.0.14/include/SDL_test_crc32.h"
    "E:/GitHub/qyfc/SDL2-2.0.14/include/SDL_test_font.h"
    "E:/GitHub/qyfc/SDL2-2.0.14/include/SDL_test_fuzzer.h"
    "E:/GitHub/qyfc/SDL2-2.0.14/include/SDL_test_harness.h"
    "E:/GitHub/qyfc/SDL2-2.0.14/include/SDL_test_images.h"
    "E:/GitHub/qyfc/SDL2-2.0.14/include/SDL_test_log.h"
    "E:/GitHub/qyfc/SDL2-2.0.14/include/SDL_test_md5.h"
    "E:/GitHub/qyfc/SDL2-2.0.14/include/SDL_test_memory.h"
    "E:/GitHub/qyfc/SDL2-2.0.14/include/SDL_test_random.h"
    "E:/GitHub/qyfc/SDL2-2.0.14/include/SDL_thread.h"
    "E:/GitHub/qyfc/SDL2-2.0.14/include/SDL_timer.h"
    "E:/GitHub/qyfc/SDL2-2.0.14/include/SDL_touch.h"
    "E:/GitHub/qyfc/SDL2-2.0.14/include/SDL_types.h"
    "E:/GitHub/qyfc/SDL2-2.0.14/include/SDL_version.h"
    "E:/GitHub/qyfc/SDL2-2.0.14/include/SDL_video.h"
    "E:/GitHub/qyfc/SDL2-2.0.14/include/SDL_vulkan.h"
    "E:/GitHub/qyfc/SDL2-2.0.14/include/begin_code.h"
    "E:/GitHub/qyfc/SDL2-2.0.14/include/close_code.h"
    "E:/GitHub/qyfc/SDL2-2.0.14/cmake-out/include/SDL_config.h"
    )
endif()

if(CMAKE_INSTALL_COMPONENT)
  set(CMAKE_INSTALL_MANIFEST "install_manifest_${CMAKE_INSTALL_COMPONENT}.txt")
else()
  set(CMAKE_INSTALL_MANIFEST "install_manifest.txt")
endif()

string(REPLACE ";" "\n" CMAKE_INSTALL_MANIFEST_CONTENT
       "${CMAKE_INSTALL_MANIFEST_FILES}")
file(WRITE "E:/GitHub/qyfc/SDL2-2.0.14/cmake-out/${CMAKE_INSTALL_MANIFEST}"
     "${CMAKE_INSTALL_MANIFEST_CONTENT}")
