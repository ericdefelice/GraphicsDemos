//-------------------------------------------------------------------------------
//
// LightingDemo
//   - Using GumshoeFramework (v1.00)
//
//  All code licensed under the MIT license
//
//-------------------------------------------------------------------------------

#include <PCH.h>

#include "LightingDemo.h"
#include "SharedConstants.h"

#include "resource.h"
#include <InterfacePointers.h>
#include <Window.h>
#include <Graphics\\DeviceManager.h>
#include <Input.h>
#include <Graphics\\SpriteRenderer.h>
#include <Graphics\\Model.h>
#include <Utility.h>
#include <Graphics\\Camera.h>
#include <Graphics\\ShaderCompilation.h>
#include <Graphics\\Profiler.h>
#include <Graphics\\Textures.h>
#include <Graphics\\Sampling.h>
#include <Graphics\\GeometryGenerator.h>

using namespace GumshoeFramework10;
using std::wstring;

const uint32 WindowWidth = 1280;
const uint32 WindowHeight = 720;
const float WindowWidthF = static_cast<float>(WindowWidth);
const float WindowHeightF = static_cast<float>(WindowHeight);

static const float NearClip = 0.01f;
static const float FarClip = 1000.0f;

// Model filenames
static const wstring ModelPaths[uint64(Scenes::NumValues)] =
{
    L"",
    L""
};

