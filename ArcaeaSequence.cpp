#include "ArcaeaSequence.h"
#include "sprite.h"
#include "Camera.h"
#include "texture.h"
#include <cmath>

HRESULT ArcaeaSequence::Init( void )
{
	CreateVertexShader( &m_VertexShader, &m_VertexLayout, "PosterizeVS.cso" );
	CreatePixelShader( &m_PixelShader, "PosterizePS.cso" );

	m_TexFloorID = TextureLoad( L"asset\\texture\\texture_temple_floor.png" );
	m_TexSwordID = TextureLoad( L"asset\\texture\\texture_sacred_sword.png" );
	m_TexShardsID = TextureLoad( L"asset\\texture\\texture_glass_shards.png" );
	m_TexPillarsID = TextureLoad( L"asset\\texture\\texture_temple_pillars.png" );
	m_TexGlareID = TextureLoad( L"asset\\texture\\texture_radial_glare_mask.png" );
	m_TexSpectrumID = TextureLoad( L"asset\\texture\\texture_prism_spectrum.png" );

	m_Timer = 0.0f;
	m_FloorRot = 0.0f;
	m_SwordConvergence = 0.0f;

	for ( int i = 0; i < SHARD_COUNT; ++i )
	{
		float angle = (float)i * ( 6.2831853f / SHARD_COUNT );
		float radius = 150.0f + (float)( rand() % 350 );
		m_Shards[ i ].pos = XMFLOAT3( (float)( SCREEN_WIDTH / 2 ) + cosf( angle ) * radius, (float)( SCREEN_HEIGHT / 2 ) + sinf( angle ) * radius, 0.0f );
		m_Shards[ i ].vel = XMFLOAT3( cosf( angle ) * ( 1.5f + ( rand() % 3 ) ), sinf( angle ) * ( 1.5f + ( rand() % 3 ) ), 0.0f );
		m_Shards[ i ].size = XMFLOAT2( 200.0f, 200.0f );
		m_Shards[ i ].rotate = (float)( rand() % 360 );
		m_Shards[ i ].rotVel = (float)( ( rand() % 100 ) - 50 ) * 0.05f;
	}
	return S_OK;
}

void ArcaeaSequence::Finalize( void )
{
	if ( m_VertexLayout ) { m_VertexLayout->Release(); m_VertexLayout = nullptr; }
	if ( m_VertexShader ) { m_VertexShader->Release(); m_VertexShader = nullptr; }
	if ( m_PixelShader ) { m_PixelShader->Release(); m_PixelShader = nullptr; }
}

void ArcaeaSequence::Update( void )
{
	m_Timer += 1.0f / 60.0f;
	if ( m_Timer > 20.0f ) { m_Timer = 0.0f; }

	m_FloorRot += 0.3f;

	for ( int i = 0; i < SHARD_COUNT; ++i )
	{
		m_Shards[ i ].pos.x += m_Shards[ i ].vel.x;
		m_Shards[ i ].pos.y += m_Shards[ i ].vel.y;
		m_Shards[ i ].rotate += m_Shards[ i ].rotVel;
	}

	if ( m_Timer >= 5.0f && m_Timer < 14.0f )
	{
		m_SwordConvergence += 0.008f;
	}
	else if ( m_Timer >= 14.0f )
	{
		m_SwordConvergence += 0.02f;
	}
	else
	{
		m_SwordConvergence = 0.0f;
	}
}

void ArcaeaSequence::DrawQuad( int texID, XMFLOAT3 pos, XMFLOAT2 size, XMFLOAT2 scale, float rot, XMFLOAT4 color )
{
	ID3D11ShaderResourceView* tex = GetTexture( texID );
	if ( !tex ) return;

	GetDeviceContext()->PSSetShaderResources( 0, 1, &tex );

	XMMATRIX TranslationMatrix = XMMatrixTranslation( pos.x, pos.y, 0.0f );
	XMMATRIX RotationMatrix = XMMatrixRotationZ( XMConvertToRadians( rot ) );
	XMMATRIX ScalingMatrix = XMMatrixScaling( scale.x, scale.y, 1.0f );
	XMMATRIX WorldMatrix = ScalingMatrix * RotationMatrix * TranslationMatrix;

	SetWorldMatrix( WorldMatrix );
	GetDeviceContext()->GenerateMips( tex );
	DrawSprite( size, color );
}

