#include "fay/app/app.h"
#include "fay/core/config.h"
#include "fay/core/fay.h"

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <iostream>

#define glcheck_errors() CHECK(glGetError() == GL_NO_ERROR)

fay::render_desc render_desc;

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
        float triangles[] = {
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
            bd0.data = triangles;
            bd0.type = fay::buffer_type::vertex;

            bd0.layout = { {fay::attribute_usage::position, fay::attribute_format::float3} };
        }
        auto line_strip_vb = render->create(bd0);



        float vertices[] = {
             0.6f,  0.45f, 0.0f,  // right top
             0.6f, -0.45f, 0.0f,  // right bottom
            -0.6f, -0.45f, 0.0f,  // left bottom
            -0.6f,  0.45f, 0.0f   // left top
        };
        unsigned int indices[] = {  // note that we start from 0!
            0, 1, 3,  // first Triangle
            1, 2, 3   // second Triangle
        };
        fay::buffer_desc bd; {
            bd.name = "triangle_vb";
            bd.size = 4;// sizeof(vertices);
            bd.stride = 12; // TODO: do it by helper functions;
            bd.data = vertices;
            bd.type = fay::buffer_type::vertex;

            bd.layout = { {fay::attribute_usage::position, fay::attribute_format::float3} };
        }
        fay::buffer_desc id(fay::buffer_type::index); {
            id.name = "triangle_ib";
            id.size = 6;
            id.data = indices;
        }
        auto triangle_vb = render->create(bd);
        auto triangle_ib = render->create(id);



        fay::shader_desc sd; {
            sd.name = "shd";
            sd.vs = R"(
                #version 330 core
                layout (location = 0) in vec3 aPos;

                void main()
                {
                   gl_Position = vec4(aPos.x, aPos.y, aPos.z, 1.0);
                }
            )";
            sd.fs = R"(
                #version 330 core
                out vec4 FragColor;

                layout (std140) uniform color
                {
                    vec4 a;
                    vec4 b;
                };
                
                uniform int flag;

                void main()
                {
                   if(flag == 1)
                       FragColor = a;
                   else
                       FragColor = b;

                   //FragColor = vec4(1.0f, 0.5f, 0.2f, 1.0f);
                }
            )";

            sd.layout = { { fay::attribute_usage::position, fay::attribute_format::float3 } };
            sd.uniform_blocks = { { "color", sizeof(render_paras) } };
        }
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

        render_paras paras;
        paras.a = { 1.f, 0.f, 0.f, 1.f };
        paras.b = { 0.f, 1.f, 0.f, 1.f };

        {
            pass1
                .begin_default_frame()
                .clear_frame()
                .apply_shader(shd_id)
                .bind_uniform_block("color", fay::memory{ (uint8_t*)&paras, sizeof(render_paras) })

                .apply_pipeline(pipe_id)
                .bind_vertex(line_strip_vb)
                .bind_uniform("flag", 1)
                .draw()

                .apply_pipeline(pipe2_id)
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
        render->submit(pass1);
        render->execute();
    }

    fay::buffer_id buf_id;
    fay::command_list pass1, pass2;
};

/*
fay::app_desc texture_desc;
class texture : public fay::app
{
public:
    // using fay::app;
    texture(const fay::app_desc& desc) : fay::app(desc)
    {
        desc_.window.title = "texture";
    }

    void init() override
    {
        render = fay::create_device_opengl(fay::g_config);

        // set up vertex data (and buffer(s)) and configure vertex attributes
        // ------------------------------------------------------------------
        float vertices[] = {
             0.5f,  0.5f, 0.0f,  // top right
             0.5f, -0.5f, 0.0f,  // bottom right
            -0.5f, -0.5f, 0.0f,  // bottom left
            -0.5f,  0.5f, 0.0f   // top left
        };
        unsigned int indices[] = {  // note that we start from 0!
            0, 1, 3,  // first Triangle
            1, 2, 3   // second Triangle
        };

        fay::buffer_desc bd; {
            bd.size = 4;// sizeof(vertices);
            bd.stride = 12; // TODO: do it by helper functions;
            bd.data = vertices;
            bd.type = fay::buffer_type::vertex;

            bd.layout = { {fay::attribute_usage::position, fay::attribute_format::float3} };
        }
        auto vertex_id = render->create(bd);

        fay::buffer_desc bd2; {
            bd2.size = 6;
            bd2.data = indices;
        }
        auto index_id = render->create(bd2);

        fay::shader_desc sd; {
            sd.vs = R"(
                #version 330 core
                layout (location = 0) in vec3 aPos;
                void main()
                {
                   gl_Position = vec4(aPos.x, aPos.y, aPos.z, 1.0);
                }
            )";
            sd.fs = R"(
                #version 330 core
                out vec4 FragColor;
                void main()
                {
                   FragColor = vec4(1.0f, 0.5f, 0.2f, 1.0f);
                }
            )";
        }
        auto shd_id = render->create(sd);

        fay::pipeline_desc pd; {
            pd.primitive_type = fay::primitive_type::line_strip;
        }
        auto pipe_id = render->create(pd);

        fay::pass_desc default_pass; {
           // default_pass.index = index_id;
            default_pass.buffers[0] = vertex_id;
            default_pass.shd_id = shd_id;
            default_pass.pipe_id = pipe_id;
        }
        pass = default_pass;

        // uncomment this call to draw in wireframe polygons.
        //glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    }

    void update() override
    {
        render->draw(pass);
    }

    void clear() override
    {

    }

    fay::pass_desc pass;
    fay_device_ptr render{};
};
*/



int main(int argc, char** argv)
{
    google::InitGoogleLogging(argv[0]);
    //--stderrthreshold=0 --logtostderr=true
    //FLAGS_logtostderr = true;
    //FLAGS_stderrthreshold = 0;
    //FLAGS_v = 2;

    // vector<{XX_desc, XX}> ...;
    fay::app_desc desc;
    {
        //
    }
    triangle app_{ desc };
    return app_.run();
}