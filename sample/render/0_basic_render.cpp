#include "fay/app/app_flexible.h"

#include "fay/render/define.h"
#include "sample_render_app.h"

//fay::app_desc clear_desc = global_desc;
//clear_desc.window.title = "clear";

class clear : public fay::app
{
public:
    clear(const fay::app_desc& _desc) : fay::app(_desc)
    {
        desc.window.title = "clear";
    }

    void setup() override
    {
    }

    void render() override
    {
        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);
        glcheck_errors();
    }
};

struct render_paras
{
    glm::vec4 a{};
    glm::vec4 b{};
};

class triangle : public fay::app
{
public:
    fay::buffer_id buf_id;

    static const inline size_t nInstance = 2;
    glm::mat4 instance_transforms[nInstance]
    {
        fay::transform{ glm::vec3{ 0, 0, 0 } }.model_matrix(),
        fay::transform{ glm::vec3{ 0.5, 0, 0 } }.model_matrix(),
    };
    fay::buffer_id instance_transforms_id;

    render_paras paras;
    fay::command_list pass1, pass2;

    triangle(const fay::app_desc& _desc) : fay::app(_desc)
    {
        desc.window.title = "triangle";
    }

    void setup() override
    {
        // set up vertex data (and buffer(s)) and configure vertex attributes
        // ------------------------------------------------------------------
        float lines[] = {
            -0.3f,  0.9f, 0.0f,
             0.3f,  0.9f, 0.0f,
             0.3f, -0.9f, 0.0f,
             0.3f, -0.9f, 0.0f,
            -0.3f, -0.9f, 0.0f,
            -0.3f,  0.9f, 0.0f,
        };
        fay::buffer_desc bd0; {
            bd0.name = "line_stripe_vb";
            bd0.size = 6;// sizeof(vertices);
            bd0.data = lines;
            bd0.type = fay::buffer_type::vertex;

            bd0.layout = { {fay::attribute_usage::position, fay::attribute_format::float3} };
            bd0.stride = bd0.layout.stride();
        }
        auto line_strip_vb = device->create(bd0);



        float vertices[] = {
             0.6f,  0.45f, 0.5f,   0.f, 0.f, -1.f,   1.f, 1.f, // right top
             0.6f, -0.45f, 0.5f,   0.f, 0.f, -1.f,   1.f, 0.f, // right bottom
            -0.6f, -0.45f, 0.5f,   0.f, 0.f, -1.f,   0.f, 0.f, // left bottom
            -0.6f,  0.45f, 0.5f,   0.f, 0.f, -1.f,   0.f, 1.f, // left top
        };
        unsigned int indices[] = {  // note that we start from 0!
            0, 1, 3,  // first Triangle
            1, 2, 3   // second Triangle
        };
        fay::buffer_desc bd; {
            bd.name = "triangle_vb";
            bd.size = 4;// sizeof(vertices);
            bd.data = vertices;
            bd.type = fay::buffer_type::vertex;

            bd.layout =
            {
                {fay::attribute_usage::position,  fay::attribute_format::float3},
                {fay::attribute_usage::normal,    fay::attribute_format::float3},
                {fay::attribute_usage::texcoord0, fay::attribute_format::float2}
            };
            bd.stride = bd.layout.stride();
        }
        fay::buffer_desc id(fay::buffer_type::index); {
            id.name = "triangle_ib";
            id.size = 6;
            id.data = indices;
        }
        auto triangle_vb = device->create(bd);
        auto triangle_ib = device->create(id);

        {
            fay::buffer_desc desc;
            desc.name = "instance_model";
            desc.size = nInstance;
            desc.layout =
            {
                {fay::attribute_usage::instance_model,  fay::attribute_format::float4, 4},
            };
            desc.stride = desc.layout.stride();
            desc.data = &instance_transforms;
            desc.type = fay::buffer_type::instance;
            instance_transforms_id = device->create(desc);
        }

        fay::image img("texture/awesomeface.png", true);
        auto triangle_tbo = create_2d(this->device, "hello", img);

        auto vs_code = R"(
                #version 330 core
                layout (location = 0) in vec3 mPos;
                layout (location = 1) in vec3 mNor;
                layout (location = 2) in vec2 mTex;

                out vec2 vTex;

                void main()
                {
                   gl_Position = vec4(mPos.x, mPos.y, mPos.z, 1.0);
                   vTex = mTex;   
                }
            )";
        auto fs_code = R"(
                #version 330 core
                in vec2 vTex;
                out vec4 FragColor;