LightingDemo::LightingDemo()
  :  App(L"Gumshoe Lighting Demo App", L"Icon.ico")
  ,  camera(WindowWidthF / WindowHeightF, Pi_4 * 0.75f, NearClip, FarClip)
{
    deviceManager.SetBackBufferWidth(WindowWidth);
    deviceManager.SetBackBufferHeight(WindowHeight);
    deviceManager.SetMinFeatureLevel(D3D_FEATURE_LEVEL_11_0);

    window.SetClientArea(WindowWidth, WindowHeight);

    // Setup the terrain scene world matrices
    XMMATRIX I = XMMatrixIdentity();
    XMStoreFloat4x4(&mLandWorld, I);
    XMStoreFloat4x4(&mWavesWorld, I);

    XMMATRIX wavesScale = XMMatrixScaling(1.0f, 1.0f, 1.0f);
    XMMATRIX wavesOffset = XMMatrixTranslation(0.0f, -10.0f, 0.0f);
    XMStoreFloat4x4(&mWavesWorld, XMMatrixMultiply(wavesScale, wavesOffset));

    // Setup the skull scene world matrices
    XMMATRIX boxScale = XMMatrixScaling(3.0f, 1.0f, 3.0f);
    XMMATRIX boxOffset = XMMatrixTranslation(0.0f, 0.5f, 0.0f);
    XMStoreFloat4x4(&mShapeWorld[0], XMMatrixMultiply(boxScale, boxOffset));

    XMStoreFloat4x4(&mShapeWorld[1], I);
    XMStoreFloat4x4(&mShapeWorld[2], I);
    XMStoreFloat4x4(&mShapeWorld[3], I);

    XMMATRIX skullScale = XMMatrixScaling(0.5f, 0.5f, 0.5f);
    XMMATRIX skullOffset = XMMatrixTranslation(0.0f, 1.0f, 0.0f);
    XMStoreFloat4x4(&mSkullWorld, XMMatrixMultiply(skullScale, skullOffset));

    for(int i = 0; i < 5; ++i)
    {
        XMStoreFloat4x4(&mCylWorldTranslate[i*2+0], XMMatrixTranslation(-5.0f, 1.5f, -10.0f + i*5.0f));
        XMStoreFloat4x4(&mCylWorldTranslate[i*2+1], XMMatrixTranslation(+5.0f, 1.5f, -10.0f + i*5.0f));

        XMStoreFloat4x4(&mSphereWorldTranslate[i*2+0], XMMatrixTranslation(-5.0f, 3.5f, -10.0f + i*5.0f));
        XMStoreFloat4x4(&mSphereWorldTranslate[i*2+1], XMMatrixTranslation(+5.0f, 3.5f, -10.0f + i*5.0f));
    }

    // Directional lights.
    mDirLights[0].Ambient  = XMFLOAT4(0.05f, 0.05f, 0.05f, 1.0f);
    mDirLights[0].Diffuse  = XMFLOAT4(0.5f, 0.5f, 0.5f, 1.0f);
    mDirLights[0].Specular = XMFLOAT4(0.5f, 0.5f, 0.5f, 1.0f);
    XMStoreFloat3(&mDirLights[0].Direction, AppSettings::DirLightDirection.Value().ToSIMD());

    mDirLights[1].Ambient  = XMFLOAT4(0.0f, 0.0f, 0.0f, 1.0f);
    mDirLights[1].Diffuse  = XMFLOAT4(0.20f, 0.20f, 0.20f, 1.0f);
    mDirLights[1].Specular = XMFLOAT4(0.25f, 0.25f, 0.25f, 1.0f);
    mDirLights[1].Direction = XMFLOAT3(-0.57735f, -0.57735f, 0.57735f);

    mDirLights[2].Ambient  = XMFLOAT4(0.0f, 0.0f, 0.0f, 1.0f);
    mDirLights[2].Diffuse  = XMFLOAT4(0.2f, 0.2f, 0.2f, 1.0f);
    mDirLights[2].Specular = XMFLOAT4(0.0f, 0.0f, 0.0f, 1.0f);
    mDirLights[2].Direction = XMFLOAT3(0.0f, -0.707f, -0.707f);
 
    // Point light--position is changed every frame to animate in UpdateScene function.
    mPointLight.Diffuse  = XMFLOAT4(0.7f, 0.7f, 0.7f, 1.0f);
    mPointLight.Specular = XMFLOAT4(0.7f, 0.7f, 0.7f, 1.0f);
    mPointLight.Att      = XMFLOAT3(1.0f, 0.1f, 0.0f);
    mPointLight.Range    = AppSettings::PointLightRange.Value();

    mPointLightMat.Ambient  = XMFLOAT4(0.0f, 0.0f, 0.0f, 0.0f);
    mPointLightMat.Diffuse  = XMFLOAT4(0.0f, 0.0f, 0.0f, 0.0f);
    mPointLightMat.Specular = XMFLOAT4(0.0f, 0.0f, 0.0f, 0.0f);
    mPointLightMat.Emissive = XMFLOAT4(0.7f, 0.7f, 0.7f, 1.0f);

    // Spot light--position and direction changed every frame to animate in UpdateScene function.
    mSpotLight.Diffuse    = XMFLOAT4(0.7f, 0.7f, 0.0f, 1.0f);
    mSpotLight.Specular   = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
    mSpotLight.Att        = XMFLOAT3(1.0f, 0.08f, 0.0f);
    mSpotLight.Falloff    = 0.25f;
    mSpotLight.InnerAngle = XMConvertToRadians(0.0f);
    mSpotLight.OuterAngle = XMConvertToRadians(30.0f);

    mSpotLightMat.Ambient  = XMFLOAT4(0.0f, 0.0f, 0.0f, 0.0f);
    mSpotLightMat.Diffuse  = XMFLOAT4(0.0f, 0.0f, 0.0f, 0.0f);
    mSpotLightMat.Specular = XMFLOAT4(0.0f, 0.0f, 0.0f, 0.0f);
    mSpotLightMat.Emissive = XMFLOAT4(1.0f, 1.0f, 0.0f, 1.0f);

    // Materials
    mLandMat.Ambient  = XMFLOAT4(0.48f, 0.77f, 0.46f, 1.0f);
    mLandMat.Diffuse  = XMFLOAT4(0.48f, 0.77f, 0.46f, 1.0f);
    mLandMat.Specular = XMFLOAT4(0.2f, 0.2f, 0.2f, 16.0f);

    mWavesMat.Ambient  = XMFLOAT4(0.137f, 0.42f, 0.556f, 1.0f);
    mWavesMat.Diffuse  = XMFLOAT4(0.137f, 0.42f, 0.556f, 1.0f);
    mWavesMat.Specular = XMFLOAT4(0.8f, 0.8f, 0.8f, 96.0f);

    mShapeMat[0].Ambient  = XMFLOAT4(0.651f, 0.5f, 0.392f, 1.0f);
    mShapeMat[0].Diffuse  = XMFLOAT4(0.651f, 0.5f, 0.392f, 1.0f);
    mShapeMat[0].Specular = XMFLOAT4(0.2f, 0.2f, 0.2f, 16.0f);

    mShapeMat[1].Ambient  = XMFLOAT4(0.48f, 0.77f, 0.46f, 1.0f);
    mShapeMat[1].Diffuse  = XMFLOAT4(0.48f, 0.77f, 0.46f, 1.0f);
    mShapeMat[1].Specular = XMFLOAT4(0.2f, 0.2f, 0.2f, 16.0f);

    mShapeMat[2].Ambient  = XMFLOAT4(0.1f, 0.2f, 0.3f, 1.0f);
    mShapeMat[2].Diffuse  = XMFLOAT4(0.2f, 0.4f, 0.6f, 1.0f);
    mShapeMat[2].Specular = XMFLOAT4(0.9f, 0.9f, 0.9f, 16.0f);

    mShapeMat[3].Ambient  = XMFLOAT4(0.7f, 0.85f, 0.7f, 1.0f);
    mShapeMat[3].Diffuse  = XMFLOAT4(0.7f, 0.85f, 0.7f, 1.0f);
    mShapeMat[3].Specular = XMFLOAT4(0.8f, 0.8f, 0.8f, 16.0f);

    mSkullMat.Ambient  = XMFLOAT4(0.8f, 0.8f, 0.8f, 1.0f);
    mSkullMat.Diffuse  = XMFLOAT4(0.8f, 0.8f, 0.8f, 1.0f);
    mSkullMat.Specular = XMFLOAT4(0.8f, 0.8f, 0.8f, 16.0f);
}

