//-------------------------------------------------------------------------------
//
// Lighting Demo
//   - Using GumshoeFramework (v1.00)
//
//  All code licensed under the MIT license
//
//-------------------------------------------------------------------------------

#pragma once

#include <PCH.h>

#include <App.h>
#include <InterfacePointers.h>
#include <Graphics\\Camera.h>
#include <Graphics\\Lights.h>
#include <Graphics\\SpriteFont.h>
#include <Graphics\\SpriteRenderer.h>
#include <Graphics\\GraphicsTypes.h>
#include <Graphics\\ShaderCompilation.h>

#include "MeshRenderer.h"
#include "Waves.h"

using namespace GumshoeFramework10;

typedef std::array<std::vector<MeshRenderObject>, uint64(Scenes::NumValues)> SceneMeshMapT;

class LightingDemo : public App
{

protected:

    FirstPersonCamera camera;

    SpriteFont font;
    GumshoeFramework10::SpriteRenderer spriteRenderer;

    // Use the BackBuffer in the DeviceManager class for rendering
    DepthStencilBuffer dsBuffer;
    uint64 frameCount = 0;

    // Renderer
    MeshRenderer meshRenderer;

    // Meshes for the various scenes
    SceneMeshMapT meshMap;

    // Wave simulation object
    Waves mWaves;

    // Directional light objects
    DirectionalLight mDirLights[3];

    // Point light objects
    PointLight mPointLight;
    SimpleMaterial mPointLightMat;
    XMFLOAT4X4 mPointLightWorld;
    MeshRenderObject mPointLightSphere;

    // Spot light objects
    SpotLight mSpotLight;
    SimpleMaterial mSpotLightMat;
    XMFLOAT4X4 mSpotLightWorld;
    MeshRenderObject mSpotLightCone;

    // Materials
    SimpleMaterial mLandMat;
    SimpleMaterial mWavesMat;
    SimpleMaterial mShapeMat[4];
    SimpleMaterial mSkullMat;

    // Define transformations from local spaces to world space.
    XMFLOAT4X4 mLandWorld;
    XMFLOAT4X4 mWavesWorld;

    XMFLOAT4X4 mShapeWorld[4];
    XMFLOAT4X4 mSphereWorldTranslate[10];
    XMFLOAT4X4 mCylWorldTranslate[10];
    XMFLOAT4X4 mSkullWorld;

    // Functions
    virtual void Initialize() override;
    virtual void Render(const Timer& timer) override;
    virtual void Update(const Timer& timer) override;
    virtual void BeforeReset() override;
    virtual void AfterReset() override;

    void CreateRenderTargets();

    void RenderScene();
    void RenderHUD();

    float GetHillHeight(float x, float z)const;
    XMFLOAT3 GetHillNormal(float x, float z)const;
    void BuildLandGeometryBuffers(ID3D11Device* device);
    void BuildWaveGeometryBuffers(ID3D11Device* device);
    void BuildShapeGeometryBuffers(ID3D11Device* device);
    void BuildSkullGeometryBuffers(ID3D11Device* device);
    void BuildLightGeometryBuffers(ID3D11Device* device);

public:

    LightingDemo();
};