                uniform sampler2D Diffuse;

                layout (std140) uniform color
                {
                    vec4 a;
                    vec4 b;
                };
                
                uniform int flag;

                void main()
                {
                   if(flag == 1)
                       FragColor = texture(Diffuse, vTex);
                   else
                       FragColor = texture(Diffuse, vTex);

                   //FragColor = vec4(1.0f, 0.5f, 0.2f, 1.0f);
                }
            )";
        fay::shader_desc sd = fay::scan_shader_program("gfx/29_instancing.vs", "gfx/29_instancing.fs"); // fay::scan_shader_program(vs_code, fs_code, true);
        sd.name = "shd"; //todo
        auto shd_id = device->create(sd);

        fay::pipeline_desc pd;

        {
            pd.name = "line_strip";
            pd.primitive_type = fay::primitive_type::line_strip;
        }
        auto pipe_id = device->create(pd);

        {
            pd.name = "triangles";
            pd.primitive_type = fay::primitive_type::triangles;
            pd.cull_mode = fay::cull_mode::none;
        }
        auto pipe2_id = device->create(pd);

        paras.a = { 1.f, 0.f, 0.f, 1.f };
        paras.b = { 0.f, 1.f, 0.f, 1.f };

        {
            pass1
                .begin_default_frame()
                .clear_frame()
                .apply_shader(shd_id)
                .bind_uniform_block("color", fay::memory{ (uint8_t*)&paras, sizeof(render_paras) })

                //.apply_pipeline(pipe_id)
                //.bind_vertex(line_strip_vb)
                //.bind_uniform("flag", 1)
                //.draw()

                .apply_pipeline(pipe2_id)
                .bind_textures({ triangle_tbo })
                .bind_index(triangle_ib)
                .bind_vertex(triangle_vb)
                .bind_instance(instance_transforms_id, { fay::attribute_usage::instance_model })
                .bind_uniform("flag", 0)
                .draw_index(6, 0, 2)

                .end_frame();
        }

        // device->submit(pass1);
        // device->submit(pass2);
        // device->complie(pass);

        // uncomment this call to draw in wireframe polygons.
        //glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    }

    void render() override
    {
       // device->draw(pass1);
       // device->draw(pass2);
        device->submit(pass1);
        device->execute();
    }
};

class two_passes : public fay::app
{
public:
    /*
    GLfloat near_plane = 1.0f, far_plane = 300.f;
        //glm::mat4 lightOrtho = glm::ortho(-150.0f, 150.0f, -200.0f, 200.0f, near_plane, far_plane);
        glm::mat4 lightProj = glm::perspective(glm::radians(120.f),
            1.f, near_plane, far_plane);
        glm::mat4 lightView = glm::lookAt(
            light->position(), glm::vec3(0.0f), glm::vec3(1.f, 1.f, 0.f));
        glm::mat4 lightSpace = lightProj * lightView;
    */
    fay::camera cameras_[3]
    {
        fay::camera{ glm::vec3{ 0, 40, 80 }, -90, 0, 1.f, 300.f },

        //pitch 0 -> -30
        fay::camera{ glm::vec3{ -100, 100, 0 }, /*-180*/0, -45, 1.f, 300.f }, // light1
        fay::camera{ glm::vec3{ 100, 100, 0 },    0, 0,  1.f, 300.f }, // light2
    };
    fay::light lights_[3]
    {
        fay::light{ glm::vec3{ 0, 100, 100 } }, // WARNING: shadow map -> fay::light{ glm::vec3{ -100, 100, 0 } },
        fay::light{ glm::vec3{ 100, 100, 0 } },
        fay::light{ glm::vec3{ 0, 100, 100 } },
    };
    fay::transform transforms_[2]
    {
        fay::transform{ glm::vec3{ 0, 0, 0 } },
        fay::transform{ glm::vec3{ -100, 0, 0 } },
    };
    const fay::camera* camera{ cameras_ };
    const fay::light* light{ lights_ };
    const fay::transform* transform{ transforms_ };

