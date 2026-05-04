#include "MainModule.h"
#include "MeshUtil.h"

#include<Resources/MeshImporter.h>
#include<API/UI.h>

#include<optional>
#include<portable-file-dialogs.h>

void MainModule::OnRegister()
{
	FT_TRACE("MainModule registered");

	m_EditingModule = ft::Application::Get().RegisterModule<MeshEditingModule>();
	m_EditingModule->mainModule = this;

	ft::Renderer3D::SetClearColor(0.1, 0.1, 0.1, 0.1);

	m_NamePanel = ft::UI::AddElement<ft::Panel>("assets/namePanel.png", ft::Rect(0, 0, 240, 150));

	SetStudioLightingMode();

	ft::WorldCamera3D* camera = ft::Renderer3D::GetCamera();
	camera->SetPosition(glm::vec3(0.0f, 0.0f, 5.0f));
	camera->SetFront(glm::vec3(0.0f, 0.0f, -1.0f));
	CreateGrid();
	//camera->SetFront(glm::vec3(0.0f, -0.5f, -1.0f));
	//Mesh mesh = Mesh::CreateCube(Transform3D(glm::vec3(0, -2, 0), Vector::One * 2.0f));
	//Mesh mesh = Mesh::CreateSphere(Transform3D(glm::vec3(0, -2, -2), Vector::One * 4.0f), 160, 180);
	//Mesh mesh = Mesh::CreateCone(Transform3D(glm::vec3(0, -3, -2), Vector::One * 4.0f, Vector::Forward * 30.0f), 30);
	//test_mesh = ft::Renderer3D::AddMesh(
	//	ft::Mesh::CreateCone(
	//		ft::Transform3D(glm::vec3(0, -3, -2),
	//			ft::Vector::One * 3.0f,
	//			ft::Vector::Forward * 30.0f),
	//		glm::vec4(0.2f, 0.8f, 0.8f, 1.0f), 50));
	//ft::Mesh* newMesh = ft::Renderer3D::AddMesh(
	//	ft::Mesh::CreateSphere(
	//		ft::Transform3D(glm::vec3(0, -3, -2),
	//			ft::Vector::One * 3.0f,
	//			ft::Vector::Forward * 30.0f),
	//		glm::vec4(0.2f, 0.8f, 0.8f, 1.0f), 50, 50));

	ft::Mesh* newMesh = ft::Renderer3D::AddMesh(
						ft::Mesh::CreateCylinder(
							ft::Transform3D(glm::vec3(0, 0, 0), 
							ft::Vector::One * 3.0f), 
							//ft::Vector::Forward * 30.0f), 
							glm::vec4(0.2f, 0.8f, 0.8f, 1.0f), 30));
	newMesh->outlineColor = glm::vec4(1.0f, 0.7f, 0.157f, 1.0f);
	meshes.emplace(newMesh->GetID(), newMesh);

	//ft::Renderer3D::AddMesh(ft::Mesh::CreatePlane(ft::Transform3D(glm::vec3(0, -5.5, 0), ft::Vector::One * 10.0f), glm::vec4(1), true));
	selectionMesh = ft::Renderer3D::AddMesh(ft::Mesh(ft::Transform3D(), glm::vec4(1.0f, 0.7f, 0.157f, 0.8f), false));
	selectionMesh->SetRenderMode(ft::RenderMode::Overlay);

		FT_INFO(R"(
=== CONTROLS GUIDE ===

[ SYSTEM & FILES ]
 Info Panel   : F1 (Toggle Name/Info)
 Import OBJ   : F9
 Export OBJ   : F10 *Requires Selection*
 Quit         : ESC

[ VIEWPORT VISUALS ]
 Wireframe    : F2 (Toggle Solid/Wireframe) *Requires Selection*
 Projection   : F3 (Toggle Perspective/Orthographic)
 Lighting     : F4 (Toggle Studio/Default)

[ MODELING MODES ]
 Select Mode  : F5 (Toggle Object / Face Mode)
 Creation     : F6 (Then press 1-5 for shapes)
 Smoothing    : F7 (Cycle Flat -> Smooth -> Angle) *Requires Selection*
 Merge Mesh   : F8 (Merges Current + Last Selected)

[ CAMERA NAVIGATION ]
 Rotate       : Middle Mouse Button (Hold)
 Pan          : Shift + Middle Mouse Button (Hold)
 Zoom         : Scroll Wheel
 Move         : Shift + W/A/S/D (Planar) | Shift + Q/E (Vertical)
 Snap View    : 1-6 (Front, Left, Top, Back, Right, Bottom) *Requires Selection*

[ EDITING TOOLS ]
 Select       : Left Click
 Add/Toggle   : Shift + Left Click
 Deselect     : Right Click (Empty space)
 Delete       : DELETE
 Select All   : A (Face Mode Only)
 Move         : G
 Rotate       : R
 Scale        : S (Uniform Scale with Ctrl)
 Extrude      : E (Face Mode Only)
 Lock Axis    : X / Y / Z (During Edit)
 Confirm      : Left Click
 Cancel       : Right Click
)");
}

