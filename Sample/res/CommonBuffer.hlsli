#ifndef CAMERA_BUFFER_HLSLI
#define CAMERA_BUFFER_HLSLI
#endif//CAMERA_BUFFER_HLSLI
///////////////////////////////////////////////////////////////////////////////
// Common constant buffer.
///////////////////////////////////////////////////////////////////////////////
cbuffer CbCommon : register(b2)
{
    float3 CameraPosition   : packoffset(c0); // �J�����ʒu�ł�.
    float2 FogArea          : packoffset(c1); // Fog�̃X�^�[�g�ʒu�ƃG���h�ʒu
    float3 FogColor         : packoffset(c2); // Fog�̐F
}