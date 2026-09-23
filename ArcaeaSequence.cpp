#include "ArcaeaSequence.h"
#include "sprite.h"
#include "Camera.h"
#include "texture.h"
#include <cmath>

HRESULT ArcaeaSequence::Init( void )
{
	CreateVertexShader( &m_VertexShader, &m_VertexLayout, "PosterizeVS.cso" );
	CreatePixelShader( &m_PixelShader, "ArcaeaPS.cso" );

	m_TexFloorID = TextureLoad( L"asset\\texture\\texture_temple_floor.png" );
	m_TexMagicCircleID = TextureLoad( L"asset\\texture\\texture_magic_circle.png" );
	m_TexSwordID = TextureLoad( L"asset\\texture\\texture_sacred_sword.png" );
	m_TexShardsID = TextureLoad( L"asset\\texture\\texture_glass_shards.png" );
	m_TexPillarsID = TextureLoad( L"asset\\texture\\texture_temple_pillars.png" );
	m_TexGlareID = TextureLoad( L"asset\\texture\\texture_radial_glare_mask.png" );
	m_TexSpectrumID = TextureLoad( L"asset\\texture\\texture_prism_spectrum.png" );

	m_Timer = 0.0f;
	m_FloorRot = 0.0f;
	m_CircleRot = 0.0f;
	m_SwordConvergence = 0.0f;
	m_LightPulse = 1.0f;

	for ( int i = 0; i < SHARD_COUNT; ++i )
	{
		float angle = (float)i * ( 6.2831853f / SHARD_COUNT );
		float dist = 40.0f + (float)( rand() % 200 );
		m_Shards[ i ].pos = XMFLOAT3( (float)( SCREEN_WIDTH / 2 ) + cosf( angle ) * dist, (float)( SCREEN_HEIGHT / 2 ) + sinf( angle ) * dist, 0.0f );
		m_Shards[ i ].vel = XMFLOAT3( cosf( angle ) * ( 2.5f + ( rand() % 4 ) ), sinf( angle ) * ( 2.5f + ( rand() % 4 ) ), 0.0f );
		m_Shards[ i ].size = XMFLOAT2( 90.0f, 90.0f );
		m_Shards[ i ].rotate = (float)( rand() % 360 );
		m_Shards[ i ].rotVel = (float)( ( rand() % 100 ) - 50 ) * 0.1f;
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

	m_FloorRot += 0.35f;
	m_CircleRot -= 0.75f;

	m_LightPulse = 0.9f + sinf( m_Timer * 6.0f ) * 0.1f;

	for ( int i = 0; i < SHARD_COUNT; ++i )
	{
		m_Shards[ i ].pos.x += m_Shards[ i ].vel.x;
		m_Shards[ i ].pos.y += m_Shards[ i ].vel.y;
		m_Shards[ i ].rotate += m_Shards[ i ].rotVel;
		m_Shards[ i ].size.x += 1.2f;
		m_Shards[ i ].size.y += 1.2f;

		if ( m_Shards[ i ].pos.x < -200.0f || m_Shards[ i ].pos.x > SCREEN_WIDTH + 200.0f ||
			 m_Shards[ i ].pos.y < -200.0f || m_Shards[ i ].pos.y > SCREEN_HEIGHT + 200.0f )
		{
			float angle = (float)( rand() % 360 ) * 0.01745329f;
			float dist = 20.0f + (float)( rand() % 80 );
			m_Shards[ i ].pos = XMFLOAT3( (float)( SCREEN_WIDTH / 2 ) + cosf( angle ) * dist, (float)( SCREEN_HEIGHT / 2 ) + sinf( angle ) * dist, 0.0f );
			m_Shards[ i ].size = XMFLOAT2( 80.0f, 80.0f );
		}
	}

	if ( m_Timer >= 4.0f && m_Timer < 14.0f ) { m_SwordConvergence += 0.012f; }
	else if ( m_Timer >= 14.0f ) { m_SwordConvergence += 0.025f; }
	else { m_SwordConvergence = 0.0f; }
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
	GetDeviceContext()->IASetInputLayout( m_VertexLayout );
	GetDeviceContext()->VSSetShader( m_VertexShader, NULL, 0 );
	GetDeviceContext()->PSSetShader( m_PixelShader, NULL, 0 );
	SetDepthEnable( false );

	MATERIAL material;
	ZeroMemory( &material, sizeof( material ) );
	material.Diffuse = XMFLOAT4( 1.0f, 1.0f, 1.0f, 1.0f );
	SetMaterial( material );

	XMFLOAT3 center = XMFLOAT3( (float)( SCREEN_WIDTH / 2 ), (float)( SCREEN_HEIGHT / 2 ), 0.0f );

	// 1. 神殿背景クロスフェード
	float pillarAlpha = 0.0f;
	if ( m_Timer >= 6.0f && m_Timer < 8.0f ) { pillarAlpha = ( m_Timer - 6.0f ) / 2.0f; }
	else if ( m_Timer >= 8.0f && m_Timer < 14.0f ) { pillarAlpha = 1.0f; }
	else if ( m_Timer >= 14.0f && m_Timer < 16.0f ) { pillarAlpha = 1.0f - ( ( m_Timer - 14.0f ) / 2.0f ); }
	float floorAlpha = 1.0f - pillarAlpha;

	// 神殿床面（円形マスクON: Parameter.x = 1.0f でフチをぼかす）
	SetParameter( XMFLOAT4( 1.0f, 0.0f, 0.0f, 0.0f ) );
	if ( floorAlpha > 0.01f )
	{
		float floorZoom = 1.0f + ( m_Timer * 0.015f );
		XMFLOAT4 floorCol = XMFLOAT4( m_LightPulse, m_LightPulse, m_LightPulse, floorAlpha );
		DrawQuad( m_TexFloorID, center, XMFLOAT2( (float)SCREEN_HEIGHT, (float)SCREEN_HEIGHT ), XMFLOAT2( floorZoom, floorZoom ), m_FloorRot, floorCol );
	}

	// 神殿列柱背景（全画面: Parameter.x = 0.0f）
	SetParameter( XMFLOAT4( 0.0f, 0.0f, 0.0f, 0.0f ) );
	if ( pillarAlpha > 0.01f )
	{
		XMFLOAT4 pillarCol = XMFLOAT4( m_LightPulse, m_LightPulse, m_LightPulse, pillarAlpha );
		DrawQuad( m_TexPillarsID, center, XMFLOAT2( (float)SCREEN_WIDTH, (float)SCREEN_HEIGHT ), XMFLOAT2( 1.05f, 1.05f ), 0.0f, pillarCol );
	}

	// 2. 魔法陣リング（円形マスクON）
	SetParameter( XMFLOAT4( 1.0f, 0.0f, 0.0f, 0.0f ) );
	float circleScale = 0.9f + sinf( m_Timer * 2.0f ) * 0.04f;
	DrawQuad( m_TexMagicCircleID, center, XMFLOAT2( (float)SCREEN_HEIGHT * 0.95f, (float)SCREEN_HEIGHT * 0.95f ), XMFLOAT2( circleScale, circleScale ), m_CircleRot, XMFLOAT4( 1.0f, 0.98f, 0.9f, 0.85f ) );

	// 3. 2重交差の螺旋剣（通常マスク: Parameter.x = 0.0f）
	SetParameter( XMFLOAT4( 0.0f, 0.0f, 0.0f, 0.0f ) );
	float swordAlpha = 0.0f;
	if ( m_Timer >= 3.5f && m_Timer < 5.0f ) { swordAlpha = ( m_Timer - 3.5f ) / 1.5f; }
	else if ( m_Timer >= 5.0f && m_Timer < 17.0f ) { swordAlpha = 1.0f; }
	else if ( m_Timer >= 17.0f ) { swordAlpha = 1.0f - ( ( m_Timer - 17.0f ) / 3.0f ); }

	if ( swordAlpha > 0.01f )
	{
		for ( int i = 0; i < 16; ++i )
		{
			float angle = (float)i * ( 6.2831853f / 16 ) + XMConvertToRadians( m_FloorRot * 1.5f );
			float r = 580.0f - ( m_SwordConvergence * 22.0f );
			if ( r < 90.0f ) r = 90.0f;
			float x = center.x + cosf( angle ) * r;
			float y = center.y + sinf( angle ) * r;
			DrawQuad( m_TexSwordID, XMFLOAT3( x, y, 0.0f ), XMFLOAT2( 70.0f, 280.0f ), XMFLOAT2( 0.6f, 0.6f ), XMConvertToDegrees( angle ) + 90.0f, XMFLOAT4( 1.0f, 1.0f, 1.0f, swordAlpha ) );
		}
		for ( int i = 0; i < 16; ++i )
		{
			float angle = (float)i * ( 6.2831853f / 16 ) - XMConvertToRadians( m_FloorRot * 2.2f );
			float r = 420.0f - ( m_SwordConvergence * 18.0f );
			if ( r < 50.0f ) r = 50.0f;
			float x = center.x + cosf( angle ) * r;
			float y = center.y + sinf( angle ) * r;
			DrawQuad( m_TexSwordID, XMFLOAT3( x, y, 0.0f ), XMFLOAT2( 55.0f, 220.0f ), XMFLOAT2( 0.5f, 0.5f ), XMConvertToDegrees( angle ) + 90.0f, XMFLOAT4( 0.95f, 0.95f, 1.0f, swordAlpha * 0.85f ) );
		}
	}

	// 4. ガラス破片
	for ( int i = 0; i < SHARD_COUNT; ++i )
	{
		DrawQuad( m_TexShardsID, m_Shards[ i ].pos, m_Shards[ i ].size, XMFLOAT2( 1.0f, 1.0f ), m_Shards[ i ].rotate, XMFLOAT4( 1.0f, 1.0f, 1.0f, 0.85f ) );
	}

	// 5. 放射グレア＆スペクトル光条
	float glareScale = 1.25f + sinf( m_Timer * 5.0f ) * 0.12f;
	DrawQuad( m_TexGlareID, center, XMFLOAT2( (float)SCREEN_WIDTH, (float)SCREEN_HEIGHT ), XMFLOAT2( glareScale, glareScale ), -m_FloorRot * 1.8f, XMFLOAT4( 1.0f, 0.98f, 0.95f, 0.7f ) );

	if ( m_Timer >= 2.0f && m_Timer <= 11.0f )
	{
		DrawQuad( m_TexSpectrumID, center, XMFLOAT2( (float)SCREEN_WIDTH, (float)SCREEN_HEIGHT ), XMFLOAT2( 1.0f, 1.0f ), 0.0f, XMFLOAT4( 1.0f, 1.0f, 1.0f, 0.85f ) );
	}
}