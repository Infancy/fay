#pragma once

#include <variant>

#include "glm/glm.hpp"

#include "fay/core/enum.h"
#include "fay/core/define.h"
#include "fay/core/fay.h"
#include "fay/core/memory.h"
#include "fay/core/range.h"
#include "fay/core/utility.h"

namespace fay
{

#pragma region const

// enum { ... };
// TODO: const_t, const32_t, constant...
constexpr inline uint InvalidId               = 0;
constexpr inline uint NumInflightFrames       = 2;
constexpr inline uint MaxColorAttachments     = 6;

constexpr inline uint MaxShaderStages         = 2;
constexpr inline uint MaxShaderBuffers        = 4;
constexpr inline uint MaxShaderTextures       = 12;
constexpr inline uint MaxShaderUniformBlocks  = 4;

constexpr inline uint MaxUniformBlocksMembers = 16;
constexpr inline uint MaxVertexAttributes     = 16;
constexpr inline uint MaxMipmaps              = 16;
constexpr inline uint MaxTextureArrayLayers   = 128;

#pragma endregion const

#pragma region enum

// -------------------------------------------------------------------------------------------------
// render features

enum class anti_aliasing
{
    none,
    //SSAA, // Super Sample Anti-Aliasing
    MSAA, // Multi Sample Anti-Aliasing, hardware built-in AA, will reduce some frame rates

    // Post Processing AA(PPAA)
    FXAA, // Fast approXimate Anti-Aliasing
    SMAA, // Sub-pixel Morphological Anti-Aliasing
    //TXAA,
};

// -------------------------------------------------------------------------------------------------
// render types

// A resource usage hint describing the update strategy of buffers and images.
// rename: ...
enum class resource_usage
{
    // none,
	immutable, // initialize at creation and then never changed, this is the default strategy
	dynamic,   // 
	stream,    // updated each frame
};

// TODO
enum class resource_state
{
	usable,
	alloc,
	//initial,
	valid,
	invalid,
};

// buffer enum

enum class buffer_type
{
    none,

	vertex,
	//index16,
	index,
    instance,

    indirect = 0x00000004,

    uniform_cbv = 0x00000020,

    storage_srv = 0x00000040,
    storage_uav = 0x00000080,

    uniform_texel_srv = 0x00000100,
    storage_texel_uav = 0x00000200,

    counter_uav = 0x00000400,

    transfer_src_ = 0x00000008,
    transfer_dst_ = 0x00000010,

    raytracing_accel_,
};
FAY_ENUM_CLASS_OPERATORS(buffer_type)

enum class /*vertex_*/attribute_usage
{
    // position2d,
    position, 
    normal,

    tangent,
    bitangent,

    bone_index,
    bone_weight,

    color0,
    color1,
    color2,
    color3,

    texcoord0,
    texcoord1,
    texcoord2,
    texcoord3,

    // instance
    instance_model,

    unknown0,
    unknown1,
    unknown2,
    unknown3,
}; 

enum class /*vertex_*/attribute_format
{
    float1,
    float2,
    float3,
    float4,
    // floatx, could use float1 * 16 or float4 * 4

    // WARNING: The following type in opengl maybe need to normalized
    short2,
    short4,

    byte4,
    ubyte4,

    // uint10_x2,
};

// texture enum

enum class texture_type
{
    one,
	two,
	three,
    cube,
	array,
	//data,	// ues 2d texture as array of data
	//array_data
};

// TODO
struct texture_type_ // : enum_class<texture_type_>
{
    enum enums
    {
        none,
        two,
        cube,
        three,
        array
    };

    texture_type_() = default;
    texture_type_(enums e) : e{ e } {}

    uint faces() { return e == cube ? 6 : 1; }

    bool is_mutil()
    {
        return (e == three) || (e == array);
    }

    enums e;
};

// TODO: rename or remove
enum class texture_usage
{
    base_color,
    metallic_roughness,
    normal,
    occlusion,
    emissive, 
    
    unknown0,
    unknown1,
    unknown2,
    unknown3,
};

enum class texture_usage_
{
    undefined = 0x00000000,

    sampled_texture = 0x00000004,
    storage_texture = 0x00000008,

    color_attachment = 0x00000010,
    depth_stencil_attachment = 0x00000020,

    present = 0x00000100,

    resolve_src_ = 0x00000040,
    resolve_dst_ = 0x00000080,

    transfer_src_ = 0x00000001,
    transfer_dst_ = 0x00000002,
};
FAY_ENUM_CLASS_OPERATORS(texture_usage_)

enum class render_target
{
    none          = 0b0000,
    color         = 0b0001,
    depth         = 0b0010,
    stencil       = 0b0100,
    depth_stencil = 0b1000,
    depth_or_stencil = depth | stencil | depth_stencil, // ???
    DepthStencil  = depth | stencil | depth_stencil
};
FAY_ENUM_CLASS_OPERATORS(render_target)

enum class filter_mode
{
	anisotropy, // only for d3d11

