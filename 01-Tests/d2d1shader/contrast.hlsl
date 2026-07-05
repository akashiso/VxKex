//********************************************************* 
// 
// Copyright (c) Microsoft. All rights reserved. 
// This code is licensed under the MIT License (MIT). 
// THIS CODE IS PROVIDED *AS IS* WITHOUT WARRANTY OF 
// ANY KIND, EITHER EXPRESS OR IMPLIED, INCLUDING ANY 
// IMPLIED WARRANTIES OF FITNESS FOR A PARTICULAR 
// PURPOSE, MERCHANTABILITY, OR NON-INFRINGEMENT. 
// 
//*********************************************************

// Custom effects using pixel shaders should use HLSL helper functions defined in
// d2d1effecthelpers.hlsli to make use of effect shader linking.
#define D2D_INPUT_COUNT 1           // The pixel shader takes 1 input texture.
#define D2D_INPUT0_SIMPLE

// Note that the custom build step must provide the correct path to find d2d1effecthelpers.hlsli when calling fxc.exe.
#include "F:\Windows Kits\10\Include\10.0.19041.0\um\d2d1effecthelpers.hlsli"

cbuffer constants : register(b0)
{
    float parameter : packoffset(c0.x);
    bool clampInput : packoffset(c0.y);
};

D2D_PS_ENTRY(main)
{
    float4 color0 = D2DGetInput(0);
    
    if (clampInput)
        color0 = saturate(color0);

    float s = 1.0 - (3.0 * parameter / 400.0);
    bool4 cond = color0 > 0.5;
    
    float4 v1 = 2.0 * (1.0 - s) * color0 * color0 + s * color0;
    float4 v2 = 2.0 * (s - 1.0) * color0 * color0 + (4.0 - 3.0 * s) * color0 + s - 1.0;

    return cond ? v2 : v1;
}