void LightingDemo::BeforeReset()
{
    App::BeforeReset();
}

void LightingDemo::AfterReset()
{
    App::AfterReset();

    float aspect = static_cast<float>(deviceManager.BackBufferWidth()) / deviceManager.BackBufferHeight();
    camera.SetAspectRatio(aspect);

    CreateRenderTargets();
}

void LightingDemo::Initialize()
{
    App::Initialize();

    ID3D11DevicePtr device = deviceManager.Device();
    ID3D11DeviceContextPtr deviceContext = deviceManager.ImmediateContext();
    
    // Create a font + SpriteRenderer
    font.Initialize(L"Arial", 14, SpriteFont::Regular, true, device);
    spriteRenderer.Initialize(device);

    // Camera setup
    camera.SetPosition(Float3(0.0f, 100.0f, -180.0f));

    // Initialize the waves model
    mWaves.Init(160, 160, 1.0f, 0.03f, 3.25f, 0.4f);

    // Build the terrain scene
    BuildLandGeometryBuffers(device);
    BuildWaveGeometryBuffers(device);

    // Build the skull scene
    BuildShapeGeometryBuffers(device);
    BuildSkullGeometryBuffers(device);

    // Build the light object geometry
    BuildLightGeometryBuffers(device);

    // Initialize the renderer
    meshRenderer.Initialize(device, deviceManager.ImmediateContext());
    meshRenderer.SetRenderObject(&meshMap[AppSettings::CurrentScene].front());
}

// Creates all required render targets
void LightingDemo::CreateRenderTargets()
{
    ID3D11Device* device = deviceManager.Device();
    uint32 width = deviceManager.BackBufferWidth();
    uint32 height = deviceManager.BackBufferHeight();

    dsBuffer.Initialize(device, width, height, DXGI_FORMAT_D24_UNORM_S8_UINT, false);
}

