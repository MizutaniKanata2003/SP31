#include "Posterize.h"
#include "sprite.h"
#include "Camera.h"
#include "texture.h"

//=============================================================================
// 初期化処理
//=============================================================================
HRESULT Posterize::Init( void )
{
	//シェーダー読み込み
	CreateVertexShader( &m_VertexShader, &m_VertexLayout, "PosterizeVS.cso" );
	CreatePixelShader( &m_PixelShader, "PosterizePS.cso" );

	m_TexID = TextureLoad( L"asset\\texture\\test.png" );

	//2Dオブジェクト初期化
	m_Position = XMFLOAT3( SCREEN_WIDTH / 2, SCREEN_HEIGHT / 2, 0.0f );
	m_Color = XMFLOAT4( 1.0f, 1.0f, 1.0f, 1.0f );
	m_Scale = XMFLOAT2( 1.0f, 1.0f );
	m_Size = XMFLOAT2( SCREEN_WIDTH, SCREEN_HEIGHT );
	m_Rotate = 0.0f;

	m_Parameter = XMFLOAT4( 64.0f, 0.2f, 0.7f, 1.0f );
	return S_OK;
}

//=============================================================================
// 終了処理
//=============================================================================
void Posterize::Finalize( void )
{
	m_VertexLayout->Release();
	m_VertexShader->Release();
	m_PixelShader->Release();

}

//=============================================================================
// 更新処理
//=============================================================================
void Posterize::Update( void )
{
	ImGui::Begin( "Posterize" );
	{
		ImGui::SliderFloat( "Gradation", &m_Parameter.x, 1.0f, 64.0f, " %.0f" );
		ImGui::SliderFloat( "MIN", &m_Parameter.y, 0.0f, 1.0f, " %.4f" );
		ImGui::SliderFloat( "MAX", &m_Parameter.z, 0.0f, 1.0f, " %.4f" );
		ImGui::SliderFloat( "POW", &m_Parameter.w, 1.0f, 30.0f, " %.0f" );

	}
	ImGui::End();
}

//=============================================================================
// 描画処理
//=============================================================================
void Posterize::Draw( void )
{
	SetParameter( m_Parameter );
	// 頂点レイアウト設定
	GetDeviceContext()->IASetInputLayout( m_VertexLayout );
	//頂点シェーダーをセット
	GetDeviceContext()->VSSetShader( m_VertexShader, NULL, 0 );
	//ピクセルシェーダーをセット
	GetDeviceContext()->PSSetShader( m_PixelShader, NULL, 0 );

	//奥行き処理をOFF
	SetDepthEnable( false );

	// マテリアル設定
	MATERIAL material;
	ZeroMemory( &material, sizeof( material ) );
	material.Diffuse = XMFLOAT4( 1.0f, 1.0f, 1.0f, 1.0f );
	SetMaterial( material );

	{//2Dポリゴン1枚ずつで必要な処理

		//テクスチャをセット
		ID3D11ShaderResourceView* tex = GetTexture( m_TexID );
		GetDeviceContext()->PSSetShaderResources( 0, 1, &tex );

		//平行移動行列の作成（表示座標を決める）
		XMMATRIX	TranslationMatrix = XMMatrixTranslation(
			m_Position.x, m_Position.y, 0.0f );

		//回転行列（Z回転）行列の作成
		XMMATRIX	RotationMatrix = XMMatrixRotationZ( XMConvertToRadians( m_Rotate ) );

		//スケーリング行列作成（倍率1.0が等倍、0倍はダメ！）
		XMMATRIX	ScalingMatrix = XMMatrixScaling( m_Scale.x, m_Scale.y, 1.0f );

		//ワールド行列の作成（ポリゴンの表示の仕方を指定する最終的な行列
		XMMATRIX	WorldMatrix = ScalingMatrix * RotationMatrix * TranslationMatrix;

		//ワールド行列をDirectXへセット
		SetWorldMatrix( WorldMatrix );
		GetDeviceContext()->GenerateMips( tex );
		// ポリゴン描画
		DrawSprite( m_Size, m_Color );
	}
}
