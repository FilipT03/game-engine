#pragma once
#include<GameEngine.h>
#include<UI/UIElement.h>
#include <GLFW/glfw3.h>

class MainModule : public ft::Module
{
public:
	void OnRegister();
	void OnDelete() {};
	void OnUpdate();
	bool OnEvent(const ft::Event& event) { return false; };
	bool OnKeyEvent(const ft::KeyEvent& event);
	bool OnMouseEvent(const ft::MouseEvent& event);

private:
	void CreateGrid();
	void SetDefaultLight();
	void SetAmbientOnlyLight();

	ft::Mesh* m_SelectionMesh;
	std::unordered_map<uint32_t, ft::Mesh*> m_Meshes;
	ft::Mesh* m_Grid, *m_XAxis, *m_ZAxis;
	bool m_Perspective = true;
	bool m_Solid = true;
	bool m_UsingAmbientOnlyLight = false;
	uint32_t m_SelectedMeshID = 0;
	std::set<uint32_t> m_SelectedFaces;
};

