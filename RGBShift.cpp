#include "RGBShift.h"
#include "sprite.h"
#include "Camera.h"
#include "texture.h"

//=============================================================================
// ‰Šú‰»ˆ—
//=============================================================================
HRESULT RGBShift::Init( void )
{
	CreateVertexShader( &m_VertexShader, &m_VertexLayout, "RGBShiftVS.cso" );
	CreatePixelShader( &m_PixelShader, "RGBShiftPS.cso" );

	m_Position = XMFLOAT3( SCREEN_WIDTH / 2, SCREEN_HEIGHT / 2, 0.0f );
	m_Color = XMFLOAT4( 1.0f, 1.0f, 1.0f, 1.0f );
	m_Scale = XMFLOAT2( 1.0f, 1.0f );
	m_Size = XMFLOAT2( SCREEN_WIDTH, SCREEN_HEIGHT );
	m_Rotate = 0.0f;
	m_Parameter = XMFLOAT4( 0.0f, 0.0f, 0.0f, 0.0f );

	return S_OK;
}

//=============================================================================
// I—¹ˆ—
//=============================================================================
void RGBShift::Finalize( void )
{
	m_VertexLayout->Release();
	m_VertexShader->Release();
	m_PixelShader->Release();
}

//=============================================================================
// XVˆ—
//=============================================================================
void RGBShift::Update( void )
{
	ImGui::Begin( "RGBShift" );
	{
		ImGui::SliderFloat( "XShift", &m_Parameter.x, 0.0f, 0.5f, "%.4f" );
		ImGui::SliderFloat( "YShift", &m_Parameter.y, 0.0f, 0.5f, "%.4f" );
	}
	ImGui::End();
}

//=============================================================================
// •`‰æˆ—
//=============================================================================
void RGBShift::Draw( void )
{
	SetParameter( m_Parameter );
	GetDeviceContext()->IASetInputLayout( m_VertexLayout );
	GetDeviceContext()->VSSetShader( m_VertexShader, NULL, 0 );
	GetDeviceContext()->PSSetShader( m_PixelShader, NULL, 0 );
	SetDepthEnable( false );

	MATERIAL material;
	ZeroMemory( &material, sizeof( material ) );
	material.Diffuse = XMFLOAT4( 1.0f, 1.0f, 1.0f, 1.0f );
	SetMaterial( material );

	{
		ID3D11ShaderResourceView* tex = GetPeTexture();
		GetDeviceContext()->PSSetShaderResources( 0, 1, &tex );

		XMMATRIX TranslationMatrix = XMMatrixTranslation( m_Position.x, m_Position.y, 0.0f );
		XMMATRIX RotationMatrix = XMMatrixRotationZ( XMConvertToRadians( m_Rotate ) );
		XMMATRIX ScalingMatrix = XMMatrixScaling( m_Scale.x, m_Scale.y, 1.0f );
		XMMATRIX WorldMatrix = ScalingMatrix * RotationMatrix * TranslationMatrix;

		SetWorldMatrix( WorldMatrix );
		GetDeviceContext()->GenerateMips( tex );
		DrawSprite( m_Size, m_Color );
	}
}