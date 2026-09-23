#pragma once

#include "main.h"
#include "renderer.h"

class ArcaeaSequence
{
private:
	static const int SWORD_COUNT = 32;
	static const int SHARD_COUNT = 24;

	struct ShardData
	{
		XMFLOAT3 pos;
		XMFLOAT3 vel;
		XMFLOAT2 size;
		float rotate;
		float rotVel;
	};

	int m_TexFloorID;
	int m_TexSwordID;
	int m_TexShardsID;
	int m_TexPillarsID;
	int m_TexGlareID;
	int m_TexSpectrumID;

	ID3D11VertexShader* m_VertexShader;
	ID3D11PixelShader* m_PixelShader;
	ID3D11InputLayout* m_VertexLayout;

	ShardData m_Shards[ SHARD_COUNT ];
	float m_Timer;
	float m_FloorRot;
	float m_SwordConvergence;

	void DrawQuad( int texID, XMFLOAT3 pos, XMFLOAT2 size, XMFLOAT2 scale, float rot, XMFLOAT4 color );

public:
	HRESULT Init( void );
	void Finalize( void );
	void Update( void );
	void Draw( void );
};