void LightingDemo::Update(const Timer& timer)
{
    AppSettings::UpdateUI();

    MouseState mouseState = MouseState::GetMouseState(window);
    KeyboardState kbState = KeyboardState::GetKeyboardState(window);

    if(kbState.IsKeyDown(KeyboardState::Escape))
        window.Destroy();

    float CamMoveSpeed = 15.0f * timer.DeltaSecondsF();
    const float CamRotSpeed = 0.180f * timer.DeltaSecondsF();
    const float MeshRotSpeed = 0.180f * timer.DeltaSecondsF();

    // Move the camera with keyboard input
    if(kbState.IsKeyDown(KeyboardState::LeftShift))
        CamMoveSpeed *= 0.50f;

    Float3 camPos = camera.Position();
    if(kbState.IsKeyDown(KeyboardState::W))
        camPos += camera.Forward() * CamMoveSpeed;
    else if (kbState.IsKeyDown(KeyboardState::S))
        camPos += camera.Back() * CamMoveSpeed;
    if(kbState.IsKeyDown(KeyboardState::A))
        camPos += camera.Left() * CamMoveSpeed;
    else if (kbState.IsKeyDown(KeyboardState::D))
        camPos += camera.Right() * CamMoveSpeed;
    if(kbState.IsKeyDown(KeyboardState::Q))
        camPos += camera.Up() * CamMoveSpeed;
    else if (kbState.IsKeyDown(KeyboardState::E))
        camPos += camera.Down() * CamMoveSpeed;
    camera.SetPosition(camPos);

    // Rotate the camera with the mouse
    if(mouseState.RButton.Pressed && mouseState.IsOverWindow)
    {
        float xRot = camera.XRotation();
        float yRot = camera.YRotation();
        xRot += mouseState.DY * CamRotSpeed;
        yRot += mouseState.DX * CamRotSpeed;
        camera.SetXRotation(xRot);
        camera.SetYRotation(yRot);
    }

    // Reset the camera projection
    camera.SetAspectRatio(camera.AspectRatio());

    // Update the direction light direction & color
    XMStoreFloat3(&mDirLights[0].Direction, AppSettings::DirLightDirection.Value().ToSIMD());
    Float3 dirLightColor = AppSettings::DirLightColor.Value();
    mDirLights[0].Diffuse = XMFLOAT4(dirLightColor.x, dirLightColor.y, dirLightColor.z, 1.0f);
    mDirLights[0].Specular = XMFLOAT4(dirLightColor.x, dirLightColor.y, dirLightColor.z, 1.0f);

    // Update the point light color & attenuation
    Float3 ptLightColor = AppSettings::PointLightColor.Value();
    mPointLight.Diffuse     = XMFLOAT4(ptLightColor.x, ptLightColor.y, ptLightColor.z, 1.0f);
    mPointLight.Specular    = XMFLOAT4(ptLightColor.x, ptLightColor.y, ptLightColor.z, 1.0f);
    mPointLightMat.Emissive = XMFLOAT4(ptLightColor.x, ptLightColor.y, ptLightColor.z, 1.0f);
    mPointLight.Att         = XMFLOAT3(AppSettings::PointLightConstAtten.Value(),
                                       AppSettings::PointLightLinAtten.Value(),
                                       AppSettings::PointLightQuadAtten.Value() );
    mPointLight.Range       = AppSettings::PointLightRange.Value();

    // Update the spot light color & attenuation
    Float3 spotLightColor = AppSettings::SpotLightColor.Value();
    mSpotLight.Diffuse     = XMFLOAT4(spotLightColor.x, spotLightColor.y, spotLightColor.z, 1.0f);
    mSpotLight.Specular    = XMFLOAT4(spotLightColor.x, spotLightColor.y, spotLightColor.z, 1.0f);
    mSpotLightMat.Emissive = XMFLOAT4(spotLightColor.x, spotLightColor.y, spotLightColor.z, 1.0f);
    mSpotLight.Falloff     = AppSettings::SpotLightConeFalloff.Value();
    mSpotLight.InnerAngle  = XMConvertToRadians(AppSettings::SpotInnerAngle.Value());
    mSpotLight.OuterAngle  = XMConvertToRadians(AppSettings::SpotOuterAngle.Value());
    mSpotLight.Range       = AppSettings::SpotLightRange.Value();

    // Toggle VSYNC
    if(kbState.RisingEdge(KeyboardState::V))
        deviceManager.SetVSYNCEnabled(!deviceManager.VSYNCEnabled());

    deviceManager.SetNumVSYNCIntervals(AppSettings::DoubleSyncInterval ? 2 : 1);

    if(AppSettings::CurrentScene.Changed())
    {
        meshRenderer.SetRenderObject(&meshMap[AppSettings::CurrentScene].front());
        
        // Update camera position on scene change
        if (AppSettings::CurrentScene == uint64(Scenes::Terrain))
            camera.SetPosition(Float3(0.0f, 100.0f, -180.0f));
        else
            camera.SetPosition(Float3(0.0f, 25.0f, -25.0f));
    }

    if( AppSettings::CurrentScene == uint64(Scenes::Terrain) )
    {
        // Every quarter second, generate a random wave.
        static float tBase = 0.0f;
        if( (timer.ElapsedMillisecondsF() - tBase) >= 4.00f )
        {
            tBase += 4.00f;
     
            DWORD i = 5 + rand() % (mWaves.RowCount()-10);
            DWORD j = 5 + rand() % (mWaves.ColumnCount()-10);

            float r = RandFloat(0.0f, 1.0f);

            mWaves.Disturb(i, j, r);
        }

        mWaves.Update(timer.DeltaSecondsF());

        // Update the wave vertex buffer with the new solution
        ID3D11DeviceContextPtr context = deviceManager.ImmediateContext();

        D3D11_MAPPED_SUBRESOURCE mappedData;
        DXCall(context->Map(meshMap[uint64(Scenes::Terrain)].back().vertexBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedData));

        SimpleVertex* v = reinterpret_cast<SimpleVertex*>(mappedData.pData);
        for(UINT i = 0; i < mWaves.VertexCount(); ++i)
        {
            v[i].Pos    = mWaves[i];
            v[i].Normal = mWaves.Normal(i);
        }

        context->Unmap(meshMap[uint64(Scenes::Terrain)].back().vertexBuffer, 0);

        // Animate the point light.
        if (AppSettings::EnablePointLightAnim)
        {
            // Circle light over the land surface.
            mPointLight.Position.x = 70.0f*cosf( 0.2f*timer.ElapsedSecondsF() );
            mPointLight.Position.z = 70.0f*sinf( 0.2f*timer.ElapsedSecondsF() );
            mPointLight.Position.y = Max( GetHillHeight(mPointLight.Position.x, mPointLight.Position.z), -3.0f ) + 10.0f;
        }

        // The spotlight takes on the camera position and is aimed in the
        // same direction the camera is looking.  In this way, it looks
        // like we are holding a flashlight.
        XMStoreFloat3(&mSpotLight.Position, camPos.ToSIMD());

        // Set the spot light direction to be the same as the camera direction
        Float4x4 lookAtMat = camera.ViewMatrix();
        mSpotLight.Direction = XMFLOAT3(lookAtMat._13, lookAtMat._23, lookAtMat._33);
    }
    else
    {
        // Animate the point light.
        if (AppSettings::EnablePointLightAnim)
        {
            mPointLight.Position.x = 10.0f*cosf( 0.2f*timer.ElapsedSecondsF() );
            mPointLight.Position.z = 10.0f*sinf( 0.2f*timer.ElapsedSecondsF() );
            mPointLight.Position.y = 2.0f + cosf( 1.0f*timer.ElapsedSecondsF() );
        }

        if (AppSettings::EnableSpotLightAnim)
        {
            // Animate the spot light.
            mSpotLight.Position.x = 8.0f*sinf( 0.2f*timer.ElapsedSecondsF() );
            mSpotLight.Position.z = 8.0f*cosf( 0.2f*timer.ElapsedSecondsF() );
            mSpotLight.Position.y = 8.0f;

            mSpotLight.Direction = XMFLOAT3(0.2f, -0.8f, 0.0f);
            //mSpotLight.Direction = XMFLOAT3(0.0f, -1.0f, 0.0f);
        }
    }

    // Update the point light world
    XMStoreFloat4x4(&mPointLightWorld, XMMatrixTranslation(mPointLight.Position.x, mPointLight.Position.y, mPointLight.Position.z));

    // Update the spot light world
    //XMMATRIX spotTrans = XMMatrixTranslation(mSpotLight.Position.x, mSpotLight.Position.y, mSpotLight.Position.z);
    //XMMATRIX spotRotateX = XMMatrixRotationX(mSpotLight.Direction.x);
    //XMMATRIX spotRotateZ = XMMatrixRotationZ(mSpotLight.Direction.x);
    //XMMATRIX spotRotate = XMMatrixMultiply(spotRotateX, spotRotateZ);
    //XMStoreFloat4x4(&mSpotLightWorld, XMMatrixMultiply(spotTrans, spotRotateZ));
    XMStoreFloat4x4(&mSpotLightWorld, XMMatrixTranslation(mSpotLight.Position.x, mSpotLight.Position.y, mSpotLight.Position.z));
}

