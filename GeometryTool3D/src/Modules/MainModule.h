#pragma once

#include<GameEngine.h>
#include<UI/UIElement.h>
#include <GLFW/glfw3.h>

#include "MeshEditingModule.h"
#include "MeshHistoryModule.h"

enum class SelectionMode {
	Object,
	Face
};

class MainModule : public ft::Module
{
public:
	void OnRegister();
	void OnDelete() {};
	void OnUpdate();
	bool OnEvent(const ft::Event& event) { return false; };
	bool OnKeyEvent(const ft::KeyEvent& event);
	bool OnMouseEvent(const ft::MouseEvent& event);

	std::unordered_map<uint32_t, ft::Mesh*> meshes;
	ft::Mesh* selectionMesh;
	SelectionMode selectionMode = SelectionMode::Object;
	uint32_t selectedMeshID = 0;
	std::set<uint32_t> selectedFaces;

	void SetSelectedID(uint32_t id) {
		if (selectedMeshID != 0 && meshes.contains(selectedMeshID))
			meshes[selectedMeshID]->outlined = false;
		m_LastSelectedMeshID = selectedMeshID;
		selectedMeshID = id;
	}
	ft::Mesh* GetSelectedMesh() {
		if (selectedMeshID == 0)
			return nullptr;
		return meshes[selectedMeshID];
	}
	void ClearSelectionMesh();
	void AddMesh(ft::Mesh&& mesh);
	void RemoveMesh(uint32_t id);
private:
	void CreateGrid();
	void SetDefaultLight();
	void SetStudioLightingMode();

	MeshEditingModule* m_EditingModule;
	MeshHistoryModule* m_HistoryModule;

	ft::Mesh* m_Grid, *m_XAxis, *m_ZAxis;
	bool m_Perspective = true;
	bool m_Solid = true;
	bool m_UsingStudioLighting = true;
	bool m_ExpectingMeshAddKeyPress = false;
	uint32_t m_LastSelectedMeshID = 0;
	ft::UIElement* m_NamePanel;
	glm::vec4 m_SelectionColor = glm::vec4(1,1,1,1);
	glm::vec4 m_ObjectColor = glm::vec4(1,1,1,1);
};