void MainModule::OnUpdate()
{
	for (auto& [id, mesh] : meshes) {
		//mesh->transform.rotation.y += 90.0f * ft::Time::DeltaTime();
		//mesh->CalculateModelMatrix();
	}
	if (selectedMeshID != 0) {
		if (selectionMode == SelectionMode::Object) {
			meshes[selectedMeshID]->outlined = true;
		}
		else {
			meshes[selectedMeshID]->outlined = false;
			selectionMesh->transform = meshes[selectedMeshID]->transform;
			selectionMesh->CalculateModelMatrix();
		}
	}
}

bool MainModule::OnKeyEvent(const ft::KeyEvent& event)
{
	if (event.type == ft::EventType::KeyPress) {
		if (m_ExpectingMeshAddKeyPress) {
			if (m_EditingModule->editingMode != EditingMode::Select) {
				m_ExpectingMeshAddKeyPress = false;
				return false;
			}
			glm::vec4 color = glm::vec4(0.2f, 0.8f, 0.8f, 1.0f);
			ft::Transform3D transform;
			transform.position = ft::Renderer3D::GetCamera()->GetPosition() + ft::Renderer3D::GetCamera()->GetFront() * 3.0f;
			ft::Mesh* newMesh = nullptr;
			if (event.key == GLFW_KEY_1) 
				newMesh = ft::Renderer3D::AddMesh(ft::Mesh::CreateCube(transform, color));
			else if (event.key == GLFW_KEY_2) 
				newMesh = ft::Renderer3D::AddMesh(ft::Mesh::CreateSphere(transform, color, 36, 18));
			else if (event.key == GLFW_KEY_3)
				newMesh = ft::Renderer3D::AddMesh(ft::Mesh::CreateCylinder(transform, color, 36));
			else if (event.key == GLFW_KEY_4)
				newMesh = ft::Renderer3D::AddMesh(ft::Mesh::CreateCone(transform, color, 36));
			else if (event.key == GLFW_KEY_5) 
				newMesh = ft::Renderer3D::AddMesh(ft::Mesh::CreatePlane(transform, color));

			m_ExpectingMeshAddKeyPress = false;
			if (newMesh == nullptr)
				return false;

			newMesh->outlineColor = glm::vec4(1.0f, 0.7f, 0.157f, 1.0f);
			if (m_Solid)
				newMesh->SetRenderMode(ft::RenderMode::Solid);
			else
				newMesh->SetRenderMode(ft::RenderMode::Wireframe);
			meshes.emplace(newMesh->GetID(), newMesh);
			return false;
		}

		if (event.key == GLFW_KEY_ESCAPE)
			ft::Application::Get().Close();
		if (event.key == GLFW_KEY_F3) {
			if (m_Perspective)
				ft::Renderer3D::GetCamera()->SetProjectionMode(ft::ProjectionMode::Orthographic);
			else
				ft::Renderer3D::GetCamera()->SetProjectionMode(ft::ProjectionMode::Perspective);
			m_Perspective = !m_Perspective;
		}
		if (event.key == GLFW_KEY_F2) {
			for (auto& [id, mesh] : meshes) {
				if (m_Solid)
					mesh->SetRenderMode(ft::RenderMode::Wireframe);
				else
					mesh->SetRenderMode(ft::RenderMode::Solid);
			}
			m_Solid = !m_Solid;
		}
		if (event.key == GLFW_KEY_F9 && m_EditingModule->editingMode == EditingMode::Select) {
			auto selection = pfd::open_file("Select a mesh file", ".", { "Wavefront .obj File", "*.obj" }).result();

			std::optional<ft::Mesh> mesh;
			if (selection.empty())
				FT_TRACE("No file selected.");
			else 
				mesh = ft::MeshImporter::ImportMesh(selection[0]);

			if (mesh) {
				mesh->transform.position = ft::Renderer3D::GetCamera()->GetPosition() + ft::Renderer3D::GetCamera()->GetFront() * 5.0f;
				mesh->CalculateModelMatrix();
				ft::Mesh* newMesh = (ft::Renderer3D::AddMesh(std::move(*mesh)));
				newMesh->outlineColor = glm::vec4(1.0f, 0.7f, 0.157f, 1.0f);
				if (m_Solid)
					newMesh->SetRenderMode(ft::RenderMode::Solid);
				else
					newMesh->SetRenderMode(ft::RenderMode::Wireframe);
				meshes.emplace(newMesh->GetID(), newMesh);
			}
		}
		if (event.key == GLFW_KEY_F4) {
			if (m_UsingStudioLighting)
				SetDefaultLight();
			else
				SetStudioLightingMode();
		}
		if (event.key == GLFW_KEY_F5 && m_EditingModule->editingMode == EditingMode::Select) { // Block mode change while editing
			if (selectionMode == SelectionMode::Object)
				selectionMode = SelectionMode::Face;
			else {
				selectionMode = SelectionMode::Object;
				ClearSelectionMesh();
			}
		}
		if (event.key == GLFW_KEY_F7 && selectedMeshID != 0) {
			ft::MeshData* data = GetSelectedMesh()->GetData();
			switch (data->GetSmoothingMode()) {
				case ft::SmoothingMode::Flat:
					data->SetSmoothingMode(ft::SmoothingMode::Smooth);
					break;
				case ft::SmoothingMode::Smooth:
					data->SetSmoothingMode(ft::SmoothingMode::SmoothByAngle);
					break;
				case ft::SmoothingMode::SmoothByAngle:
					data->SetSmoothingMode(ft::SmoothingMode::Flat);
					break;
			}
		}
		if (event.key == GLFW_KEY_F10 && selectedMeshID != 0) {
			auto selection = pfd::save_file("Export mesh as", ".", { "Wavefront .obj File", "*.obj" }).result();
			if (selection.empty())
				FT_TRACE("No file selected.");
			else
				ft::MeshImporter::ExportMesh(*GetSelectedMesh()->GetData(), selection);
		}
		if (event.key == GLFW_KEY_F6 && m_EditingModule->editingMode == EditingMode::Select) {
			m_ExpectingMeshAddKeyPress = true;
			return false;
		}
		if (event.key == GLFW_KEY_F8 && selectedMeshID != 0 && m_LastSelectedMeshID != 0 && m_EditingModule->editingMode == EditingMode::Select) {
			ft::MeshData merged;
			MeshUtil::MergeMeshes({ *meshes[selectedMeshID]->GetData(), *meshes[m_LastSelectedMeshID]->GetData() }, merged);
			meshes[selectedMeshID]->SetData(merged);
			ft::Renderer3D::RemoveMesh(m_LastSelectedMeshID);
			meshes.erase(m_LastSelectedMeshID);
			m_LastSelectedMeshID = 0;
			ClearSelectionMesh();
		}
		if (event.key == GLFW_KEY_F1) {
			if (m_NamePanel->ShouldRender())
				m_NamePanel->DisableRender();
			else
				m_NamePanel->EnableRender();
		}
		if (event.key == GLFW_KEY_DELETE && selectedMeshID != 0 && m_EditingModule->editingMode == EditingMode::Select) {
			ft::Renderer3D::RemoveMesh(selectedMeshID);
			meshes.erase(selectedMeshID);
			ClearSelectionMesh();
			SetSelectedID(0);
		}
		if (selectedMeshID != 0) {
			bool move = false;
			glm::vec3 direction;
			if (event.key == GLFW_KEY_1) {
				direction = ft::Vector::Forward;
				move = true;
			}
			else if (event.key == GLFW_KEY_2) {
				direction = ft::Vector::Left;
				move = true;
			}
			else if (event.key == GLFW_KEY_3) {
				direction = ft::Vector::Down;
				move = true;
			}
			else if (event.key == GLFW_KEY_4) {
				direction = ft::Vector::Back;
				move = true;
			}
			else if (event.key == GLFW_KEY_5) {
				direction = ft::Vector::Right;
				move = true;
			}
			else if (event.key == GLFW_KEY_6) {
				direction = ft::Vector::Up;
				move = true;
			}
			if (move) {
				ft::WorldCamera3D* camera = ft::Renderer3D::GetCamera();
				float depth = glm::length(camera->GetPosition() - meshes[selectedMeshID]->transform.position);
				camera->SetFront(direction);
				camera->SetPosition(meshes[selectedMeshID]->transform.position - direction * depth);
			}
		}
		if (event.key == GLFW_KEY_A && !(event.mods & GLFW_MOD_SHIFT)) {
			if (selectedMeshID != 0 && selectionMode == SelectionMode::Face && m_EditingModule->editingMode == EditingMode::Select)
			{
				ft::Mesh* mesh = meshes[selectedMeshID];
				ft::MeshData* selectionData = selectionMesh->GetData();
				if (selectedFaces.size() != mesh->GetData()->polygonSizes.size()) {
					for (uint32_t i = 0; i < mesh->GetData()->polygonSizes.size(); i++)
						selectedFaces.insert(i);
				}
				else
					selectedFaces.clear();
				MeshUtil::CreateFromFaces(*mesh->GetData(), *selectionData, selectedFaces);
				selectionMesh->BakeToRenderMesh();
			}
			return true; // Consume to prevent camera movement
		}
	}
	return false;
}

