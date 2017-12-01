//-------------------------------------------------------------------------------
//
// Lighting Demo (Settings)
//   - Using GumshoeFramework (v1.00)
//
//  All code licensed under the MIT license
//
//-------------------------------------------------------------------------------

public class Settings
{
    enum Scenes
    {
        Terrain,
        Skull
    }
    
    public class SceneControls
    {
        Scenes CurrentScene = Scenes.Terrain;
        bool DoubleSyncInterval = false;
    }

    public class DirectionalLightControls
    {
        //Scenes CurrentScene = Scenes.Terrain;

        [HelpText("Enables ambient lighting from the environment")]
        bool EnableAmbientLighting = true;

        [HelpText("Enables directional lighting from the environment")]
        bool EnableDirectionalLight = true;

        [DisplayName("Light Direction")]
        [HelpText("The direction of the light")]
        Direction DirLightDirection = new Direction(0.57735f, -0.57735f, 0.57735f);

        [DisplayName("Light Color")]
        [HelpText("The color of the directional light")]
        [MinValue(0.0f)]
        [MaxValue(1.0f)]
        [StepSize(0.05f)]
        [HDR(false)]
        Color DirLightColor = new Color(0.5f, 0.5f, 0.5f);
    }

    public class PointLightControls
    {
        [HelpText("Enables point light")]
        bool EnablePointLight = true;

        [HelpText("Enables point light annimation")]
        bool EnablePointLightAnim = true;

        [DisplayName("Point Light Color")]
        [HelpText("The color of the point light")]
        [MinValue(0.0f)]
        [MaxValue(1.0f)]
        [StepSize(0.05f)]
        [HDR(false)]
        Color PointLightColor = new Color(0.7f, 0.7f, 0.7f);

        [DisplayName("PointLight Constant Attenuation")]
        [MinValue(0.0f)]
        [MaxValue(1.0f)]
        [StepSize(0.05f)]
        [HelpText("The constant falloff for the point light")]
        float PointLightConstAtten = 1.0f;

        [DisplayName("PointLight Linear Attenuation")]
        [MinValue(0.0f)]
        [MaxValue(1.0f)]
        [StepSize(0.01f)]
        [HelpText("The linear falloff for the point light")]
        float PointLightLinAtten = 0.08f;

        [DisplayName("PointLight Quadratic Attenuation")]
        [MinValue(0.0f)]
        [MaxValue(1.0f)]
        [StepSize(0.01f)]
        [HelpText("The quadratic falloff for the point light")]
        float PointLightQuadAtten = 0.0f;

        [DisplayName("PointLight Range")]
        [MinValue(0.0f)]
        [MaxValue(60.0f)]
        [StepSize(0.25f)]
        [HelpText("The radius for the range of the point light")]
        float PointLightRange = 30.0f;
    }

    public class SpotLightControls
    {
        [HelpText("Enables spot light")]
        bool EnableSpotLight = true;

        [HelpText("Enables spot light annimation")]
        bool EnableSpotLightAnim = true;

        [DisplayName("Spot Light Color")]
        [HelpText("The color of the spot light")]
        [MinValue(0.0f)]
        [MaxValue(1.0f)]
        [StepSize(0.05f)]
        [HDR(false)]
        Color SpotLightColor = new Color(0.7f, 0.7f, 0.0f);

        [DisplayName("SpotLight Range")]
        [MinValue(0.0f)]
        [MaxValue(60.0f)]
        [StepSize(0.25f)]
        [HelpText("The radius for the range of the spot light")]
        float SpotLightRange = 30.0f;

        [DisplayName("SpotLight Inner Angle")]
        [MinValue(0.0f)]
        [MaxValue(90.0f)]
        [StepSize(1.0f)]
        [HelpText("The inner angle (in degrees) of spot light cone")]
        float SpotInnerAngle = 20.0f;

        [DisplayName("SpotLight Outer Angle")]
        [MinValue(0.0f)]
        [MaxValue(90.0f)]
        [StepSize(1.0f)]
        [HelpText("The outer angle (in degrees) of spot light cone")]
        float SpotOuterAngle = 50.0f;

        [DisplayName("SpotLight Cone Falloff")]
        [MinValue(0.0f)]
        [MaxValue(10.0f)]
        [StepSize(0.01f)]
        [HelpText("The falloff of brightness from the inner angle to the outer angle")]
        float SpotLightConeFalloff = 0.60f;
    }

    // No auto-exposure for this sample
    //const bool EnableAutoExposure = false;
    //const float KeyValue = 0.115f;
    //const float AdaptationRate = 0.5f;
}