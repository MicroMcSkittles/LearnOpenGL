#include "Camera.h"
#include <glm/gtc/matrix_transform.hpp>
#include "Core/Application.h"

Camera::Camera(ViewFrustum frustum, glm::vec2 position, float zoom, float rotation) {
	m_Frustum = frustum;
	m_Position = position;
	m_Rotation = rotation;
	m_Zoom = zoom;

	m_View = glm::mat4(1);
	m_View = glm::rotate(m_View, m_Rotation, { 0,0,1 });
	m_View = glm::translate(m_View, { -m_Position, 0 });

	CalculateProjection();
}

void Camera::CalculateView() {
	m_View = glm::mat4(1);
	m_View = glm::rotate(m_View, m_Rotation, { 0,0,1 });
	m_View = glm::translate(m_View, { -m_Position, 0 });

	m_ViewProjection = m_Projection * m_View;
}

void Camera::CalculateProjection()
{
	m_ZoomFrustum.left = m_Frustum.left * m_Zoom;
	m_ZoomFrustum.right = m_Frustum.right * m_Zoom;
	m_ZoomFrustum.bottom = m_Frustum.bottom * m_Zoom;
	m_ZoomFrustum.top = m_Frustum.top * m_Zoom;

	m_Projection = glm::ortho(
		m_ZoomFrustum.left,
		m_ZoomFrustum.right,
		m_ZoomFrustum.bottom,
		m_ZoomFrustum.top,
		m_ZoomFrustum.near,
		m_ZoomFrustum.far
	);

	m_ViewProjection = m_Projection * m_View;
}

void Camera::SetViewFrustum(const ViewFrustum& frustum)
{
	m_Frustum = frustum;
	CalculateProjection();
}
void Camera::SetPosition(glm::vec2 position) {
	m_Position = position;
	CalculateView();
}
void Camera::SetRotation(float rotation) {
	m_Rotation = rotation;
	CalculateView();
}
void Camera::SetZoom(float zoom) {
	m_Zoom = zoom;
	CalculateProjection();
}

glm::vec2 Camera::ScreenSpacePos(const glm::vec2& v)
{
	glm::vec2 screenPos = m_ViewProjection * glm::vec4(v, 0.0f, 1.0f);
	auto window = Application::Get()->GetWindow();
	screenPos.x = window->GetWidth() * ((screenPos.x + 1.0f) / 2.0f);
	screenPos.y = window->GetHeight() - (window->GetHeight() * ((screenPos.y + 1.0f) / 2.0f));
	return screenPos;
}

ViewFrustum ViewFrustum::CalculateScreenFrustum() 
{
	ViewFrustum frustum;
	auto window = Application::Get()->GetWindow();
	float width = (float)window->GetWidth();
	float height = (float)window->GetHeight();
	float aspectRatio = width / height;

	frustum.left = -aspectRatio;
	frustum.right = aspectRatio;
	frustum.bottom = -1;
	frustum.top = 1;

	return frustum;
}
