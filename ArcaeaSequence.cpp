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

	m_FloorRot += 0.25f;
	m_CircleRot -= 0.6f;

	m_LightPulse = 0.85f + sinf( m_Timer * 6.0f ) * 0.15f;

	for ( int i = 0; i < SHARD_COUNT; ++i )
	{
		m_Shards[ i ].pos.x += m_Shards[ i ].vel.x;
		m_Shards[ i ].pos.y += m_Shards[ i ].vel.y;
		m_Shards[ i ].rotate += m_Shards[ i ].rotVel;
	}

	if ( m_Timer >= 4.0f && m_Timer < 14.0f )
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

	// 1. 背景のアルファクロスフェード補間（神殿見下ろし円盤 ⇔ 列柱背景）
	float pillarAlpha = 0.0f;
	if ( m_Timer >= 6.0f && m_Timer < 8.0f )
	{
		pillarAlpha = ( m_Timer - 6.0f ) / 2.0f;
	}
	else if ( m_Timer >= 8.0f && m_Timer < 14.0f )
	{
		pillarAlpha = 1.0f;
	}
	else if ( m_Timer >= 14.0f && m_Timer < 16.0f )
	{
		pillarAlpha = 1.0f - ( ( m_Timer - 14.0f ) / 2.0f );
	}
	float floorAlpha = 1.0f - pillarAlpha;

	// 神殿床面（円盤）
	if ( floorAlpha > 0.01f )
	{
		float floorScale = 1.0f + sinf( m_Timer * 0.5f ) * 0.05f;
		XMFLOAT4 floorColor = XMFLOAT4( m_LightPulse, m_LightPulse, m_LightPulse, floorAlpha );
		DrawQuad( m_TexFloorID, center, XMFLOAT2( (float)SCREEN_HEIGHT, (float)SCREEN_HEIGHT ), XMFLOAT2( floorScale, floorScale ), m_FloorRot, floorColor );
	}

	// 神殿列柱背景
	if ( pillarAlpha > 0.01f )
	{
		XMFLOAT4 pillarColor = XMFLOAT4( m_LightPulse, m_LightPulse, m_LightPulse, pillarAlpha );
		DrawQuad( m_TexPillarsID, center, XMFLOAT2( (float)SCREEN_WIDTH, (float)SCREEN_HEIGHT ), XMFLOAT2( 1.05f, 1.05f ), 0.0f, pillarColor );
	}

	// 2. 魔法陣テクスチャの多層回転（時計台・魔法陣ギミック）
	float circleAlpha = 0.75f * m_LightPulse;
	float circleScale = 0.85f + sinf( m_Timer * 1.5f ) * 0.05f;
	DrawQuad( m_TexMagicCircleID, center, XMFLOAT2( (float)SCREEN_HEIGHT * 0.9f, (float)SCREEN_HEIGHT * 0.9f ), XMFLOAT2( circleScale, circleScale ), m_CircleRot, XMFLOAT4( 1.0f, 0.95f, 0.8f, circleAlpha ) );

	// 3. 螺旋状に整列・落下する長剣（フェードイン・ライティング）
	float swordAlpha = 0.0f;
	if ( m_Timer >= 3.5f && m_Timer < 5.0f ) { swordAlpha = ( m_Timer - 3.5f ) / 1.5f; }
	else if ( m_Timer >= 5.0f && m_Timer < 17.0f ) { swordAlpha = 1.0f; }
	else if ( m_Timer >= 17.0f ) { swordAlpha = 1.0f - ( ( m_Timer - 17.0f ) / 3.0f ); }

	if ( swordAlpha > 0.01f )
	{
		for ( int i = 0; i < SWORD_COUNT; ++i )
		{
			float step = (float)i * ( 6.2831853f / SWORD_COUNT );
			float currentAngle = step + XMConvertToRadians( m_FloorRot * 2.0f );
			float radius = 520.0f - ( m_SwordConvergence * 20.0f ) + sinf( m_Timer * 2.0f + i ) * 25.0f;
			if ( radius < 60.0f ) radius = 60.0f;

			float x = center.x + cosf( currentAngle ) * radius;
			float y = center.y + sinf( currentAngle ) * radius;

			float swordLight = 0.9f + sinf( m_Timer * 8.0f + i * 0.5f ) * 0.2f;
			XMFLOAT4 swordColor = XMFLOAT4( swordLight, swordLight, swordLight, swordAlpha );
			DrawQuad( m_TexSwordID, XMFLOAT3( x, y, 0.0f ), XMFLOAT2( 80.0f, 320.0f ), XMFLOAT2( 0.6f, 0.6f ), XMConvertToDegrees( currentAngle ) + 90.0f, swordColor );
		}
	}

	// 4. 浮遊するガラス破片
	for ( int i = 0; i < SHARD_COUNT; ++i )
	{
		float shardShine = 0.8f + sinf( m_Timer * 4.0f + i ) * 0.2f;
		DrawQuad( m_TexShardsID, m_Shards[ i ].pos, m_Shards[ i ].size, XMFLOAT2( 1.0f, 1.0f ), m_Shards[ i ].rotate, XMFLOAT4( shardShine, shardShine, 1.0f, 0.85f ) );
	}

	// 5. 放射グレア（中心光条・明滅ライティング）
	float glareScale = 1.2f + sinf( m_Timer * 4.0f ) * 0.1f;
	float glareAlpha = 0.5f + ( m_LightPulse - 0.85f ) * 2.0f;
	DrawQuad( m_TexGlareID, center, XMFLOAT2( (float)SCREEN_WIDTH, (float)SCREEN_HEIGHT ), XMFLOAT2( glareScale, glareScale ), -m_FloorRot * 1.5f, XMFLOAT4( 1.0f, 0.98f, 0.9f, glareAlpha ) );

	// 6. スペクトル光条（ピーク時の虹色フレア）
	float specAlpha = 0.0f;
	if ( m_Timer >= 2.0f && m_Timer < 4.0f ) { specAlpha = ( m_Timer - 2.0f ) / 2.0f; }
	else if ( m_Timer >= 4.0f && m_Timer < 8.0f ) { specAlpha = 0.85f; }
	else if ( m_Timer >= 8.0f && m_Timer <= 11.0f ) { specAlpha = 0.85f * ( 1.0f - ( ( m_Timer - 8.0f ) / 3.0f ) ); }

	if ( specAlpha > 0.01f )
	{
		DrawQuad( m_TexSpectrumID, center, XMFLOAT2( (float)SCREEN_WIDTH, (float)SCREEN_HEIGHT ), XMFLOAT2( 1.0f, 1.0f ), 0.0f, XMFLOAT4( 1.0f, 1.0f, 1.0f, specAlpha ) );
	}
}