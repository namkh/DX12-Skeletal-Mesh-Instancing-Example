#pragma once

#include "Defaults.h"
#include "CoreEventManager.h"
#include "Events.h"

class SphericalCoordMovementCamera
{
private:

	static const float DEFAULT_ROTATION_SENSITIVITY;
	static const float DEFAULT_ZOOM_SENSITIVITY;

	static const float PHI_LIMIT;
	static const float NEAR_LIMIT;

public:

	SphericalCoordMovementCamera();

public:

	bool Initialize();
	bool Initialize(float width, float height, float nearDistance, float farDistance, float fovAngleY);
	void Destroy();

	void RegisterEvents();
	void UnregisterEvents();

	void OnMouseEvent(MouseEvent* mouseEvent);
	void OnScreenSizeChanged(ScreenSizeChangedEvent* coreSystemEvent);

public:

	float	GetFovAngleY();
	void	SetFovAngleY(float fovAngleY);

	float	GetWidth();
	void	SetWidth(float width);

	float	GetHeight();
	void	SetHeight(float height);

	float	GetNearDistance();
	void	SetNearDistance(float nearDistance);
	float	GetFarDistance();
	void	SetFarDistance(float farDistance);

	float	GetRadius();
	void	SetRadius(float radius);

	void	SetRotSensitivity(float sensitivity);
	void	SetZoomSensitivity(float sensitivity);
	float	GetAspectRatio();
	XMFLOAT3 GetPosition();

	void	SetSeta(float seta);
	void	SetPhi(float phi);
	
public:

	XMMATRIX GetViewMatrix();
	XMMATRIX GetProjectionMatrix();
	XMMATRIX GetViewProjectionMatrix();

protected:

	XMMATRIX UpdateProjectionMatrix();

private:

	float	m_width = 0.0f;
	float	m_height = 0.0f;
	float	m_fovAngleY = 0.0f;
	float	m_near = 0.0f;
	float	m_far = 0.0f;

	float	m_seta = 0.0f;
	float	m_phi = 0.0f;
	float	m_radaius = 10.0f;

	float	m_rotSensitivity = 1.0f;
	float	m_zoomSensitivity = 1.0f;

	XMMATRIX	m_cameraMatrix = {};

	XMFLOAT4X4	m_viewMatrix = IDENTITY_FLOAT4X4;
	XMFLOAT4X4	m_projectionMatrix = IDENTITY_FLOAT4X4;
	XMFLOAT4X4	m_viewProjMatrix = IDENTITY_FLOAT4X4;

	bool m_needUpdateViewMatrix = true;
	bool m_needUpdateProjectionMatrix = true;

	XMFLOAT3 m_positionFloat3x3 = { 0.0f, 0.0f, 0.0f };
	XMVECTOR m_position = {};
	XMVECTOR m_lookAt = {};
	XMVECTOR m_up = {};

private:

	CoreEventHandle m_mouseEventHandle;
	CoreEventHandle m_screenSizeChangedEventHandle;
};

