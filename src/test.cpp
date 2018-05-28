#include "engine.hpp"
#include "listener.hpp"
#include "physics/physics.hpp"
#include "data.hpp"
#include "sprite.hpp"

void init();
void test();
#ifdef main
#undef main
#endif

int main()
{
	tz::initialise();
	//test();
	init();
	tz::terminate();
	return 0;
}

void init()
{
	Window wnd("Topaz Development Window", 0, 0, 800, 600);
    wnd.centre_position();
	Engine engine(&wnd, "../../../res/runtime/properties.mdl");
    const EngineMeta& meta = engine.get_meta();

	unsigned int seconds = tz::util::cast::from_string<unsigned int>(meta.get_resources().get_tag("played"));
	float rotational_speed = tz::util::cast::from_string<float>(meta.get_resources().get_tag("rotational_speed"));
	constexpr std::size_t shader_id = 0;
	
	KeyListener key_listener(wnd);
	MouseListener mouse_listener(wnd);

	CubeMap skybox_texture("../../../res/runtime/textures/skybox/", "greenhaze", ".png");
	Shader skybox_shader("../../../src/shaders/skybox");
	
	Timer updater;
	bool noclip = false;
	
	Skybox skybox("../../../res/runtime/models/skybox.obj", skybox_texture);
	engine.emplace_trivial_update_command([&](){skybox.render(engine.camera, skybox_shader, engine.get_meshes(), wnd.get_width(), wnd.get_height());});
	
	Object player_object(engine.get_meshes().back().get(), engine.scene.get_objects().front().get().get_material(), Vector3F(), Vector3F(), Vector3F(5,5,5));
	engine.emplace_trivial_update_command([&](){if(engine.camera.has_perspective_projection()) return;player_object.render(engine.camera, &(engine.default_shader), wnd.get_width(), wnd.get_height());});
	engine.emplace_trivial_tick_command([&](){player_object.position = engine.camera.position;player_object.rotation = engine.camera.rotation;});
	
	Shader shader_2d("../../../src/shaders/2D");
    Shader shadow_shader("../../../src/shaders/depth");
	
	Sprite test_plane(Vector2F(0.0f, 50.0f), 0.0f, Vector2F(10, 10), engine.scene.get_objects().front().get().get_material().get_texture());
	Sprite another_test_plane(Vector2F(0.0f, 50.0f), 0.0f, Vector2F(10, 10), tz::graphics::texture::default_texture.get());
	engine.emplace_trivial_update_command([&]() {
        test_plane.render(engine.camera, &(shader_2d), wnd.get_width(), wnd.get_height());
        another_test_plane.render(engine.camera, &(shader_2d), wnd.get_width(), wnd.get_height());
    });

	FrameBuffer plane_texture_buffer(512, 512);
	Texture& plane_texture = plane_texture_buffer.emplace_texture(GL_DEPTH_ATTACHMENT, 512, 512, tz::graphics::TextureComponent::DEPTH_TEXTURE);
	//plane_texture_buffer.emplace_renderbuffer(GL_COLOR_ATTACHMENT0, 512, 512, GL_RGBA);
	plane_texture_buffer.set_output_attachment(GL_NONE);

	engine.emplace_trivial_tick_command([&](){if(key_listener.catch_key_pressed("H"))tz::util::log::message("harrapelord!");});

    GUI test_gui({10, 10}, {100, 100});
    const GUI& test_child = *test_gui.emplace_child<GUI>(Vector2<int>{0, 0}, Vector2<int>{50, 50}, &test_gui, std::initializer_list<GUI*>{});

    GUI test_copy = test_gui;
    std::cout << "test gui position: " << test_gui.get_x() << ", " << test_gui.get_y() << "\n";
    std::cout << "test gui address = " << &test_gui << ", number of children = " << test_gui.get_children().size() << "\n";
    std::cout << "test copy address = " << &test_copy << ", number of children = " << test_copy.get_children().size() << "\n";
    std::cout << "test child parent address = " << test_child.get_parent() << "\n";
    std::cout << "test child position: " << test_child.get_x() << ", " << test_child.get_y() << "\n";
    std::cout << "test child dimensions: " << test_child.get_width() << ", " << test_child.get_height() << "\n";
    std::cout << "screen width = " << tz::util::gui::display::resolution().x << ", height = " << tz::util::gui::display::resolution().y << "\n";
    std::cout << "test child screen width%: " << test_child.get_normalised_screen_width() << "\n";
    std::cout << "test child screen height%: " << test_child.get_normalised_screen_height() << "\n";

	while(!engine.get_window().is_close_requested())
	{
		float multiplier = tz::util::cast::from_string<float>(MDLFile(meta.get_properties().get_tag("resources")).get_tag("speed"));
		float velocity = multiplier;// * test_slider.position;

		if(updater.millis_passed(1000))
		{
			updater.reload();
			seconds++;
		}

		plane_texture_buffer.clear(GL_DEPTH_BUFFER_BIT, 0.0f, 0.0f, 0.0f, 0.0f);
		plane_texture_buffer.set_render_target();
		Camera behind = engine.camera;
		behind.rotation.y = tz::consts::pi - behind.rotation.y;
        behind.set_has_perspective_projection(false);
		engine.scene.render(behind, &(shadow_shader), wnd.get_width(), wnd.get_height());
        // placement-new re-alloc instead of re-assigning
		new (&test_plane) Sprite(Vector2F(0.0f, 300.0f), tz::consts::pi, Vector2F(100, 100 / wnd.get_width() * wnd.get_height()), &plane_texture);
		
		if(engine.is_update_due())
		{
			if(key_listener.is_key_pressed("Up"))
				engine.camera.fov -= (tz::consts::pi / 200.0f);
			if(key_listener.is_key_pressed("Down"))
				engine.camera.fov += (tz::consts::pi / 200.0f);
			if(key_listener.catch_key_pressed("P"))
				engine.camera.set_has_perspective_projection(!engine.camera.has_perspective_projection());
			if(key_listener.catch_key_pressed("F"))
				wnd.set_fullscreen(!wnd.is_fullscreen() ? Window::FullscreenType::DESKTOP_MODE : Window::FullscreenType::WINDOWED_MODE);
			if(key_listener.is_key_pressed("W"))
			{
				engine.camera.position += (engine.camera.forward() * velocity);
			}
			if(key_listener.is_key_pressed("S"))
			{
				engine.camera.position += (engine.camera.backward() * velocity);
			}
			if(key_listener.is_key_pressed("A"))
			{
				engine.camera.position += (engine.camera.left() * velocity);
			}
			if(key_listener.is_key_pressed("D"))
			{
				engine.camera.position += (engine.camera.right() * velocity);
			}
			if(key_listener.is_key_pressed("Space"))
			{
				engine.camera.position += (Vector3F{0.0f, 1.0f, 0.0f} * velocity);
			}
			if(key_listener.is_key_pressed("Z"))
			{
				engine.camera.position += (Vector3F{0.0f, -1.0f, 0.0f} * velocity);
			}
			float angular_speed = tz::util::cast::from_string<float>(meta.get_properties().get_tag("rotational_speed"));
			if(key_listener.is_key_pressed("I"))
			{
				engine.camera.rotation += (Vector3F(1.0f/360.0f, 0, 0) * multiplier * angular_speed * engine.get_time_profiler().get_last_delta());
			}
			if(key_listener.is_key_pressed("K"))
			{
				engine.camera.rotation += (Vector3F(-1.0f/360.0f, 0, 0) * multiplier * angular_speed * engine.get_time_profiler().get_last_delta());
			}
			if(key_listener.is_key_pressed("J"))
			{
				engine.camera.rotation += (Vector3F(0, -1.0f/360.0f, 0) * multiplier * angular_speed * engine.get_time_profiler().get_last_delta());
			}
			if(key_listener.is_key_pressed("L"))
			{
				engine.camera.rotation += (Vector3F(0, 1.0f/360.0f, 0) * multiplier * angular_speed * engine.get_time_profiler().get_last_delta());
			}
			if(key_listener.is_key_pressed("R"))
			{
				engine.camera.position = engine.scene.spawn_point;
				engine.camera.rotation = engine.scene.spawn_orientation;
			}
			//if(key_listener.catch_key_pressed("Escape"))
			//	gui_panel.set_hidden(!gui_panel.is_hidden());
			if(mouse_listener.is_left_clicked() /*&& gui_panel.is_hidden()*/)
			{
				Vector2F delta = mouse_listener.get_mouse_delta_pos();
				engine.camera.rotation.y += rotational_speed * delta.x;
				engine.camera.rotation.x -= rotational_speed * delta.y;
				mouse_listener.reload_mouse_delta();
			}
		}
		//exit_gui_button.set_x(wnd.get_width() - (exit_gui_button.get_width() * 2));
		//exit_gui_button.set_y(wnd.get_height() - (exit_gui_button.get_height() * 2));
		//gui_title.set_y(wnd.get_height() - (gui_title.get_height() * 2));
		updater.update();
		engine.update(shader_id);
	}
	MDLFile(meta.get_resources()).edit_tag("played", tz::util::cast::to_string(seconds));
}

void test()
{
	using namespace std::chrono_literals;
	using namespace tz::util::log;
	auto cls = [](){system("cls");};
	
	message("Playing test.wav asynchronously...");
	AudioClip test_wav("../../../res/runtime/music/test.wav");
	test_wav.play();
	message("Waiting 5 seconds...");
	std::this_thread::sleep_for(5s);
	AudioClip test_wav_copy(test_wav);
	cls();
	message("Playing copy of test wav...");
	test_wav_copy.play();
	message("Waiting 5 seconds...");
	std::this_thread::sleep_for(5s);
	cls();
	message("Re-playing origin copy of test.wav...");
	test_wav.play();
	message("Waiting 5 seconds");
	cls();
	AudioClip test_wav_moved = std::move(test_wav);
	message("Moved test.wav to a new instance, playing once more...");
	test_wav_moved.play();
	message("Waiting 5 seconds...");
	std::this_thread::sleep_for(5s);
	cls();
	// test RNG
	Random rand;
	MersenneTwister mt;
	for(unsigned int i = 0; i < 100; i++)
	{
		message("Default Random: ", rand.operator()<int>(0, 100), ", MersenneTwister: ", mt.operator()<int>(0, 100));
	}
	cls();
	message("TESTING COMPLETE");
}