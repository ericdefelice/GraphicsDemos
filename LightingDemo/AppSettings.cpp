#include <PCH.h>
#include <TwHelper.h>
#include "AppSettings.h"

using namespace GumshoeFramework10;

static const char* ScenesLabels[2] =
{
    "Terrain",
    "Skull",
};

namespace AppSettings
{
    ScenesSetting CurrentScene;
    BoolSetting DoubleSyncInterval;
    BoolSetting EnableAmbientLighting;
    BoolSetting EnableDirectionalLight;
    DirectionSetting DirLightDirection;
    ColorSetting DirLightColor;
    BoolSetting EnablePointLight;
    BoolSetting EnablePointLightAnim;
    ColorSetting PointLightColor;
    FloatSetting PointLightConstAtten;
    FloatSetting PointLightLinAtten;
    FloatSetting PointLightQuadAtten;
    FloatSetting PointLightRange;
    BoolSetting EnableSpotLight;
    BoolSetting EnableSpotLightAnim;
    ColorSetting SpotLightColor;
    FloatSetting SpotLightRange;
    FloatSetting SpotInnerAngle;
    FloatSetting SpotOuterAngle;
    FloatSetting SpotLightConeFalloff;

    ConstantBuffer<AppSettingsCBuffer> CBuffer;

    void Initialize(ID3D11Device* device)
    {
        TwBar* tweakBar = Settings.TweakBar();

        CurrentScene.Initialize(tweakBar, "CurrentScene", "Scene Controls", "Current Scene", "", Scenes::Terrain, 2, ScenesLabels);
        Settings.AddSetting(&CurrentScene);

        DoubleSyncInterval.Initialize(tweakBar, "DoubleSyncInterval", "Scene Controls", "Double Sync Interval", "", false);
        Settings.AddSetting(&DoubleSyncInterval);

        EnableAmbientLighting.Initialize(tweakBar, "EnableAmbientLighting", "Directional Light Controls", "Enable Ambient Lighting", "Enables ambient lighting from the environment", true);
        Settings.AddSetting(&EnableAmbientLighting);

        EnableDirectionalLight.Initialize(tweakBar, "EnableDirectionalLight", "Directional Light Controls", "Enable Directional Light", "Enables directional lighting from the environment", true);
        Settings.AddSetting(&EnableDirectionalLight);

        DirLightDirection.Initialize(tweakBar, "DirLightDirection", "Directional Light Controls", "Light Direction", "The direction of the light", Float3(0.5774f, -0.5774f, 0.5774f));
        Settings.AddSetting(&DirLightDirection);

        DirLightColor.Initialize(tweakBar, "DirLightColor", "Directional Light Controls", "Light Color", "The color of the directional light", Float3(0.5000f, 0.5000f, 0.5000f), false, 0.0000f, 1.0000f, 0.0500f, ColorUnit::None);
        Settings.AddSetting(&DirLightColor);

        EnablePointLight.Initialize(tweakBar, "EnablePointLight", "Point Light Controls", "Enable Point Light", "Enables point light", true);
        Settings.AddSetting(&EnablePointLight);

        EnablePointLightAnim.Initialize(tweakBar, "EnablePointLightAnim", "Point Light Controls", "Enable Point Light Anim", "Enables point light annimation", true);
        Settings.AddSetting(&EnablePointLightAnim);

        PointLightColor.Initialize(tweakBar, "PointLightColor", "Point Light Controls", "Point Light Color", "The color of the point light", Float3(0.7000f, 0.7000f, 0.7000f), false, 0.0000f, 1.0000f, 0.0500f, ColorUnit::None);
        Settings.AddSetting(&PointLightColor);

        PointLightConstAtten.Initialize(tweakBar, "PointLightConstAtten", "Point Light Controls", "PointLight Constant Attenuation", "The constant falloff for the point light", 1.0000f, 0.0000f, 1.0000f, 0.0500f, ConversionMode::None, 1.0000f);
        Settings.AddSetting(&PointLightConstAtten);

        PointLightLinAtten.Initialize(tweakBar, "PointLightLinAtten", "Point Light Controls", "PointLight Linear Attenuation", "The linear falloff for the point light", 0.0800f, 0.0000f, 1.0000f, 0.0100f, ConversionMode::None, 1.0000f);
        Settings.AddSetting(&PointLightLinAtten);

        PointLightQuadAtten.Initialize(tweakBar, "PointLightQuadAtten", "Point Light Controls", "PointLight Quadratic Attenuation", "The quadratic falloff for the point light", 0.0000f, 0.0000f, 1.0000f, 0.0100f, ConversionMode::None, 1.0000f);
        Settings.AddSetting(&PointLightQuadAtten);

        PointLightRange.Initialize(tweakBar, "PointLightRange", "Point Light Controls", "PointLight Range", "The radius for the range of the point light", 30.0000f, 0.0000f, 60.0000f, 0.2500f, ConversionMode::None, 1.0000f);
        Settings.AddSetting(&PointLightRange);

        EnableSpotLight.Initialize(tweakBar, "EnableSpotLight", "Spot Light Controls", "Enable Spot Light", "Enables spot light", true);
        Settings.AddSetting(&EnableSpotLight);

        EnableSpotLightAnim.Initialize(tweakBar, "EnableSpotLightAnim", "Spot Light Controls", "Enable Spot Light Anim", "Enables spot light annimation", true);
        Settings.AddSetting(&EnableSpotLightAnim);

        SpotLightColor.Initialize(tweakBar, "SpotLightColor", "Spot Light Controls", "Spot Light Color", "The color of the spot light", Float3(0.7000f, 0.7000f, 0.0000f), false, 0.0000f, 1.0000f, 0.0500f, ColorUnit::None);
        Settings.AddSetting(&SpotLightColor);

        SpotLightRange.Initialize(tweakBar, "SpotLightRange", "Spot Light Controls", "SpotLight Range", "The radius for the range of the spot light", 30.0000f, 0.0000f, 60.0000f, 0.2500f, ConversionMode::None, 1.0000f);
        Settings.AddSetting(&SpotLightRange);

        SpotInnerAngle.Initialize(tweakBar, "SpotInnerAngle", "Spot Light Controls", "SpotLight Inner Angle", "The inner angle (in degrees) of spot light cone", 20.0000f, 0.0000f, 90.0000f, 1.0000f, ConversionMode::None, 1.0000f);
        Settings.AddSetting(&SpotInnerAngle);

        SpotOuterAngle.Initialize(tweakBar, "SpotOuterAngle", "Spot Light Controls", "SpotLight Outer Angle", "The outer angle (in degrees) of spot light cone", 50.0000f, 0.0000f, 90.0000f, 1.0000f, ConversionMode::None, 1.0000f);
        Settings.AddSetting(&SpotOuterAngle);

        SpotLightConeFalloff.Initialize(tweakBar, "SpotLightConeFalloff", "Spot Light Controls", "SpotLight Cone Falloff", "The falloff of brightness from the inner angle to the outer angle", 0.6000f, 0.0000f, 10.0000f, 0.0100f, ConversionMode::None, 1.0000f);
        Settings.AddSetting(&SpotLightConeFalloff);

        TwHelper::SetOpened(tweakBar, "Scene Controls", true);

        TwHelper::SetOpened(tweakBar, "Directional Light Controls", true);

        TwHelper::SetOpened(tweakBar, "Point Light Controls", true);

        TwHelper::SetOpened(tweakBar, "Spot Light Controls", true);

        CBuffer.Initialize(device);
    }

