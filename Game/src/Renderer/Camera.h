#pragma once
#include <glm/glm.hpp>


struct ViewFrustum {
	float left;
	float right;
	float top;
	float bottom;
	float near = 0.1f;
	float far = 1000.0f;

	static ViewFrustum CalculateScreenFrustum();
};

class Camera {
public:
	Camera() { }
	Camera(ViewFrustum frustum, glm::vec2 position = { 0,0 }, float zoom = 1, float rotation = 0);

	void CalculateView();
	void CalculateProjection();

	const glm::mat4& GetView() { return m_View; }
	const glm::mat4& GetProjection() { return m_Projection; }
	const glm::mat4& GetViewProjection() { return m_ViewProjection; }

	const ViewFrustum& GetViewFrustum() { return m_Frustum; }
	const ViewFrustum& GetZoomFrustum() { return m_ZoomFrustum; }

	const glm::vec2& GetPosition() { return m_Position; }
	float GetRotation() { return m_Rotation; }
	float GetZoom() { return m_Zoom; }

	void SetViewFrustum(const ViewFrustum& frustum);
	void SetPosition(glm::vec2 position);
	void SetRotation(float rotation);
	void SetZoom(float zoom);

	glm::vec2 ScreenSpacePos(const glm::vec2& v);

private:
	glm::vec2 m_Position;
	float m_Rotation;
	float m_Zoom;

	ViewFrustum m_Frustum;
	ViewFrustum m_ZoomFrustum;
	
	glm::mat4 m_Projection;
	glm::mat4 m_View;
	glm::mat4 m_ViewProjection;
};