	nearest,
    nearest_mipmap_nearest,
    nearest_mipmap_linear,

	linear,
	linear_mipmap_nearest,
	linear_mipmap_linear,
};

enum class wrap_mode
{
	repeat,
	clamp_to_edge,
	mirrored_repeat,
};

enum class cube_face
{
	pos_x, // 0
	neg_x, // 1
	pos_y, // 2
	neg_y, // 3
	pos_z, // 4
	neg_z, // 5
};

// core/define/
// enum class pixel_format

// shader enum

enum class shader_stage
{
    none,

    //
	vertex,
    hull,
    domn,
	geometry,

    task,
    mesh,

	fragment,

    //
    compute,

    //
    ray_gen,
    ray_miss,
    ray_intersect,
    ray_any_hit,
    ray_hit,
};

enum class uniform_type
{
	none,

    bvec1, // bool
    bvec2,
    bvec3,
    bvec4,

    ivec1, // int
    ivec2,
    ivec3,
    ivec4,

    uvec1, // uint
    uvec2,
    uvec3,
    uvec4,

    vec1, // float
    vec2,
    vec3,
    vec4,

    mat2,
    mat3,
	mat4,

    mat2x2,
    mat2x3,
    mat2x4,
    mat3x2,
    mat3x3,
    mat3x4,
    mat4x2,
    mat4x3,
    mat4x4,

    // double
};

// pipeline enum

enum class pipeline_type
{
	rasterization,
	raytracing,
	compute,
};

enum class primitive_type
{
	points,
	lines,
	line_strip,
	triangles,
	triangle_strip,
};

enum class face_winding
{
	ccw, // Counter-ClockWise order
	cw,  // ClockWise order
};

enum class cull_mode
{
	none,
	front, // cw
	back,  // ccw
};

enum class fill_mode
{
    none,
    wireframe,
    solid,
};

// used for depth and stencil tests
enum class compare_op
{
	never,

	less,
	less_equal,

	equal,
	not_equal,

	greater,
	greater_equal,

	always,
};

enum class stencil_op
{
	keep,
	zero,
	replace,
	incr_clamp,
	decr_clamp,
	invert,
	incr_wrap,
	decr_wrap,
};

enum class blend_factor
{
	zero,
	one,

	src_color,
	one_minus_src_color,
	src_alpha,
	one_minus_src_alpha,

	dst_color,
	one_minus_dst_color,
	dst_alpha,
	one_minus_dst_alpha,

	src_alpha_saturated,

