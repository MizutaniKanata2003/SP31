// マイクロファセットの分布関数
float CalculateBeckmann(float smooth, float nh)
{
    if (nh <= 0.0f)
        return 0.0f; // ゼロディバイド対策
    if (smooth <= 0.0f)
        return 0.0f; // ゼロディバイト対策
    
    float smo2 = smooth * smooth;
    float nh2 = nh * nh;
    float nh4 = nh2 * nh2;
    
    // 指数部分の計算
    float ex = -(1.0f - nh2) / (smo2 * nh2);
    
    // 分母の計算
    float PI = 3.14159265f;
    float bun = PI * smo2 * nh4;
    
    return exp(ex) / bun;
}

//　フレネル項
float CalculateFresnel(float metal, float hv)
{
 // Schlick近似
    metal = lerp(0.04f, 1.0f, metal); // metal = 0.04f～1.0f
    return metal + (1.0f - metal) * pow(1.0f - hv, 5.0f);
}

// 幾何減数率
float CalculateGeometricDamping(float nh, float nv, float n1, float vh)
{
    float G;
    vh = max(vh, 0.0001f);
    G = min(1.0f, min(2.0f * nh * nv / vh, 2.0f * nh * n1 / vh));
    
    return G;
}