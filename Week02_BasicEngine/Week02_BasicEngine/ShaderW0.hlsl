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
}

PS_INPUT mainVS(VS_INPUT input)
{
    PS_INPUT output;

    /*
    float FOV = 45.0f; // 시야각 (예: 1.57f 대략 90도 또는 radians(45.0f))
    float AspectRatio = 9.0f / 9.0f; // 화면 가로/세로 비율 (예: 16.0f / 9.0f)
    float NearZ = 0.1f; // 최소 클리핑 거리 (예: 0.1f)
    float FarZ = 100.f; // 최대 클리핑 거리 (예: 100.0f)
    float3 worldPos = float3(finalX, finalY, finalZ);
    // [2-1] 카메라 위치를 뺀다 (역이동)
    float3 viewPos = worldPos - CameraLocation;

    // [2-2] 카메라 회전의 역회전 적용 (마이너스 각도 사용, X -> Y -> Z 역순 적용)
    float3 camRad = radians(-CameraRotation);

     cosX = cos(camRad.x);
     sinX = sin(camRad.x);
     cosY = cos(camRad.y);
     sinY = sin(camRad.y);
     cosZ = cos(camRad.z);
     sinZ = sin(camRad.z);

// X축 역회전
    float ty1 = viewPos.y * cosX - viewPos.z * sinX;
    float tz1 = viewPos.y * sinX + viewPos.z * cosX;
    float tx1 = viewPos.x;

// Y축 역회전
    float tx2 = tx1 * cosY + tz1 * sinY;
    float tz2 = -tx1 * sinY + tz1 * cosY;
    float ty2 = ty1;

// Z축 역회전
    float finalViewX = tx2 * cosZ - ty2 * sinZ;
    float finalViewY = tx2 * sinZ + ty2 * cosZ;
    float finalViewZ = tz2;

// 이제 이 최종 뷰 좌표가 카메라 기준의 진짜 거리가 됩니다!
    float viewZ = finalViewZ;
    if (viewZ <= 0.0001f)
        viewZ = 0.0001f; // 0 나누기 방지

// --- 3. 원근 투영(Perspective Projection) 적용 ---
    float f = 1.0f / tan(FOV * 0.5f);
    float projX = (finalViewX * (f / AspectRatio)) / viewZ;
    float projY = (finalViewY * f) / viewZ;

    output.position.x = projX * viewZ;
    output.position.y = projY * viewZ;
    output.position.z = ((FarZ / (FarZ - NearZ)) * viewZ - (FarZ * NearZ) / (FarZ - NearZ));
    output.position.w = viewZ; // 이 W값이 핵심입니다!
    */
    
    output.position = mul(input.position, World);
    
    // Pass the color to the pixel shader
    output.color = input.color;
    
    return output;
}

float4 mainPS(PS_INPUT input) : SV_TARGET
{
    // Output the color directly
    return input.color;
}
