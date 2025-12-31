cmake_minimum_required(VERSION 3.15)
project(SROS C CXX ASM)

# Set C/C++ standards
set(CMAKE_C_STANDARD 99)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_C_FLAGS_RELEASE "-O2 -fPIC -m64")
set(CMAKE_CXX_FLAGS_RELEASE "-O2 -fPIC -m64")

# Architecture detection
if(NOT DEFINED PLATFORM)
    set(PLATFORM "x86_64")
endif()

message(STATUS "SROS Build System - Platform: ${PLATFORM}")
message(STATUS "Build Type: ${CMAKE_BUILD_TYPE}")

# Symbian source root
set(SYMBIAN_ROOT "${CMAKE_CURRENT_SOURCE_DIR}/symbian-os")

# Find packages
find_package(PkgConfig)
find_program(MAKE_EXECUTABLE make)

if(NOT MAKE_EXECUTABLE)
    message(WARNING "make not found - some components may not compile")
endif()

# Create build targets directory
set(BUILD_OUTPUT_DIR "${CMAKE_BINARY_DIR}/../x86_64/artifacts")
file(MAKE_DIRECTORY "${BUILD_OUTPUT_DIR}/kernel")
file(MAKE_DIRECTORY "${BUILD_OUTPUT_DIR}/drivers")
file(MAKE_DIRECTORY "${BUILD_OUTPUT_DIR}/system")

# ========================================
# KERNEL BUILD TARGET
# ========================================
if(EXISTS "${SYMBIAN_ROOT}/kernel")
    message(STATUS "Adding kernel target...")
    
    # Kernel source files
    file(GLOB_RECURSE KERNEL_C_SOURCES
        "${SYMBIAN_ROOT}/kernel/eka/kernel/*.c"
        "${SYMBIAN_ROOT}/kernel/eka/nanokernel/*.c"
        "${SYMBIAN_ROOT}/kernel/hal/src/*.c"
        "${SYMBIAN_ROOT}/kernel/hal/src/hal*.c"
    )
    
    file(GLOB_RECURSE KERNEL_CPP_SOURCES
        "${SYMBIAN_ROOT}/kernel/eka/kernel/*.cpp"
        "${SYMBIAN_ROOT}/kernel/eka/nanokernel/*.cpp"
    )
    
    set(KERNEL_SOURCES ${KERNEL_C_SOURCES} ${KERNEL_CPP_SOURCES})
    
    if(KERNEL_SOURCES)
        list(LENGTH KERNEL_SOURCES KERNEL_COUNT)
        message(STATUS "  Found ${KERNEL_COUNT} kernel source files")
        
        add_library(kernel_obj OBJECT ${KERNEL_SOURCES})
        target_include_directories(kernel_obj PUBLIC
            "${SYMBIAN_ROOT}/kernel/eka/include"
            "${SYMBIAN_ROOT}/kernel/eka/nanokernel"
            "${SYMBIAN_ROOT}/kernel/hal/include"
            "${SYMBIAN_ROOT}/kernel/hal/include/platform"
            "${SYMBIAN_ROOT}/kernel/memmodel/epoc"
        )
        set_target_properties(kernel_obj PROPERTIES
            COMPILE_FLAGS "-O2 -fPIC -m64 -Wno-deprecated-declarations"
        )
    else()
        message(STATUS "  No kernel sources found, creating stub")
        file(WRITE "${BUILD_OUTPUT_DIR}/kernel/kernel.bin" "SROS_KERNEL_STUB")
    endif()
else()
    message(WARNING "kernel directory not found")
endif()

# ========================================
# GRAPHICS BUILD TARGET
# ========================================
if(EXISTS "${SYMBIAN_ROOT}/graphics")
    message(STATUS "Adding graphics target...")
    
    file(GLOB_RECURSE GRAPHICS_SOURCES
        "${SYMBIAN_ROOT}/graphics/graphicsdeviceinterface/gdi/src/*.cpp"
        "${SYMBIAN_ROOT}/graphics/graphicsdeviceinterface/gdi/src/*.c"
        "${SYMBIAN_ROOT}/graphics/windowing/windowserver/src/*.cpp"
        "${SYMBIAN_ROOT}/graphics/windowing/windowserver/src/*.c"
    )
    
    if(GRAPHICS_SOURCES)
        list(LENGTH GRAPHICS_SOURCES GRAPHICS_COUNT)
        message(STATUS "  Found ${GRAPHICS_COUNT} graphics source files")
        
        add_library(graphics_obj OBJECT ${GRAPHICS_SOURCES})
        target_include_directories(graphics_obj PUBLIC
            "${SYMBIAN_ROOT}/graphics/graphicsdeviceinterface/gdi/include"
            "${SYMBIAN_ROOT}/graphics/windowing/windowserver/inc"
            "${SYMBIAN_ROOT}/graphics/opengl/include"
            "${SYMBIAN_ROOT}/kernel/eka/include"
        )
        set_target_properties(graphics_obj PROPERTIES
            COMPILE_FLAGS "-O2 -fPIC -m64 -Wno-deprecated-declarations"
        )
    else()
        message(STATUS "  No graphics sources found, creating stub")
        file(WRITE "${BUILD_OUTPUT_DIR}/drivers/graphics.so" "SROS_GRAPHICS_STUB")
    endif()
