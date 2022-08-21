#ifndef PATH_TRACER_HLSL
#define PATH_TRACER_HLSL

#define HLSL
#include "../../Graphics/DirectX/HlslCompatibilityDefinitions.h"

typedef BuiltInTriangleIntersectionAttributes MyAttributes;
struct RayPayload
{
    float4 color;
};

[shader("raygeneration")]
void MyRayGenerationShader() {
}

[shader("closesthit")]
void MyClosestHitShader(inout RayPayload payload, in MyAttributes attr) {
}

[shader("miss")]
void MyMissShader(inout RayPayload payload) {
}

#endif 