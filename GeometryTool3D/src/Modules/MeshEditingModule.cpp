#include "MeshEditingModule.h"
#include "MainModule.h"
#include "../MeshUtil.h"


void MeshEditingModule::OnRegister()
{
	ft::Mesh sphere = ft::Mesh::CreateSphere(ft::Transform3D(ft::Vector::Zero, ft::Vector::One * 0.08f), m_CuttingCurrentColor, 90, 45, true);
	sphere.DisableRendering();
	sphere.SetRenderMode(ft::RenderMode::Overlay);
	m_CurrentVertexHighlight = ft::Renderer3D::AddMesh(std::move(sphere));
	
	sphere = ft::Mesh::CreateSphere(ft::Transform3D(ft::Vector::One, ft::Vector::One * 0.08f), m_CuttingSelectedColor, 90, 45, true);
	sphere.DisableRendering();
	sphere.SetRenderMode(ft::RenderMode::Overlay);
	m_SelectedVertexHighlight = ft::Renderer3D::AddMesh(std::move(sphere));

	m_CurrentVertexIndex = std::nullopt;
	m_SelectedVertexIndex = std::nullopt;

	UpdateLockAxisMesh();

	m_MouseStartIndicator = ft::UI::AddElement<ft::UIShape>(ft::ShapeType::Ellipse, ft::Rect(0, 0, 5, 5), glm::vec4(1.0f, 0.1f, 0.1f, 0.65f));
	ft::Ellipse* circle = static_cast<ft::Ellipse*>(m_MouseStartIndicator->GetShape());
	circle->AA = 0.7f;
	m_MouseStartIndicator->DisableRender();
}

