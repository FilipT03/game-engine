#include "MeshEditingModule.h"
#include "MainModule.h"


void MeshEditingModule::OnUpdate()
{
	if (mainModule->selectedMeshID == 0 || editingMode == EditingMode::Select)
		return;
	ft::Mesh* selectedMesh = mainModule->GetSelectedMesh();
	glm::vec2 mousePos = ft::Input::GetMousePosition();
	glm::vec2 mouseDelta = mousePos - m_StartMousePos;
	glm::vec3 objectDelta = ft::Renderer3D::ScreenDeltaToWorld(mousePos, mouseDelta, m_StartObjectDepth);
	if (glm::length(m_LockVector) > 0.0001f) {
		float projectedDist = glm::dot(objectDelta, m_LockVector);

		// Reconstruct delta only along that axis
		objectDelta = m_LockVector * projectedDist;
		//objectDelta = m_LockVector * glm::length(objectDelta) * glm::sign(objectDelta);
	}

	if (mainModule->selectionMode == SelectionMode::Object) {
		switch (editingMode) {
		case EditingMode::Move:
			selectedMesh->transform.position = m_OriginalTransform.position + objectDelta;
			break;
		case EditingMode::Rotate:
			selectedMesh->transform.rotation = m_OriginalTransform.rotation + objectDelta * 15.0f;
			break;
		case EditingMode::Scale:
			selectedMesh->transform.scale = m_OriginalTransform.scale * (1.0f + objectDelta * 0.2f);
			break;
		}
		selectedMesh->CalculateModelMatrix();
		return;
	}
	else {
		switch (editingMode) {
		case EditingMode::Move:
			break;
		case EditingMode::Rotate:
			break;
		case EditingMode::Scale:
			break;
		case EditingMode::Extrude:
			break;
		}
	}
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
			if (mainModule->selectionMode == SelectionMode::Face)
				break;
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

		case GLFW_KEY_X:
			if (editingMode == EditingMode::Select)
				break;
			if (m_LockVector.x == 1)
				m_LockVector = { 0, 0, 0 };
			else 
				m_LockVector = { 1, 0, 0 };
			UpdateLockAxisMesh();
			break;
		case GLFW_KEY_Y:
			if (editingMode == EditingMode::Select)
				break;
			if (m_LockVector.y == 1)
				m_LockVector = { 0, 0, 0 };
			else
				m_LockVector = { 0, 1, 0 };
			UpdateLockAxisMesh();
			break;
		case GLFW_KEY_Z:
			if (editingMode == EditingMode::Select)
				break;
			if (m_LockVector.z == 1)
				m_LockVector = { 0, 0, 0 };
			else 
				m_LockVector = { 0, 0, 1 };
			UpdateLockAxisMesh();
			break;
		default:
			break;
		}
	}
	return false;
}

bool MeshEditingModule::OnMouseEvent(const ft::MouseEvent& event)
{
	if (event.type == ft::EventType::MousePress) {
		ft::MousePressEvent mousePressEvent = ft::As<ft::MousePressEvent>(event);
		if (mainModule->selectedMeshID == 0 || editingMode == EditingMode::Select)
			return false;
		if (mousePressEvent.button == GLFW_MOUSE_BUTTON_LEFT) {
			// Selected mesh already has the tranformation applied to it, so we can just exit the editing mode here
			editingMode = EditingMode::Select;
			m_LockVector = { 0, 0, 0 };
			UpdateLockAxisMesh();
		}
		else if (mousePressEvent.button == GLFW_MOUSE_BUTTON_RIGHT) {
			// This will revert the mesh to its original state
			SetEditingModeAndRevertChanges(EditingMode::Select);
		}
		
	}
	return false;
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
		else {
			mainModule->GetSelectedMesh()->SetData(m_OriginalMeshData);
		}
	}

	if (editingMode == newMode)
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
		m_StartObjectDepth = glm::length(mainModule->GetSelectedMesh()->transform.position - ft::Renderer3D::GetCamera()->GetPosition());
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
