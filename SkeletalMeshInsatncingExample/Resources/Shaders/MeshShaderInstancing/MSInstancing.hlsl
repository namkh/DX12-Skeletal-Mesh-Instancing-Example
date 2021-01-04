#define MAX_VERTS 64
#define MAX_PRIMS 126
#define INST_GROUP_SIZE 64

struct DrawParam
{
    uint instanceCountPerDispatch;
    uint instanceOffset;
    uint meshletCount;
    uint lastMeshletVertCount;
    uint lastMeshletPrimCount;
};

struct Vertex
{
    float3 position;
    float3 normal;
    float3 tangent;
    float4 color;
    float2 uv;
    uint4 blendIndex;
    float4 blendWeight;
};

struct VertexOut
{
    float4 position      : SV_POSITION;
    float3 normal        : NORMAL0;
    float3 tangent       : TANGENT;
    float4 color         : COLOR;
    float2 uv            : TEXCOORD0;
    uint   materialID    : TEXCOORD1;
};

struct Meshlet
{
    uint vertexCount;
    uint vertexOffset;
    uint primitiveCount;
    uint primitiveOffset;
};

struct InstanceData
{
    float4x4 matWorld;
    uint materialID;
    uint skeletonOffset;
    float padding1;
    float padding2;
};

struct MaterialData
{
    uint diffuseTextureIndex;
    float padding0;
    float padding1;
    float padding2;
};

struct Payload
{
    uint instanceCountPerDispatchMesh;
    uint instanceOffset;
};

groupshared Payload payload;

cbuffer GlobalConstants : register(b0)
{
    float4x4 matViewProj;
    float3 lightDir;
    float padding0;
    float4 ambient;
};

ConstantBuffer<DrawParam>        drawParam        : register(b1);

StructuredBuffer<Vertex>         vertices             : register(t0, space1);
StructuredBuffer<Meshlet>        meshlets             : register(t1, space1);
StructuredBuffer<uint>           localVertexIndices   : register(t2, space1);
StructuredBuffer<uint>           localPrimIndices     : register(t3, space1);
StructuredBuffer<InstanceData>   instanceDatas        : register(t4, space1);
StructuredBuffer<MaterialData>   materialDatas        : register(t5, space1);
StructuredBuffer<float4x4>       skeletonDatas        : register(t6, space1);

SamplerState samplerLinearWrap : register(s0);

Texture2D textures[] : register(t0, space0);

uint DivRoundUp(uint num, uint denom)
{
    return (num + denom - 1) / denom;
}

[NumThreads(1, 1, 1)]
void AS(uint dtid : SV_DISPATCHTHREADID, uint gtid : SV_GROUPTHREADID, uint gid : SV_GROUPID)
{
    uint instanceCount = min(drawParam.instanceCountPerDispatch - (dtid * INST_GROUP_SIZE), INST_GROUP_SIZE);
    uint unpackedGroupCount = (drawParam.meshletCount - 1) * instanceCount;
    uint packCount = min(MAX_VERTS / drawParam.lastMeshletVertCount, MAX_PRIMS / drawParam.lastMeshletPrimCount);
    uint packedGroupCount = DivRoundUp(instanceCount, packCount);
    uint dispatchCount = unpackedGroupCount + packedGroupCount;

    payload.instanceOffset = gid * instanceCount + drawParam.instanceOffset;
    payload.instanceCountPerDispatchMesh = instanceCount;

    DispatchMesh(dispatchCount, 1, 1, payload);
}

uint3 UnpackPrimitive(uint primitive)
{
    return uint3(primitive & 0x3FF, (primitive >> 10) & 0x3FF, (primitive >> 20) & 0x3FF);
}

uint3 GetPrimitive(Meshlet meshlet, uint index)
{
    return UnpackPrimitive(localPrimIndices[meshlet.primitiveOffset + index]);
}

uint GetVertexIndex(Meshlet meshlet, uint localIndex)
{
    localIndex = meshlet.vertexOffset + localIndex;
    return localVertexIndices[localIndex];
}