    fay::shader_id debug_shd_id;
    fay::pipeline_id debug_pipe_id;

    fay::buffer_id vbo;
    fay::buffer_id ibo;

    fay::renderable_sp mesh;
    fay::texture_id tex_id;
    fay::shader_id shd_id;
    fay::pipeline_id pipe_id;

    fay::texture_id offscreen_tex_id;
    fay::texture_id offscreen_tex_id2;
    fay::texture_id offscreen_tex_id3;
    fay::texture_id offscreen_ds_id;
    fay::frame_id offscreen_frm_id;

    fay::renderable_sp mesh2;
    fay::texture_id tex_id2;
    fay::shader_id shd_id2;
    fay::pipeline_id pipe_id2;

    render_paras paras;
    //fay::command_list pass1, pass2;

public:
    using fay::app::app;

    void add_update_items()
    {
        add_update_items_
        (
            { cameras_, cameras_ + 1 },
            { lights_, lights_+1 },
            { transforms_, transforms_ + 1 }
        );
    }

    void debug_setup()
    {
        fay::shader_desc sd = fay::scan_shader_program("gfx/lines.vs", "gfx/lines.fs");
        sd.name = "light_shd";
        debug_shd_id = device->create(sd);

        fay::pipeline_desc pd;
        {
            pd.name = "light_pipe";
            pd.stencil_enabled = false;
            pd.primitive_type = fay::primitive_type::lines;
            debug_pipe_id = device->create(pd);
        }
    }

    void update(const fay::single_input& io) override
    {
        static size_t current_mode_{};
        static size_t current_items_[3]{};

        if (io[' ']) current_mode_ = ++current_mode_ % 3;
        if (io['z']) current_mode_ = 0;
        if (io['x']) current_mode_ = 1;
        if (io['c']) current_mode_ = 2;

        //if (io['0']) current_item_ = 0;
        if (io['1']) current_items_[current_mode_] = 0;
        if (io['2']) current_items_[current_mode_] = 1;
        if (io['3']) current_items_[current_mode_] = 2;

        camera = cameras_ + current_items_[0];
        light = lights_ + current_items_[1];
        transform = transforms_ + current_items_[2];
    }
};

// post_processing
class offscreen : public two_passes
{
public:
    // using fay::app;
    offscreen(const fay::app_desc& _desc) : two_passes(_desc)
    {
        desc.window.title = "post_proc";
    }

    void setup() override
    {
        add_update_items();

        mesh = fay::create_raw_renderable(fay::Box, device.get());

        fay::image img("texture/awesomeface.png", true);
        tex_id = create_2d(this->device, "hello", img);

        fay::shader_desc sd = fay::scan_shader_program("gfx/renderable.vs", "gfx/renderable.fs");
        sd.name = "shd"; //todo
        shd_id = device->create(sd);

        fay::pipeline_desc pd;
        {
            pd.name = "triangles";
        }
        pipe_id = device->create(pd);

        auto frame = fay::create_frame(device.get(), "offscreen_frm", 512, 512);

        offscreen_frm_id = std::get<0>(frame);
        offscreen_tex_id = std::get<1>(frame);
        offscreen_ds_id = std::get<2>(frame);
    }

