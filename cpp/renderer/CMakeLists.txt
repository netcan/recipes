add_executable(renderer
    main.cpp
    CustomRendering.cpp
    Model.cpp
    StbImpl.cpp
    ${CMAKE_SOURCE_DIR}/third_party/imgui/imgui.cpp
    ${CMAKE_SOURCE_DIR}/third_party/imgui/imgui_draw.cpp
    ${CMAKE_SOURCE_DIR}/third_party/imgui/imgui_widgets.cpp
    ${CMAKE_SOURCE_DIR}/third_party/imgui/imgui_tables.cpp
    ${CMAKE_SOURCE_DIR}/third_party/imgui/imgui_demo.cpp
    ${CMAKE_SOURCE_DIR}/third_party/imgui/backends/imgui_impl_sdl2.cpp
    ${CMAKE_SOURCE_DIR}/third_party/imgui/backends/imgui_impl_sdlrenderer2.cpp
    )

target_include_directories(renderer
    PRIVATE ${CMAKE_SOURCE_DIR}/third_party/imgui/)
target_link_libraries(renderer SDL2)

add_custom_target(copy_object
    COMMAND ${CMAKE_COMMAND} -E copy_directory ${CMAKE_CURRENT_LIST_DIR}/object ${CMAKE_CURRENT_BINARY_DIR}/object
)
add_dependencies(renderer copy_object)

