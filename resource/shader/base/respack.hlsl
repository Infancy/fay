cbuffer UniformBlock0 : register(b0)
{
	float4 vs_offset;
};

cbuffer UniformBlock0 : register(b1)
{
	float4 ps_offset;
};

struct VertexIn
{
	float3 mPos : POSITION;
    float2 mTex : TEXCOORD0;
};

struct VertexOut
{
	float4 rPos : SV_POSITION;
    float2 rTex : TEXCOORD;
};

VertexOut vs_main(VertexIn vIn)
{
    VertexOut vOut;
    vOut.rPos = float4(vIn.mPos, 1.f) + vs_offset;
    vOut.rTex = vIn.mTex;

    return vOut;
}

Texture2D gTex : register(t0);
SamplerState gSampler : register(s0);

float4 ps_main(VertexOut vOut) : SV_TARGET
{
   return gTex.Sample(gSampler, vOut.rTex) + ps_offset;
   
   //return float4(vOut.rTex.x, vOut.rTex.y, 0.f, 1.f) + ps_offset;
}