#pragma once

#include "main.h"
#include "renderer.h"
#include "model.h"

class DirectionalLighting
{
protected:
	XMFLOAT3	m_Position;
	XMFLOAT3	m_Scale;
	XMFLOAT3	m_Rotate;
	int			m_TexID;

	ID3D11VertexShader* m_VertexShader; //頂点シェーダーオブジェクト
	ID3D11PixelShader* m_PixelShader; //ピクセルシェーダーオブジェクト
	ID3D11InputLayout* m_VertexLayout; //頂点レイアウトオブジェクト

	LIGHT m_Light;

	MODEL* m_Model;

public:
	HRESULT Init( void );
	void Finalize( void );
	void Update( void );
	void Draw( void );
};







//*****************************************************************************
// プロトタイプ宣言
//*****************************************************************************

