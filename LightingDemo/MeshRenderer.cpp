//-------------------------------------------------------------------------------
//
// Lighting Demo (MeshRenderer)
//   - Using GumshoeFramework (v1.00)
//
//  All code licensed under the MIT license
//
//-------------------------------------------------------------------------------

#include "PCH.h"

#include "MeshRenderer.h"

#include <Exceptions.h>
#include <Utility.h>
#include <Graphics\\ShaderCompilation.h>
#include <App.h>
#include <Graphics\\Textures.h>
#include <GF_Math.h>

#include "AppSettings.h"
#include "SharedConstants.h"

// Constants

void MeshRenderer::LoadShaders()
{
    // Load the mesh shaders
    CompileOptions opts;
    
    meshVS = CompileVSFromFile(device, L"Mesh.hlsl", "VS", "vs_5_0", opts);
    meshPS = CompilePSFromFile(device, L"Mesh.hlsl", "PS", "ps_5_0", opts);
}

void MeshRenderer::SetRenderObject(const MeshRenderObject* obj)
{
    this->renderObject = obj;

    VertexShaderPtr vs = meshVS;
    ID3D11InputLayoutPtr inputLayout;

    // Create the vertex input layout.
    DXCall( device->CreateInputLayout(PosNormVertexDesc, 2,
            vs->ByteCode->GetBufferPointer(), vs->ByteCode->GetBufferSize(), &meshInputLayout) );
}

// Loads resources
void MeshRenderer::Initialize(ID3D11Device* device, ID3D11DeviceContext* context)
{
    this->device = device;

    blendStates.Initialize(device);
    rasterizerStates.Initialize(device);
    depthStencilStates.Initialize(device);
    samplerStates.Initialize(device);

    meshPerObjConstBuffer.Initialize(device);
    meshPerFrameConstBuffer.Initialize(device);

    LoadShaders();

    // Create the sampler states
    D3D11_SAMPLER_DESC sampDesc = SamplerStates::AnisotropicDesc();
    DXCall(device->CreateSamplerState(&sampDesc, &AnisoSampler));

    sampDesc = SamplerStates::LinearDesc();
    DXCall(device->CreateSamplerState(&sampDesc, &LinearSampler));
}

void MeshRenderer::Update()
{
}

// Renders a single mesh object in the scene
void MeshRenderer::Render(ID3D11DeviceContext* context, const Camera& camera,
                          const DirectionalLight& dirLight, const PointLight& pointLight,
                          const SpotLight& spotLight)
{
    PIXEvent event(L"Mesh Rendering");

    // Set states
    float blendFactor[4] = {1, 1, 1, 1};
    context->OMSetBlendState(blendStates.BlendDisabled(), blendFactor, 0xFFFFFFFF);
    context->OMSetDepthStencilState(depthStencilStates.DepthEnabled(), 0);
    context->RSSetState(rasterizerStates.BackFaceCull());

    // Set the per-frame constants
    meshPerFrameConstBuffer.Data.DirLight = dirLight;
    meshPerFrameConstBuffer.Data.PointLight = pointLight;
    meshPerFrameConstBuffer.Data.SpotLight = spotLight;
    meshPerFrameConstBuffer.Data.CameraPosWS = camera.Position();
    meshPerFrameConstBuffer.ApplyChanges(context);
    meshPerFrameConstBuffer.SetPS(context, 0);

    // Get the world space transform for that object
    Float4x4 world = *(renderObject->world);

    // Set the per-object constants
    meshPerObjConstBuffer.Data.World = Float4x4::Transpose(world);
    meshPerObjConstBuffer.Data.View = Float4x4::Transpose(camera.ViewMatrix());
    meshPerObjConstBuffer.Data.WorldViewProjection = Float4x4::Transpose(world * camera.ViewProjectionMatrix());
    meshPerObjConstBuffer.Data.Material = *(renderObject->material);
    meshPerObjConstBuffer.ApplyChanges(context);
    meshPerObjConstBuffer.SetVS(context, 1);
    meshPerObjConstBuffer.SetPS(context, 1);

    // Set shaders
    context->DSSetShader(nullptr, nullptr, 0);
    context->HSSetShader(nullptr, nullptr, 0);
    context->GSSetShader(nullptr, nullptr, 0);
    context->VSSetShader(meshVS,  nullptr, 0);
    context->PSSetShader(meshPS,  nullptr, 0);

    // Draw the current render object bound to the renderer
    context->IASetInputLayout(meshInputLayout);
    context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

    UINT stride = sizeof(SimpleVertex);
    UINT offset = 0;

	ID3D11Buffer* const vtxBufferPtr = &(*renderObject->vertexBuffer);

	context->IASetVertexBuffers(0, 1, &vtxBufferPtr, &stride, &offset);
    context->IASetIndexBuffer(renderObject->indexBuffer, DXGI_FORMAT_R32_UINT, 0);
    
    context->DrawIndexed(renderObject->indexCount, 0, 0);
}
