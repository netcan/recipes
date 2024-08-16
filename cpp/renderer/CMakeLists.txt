find_package(SDL2 REQUIRED)

if (SDL2_FOUND)
    include_directories(
        imgui
        imgui/backends
    )
    include_directories(${SDL2_INCLUDE_DIRS})

    add_executable(renderer
        main.cpp
        CustomRendering.cpp
        imgui/imgui.cpp
        imgui/imgui_draw.cpp
        imgui/imgui_widgets.cpp
        imgui/imgui_tables.cpp
        imgui/imgui_demo.cpp
        imgui/backends/imgui_impl_sdl2.cpp
        imgui/backends/imgui_impl_sdlrenderer2.cpp
        )

    target_link_libraries(renderer ${SDL2_LIBRARIES})
endif()