else()
    message(WARNING "graphics directory not found")
endif()

# ========================================
# DEVICE SERVICES BUILD TARGET
# ========================================
if(EXISTS "${SYMBIAN_ROOT}/devicesrv")
    message(STATUS "Adding device services target...")
    
    file(GLOB_RECURSE DEVICESRV_SOURCES
        "${SYMBIAN_ROOT}/devicesrv/os/devicesrv/src/*.cpp"
        "${SYMBIAN_ROOT}/devicesrv/os/devicesrv/src/*.c"
        "${SYMBIAN_ROOT}/devicesrv/os/devicesrv/hal/*.cpp"
    )
    
    if(DEVICESRV_SOURCES)
        list(LENGTH DEVICESRV_SOURCES DEVICESRV_COUNT)
        message(STATUS "  Found ${DEVICESRV_COUNT} device services source files")
        
        add_library(devicesrv_obj OBJECT ${DEVICESRV_SOURCES})
        target_include_directories(devicesrv_obj PUBLIC
            "${SYMBIAN_ROOT}/devicesrv/os/devicesrv/inc"
            "${SYMBIAN_ROOT}/devicesrv/os/devicesrv/hal/inc"
            "${SYMBIAN_ROOT}/kernel/eka/include"
        )
        set_target_properties(devicesrv_obj PROPERTIES
            COMPILE_FLAGS "-O2 -fPIC -m64 -Wno-deprecated-declarations"
        )
    else()
        message(STATUS "  No device services sources found, creating stub")
        file(WRITE "${BUILD_OUTPUT_DIR}/drivers/devicesrv.so" "SROS_DEVICESRV_STUB")
    endif()
else()
    message(WARNING "devicesrv directory not found")
endif()

# ========================================
# USER LIBRARY BUILD TARGET
# ========================================
if(EXISTS "${SYMBIAN_ROOT}/userlib")
    message(STATUS "Adding user library target...")
    
    file(GLOB_RECURSE USERLIB_SOURCES
        "${SYMBIAN_ROOT}/userlib/*.cpp"
        "${SYMBIAN_ROOT}/userlib/*.c"
    )
    
    if(USERLIB_SOURCES)
        list(LENGTH USERLIB_SOURCES USERLIB_COUNT)
        message(STATUS "  Found ${USERLIB_COUNT} user library source files")
        
        add_library(userlib_obj OBJECT ${USERLIB_SOURCES})
        target_include_directories(userlib_obj PUBLIC
            "${SYMBIAN_ROOT}/userlib/inc"
            "${SYMBIAN_ROOT}/kernel/eka/include"
        )
        set_target_properties(userlib_obj PROPERTIES
            COMPILE_FLAGS "-O2 -fPIC -m64 -Wno-deprecated-declarations"
        )
    else()
        message(STATUS "  No user library sources found, creating stub")
        file(WRITE "${BUILD_OUTPUT_DIR}/drivers/userlib.so" "SROS_USERLIB_STUB")
    endif()
else()
    message(WARNING "userlib directory not found")
endif()

# ========================================
# S60 FRAMEWORK BUILD TARGET
# ========================================
if(EXISTS "${SYMBIAN_ROOT}/s60")
    message(STATUS "Adding S60 framework target...")
    
    file(GLOB_RECURSE S60_SOURCES
        "${SYMBIAN_ROOT}/s60/src/*.cpp"
        "${SYMBIAN_ROOT}/s60/src/*.c"
    )
    
    if(S60_SOURCES)
        list(LENGTH S60_SOURCES S60_COUNT)
        message(STATUS "  Found ${S60_COUNT} S60 source files")
        
        add_library(s60_obj OBJECT ${S60_SOURCES})
        target_include_directories(s60_obj PUBLIC
            "${SYMBIAN_ROOT}/s60/inc"
            "${SYMBIAN_ROOT}/s60/include"
            "${SYMBIAN_ROOT}/kernel/eka/include"
            "${SYMBIAN_ROOT}/graphics/windowing/windowserver/inc"
        )
        set_target_properties(s60_obj PROPERTIES
            COMPILE_FLAGS "-O2 -fPIC -m64 -Wno-deprecated-declarations"
        )
    else()
        message(STATUS "  No S60 sources found, creating stub")
        file(WRITE "${BUILD_OUTPUT_DIR}/drivers/s60.so" "SROS_S60_STUB")
    endif()
else()
    message(WARNING "s60 directory not found")
endif()

# ========================================
# BUILD SUMMARY
# ========================================
message(STATUS "")
message(STATUS "=== SROS Build Configuration ===")
message(STATUS "Platform:       ${PLATFORM}")
message(STATUS "Build Type:     ${CMAKE_BUILD_TYPE}")
message(STATUS "Symbian Root:   ${SYMBIAN_ROOT}")
message(STATUS "Output Dir:     ${BUILD_OUTPUT_DIR}")
message(STATUS "")
message(STATUS "Build targets ready. Run: make -C build/x86_64/cmake -j4")
message(STATUS "")
