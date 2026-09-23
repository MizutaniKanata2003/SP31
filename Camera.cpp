
//Camera.cpp

#include	"Camera.h"
#include	"renderer.h"
#include	"keyboard.h"

Camera		g_Camera;	//カメラ管理構造体


void	InitCamera()
{
	//カメラの初期化
	g_Camera.m_Position = XMFLOAT3( 0.0f, 1.0f, -1.5f );	//カメラ基本座標
	g_Camera.m_UpVector = XMFLOAT3( 0.0f, 1.0f, 0.0f );		//カメラの上方ベクトル
	g_Camera.m_AtPosition = XMFLOAT3( 0.0f, 0.0f, 0.0f );	//カメラの注視点座標
	g_Camera.m_Fov = 45.0f;								//画角
	g_Camera.m_Nearclip = 0.5f;							//近面クリップ
	g_Camera.m_Farclip = 1000.0f;							//遠面クリップ
	g_Camera.m_Rotation = 0.0f;
}

void	FinalizeCamera()
{

}

void	UpdateCamera()
{

	g_Camera.m_Rotation = 0;

	if( Keyboard_IsKeyDown( KK_A ) )
	{
		g_Camera.m_Rotation = 0.3f;
	}
	else if( Keyboard_IsKeyDown( KK_D ) )
	{
		g_Camera.m_Rotation = -0.3f;
	}

	float co = cosf( XMConvertToRadians( g_Camera.m_Rotation ) );
	float si = sinf( XMConvertToRadians( g_Camera.m_Rotation ) );
	float posx = ( g_Camera.m_Position.x * co ) - ( g_Camera.m_Position.z * si );
	float posz = ( g_Camera.m_Position.x * si ) + ( g_Camera.m_Position.z * co );

	g_Camera.m_Position.x = posx;
	g_Camera.m_Position.z = posz;

}

void	DrawCamera()//3D使用時
{
	//プロジェクション行列を作成
	XMMATRIX	ProjectionMatrix =
		XMMatrixPerspectiveFovLH(
			XMConvertToRadians( g_Camera.m_Fov ),
			( float ) SCREEN_WIDTH / ( float ) SCREEN_HEIGHT,
			g_Camera.m_Nearclip,
			g_Camera.m_Farclip
		);
	//プロジェクション行列をセット
	SetProjectionMatrix( ProjectionMatrix );

	//カメラ行列を作成
	XMVECTOR	eyev = XMLoadFloat3( &g_Camera.m_AtPosition );
	XMVECTOR	pos = XMLoadFloat3( &g_Camera.m_Position );
	XMVECTOR	up = XMLoadFloat3( &g_Camera.m_UpVector );
	XMMATRIX	ViewMatrix =
		XMMatrixLookAtLH( pos, eyev, up );

	//カメラ行列をセット
	SetViewMatrix( ViewMatrix );
	//カメラ座標セット
	SetCameraPosition( g_Camera.m_Position );
}




