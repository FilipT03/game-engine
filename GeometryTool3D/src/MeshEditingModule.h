#pragma once

#include<GameEngine.h>

class MainModule;
enum class EditingMode {
	Select,
	Extrude,
	Move,
	Rotate,
	Scale
};

class MeshEditingModule : public ft::Module
{
public:
	void OnRegister() {};
	void OnDelete() {};
	void OnUpdate();
	bool OnEvent(const ft::Event& event) { return false; };
	bool OnKeyEvent(const ft::KeyEvent& event);
	bool OnMouseEvent(const ft::MouseEvent& event);
	MainModule* mainModule;

	EditingMode editingMode = EditingMode::Select;

private:
	void SetEditingModeAndRevertChanges(EditingMode newMode);
	void UpdateLockAxisMesh();

	glm::vec2 m_StartMousePos;
	float m_StartObjectDepth;
	ft::MeshData m_OriginalMeshData;
	ft::Transform3D m_OriginalTransform;
	glm::vec3 m_LockVector;
	ft::Mesh* m_LockAxisMesh;
};

