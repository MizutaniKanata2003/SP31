#pragma once

//Camera.h
#include "main.h"

class Camera
{
public:
	XMFLOAT3	m_Position;		//カメラの座標
	XMFLOAT3	m_AtPosition;		//カメラの注視点
	XMFLOAT3	m_UpVector;		//上方ベクトル
	float		m_Fov;			//視野角
	float		m_Nearclip;		//どこまで近くが見えるか
	float		m_Farclip;		//どこまで遠くが見えるか
	float		m_Rotation;
};

void	InitCamera();
void	FinalizeCamera();
void	UpdateCamera();
void	DrawCamera();

