add_demo(
    TARGET vk_init_demo
    SOURCE_FILES vk_init_demo.cpp
)

add_demo(
    TARGET vk_device_demo
    SOURCE_FILES vk_device_demo.cpp
)

add_demo(
    TARGET vk_window_demo
    SOURCE_FILES vk_window_demo.cpp
)

add_demo(
    TARGET vk_squares_demo
    SOURCE_FILES vk_squares_demo.cpp
    SHADER_SOURCES
        demo/gl/impl/backend/vk/basic.vertex.glsl
        demo/gl/impl/backend/vk/basic.fragment.glsl
)

add_demo(
    TARGET vk_triangle_demo_headless
    SOURCE_FILES vk_triangle_demo_headless.cpp
    SHADER_SOURCES
        demo/gl/impl/backend/vk/triangle_demo.vertex.glsl
        demo/gl/impl/backend/vk/triangle_demo.fragment.glsl
)

add_demo(
    TARGET vk_triangle_demo
    SOURCE_FILES vk_triangle_demo.cpp
    SHADER_SOURCES
        demo/gl/impl/backend/vk/triangle_demo.vertex.glsl
        demo/gl/impl/backend/vk/triangle_demo.fragment.glsl
)