void LightingDemo::Render(const Timer& timer)
{
    ID3D11DeviceContextPtr context = deviceManager.ImmediateContext();

    AppSettings::UpdateCBuffer(context);

    // Set default render targets
    ID3D11RenderTargetView* renderTargets[1] = { deviceManager.BackBuffer() };
    context->OMSetRenderTargets(1, renderTargets, dsBuffer.DSView);

    // Render the current scene
    RenderScene();

    // No post-processing for this demo

    context->OMSetRenderTargets(1, renderTargets, NULL);

    SetViewport(context, deviceManager.BackBufferWidth(), deviceManager.BackBufferHeight());

    RenderHUD();

    ++frameCount;
}

void LightingDemo::RenderScene()
{
    PIXEvent event(L"Render Scene");

    ID3D11DeviceContextPtr context = deviceManager.ImmediateContext();

    float clearColor[4] = { 0.0f, 0.0f, 0.0f, 0.0f };
    context->ClearRenderTargetView(deviceManager.BackBuffer(), clearColor);
    context->ClearDepthStencilView(dsBuffer.DSView, D3D11_CLEAR_DEPTH|D3D11_CLEAR_STENCIL, 1.0f, 0);

    SetViewport(context, deviceManager.BackBufferWidth(), deviceManager.BackBufferHeight());
    
    // Loop through all of the meshes in the current scene and render them
    std::vector<MeshRenderObject> currSceneMeshes = meshMap[uint64(AppSettings::CurrentScene)];
    for (uint32 i = 0; i < currSceneMeshes.size(); i++)
    {
        // Update the location of the spheres and cylinders if it is the skull scene
        if (AppSettings::CurrentScene == uint64(Scenes::Skull) && (i == 2 || i == 3))
        {
            for (uint32 j = 0; j < 10; j++)
            {
                if (i == 2)
                    currSceneMeshes[i].world = &mSphereWorldTranslate[j];
                else
                    currSceneMeshes[i].world = &mCylWorldTranslate[j];
 
                meshRenderer.SetRenderObject(&currSceneMeshes[i]);
                meshRenderer.Render(context, camera, mDirLights[0], mPointLight, mSpotLight);
            }
        }
        else
        {
            meshRenderer.SetRenderObject(&currSceneMeshes[i]);
            meshRenderer.Render(context, camera, mDirLights[0], mPointLight, mSpotLight);
        }
    }

    // Render the lights
    if (AppSettings::EnablePointLight)
    {
        meshRenderer.SetRenderObject(&mPointLightSphere);
        meshRenderer.Render(context, camera, mDirLights[0], mPointLight, mSpotLight);
    }

    if (AppSettings::EnableSpotLight)
    {
        meshRenderer.SetRenderObject(&mSpotLightCone);
        meshRenderer.Render(context, camera, mDirLights[0], mPointLight, mSpotLight);
    }
}

void LightingDemo::RenderHUD()
{
    PIXEvent pixEvent(L"HUD Pass");

    spriteRenderer.Begin(deviceManager.ImmediateContext(), SpriteRenderer::Point);

    Float4x4 transform = Float4x4::TranslationMatrix(Float3(25.0f, 25.0f, 0.0f));
    wstring fpsText(L"FPS: ");
    fpsText += ToString(fps) + L" (" + ToString(1000.0f / fps) + L"ms)";
    spriteRenderer.RenderText(font, fpsText.c_str(), transform, XMFLOAT4(1, 1, 0, 1));

    transform._42 += 25.0f;
    wstring vsyncText(L"VSYNC (V): ");
    vsyncText += deviceManager.VSYNCEnabled() ? L"Enabled" : L"Disabled";
    spriteRenderer.RenderText(font, vsyncText.c_str(), transform, XMFLOAT4(1, 1, 0, 1));

    Profiler::GlobalProfiler.EndFrame(spriteRenderer, font);

    spriteRenderer.End();
}

