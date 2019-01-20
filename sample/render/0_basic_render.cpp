#include "fay/app/app_flexible.h"

#include "fay/render/define.h"
#include "sample_render_app.h"

//fay::app_desc clear_desc = global_desc;
//clear_desc.window.title = "clear";

class clear : public fay::app
{
public:
    clear(const fay::app_desc& desc) : fay::app(desc)
    {
        desc_.window.title = "clear";
    }

    void setup() override
    {
    }

    void update() override
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
    // using fay::app;
    triangle(const fay::app_desc& desc) : fay::app(desc)
    {
        desc_.window.title = "triangle";
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
            bd0.stride = 12; // TODO: do it by helper functions;
            bd0.data = lines;
            bd0.type = fay::buffer_type::vertex;

            bd0.layout = { {fay::attribute_usage::position, fay::attribute_format::float3} };
        }
        auto line_strip_vb = render->create(bd0);



        float vertices[] = {
             0.6f,  0.45f, 0.0f,   1.f, 1.f, // right top
             0.6f, -0.45f, 0.0f,   1.f, 0.f, // right bottom
            -0.6f, -0.45f, 0.0f,   0.f, 0.f, // left bottom
            -0.6f,  0.45f, 0.0f,   0.f, 1.f, // left top
        };
        unsigned int indices[] = {  // note that we start from 0!
            0, 1, 3,  // first Triangle
            1, 2, 3   // second Triangle
        };
        fay::buffer_desc bd; {
            bd.name = "triangle_vb";
            bd.size = 4;// sizeof(vertices);
            bd.stride = 20; // TODO: do it by helper functions;
            bd.data = vertices;
            bd.type = fay::buffer_type::vertex;

            bd.layout =
            {
                {fay::attribute_usage::position,  fay::attribute_format::float3},
                {fay::attribute_usage::texcoord0, fay::attribute_format::float2}
            };
        }
        fay::buffer_desc id(fay::buffer_type::index); {
            id.name = "triangle_ib";
            id.size = 6;
            id.data = indices;
        }
        auto triangle_vb = render->create(bd);
        auto triangle_ib = render->create(id);

        fay::image img("texture/awesomeface.png", true);
        auto triangle_tbo = create_2d(this->render, "hello", img);

        auto vs_code = R"(
                #version 330 core
                layout (location = 0) in vec3 mPos;
                layout (location = 1) in vec2 mTex;

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
        fay::shader_desc sd = fay::scan_shader_program(vs_code, fs_code, true);
        sd.name = "shd"; //todo
        auto shd_id = render->create(sd);

        fay::pipeline_desc pd;

        {
            pd.name = "line_strip";
            pd.primitive_type = fay::primitive_type::line_strip;
        }
        auto pipe_id = render->create(pd);

        {
            pd.name = "triangles";
            pd.primitive_type = fay::primitive_type::triangles;
        }
        auto pipe2_id = render->create(pd);

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
                .bind_uniform("flag", 0)
                .draw_index()

                .end_frame();
        }

        // render->submit(pass1);
        // render->submit(pass2);
        // device->complie(pass);

        // uncomment this call to draw in wireframe polygons.
        //glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    }

    void update() override
    {
       // render->draw(pass1);
       // render->draw(pass2);
        auto cmds = pass1;
        render->submit(pass1);
        render->execute();
    }

    fay::buffer_id buf_id;

    render_paras paras;
    fay::command_list pass1, pass2;
};

class post_processing : public fay::app
{
public:
    // using fay::app;
    post_processing(const fay::app_desc& desc) : fay::app(desc)
    {
        desc_.window.title = "post_proc";
    }

    void setup() override
    {
        mesh = fay::create_raw_renderable("object/box/box.obj", render.get());

        fay::image img("texture/awesomeface.png", true);
        tex_id = create_2d(this->render, "hello", img);

        fay::shader_desc sd = fay::scan_shader_program("gfx/renderable.vs", "gfx/renderable.fs", false);
        sd.name = "shd"; //todo
        shd_id = render->create(sd);

        fay::pipeline_desc pd;
        {
            pd.name = "triangles";
        }
        pipe_id = render->create(pd);

        auto frame = fay::create_frame(render.get(), "offscreen_frm", 512, 512);

        offscreen_frm_id = std::get<0>(frame);
        offscreen_tex_id = std::get<1>(frame);
        offscreen_ds_id  = std::get<2>(frame);
    }

