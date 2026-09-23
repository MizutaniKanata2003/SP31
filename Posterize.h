#pragma once

#include "main.h"
#include "renderer.h"

class Posterize
{
protected:
	XMFLOAT3	m_Position;		//表示座標
	XMFLOAT4	m_Color;			//色
	XMFLOAT4	m_Parameter;
	XMFLOAT2	m_Size;			//サイズ
	XMFLOAT2	m_Scale;			//拡大縮小率
	float		m_Rotate;			//回転角度（360表現）
	int			m_TexID;			//テクスチャID
	ID3D11VertexShader* m_VertexShader; //頂点シェーダーオブジェクト
	ID3D11PixelShader* m_PixelShader; //ピクセルシェーダーオブジェクト
	ID3D11InputLayout* m_VertexLayout; //頂点レイアウトオブジェクト

public:
	HRESULT Init( void );
	void Finalize( void );
	void Update( void );
	void Draw( void );
};

