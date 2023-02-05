cmake_minimum_required(VERSION 3.0.0)
project(Chess VERSION 0.1.0)
set(CMAKE_CXX_STANDARD 20)

INCLUDE_DIRECTORIES(${CMAKE_SOURCE_DIR} ../../)


add_executable(${PROJECT_NAME} chess.hpp main.cpp UCI.hpp chess.cpp game/player.cpp game/player.hpp game/common.hpp game/board.cpp game/board.hpp)

target_sources(${PROJECT_NAME} PRIVATE main.cpp)


set(CPACK_PROJECT_NAME ${PROJECT_NAME})
set(CPACK_PROJECT_VERSION ${PROJECT_VERSION})

#set(CPACK_GENERATOR RPM)
set(CPACK_RPM_COMPONENT_INSTALL ON)
set(CPACK_RPM_PACKAGE_GROUP "Chess")
set(CPACK_RPM_PACKAGE_LICENSE "Apache 2.0 + Common Clause 1.0")
set(CPACK_RPM_MAIN_COMPONENT major)

set(DESTDIR ~)
set(CPACK_SET_DESTDIR ~/.local)
set(SET_CUSTOM_RPATH TRUE)

if(SET_CUSTOM_RPATH)
    set(CMAKE_SKIP_BUILD_RPATH FALSE)

    # when building, don't use the install RPATH already
    # (but later on when installing)
    set(CMAKE_BUILD_WITH_INSTALL_RPATH FALSE)
    set(CMAKE_INSTALL_RPATH_USE_LINK_PATH TRUE)

    set(CMAKE_INSTALL_RPATH "${CMAKE_INSTALL_PREFIX}/lib:../lib")

    # add the automatically determined parts of the RPATH
    # which point to directories outside the build tree to the install RPATH
    set(CMAKE_BUILD_RPATH "${CMAKE_INSTALL_PREFIX}/lib:../lib")

    set_target_properties( ${PROJECT_NAME} PROPERTIES INSTALL_RPATH "${CMAKE_INSTALL_RPATH}" )

endif()

install(TARGETS ${PROJECT_NAME} RUNTIME DESTINATION bin)


message(STATUS "location of epsilon build: " ${EPSILON_BUILD_LOCATION})
message(STATUS "location of vulkan libs: " ${Vulkan_LIBRARIES})

add_dependencies(${PROJECT_NAME} Epsilon)

if (BUILD_AS_LIBRARY)
    if (MSVC)
        target_link_libraries(${PROJECT_NAME} LINK_PUBLIC Epsilon)
    else ()
        target_link_libraries(${PROJECT_NAME} PUBLIC ${EPSILON_BUILD_LOCATION}/libEpsilon.so)
    #  target_link_libraries(${PROJECT_NAME} PUBLIC Epsilon)
    endif ()
else()
    target_sources(${PROJECT_NAME} PUBLIC ${PROJECT_FILES})
endif()

target_include_directories(${PROJECT_NAME} PUBLIC ${CMAKE_SOURCE_DIR})

target_link_libraries(${PROJECT_NAME} PUBLIC ${Vulkan_LIBRARIES})
target_include_directories(${PROJECT_NAME} PUBLIC ${Vulkan_INCLUDE_DIR})

target_link_libraries(${PROJECT_NAME} PRIVATE glfw3)

if(DBUILD_ANDROID EQUAL 0)
target_link_libraries(${PROJECT_NAME} PRIVATE assimp)
target_link_libraries(${PROJECT_NAME} PRIVATE openal)
endif()
target_link_libraries(${PROJECT_NAME} PUBLIC ${CMAKE_DL_LIBS})



