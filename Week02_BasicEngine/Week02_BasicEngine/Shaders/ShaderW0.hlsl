// ShaderW0.hlsl
struct VS_INPUT
{
    float4 position : POSITION; // Input position from vertex buffer
    float4 color : COLOR; // Input color from vertex buffer
    float2 uv : TEXCOORD0;
};

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

PS_INPUT mainVS(VS_INPUT input)
{
    PS_INPUT output;
    
    
    output.position = mul(input.position, MVP);
    //output.color = input.color * Color;
    output.color = UseColor? Color : input.color;
    //output.color = input.color;
    
    // Pass the color to the pixel shader
    
    //output.color = lerp(input.color, HightLightIntensity, 0.5); // 하이라이트 효과
    output.color.rgb = output.color.rgb * HightLightIntensity; // 하이라이트 효과
    //output.color.a = 0.8f;
    
    output.uv = input.uv;
    
    return output;
}

float4 mainPS(PS_INPUT input) : SV_TARGET
{
    // Output the color directly
    return input.color;
}
