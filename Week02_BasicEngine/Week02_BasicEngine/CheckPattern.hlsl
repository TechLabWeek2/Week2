// CheckPattern.hlsl

struct PS_INPUT
{
    float4 position : SV_POSITION; // Transformed position to pass to the pixel shader
    float4 color : COLOR; // Color to pass to the pixel shader
    float2 uv : TEXCOORD0;
};

cbuffer constants : register(b0)
{    
    row_major float4x4 MVP;
    
    float HightLightIntensity;
    bool UseColor;
    float PatternNum;
    float Padding;
    float4 Color;
}

float4 mainPS(PS_INPUT input) : SV_TARGET
{
    float2 CheckerCoord = input.uv * PatternNum;
    CheckerCoord = floor(CheckerCoord);
    //CheckerCoord /= PatternNum;
    float PatternAlpha = fmod(CheckerCoord.x + CheckerCoord.y, 2.f);
    float4 CheckColor = lerp(float4(0.3f, 0.3f, 0.3f, input.color.a), float4(0.8f, 0.8f, 0.8f, input.color.a), PatternAlpha);
    //float4 CheckColor = lerp(float4(0.f, 0.f, 0.f, 1.f), float4(1.f, 1.f, 1.f, 1.f), input.uv.x);
    //float4 CheckColor = lerp(float4(0.f, 0.f, 0.f, 1.f), float4(1.f, 1.f, 1.f, 1.f), CheckerCoord.x);
    return CheckColor;
}