void MeshEditingModule::OnUpdate()
{
	if (mainModule->selectedMeshID == 0 || editingMode == EditingMode::Select)
		return;
	if (editingMode == EditingMode::Cut)
	{
		HandleCutting();
		return;
	}

	ft::WorldCamera3D* camera = ft::Renderer3D::GetCamera();
	ft::Mesh* selectedMesh = mainModule->GetSelectedMesh();

	glm::vec3 objectDelta;
	glm::vec2 mouseDelta;
	float numericValue;
	if (!m_NumericInput.active) {
		glm::vec2 mousePos = ft::Input::GetMousePosition();
		mouseDelta = mousePos - m_StartMousePos;
		objectDelta = ft::Renderer3D::ScreenDeltaToWorld(mousePos, mouseDelta, m_StartObjectDepth);
		if (glm::length(m_LockVector) > 0.0001f) {
			float projectedDist = glm::dot(objectDelta, m_LockVector);
			objectDelta = m_LockVector * projectedDist;
		}
	}
	else {
		numericValue = m_NumericInput.GetValue();
		if (glm::length(m_LockVector) > 0.0001f)
			objectDelta = numericValue * m_LockVector;
		else
			objectDelta = numericValue * ft::Vector::One;
	}

	if (mainModule->selectionMode == SelectionMode::Object) {
		switch (editingMode) {
		case EditingMode::Move:
			if (m_NumericInput.active) {
				if (glm::length(m_LockVector) <= 0.0001f)
					objectDelta *= ft::Vector::Forward;
			}
			selectedMesh->transform.position = m_OriginalTransform.position + objectDelta;
			break;
		case EditingMode::Rotate:
			if (!m_NumericInput.active)
				objectDelta *= 15;
			selectedMesh->transform.rotation = m_OriginalTransform.rotation + objectDelta;
			break;
		case EditingMode::Scale: {
			if (!m_NumericInput.active)
			{
				if (glm::length(m_LockVector) > 0.00001f)
					objectDelta += ft::Vector::One - m_LockVector;
				else {
					float combined = glm::abs(objectDelta.x) + glm::abs(objectDelta.y) + glm::abs(objectDelta.z);
					if (ft::Input::IsCtrlDown())
						objectDelta = glm::vec3(combined / 3.0f);
					objectDelta = (glm::abs(objectDelta) + 0.1f) * 1.2f;
				}
			}
			else if (glm::length(m_LockVector) > 0.00001f)
				objectDelta += ft::Vector::One - m_LockVector;
			selectedMesh->transform.scale = m_OriginalTransform.scale * objectDelta;
			break;
		}
		}
		selectedMesh->CalculateModelMatrix();
		return;
	}
	else {
		selectedMesh->SetData(m_OriginalMeshData);
		ft::MeshData* selectedMeshData = selectedMesh->GetData();
		if (selectedMeshData->UsesCustomNormals() && editingMode != EditingMode::Select) {
			selectedMeshData->SetUsesCustomNormals(false);
			selectedMeshData->SetSmoothingMode(selectedMeshData->GetSmoothingMode());
		}
		// Convert to local space rotation
		objectDelta = glm::mat3(glm::inverse(selectedMesh->GetRotationMatrix())) * objectDelta;
		switch (editingMode) {
		case EditingMode::Move:
			if (m_NumericInput.active) {
				if (glm::length(m_LockVector) <= 0.0001f)
					objectDelta *= ft::Vector::Forward;
			}
			MeshUtil::MoveFaces(*selectedMeshData, mainModule->selectedFaces, objectDelta * 0.2f);
			break;
		case EditingMode::Scale: {
			if (!m_NumericInput.active)
			{
				if (glm::length(m_LockVector) > 0.00001f) {
					objectDelta += ft::Vector::One - m_LockVector;
				}
				else {
					float combined = glm::abs(objectDelta.x) + glm::abs(objectDelta.y) + glm::abs(objectDelta.z);
					if (ft::Input::IsCtrlDown())
						objectDelta = glm::vec3(combined / 3.0f);
					objectDelta = (glm::abs(objectDelta) + 0.1f) * 3.0f;
				}
			}
			MeshUtil::ScaleFaces(*selectedMeshData, mainModule->selectedFaces, objectDelta, glm::length(m_LockVector) > 0.00001f);
			break;
		}
		case EditingMode::Extrude:
			if (m_NumericInput.active) {
				if (glm::length(m_LockVector) <= 0.0001f)
					objectDelta *= ft::Vector::Forward;
			}
			MeshUtil::ExtrudeFaces(*selectedMeshData, mainModule->selectedFaces, objectDelta * 0.2f);
			break;
		case EditingMode::Rotate: {
			glm::mat4 rotMatrix(1.0f);
			float sensitivity = 0.01f;
			if (glm::length(m_LockVector) > 0.0001f) {
				float angle = 0.0f;
				if (m_NumericInput.active)
					angle = glm::radians(numericValue);
				else
					angle = (mouseDelta.x + mouseDelta.y) * sensitivity;
				rotMatrix = glm::rotate(glm::mat4(1.0f), angle, m_LockVector);
			}
			else {
				if (m_NumericInput.active)
					rotMatrix = glm::rotate(glm::mat4(1.0f), glm::radians(numericValue), camera->GetFront());
				else {
					glm::vec3 camUp = camera->GetUp();
					glm::vec3 camRight = camera->GetRight();

					glm::mat4 yaw = glm::rotate(glm::mat4(1.0f), -mouseDelta.x * sensitivity, camUp);
					glm::mat4 pitch = glm::rotate(glm::mat4(1.0f), -mouseDelta.y * sensitivity, camRight);

					rotMatrix = yaw * pitch;
				}
			}
			MeshUtil::RotateFaces(*selectedMeshData, mainModule->selectedFaces, rotMatrix);
			break;
		}
		}
		selectedMesh->BakeToRenderMesh();
		ft::MeshData* selectionData = mainModule->selectionMesh->GetData();
		MeshUtil::CreateFromFaces(*selectedMeshData, *selectionData, mainModule->selectedFaces);
		mainModule->selectionMesh->BakeToRenderMesh();
	}
}

