#pragma once

#include "main.h"
#include "renderer.h"

class Field3D
{
protected:
	XMFLOAT3	Position;
	XMFLOAT3	Scale;
	XMFLOAT3	Rotate;
	int			TexID;

	ID3D11Buffer* VertexBuffer;	//頂点バッファ
	ID3D11Buffer* IndexBuffer;	//インデックスバッファ
	ID3D11VertexShader* VertexShader; //頂点シェーダーオブジェクト
	ID3D11PixelShader* PixelShader; //ピクセルシェーダーオブジェクト
	ID3D11InputLayout* VertexLayout; //頂点レイアウトオブジェクト

public:
	HRESULT Init( void );
	void Finalize( void );
	void Update( void );
	void Draw( void );
};
