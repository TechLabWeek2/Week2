// ShaderW0.hlsl
struct VS_INPUT
{
    float4 position : POSITION; // Input position from vertex buffer
    float4 color : COLOR; // Input color from vertex buffer
};

struct PS_INPUT
{
    float4 position : SV_POSITION; // Transformed position to pass to the pixel shader
    float4 color : COLOR; // Color to pass to the pixel shader
};

cbuffer constants : register(b0)
{
    row_major float4x4 World;
    
    float HightLightIntensity;
    float3 Padding;
}

PS_INPUT mainVS(VS_INPUT input)
{
    PS_INPUT output;
    
    
    output.position = mul(input.position, World);
    
    // Pass the color to the pixel shader
    
    //output.color = lerp(input.color, HightLightIntensity, 0.5); // 하이라이트 효과
    output.color = input.color * HightLightIntensity; // 하이라이트 효과
    
    return output;
}

float4 mainPS(PS_INPUT input) : SV_TARGET
{
    // Output the color directly
    return input.color;
}
