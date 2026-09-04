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
    float3 offset;
    float Scale;
    float3 rotation;
}

PS_INPUT mainVS(VS_INPUT input)
{
    PS_INPUT output;
    
    // 1. Scale 적용
    float x = input.position.x * Scale;
    float y = input.position.y * Scale;
    float z = input.position.z * Scale;
    
    // Z축 회전
    float cosZ = cos(rotation.z);
    float sinZ = sin(rotation.z);
    float rx1 = x * cosZ - y * sinZ;
    float ry1 = x * sinZ + y * cosZ;
    float rz1 = z;

    // Y축 회전
    float cosY = cos(rotation.y);
    float sinY = sin(rotation.y);
    float rx2 = rx1 * cosY + rz1 * sinY;
    float ry2 = ry1;
    float rz2 = -rx1 * sinY + rz1 * cosY;

    // X축 회전
    float cosX = cos(rotation.x);
    float sinX = sin(rotation.x);
    float finalX = rx2;
    float finalY = ry2 * cosX - rz2 * sinX;
    float finalZ = ry2 * sinX + rz2 * cosX;

    // 3. Translation (Location) 적용
    finalX += offset.x;
    finalY += offset.y;
    finalZ += offset.z;
    
    output.position.x = finalX;
    output.position.y = finalY;
    output.position.z = finalZ;
    output.position.w = 1.f;
    
    // Pass the color to the pixel shader
    output.color = input.color;
    
    return output;
}

float4 mainPS(PS_INPUT input) : SV_TARGET
{
    // Output the color directly
    return input.color;
}
