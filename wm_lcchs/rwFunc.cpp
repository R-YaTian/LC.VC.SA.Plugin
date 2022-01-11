﻿#include "rwFunc.h"
#include "../deps/selector/asnew.hpp"

__int32 *rwFunc::RsGlobalW;
__int32 *rwFunc::RsGlobalH;

void *rwFunc::fpRwRenderStateSet;
void *rwFunc::fpRwIm2DRenderPrimitive;

__int32 rwFunc::RwRenderStateSet(RwRenderState state, void *value)
{
	return ((__int32(__cdecl *)(RwRenderState, void *))(fpRwRenderStateSet))(state, value);
}

__int32 rwFunc::RwIm2DRenderPrimitive(RwPrimitiveType primType, RwD3D8Vertex *vertices, __int32 numVertices)
{
	return ((__int32(__cdecl *)(RwPrimitiveType, RwD3D8Vertex *, __int32))(fpRwIm2DRenderPrimitive))(primType, vertices, numVertices);
}

rwFunc::rwFunc()
{
    RsGlobalW = addr_sel::lc::select_address<__int32>({ 0x8F436C, 0x0, 0x904560 });
	RsGlobalH = RsGlobalW + 1;

    fpRwRenderStateSet = addr_sel::lc::select_address({ 0x5A43C0, 0x0, 0x5A5360 });
    fpRwIm2DRenderPrimitive = addr_sel::lc::select_address({ 0x5A4430, 0x0, 0x5A5410 });
}

static rwFunc instance;
