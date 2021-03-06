#pragma once
#include "fay/render/backend.h"
using namespace std::string_literals;

#ifdef FAY_IN_WINDOWS

//#define CINTERFACE
//#define COBJMACROS
#define D3D11_NO_HELPERS
#define WIN32_LEAN_AND_MEAN

#include <Windows.h>
#include <comdef.h>
#include <dxcapi.h>
#include <d3d11_4.h>
#include <d3d12.h>
//#include <d3dx12.h>
#include <d3dcompiler.h>
#include <dxgi1_5.h>

#pragma comment (lib, "d3d11.lib")
#pragma comment (lib, "d3d12.lib")
#pragma comment (lib, "d3dcompiler.lib")
#pragma comment (lib, "dxcompiler.lib")
#pragma comment (lib, "dxgi.lib")
#pragma comment (lib, "dxguid.lib")
//#pragma comment(lib, "winmm.lib")



#define FAY_SMART_COM_PTR(_a) _COM_SMARTPTR_TYPEDEF(_a, __uuidof(_a))

#ifdef FAY_DEBUG

#ifndef D3D_CHECK
#define D3D_CHECK(func)	\
	    { \
		    HRESULT hr = (func); \
		    if(FAILED(hr)) \
		    { \
                d3d_error_handle(hr, #func); \
		    } \
	    }
#endif

#ifndef D3D_CHECK2
#define D3D_CHECK2(func, ptr) \
            D3D_CHECK(func) \
            { if(!ptr) LOG(ERROR) << "ptr is nullptr"s; }
#endif

#ifndef D3D12_CHECK
#define D3D12_CHECK(func) \
	    { \
		    HRESULT hr = (func); \
		    if(FAILED(hr)) \
		    { \
                d3d_error_handle(hr, #func); \
                d3d12_error_handle(hr); \
		    } \
	    }
#endif

#ifndef D3D12_CHECK2
#define D3D12_CHECK2(func, ptr) \
            D3D12_CHECK(func) \
            { if(!ptr) LOG(ERROR) << "ptr is nullptr"s; }
#endif

#else

#ifndef D3D_CHECK
#define D3D_CHECK(func) (func)
#endif

#ifndef D3D_CHECK2
#define D3D_CHECK2(func, ptr) (func)
#endif

#ifndef D3D12_CHECK
#define D3D12_CHECK(func) (func)
#endif

#ifndef D3D12_CHECK2
#define D3D12_CHECK2(func, ptr) (func)
#endif

#endif

namespace fay::d3d
{

#pragma region com_ptr

FAY_SMART_COM_PTR(IUnknown);
FAY_SMART_COM_PTR(ID3DBlob);

FAY_SMART_COM_PTR(IDXGIFactory5);
FAY_SMART_COM_PTR(IDXGIAdapter1);
FAY_SMART_COM_PTR(IDXGIAdapter3);
FAY_SMART_COM_PTR(IDXGIDevice);
FAY_SMART_COM_PTR(IDXGISwapChain);
FAY_SMART_COM_PTR(IDXGISwapChain1);
FAY_SMART_COM_PTR(IDXGISwapChain4);


FAY_SMART_COM_PTR(ID3D11Device);
FAY_SMART_COM_PTR(ID3D11DeviceContext);

FAY_SMART_COM_PTR(ID3D11Buffer);
FAY_SMART_COM_PTR(ID3D11Texture2D);
FAY_SMART_COM_PTR(ID3D11Texture3D);
FAY_SMART_COM_PTR(ID3D11SamplerState);
FAY_SMART_COM_PTR(ID3D11ShaderResourceView);
FAY_SMART_COM_PTR(ID3D11RenderTargetView);
FAY_SMART_COM_PTR(ID3D11DepthStencilView);

FAY_SMART_COM_PTR(ID3D11VertexShader);
FAY_SMART_COM_PTR(ID3D11PixelShader);

FAY_SMART_COM_PTR(ID3D11InputLayout);
FAY_SMART_COM_PTR(ID3D11RasterizerState);
FAY_SMART_COM_PTR(ID3D11DepthStencilState);
FAY_SMART_COM_PTR(ID3D11BlendState);


FAY_SMART_COM_PTR(ID3D12Device);
FAY_SMART_COM_PTR(ID3D12Device5);
FAY_SMART_COM_PTR(ID3D12Debug);
FAY_SMART_COM_PTR(ID3D12Debug3);
FAY_SMART_COM_PTR(ID3D12DebugDevice1);
FAY_SMART_COM_PTR(ID3D12CommandQueue);
FAY_SMART_COM_PTR(ID3D12CommandAllocator);
FAY_SMART_COM_PTR(ID3D12GraphicsCommandList4);

FAY_SMART_COM_PTR(IDxcBlob);

FAY_SMART_COM_PTR(ID3D12DescriptorHeap);
FAY_SMART_COM_PTR(ID3D12Resource);
FAY_SMART_COM_PTR(ID3D12Fence1);
FAY_SMART_COM_PTR(ID3D12PipelineState);
FAY_SMART_COM_PTR(ID3D12StateObject);
FAY_SMART_COM_PTR(ID3D12StateObjectProperties);
FAY_SMART_COM_PTR(ID3D12ShaderReflection);
FAY_SMART_COM_PTR(ID3D12RootSignature);
FAY_SMART_COM_PTR(ID3D12QueryHeap);
FAY_SMART_COM_PTR(ID3D12CommandSignature);
//FAY_SMART_COM_PTR(ID3D12DeviceRaytracingPrototype);
//FAY_SMART_COM_PTR(ID3D12CommandListRaytracingPrototype);

#pragma endregion com_ptr



#pragma region enum

const D3D_FEATURE_LEVEL D3D_FEATURE_LEVELS[] =
{
    D3D_FEATURE_LEVEL_12_1,
    D3D_FEATURE_LEVEL_12_0,
    D3D_FEATURE_LEVEL_11_1,
    D3D_FEATURE_LEVEL_11_0,
    D3D_FEATURE_LEVEL_10_1,
};

#pragma endregion



#pragma region type

const inline enum_class_map<attribute_usage, const char*>
semantic_name_map
{
    { attribute_usage::position,       "POSITION" },
    { attribute_usage::normal,         "NORMAL" },
    { attribute_usage::tangent,        "TANGENT" },
    { attribute_usage::bitangent,      "BITANGENT" },
    { attribute_usage::texcoord0,      "TEXCOORD" }, // TODO
    { attribute_usage::texcoord1,      "TEXCOORD1" },
    { attribute_usage::instance_model, "INSTANCE_MODEL" },
};

const inline enum_class_map<attribute_format, DXGI_FORMAT>
vertex_format_map
{
    { attribute_format::float1, DXGI_FORMAT_R32_FLOAT },
    { attribute_format::float2, DXGI_FORMAT_R32G32_FLOAT },
    { attribute_format::float3, DXGI_FORMAT_R32G32B32_FLOAT },
    { attribute_format::float4, DXGI_FORMAT_R32G32B32A32_FLOAT },

    { attribute_format::short2, DXGI_FORMAT_R16G16_SINT },
    { attribute_format::short4, DXGI_FORMAT_R16G16B16A16_SINT },

    { attribute_format::byte4,  DXGI_FORMAT_R8G8B8A8_SINT },
    { attribute_format::ubyte4, DXGI_FORMAT_R8G8B8A8_UINT },
};

const inline enum_class_map<primitive_type, D3D_PRIMITIVE_TOPOLOGY>
primitive_topology_map
{
    { primitive_type::points,         D3D_PRIMITIVE_TOPOLOGY_POINTLIST },
    { primitive_type::lines,          D3D_PRIMITIVE_TOPOLOGY_LINELIST },
    { primitive_type::line_strip,     D3D_PRIMITIVE_TOPOLOGY_LINESTRIP },
    { primitive_type::triangles,      D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST },
    { primitive_type::triangle_strip, D3D_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP },
};


#pragma endregion



#pragma region function

inline void d3d_error_handle(HRESULT hr, const char* funcname)
{
    char hr_msg[256];
    FormatMessageA(FORMAT_MESSAGE_FROM_SYSTEM, nullptr, hr, 0, hr_msg, ARRAYSIZE(hr_msg), nullptr);

    std::string error_msg = "\nError!\nline: "s + std::to_string(__LINE__) + "\nhr: " + std::to_string(hr) + "\nfunc: " + funcname + "\nerror msg: " + hr_msg;
    LOG(ERROR) << error_msg;
}

/*
IDXGIAdapter3Ptr FindAdapter()
{

}

IDXGISwapChain4Ptr CreateSwapChain()
{

}
*/

#pragma endregion

}

#endif // FAY_IN_WINDOWS
