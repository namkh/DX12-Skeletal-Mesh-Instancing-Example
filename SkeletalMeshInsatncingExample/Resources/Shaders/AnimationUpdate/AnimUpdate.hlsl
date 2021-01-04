#define ROOT_SIG "SRV(t0), \
                  SRV(t1), \
                  SRV(t2), \
                  SRV(t3), \
                  UAV(u0), \
                  RootConstants(b0, num32BitConstants = 2)"

#define QUATERNION_IDENTITY float4(0, 0, 0, 1)

struct AnimUpdateInstance
{
    float time;
    float duration;
    uint skeletonOffset;
    uint keyCount;
};

struct AnimUpdateInstancePerBone
{
    uint animDataOffset;
    uint boneIndex;
    uint animUpdateInstanceIndex;
    uint hasAnim;
};

struct AnimKey
{
    float3 translation;
    float4 rotation;
    float3 scale;
    float time;
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

StructuredBuffer<AnimUpdateInstance> animUpdateInstanceList : register(t0);
StructuredBuffer<AnimUpdateInstancePerBone> animUpdateInstancePerBoneList : register(t1);
StructuredBuffer<AnimKey> animKeys : register(t2);
StructuredBuffer<TransformData> skeletonDatas : register(t3);
ConstantBuffer<DrawParam> drawParams : register(b0);
RWStructuredBuffer<TransformData> output : register(u0);

static const float4x4 MAT_IDENTITY =
{
    1.0f, 0.0f, 0.0f, 0.0f,
    0.0f, 1.0f, 0.0f, 0.0f,
    0.0f, 0.0f, 1.0f, 0.0f,
    0.0f, 0.0f, 0.0f, 1.0f
};

float4 q_slerp(in float4 a, in float4 b, float t)
{
    if (length(a) == 0.0)
    {
        if (length(b) == 0.0)
        {
            return QUATERNION_IDENTITY;
        }
        return b;
    }
    else if (length(b) == 0.0)
    {
        return a;
    }

    float cosHalfAngle = a.w * b.w + dot(a.xyz, b.xyz);

    if (cosHalfAngle >= 1.0 || cosHalfAngle <= -1.0)
    {
        return a;
    }
    else if (cosHalfAngle < 0.0)
    {
        b.xyz = -b.xyz;
        b.w = -b.w;
        cosHalfAngle = -cosHalfAngle;
    }

    float blendA;
    float blendB;
    if (cosHalfAngle < 0.99)
    {
        float halfAngle = acos(cosHalfAngle);
        float sinHalfAngle = sin(halfAngle);
        float oneOverSinHalfAngle = 1.0 / sinHalfAngle;
        blendA = sin(halfAngle * (1.0 - t)) * oneOverSinHalfAngle;
        blendB = sin(halfAngle * t) * oneOverSinHalfAngle;
    }
    else
    {
        blendA = 1.0 - t;
        blendB = t;
    }

    float4 result = float4(blendA * a.xyz + blendB * b.xyz, blendA * a.w + blendB * b.w);
    if (length(result) > 0.0)
    {
        return normalize(result);
    }
    return QUATERNION_IDENTITY;
}


float4x4 quaternion_to_matrix(float4 quat)
{
    float4x4 m = float4x4(float4(0, 0, 0, 0), float4(0, 0, 0, 0), float4(0, 0, 0, 0), float4(0, 0, 0, 0));

    float x = quat.x, y = quat.y, z = quat.z, w = quat.w;
    float x2 = x + x, y2 = y + y, z2 = z + z;
    float xx = x * x2, xy = x * y2, xz = x * z2;
    float yy = y * y2, yz = y * z2, zz = z * z2;
    float wx = w * x2, wy = w * y2, wz = w * z2;

    m[0][0] = 1.0 - (yy + zz);
    m[0][1] = xy - wz;
    m[0][2] = xz + wy;

    m[1][0] = xy + wz;
    m[1][1] = 1.0 - (xx + zz);
    m[1][2] = yz - wx;

    m[2][0] = xz - wy;
    m[2][1] = yz + wx;
    m[2][2] = 1.0 - (xx + yy);

    m[3][3] = 1.0;

    return m;
}

[RootSignature(ROOT_SIG)]
[numthreads(64, 1, 1)]
void CS(int dtid : SV_DISPATCHTHREADID)
{
    uint instPerBoneIndex = dtid + drawParams.dispatchOffset;
    if(instPerBoneIndex >= drawParams.numInstancePerBones)
    {
        return;
    }

    AnimUpdateInstancePerBone animUpdateInstancePerBone = animUpdateInstancePerBoneList[instPerBoneIndex];
    AnimUpdateInstance animUpdateInstance = animUpdateInstanceList[animUpdateInstancePerBone.animUpdateInstanceIndex];
    uint srcBoneIndex = animUpdateInstancePerBone.boneIndex + animUpdateInstance.skeletonOffset;
    
    if(animUpdateInstancePerBone.hasAnim != 0)
    {
        float3 translation;
        float4 rotation;
        float3 scale;

        uint rightKeyIndex = ((animUpdateInstance.time * (1.0f / animUpdateInstance.duration)) * (float)(animUpdateInstance.keyCount)) + animUpdateInstancePerBone.animDataOffset;
        uint leftKeyIndex = 0;
        
        if(rightKeyIndex != 0)
        {
            uint leftKeyIndex = rightKeyIndex - 1;
            AnimKey leftKey = animKeys[leftKeyIndex];
            float leftKeyTime = leftKey.time;

            AnimKey rightKey = animKeys[rightKeyIndex];
            float rightKeyTime = rightKey.time;

            float keyTimeDelta = rightKeyTime - leftKeyTime;
            float blendWeight = (animUpdateInstance.time - leftKeyTime) * (1.0f / keyTimeDelta);
    
            translation = lerp(leftKey.translation, rightKey.translation, blendWeight);
            rotation = q_slerp(leftKey.rotation, rightKey.rotation, blendWeight);
            scale = lerp(leftKey.scale, rightKey.scale, blendWeight);
        }
        else
        {
            AnimKey rightKey = animKeys[rightKeyIndex];
            translation = rightKey.translation;
            rotation = rightKey.rotation;
            scale = rightKey.scale;
        }

        float4x4 matTrans =
        {
            1.0f, 0.0f, 0.0f, 0.0f,
            0.0f, 1.0f, 0.0f, 0.0f,
            0.0f, 0.0f, 1.0f, 0.0f,
            translation.x, translation.y, translation.z, 1.0f
        };

        float4x4 matQuat = quaternion_to_matrix(rotation);
        matQuat = transpose(matQuat);

        float4x4 matScale =
        {
            scale.x, 0.0f, 0.0f, 0.0f,
            0.0f, scale.y, 0.0f, 0.0f,
            0.0f, 0.0f, scale.z, 0.0f,
            0.0f, 0.0f, 0.0f, 1.0f
        };

        float4x4 matAnim = mul(matQuat, matScale);
        matAnim = mul(matAnim, matTrans) ;

        output[instPerBoneIndex].offset = skeletonDatas[srcBoneIndex].offset;
        output[instPerBoneIndex].anim = matAnim;
    }
    else
    {
        output[instPerBoneIndex].offset = skeletonDatas[srcBoneIndex].offset;
        output[instPerBoneIndex].anim = MAT_IDENTITY;
    }
    
    if(skeletonDatas[srcBoneIndex].parentIndex >= 0)
    {
        output[instPerBoneIndex].parentIndex = skeletonDatas[srcBoneIndex].parentIndex + animUpdateInstance.skeletonOffset;
    }
    else
    {
        output[instPerBoneIndex].parentIndex = -1;
    }
}