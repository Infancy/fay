#include "fay/core/range.h"
#include "fay/core/utility.h"
#include "fay/render/device.h"

namespace fay
{

render_backend_ptr create_backend_opengl(const render_desc& desc);

render_device::render_device(const render_desc& desc)
{
    switch (desc.render_backend_type)
    {
        case render_backend_type::opengl:
            backend_ = std::move(create_backend_opengl(desc));
            break;
        case render_backend_type::opengl_dsa:
            //break;
        case render_backend_type::d3d11:
            //break;
        case render_backend_type::none:
        default:
            LOG(ERROR) << "render_device: no render_backend";
            break;
    }

    context_.name = desc.name;
    context_.type = desc.render_backend_type;

    ctx_.pipe_id = create(pipeline_desc());
    ctx_.pipe = desc_[ctx_.pipe_id];
    // make device have a old_pipe
    backend_->apply_pipeline(ctx_.pipe_id, {true, true, true, true});

    //execute(command_list()
    //    .begin_default_frame() // TODO: other backend can bind default without init???
    //    .apply_pipeline(context_.default_pipe)
    //    .end_frame());
}

void render_device::apply_pipeline(const pipeline_id id)
{
    DCHECK(query_valid(id)) << "invalid id";

    // !!!!!!!!!!!!!!!!!!!!
    if (ctx_.pipe_id == id)
        return;

    const auto& old = ctx_.pipe;
    ctx_.pipe_id = id;
    ctx_.pipe = desc_[ctx_.pipe_id];
    const auto& now = ctx_.pipe;
    
    std::array<bool, 4> flags{};

    if (old.primitive_type != now.primitive_type)
    {
        flags[0] = true;
    }

    if ((old.alpha_to_coverage_enabled != now.alpha_to_coverage_enabled) ||
        (old.cull_mode                 != now.cull_mode) ||
        (old.face_winding              != now.face_winding) ||
        (old.rasteriza_sample_count    != now.rasteriza_sample_count) ||
        (old.depth_bias                != now.depth_bias) ||
        (old.depth_bias_slope_scale    != now.depth_bias_slope_scale) ||
        (old.depth_bias_clamp          != now.depth_bias_clamp))
    {
        flags[1] = true;
    }

    if ((old.depth_enabled       != now.depth_enabled) ||
        (old.depth_compare_op    != now.depth_compare_op) ||
        (old.stencil_enabled     != now.stencil_enabled) ||
        (old.stencil_front       != now.stencil_front) ||
        (old.stencil_back        != now.stencil_back) ||
        (old.stencil_test_mask   != now.stencil_test_mask) ||
        (old.stencil_write_mask  != now.stencil_write_mask) ||
        (old.stencil_ref         != now.stencil_ref))
    {
        flags[2] = true;
    }

    if ((old.blend_enabled          != now.blend_enabled) ||
        (old.blend_rgb              != now.blend_rgb) ||
        (old.blend_alpha            != now.blend_alpha) ||
        (old.blend_write_mask       != now.blend_write_mask) ||
        (old.color_attachment_count != now.color_attachment_count) ||
        (old.blend_color_format     != now.blend_color_format) ||
        (old.blend_depth_format     != now.blend_depth_format) ||
        (old.blend_color            != now.blend_color)) // TODO???: float compare
    {
        flags[3] = true;
    }

    //if (any_of(flags, std::equal_to{}, true))
    backend_->apply_pipeline(id, flags);
}

void render_device::bind_buffer(const buffer_id id, const std::vector<attribute_usage>& attrs, uint32_t instance_rate)
{
    DCHECK(query_valid(id)) << "invalid id";
    DCHECK(instance_rate >= 0);

    if(instance_rate == 0)
        ctx_.vertex_count = desc_[id].size; // WARNNING: ???

    std::vector<uint32_t> attrs_, slots_;

    const auto& buf_layout = desc_[id].layout;
    const auto& shd_layout = ctx_.shd.layout;

    if (attrs.size() == 0)
    {
        int num = std::min(buf_layout.size(), shd_layout.size());

        attrs_.resize(num);
        slots_.resize(num);

        for (auto i : range(num))
        {
            attrs_[i] = i;
            slots_[i] = i;
        }
    }
    else
    {
        DCHECK(attrs.size() < ctx_.shd.layout.size());

        for (auto usage : attrs)
        {
            auto idx0 = index(buf_layout, [usage](const vertex_attribute& attr)
            {
                return attr.usage == usage;
            });
            DCHECK(idx0.has_value()) << "invaild vertex attribute";

            auto idx1 = index(shd_layout, [usage](const vertex_attribute& attr)
            {
                return attr.usage == usage;
            });
            DCHECK(idx1.has_value()) << "invaild vertex attribute";

            DCHECK(buf_layout[idx0.value()] == shd_layout[idx1.value()]) << "not same vertex attribute";

            attrs_.push_back(idx0.value());
            slots_.push_back(idx1.value());
        }
    }

    backend_->bind_vertex(id, attrs_, slots_, instance_rate);
    return;
}



void render_device::execute_command_list(const command_list& cmds)
{
    const auto& cs = cmds.commands_();

    DCHECK(cs.size() >= 2);
    DCHECK(
        ((cs.front().type_ == command_type::begin_default_frame) || (cs.front().type_ == command_type::begin_frame)) &&
        (cs.back().type_ == command_type::end_frame))
        << "first cmd must be begin_xx_frame and last cmd must be end_frame";

    DCHECK((cs.size() > 2) && std::none_of(++cs.cbegin(), --cs.cend(), [](const command& cmd) 
    {
        return 
            (cmd.type_ == command_type::begin_default_frame) ||
            (cmd.type_ == command_type::begin_frame) ||
            (cmd.type_ == command_type::end_frame);
    })) << "cmd in the middle can't be begin_xx_frame or end_frame";

    // TOCHECK

    ctx_ = {};

    for (const auto& cmd : cs)
        execute_command(cmd);
}

void render_device::execute_command(const command& cmd)
{
    switch (cmd.type_)
    {
        case command_type::begin_default_frame:

            begin_default_frame();
            break;

        case command_type::begin_frame:

            begin_frame(cmd.frm_);
            break;

        case command_type::end_frame:

            end_frame();
            break;

        case command_type::clear_color:

            clear_color(cmd.float4_, cmd.uints_);
            break;

        case command_type::clear_depth:

            clear_depth(cmd.float_);
            break;

        case command_type::clear_stencil:

            clear_stencil(cmd.uint_);
            break;

        case command_type::set_viewport:

            set_viewport(cmd.uint4_.x, cmd.uint4_.y, cmd.uint4_.z, cmd.uint4_.w);
            break;

        case command_type::set_scissor:

            set_scissor(cmd.uint4_.x, cmd.uint4_.y, cmd.uint4_.z, cmd.uint4_.w);
            break;

        case command_type::apply_shader:

            apply_shader(cmd.shd_);
            break;

        case command_type::apply_pipeline:

            apply_pipeline(cmd.pip_);
            break;

        case command_type::bind_index:

            bind_index(cmd.buf_);
            break;

        case command_type::bind_buffer:

            bind_buffer(cmd.buf_, cmd.attrs_, cmd.uint_);
            break;

        case command_type::bind_named_texture:

            bind_texture(cmd.tex_, ctx_.tex_unit++, cmd.str_);
            break;

        case command_type::bind_textures:

            bind_texture(cmd.texs_);
            break;

        case command_type::bind_uniform:

            bind_uniform(cmd.str_, cmd.uniform_);
            break;

        case command_type::bind_uniform_block:

            bind_uniform(cmd.str_, cmd.uniform_block_.data(), cmd.uniform_block_.size());
            break;

        case command_type::draw:

            draw(cmd.uint4_.x, cmd.uint4_.y, cmd.uint4_.z);
            break;

        case command_type::draw_index:

            draw_index(cmd.uint4_.x, cmd.uint4_.y, cmd.uint4_.z);
            break;

        default:

            LOG(ERROR) << "shouldn't be here";
            break;
    }
}

} // namespace fay