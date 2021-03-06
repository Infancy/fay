#pragma once

#include <glm/glm.hpp>

#include "fay/core/fay.h"
#include "fay/render/define.h"
#include "fay/render/device.h"
#include "fay/resource/define.h"

namespace fay
{


struct material // : std::enabled_shared_from_this<material>
{
    material(fay::render_device* device, const resource_material& res)
    {
        name = res.name;

        if (!res.base_color.empty())             textures[0] = create_2d_(device, name + "_base_color",         texture_usage::base_color,             res.base_color);
        if (!res.metallic_roughness.empty()) textures[2] = create_2d_(device, name + "_metallic_roughness", texture_usage::metallic_roughness, res.metallic_roughness);
        if (!res.normal.empty())             textures[1] = create_2d_(device, name + "_normal",             texture_usage::normal,             res.normal);
        if (!res.occlusion.empty())          textures[3] = create_2d_(device, name + "_occlusion",          texture_usage::occlusion,          res.occlusion);
        if (!res.emissive.empty())           textures[4] = create_2d_(device, name + "_emissive",           texture_usage::emissive,           res.emissive);

        albedo = res.base_factor;
        emissive = res.emissive_factor;
        metallic = res.metallic_factor;
        roughness = res.roughness_factor;
        normal = res.normal_factor;
        //pipeline = res.pipeline;
        two_sided = res.two_sided;
        //shader_variant = res.bandlimited_pixel ? 1 : 0;
    }

    texture_id create_2d_(render_device* device, const std::string& name, texture_usage usage, const image& img)
    {
        fay::texture_desc desc;

        desc.name = name;
        desc.width = img.width();
        desc.height = img.height();
        desc.depth = 1;
        desc.format = img.format();

        desc.size = img.size() * img.channel();
        desc.data = { img.data() };

        desc.type = fay::texture_type::two;
        desc._usage = usage;

        return device->create(desc);
    }

    virtual ~material() = default;


    std::string name{};

    std::array<texture_id, 5> textures{};

    glm::vec4 albedo{ 1.f };
    float metallic = 1.f;
    float roughness = 1.f;
    float normal = 1.f;
    glm::vec3 emissive{ 0.f };

    uint32_t shader_variant = 0;
    //DrawPipeline pipeline = DrawPipeline::Opaque;
    bool two_sided = false;
    bool needs_emissive = false;
};

FAY_SHARED_PTR(material)

} // namespace fay