    void update() override
    {
        misc.update_io();
        glm::mat4 view = misc.camera_.GetViewMatrix();
        glm::mat4 projection = glm::perspective(glm::radians(misc.camera_.Zoom),
            (float)misc.Width / (float)misc.Height, 0.1f, 10000.0f);

        // draw
        glm::mat4 model(1.f);
        auto MVP = projection * view * model;

        fay::command_list pass1, pass2;
        
        pass1
            .begin_frame(offscreen_frm_id)
            .clear_color({1.f, 0.f, 0.f, 1.f})
            .clear_depth()
            .clear_stencil()
            .apply_pipeline(pipe_id)
            .apply_shader(shd_id)
            .bind_uniform_block("color", fay::memory{ (uint8_t*)&paras, sizeof(render_paras) })
            .bind_uniform("MVP", MVP)
            .bind_uniform("bAlbedo", true)
            .bind_textures({ tex_id });

        mesh->render(pass1);
        pass1.end_frame();

        pass2
            .begin_default_frame()
            .clear_frame()
            .apply_pipeline(pipe_id)
            .apply_shader(shd_id)
            .bind_uniform_block("color", fay::memory{ (uint8_t*)&paras, sizeof(render_paras) })
            .bind_uniform("MVP", MVP)
            .bind_uniform("bAlbedo", true)
            .bind_textures({ offscreen_tex_id });

        mesh->render(pass2);
        pass2.end_frame();

        render->submit(pass1);
        render->submit(pass2);
        render->execute();
    }

    fay::render_misc misc;

    fay::renderable_sp mesh;

    fay::buffer_id triangle_vb;
    fay::buffer_id triangle_ib;

    fay::shader_id shd_id;
    fay::pipeline_id pipe_id;
    fay::texture_id tex_id;

    fay::texture_id offscreen_tex_id;
    fay::texture_id offscreen_ds_id;
    fay::frame_id offscreen_frm_id;

    render_paras paras;
    //fay::command_list pass1, pass2;
};


class shadow_map : public post_processing
{
public:
    // using fay::app;
    shadow_map(const fay::app_desc& desc) : post_processing(desc)
    {
        desc_.window.title = "post_proc";
    }

    void setup() override
    {
        post_processing::setup();

        mesh = fay::create_raw_renderable(fay::Blocks, render.get());

        fay::pipeline_desc pd;
        {
            pd.name = "shadow_pipe";
            pd.stencil_enabled = false;
        }
        shadow_pipe_id = render->create(pd);
    }

    void update() override
    {
        misc.update_io();
        glm::mat4 view = misc.camera_.GetViewMatrix();
        glm::mat4 projection = glm::perspective(glm::radians(misc.camera_.Zoom),
            (float)misc.Width / (float)misc.Height, 0.1f, 10000.0f);

        // draw
        glm::mat4 model(1.f);
        auto MVP = projection * view * model;

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
            .bind_textures({ tex_id });

        mesh->render(pass1);
        pass1.end_frame();

        pass2
            .begin_default_frame()
            .clear_frame()
            .apply_pipeline(pipe_id)
            .apply_shader(shd_id)
            .bind_uniform_block("color", fay::memory{ (uint8_t*)&paras, sizeof(render_paras) })
            .bind_uniform("MVP", MVP)
            .bind_uniform("bAlbedo", true)
            .bind_textures({ offscreen_tex_id });

        mesh->render(pass2);
        pass2.end_frame();

        render->submit(pass1);
        render->submit(pass2);
        render->execute();
    }

    fay::pipeline_id shadow_pipe_id;
};

SAMPLE_RENDER_APP_IMPL(clear)
SAMPLE_RENDER_APP_IMPL(triangle)
SAMPLE_RENDER_APP_IMPL(post_processing)
SAMPLE_RENDER_APP_IMPL(shadow_map)