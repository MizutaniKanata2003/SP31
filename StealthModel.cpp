#include "StealthModel.h"
#include "texture.h"
#include "model.h"
#include "keyboard.h"

//=============================================================================
// 初期化処理
//=============================================================================
HRESULT StealthModel::Init( void )
{
	//シェーダー読み込み
	CreateVertexShader( &m_VertexShader, &m_VertexLayout, "StealthVS.cso" );
	CreatePixelShader( &m_PixelShader, "StealthPS.cso" );

	//3Dオブジェクト管理構造体の初期化
	m_Position = XMFLOAT3( 0.0f, 0.2f, 0.0f );
	m_Rotate = XMFLOAT3( 0.0f, 0.0f, 0.0f );
	m_Scale = XMFLOAT3( 0.2f, 0.2f, 0.2f );

	//モデル読み込み
	m_Model = ModelLoad( "asset\\model\\model.fbx" );

	return S_OK;
}

//=============================================================================
// 終了処理
//=============================================================================
void StealthModel::Finalize( void )
{
	m_VertexLayout->Release();
	m_VertexShader->Release();
	m_PixelShader->Release();

	ModelRelease( m_Model );

	m_Parameter = XMFLOAT4( 0.0f, 0.0f, 0.0f, 0.0f );
}

//=============================================================================
// 更新処理
//=============================================================================
void StealthModel::Update( void )
{
	if ( Keyboard_IsKeyDown( KK_UP ) )
	{
		m_Position.z += 0.3f * ( 1.0f / 60.0f );
	}
	else if ( Keyboard_IsKeyDown( KK_DOWN ) )
	{
		m_Position.z -= 0.3f * ( 1.0f / 60.0f );
	}
	if ( Keyboard_IsKeyDown( KK_RIGHT ) )
	{
		m_Position.x += 0.3f * ( 1.0f / 60.0f );
	}
	else if ( Keyboard_IsKeyDown( KK_LEFT ) )
	{
		m_Position.x -= 0.3f * ( 1.0f / 60.0f );
	}
	if ( Keyboard_IsKeyDown( KK_Z ) )
	{
		m_Rotate.x += 60.0f * ( 1.0f / 60.0f );
	}
	else if ( Keyboard_IsKeyDown( KK_X ) )
	{
		m_Rotate.x -= 60.0f * ( 1.0f / 60.0f );
	}

	ImGui::Begin( "Stealth" );
	{
		ImGui::SliderFloat( "Refractive", &m_Parameter.x, 0.0f, 5.0f, "%.4f" );
	}
	ImGui::End();
}
//=============================================================================
// 描画処理
//=============================================================================
void StealthModel::Draw( void )
{
	SetParameter( m_Parameter );
	// 頂点レイアウト設定
	GetDeviceContext()->IASetInputLayout( m_VertexLayout );
	//頂点シェーダーをセット
	GetDeviceContext()->VSSetShader( m_VertexShader, NULL, 0 );
	//ピクセルシェーダーをセット
	GetDeviceContext()->PSSetShader( m_PixelShader, NULL, 0 );

	{//3Dポリゴン１つずつの処理
		//テクスチャをセット
		ID3D11ShaderResourceView* tex = GetPeTexture();
		GetDeviceContext()->PSSetShaderResources( 1, 1, &tex );

		//平行移動行列作成
		XMMATRIX	TranslationMatrix = XMMatrixTranslation( m_Position.x, m_Position.y, m_Position.z );
		//回転行列作成
		XMMATRIX	RotationMatrix =
			XMMatrixRotationRollPitchYaw( XMConvertToRadians( m_Rotate.x ), XMConvertToRadians( m_Rotate.y ), XMConvertToRadians( m_Rotate.z ) );
		//スケーリング行列作成
		XMMATRIX	ScalingMatrix =
			XMMatrixScaling( m_Scale.x, m_Scale.y, m_Scale.z );
		//ワールド行列作成 ※乗算の順番に注意
		XMMATRIX	WorldMatrix = ScalingMatrix * RotationMatrix * TranslationMatrix;
		//DirectXへセット
		SetWorldMatrix( WorldMatrix );

		//プリミティブトポロジーの設定
		GetDeviceContext()->IASetPrimitiveTopology( D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST );
		//マテリアル設定
		MATERIAL	material;
		ZeroMemory( &material, sizeof( MATERIAL ) );
		material.Diffuse = XMFLOAT4( 1.0f, 1.0f, 1.0f, 1.0f );
		SetMaterial( material );

		//描画
		ModelDraw( m_Model );
	}
}
