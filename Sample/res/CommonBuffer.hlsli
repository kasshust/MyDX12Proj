#ifndef CAMERA_BUFFER_HLSLI
#define CAMERA_BUFFER_HLSLI
#endif//CAMERA_BUFFER_HLSLI
///////////////////////////////////////////////////////////////////////////////
// Common constant buffer.
///////////////////////////////////////////////////////////////////////////////
cbuffer CbCommon : register(b2)
{
    float3 CameraPosition   : packoffset(c0); // カメラ位置です.
    float2 FogArea          : packoffset(c1); // Fogのスタート位置とエンド位置
    float3 FogColor         : packoffset(c2); // Fogの色
}