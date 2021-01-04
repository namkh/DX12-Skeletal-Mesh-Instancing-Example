
#include <math.h>

#include "SphericalCoordMovementCamera.h"
#include "GlobalSystemValues.h"
#include "CoreEventManager.h"
#include "Utils.h"

const float	SphericalCoordMovementCamera::DEFAULT_ROTATION_SENSITIVITY = 0.005f;
const float	SphericalCoordMovementCamera::DEFAULT_ZOOM_SENSITIVITY = 0.2f;


const float SphericalCoordMovementCamera::PHI_LIMIT = PI * 0.99999f;
const float SphericalCoordMovementCamera::NEAR_LIMIT = 0.00001f;

SphericalCoordMovementCamera::SphericalCoordMovementCamera()
{
	XMFLOAT3 lookAt = XMFLOAT3(0.0f, 0.0f, 0.0f);
	m_lookAt = XMLoadFloat3(&lookAt);
	XMFLOAT3 up = XMFLOAT3(0.0f, 1.0f, 0.0f);
	m_up = XMLoadFloat3(&up);

	m_cameraMatrix = XMMatrixIdentity();

	SET_IDENTITY_FLOAT4X4(m_viewMatrix);
	SET_IDENTITY_FLOAT4X4(m_projectionMatrix);
	SET_IDENTITY_FLOAT4X4(m_viewProjMatrix);

	memset(&m_mouseEventHandle, 0, sizeof(CoreEventHandle));
}

bool SphericalCoordMovementCamera::Initialize()
{
	RegisterEvents();

	m_width = static_cast<float>(GlobalSystemValues::Instance().ScreenWidth);
	m_height = static_cast<float>(GlobalSystemValues::Instance().ScreenHeight);
	m_near = GlobalSystemValues::Instance().ViewportNearDistance;
	m_far = GlobalSystemValues::Instance().ViewportFarDistance;
	m_fovAngleY = GlobalSystemValues::Instance().FovAngleY;

	XMMATRIX matProj = UpdateProjectionMatrix();
	XMStoreFloat4x4(&m_projectionMatrix, matProj);

	return true;
}

void SphericalCoordMovementCamera::Destroy()
{
	UnregisterEvents();
}

bool SphericalCoordMovementCamera::Initialize(float width, float height, float nearDistance, float farDistance, float fovAngleY)
{
	RegisterEvents();
	m_width = width;
	m_height = height;
	m_near = nearDistance;
	m_far = farDistance;
	m_fovAngleY = fovAngleY;
	XMMATRIX matProj = UpdateProjectionMatrix();
	XMStoreFloat4x4(&m_projectionMatrix, matProj);

	return true;
}

void SphericalCoordMovementCamera::RegisterEvents()
{
	m_mouseEventHandle = CoreEventManager::Instance().RegisterMouseEventCallback(this, &SphericalCoordMovementCamera::OnMouseEvent);
	m_screenSizeChangedEventHandle = CoreEventManager::Instance().RegisterScreenSizeChangedEventCallback(this, &SphericalCoordMovementCamera::OnScreenSizeChanged);
}

void SphericalCoordMovementCamera::UnregisterEvents()
{
	CoreEventManager::Instance().UnregisterEventCallback(m_mouseEventHandle);
	CoreEventManager::Instance().UnregisterEventCallback(m_screenSizeChangedEventHandle);
}

void SphericalCoordMovementCamera::OnMouseEvent(MouseEvent* mouseEvent)
{
	if (mouseEvent->CheckMouseEvent(EMouseEvent::MOUSE_MOVE))
	{
		m_seta += mouseEvent->m_dx * DEFAULT_ROTATION_SENSITIVITY * m_rotSensitivity;
		m_phi += mouseEvent->m_dy * DEFAULT_ROTATION_SENSITIVITY * m_rotSensitivity;
		m_phi = min(m_phi, PHI_LIMIT);
		m_phi = max(m_phi, 0.000001f);
		m_needUpdateViewMatrix = true;
	}
	if (mouseEvent->CheckMouseEvent(EMouseEvent::MOUSE_WHEEL_UP))
	{
		m_radaius -= DEFAULT_ZOOM_SENSITIVITY * m_zoomSensitivity;
		m_radaius = max(m_radaius, NEAR_LIMIT);
		m_needUpdateViewMatrix = true;
	}
	if (mouseEvent->CheckMouseEvent(EMouseEvent::MOUSE_WHEEL_DOWN))
	{
		m_radaius += DEFAULT_ZOOM_SENSITIVITY * m_zoomSensitivity;
		m_needUpdateViewMatrix = true;
	}
}