    void render() override
    {
        auto MVP = camera->world_to_ndc() * transform->model_matrix();

        fay::command_list pass1, pass2;

        pass1
            .begin_frame(offscreen_frm_id)
            .clear_color({ 1.f, 0.f, 0.f, 1.f })
            .clear_depth()
            .clear_stencil()
            .apply_pipeline(pipe_id)
            .apply_shader(shd_id)
            .bind_uniform_block("color", fay::memory{ (uint8_t*)&paras, sizeof(render_paras) })
            .bind_uniform("MVP", MVP)
            .bind_uniform("bAlbedo", true)
            .bind_textures({ tex_id })
            .draw(mesh.get())
            .end_frame();

        pass2
            .begin_default_frame()
            .clear_frame()
            .apply_pipeline(pipe_id)
            .apply_shader(shd_id)
            .bind_uniform_block("color", fay::memory{ (uint8_t*)&paras, sizeof(render_paras) })
            .bind_uniform("MVP", MVP)
            .bind_uniform("bAlbedo", true)
            .bind_textures({ offscreen_tex_id })
            .draw(mesh.get())
            .end_frame();

        device->execute({ pass1, pass2});
    }
};

class shadow_map : public two_passes
{
public:
    shadow_map(const fay::app_desc& _desc) : two_passes(_desc)
    {
        desc.window.title = "shadow_map";
    }

    void setup() override
    {
        add_update_items();
        debug_setup();

        mesh = fay::create_renderable(fay::Plants, device.get());

        {
            fay::image img("texture/awesomeface.png", true);
            tex_id = create_2d(this->device, "hello", img);
        }

        {
            fay::shader_desc sd = fay::scan_shader_program("gfx/32_shadow_map.vs", "gfx/32_shadow_map.fs", false);
            sd.name = "shd"; //todo
            shd_id = device->create(sd);

            fay::pipeline_desc pd;
            pd.name = "shadow_pipe";
            // pd.cull_mode = fay::cull_mode::front; // sometimes could improve the quality
            pd.stencil_enabled = false;
            pipe_id = device->create(pd);
        }

        {
            //fay::shader_desc sd2 = fay::scan_shader_program("gfx/32_shadow_model.vs", "gfx/32_shadow_model.vs", false);
            fay::shader_desc sd = fay::scan_shader_program("gfx/32_shadow_model.vs", "gfx/32_shadow_model.fs", false);
            sd.name = "shd2"; //todo
            shd_id2 = device->create(sd);

            fay::pipeline_desc pd;
            pd.name = "pipe2";
            //pd.cull_mode = fay::cull_mode::none;
            pipe_id2 = device->create(pd);
        }

        auto frame = fay::create_depth_frame("shadowmap_frame", 1024, 1024, device.get());

        offscreen_frm_id = std::get<0>(frame);
        offscreen_tex_id = std::get<1>(frame);
        offscreen_ds_id  = std::get<2>(frame);
    }

    void render() override
    {
        GLfloat near_plane = 1.f, far_plane = 200.f;
        glm::mat4 lightOrtho = glm::ortho(-150.f, 150.f, -100.0f, 100.0f, near_plane, far_plane);
        glm::mat4 lightProj = glm::perspective(glm::radians(90.f),
            1080.f / 720.f, near_plane, far_plane);
        glm::mat4 lightView = glm::lookAt(
            light->position(), glm::vec3(0.0f), glm::vec3(0.f, 1.f, 0.f));
        glm::mat4 lightSpace = lightProj * lightView;

        // debug info
        // FIXME: over the GPU memory
        //fay::bounds3 box_light(-70, 70);
        fay::frustum box_light(lightSpace);
        auto debug_light = create_box_mesh(box_light, device.get());

        //fay::bounds3 box(-70, 70);
        fay::frustum box_camera(cameras_[0].world_to_ndc());
        auto debug_camera = create_box_mesh(box_camera, device.get());


        fay::command_list pass1, pass2;
        // depth map
        pass1
            .begin_frame(offscreen_frm_id)
            .clear_color({ 1.f, 0.f, 0.f, 1.f }) // rgb32f
            .clear_depth()
            .clear_stencil()
            .apply_pipeline(pipe_id)
            .apply_shader(shd_id)
            .bind_uniform("MVP", lightSpace * transform->model_matrix())
            .draw(mesh.get())
            .end_frame();

        pass2
            .begin_default(pipe_id2, shd_id2)
            //.bind_uniform_block("color", fay::memory{ (uint8_t*)&paras, sizeof(render_paras) })
            .bind_uniform("Proj", camera->persp())
            .bind_uniform("View", camera->view())
            .bind_uniform("Model", transform->model_matrix())
            .bind_uniform("LightSpace", lightSpace)
            .bind_uniform("LightPos", light->position())
            .bind_uniform("ViewPos", camera->position())
            .bind_texture(offscreen_ds_id, "Shadowmap")
            .draw(mesh.get())
            // debug info
            .apply_pipeline(debug_pipe_id)
            .apply_shader(debug_shd_id)
            .bind_uniform("MVP", camera->world_to_ndc())
            .draw(debug_camera.get()) // they are in the world space, doesn't need model matrix.
            .draw(debug_light.get())
            .end_frame();

        device->execute({ pass1, pass2 });
    }
};