float LightingDemo::GetHillHeight(float x, float z)const
{
    return 0.3f*( z*sinf(0.1f*x) + x*cosf(0.1f*z) );
}

XMFLOAT3 LightingDemo::GetHillNormal(float x, float z)const
{
    // n = (-df/dx, 1, -df/dz)
    XMFLOAT3 n(
        -0.03f*z*cosf(0.1f*x) - 0.3f*cosf(0.1f*z),
        1.0f,
        -0.3f*sinf(0.1f*x) + 0.03f*x*sinf(0.1f*z));
    
    XMVECTOR unitNormal = XMVector3Normalize(XMLoadFloat3(&n));
    XMStoreFloat3(&n, unitNormal);

    return n;
}

void LightingDemo::BuildLandGeometryBuffers(ID3D11Device* device)
{
    GeometryGenerator::MeshData grid;
 
    GeometryGenerator geoGen;

    geoGen.CreateGrid(160.0f, 160.0f, 50, 50, grid);

    UINT landIndexCount = (UINT)grid.Indices.size();

    // Object to be added to the mesh render list for the scene
    MeshRenderObject landRenderObject;

    // Extract the vertex elements we are interested and apply the height function to
    // each vertex.  
    std::vector<SimpleVertex> vertices(grid.Vertices.size());
    for(size_t i = 0; i < grid.Vertices.size(); ++i)
    {
        XMFLOAT3 p = grid.Vertices[i].Position;

        p.y = GetHillHeight(p.x, p.z);
        
        vertices[i].Pos    = p;
        vertices[i].Normal = GetHillNormal(p.x, p.z);
    }

    D3D11_BUFFER_DESC vbd;
    vbd.Usage = D3D11_USAGE_IMMUTABLE;
    vbd.ByteWidth = sizeof(SimpleVertex) * (UINT)grid.Vertices.size();
    vbd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
    vbd.CPUAccessFlags = 0;
    vbd.MiscFlags = 0;
    D3D11_SUBRESOURCE_DATA vinitData;
    vinitData.pSysMem = &vertices[0];
    DXCall(device->CreateBuffer(&vbd, &vinitData, &landRenderObject.vertexBuffer));

    // Pack the indices of all the meshes into one index buffer.
    D3D11_BUFFER_DESC ibd;
    ibd.Usage = D3D11_USAGE_IMMUTABLE;
    ibd.ByteWidth = sizeof(UINT) * landIndexCount;
    ibd.BindFlags = D3D11_BIND_INDEX_BUFFER;
    ibd.CPUAccessFlags = 0;
    ibd.MiscFlags = 0;
    D3D11_SUBRESOURCE_DATA iinitData;
    iinitData.pSysMem = &grid.Indices[0];
    DXCall(device->CreateBuffer(&ibd, &iinitData, &landRenderObject.indexBuffer));

    // Add the mesh to the render object list for the scene
    landRenderObject.vertexDescIdx = 0;
    landRenderObject.indexCount    = landIndexCount;
    landRenderObject.material      = &mLandMat;
    landRenderObject.world         = &mLandWorld;

    meshMap[uint64(Scenes::Terrain)].push_back(landRenderObject);
}

void LightingDemo::BuildWaveGeometryBuffers(ID3D11Device* device)
{
    // Object to be added to the mesh render list for the scene
    MeshRenderObject wavesRenderObject;

    // Create the vertex buffer.  Note that we allocate space only, as
    // we will be updating the data every time step of the simulation.
    D3D11_BUFFER_DESC vbd;
    vbd.Usage = D3D11_USAGE_DYNAMIC;
    vbd.ByteWidth = sizeof(SimpleVertex) * mWaves.VertexCount();
    vbd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
    vbd.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
    vbd.MiscFlags = 0;
    DXCall(device->CreateBuffer(&vbd, 0, &wavesRenderObject.vertexBuffer));


    // Create the index buffer.  The index buffer is fixed, so we only 
    // need to create and set once.
    std::vector<UINT> indices(3*mWaves.TriangleCount()); // 3 indices per face

    // Iterate over each quad.
    UINT m = mWaves.RowCount();
    UINT n = mWaves.ColumnCount();
    int k = 0;
    for(UINT i = 0; i < m-1; ++i)
    {
        for(DWORD j = 0; j < n-1; ++j)
        {
            indices[k]   = i*n+j;
            indices[k+1] = i*n+j+1;
            indices[k+2] = (i+1)*n+j;

            indices[k+3] = (i+1)*n+j;
            indices[k+4] = i*n+j+1;
            indices[k+5] = (i+1)*n+j+1;

            k += 6; // next quad
        }
    }

    D3D11_BUFFER_DESC ibd;
    ibd.Usage = D3D11_USAGE_IMMUTABLE;
    ibd.ByteWidth = sizeof(UINT) * (UINT)indices.size();
    ibd.BindFlags = D3D11_BIND_INDEX_BUFFER;
    ibd.CPUAccessFlags = 0;
    ibd.MiscFlags = 0;
    D3D11_SUBRESOURCE_DATA iinitData;
    iinitData.pSysMem = &indices[0];
    DXCall(device->CreateBuffer(&ibd, &iinitData, &wavesRenderObject.indexBuffer));

    // Add the mesh to the render object list for the scene
    wavesRenderObject.vertexDescIdx = 0;
    wavesRenderObject.indexCount    = UINT(3*mWaves.TriangleCount());
    wavesRenderObject.material      = &mWavesMat;
    wavesRenderObject.world         = &mWavesWorld;

    meshMap[uint64(Scenes::Terrain)].push_back(wavesRenderObject);
}

