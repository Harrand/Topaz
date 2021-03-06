#include "core/tz.hpp"
#include "gl/device.hpp"
#include "gl/renderer.hpp"

int main()
{
    constexpr std::size_t frame_number = 10;
    tz::initialise({"tz_renderer_test", tz::Version{1, 0, 0}, tz::info()}, tz::ApplicationType::Headless);
    {
        // Simply create some basic shaders. Don't try and render or anything.
        tz::gl::Device device{tz::gl::DeviceBuilder{}};

        tz::gl::ShaderBuilder builder;
        builder.set_shader_file(tz::gl::ShaderType::VertexShader, ".\\test\\gl\\shader_test.vertex.glsl");
        builder.set_shader_file(tz::gl::ShaderType::FragmentShader, ".\\test\\gl\\shader_test.fragment.glsl");
        tz::gl::Shader shader = device.create_shader(builder);

        std::array<int, 5> values{1, 2, 3, 4, 5};
        tz::gl::BufferResource int_resource{tz::gl::BufferData::from_array<int>(values)};

        tz::gl::RendererBuilder renderer_builder;
        tz::gl::ResourceHandle int_handle = renderer_builder.add_resource(int_resource);
        renderer_builder.set_output(tz::window());
        renderer_builder.set_pass(tz::gl::RenderPassAttachment::Colour);
        renderer_builder.set_shader(shader);
        tz::gl::Renderer renderer = device.create_renderer(renderer_builder);

        tz_assert(renderer.input_count() == 0, "Renderer had unexpected number of inputs");
        tz_assert(renderer.input_count_of(tz::gl::RendererInputDataAccess::StaticFixed) == 0, "Renderer had unexpected number of static inputs. Expected %d, but it has %zu", 0, renderer.input_count_of(tz::gl::RendererInputDataAccess::StaticFixed));
        tz_assert(renderer.input_count_of(tz::gl::RendererInputDataAccess::DynamicFixed) == 0, "Renderer had unexpected number of dynamic inputs. Expected %d, but it has %zu", 0, renderer.input_count_of(tz::gl::RendererInputDataAccess::DynamicFixed));

        tz_assert(renderer.resource_count() == 1, "Renderer had unexpected number of resources. Expected %d, got %zu", 1, renderer.resource_count());
        tz_assert(renderer.resource_count_of(tz::gl::ResourceType::Buffer) == 1, "Renderer had unexpected number of buffer resources. Expected %d, but it has %zu", 1, renderer.resource_count_of(tz::gl::ResourceType::Buffer));
        tz_assert(renderer.resource_count_of(tz::gl::ResourceType::Texture) == 0, "Renderer had unexpected number of texture resources. Expected %d, but it has %zu", 0, renderer.resource_count_of(tz::gl::ResourceType::Texture));
        // Clear Colour
        constexpr tz::Vec4 default_clear_colour{0.0f, 0.0f, 0.0f, 0.0f};
        tz_assert(renderer.get_clear_colour() == default_clear_colour, "Default renderer clear-colour is not {0, 0, 0, 0}");
        constexpr tz::Vec4 my_clear_colour{1.0f, 1.0f, 1.0f, 1.0f};
        renderer.set_clear_colour(my_clear_colour);
        tz::Vec4 rend_cc = renderer.get_clear_colour();
        tz_assert(rend_cc == my_clear_colour, "Expected clear colour {1, 1, 1, 1}, got {%.2f, %.2f, %.2f, %.2f}", rend_cc[0], rend_cc[1], rend_cc[2], rend_cc[3]);

        // Ensure resources make sense.
        tz::gl::IResource* same_int_resource = renderer.get_resource(int_handle);
        tz_assert(same_int_resource != nullptr, "Renderer failed to process resource");
        tz_assert(same_int_resource != &int_resource, "Renderer does not clone resource");
        {
            std::span<const std::byte> byte_span = same_int_resource->get_resource_bytes();
            auto* values_again = reinterpret_cast<const int*>(byte_span.data());
            tz_assert(byte_span.size_bytes() == sizeof(values), "Renderer resource had unexpected byte span size. Expected %zu, got %zu.", sizeof(values), byte_span.size_bytes());
            for(std::size_t i = 0; i < values.size(); i++)
            {
                tz_assert(values[i] == values_again[i], "Renderer static resource data does not match the resource provided in the RendererBuilder.");
            }
        }

        for(std::size_t i = 0; i < frame_number; i++)
        {
            tz::window().update();
            renderer.render();
        }
    }
    tz::terminate();
}