void SphericalCoordMovementCamera::OnScreenSizeChanged(ScreenSizeChangedEvent* screenSizeChangedEvent)
{
	if (screenSizeChangedEvent != nullptr)
	{
		SetWidth(static_cast<float>(screenSizeChangedEvent->Width));
		SetHeight(static_cast<float>(screenSizeChangedEvent->Height));
	}
}

float SphericalCoordMovementCamera::GetFovAngleY()
{
	return m_fovAngleY;
}

void SphericalCoordMovementCamera::SetFovAngleY(float fovAngleY)
{
	m_needUpdateProjectionMatrix = true;
	m_fovAngleY = fovAngleY;
}

float SphericalCoordMovementCamera::GetWidth()
{
	return m_width;
}

void SphericalCoordMovementCamera::SetWidth(float width)
{
	m_needUpdateProjectionMatrix = true;
	m_width = width;
}

float SphericalCoordMovementCamera::GetHeight()
{
	return m_height;
}

void SphericalCoordMovementCamera::SetHeight(float height)
{
	m_needUpdateProjectionMatrix = true;
	m_height = height;
}

float SphericalCoordMovementCamera::GetNearDistance()
{
	m_needUpdateProjectionMatrix = true;
	return m_near;
}

void SphericalCoordMovementCamera::SetNearDistance(float nearDistance)
{
	m_near = nearDistance;
}

float SphericalCoordMovementCamera::GetFarDistance()
{
	m_needUpdateProjectionMatrix = true;
	return m_far;
}

void SphericalCoordMovementCamera::SetFarDistance(float farDistance)
{
	m_far = farDistance;
}

float SphericalCoordMovementCamera::GetRadius()
{
	return m_radaius;
}

void SphericalCoordMovementCamera::SetRadius(float radius)
{
	m_radaius = radius;
}

void SphericalCoordMovementCamera::SetRotSensitivity(float sensitivity)
{
	m_rotSensitivity = sensitivity;
}

void SphericalCoordMovementCamera::SetZoomSensitivity(float sensitivity)
{
	m_zoomSensitivity = sensitivity;
}

float SphericalCoordMovementCamera::GetAspectRatio()
{
	if (m_height > 0)
	{
		return m_width / m_height;
	}
	return -1.0f;
}

XMFLOAT3 SphericalCoordMovementCamera::GetPosition()
{
	return m_positionFloat3x3;
}

void SphericalCoordMovementCamera::SetSeta(float seta)
{
	m_seta = seta;
}

void SphericalCoordMovementCamera::SetPhi(float phi)
{
	m_phi = phi;
}

XMMATRIX SphericalCoordMovementCamera::GetViewMatrix()
{
	if (m_needUpdateViewMatrix)
	{
		m_needUpdateViewMatrix = false;
		
		m_positionFloat3x3 = XMFLOAT3(m_radaius * sinf(m_phi) * cosf(m_seta),
									  m_radaius * cosf(m_phi),
									  m_radaius * sinf(m_phi) * sinf(m_seta));
		m_position = XMLoadFloat3(&m_positionFloat3x3);
		XMFLOAT3 lookAtPos = XMFLOAT3(0.0f, 0.0f, 0.0f);
		XMFLOAT3 up = XMFLOAT3(0.0f, 1.0f, 0.0f);
		XMMATRIX matView = XMMatrixLookAtRH(m_position, m_lookAt, m_up);
		XMStoreFloat4x4(&m_viewMatrix, matView);
		return matView;
	}
	
	return XMLoadFloat4x4(&m_viewMatrix);
}

XMMATRIX SphericalCoordMovementCamera::GetProjectionMatrix()
{
	if (m_needUpdateProjectionMatrix)
	{
		m_needUpdateProjectionMatrix = false;
		XMMATRIX matProj = UpdateProjectionMatrix();
		XMStoreFloat4x4(&m_projectionMatrix, matProj);
		return matProj;
	}
	return XMLoadFloat4x4(&m_projectionMatrix);
}

XMMATRIX SphericalCoordMovementCamera::GetViewProjectionMatrix()
{
	return XMMatrixMultiply(GetViewMatrix(), GetProjectionMatrix());
}

XMMATRIX SphericalCoordMovementCamera::UpdateProjectionMatrix()
{
	return XMMatrixPerspectiveFovRH(m_fovAngleY, GetAspectRatio(), m_near, m_far);
}