void LightingDemo::BuildShapeGeometryBuffers(ID3D11Device* device)
{
    // box, grid, sphere, cylinder meshes
    GeometryGenerator::MeshData shape[4];

    GeometryGenerator geoGen;
    geoGen.CreateBox(1.0f, 1.0f, 1.0f, shape[0]);
    geoGen.CreateGrid(20.0f, 30.0f, 60, 40, shape[1]);
    geoGen.CreateSphere(0.5f, 20, 20, shape[2]);
    geoGen.CreateCylinder(0.5f, 0.3f, 3.0f, 20, 20, shape[3]);

    // Extract the vertex and index elements from the mesh
    // and store into the scenes mesh map
    for (int j = 0; j < 4; j++)
    {
        // Object to be added to the mesh render list for the scene
        MeshRenderObject tempRenderObject;

        UINT shapeIndexCount  = (UINT)shape[j].Indices.size();
        int  shapeVertexCount = (int)shape[j].Vertices.size();
        
        std::vector<SimpleVertex> vertices(shapeVertexCount);

        for(int i = 0; i < shapeVertexCount; i++)
        {
            vertices[i].Pos    = shape[j].Vertices[i].Position;
            vertices[i].Normal = shape[j].Vertices[i].Normal;
        }

        // Fill the vertex buffer for the shape
        D3D11_BUFFER_DESC vbd;
        vbd.Usage = D3D11_USAGE_IMMUTABLE;
        vbd.ByteWidth = sizeof(SimpleVertex) * shapeVertexCount;
        vbd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
        vbd.CPUAccessFlags = 0;
        vbd.MiscFlags = 0;
        D3D11_SUBRESOURCE_DATA vinitData;
        vinitData.pSysMem = &vertices[0];
        DXCall(device->CreateBuffer(&vbd, &vinitData, &tempRenderObject.vertexBuffer));

        // Fill the intex buffer for the shape
        std::vector<UINT> indices;
        indices.insert(indices.end(), shape[j].Indices.begin(), shape[j].Indices.end());

        D3D11_BUFFER_DESC ibd;
        ibd.Usage = D3D11_USAGE_IMMUTABLE;
        ibd.ByteWidth = sizeof(UINT) * shapeIndexCount;
        ibd.BindFlags = D3D11_BIND_INDEX_BUFFER;
        ibd.CPUAccessFlags = 0;
        ibd.MiscFlags = 0;
        D3D11_SUBRESOURCE_DATA iinitData;
        iinitData.pSysMem = &indices[0];
        DXCall(device->CreateBuffer(&ibd, &iinitData, &tempRenderObject.indexBuffer));

        // Add the mesh to the render object list for the scene
        tempRenderObject.vertexDescIdx = 0;
        tempRenderObject.indexCount    = shapeIndexCount;
        tempRenderObject.material      = &mShapeMat[j];
        tempRenderObject.world         = &mShapeWorld[j];

        meshMap[uint64(Scenes::Skull)].push_back(tempRenderObject);
    }
}

