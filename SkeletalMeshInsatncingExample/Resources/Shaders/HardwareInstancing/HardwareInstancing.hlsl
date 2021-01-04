
#define PI  3.141592653589

struct MaterialData
{
    int diffuseTextureIndex;
    float padding0;
    float padding1;
    float padding2;
};

struct InstanceData
{
    float4x4 matWorld;
    int materialIndex;
    uint skeletonOffset;
    float padding0;
    float padding1;
};

cbuffer GlobalConstants : register(b0)
{
    float4x4 matViewProj;
    float3 lightDir;
    float padding0;
    float4 ambient;
};

struct TransformData
{
    float4x4 boneMat;
    float4x4 offset;
    float4x4 anim;
    int parentIndex;
};

SamplerState samplerLinearWrap : register(s0);

Texture2D    textures[] : register(t0, space0);

StructuredBuffer<InstanceData> instanceDatas  : register(t0, space1);
StructuredBuffer<MaterialData> materialDatas  : register(t1, space1);
StructuredBuffer<float4x4> skeletonDatas : register(t2, space1);

struct VS_IN
{
    float3 position     : POSITION;
    float3 normal       : NORMAL;
    float3 tangent      : TANGENT;
    float4 color        : COLOR;
    float2 uv           : TEXCOORD;
    uint4 blendIndex    : BLENDINDICES;
    float4 blendWeight  : BLENDWEIGHT;
};

struct VS_OUT
{
    float4 position     : SV_POSITION;
    float3 normal       : NORMAL;
    float2 uv           : TEXCOORD0;
    float materialID    : TEXCOORD1;
    float4 color        : COLOR;
};

VS_OUT VS(VS_IN vsIn, uint instanceID : SV_InstanceID)
{
    VS_OUT vsOut;
    InstanceData instData = instanceDatas[instanceID];

    uint blendIndex0 = vsIn.blendIndex.x + instData.skeletonOffset;
    uint blendIndex1 = vsIn.blendIndex.y + instData.skeletonOffset;
    uint blendIndex2 = vsIn.blendIndex.z + instData.skeletonOffset;
    uint blendIndex3 = vsIn.blendIndex.w + instData.skeletonOffset;

    float4 pos0 = mul(float4(vsIn.position, 1.0f), skeletonDatas[blendIndex0]);
    float4 pos1 = mul(float4(vsIn.position, 1.0f), skeletonDatas[blendIndex1]);
    float4 pos2 = mul(float4(vsIn.position, 1.0f), skeletonDatas[blendIndex2]);
    float4 pos3 = mul(float4(vsIn.position, 1.0f), skeletonDatas[blendIndex3]);

    float4 pos = pos0 * vsIn.blendWeight.x + pos1 * vsIn.blendWeight.y + pos2 * vsIn.blendWeight.z + pos3 * vsIn.blendWeight.w;

    float3 normal0 = mul(vsIn.normal, (float3x3)skeletonDatas[blendIndex0]);
    float3 normal1 = mul(vsIn.normal, (float3x3)skeletonDatas[blendIndex1]);
    float3 normal2 = mul(vsIn.normal, (float3x3)skeletonDatas[blendIndex2]);
    float3 normal3 = mul(vsIn.normal, (float3x3)skeletonDatas[blendIndex3]);
    float3 normal = normal0 * vsIn.blendWeight.x + normal1 * vsIn.blendWeight.y + normal2 * vsIn.blendWeight.z + normal3 * vsIn.blendWeight.w;

    float4 worldPos = mul(pos, instData.matWorld);
    vsOut.position = mul(worldPos, matViewProj);
    vsOut.normal = normalize(mul(normal, (float3x3)instData.matWorld));
    vsOut.uv = vsIn.uv;
    vsOut.materialID = instData.materialIndex;

    return vsOut;
}

float4 PS(VS_OUT psIn) : SV_Target
{
    int texIndex = materialDatas[round(psIn.materialID)].diffuseTextureIndex;
    float4 diffuse = textures[texIndex].Sample(samplerLinearWrap, psIn.uv);
    float3 res = dot(psIn.normal, -lightDir) * diffuse.xyz ;

    return float4(res + diffuse.xyz * ambient.xyz, 1.0f);
}