bool MainModule::OnMouseEvent(const ft::MouseEvent& event)
{
	if (event.type == ft::EventType::MousePress) {
		ft::MousePressEvent pressEvent = ft::As<ft::MousePressEvent>(event);
		if (pressEvent.button == GLFW_MOUSE_BUTTON_LEFT && m_EditingModule->editingMode == EditingMode::Select) {
			ft::WorldCamera3D* camera = ft::Renderer3D::GetCamera();
			glm::vec2 mousePos = ft::Input::GetMousePosition();
			glm::vec3 rayOrigin, rayDir;
			camera->ScreenPointToRay(mousePos, rayOrigin, rayDir);

			int faceIndex = -1;
			uint32_t meshID = 0;
			float closestDistance = std::numeric_limits<float>::max();
			for (auto& [id, mesh] : meshes) {
				ft::MeshRaycastHit result = mesh->GetFaceIndexFromRay(rayOrigin, rayDir);
				float distance = glm::length(rayOrigin - result.hitPoint);
				if (result.hit && distance < closestDistance) {
					faceIndex = result.faceIndex;
					meshID = id;
					closestDistance = distance;
				}
			}

			ft::MeshData* data = selectionMesh->GetData();
			data->Clear();
			if (faceIndex != -1) {
				if (meshID != selectedMeshID) {
					SetSelectedID(meshID);
					selectedFaces.clear();
					if (selectionMode == SelectionMode::Object) {
						ClearSelectionMesh();
						return true;
					}
				}
				if (selectionMode == SelectionMode::Object)
					return true; // Don't touch face selection in object mode
				if (pressEvent.mods == GLFW_MOD_SHIFT) {
					if (selectedFaces.contains(faceIndex))
						selectedFaces.erase(faceIndex);
					else
						selectedFaces.insert(faceIndex);
				}
				else {
					selectedFaces.clear(); 
					selectedFaces.insert(faceIndex);
				}

				if (!selectedFaces.empty()) {
					ft::Mesh* mesh = meshes[meshID];
					ft::MeshData* selectionData = selectionMesh->GetData();
					MeshUtil::CreateFromFaces(*mesh->GetData(), *selectionData, selectedFaces);
					
					selectionMesh->BakeToRenderMesh();
				}
				else {
					ClearSelectionMesh();
					SetSelectedID(0);
				}

			}
			else {
				ClearSelectionMesh();
				SetSelectedID(0);
			}
		}
	}
	return false;
}