void ArcaeaSequence::Draw( void )
{
	XMFLOAT4 defaultParam = XMFLOAT4( 64.0f, 0.0f, 1.0f, 1.0f );
	SetParameter( defaultParam );

	GetDeviceContext()->IASetInputLayout( m_VertexLayout );
	GetDeviceContext()->VSSetShader( m_VertexShader, NULL, 0 );
	GetDeviceContext()->PSSetShader( m_PixelShader, NULL, 0 );
	SetDepthEnable( false );

	MATERIAL material;
	ZeroMemory( &material, sizeof( material ) );
	material.Diffuse = XMFLOAT4( 1.0f, 1.0f, 1.0f, 1.0f );
	SetMaterial( material );

	XMFLOAT3 center = XMFLOAT3( (float)( SCREEN_WIDTH / 2 ), (float)( SCREEN_HEIGHT / 2 ), 0.0f );

	// 1. 神殿背景・見下ろし円盤
	if ( m_Timer < 8.0f || m_Timer >= 16.0f )
	{
		float floorScale = 1.0f + sinf( m_Timer * 0.5f ) * 0.05f;
		DrawQuad( m_TexFloorID, center, XMFLOAT2( (float)SCREEN_HEIGHT, (float)SCREEN_HEIGHT ), XMFLOAT2( floorScale, floorScale ), m_FloorRot, XMFLOAT4( 1.0f, 1.0f, 1.0f, 1.0f ) );
	}
	else
	{
		DrawQuad( m_TexPillarsID, center, XMFLOAT2( (float)SCREEN_WIDTH, (float)SCREEN_HEIGHT ), XMFLOAT2( 1.0f, 1.0f ), 0.0f, XMFLOAT4( 1.0f, 1.0f, 1.0f, 1.0f ) );
	}

	// 2. 螺旋長剣
	if ( m_Timer >= 4.0f )
	{
		for ( int i = 0; i < SWORD_COUNT; ++i )
		{
			float step = (float)i * ( 6.2831853f / SWORD_COUNT );
			float currentAngle = step + XMConvertToRadians( m_FloorRot * 2.0f );
			float radius = 520.0f - ( m_SwordConvergence * 20.0f ) + sinf( m_Timer * 2.0f + i ) * 25.0f;
			if ( radius < 60.0f ) radius = 60.0f;

			float x = center.x + cosf( currentAngle ) * radius;
			float y = center.y + sinf( currentAngle ) * radius;

			DrawQuad( m_TexSwordID, XMFLOAT3( x, y, 0.0f ), XMFLOAT2( 80.0f, 320.0f ), XMFLOAT2( 0.6f, 0.6f ), XMConvertToDegrees( currentAngle ) + 90.0f, XMFLOAT4( 1.0f, 1.0f, 1.0f, 1.0f ) );
		}
	}

	// 3. ガラス破片
	for ( int i = 0; i < SHARD_COUNT; ++i )
	{
		DrawQuad( m_TexShardsID, m_Shards[ i ].pos, m_Shards[ i ].size, XMFLOAT2( 1.0f, 1.0f ), m_Shards[ i ].rotate, XMFLOAT4( 1.0f, 1.0f, 1.0f, 0.85f ) );
	}

	// 4. 放射グレア
	float glareScale = 1.2f + sinf( m_Timer * 4.0f ) * 0.1f;
	DrawQuad( m_TexGlareID, center, XMFLOAT2( (float)SCREEN_WIDTH, (float)SCREEN_HEIGHT ), XMFLOAT2( glareScale, glareScale ), -m_FloorRot * 1.5f, XMFLOAT4( 1.0f, 1.0f, 1.0f, 0.7f ) );

	// 5. スペクトル光条
	if ( m_Timer >= 2.0f && m_Timer <= 10.0f )
	{
		DrawQuad( m_TexSpectrumID, center, XMFLOAT2( (float)SCREEN_WIDTH, (float)SCREEN_HEIGHT ), XMFLOAT2( 1.0f, 1.0f ), 0.0f, XMFLOAT4( 1.0f, 1.0f, 1.0f, 0.9f ) );
	}
}