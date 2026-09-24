#pragma once

#include "main.h"
#include "renderer.h"
#include "model.h"

class StealthModel
{
protected:
	XMFLOAT3	m_Position;
	XMFLOAT3	m_Scale;
	XMFLOAT3	m_Rotate;
	XMFLOAT4	m_Parameter{};
	int			m_TexID;

	ID3D11VertexShader* m_VertexShader; //頂点シェーダーオブジェクト
	ID3D11PixelShader* m_PixelShader; //ピクセルシェーダーオブジェクト
	ID3D11InputLayout* m_VertexLayout; //頂点レイアウトオブジェクト

	MODEL* m_Model;
public:
	HRESULT Init( void );
	void Finalize( void );
	void Update( void );
	void Draw( void );
};