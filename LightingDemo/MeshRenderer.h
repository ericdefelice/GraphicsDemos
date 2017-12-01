//-------------------------------------------------------------------------------
//
// LightingDemo (MeshRenderer)
//   - Using GumshoeFramework (v1.00)
//
//  All code licensed under the MIT license
//
//-------------------------------------------------------------------------------

#pragma once

#include <PCH.h>

#include <Graphics\\Model.h>
#include <Graphics\\GraphicsTypes.h>
#include <Graphics\\DeviceStates.h>
#include <Graphics\\Camera.h>
#include <Graphics\\Lights.h>
#include <Graphics\\SH.h>
#include <Graphics\\ShaderCompilation.h>

#include "AppSettings.h"

using namespace GumshoeFramework10;

struct SimpleVertex
{
    XMFLOAT3 Pos;
    XMFLOAT3 Normal;
};

// Vertex Layout
static const D3D11_INPUT_ELEMENT_DESC PosNormVertexDesc[] =
{
    {"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0},
    {"NORMAL",    0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0}
};

// Mesh Object to be rendered
class MeshRenderObject
{

public:
    
    UINT             vertexDescIdx;
    UINT             indexCount;
    ID3D11BufferPtr  vertexBuffer;
    ID3D11BufferPtr  indexBuffer;
    SimpleMaterial*  material;
    XMFLOAT4X4*      world;
};

// Mesh Renderer class
class MeshRenderer
{

public:

    void Initialize(ID3D11Device* device, ID3D11DeviceContext* context);
    void BuildVertexLayout();

    void Render(ID3D11DeviceContext* context, const Camera& camera,
                const DirectionalLight& dirLight, const PointLight& pointLight,
                const SpotLight& spotLight);
    void SetRenderObject(const MeshRenderObject* obj);
    void Update();

protected:

    void LoadShaders();

    ID3D11DevicePtr device;

    BlendStates blendStates;
    RasterizerStates rasterizerStates;
    DepthStencilStates depthStencilStates;
    SamplerStates samplerStates;

    ID3D11SamplerStatePtr AnisoSampler;
    ID3D11SamplerStatePtr LinearSampler;

    VertexShaderPtr meshVS;
    PixelShaderPtr  meshPS;

    const MeshRenderObject* renderObject = nullptr;
    ID3D11InputLayoutPtr meshInputLayout;

    // Constant buffers
    struct MeshPerObjConsts
    {
        Float4Align Float4x4 World;
        Float4Align Float4x4 View;
        Float4Align Float4x4 WorldViewProjection;
        Float4Align SimpleMaterial Material;
    };

    struct MeshPerFrameConsts
    {
        Float4Align DirectionalLight DirLight;
        Float4Align PointLight       PointLight;
        Float4Align SpotLight        SpotLight;
        Float4Align Float3           CameraPosWS;           
    };

    ConstantBuffer<MeshPerObjConsts>   meshPerObjConstBuffer;
    ConstantBuffer<MeshPerFrameConsts> meshPerFrameConstBuffer;
};