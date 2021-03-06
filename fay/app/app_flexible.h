#pragma once

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include "fay/app/app.h"
#include "fay/core/config.h"
#include "fay/core/fay.h"
#include "fay/gfx/camera.h"
#include "fay/gfx/frame.h"
#include "fay/gfx/light.h"
#include "fay/gfx/mesh.h"
#include "fay/render/define.h"
#include "fay/render/device.h"
#include "fay/render/shader.h"
#include "fay/resource/image.h"
#include "fay/resource/model.h"

namespace fay
{

using buffer_sp = std::shared_ptr<buffer_id>;
inline buffer_sp create_buffer_sp(render_device* device, const buffer_desc desc)
{
    auto id = device->create(desc);
    return buffer_sp(new buffer_id(id), [device](const buffer_id* ptr) { device->destroy(*ptr); delete ptr; });
}

// -------------------------------------------------------------------------------------------------

// inline app_desc global_desc;

inline renderable_sp create_box_mesh(std::array<glm::vec3, 8> box_mesh, render_device* device)
{
    resource_mesh mesh;
    mesh.size = 8;
    mesh.layout = vertex_layout{
        {fay::attribute_usage::position,  fay::attribute_format::float3}
    };

    uint byte_size = mesh.size * mesh.layout.stride();
    mesh.vertices.reserve(byte_size);
    mesh.vertices.resize(byte_size);
    std::memcpy(mesh.vertices.data(), box_mesh.data(), byte_size);

    mesh.indices =
    {
        0, 1, 1, 2, 2, 3, 3, 0,
        4, 5, 5, 6, 6, 7, 7, 4,
        0, 4, 1, 5, 2, 6, 3, 7
    };
    mesh.primitive_ = primitive_type::lines;

    return std::make_shared<raw_mesh>(device, mesh);
}

template<typename Box>
inline renderable_sp create_box_mesh(const Box& box, render_device* device)
{
    std::array<glm::vec3, 8> box_mesh
    {
        box.corner(box_corner::I),
        box.corner(box_corner::II),
        box.corner(box_corner::III),
        box.corner(box_corner::IV),
        box.corner(box_corner::V),
        box.corner(box_corner::VI),
        box.corner(box_corner::VII),
        box.corner(box_corner::VIII),
    };

    return create_box_mesh(box_mesh, device);
}

inline renderable_sp create_raw_renderable(const std::string& model_path, render_device* device)
{
    auto model = create_resource_model(model_path, device->type());

    std::vector<renderable_sp> mesh_list;

    for (const auto& mesh : model->meshes())
    {
        mesh_list.emplace_back(std::make_shared<raw_mesh>(device, mesh));
    }

    return std::make_shared<array_mesh>(mesh_list);
}

inline std::vector<renderable_sp> create_renderables(const resource_model& model, render_device* device)
{
    std::vector<material_sp> material_list;
    std::vector<renderable_sp> mesh_list;

    for (const auto& mat : model.materials())
    {
        material_list.emplace_back(std::make_shared<material>(device, mat));
    }

    for (const auto& mesh : model.meshes())
    {
        mesh_list.emplace_back(std::make_shared<static_mesh>(device, mesh, material_list[mesh.material_index]));
    }

    return mesh_list;
}

inline std::vector<renderable_sp> create_renderables(const std::string& model_path, render_device* device)
{
    auto model = create_resource_model(model_path, device->type());
    return create_renderables(*model, device);
}

inline renderable_sp create_renderable(const std::string& model_path, render_device* device)
{
    auto meshes = create_renderables(model_path, device);
    return std::make_shared<array_mesh>(meshes);
}

// -------------------------------------------------------------------------------------------------

inline shader_id create_shader(render_device* device, const std::string name, const std::string vs, const std::string fs)
{
    auto desc = fay::scan_shader_program(name, vs, fs, device->type()); // TODO: device->backend();
    return device->create(desc);
}

// -------------------------------------------------------------------------------------------------

// TODO: remove
inline texture_desc create_texture_desc(
    const std::string& name, 
    uint width, 
    uint height, 
    fay::pixel_format fmt, 
    uint pixel_bytesize,
    wrap_mode wrap, 
    filter_mode filter)
{
    fay::texture_desc desc;

    // TODO

    return desc;
}

// most of the time, depth-stencil attachment is written only.
inline texture_id create_depth_stencil_map(render_device* device, const std::string& name, uint width, uint height)
{
    texture_desc desc;

    desc.name = name;
    desc.width = width;
    desc.height = height;
    desc.data = { nullptr };
    desc.type = texture_type::two;
    desc.mipmap = false; // TODO: default set it false

    desc.min_filter = filter_mode::nearest;
    desc.max_filter = filter_mode::nearest;
    desc.wrap_u = wrap_mode::repeat;
    desc.wrap_v = wrap_mode::repeat;


    desc.as_render_target = render_target::depth_stencil;
    desc.format = pixel_format::depthstencil; // rename: depth_stencil
    desc.size = width * height * 4;
    
    // stupid bug
    // auto ds_id = device->create(desc);

    return device->create(desc);
}

// could used in shadow map
inline texture_id create_readable_depth_map(const std::string& name, uint width, uint height, render_device* device)
{
    texture_desc desc;

    desc.name = name;
    desc.width = width;
    desc.height = height;
    desc.data = { nullptr };
    desc.type = texture_type::two;

    desc.min_filter = filter_mode::nearest;
    desc.max_filter = filter_mode::nearest;
    desc.wrap_u = wrap_mode::repeat;
    desc.wrap_v = wrap_mode::repeat;
    desc.mipmap = false;

    desc.as_render_target = render_target::depth;
    desc.format = pixel_format::depth; // r32f,float
    desc.size = width * height * 4; // byte size
    auto ds_id = device->create(desc);

    return device->create(desc);
}

// TODO: remove
inline texture_id create_2d(render_device_ptr& device, const std::string& name, const image& img, bool as_target = false)
{
    texture_desc desc;

    desc.name = name;
    desc.width = img.width();
    desc.height = img.height();
    desc.depth = 1;
    desc.format = img.format();

    desc.size = img.size() * img.pixel_size();
    //if (get_filetype(std::string(img.filepath())) == "hdr") // TODO: get_filetype(std::string_view)
    desc.data = { img.data() };

    desc.type = texture_type::two;

    desc.as_render_target = as_target ? render_target::color : render_target::none;

    return device->create(desc);
}

inline texture_id create_texture(render_device* device, const std::string& filepath, bool as_target = false)
{
    fay::image img(filepath);
    texture_desc desc(img.name(), img.width(), img.height(), img.format(), texture_type::two, { img.data() });

    // not need
    // desc.size = img.size() * img.pixel_size();
    // desc.as_render_target = as_target ? render_target::color : render_target::none;

    return device->create(desc);
}

inline fay::texture_id create_cubemap(render_device* device, const std::string& name, uint width, uint height, fay::pixel_format fmt, uint pixel_bytesize, bool mipmap = false, bool as_target = false) // TODO: compute pixel_bytesize by help func
{
    fay::texture_desc desc;
    desc.name = name;
    desc.width = width;
    desc.height = height;
    desc.depth = 1;
    desc.type = fay::texture_type::cube;
    desc.format = fmt;

    desc.size = width * height * pixel_bytesize; // TODO: do it by help func
    desc.data = { nullptr, nullptr, nullptr, nullptr, nullptr, nullptr };

    desc.wrap_u = wrap_mode::clamp_to_edge;
    desc.wrap_v = wrap_mode::clamp_to_edge;
    desc.wrap_w = wrap_mode::clamp_to_edge;
    desc.mipmap = mipmap;

    desc.as_render_target = as_target ? render_target::color : render_target::none;

    return device->create(desc);
}

/*
inline fay::texture_id create_cubemap(render_device* device, const std::string& name, const image& img) // TODO: compute pixel_bytesize by help func
{
    // FIXME
    return create_cubemap(device, name, img.width(), img.height(), img.format(), img.pixel_size());
}
*/

// -------------------------------------------------------------------------------------------------

inline frame create_cubemap_frame(render_device* device, const std::string& name, uint width, uint height, fay::pixel_format fmt, uint pixel_size, bool mipmap = false)
{
    texture_id color_id = create_cubemap(device, name, width, height, fmt, pixel_size, mipmap);
    texture_id ds_id = create_depth_stencil_map(device, name, width, height);

    frame_desc fd;
    fd.name = name;
    fd.width = width;
    fd.height = height;

    fd.render_targets =
    { 
        { color_id, 0, 0 },
        { color_id, 1, 0 },
        { color_id, 2, 0 },
        { color_id, 3, 0 },
        { color_id, 4, 0 },
        { color_id, 5, 0 },
    };
    fd.depth_stencil = { ds_id, 0, 0 };

    auto frm_id = device->create(fd);
    return { frm_id, color_id, ds_id };
}

// TODO: better way
inline frame create_mipmap_cubemap_frame(render_device* device, const std::string& name, uint width, uint height, texture_id color_id, texture_id ds_id, uint32_t level)
{
    frame_desc fd;
    fd.name = name;
    fd.width = width;
    fd.height = height;

    fd.render_targets =
    {
        { color_id, 0, level },
        { color_id, 1, level },
        { color_id, 2, level },
        { color_id, 3, level },
        { color_id, 4, level },
        { color_id, 5, level },
    };
    fd.depth_stencil = { ds_id, 0, 0 };

    auto frm_id = device->create(fd);
    return { frm_id, color_id, ds_id };
}

inline frame create_depth_frame(const std::string& name, uint width, uint height, render_device* device)
{
    texture_desc desc;

    desc.name = name;
    desc.width = width;
    desc.height = height;
    desc.data = { nullptr };
    desc.type = texture_type::two;

    desc.min_filter = filter_mode::nearest;
    desc.max_filter = filter_mode::nearest;
    desc.wrap_u = wrap_mode::repeat;
    desc.wrap_v = wrap_mode::repeat;
    desc.mipmap = false;

    desc.as_render_target = render_target::color;
    desc.format = pixel_format::rgba32f; // for debug
    desc.size = width * height * 16; // byte size
    auto color_id = device->create(desc);

    // ???
    desc.as_render_target = render_target::depth;
    desc.format = pixel_format::depth; // r32f,float
    desc.size = width * height * 4; // byte size
    auto ds_id = device->create(desc);

    frame_desc fd;
    fd.name = name;
    fd.width = width;
    fd.height = height;

    // TODO: depth frame doesn't need color frame
    fd.render_targets = { { color_id, 0, 0 } };
    fd.depth_stencil = { ds_id, 0, 0 };
    auto frm_id = device->create(fd);

    return { frm_id, color_id, ds_id };
}

inline frame create_frame(render_device* device, const std::string& name, uint width, uint height, pixel_format fmt = pixel_format::rgba8)
{
    texture_desc desc;
    desc.name = name;
    desc.width = width;
    desc.height = height;
    desc.depth = 1; // TODO
    desc.size = width * height * 4 * (fmt == pixel_format::rgba8 ? 1 : 4); // WARNNING
    desc.data = { nullptr };
    desc.type = texture_type::two;
    desc.mipmap = false;

    desc.as_render_target = render_target::color;
    desc.format = fmt;
    auto color_id = device->create(desc);

    desc.as_render_target = render_target::depth_stencil;
    desc.format = pixel_format::depthstencil; // TODO: depth_stencil;
    auto ds_id = device->create(desc);

    frame_desc fd;
    fd.name = name;
    fd.width = width;
    fd.height = height;
    fd.render_targets = { { color_id, 0, 0 } };
    fd.depth_stencil = { ds_id, 0, 0 };
    auto frm_id = device->create(fd);

    return { frm_id, color_id, ds_id };
}

inline frame create_OITbuffer(render_device* device, const std::string& name, uint width, uint height)
{
    texture_desc desc;

    desc.name = name;
    desc.width = width;
    desc.height = height;
    desc.size = width * height * 4; // byte size
    desc.data = { nullptr };
    desc.type = texture_type::two;

    desc.min_filter = filter_mode::linear;
    desc.max_filter = filter_mode::linear;
    desc.wrap_u = wrap_mode::repeat;
    desc.wrap_v = wrap_mode::repeat;
    desc.mipmap = false;

    desc.as_render_target = render_target::color;
    desc.format = pixel_format::rgba32f;
    auto color_id = device->create(desc);

    desc.as_render_target = render_target::color;
    desc.format = pixel_format::rgba32f;
    auto color_id2 = device->create(desc);

    desc.as_render_target = render_target::depth_stencil;
    desc.format = pixel_format::depthstencil; // TODO: depth_stencil;
    auto ds_id = device->create(desc);

    frame_desc fd;
    fd.name = name;
    fd.width = width;
    fd.height = height;
    fd.render_targets = { { color_id, 0, 0 }, { color_id2, 0, 0 } };
    fd.depth_stencil = { ds_id, 0, 0 };
    auto frm_id = device->create(fd);

    return { frm_id, { color_id, color_id2 }, ds_id };
}

inline frame create_Gbuffer(render_device* device, const std::string& name, uint width, uint height)
{
    texture_desc desc;

    desc.name = name;
    desc.width = width;
    desc.height = height;
    desc.size = width * height * 4; // byte size
    desc.data = { nullptr };
    desc.type = texture_type::two;

    desc.min_filter = filter_mode::nearest;
    desc.max_filter = filter_mode::nearest;
    desc.wrap_u = wrap_mode::repeat;
    desc.wrap_v = wrap_mode::repeat;
    desc.mipmap = false;

    desc.as_render_target = render_target::color;
    desc.format = pixel_format::rgba32f;
    auto color_id = device->create(desc);

    desc.as_render_target = render_target::color;
    desc.format = pixel_format::rgb32f;
    auto color_id2 = device->create(desc);

    desc.as_render_target = render_target::color;
    desc.format = pixel_format::rgba8;
    auto color_id3 = device->create(desc);

    desc.format = pixel_format::rgba32f;
    auto color_id4 = device->create(desc);

    desc.as_render_target = render_target::depth_stencil;
    desc.format = pixel_format::depthstencil; // TODO: depth_stencil;
    auto ds_id = device->create(desc);

    frame_desc fd;
    fd.name = name;
    fd.width = width;
    fd.height = height;
    fd.render_targets = { { color_id, 0, 0 }, { color_id2, 0, 0 }, { color_id3, 0, 0 }, { color_id4, 0, 0 } };
    fd.depth_stencil = { ds_id, 0, 0 };
    auto frm_id = device->create(fd);

    return { frm_id, { color_id, color_id2, color_id3, color_id4 }, ds_id };
}

// -------------------------------------------------------------------------------------------------
// IO

const inline std::string Box        = "model/box/scene.obj";
const inline std::string Blocks     = "model/blocks/scene.obj";
const inline std::string CornellBox = "model/CornellBox/CornellBox.obj";
const inline std::string dancing    = "model/silly_dancing.fbx";
const inline std::string Face       = "model/face/face.obj";
const inline std::string Fairy      = "model/fairy/fairy.obj";
const inline std::string ftm        = "model/ftm/ftm_sketchfab.blend";
const inline std::string LightBulb  = "model/LightBulb/LightBulb.obj";
const inline std::string Planet     = "model/planet/planet.obj";
const inline std::string Plants     = "model/plants/scene.obj";
const inline std::string Rei        = "model/Rei/Rei.obj";
const inline std::string Rock       = "model/rock/rock.obj";
const inline std::string Nanosuit   = "model/nanosuit/nanosuit.obj";
const inline std::string Nier_2b_0  = "model/nierautomata_2b/scene.gltf";
const inline std::string Nier_2b_1  = "model/Nier_2b/2b.obj";
const inline std::string Nier_2b_2  = "model/Nier_2b_ik_rigged/scene.gltf";
const inline std::string Sphere     = "model/sphere/scene.obj";
const inline std::string Sponza     = "model/sponza/sponza.obj";
const inline std::string Gallery    = "model/gallery/gallery.obj";
const inline std::string Rungholt   = "model/rungholt/rungholt.obj";
const inline std::string PowerPlant = "model/powerplant/powerplant.obj";
const inline std::string SanMiguel  = "model/San_Miguel/san-miguel-low-poly.obj";
//const inline std::string shadows  = "model/shadows/shadows.obj";

struct render_paras
{
    glm::vec4 a{};
    glm::vec4 b{};
};

} // namespace fay