void MeshEditingModule::HandleCutting()
{
	ft::WorldCamera3D* camera = ft::Renderer3D::GetCamera();
	glm::vec2 mousePos = ft::Input::GetMousePosition();
	uint32_t vertexIndex = 0;
	bool found = MeshUtil::FindClosestVertexToMouse(mainModule->GetSelectedMesh(), m_CuttingFaceIndex, mousePos, camera, vertexIndex, 30.0f);
	if (found) {
		m_CurrentVertexIndex = vertexIndex;
		
		if (m_SelectedVertexIndex != m_CurrentVertexIndex) {
			const ft::Mesh* mesh = mainModule->GetSelectedMesh();
			const ft::MeshData* data = mesh->GetData();
			m_CurrentVertexHighlight->EnableRendering();
			const glm::vec3& localPosition = data->positions[vertexIndex];
			const glm::vec3 worldPosition = glm::vec3(mesh->modelMatrix * glm::vec4(localPosition, 1.0f));
			m_CurrentVertexHighlight->transform.position = worldPosition;
			m_CurrentVertexHighlight->CalculateModelMatrix();
		}
	}
	else {
		m_CurrentVertexHighlight->DisableRendering();
		m_CurrentVertexIndex = std::nullopt;
	}

	if (m_SelectedVertexIndex == m_CurrentVertexIndex)
		m_SelectedVertexHighlight->color = m_CutttingSelectedHoverColor;
	else
		m_SelectedVertexHighlight->color = m_CuttingSelectedColor;
}

bool MeshEditingModule::OnKeyEvent(const ft::KeyEvent& event)
{
	if (event.type == ft::EventType::KeyPress) {
		if (mainModule->selectedMeshID == 0 || (event.mods & GLFW_MOD_SHIFT))
			return false;
		switch (event.key) {
		case GLFW_KEY_G:
			SetEditingModeAndRevertChanges(EditingMode::Move);
			break;
		case GLFW_KEY_R:
			SetEditingModeAndRevertChanges(EditingMode::Rotate);
			break;
		case GLFW_KEY_S:
			SetEditingModeAndRevertChanges(EditingMode::Scale);
			break;
		case GLFW_KEY_E:
			if (mainModule->selectionMode == SelectionMode::Object)
				break;
			SetEditingModeAndRevertChanges(EditingMode::Extrude);
			break;
		case GLFW_KEY_C:
			if (mainModule->selectionMode == SelectionMode::Object)
				break;
			if (mainModule->selectedFaces.size() != 1) // Only allow cutting a single face at a time
				break;
			m_CuttingFaceIndex = *mainModule->selectedFaces.begin();
			SetEditingModeAndRevertChanges(EditingMode::Cut);
			break;

		case GLFW_KEY_X:
			if (editingMode == EditingMode::Select || editingMode == EditingMode::Cut)
				break;
			if (m_LockVector.x == 1)
				m_LockVector = { 0, 0, 0 };
			else 
				m_LockVector = { 1, 0, 0 };
			UpdateLockAxisMesh();
			break;
		case GLFW_KEY_Y:
			if (editingMode == EditingMode::Select || editingMode == EditingMode::Cut)
				break;
			if (m_LockVector.y == 1)
				m_LockVector = { 0, 0, 0 };
			else
				m_LockVector = { 0, 1, 0 };
			UpdateLockAxisMesh();
			break;
		case GLFW_KEY_Z:
			if (editingMode == EditingMode::Select || editingMode == EditingMode::Cut)
				break;
			if (m_LockVector.z == 1)
				m_LockVector = { 0, 0, 0 };
			else 
				m_LockVector = { 0, 0, 1 };
			UpdateLockAxisMesh();
			break;
		case GLFW_KEY_ESCAPE:
			SetEditingModeAndRevertChanges(EditingMode::Select);
			break;
		case GLFW_KEY_ENTER:
			ConfirmEdit();
			break;
		default:
			break;
		}
		if (editingMode == EditingMode::Move || editingMode == EditingMode::Rotate
			|| editingMode == EditingMode::Scale || editingMode == EditingMode::Extrude)
			HandleNumericInput(event.key, event.mods);
	}
	return false;
}

