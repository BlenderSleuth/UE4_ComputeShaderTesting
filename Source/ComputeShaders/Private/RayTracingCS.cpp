// Copyright Ben Sutherland 2021. All rights reserved.

#include "RayTracingCS.h"


//                      Shader Class            Shader Virtual Path			HLSL main function name			Type
IMPLEMENT_GLOBAL_SHADER(FRayTracingCS, "/ComputeShaders/RayTracingCS.usf",			"MainCS",			SF_Compute)