    void Update()
    {
    }

    void UpdateCBuffer(ID3D11DeviceContext* context)
    {
        CBuffer.Data.CurrentScene = CurrentScene;
        CBuffer.Data.DoubleSyncInterval = DoubleSyncInterval;
        CBuffer.Data.EnableAmbientLighting = EnableAmbientLighting;
        CBuffer.Data.EnableDirectionalLight = EnableDirectionalLight;
        CBuffer.Data.DirLightDirection = DirLightDirection;
        CBuffer.Data.DirLightColor = DirLightColor;
        CBuffer.Data.EnablePointLight = EnablePointLight;
        CBuffer.Data.EnablePointLightAnim = EnablePointLightAnim;
        CBuffer.Data.PointLightColor = PointLightColor;
        CBuffer.Data.PointLightConstAtten = PointLightConstAtten;
        CBuffer.Data.PointLightLinAtten = PointLightLinAtten;
        CBuffer.Data.PointLightQuadAtten = PointLightQuadAtten;
        CBuffer.Data.PointLightRange = PointLightRange;
        CBuffer.Data.EnableSpotLight = EnableSpotLight;
        CBuffer.Data.EnableSpotLightAnim = EnableSpotLightAnim;
        CBuffer.Data.SpotLightColor = SpotLightColor;
        CBuffer.Data.SpotLightRange = SpotLightRange;
        CBuffer.Data.SpotInnerAngle = SpotInnerAngle;
        CBuffer.Data.SpotOuterAngle = SpotOuterAngle;
        CBuffer.Data.SpotLightConeFalloff = SpotLightConeFalloff;

        CBuffer.ApplyChanges(context);
        CBuffer.SetVS(context, 7);
        CBuffer.SetHS(context, 7);
        CBuffer.SetDS(context, 7);
        CBuffer.SetGS(context, 7);
        CBuffer.SetPS(context, 7);
        CBuffer.SetCS(context, 7);
    }
}

// ================================================================================================

namespace AppSettings
{
    void UpdateUI()
    {
 
    }
}
