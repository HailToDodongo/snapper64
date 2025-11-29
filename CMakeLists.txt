cmake_minimum_required(VERSION 3.30)
project(rdp_test)

set(CMAKE_C_COMPILER gcc)
set(CMAKE_CXX_COMPILER g++)

set(CMAKE_C_STANDARD 23)
set(CMAKE_CXX_STANDARD 23)

include_directories(../libdragon/include)

include_directories(src)

add_executable(rdp_test
        src/main.cpp
        src/renderer/text.cpp
        src/utils/miMemory.cpp
        src/renderer/rdp.h
        src/renderer/rdp.cpp
        src/testList.h
        src/tests/RDPFillTriRand.cpp
        src/framework/testGroup.h
        src/framework/assert.h
        src/framework/testGroup.cpp
        src/framework/testSurface.h
        src/renderer/vi.h
        src/renderer/vi.cpp
        src/utils/rng.h
        src/utils/rng.cpp
        src/utils/vecMath.h
        src/framework/testSurface.cpp
        src/framework/testSurface.cpp
        src/utils/hash.h
        src/framework/assert.cpp
        tools/dataConverter.cpp
        src/tests/RDPFillTri.cpp
)

set_property(TARGET rdp_test PROPERTY CXX_STANDARD 23)