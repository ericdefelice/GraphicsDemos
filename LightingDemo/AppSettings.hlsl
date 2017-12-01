cbuffer AppSettings : register(b7)
{
    int CurrentScene;
    bool DoubleSyncInterval;
    bool EnableAmbientLighting;
    bool EnableDirectionalLight;
    float3 DirLightDirection;
    float3 DirLightColor;
    bool EnablePointLight;
    bool EnablePointLightAnim;
    float3 PointLightColor;
    float PointLightConstAtten;
    float PointLightLinAtten;
    float PointLightQuadAtten;
    float PointLightRange;
    bool EnableSpotLight;
    bool EnableSpotLightAnim;
    float3 SpotLightColor;
    float SpotLightRange;
    float SpotInnerAngle;
    float SpotOuterAngle;
    float SpotLightConeFalloff;
}

static const int Scenes_Terrain = 0;
static const int Scenes_Skull = 1;