void LightingDemo::BuildSkullGeometryBuffers(ID3D11Device* device)
{
    std::ifstream fin("../Content/Models/skull.txt");
    
    if(!fin)
    {
        MessageBox(0, L"../Content/Models/skull.txt not found.", 0, 0);
        return;
    }

    // Object to be added to the mesh render list for the scene
    MeshRenderObject tempRenderObject;

    UINT vcount = 0;
    UINT tcount = 0;
    std::string ignore;

    fin >> ignore >> vcount;
    fin >> ignore >> tcount;
    fin >> ignore >> ignore >> ignore >> ignore;
    
    std::vector<SimpleVertex> vertices(vcount);
    for(UINT i = 0; i < vcount; ++i)
    {
        fin >> vertices[i].Pos.x >> vertices[i].Pos.y >> vertices[i].Pos.z;
        fin >> vertices[i].Normal.x >> vertices[i].Normal.y >> vertices[i].Normal.z;
    }

    fin >> ignore;
    fin >> ignore;
    fin >> ignore;

    UINT skullIndexCount = 3*tcount;
    std::vector<UINT> indices(skullIndexCount);
    for(UINT i = 0; i < tcount; ++i)
    {
        fin >> indices[i*3+0] >> indices[i*3+1] >> indices[i*3+2];
    }

    fin.close();

    // Fill the vertex buffer
    D3D11_BUFFER_DESC vbd;
    vbd.Usage = D3D11_USAGE_IMMUTABLE;
    vbd.ByteWidth = sizeof(SimpleVertex) * vcount;
    vbd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
    vbd.CPUAccessFlags = 0;
    vbd.MiscFlags = 0;
    D3D11_SUBRESOURCE_DATA vinitData;
    vinitData.pSysMem = &vertices[0];
    DXCall(device->CreateBuffer(&vbd, &vinitData, &tempRenderObject.vertexBuffer));

    // Fill the index buffer
    D3D11_BUFFER_DESC ibd;
    ibd.Usage = D3D11_USAGE_IMMUTABLE;
    ibd.ByteWidth = sizeof(UINT) * skullIndexCount;
    ibd.BindFlags = D3D11_BIND_INDEX_BUFFER;
    ibd.CPUAccessFlags = 0;
    ibd.MiscFlags = 0;
    D3D11_SUBRESOURCE_DATA iinitData;
    iinitData.pSysMem = &indices[0];
    DXCall(device->CreateBuffer(&ibd, &iinitData, &tempRenderObject.indexBuffer));

    // Add the mesh to the render object list for the scene
    tempRenderObject.vertexDescIdx = 0;
    tempRenderObject.indexCount    = skullIndexCount;
    tempRenderObject.material      = &mSkullMat;
    tempRenderObject.world         = &mSkullWorld;

    meshMap[uint64(Scenes::Skull)].push_back(tempRenderObject);
}

void LightingDemo::BuildLightGeometryBuffers(ID3D11Device* device)
{
    // box, grid, sphere, cylinder meshes
    GeometryGenerator::MeshData shape[2];

    GeometryGenerator geoGen;
    geoGen.CreateSphere(0.25f, 10, 10, shape[0]);
    geoGen.CreateCylinder(0.20f, 0.0f, 0.50f, 10, 10, shape[1]);

    // Extract the vertex and index elements from the mesh
    // and store into the scenes mesh map
    for (int j = 0; j < 2; j++)
    {
        // Object to be added to the mesh render list for the scene
        MeshRenderObject tempRenderObject;

        UINT shapeIndexCount  = (UINT)shape[j].Indices.size();
        int  shapeVertexCount = (int)shape[j].Vertices.size();
        
        std::vector<SimpleVertex> vertices(shapeVertexCount);

        for(int i = 0; i < shapeVertexCount; i++)
        {
            vertices[i].Pos    = shape[j].Vertices[i].Position;
            vertices[i].Normal = shape[j].Vertices[i].Normal;
        }

        // Fill the vertex buffer for the shape
        D3D11_BUFFER_DESC vbd;
        vbd.Usage = D3D11_USAGE_IMMUTABLE;
        vbd.ByteWidth = sizeof(SimpleVertex) * shapeVertexCount;
        vbd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
        vbd.CPUAccessFlags = 0;
        vbd.MiscFlags = 0;
        D3D11_SUBRESOURCE_DATA vinitData;
        vinitData.pSysMem = &vertices[0];
        DXCall(device->CreateBuffer(&vbd, &vinitData, &tempRenderObject.vertexBuffer));

        // Fill the intex buffer for the shape
        std::vector<UINT> indices;
        indices.insert(indices.end(), shape[j].Indices.begin(), shape[j].Indices.end());

        D3D11_BUFFER_DESC ibd;
        ibd.Usage = D3D11_USAGE_IMMUTABLE;
        ibd.ByteWidth = sizeof(UINT) * shapeIndexCount;
        ibd.BindFlags = D3D11_BIND_INDEX_BUFFER;
        ibd.CPUAccessFlags = 0;
        ibd.MiscFlags = 0;
        D3D11_SUBRESOURCE_DATA iinitData;
        iinitData.pSysMem = &indices[0];
        DXCall(device->CreateBuffer(&ibd, &iinitData, &tempRenderObject.indexBuffer));

        // Add the mesh to the render object list for the scene
        tempRenderObject.vertexDescIdx = 0;
        tempRenderObject.indexCount    = shapeIndexCount;
        if (j == 0)
        {
            tempRenderObject.material = &mPointLightMat;
            tempRenderObject.world    = &mPointLightWorld;
            mPointLightSphere = tempRenderObject;
        } else {
            tempRenderObject.material = &mSpotLightMat;
            tempRenderObject.world    = &mSpotLightWorld;
            mSpotLightCone = tempRenderObject;
        }
    }
}

// -------------------------------------------------------------------
// Main Entry Point
// -------------------------------------------------------------------
int APIENTRY wWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPWSTR lpCmdLine, int nCmdShow)
{
    LightingDemo app;
    app.Run();
}