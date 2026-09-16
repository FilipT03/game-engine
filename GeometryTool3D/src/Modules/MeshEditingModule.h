#pragma once

#include<GameEngine.h>
#include<API/UI.h>

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

struct NumericInput
{
	bool active = false, negative = false;
	std::string text;

	void Clear() {
		active = false;
		negative = false;
		text.clear();
	}

	float GetValue() const {
		if (text.empty() || text == ".")
			return 0.0f;

		try {
			float value = std::stof(text);
			return negative ? -value : value;
		}
		catch (...) {
			return 0.0f;
		}
	}
};

struct LockFrame
{
	glm::vec3 axisX, axisY, axisZ;
	bool lockX = false, lockY = false, lockZ = false;

	void CalculateX() {
		axisX = glm::normalize(glm::cross(axisY, axisZ));
	}
	void CalculateY() {
		axisY = glm::normalize(glm::cross(axisZ, axisX));
	}
	void CalculateZ() {
		axisZ = glm::normalize(glm::cross(axisX, axisY));
	}
	void Clear() {
		lockX = lockY = lockZ = false;
	}
	void SetCardinal() {
		axisX = glm::vec3(1.0f, 0.0f, 0.0f);
		axisY = glm::vec3(0.0f, 1.0f, 0.0f);
		axisZ = glm::vec3(0.0f, 0.0f, 1.0f);
	}
	void SetFromTransform(const ft::Transform3D& transform) {
		SetCardinal();
		glm::mat4 
	}
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
	bool HandleNumericInput(const int key, const int mods);
	void ConfirmEdit();
	std::string GetActionNameForMode(EditingMode mode);

	glm::vec2 m_StartMousePos;
	float m_StartObjectDepth;
	ft::MeshData m_OriginalMeshData;
	ft::Transform3D m_OriginalTransform;
	glm::vec3 m_LockVector;
	ft::Mesh* m_LockAxisMesh;
	ft::UIShape* m_MouseStartIndicator;
	NumericInput m_NumericInput;
	ft::Mesh* m_CurrentVertexHighlight, *m_SelectedVertexHighlight;
	std::optional<uint32_t> m_CurrentVertexIndex = 0, m_SelectedVertexIndex = 0;
	uint32_t m_CuttingFaceIndex = 0;

	const glm::vec4 m_CuttingCurrentColor = glm::vec4(1.0f, 0.0f, 0.0f, 1.0f);
	const glm::vec4 m_CuttingSelectedColor = glm::vec4(1.0f, 0.5f, 0.0f, 1.0f);
	const glm::vec4 m_CutttingSelectedHoverColor = glm::vec4(1.0f, 0.2f, 0.0f, 1.0f);
};

