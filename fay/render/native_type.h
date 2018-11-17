#if defined(_MSC_VER)
#pragma once
#endif

#ifndef FAY_RENDER_NATIVE_TYPE_H
#define FAY_RENDER_NATIVE_TYPE_H

#include <d3d11.h>
#include "glad/glad.h"

#include "fay/render/define.h"

namespace fay::render
{

struct native_type
{
	GLenum opengl;
	int d3d11;
};

/*
auto enum_class_hash = [] (auto k)
{
    // return static_cast<std::underlying_type_t<Key>>(k);
    return static_cast<std::size_t>(k);
};
*/

// TODO: move to core/..
struct enum_class_hash
{
    template <typename T>
    std::size_t operator()(T t) const
    {
        return static_cast<std::size_t>(t);
    }
};

template <typename Key, typename T>
using enum_class_map = std::unordered_map<Key, T, enum_class_hash>;



const inline enum_class_map<resource_usage, native_type>
resource_usage_map
{
    { resource_usage::immutable, { GL_STATIC_DRAW,  D3D11_USAGE_IMMUTABLE } },
    { resource_usage::dynamic,   { GL_DYNAMIC_DRAW, D3D11_USAGE_DYNAMIC } },
    { resource_usage::stream,    { GL_STREAM_DRAW,  D3D11_USAGE_DYNAMIC } },
};

const inline enum_class_map<buffer_type, native_type>
buffer_type_map
{
	{ buffer_type::vertex,    { GL_ARRAY_BUFFER, 0 } },
	{ buffer_type::index,     { GL_ELEMENT_ARRAY_BUFFER, 0 } },
    { buffer_type::instance,  { GL_ARRAY_BUFFER, 0 } },
};

const inline enum_class_map<texture_type, native_type>
texture_type_map
{
	{ texture_type::two,   { GL_TEXTURE_2D, 0 } },
	{ texture_type::cube,  { GL_TEXTURE_CUBE_MAP, 0 } },
	{ texture_type::three, { GL_TEXTURE_3D, 0 } },
	{ texture_type::array, { GL_TEXTURE_2D_ARRAY, 0 } },
	//{ texture_type::texture_array_2d,   { 0, 0 } },
	//{ texture_type::texture_array_data, { 0, 0 } },
};

const inline enum_class_map<filter_mode, native_type>
filter_mode_map
{
    { filter_mode::anisotropy,             { 0,                         D3D11_FILTER_ANISOTROPIC } },
    { filter_mode::nearest,                { GL_NEAREST,                0 } },
    { filter_mode::linear,                 { GL_LINEAR,                 0 } },
    { filter_mode::nearest_mipmap_nearest, { GL_NEAREST_MIPMAP_NEAREST, 0 } },
    { filter_mode::nearest_mipmap_linear,  { GL_NEAREST_MIPMAP_LINEAR,  0 } },
    { filter_mode::linear_mipmap_nearest,  { GL_LINEAR_MIPMAP_NEAREST,  0 } },
    { filter_mode::linear_mipmap_linear,   { GL_LINEAR_MIPMAP_LINEAR,   0 } },
};

const inline enum_class_map<wrap_mode, native_type>
wrap_mode_map
{
    { wrap_mode::repeat,          { GL_REPEAT,          D3D11_TEXTURE_ADDRESS_WRAP } },
    { wrap_mode::clamp_to_edge,   { GL_CLAMP_TO_EDGE,   D3D11_TEXTURE_ADDRESS_CLAMP } },
    { wrap_mode::mirrored_repeat, { GL_MIRRORED_REPEAT, D3D11_TEXTURE_ADDRESS_MIRROR } },
};

const inline enum_class_map<cube_face, native_type>
cube_face_map
{
    { cube_face::pos_x, { GL_TEXTURE_CUBE_MAP_POSITIVE_X, 0 } },
    { cube_face::neg_x, { GL_TEXTURE_CUBE_MAP_NEGATIVE_X, 0 } },
    { cube_face::pos_y, { GL_TEXTURE_CUBE_MAP_POSITIVE_Y, 0 } },
    { cube_face::neg_y, { GL_TEXTURE_CUBE_MAP_NEGATIVE_Y, 0 } },
    { cube_face::pos_z, { GL_TEXTURE_CUBE_MAP_POSITIVE_Z, 0 } },
    { cube_face::neg_z, { GL_TEXTURE_CUBE_MAP_NEGATIVE_Z, 0 } },
};

const inline enum_class_map<shader_type, native_type>
shader_type_map
{
	{ shader_type::vertex,   { GL_VERTEX_SHADER, 0 } },
	{ shader_type::geometry, { GL_GEOMETRY_SHADER, 0 } },
	{ shader_type::fragment, { GL_FRAGMENT_SHADER, 0 } },
};

const inline enum_class_map<primitive_type, native_type>
primitive_type_map
{
	{ primitive_type::points,         { GL_POINTS,         D3D11_PRIMITIVE_TOPOLOGY_POINTLIST } },
	{ primitive_type::lines,          { GL_LINES,          D3D11_PRIMITIVE_TOPOLOGY_LINELIST } },
	{ primitive_type::line_strip,     { GL_LINE_STRIP,     D3D11_PRIMITIVE_TOPOLOGY_LINESTRIP } },
	{ primitive_type::triangles,      { GL_TRIANGLES,      D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST } },
	{ primitive_type::triangle_strip, { GL_TRIANGLE_STRIP, D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP } },
};

const inline enum_class_map<cull_mode, native_type>
cull_mode_map
{
	{ cull_mode::none,  { 0, D3D11_CULL_NONE } },
	{ cull_mode::front, { 0, D3D11_CULL_FRONT } },
	{ cull_mode::back,  { 0, D3D11_CULL_BACK } },
};

const inline enum_class_map<blend_factor, native_type>
blend_factor_map
{
    { blend_factor::zero,                  { GL_ZERO,                     D3D11_BLEND_ZERO } },
    { blend_factor::one,                   { GL_ONE,                      D3D11_BLEND_ONE } },

    { blend_factor::src_color,             { GL_SRC_COLOR,                D3D11_BLEND_SRC_COLOR } },
    { blend_factor::one_minus_src_color,   { GL_ONE_MINUS_SRC_COLOR,      D3D11_BLEND_INV_SRC_COLOR } },
    { blend_factor::src_alpha,             { GL_SRC_ALPHA,                D3D11_BLEND_SRC_ALPHA } },
    { blend_factor::one_minus_src_alpha,   { GL_ONE_MINUS_SRC_ALPHA,      D3D11_BLEND_INV_SRC_ALPHA } },

    { blend_factor::dst_color,             { GL_DST_COLOR,                D3D11_BLEND_DEST_COLOR } },
    { blend_factor::one_minus_dst_color,   { GL_ONE_MINUS_DST_COLOR,      D3D11_BLEND_INV_DEST_COLOR } },
    { blend_factor::dst_alpha,             { GL_DST_ALPHA,                D3D11_BLEND_DEST_ALPHA } },
    { blend_factor::one_minus_dst_alpha,   { GL_ONE_MINUS_DST_ALPHA,      D3D11_BLEND_INV_DEST_ALPHA } },

    { blend_factor::src_alpha_saturated,   { GL_SRC_ALPHA_SATURATE,       D3D11_BLEND_SRC_ALPHA_SAT } },

    { blend_factor::blend_color,           { GL_CONSTANT_COLOR,           D3D11_BLEND_BLEND_FACTOR } },
    { blend_factor::one_minus_blend_color, { GL_ONE_MINUS_CONSTANT_COLOR, D3D11_BLEND_INV_BLEND_FACTOR } },
    { blend_factor::blend_alpha,           { GL_CONSTANT_ALPHA,           D3D11_BLEND_BLEND_FACTOR } },
    { blend_factor::one_minus_blend_alpha, { GL_ONE_MINUS_CONSTANT_ALPHA, D3D11_BLEND_INV_BLEND_FACTOR } },
};

const inline enum_class_map<blend_op, native_type>
blend_op_map
{
    { blend_op::add,              { GL_FUNC_ADD,              D3D11_BLEND_OP_ADD } },
    { blend_op::subtract,         { GL_FUNC_SUBTRACT,         D3D11_BLEND_OP_SUBTRACT } },
    { blend_op::reverse_subtract, { GL_FUNC_REVERSE_SUBTRACT, D3D11_BLEND_OP_REV_SUBTRACT } },
};

const inline enum_class_map<stencil_op, native_type>
stencil_op_map
{
	{ stencil_op::keep,       { GL_KEEP,      D3D11_STENCIL_OP_KEEP } },
	{ stencil_op::zero,       { GL_ZERO,      D3D11_STENCIL_OP_ZERO } },
	{ stencil_op::replace,    { GL_REPLACE,   D3D11_STENCIL_OP_REPLACE } },
	{ stencil_op::incr_clamp, { GL_INCR,      D3D11_STENCIL_OP_INCR_SAT } },
	{ stencil_op::decr_clamp, { GL_DECR,      D3D11_STENCIL_OP_DECR_SAT } },
	{ stencil_op::invert,     { GL_INVERT,    D3D11_STENCIL_OP_INVERT } },
	{ stencil_op::incr_wrap,  { GL_INCR_WRAP, D3D11_STENCIL_OP_INCR } },
	{ stencil_op::decr_wrap,  { GL_DECR_WRAP, D3D11_STENCIL_OP_DECR } },
};

const inline enum_class_map<compare_func, native_type>
compare_func_map
{
    { compare_func::never,         { GL_NEVER,    D3D11_COMPARISON_NEVER } },
    { compare_func::less,          { GL_LESS,     D3D11_COMPARISON_LESS } },
    { compare_func::less_equal,    { GL_LEQUAL,   D3D11_COMPARISON_LESS_EQUAL } },
    { compare_func::equal,         { GL_EQUAL,    D3D11_COMPARISON_EQUAL } },
    { compare_func::not_equal,     { GL_NOTEQUAL, D3D11_COMPARISON_NOT_EQUAL } },
    { compare_func::greater,       { GL_GREATER,  D3D11_COMPARISON_GREATER } },
    { compare_func::greater_equal, { GL_GEQUAL,   D3D11_COMPARISON_GREATER_EQUAL } },
    { compare_func::always,        { GL_ALWAYS,   D3D11_COMPARISON_ALWAYS } },
};


// =================================================================================================
// format



struct attribute_detail
{
    uint32_t size;
    uint32_t byte;
};

// nums, bytes of attribute_format
const inline enum_class_map<attribute_format, attribute_detail>
attribute_format_map
{
    { attribute_format::float1,  {1,  4} },
    { attribute_format::float2,  {2,  8} },
    { attribute_format::float3,  {3, 12} },
    { attribute_format::float4,  {4, 16} },
    { attribute_format::floatx,  {1,  4} },

    { attribute_format::byte4,   {4, 4} },
    { attribute_format::byte4x,  {4, 4} },

    { attribute_format::ubyte4,  {4, 4} },
    { attribute_format::ubyte4x, {4, 4} },

    { attribute_format::short2,  {2, 4} },
    { attribute_format::short2x, {2, 4} },

    { attribute_format::short4,  {4, 8} },
    { attribute_format::short4x, {4, 8} },

    //{ uint10_x2, {1, 4} },
};

} // namespace fay

#endif // FAY_RENDER_NATIVE_TYPE_H