class PBR : public two_passes
{
public:
    fay::texture_id tex_id0, tex_id1, tex_id2, tex_id3, tex_id4;

    PBR(const fay::app_desc& _desc) : two_passes(_desc)
    {
        desc.window.title = "PBR";
    }

    void setup() override
    {
        add_update_items();
        debug_setup();

        mesh = fay::create_raw_renderable(fay::Sphere, device.get());

        fay::image img0("texture/pbr/rusted_iron/albedo.png", true);
        fay::image img1("texture/pbr/rusted_iron/metallic.png", true);
        fay::image img2("texture/pbr/rusted_iron/roughness.png", true);
        fay::image img3("texture/pbr/rusted_iron/normal.png", true);
        fay::image img4("texture/pbr/rusted_iron/ao.png", true);
        tex_id0 = create_2d(this->device, "albedo", img0);
        tex_id1 = create_2d(this->device, "metal", img1);
        tex_id2 = create_2d(this->device, "rough", img2);
        tex_id3 = create_2d(this->device, "normal", img3);
        tex_id4 = create_2d(this->device, "ao", img4);

        fay::shader_desc sd = fay::scan_shader_program("gfx/pbr.vs", "gfx/pbr.fs");
        sd.name = "shd";
        shd_id = device->create(sd);

        fay::pipeline_desc pd;
        pd.name = "pipe";
        pipe_id = device->create(pd);
    }

    void render() override
    {
        GLfloat near_plane = 1.f, far_plane = 200.f;
        glm::mat4 lightOrtho = glm::ortho(-150.f, 150.f, -100.0f, 100.0f, near_plane, far_plane);
        glm::mat4 lightProj = glm::perspective(glm::radians(90.f),
            1080.f / 720.f, near_plane, far_plane);
        glm::mat4 lightView = glm::lookAt(
            light->position(), glm::vec3(0.0f), glm::vec3(0.f, 1.f, 0.f));
        glm::mat4 lightSpace = lightProj * lightView;

        // debug info
        // FIXME: over the GPU memory
        //fay::bounds3 box_light(-70, 70);
        fay::frustum box_light(lightSpace);
        auto debug_light = create_box_mesh(box_light, device.get());

        fay::command_list pass;

        pass
            .begin_default(pipe_id, shd_id)
            .bind_uniform("proj", camera->persp())
            .bind_uniform("view", camera->view())
            .bind_uniform("camPos", camera->position())
            .bind_uniform("lightPositions[0]", light->position())
            .bind_uniform("lightColors[0]", glm::vec3(1.f, 1.f, 1.f))

            .bind_textures({ tex_id0, tex_id1, tex_id2, tex_id3, tex_id4, })
            .bind_uniform("Albedo", glm::vec3(0.5f, 0.0f, 0.0f))
            .bind_uniform("Ao", 1.f);

        int nrRows = 7;
        int nrColumns = 7;
        float spacing = 25;
        glm::mat4 model = glm::mat4(1.0f);
        for (int row = 0; row < nrRows; ++row)
        {
            for (int col = 0; col < nrColumns; ++col)
            {
                // we clamp the roughness to 0.025 - 1.0 as perfectly smooth surfaces (roughness of 0.0) tend to look a bit off
                // on direct lighting.

                model = glm::mat4(1.0f);
                model = glm::translate(model, glm::vec3(
                    (col - (nrColumns / 2)) * spacing,
                    (row - (nrRows / 2)) * spacing,
                    0.0f
                ));

                pass
                    .bind_uniform("model", model)
                    .bind_uniform("Metallic", (float)row / (float)nrRows)
                    .bind_uniform("Roughness", glm::clamp((float)col / (float)nrColumns, 0.05f, 1.0f))
                    .draw(mesh.get());
            }
        }

        pass
            .apply_pipeline(debug_pipe_id)
            .apply_shader(debug_shd_id)
            .bind_uniform("MVP", camera->world_to_ndc())
            .draw(debug_light.get())
            .end_frame();

        device->execute(pass);
    }
};

