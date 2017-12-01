#pragma once

#include <PCH.h>
#include <Settings.h>
#include <Graphics\GraphicsTypes.h>

using namespace GumshoeFramework10;

enum class Scenes
{
    Terrain = 0,
    Skull = 1,

    NumValues
};

typedef EnumSettingT<Scenes> ScenesSetting;

namespace AppSettings
{

    extern ScenesSetting CurrentScene;
    extern BoolSetting DoubleSyncInterval;
    extern BoolSetting EnableAmbientLighting;
    extern BoolSetting EnableDirectionalLight;
    extern DirectionSetting DirLightDirection;
    extern ColorSetting DirLightColor;
    extern BoolSetting EnablePointLight;
    extern BoolSetting EnablePointLightAnim;
    extern ColorSetting PointLightColor;
    extern FloatSetting PointLightConstAtten;
    extern FloatSetting PointLightLinAtten;
    extern FloatSetting PointLightQuadAtten;
    extern FloatSetting PointLightRange;
    extern BoolSetting EnableSpotLight;
    extern BoolSetting EnableSpotLightAnim;
    extern ColorSetting SpotLightColor;
    extern FloatSetting SpotLightRange;
    extern FloatSetting SpotInnerAngle;
    extern FloatSetting SpotOuterAngle;
    extern FloatSetting SpotLightConeFalloff;

    struct AppSettingsCBuffer
    {
        int32 CurrentScene;
        bool32 DoubleSyncInterval;
        bool32 EnableAmbientLighting;
        bool32 EnableDirectionalLight;
        Float3 DirLightDirection;
        Float4Align Float3 DirLightColor;
        bool32 EnablePointLight;
        bool32 EnablePointLightAnim;
        Float3 PointLightColor;
        float PointLightConstAtten;
        float PointLightLinAtten;
        float PointLightQuadAtten;
        float PointLightRange;
        bool32 EnableSpotLight;
        bool32 EnableSpotLightAnim;
        Float4Align Float3 SpotLightColor;
        float SpotLightRange;
        float SpotInnerAngle;
        float SpotOuterAngle;
        float SpotLightConeFalloff;
    };

    extern ConstantBuffer<AppSettingsCBuffer> CBuffer;

    void Initialize(ID3D11Device* device);
    void Update();
    void UpdateCBuffer(ID3D11DeviceContext* context);
};

// ================================================================================================

namespace AppSettings
{
    void UpdateUI();
}