// #define SAMPLE_RENDER_MAIN

#include "fay/app/app.h"
#include "fay/core/fay.h"

#include "sample_render_app.h"

// shadow, defer_render

// extern class shadow shadow_app;

SAMPLE_RENDER_APP_DECL(__sample)

SAMPLE_RENDER_APP_DECL(init)
SAMPLE_RENDER_APP_DECL(clear)
SAMPLE_RENDER_APP_DECL(shader_pipeline_)
SAMPLE_RENDER_APP_DECL(vertex_index_)
SAMPLE_RENDER_APP_DECL(triangle)

SAMPLE_RENDER_APP_DECL(instancing)
SAMPLE_RENDER_APP_DECL(offscreen)

SAMPLE_RENDER_APP_DECL(FXAA)

SAMPLE_RENDER_APP_DECL(WBOIT)

SAMPLE_RENDER_APP_DECL(cascade_shadow_map) // rename: CSM // fixme: missing shadow in some viewing angle

SAMPLE_RENDER_APP_DECL(defered_shading)

SAMPLE_RENDER_APP_DECL(SSR)

SAMPLE_RENDER_APP_DECL(PBR)
SAMPLE_RENDER_APP_DECL(IBL)

int main(int argc, char** argv)
{
    //FLAGS_logtostderr = true;
    //FLAGS_stderrthreshold = 0;
    //FLAGS_v = 2;
    //--stderrthreshold=0 --logtostderr=true
    FLAGS_colorlogtostderr = true;
    google::InitGoogleLogging(argv[0]);

    // vector<{XX_desc, XX}> ...;
    fay::app_desc desc;
    {
        //desc.render.backend = fay::render_backend_type::opengl;
        //desc.render.backend = fay::render_backend_type::d3d11;
    }
    auto  app_ = CREATE_SAMPLE_RENDER_APP(vertex_index_, desc);
    return app_->run();
}