class defer_rendering : public two_passes
{
public:
    // using fay::app;
    defer_rendering(const fay::app_desc& _desc) : two_passes(_desc)
    {
        desc.window.title = "post_proc";


    }

    std::vector<glm::vec3> objectPositions;
    const unsigned int NR_LIGHTS = 32;
    std::vector<glm::vec3> lightPositions;
    std::vector<glm::vec3> lightColors;

    void setup() override
    {
        add_update_items();

        objectPositions.push_back(glm::vec3(-3.0, -3.0, -3.0));
        objectPositions.push_back(glm::vec3(0.0, -3.0, -3.0));
        objectPositions.push_back(glm::vec3(3.0, -3.0, -3.0));
        objectPositions.push_back(glm::vec3(-3.0, -3.0, 0.0));
        objectPositions.push_back(glm::vec3(0.0, -3.0, 0.0));
        objectPositions.push_back(glm::vec3(3.0, -3.0, 0.0));
        objectPositions.push_back(glm::vec3(-3.0, -3.0, 3.0));
        objectPositions.push_back(glm::vec3(0.0, -3.0, 3.0));
        objectPositions.push_back(glm::vec3(3.0, -3.0, 3.0));

        srand(glfwGetTime());
        for (unsigned int i = 0; i < NR_LIGHTS; i++)
        {
            // calculate slightly random offsets
            float xPos = ((rand() % 100) / 100.0) * 6.0 - 3.0;
            float yPos = ((rand() % 100) / 100.0) * 6.0 - 4.0;
            float zPos = ((rand() % 100) / 100.0) * 6.0 - 3.0;
            lightPositions.push_back(glm::vec3(xPos, yPos, zPos));
            // also calculate random color
            float rColor = ((rand() % 100) / 200.0f) + 0.5; // between 0.5 and 1.0
            float gColor = ((rand() % 100) / 200.0f) + 0.5; // between 0.5 and 1.0
            float bColor = ((rand() % 100) / 200.0f) + 0.5; // between 0.5 and 1.0
            lightColors.push_back(glm::vec3(rColor, gColor, bColor));
        }

        mesh  = fay::create_renderable(fay::nierautomata_2b, device.get());
        mesh2 = fay::create_raw_renderable(fay::Box, device.get());

        // quad
        std::vector<glm::ivec3> vb{ { 0, 0, 0 },{ 1, 0, 0 },{ 1, 1, 0 },{ 0, 1, 0 } };
        std::vector<uint32_t> ib{ 0,1,2,2,3,0 };

        {
            fay::image img("texture/awesomeface.png", true);
            tex_id = create_2d(this->device, "hello", img);
        }

        {
            fay::shader_desc sd = fay::scan_shader_program("gfx/30_phong_shading.vs", "gfx/38_g_buffer.fs", false);
            sd.name = "shd"; //todo
            shd_id = device->create(sd);
        }

        {
            //fay::shader_desc sd2 = fay::scan_shader_program("gfx/32_shadow_model.vs", "gfx/32_shadow_model.vs", false);
            fay::shader_desc sd2 = fay::scan_shader_program("gfx/two_passes.vs", "gfx/38_deferred_shading.fs", false);
            sd2.name = "shd2"; //todo
            shd_id2 = device->create(sd2);
        }

        {
            fay::pipeline_desc pd;
            {
                pd.name = "shadow_pipe";
                pd.cull_mode = fay::cull_mode::none;
            }
            pipe_id = device->create(pd);
        }
        {
            fay::pipeline_desc pd;
            {
                pd.name = "pipe2";
                pd.cull_mode = fay::cull_mode::none;
            }
            pipe_id2 = device->create(pd);
        }

        auto frame = fay::create_Gbuffer(device.get(), "offscreen_frm", 1024, 1024);

        offscreen_frm_id  = std::get<0>(frame);
        offscreen_tex_id  = std::get<1>(frame);
        offscreen_tex_id2 = std::get<2>(frame);
        offscreen_tex_id3 = std::get<3>(frame);
        offscreen_ds_id   = std::get<4>(frame);

        //camera = fay::camera{ glm::vec3{ 0, 0, 1.5 } };
    }