VertexOut GetVertexAttributes(uint vertexIndex, uint instanceIndex)
{
    InstanceData instData = instanceDatas[instanceIndex];
    Vertex v = vertices[vertexIndex];

    uint blendIndex0 = v.blendIndex.x + instData.skeletonOffset;
    uint blendIndex1 = v.blendIndex.y + instData.skeletonOffset;
    uint blendIndex2 = v.blendIndex.z + instData.skeletonOffset;
    uint blendIndex3 = v.blendIndex.w + instData.skeletonOffset;
    
    float4 pos0 = mul(float4(v.position, 1.0f), skeletonDatas[blendIndex0]);
    float4 pos1 = mul(float4(v.position, 1.0f), skeletonDatas[blendIndex1]);
    float4 pos2 = mul(float4(v.position, 1.0f), skeletonDatas[blendIndex2]);
    float4 pos3 = mul(float4(v.position, 1.0f), skeletonDatas[blendIndex3]);
    float4 pos = pos0 * v.blendWeight.x + pos1 * v.blendWeight.y + pos2 * v.blendWeight.z + pos3 * v.blendWeight.w;

    float3 normal0 = mul(v.normal, (float3x3)skeletonDatas[blendIndex0]);
    float3 normal1 = mul(v.normal, (float3x3)skeletonDatas[blendIndex1]);
    float3 normal2 = mul(v.normal, (float3x3)skeletonDatas[blendIndex2]);
    float3 normal3 = mul(v.normal, (float3x3)skeletonDatas[blendIndex3]);
    float3 normal = normal0 * v.blendWeight.x + normal1 * v.blendWeight.y + normal2 * v.blendWeight.z + normal3 * v.blendWeight.w;

    float3 tangent0 = mul(v.tangent, (float3x3)skeletonDatas[blendIndex0]);
    float3 tangent1 = mul(v.tangent, (float3x3)skeletonDatas[blendIndex1]);
    float3 tangent2 = mul(v.tangent, (float3x3)skeletonDatas[blendIndex2]);
    float3 tangent3 = mul(v.tangent, (float3x3)skeletonDatas[blendIndex3]);
    float3 tangent = tangent0 * v.blendWeight.x + tangent1 * v.blendWeight.y + tangent2 * v.blendWeight.z + tangent3 * v.blendWeight.w;    

    VertexOut outVert;
    float4 worldPosition    = mul(pos, instData.matWorld);
    outVert.normal          = normalize(mul(normal, (float3x3)instData.matWorld));
    outVert.tangent         = normalize(mul(tangent, (float3x3)instData.matWorld));
    outVert.position        = mul(worldPosition, matViewProj);
    outVert.color           = v.color;
    outVert.uv              = v.uv;
    outVert.materialID      = instData.materialID;

    return outVert;
}

[NumThreads(128, 1, 1)]
[OutputTopology("triangle")]
void MS(uint gtid : SV_GROUPTHREADID, 
        uint gid : SV_GROUPID, 
        in payload Payload payload,
        out indices uint3 tris[MAX_PRIMS],
        out vertices VertexOut verts[MAX_VERTS])
{
    uint meshletIndex = gid / payload.instanceCountPerDispatchMesh;
    Meshlet meshlet = meshlets[meshletIndex];

    uint startInstance = gid % payload.instanceCountPerDispatchMesh;
    uint instanceCount = 1;

    if(meshletIndex == drawParam.meshletCount - 1)
    {
        const int instanceDataPerGroups = min(MAX_VERTS / meshlet.vertexCount, MAX_PRIMS / meshlet.primitiveCount);
        uint unpackedGroupCount = (drawParam.meshletCount - 1) * payload.instanceCountPerDispatchMesh;
        uint packedIndex = gid - unpackedGroupCount;

        startInstance = packedIndex * instanceDataPerGroups;
        instanceCount = min(payload.instanceCountPerDispatchMesh - startInstance, instanceDataPerGroups);
    }

    uint vertCount = meshlet.vertexCount * instanceCount;
    uint primCount = meshlet.primitiveCount * instanceCount;

    SetMeshOutputCounts(vertCount, primCount);

    if(gtid < vertCount)
    {
        uint readIndex = gtid % meshlet.vertexCount;
        uint instanceId = gtid / meshlet.vertexCount;

        uint vertexIndex = GetVertexIndex(meshlet, readIndex);
        uint instanceIndex = startInstance + instanceId + payload.instanceOffset;

        verts[gtid] = GetVertexAttributes(vertexIndex, instanceIndex);
    }
    
    if(gtid < primCount)
    {
        uint readIndex = gtid % meshlet.primitiveCount;
        uint instanceId = gtid / meshlet.primitiveCount;

        tris[gtid] = GetPrimitive(meshlet, readIndex) + (meshlet.vertexCount * instanceId);
    }
}

float4 PS(VertexOut psIn) : SV_Target
{
    int texIndex = materialDatas[round(psIn.materialID)].diffuseTextureIndex;
    float4 diffuse = textures[texIndex].Sample(samplerLinearWrap, psIn.uv);
    float nDotL = dot(psIn.normal, -lightDir);
    return float4(diffuse.xyz * nDotL + diffuse.xyz * ambient.xyz, 1.0f);
}