bool MeshEditingModule::OnMouseEvent(const ft::MouseEvent& event)
{
	if (event.type == ft::EventType::MousePress) {
		ft::MousePressEvent mousePressEvent = ft::As<ft::MousePressEvent>(event);
		if (mainModule->selectedMeshID == 0 || editingMode == EditingMode::Select)
			return false;
		if (editingMode == EditingMode::Cut) {
			if (mousePressEvent.button == GLFW_MOUSE_BUTTON_LEFT && m_CurrentVertexIndex.has_value()) { // Vertex is under the cursor
				if (m_SelectedVertexIndex.has_value()) { // Second click
					if (m_SelectedVertexIndex == m_CurrentVertexIndex) { // Clicking the same vertex again, cancel selection
						m_SelectedVertexHighlight->DisableRendering();
						m_SelectedVertexIndex = std::nullopt;
						return true;
					}

					ft::Mesh* mesh = mainModule->GetSelectedMesh();
					ft::MeshData* data = mesh->GetData();
					MeshUtil::CutFace(*data, m_CuttingFaceIndex, *m_SelectedVertexIndex, *m_CurrentVertexIndex);
					mesh->BakeToRenderMesh();
					ft::MeshData* selectionData = mainModule->selectionMesh->GetData();
					mainModule->ClearSelectionMesh();
					mainModule->SetSelectedID(0);
					historyModule->CommitAction();
					editingMode = EditingMode::Select;
					m_CurrentVertexHighlight->DisableRendering();
					m_CurrentVertexIndex = std::nullopt;
					m_SelectedVertexHighlight->DisableRendering();
					m_SelectedVertexIndex = std::nullopt;
					m_CuttingFaceIndex = 0;
					m_MouseStartIndicator->DisableRender();
				}
				else { // First click
					m_SelectedVertexIndex = m_CurrentVertexIndex;
					m_SelectedVertexHighlight->transform.position = m_CurrentVertexHighlight->transform.position;
					m_SelectedVertexHighlight->CalculateModelMatrix();
					m_SelectedVertexHighlight->EnableRendering();
					m_CurrentVertexHighlight->DisableRendering();
				}
			}
			else if (mousePressEvent.button == GLFW_MOUSE_BUTTON_RIGHT) {
				// Cancel the cut action
				SetEditingModeAndRevertChanges(EditingMode::Select);
			}
			return true;
		}
		if (mousePressEvent.button == GLFW_MOUSE_BUTTON_LEFT) {
			// Selected mesh already has the tranformation applied to it, so we can just exit the editing mode here
			ConfirmEdit();
			return true;
		}
		else if (mousePressEvent.button == GLFW_MOUSE_BUTTON_RIGHT) {
			// This will revert the mesh to its original state
			SetEditingModeAndRevertChanges(EditingMode::Select);
			return true;
		}
		
	}
	return false;
}

bool MeshEditingModule::HandleNumericInput(const int key, const int mods)
{
	if (key >= GLFW_KEY_0 && key <= GLFW_KEY_9) {
		m_NumericInput.active = true;
		m_NumericInput.text += key - GLFW_KEY_0 + '0';
		return true;
	}
	//else if (key >= GLFW_KEY_KP_0 && key <= GLFW_KEY_KP_9) {
	//	m_NumericInput.active = true;
	//	m_NumericInput.text += key - GLFW_KEY_KP_0 + '0';
	//	return true;
	//}
	else if (key == GLFW_KEY_BACKSPACE) {
		if (mods & GLFW_MOD_CONTROL)
			m_NumericInput.Clear();
		else {
			if (m_NumericInput.text.size() > 0)
				m_NumericInput.text.pop_back();
			if (m_NumericInput.text.size() == 0)
				m_NumericInput.active = false;
		}
		return true;
	}
	else if (key == GLFW_KEY_MINUS || key == GLFW_KEY_KP_SUBTRACT) {
		m_NumericInput.negative = !m_NumericInput.negative;
		return true;
	}
	else if (key == GLFW_KEY_PERIOD || key == GLFW_KEY_COMMA) {
		m_NumericInput.active = true;
		m_NumericInput.text += '.';
		return true;
	}
}

void MeshEditingModule::ConfirmEdit()
{
	editingMode = EditingMode::Select;
	m_LockVector = { 0, 0, 0 };
	UpdateLockAxisMesh();
	m_NumericInput.Clear();
	m_MouseStartIndicator->DisableRender();
	if (mainModule->selectionMode == SelectionMode::Face && mainModule->selectedFaces.size() != 0) {
		ft::MeshData* selectionData = mainModule->selectionMesh->GetData();
		MeshUtil::CreateFromFaces(*mainModule->GetSelectedMesh()->GetData(), *selectionData, mainModule->selectedFaces);
		mainModule->selectionMesh->BakeToRenderMesh();
	}
	historyModule->CommitAction();
}