    void render() override
    {
        glm::mat4 view = camera->view();
        glm::mat4 proj = glm::perspective(glm::radians(camera->zoom()),
            (float)desc.window.width / desc.window.height, 0.1f, 10000.0f);

        auto MVP = proj * view * transform->model_matrix();

        fay::command_list pass1, pass2;

        // depth map
        pass1
            .begin_frame(offscreen_frm_id)
            .clear_frame()
            .apply_pipeline(pipe_id)
            .apply_shader(shd_id);
        for (unsigned int i = 0; i < objectPositions.size(); i++)
        {
            //glm::mat4 objectmodel = glm::mat4(50);
            glm::mat4 objectmodel = glm::mat4(1);
            objectmodel = glm::translate(objectmodel, objectPositions[i]);
            objectmodel = glm::scale(objectmodel, glm::vec3(5.f));

            glm::mat4 MV = view * objectmodel;
            glm::mat3 NormalMV = glm::mat3(glm::transpose(glm::inverse(MV)));

            pass1
                .bind_uniform("MV", MV)
                .bind_uniform("NormalMV", NormalMV)
                .bind_uniform("MVP", proj * MV);

            mesh->render(pass1);
        }
        pass1.end_frame();

        pass2
            .begin_default_frame()
            .clear_frame()
            .apply_pipeline(pipe_id2)
            .apply_shader(shd_id2)
            //.bind_uniform_block("color", fay::memory{ (uint8_t*)&paras, sizeof(render_paras) })
            .bind_texture(offscreen_tex_id, "gPosition") // TODO
            .bind_texture(offscreen_tex_id2, "gNormal")
            .bind_texture(offscreen_tex_id3, "gAlbedoSpec")
            .bind_uniform("MVP", MVP)
            .bind_uniform("viewPos", camera->position());

        for (unsigned int i = 0; i < lightPositions.size(); i++)
        {
            // render attenuation parameters and calculate radius
            const float constant = 1.0; // note that we don't send this to the shader, we assume it is always 1.0 (in our case)
            const float linear = 0.7;
            const float quadratic = 1.8;

            pass2
                .bind_uniform("lights[" + std::to_string(i) + "].Position", lightPositions[i])
                .bind_uniform("lights[" + std::to_string(i) + "].Color", lightColors[i])
                .bind_uniform("lights[" + std::to_string(i) + "].Linear", linear)
                .bind_uniform("lights[" + std::to_string(i) + "].Quadratic", quadratic);
        }
        mesh2->render(pass2);
        pass2.end_frame();

        device->execute({ pass1, pass2 });
    }
};

SAMPLE_RENDER_APP_IMPL(clear)
SAMPLE_RENDER_APP_IMPL(triangle)
SAMPLE_RENDER_APP_IMPL(offscreen)
SAMPLE_RENDER_APP_IMPL(shadow_map)
SAMPLE_RENDER_APP_IMPL(defer_rendering)

SAMPLE_RENDER_APP_IMPL(PBR)