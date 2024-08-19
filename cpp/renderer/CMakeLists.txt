find_package(SDL2 REQUIRED)

if (SDL2_FOUND)
    add_executable(renderer
        main.cpp
        CustomRendering.cpp
        ${CMAKE_SOURCE_DIR}/third_party/imgui/imgui.cpp
        ${CMAKE_SOURCE_DIR}/third_party/imgui/imgui_draw.cpp
        ${CMAKE_SOURCE_DIR}/third_party/imgui/imgui_widgets.cpp
        ${CMAKE_SOURCE_DIR}/third_party/imgui/imgui_tables.cpp
        ${CMAKE_SOURCE_DIR}/third_party/imgui/imgui_demo.cpp
        ${CMAKE_SOURCE_DIR}/third_party/imgui/backends/imgui_impl_sdl2.cpp
        ${CMAKE_SOURCE_DIR}/third_party/imgui/backends/imgui_impl_sdlrenderer2.cpp
        )

    target_include_directories(renderer
        PRIVATE ${SDL2_INCLUDE_DIRS}
        PRIVATE ${CMAKE_SOURCE_DIR}/third_party/imgui/)
    target_link_libraries(renderer ${SDL2_LIBRARIES})

    configure_file(AfricanHead.obj ${CMAKE_CURRENT_BINARY_DIR}/AfricanHead.obj COPYONLY)
endif()