void MainModule::CreateGrid()
{
	ft::MeshData gridData = MeshUtil::CreateGrid(200, 3);
	glm::vec4 color = glm::vec4(0.4f, 0.4f, 0.4f, 0.2f);
	m_Grid = ft::Renderer3D::AddMesh(ft::Mesh(std::move(gridData), ft::Transform3D(), color, true, ft::RenderMode::Wireframe));

	float size = 200 * 3;
	color = glm::vec4(0.8f, 0.2f, 0.2f, 0.5f);
	m_XAxis = ft::Renderer3D::AddMesh(ft::Mesh::CreateLine(ft::Vector::Left * size, ft::Vector::Right * size, color, true));
	color = glm::vec4(0.2f, 0.2f, 0.8f, 0.5f);
	m_ZAxis = ft::Renderer3D::AddMesh(ft::Mesh::CreateLine(ft::Vector::Back * size, ft::Vector::Forward * size, color, true));
}

void MainModule::SetDefaultLight()
{
	ft::LightSource* lightSource = ft::Renderer3D::GetLightSource();
	lightSource->position = glm::vec3(2.0f, 2.0f, 2.0f);
	lightSource->color = glm::vec3(1.0f);
	lightSource->intensity = 1.2f;
	lightSource->ambientIntensity = 0.2f;
	ft::Renderer3D::SetightingMode(false);
	m_UsingStudioLighting = false;
}

void MainModule::SetStudioLightingMode()
{
	ft::Renderer3D::SetightingMode(true);
	m_UsingStudioLighting = true;
}

void MainModule::ClearSelectionMesh()
{
	ft::MeshData* data = selectionMesh->GetData();
	data->Clear();
	selectionMesh->BakeToRenderMesh();
}