void MeshEditingModule::SetEditingModeAndRevertChanges(EditingMode newMode) 
{
	if (mainModule->selectedMeshID == 0)
		return;

	m_LockVector = { 0, 0, 0 };
	UpdateLockAxisMesh();

	// Revert changes if switching modes
	if (editingMode != EditingMode::Select) {
		if (mainModule->selectionMode == SelectionMode::Object) {
			mainModule->GetSelectedMesh()->transform = m_OriginalTransform;
			mainModule->GetSelectedMesh()->CalculateModelMatrix();
		}
		else if (editingMode == EditingMode::Cut) {
			m_CurrentVertexHighlight->DisableRendering();
			m_CurrentVertexIndex = std::nullopt;
			m_SelectedVertexHighlight->DisableRendering();
			m_SelectedVertexIndex = std::nullopt;
			m_CuttingFaceIndex = 0;
		}
		else {
			mainModule->GetSelectedMesh()->SetData(m_OriginalMeshData);
			if (mainModule->selectedFaces.size() != 0) {
				ft::MeshData* selectionData = mainModule->selectionMesh->GetData();
				MeshUtil::CreateFromFaces(*mainModule->GetSelectedMesh()->GetData(), *selectionData, mainModule->selectedFaces);
				mainModule->selectionMesh->BakeToRenderMesh();
			}
		}
		m_NumericInput.Clear();
		historyModule->CancelAction();
		m_MouseStartIndicator->DisableRender();
	}

	if (editingMode == newMode || newMode == EditingMode::Select)
		editingMode = EditingMode::Select;
	else {
		editingMode = newMode;
		if (mainModule->selectionMode == SelectionMode::Object) {
			m_OriginalTransform = mainModule->GetSelectedMesh()->transform;
		}
		else {
			m_OriginalMeshData = *mainModule->GetSelectedMesh()->GetData(); // Save current state
		}
		m_StartMousePos = ft::Input::GetMousePosition();
		if (newMode != EditingMode::Cut) {
			m_MouseStartIndicator->SetPosition(m_StartMousePos - m_MouseStartIndicator->GetRect().Size() / 2.0f);
			m_MouseStartIndicator->EnableRender();
		}

		glm::vec3 toObject = mainModule->GetSelectedMesh()->transform.position - ft::Renderer3D::GetCamera()->GetPosition();
		m_StartObjectDepth = glm::dot(toObject, ft::Renderer3D::GetCamera()->GetFront());

		historyModule->BeginAction(GetActionNameForMode(newMode));
		historyModule->CaptureMeshState(mainModule->selectedMeshID, mainModule->selectionMode == SelectionMode::Face);
	}
}

void MeshEditingModule::UpdateLockAxisMesh()
{
	if (glm::length(m_LockVector) < 0.0001f) {
		if (m_LockAxisMesh) {
			ft::Renderer3D::RemoveMesh(m_LockAxisMesh->GetID());
			m_LockAxisMesh = nullptr;
		}
		return;
	}
	if (mainModule->selectedMeshID == 0)
		return;
	if (glm::length(m_LockVector) > 0.0001f) {
		if (m_LockAxisMesh) {
			ft::Renderer3D::RemoveMesh(m_LockAxisMesh->GetID());
			m_LockAxisMesh = nullptr;
		}
		glm::vec3 position = m_OriginalTransform.position;
		glm::vec4 color = glm::vec4(m_LockVector, 1.0f);
		m_LockAxisMesh = ft::Renderer3D::AddMesh(ft::Mesh::CreateLine(position - m_LockVector * 100.0f, position + m_LockVector * 100.0f, color, false));
	}
}

std::string MeshEditingModule::GetActionNameForMode(EditingMode mode)
{
	switch (mode) {
	case EditingMode::Move:
		return "Move";
	case EditingMode::Rotate:
		return "Rotate";
	case EditingMode::Scale:
		return "Scale";
	case EditingMode::Extrude:
		return "Extrude";
	case EditingMode::Cut:
		return "Cut";
	default:
		return "Edit";
	}
}
