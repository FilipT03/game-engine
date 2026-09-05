#pragma once

#include<GameEngine.h>

class MainModule;
class MeshHistoryModule;
enum class EditingMode {
	Select,
	Extrude,
	Move,
	Rotate,
	Scale,
	Cut
};

class MeshEditingModule : public ft::Module
{
public:
	void OnRegister();
	void OnDelete() {};
	void OnUpdate();
	bool OnEvent(const ft::Event& event) { return false; };
	bool OnKeyEvent(const ft::KeyEvent& event);
	bool OnMouseEvent(const ft::MouseEvent& event);
	MainModule* mainModule;
	MeshHistoryModule* historyModule;

	EditingMode editingMode = EditingMode::Select;
	void SetEditingModeAndRevertChanges(EditingMode newMode);

private:
	void UpdateLockAxisMesh();
	void HandleCutting();
	std::string GetActionNameForMode(EditingMode mode);

	glm::vec2 m_StartMousePos;
	float m_StartObjectDepth;
	ft::MeshData m_OriginalMeshData;
	ft::Transform3D m_OriginalTransform;
	glm::vec3 m_LockVector;
	ft::Mesh* m_LockAxisMesh;
	ft::Mesh* m_CurrentVertexHighlight, *m_SelectedVertexHighlight;
	std::optional<uint32_t> m_CurrentVertexIndex = 0, m_SelectedVertexIndex = 0;
	uint32_t m_CuttingFaceIndex = 0;

	const glm::vec4 m_CuttingCurrentColor = glm::vec4(1.0f, 0.0f, 0.0f, 1.0f);
	const glm::vec4 m_CuttingSelectedColor = glm::vec4(1.0f, 0.5f, 0.0f, 1.0f);
	const glm::vec4 m_CutttingSelectedHoverColor = glm::vec4(1.0f, 0.2f, 0.0f, 1.0f);
};

