#define ROOT_SIG "SRV(t0), \
                  UAV(u0), \
                  UAV(u1), \
                  RootConstants(b0, num32BitConstants = 2)"

#define MAX_BONES 256

struct SkeletonUpdateInstance
{
    uint skeletonInstanceOffset;
    uint numBones;
};

struct TransformData
{
    float4x4 offset;
    float4x4 anim;
    int parentIndex;
};

struct DrawParam
{
    uint dispatchOffset;
    uint numInstancePerBones;
};

StructuredBuffer<SkeletonUpdateInstance> skeletonUpdateInstanceInfos : register(t0);
RWStructuredBuffer<TransformData> boneDatas : register(u0);
RWStructuredBuffer<float4x4> outputBuffer : register(u1);
ConstantBuffer<DrawParam> drawParams : register(b0);

[RootSignature(ROOT_SIG)]
[numthreads(64, 1, 1)]
void CS(int dtid : SV_DISPATCHTHREADID)
{
    uint skeletonInstanceIdex = dtid + drawParams.dispatchOffset;
    if(skeletonInstanceIdex >= drawParams.numInstancePerBones)
    {
        return;
    }
    
    uint skeletonInstanceOffset = skeletonUpdateInstanceInfos[skeletonInstanceIdex].skeletonInstanceOffset;
    uint begIndex = skeletonInstanceOffset;
    uint endIndex = begIndex + skeletonUpdateInstanceInfos[skeletonInstanceIdex].numBones;
    
    float4x4 animUpdateBuffer[MAX_BONES];
    for(int i = begIndex; i < endIndex; i++)
    {
        animUpdateBuffer[i - begIndex] = boneDatas[i].anim;
    }


    for(i = begIndex; i < endIndex; i++)
    {
        float4x4 localAnim = animUpdateBuffer[i - begIndex];
        uint parentIndex = boneDatas[i].parentIndex + skeletonInstanceOffset;
        if(boneDatas[i].parentIndex >= 0)
        {
            float4x4 parentAnim = animUpdateBuffer[parentIndex - begIndex];
            animUpdateBuffer[i - begIndex] = mul(localAnim, parentAnim);
        }
    }

    for(i = begIndex; i < endIndex; i++)
    {
        float4x4 anim = animUpdateBuffer[i - begIndex];
        float4x4 offset = boneDatas[i].offset;
        outputBuffer[i] = mul(offset, anim);
    }
}