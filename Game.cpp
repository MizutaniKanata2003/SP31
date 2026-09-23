#include "Renderer.h"
#include "Game.h"
#include "Camera.h"
#include "texture.h"
#include "Sprite2D.h"
#include "Field3D.h"
//#include "PolygonModel.h"
//#include "DirectionalLighting.h"
//#include "PixelDirectionalLighting.h"
//#include "PixelLightingBlinnPhong.h"
//#include "HemiSphereLighting.h"
#include "PointPixelLighting.h"
#include "LimLighting.h"
#include "SpotLighting.h"
#include "CookTorrance.h"
#include "DisneyPBR.h"
#include "Toon1.h"
#include "Toon2.h"
//===============================================
//グローバル変数

Camera		g_CameraObject;
Sprite2D	g_Test2D;

Field3D	g_Field;
//PolygonModel	g_Model;
//DirectionalLighting		g_VDL;
//PixelDirectionalLighting	g_PDL;
//PixelLightingBlinnPhong		g_PLB;
//HemiSphereLighting			g_HSL;
PointPixelLighting			g_PPL;
LimLighting					g_LL;
SpotLighting				g_SL;
CookTorrance				g_CT;
DisneyPBR					g_DPBR;
Toon						g_TN;
Toon2						g_TN2;
static LIGHT 		g_Light;
//ポーズフラグ
static	bool	pause = false;

//===============================================
//ポーズフラグセット
void	SetPause( bool flg )
{
	pause = flg;
}
//===============================================
//ポーズフラグ取得
bool	GetPause()
{
	return pause;
}

//===============================================
//ゲームシーン初期化
void InitGame()
{
	TextureInitialize( GetDevice() );
	InitCamera();

	g_Test2D.Init();
	g_Field.Init();
	//g_Model.Init();
	//g_VDL.Init();
	//g_PDL.Init();
	//g_PLB.Init();
	//g_HSL.Init();
	//g_PPL.Init();
	//g_LL.Init();
	//g_SL.Init();
	//g_CT.Init();
	//g_DPBR.Init();
	g_TN.Init();
	g_TN2.Init();

	// ライト構造体の初期化
	XMVECTOR dir = XMVector3Normalize( XMVectorSet( 0.0f, -1.0f, 0.0f, 0.0f ) );
	XMStoreFloat4( &g_Light.Direction, dir ); // 光のベクトル
	g_Light.Position = XMFLOAT4( 0.0f, .0f, 0.0f, 0.0f ); // ライト座標
	g_Light.Diffuse = XMFLOAT4( 0.9f, 0.9f, 0.9f, 1.0f ); // 光の色
	g_Light.Ambient = XMFLOAT4( 1.0, 1.0, 1.0, 1.0f ); // 環境光
	g_Light.PointLightParam = XMFLOAT4( 2000.0f, 1.5f, 0.0f, 0.0f ); // x距離 yPow
	g_Light.Angle.x = XMConvertToRadians( 30.0f ); // コーン角度
}

//===============================================
//ゲームシーン終了
void FinalizeGame()
{
	FinalizeCamera();
	g_Test2D.Finalize();

	g_Field.Finalize();
	//g_Model.Init();
	//g_VDL.Finalize();
	//g_PDL.Finalize();
	//g_PLB.Finalize();
	//g_HSL.Finalize();
	//g_PPL.Finalize();
	//g_LL.Finalize();
	//g_SL.Finalize();
	//g_CT.Finalize();
	//g_DPBR.Finalize();
	g_TN.Finalize();
	g_TN2.Finalize();

	TextureFinalize();
}

//===============================================
//ゲームシーン更新
void UpdateGame()
{

	if ( GetPause() == false )//ポーズ中でなければ更新実行
	{
		UpdateCamera();
		g_Test2D.Update();

		g_Field.Update();
		//g_Model.Update();
		//g_VDL.Update();
		//g_PDL.Update();
		//g_PLB.Update();
		//g_HSL.Update();
		//g_PPL.Update();
		//g_LL.Update();
		//g_SL.Update();
		//g_CT.Update();
		//g_DPBR.Update();
		g_TN.Update();
		g_TN2.Update();

	}
	//ImGui::Begin( "SpotLighting" );
	//{
	//	ImGui::SliderFloat( "DiffuseR", &g_Light.Diffuse.x, 0.0f, 1.0f, "％. 1f" );
	//	ImGui::SliderFloat( "DiffuseG", &g_Light.Diffuse.y, 0.0f, 1.0f, "％. 1f" );
	//	ImGui::SliderFloat( "DiffuseB", &g_Light.Diffuse.z, 0.0f, 1.0f, "％. 1f" );

	//	float angle = XMConvertToDegrees( g_Light.Angle.x );
	//	ImGui::SliderFloat( "Cone Angle", &angle, 5.0f, 45.0f, "％. 1f" );
	//	g_Light.Angle.x = XMConvertToRadians( angle );

	//	ImGui::SliderFloat( "Attenuation", &g_Light.PointLightParam.x, 0.0f, 10.0f, "％.1f" );
	//	ImGui::SliderFloat( "Pow", &g_Light.PointLightParam.y, 1.0f, 50.0f, "％ 2f" );
	//	ImGui::SliderFloat( "Postion.x", &g_Light.Position.x, -2.0f, 2.0f, "％, 1f" );
	//	ImGui::SliderFloat( "Postion.y", &g_Light.Position.y, -2.0f, 2.0f, "％, 1f" );
	//	ImGui::SliderFloat( "Postion.z", &g_Light.Position.z, -2.0f, 2.0f, "％, 1f" );
	//}
	//ImGui::End();
}

//===============================================
//ゲームシーン描画
void DrawGame()
{
	BeginPe();
	{
		SetDepthEnable( true );
		DrawCamera();

		SetLight( g_Light );
		g_TN2.Draw();
		g_Field.Draw();
	}

	Clear();
	{
		SetWorldViewProjection2D();
		g_Test2D.Draw();
	}
}
