#ifndef RAYTRACING_HLSL_COMPATIBILITY_DEFINITIONS_H
#define RAYTRACING_HLSL_COMPATIBILITY_DEFINITIONS_H

#ifdef HLSL
	#include "Fox/Graphics/DirectX/HlslCompatibilityDefinitions.h"
#else
	using namespace DirectX;
	typedef UINT16 Index;
#endif

    struct PerFrame
    {
        XMMATRIX inverseProjection;
        XMVECTOR cameraPosition;
        XMVECTOR lightPosition;
        XMVECTOR lightAmbientColor;
        XMVECTOR lightDiffuseColor;
    };

    struct CubeData
    {
        XMFLOAT4 albedo;
    };

    struct Vertex
    {
        XMFLOAT3 position;
        XMFLOAT3 normal;
    };

#endif