	blend_color,
	one_minus_blend_color,
	blend_alpha,
	one_minus_blend_alpha,
};

enum class blend_op
{
	add,
	subtract,
	reverse_subtract,
};

enum class blend_mask
{
	none  = 0x10,  // special value for all channels disabled
	red   = 0b0001,
	green = 0b0010,
	blue  = 0b0100,
	alpha = 0b1000,
	rgb   = red | green | blue,
	rgba  = red | green | blue | alpha, // all channels enabled
};
FAY_ENUM_CLASS_OPERATORS(blend_mask)

// pass enum

// ...

enum class pass_type
{
    none     = 0x00,
    graphics = 1 << 0,
    compute  = 1 << 1,
    blit     = 1 << 2,
    all = graphics | compute | blit,
};

#pragma endregion enum

#pragma region desc

// buffer, texture, shader, uniform, pso
// device, context, effect, pass

// base_render_id
// TODO: struct no ctor init, default ctor
// TODO: uint -> size_t
// TODO: bool() -> is_valid()
// TODO: value -> private: value
// TODO: using buffer_id = type_id<buffer, buffer_desc>
// TODO: base_type_id, frame : base_type_id
#define FAY_RENDER_TYPE_ID( type )                                          \
struct type##_id                                                            \
{                                                                           \
    uint value{};                                                           \
    type##_id() : value{ 0 } {}                                             \
    explicit type##_id(uint i) : value{ i } {}                              \
    explicit operator bool() const { return value != 0u; }                  \
    bool operator==(type##_id right) const { return value == right.value; } \
    bool operator!=(type##_id right) const { return !operator==(right); }   \
};

struct render_id                                                            
{                                                                           
    uint value{};                                                           
    explicit render_id(uint i = 0) : value{ i } {}

    explicit operator bool() const { return value != 0u; }                  
    bool operator==(render_id right) const { return value == right.value; } 
    bool operator!=(render_id right) const { return !operator==(right); }
};

FAY_RENDER_TYPE_ID(buffer)
FAY_RENDER_TYPE_ID(texture)
FAY_RENDER_TYPE_ID(accel)
FAY_RENDER_TYPE_ID(respack)
FAY_RENDER_TYPE_ID(shader)
FAY_RENDER_TYPE_ID(pipeline)
FAY_RENDER_TYPE_ID(frame)

#undef FAY_RENDER_TYPE_ID



#pragma region buffer

// a matrix instance : { fay::attribute_usage::instance_matrix,  fay::attribute_format::floatx, 16 }
class vertex_attribute
{
public:
    friend class vertex_layout;

    vertex_attribute() {}
    vertex_attribute(attribute_usage usage, attribute_format format, size_t num = 1, size_t index = 0)
    {
        this->usage_  = usage;
        this->format_ = format;
        this->index_  = index;
        this->num_    = num;
    }

    attribute_usage  usage()  const { return usage_; }
    attribute_format format() const { return format_; }
    size_t           index()  const { return index_; }

    bool operator==(const vertex_attribute& va) const
    {
        return usage_ == va.usage_ && format_ == va.format_ && index_ == va.index_ && num_ == va.num_;
    }

private:
    attribute_usage  usage_{};
    attribute_format format_{};
    size_t           index_{};

    size_t           num_{};
};

struct attribute_detail // rename: attribute_num_bytesize
{
    size_t num; // count
    size_t size; // num * sizeof(T), btsz
};

// TODO: rename vertex_attribute_format_map
// nums, total bytes of attribute_format
const /*inline*/ static enum_class_map<attribute_format, attribute_detail>
attribute_format_map
{
    { attribute_format::float1,  {1,  4} },
    { attribute_format::float2,  {2,  8} },
    { attribute_format::float3,  {3, 12} },
    { attribute_format::float4,  {4, 16} },

    { attribute_format::byte4,   {4, 4} },
    { attribute_format::ubyte4,  {4, 4} },

    { attribute_format::short2,  {2, 4} },
    { attribute_format::short4,  {4, 8} },

    //{ uint10_x2, {1, 4} },
};

/*
class vertex_layout
{
public:
    vertex_layout() {}
    vertex_layout(std::initializer_list<vertex_attribute> il) : attrs(il) {}

    uint size() { return (uint)attrs.size(); }
    bool operator==(const vertex_layout&& layout) const { return attrs == layout.attrs; }

public:
    static inline vertex_layout p3n3t2;

private:

    std::vector<vertex_attribute> attrs{};
};
*/

// TODO: better way
// WARNNNING: vector's dtor isn't virtual
class vertex_layout : public std::vector<vertex_attribute> //extends_sequence<std::vector<vertex_attribute>>
{
public:
    //using extends_sequence<std::vector<vertex_attribute>>::extends_sequence;
    using std::vector<vertex_attribute>::vector;

    vertex_layout(std::initializer_list<vertex_attribute> il)
    {
        for (const auto& attr : il)
        {
            if (attr.num_ == 1)
            {
                push_back({ attr.usage(), attr.format(), 1, 0 });
            }
            else
            {
                for (size_t i : range(attr.num_))
                    push_back({ attr.usage(), attr.format(), attr.num_, i });
            }
        }
    }

    size_t stride() const
    {
        size_t size{};

        for (const auto& attr : *this)
        {
            size += attribute_format_map.at(attr.format()).size;
        }

        return size;
    }
};

// vertex buffer, index buffer, indirect buffer, uniform buffer...
struct buffer_desc
{
public:
    std::string name{};

    buffer_type type{};
    /*legacy*/resource_usage usage{ resource_usage::immutable };

    // TODO: const byte* data/ memory memory
    const void* data{}; // data's length is size * layout.stride()
    int_t       btsz{}; // bytes, byte size, padding it???


    // size, count, num, byte_size, byte_count
    /*legacy*/int_t count{}; // count() * stride() == btsz, size * stride == bytes


    // WARNING: don't use it
    /*legacy*/uint stride{}; // byte sizes of single element. WARNNING: fay can't check if this value is right or not.


    // used for vertex/index, vertex/index nums
    uint size{};
    // used for vertex buffer, instance buffer
    // TODO: improve
    vertex_layout layout{};

    // used for buffer_type::uniform_texel_srv
    pixel_format pixel_format{};


    // used for instance buffer
    // instance buffer update data per instance(or more), instead of updating per vertex.
    // TODO: remove it
    // uint instance_rate{};

public:
    // set/get
    buffer_desc() = default;
    buffer_desc(std::string_view name, uint size, const void* data, buffer_type type = buffer_type::index, vertex_layout layout = {})
        : name{ name }
        , size{ size }
        , data{ std::move(data) }
        , type{ type }
        , layout{ layout }
    {
        switch (type)
        {
            case fay::buffer_type::vertex:
            case fay::buffer_type::instance:
                stride = layout.stride();
                break;
            case fay::buffer_type::index:
                stride = 4;
                break;
            default:
                break;
        }
    }

    uint vertex_index_size()
    {

    }

    bool is_vertex_or_instance() const { return type == buffer_type::vertex || type == buffer_type::instance; }

    //int_t count() { return size; }

private:
    struct vertex_buffer
    {

    };

    struct struct_buffer
    {

    };
};

// buffer_id create_vertex_buffer(...)

#pragma endregion buffer



struct texture_desc
{
public:
    std::string name{ "defult" };

	uint width{};
	uint height{};
    uint depth{}; // or called layers, used in texture3d or texture_array, auto calculation by ctor
    uint mipmaps{ 1 }; // How many levels of mipmaps are generated
    pixel_format format{};

    uint size{}; // used for compressed texture
    // TODO: std::array<const void*, 6> data{};
    std::vector<const void*> data{};

    texture_type type{ texture_type::two };
    /*legacy*/texture_usage _usage{}; // TODO: rename
    texture_usage_ usage_{ texture_usage_::sampled_texture };

	resource_usage usage{ resource_usage::immutable }; // update_rate rate
    // resource_state state{ resource_state::empty };


    filter_mode min_filter{ filter_mode::linear };
	filter_mode max_filter{ filter_mode::linear };
    uint max_anisotropy{ 1 }; // 1 ~ 16
    /*legacy*/bool mipmap{ true }; // TODO: remove

    wrap_mode wrap_u{ wrap_mode::repeat };
	wrap_mode wrap_v{ wrap_mode::repeat };
	wrap_mode wrap_w{ wrap_mode::repeat }; // used in texture_type::three

    float min_lod{ 0.f }; // -1000 ~ +1000 or 0 ~ 1000 ???
    float max_lod{ 1000.f }; // max_float


    // rename: target
    render_target as_render_target{ render_target::none }; // used as render target or depth_stencil target is depended by pixel_format
    uint rt_sample_count{ 1 }; // only used when texture is used as render_target or depth_stencil target, if rt_sample_count > 1, use MSAA

public:
    texture_desc() = default; // for texture2d
    texture_desc(std::string_view name, uint width, uint height, pixel_format format, texture_type type = texture_type::two, std::vector<const void*> data = { nullptr })
        : name{ name }
        , width{ width }
        , height{ height }
        , depth{ static_cast<uint>(data.size()) }
        , format{ format }
        , type{ type }
        , data{ std::move(data) }
    {
        // DCHECK( cube, 6);

        switch (type)
        {
            case fay::texture_type::two:
                break;
            case fay::texture_type::cube:
            {
                // pos_x, neg_x, pos_y, neg_y, pos_z, neg_z
                wrap_u = wrap_mode::clamp_to_edge;
                wrap_v = wrap_mode::clamp_to_edge;
                break;
            }
            case fay::texture_type::three:
                break;
            case fay::texture_type::array:
                break;
            default:
                break;
        }
    }

    texture_desc& sample(filter_mode filter, wrap_mode warp)
    {
        min_filter = filter;
        max_filter = filter;

        wrap_u = warp;
        wrap_v = warp;
        wrap_w = warp;

        return *this;
    }

    // change pixel format automatically
    texture_desc& set_target(render_target target)
    {
        as_render_target = target;
        data = { nullptr };
        mipmap = false;

        switch (target)
        {
            case fay::render_target::depth:
                format = pixel_format::depth;
                break;
            case fay::render_target::stencil:
            case fay::render_target::depth_stencil:
                format = pixel_format::depthstencil;
                break;
            default:
                LOG(ERROR) << "shouldn't be here";
                break;
        }

        return *this;
    }
};


// row major
struct render_matrix
{
    // TODO
    float m[4][4]
    {
        1.f, 0.f, 0.f, 0.f,
        0.f, 1.f, 0.f, 0.f,
        0.f, 0.f, 1.f, 0.f,
        0.f, 0.f, 0.f, 1.f,
    };
};

struct bottom_accel_desc
{
    struct shape // raw_mesh
    {
        buffer_id vertex;
        buffer_id index;
        // transform
    };

    std::vector<shape> shapes;
};

// struct top_accel_desc

struct accel_desc
{
    struct instance
    {
        int bottom_index;
        render_matrix mat{};
    };

    std::vector<bottom_accel_desc> bottoms;

    std::vector<instance> instances;
    bool allowUpdate = false;
};



// RootSignature/DescriptorSetLayout
// shader_resource/resource_set/resource_pack/resource_package/resource_bundle/resource_collection
struct respack_desc
{
    std::string name{ "defult" };

    /*legacy*/buffer_id index{};
    /*legacy*/buffer_id vertex{};
    // buffer_id vertex{ 0 }; array<buffer_id, ...> instances;
    /*legacy*/std::vector<buffer_id>  buffers{}; // buffers[0] is default vertex buffer

    std::vector<texture_id> textures{};
    std::vector<buffer_id>  uniforms{};
    //std::vector<buffer_id>  storage_buffers_srv{};
    //std::vector<buffer_id>  storage_buffers_uav{};
    //...
    // texture sampler
};



// shader_metadata
// shader sources(not filepath) + uniform blocks + texutres 
struct shader_desc
{
public:
    struct uniform_block;
    struct sampler;

public:
    std::string name{ "defult" };

    /*
    union
    {
        std::string charcode{};
        const char* bytecode{};
    }
    bool use_bytecode{};
    */
    std::string vs{};
    std::string hs{};
    std::string ds{};
    std::string gs{};
    std::string fs{};

    std::string compute{};

    std::string task{};
    std::string mesh{};

    std::string ray_gen{};
    std::string ray_miss{};
    std::string ray_intersect{};
    std::string ray_any_hit{};
    std::string ray_hit{};

    // TODO
    vertex_layout layout {};

    uint vs_uniform_block_sz{};
    uint fs_uniform_block_sz{};
    std::vector<uniform_block> uniform_blocks;

    // TODO: std::span_view
    uint vs_samplers_sz{};
    uint fs_samplers_sz{};
    std::vector<sampler> samplers{};
    //std::vector<sampler> vs_samplers{};
    //std::vector<sampler> fs_samplers{};

public:
    std::pair<uint, shader_stage> ub_info(const std::string& name, uint size) const
    {
        auto opt_idx = fay::index(uniform_blocks, [name, size](auto&& ub)
        {
            return (ub.name == name) && (0 == size || ub.size == size);
        });

        DCHECK(opt_idx.has_value()) << "can't find this uniform_block";
        uint idx = opt_idx.value();

        DCHECK(size == 0 || size == uniform_blocks[idx].size) << "input unifrom block size isn't match";

        auto stage = (idx < vs_uniform_block_sz) ? shader_stage::vertex : shader_stage::fragment; // !!!

        return { idx, stage };
    }

public: 
    // type define
    struct uniform_block
    {
        std::string name{};
        uint size{}; // byte_size
        std::vector<uniform_type> types;

        bool operator==(const uniform_block& that) const
        {
            return
                (name == that.name) &&
                (size == that.size) &&
                (types == that.types);
        }
    };

    struct sampler
    {
        std::string name{};
        texture_type type{};

        bool operator==(const sampler& that) const
        {
            return
                (name == that.name) &&
                (type == that.type);
        }
    };
};



// graphics_pipeline_desc, raytracing_pipeline_desc, compute_pipeline_desc

/*
    low level api: pipeline state object
*/
struct pipeline_desc
{
public:
    // depth-stencil state
    struct stencil_state
    {
        compare_op compare_op{ compare_op::always };

        stencil_op fail_op{ stencil_op::keep }; // stencil_fail(don't care depth test result???)
        stencil_op depth_fail_op{ stencil_op::keep }; // stencil pass but depth fail
        stencil_op pass_op{ stencil_op::keep };

        // std::as_const
        bool operator==(const stencil_state& that) const
        {
            return
                (fail_op == that.fail_op) &&
                (depth_fail_op == that.depth_fail_op) &&
                (pass_op == that.pass_op) &&
                (compare_op == that.compare_op);
        }
        bool operator!=(const stencil_state& that) const
        {
            return !operator==(that);
        }
    };

    // alpha-blending state
    struct blend_state
    {
        blend_factor src_factor{ blend_factor::one };
        blend_factor dst_factor{ blend_factor::zero };
        blend_op blend_op{ blend_op::add };

        bool operator==(const blend_state& that) const
        {
            return
                (src_factor == that.src_factor) &&
                (dst_factor == that.dst_factor) &&
                (blend_op == that.blend_op);
        }
        bool operator!=(const blend_state& that) const
        {
            return !operator==(that);
        }
    };

public:
    std::string name{ "default" };
    pipeline_type type{ pipeline_type::rasterization };

	// vertex layout and attribure
    // DOIT: set vertex layout in pipeline_desc

	// primitive type
	primitive_type primitive_type{ primitive_type::triangles };

	// shader
    // TODO???: set program in pipeline_desc

	// rasterization state
    cull_mode cull_mode{ cull_mode::back };
    face_winding face_winding{ face_winding::cw }; // D3D-style

    fill_mode fill_mode{ fill_mode::solid };
	bool alpha_to_coverage_enabled { false };
	int rasteriza_sample_count     { 1 }; // if sc > 1, use multisample

    // bool polygon_offset_enabled
    // WARNNING: Not enabled
	float depth_bias               { 0.f };
	float depth_bias_slope_scale   { 0.f };
	float depth_bias_clamp         { 0.f }; // used on d3d11 


    // depth
    bool depth_enabled{ true }; // depth_mask_write_enabled
    compare_op depth_compare_op{ compare_op::less_equal };
    

    // stencil
    bool stencil_enabled{ false }; // WARNNING: default open stencil // TODO: def close it

    stencil_state stencil_front{ };
    stencil_state stencil_back{ };
    uint8_t stencil_ref{ 0 }; // reference value
    uint8_t stencil_test_mask{ 0xff };

    uint8_t stencil_write_mask{ 0xff };


    // blend
	bool blend_enabled{ false };

    blend_state blend_rgb {};
    blend_state blend_alpha {};
    // array4f vs float4 vs vec4f
    std::array<float, 4> blend_color{ 0.f, 0.f, 0.f, 0.f }; // if use constant color & alpha

     // WARNNING: used in debug check
    int color_attachment_count { 1 };
    pixel_format blend_color_format { pixel_format::rgb8 };
	pixel_format blend_depth_format { pixel_format::r8 };

    blend_mask blend_write_mask{ blend_mask::rgba };
};



struct attachment_desc
{
    texture_id tex_id{};
    // union { int face; int layer; int slice; };
    uint layer{}; // texture::two(0), texture::cube(0~5), texture::array/three(0~depth-1) // slice
    uint level{}; // mipmap_level
};

struct state_desc
{
    std::string name{ "defult" };

    shader_id shd_id{};
    pipeline_id pipe_id{};
    // frame_id frm_id{}; // use 0 as the framebuffer by default 
};

/*
    low level api: (vulkan)render pass / (metal)RenderPassDesc
*/
struct frame_desc
{
    std::string name{ "defult" };

    // TODO: move those
    uint width{};
    uint height{};

    std::vector<attachment_desc> render_targets{}; // color_attachments
    attachment_desc depth_stencil{};

    frame_desc() = default;
    frame_desc(std::string_view name, std::vector<attachment_desc> render_targets, attachment_desc depth_stencil)
        : name{ name }
        , depth_stencil{ depth_stencil }
        , render_targets{ std::move(render_targets) }
    {}
};



struct render_desc
{
    std::string name{ "default" };
    render_backend_type backend{ render_backend_type::d3d12 };

    // feature
    anti_aliasing anti_aliasing{};
    uint multiple_sample_count{}; // if use MSAA

    // opengl-specific
    // TODO: check backend_type & window/device/context

	// d3d11-specific
    // HWND d3d_handle;
    void* d3d_handle;

    // **-specific

    // responsible by the system
    uint width;
    uint height;
    bool enable_msaa;
};

// clear_op

#pragma endregion desc

#pragma region command

// render_command_type
enum class command_type
{
    none,

    //begin_,
    //end_,

    begin_default_frame,
    begin_frame,
    end_frame,

    clear_color,
    clear_depth,
    clear_stencil,
    //clear_frame,

    set_viewport,
    set_scissor,
    // set_view

    bind_index,
    bind_buffer, 
    bind_named_texture,
    bind_textures,
    // shared memory(PushConstant)
    bind_uniform,
    bind_uniform_block,

    // bind resource index to pipeline
    bind_respack, // bind_resource, update_descriptor_set, bind_shader_params

    apply_shader,
    apply_pipeline,
    // apply_state,

    update_buffer,
    update_texture,
    update_respacck,

    draw,
    draw_index,

    compute,

    build_accel,
    tracing_ray,

    readback, // blit
    // copy_data, update_resource, apply_state(binding_state)
};

// render_command
struct command // command/encoder
{
    using uniform = std::variant<bool, int, float, glm::vec2, glm::vec3, glm::vec4, glm::mat2, glm::mat3, glm::mat4>;

    //std::string_view name{ "defult" };

    command_type type_{};

    // std::any, std::variant, union para { ... }
    int          int_{};
    uint        uint_{};
    float      float_{};

    buffer_id   buf_{};
    texture_id  tex_{};
    respack_id  res_{};
    shader_id   shd_{};
    pipeline_id pip_{};
    frame_id    frm_{};

    glm::uvec4 uint4_{};
    glm::vec4 float4_{};
    //glm::mat4 mat4_{};

    const void* data_;
    uniform uniform_;
    memory uniform_block_;

    std::string str_{};
    std::vector<uint> uints_{};
    std::vector<texture_id> texs_{};
    std::vector<attribute_usage> attrs_{};

    command(command_type type) : type_(type) {}

    /*
    #ifdef FAY_DEBUG
        int command_priority; "clear" must less than "draw"
    #endif
    */
};

/*
    low level api: command buffer/list
*/

class renderable;
class material;
class frame;

// WARNING: rebuild(not reuse it)
// rename: render_cmds or render_command_list???
class command_list
{
public:
    // std::vector<state_desc> states;
    // std::vector<respack_desc> resources;
    // std::vector<command_desc> commands;

    command_list() {} // TODO: remove default ctor, always give cmds a name
    command_list(std::string name) : name_(std::move(name)) {}

    // -------------------------------------------------------------------------------------------------

    command_list& begin_frame(frame_id id)
    {
        auto& cmd = add_command(command_type::begin_frame);
        cmd.frm_ = id;

        return *this;
    }
    command_list& begin_frame(const frame& frm);
    command_list& begin_frame(const frame& frm, pipeline_id pipe, shader_id shd);
    command_list& begin_default_frame(/*uint x, uint y, uint w, uint h*/)
    {
        add_command(command_type::begin_default_frame);

        return *this;
    }
    command_list& begin_default(const pipeline_id pipe_id, const shader_id shd_id)
    {
        begin_default_frame();
        clear();
        apply_pipeline(pipe_id);
        apply_shader(shd_id);

        return *this;
    }
    command_list& end_frame()
    {
        add_command(command_type::end_frame);

        return *this;
    }
    // command_list& bind_frame(frame_id id, uint flags, std::vector<glm::vec4> colors, float depth, int stencil);

    // WARNNING: clear all color targets if 'targets' is empty.
    command_list& clear_color(float red, float green, float blue, float alpha = 1.f, std::vector<uint> targets = {})
    {
        return clear_color({ red, green, blue, alpha });
    }
    command_list& clear_color(glm::vec4 rgba = { 0.f, 0.f, 0.f, 1.f }, std::vector<uint> targets = {})
    {
        auto& cmd = add_command(command_type::clear_color);
        cmd.float4_ = rgba;
        cmd.uints_ = targets;

        // return add_commoand(command_type::begin_frame, [](command& cmd) { cmd.float4_ = rgba; cmd.uints_ = targets; });

        return *this;
    }
    command_list& clear_depth(float depth = 1.f)
    {
        auto& cmd = add_command(command_type::clear_depth);
        cmd.float_ = depth;

        return *this;
    }
    command_list& clear_stencil(uint stencil = 0)
    {
        auto& cmd = add_command(command_type::clear_stencil);
        cmd.uint_ = stencil;

        return *this;
    }

    command_list& clear()
    {
        clear_color();
        clear_depth();
        clear_stencil();

        return *this;
    }
    
    // WARNNING: fay use top_left as origin(rather then bottom_left in opengl)
    command_list& set_viewport(uint x, uint y, uint width, uint height)
    {
        auto& cmd = add_command(command_type::set_viewport);
        cmd.uint4_ = { x, y, width, height };

        return *this;
    }
    command_list& set_scissor( uint x, uint y, uint width, uint height)
    {
        auto& cmd = add_command(command_type::set_scissor);
        cmd.uint4_ = { x, y, width, height };

        return *this;
    }

    command_list& apply_shader(const shader_id id)
    {
        auto& cmd = add_command(command_type::apply_shader);
        cmd.shd_ = id;

        return *this;
    }
    command_list& apply_pipeline(const pipeline_id id)
    {
        auto& cmd = add_command(command_type::apply_pipeline);
        cmd.pip_ = id;

        return *this;
    }
    command_list& apply_pipeline(const pipeline_id pipe, const shader_id shd)
    {
        apply_pipeline(pipe);
        apply_shader(shd);

        return *this;
    }
    command_list& apply_state(const pipeline_id pipe, const shader_id shd)
    {
        apply_pipeline(pipe);
        apply_shader(shd);

        return *this;
    }

    // -------------------------------------------------------------------------------------------------

    command_list& bind_texture(const texture_id id, const std::string sampler)
    {
        auto& cmd = add_command(command_type::bind_named_texture);
        cmd.tex_ = id;
        cmd.str_ = sampler;

        return *this;
    }

    // !!! you could write device->bind_textures(0, "xxx"); device will ignore this cmd if id is 0.
    command_list& try_bind_texture(const texture_id id, const std::string sampler)
    {
        if (id)
            bind_texture(id, sampler);

        return *this;
    }

    // !!! default bind all textures to **fragment shader**
    // !!! you could write device->bind_textures({id0, id1, 0, id3, 0, id5}); device will ignore 0.
    command_list& bind_textures(std::vector<texture_id> textures, shader_stage stage = shader_stage::fragment)
    {
        auto& cmd = add_command(command_type::bind_textures);
        cmd.texs_ = std::move(textures);
        cmd.uint_ = enum_cast(stage);

        return *this;
    }

    // -------------------------------------------------------------------------------------------------

    command_list& bind_index(const buffer_id id)
    {
        auto& cmd = add_command(command_type::bind_index);
        cmd.buf_ = id;

        return *this;
    }

    // WARNNING: only bind all buffers to **vertex shader**
    // command_list& bind_buffer(const std::vector<buffer_id> buffers);

    // WARNNING: if attrs is {}, it will use all attrs of buffer
    command_list& bind_vertex(const buffer_id id, std::vector<attribute_usage> attrs = {}) {   return bind_buffer(id, attrs, 0); }
    command_list& bind_instance(const buffer_id id, std::vector<attribute_usage> attrs = {}) { return bind_buffer(id, attrs, 1); }



    command_list& bind_uniform(std::string name, command::uniform uniform)
    {
        auto& cmd = add_command(command_type::bind_uniform);
        cmd.str_ = std::move(name);
        cmd.uniform_ = std::move(uniform);

        return *this;
    }
    command_list& bind_uniform_block(std::string ub_name, memory mem)
    {
        auto& cmd = add_command(command_type::bind_uniform_block);
        cmd.str_ = std::move(ub_name);
        cmd.uniform_block_ = std::move(mem);

        return *this;
    }

    //command_list& bind_vertex(const buffer_id id, std::vector<uint> attrs = {}, std::vector<uint> slots = {});
    //command_list& bind_instance(const buffer_id id, std::vector<uint> attrs = {}, std::vector<uint> slots = {});
    //command_list& bind_resource(const buffer_id id, const std::vector<buffer_id> buffers, const std::vector<texture_id> textures);

    command_list& bind_respack(const respack_id id)
    {
        auto& cmd = add_command(command_type::bind_respack);
        cmd.res_ = id;

        return *this;
    }

    // TODO: bind_vertex_attribute, allocate_vertex_data

    command_list& update_buffer(const buffer_id id, const void* data, uint size = 0) // cache the data
    {
        auto& cmd = add_command(command_type::update_buffer);
        cmd.buf_ = id;
        cmd.data_ = data;

        return *this;
    }
    // command_list& update_texture(const texture_id id, const void* data, uint size);
    // command_list& update_respack(const respack_id id, /*...*/);

    // WARNNING: if count is 0, it will be computed by device automatically
    // WARNNING: if instance_count is 0, use 'draw', else use 'draw_instance'
    command_list& draw(uint count = 0, uint first = 0, uint instance_count = 1)
    {
        auto& cmd = add_command(command_type::draw);
        cmd.uint4_ = { count, first, instance_count, 0 };

        return *this;
    }
    command_list& draw_index(uint count = 0, uint first = 0, uint instance_count = 1)
    {
        auto& cmd = add_command(command_type::draw_index);
        cmd.uint4_ = { count, first, instance_count, 0 };

        return *this;
    }

    /*
    template<typename Renderable>
    command_list& draw(Renderable* renderable)
    {
        renderable->render(*this);

        return *this;
    }
    */
    command_list& draw(renderable* renderable);
    // command_list& draw_post_processing(); // No vertex data required
    // Implementation details_: when use post_processing.vs, vertex data are build inside shader code.

    // command_list& draw_indirect();
    // command_list& draw_indexed_indirect();
    // command_list& compute();
    // command_list& blit(frame_id f1, frame_id f2); // transfer

    command_list& tracing_ray()
    {
        auto& cmd = add_command(command_type::tracing_ray);

        return *this;
    }

    /* const */std::string_view name() const { return name_; }
    const std::vector<command>& commands_() const { return cmds_; }

private:
    // TODO: rename: add_command_
    command& add_command(command_type type)
    {
        cmds_.emplace_back(type);
        return cmds_.back();
    }

    command_list& bind_buffer(const buffer_id id, std::vector<attribute_usage> attrs, uint instance_rate)
    {
        auto& cmd = add_command(command_type::bind_buffer);
        cmd.buf_ = id;
        cmd.attrs_ = std::move(attrs);
        cmd.uint_ = instance_rate;

        return *this;
    }

private:
    std::string name_{ "defult" };
    // pass_type type{};

    std::vector<command> cmds_{}; // don't need command_list_pool, just move it.
};

#pragma endregion command

} // namespace fay