#include "MeshEditingModule.h"
#include "MainModule.h"
#include "../MeshUtil.h"

#include <glm/gtc/matrix_inverse.hpp>


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

	m_MouseStartIndicator = ft::UI::AddElement<ft::UIShape>(ft::ShapeType::Ellipse, ft::Rect(0, 0, 5, 5), glm::vec4(1.0f, 0.1f, 0.1f, 0.65f));
	ft::Ellipse* circle = static_cast<ft::Ellipse*>(m_MouseStartIndicator->GetShape());
	circle->AA = 0.7f;
	m_MouseStartIndicator->DisableRender();

	ft::Mesh lineX = ft::Mesh::CreateLine(ft::Vector::Zero, ft::Vector::One, glm::vec4(ft::Vector::X, 1.0f), true);
	ft::Mesh lineY = ft::Mesh::CreateLine(ft::Vector::Zero, ft::Vector::One, glm::vec4(ft::Vector::Y, 1.0f), true);
	ft::Mesh lineZ = ft::Mesh::CreateLine(ft::Vector::Zero, ft::Vector::One, glm::vec4(ft::Vector::Z, 1.0f), true);
	//lineX.renderOrder = 10;
	//lineY.renderOrder = 10;
	//lineZ.renderOrder = 10;
	//lineX.alwaysOnTop = true;
	//lineY.alwaysOnTop = true;
	//lineZ.alwaysOnTop = true;
	m_LockAxisMeshX = ft::Renderer3D::AddMesh(std::move(lineX));
	m_LockAxisMeshY = ft::Renderer3D::AddMesh(std::move(lineY));
	m_LockAxisMeshZ = ft::Renderer3D::AddMesh(std::move(lineZ));
	m_LockAxisMeshX->DisableRendering();
	m_LockAxisMeshY->DisableRendering();
	m_LockAxisMeshZ->DisableRendering();

	UpdateLockAxisMesh();
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

	glm::vec3 objectDelta;
	glm::vec3 numericDelta;
	glm::vec2 mouseDelta;
	float numericValue;
	if (!m_NumericInput.active) {
		glm::vec2 windowSize = ft::Application::Get().GetWindow().GetWindowSize();
		glm::vec2 mousePos = ft::Input::GetMousePosition();
		mouseDelta = mousePos - m_StartMousePos;
		glm::vec2 normalizedDelta = mouseDelta / windowSize;
		objectDelta = ft::Renderer3D::ScreenDeltaToWorld(mousePos, mouseDelta, m_StartObjectDepth);
		numericValue = glm::length(normalizedDelta) * glm::sign(mouseDelta.x);
		if (m_ConstraintFrame.IsActive()) {
			objectDelta = m_ConstraintFrame.ProjectLocalConstrained(objectDelta);
			objectDelta = m_ConstraintFrame.ToWorld(objectDelta);
		}
	}
	else
		numericValue = m_NumericInput.GetValue();

	if (m_ConstraintFrame.IsActive())
		numericDelta = m_ConstraintFrame.GetConstrainedVector() * numericValue;
	else
		numericDelta = numericValue * ft::Vector::Right;

	if (m_NumericInput.active)
		objectDelta = numericDelta;

	ft::Mesh* selectedMesh = mainModule->GetSelectedMesh();
	if (mainModule->selectionMode == SelectionMode::Face) {
		selectedMesh->SetData(m_OriginalMeshData);
		ft::MeshData* selectedMeshData = selectedMesh->GetData();
		if (selectedMeshData->UsesCustomNormals() && editingMode != EditingMode::Select) {
			selectedMeshData->SetUsesCustomNormals(false);
			selectedMeshData->SetSmoothingMode(selectedMeshData->GetSmoothingMode());
		}
		// Convert to local space delta
		objectDelta = glm::inverse(glm::mat3(m_OriginalTransform.GetMatrix())) * objectDelta;
	}

	switch (editingMode) {
	case EditingMode::Move:
		Move(objectDelta);
		break;
	case EditingMode::Rotate:
		Rotate(numericValue);
		break;
	case EditingMode::Scale:
		Scale(numericValue);
		break;
	case EditingMode::Extrude:
		Extrude(objectDelta);
		break;
	}
		

	if (mainModule->selectionMode == SelectionMode::Object)
		selectedMesh->UpdateModelMatrix();
	else {
		ft::MeshData* selectedMeshData = selectedMesh->GetData();
		selectedMesh->BakeToRenderMesh();
		ft::MeshData* selectionData = mainModule->selectionMesh->GetData();
		MeshUtil::CreateFromFaces(*selectedMeshData, *selectionData, mainModule->selectedFaces);
		mainModule->selectionMesh->BakeToRenderMesh();
	}
}


void MeshEditingModule::Move(glm::vec3& objectDelta)
{
	ft::Mesh* selectedMesh = mainModule->GetSelectedMesh();
	ft::MeshData* selectedMeshData = selectedMesh->GetData();
	if (mainModule->selectionMode == SelectionMode::Object)
		selectedMesh->transform.position = m_OriginalTransform.position + objectDelta;
	else
		MeshUtil::MoveFaces(*selectedMeshData, mainModule->selectedFaces, objectDelta);
}

void MeshEditingModule::Rotate(float numericValue)
{
	ft::Mesh* selectedMesh = mainModule->GetSelectedMesh();
	ft::MeshData* selectedMeshData = selectedMesh->GetData();

	float angle = glm::radians(m_NumericInput.active ? numericValue : numericValue * 720.0f);

	glm::vec3 axis = m_OriginalCameraFront;
	if (m_ConstraintFrame.IsActive())
		axis = m_ConstraintFrame.OneAxisActive()
		? m_ConstraintFrame.GetConstrainedVector()
		: m_ConstraintFrame.GetUnconstrainedVector();
	axis = glm::normalize(axis);

	glm::quat worldRot = glm::angleAxis(angle, axis);

	if (mainModule->selectionMode == SelectionMode::Object) {
		glm::quat q(glm::radians(m_OriginalTransform.rotation));
		q = worldRot * q;
		selectedMesh->transform.rotation = glm::degrees(glm::eulerAngles(q));
	}
	else {
		glm::mat4 L(glm::mat3(m_OriginalTransform.GetMatrix()));
		glm::mat4 localRot = glm::inverse(L) * glm::mat4_cast(worldRot) * L;
		MeshUtil::RotateFaces(*selectedMeshData, mainModule->selectedFaces, localRot);
	}
}

void MeshEditingModule::Scale(float numericValue)
{
	ft::Mesh* selectedMesh = mainModule->GetSelectedMesh();
	ft::MeshData* selectedMeshData = selectedMesh->GetData();

	const float threshold = 0.1f;
	float valueTemp = std::abs(numericValue);
	if (!m_NumericInput.active) {
		if (valueTemp < threshold) {
			// Map [0,threshold) to [0.1-1)
			valueTemp = (valueTemp / threshold) * 0.9f + 0.1f;
		}
		else {
			// Map [threshold,X] to [1,(X/t)*3-2]
			valueTemp = (valueTemp / threshold) * 2.0f - 1.0f;
		}
		numericValue = valueTemp * glm::sign(threshold);
	}

	if (mainModule->selectionMode == SelectionMode::Object) {
		glm::vec3 multiplier{ numericValue };
		if (m_ConstraintFrame.IsActive()) {
			glm::mat3 R(m_OriginalTransform.GetRotationMatrix()); // columns are local axes in world
			glm::vec3 weight(0.0f);
			for (int i = 0; i < 3; i++) {
				const glm::vec3 axis = R[i];
				if (m_ConstraintFrame.activeX) weight[i] += glm::pow(glm::dot(axis, m_ConstraintFrame.GetAxisX()), 2.0f);
				if (m_ConstraintFrame.activeY) weight[i] += glm::pow(glm::dot(axis, m_ConstraintFrame.GetAxisY()), 2.0f);
				if (m_ConstraintFrame.activeZ) weight[i] += glm::pow(glm::dot(axis, m_ConstraintFrame.GetAxisZ()), 2.0f);
			}
			multiplier = glm::vec3(1.0f) + (numericValue - 1.0f) * weight;
		}
		selectedMesh->transform.scale = m_OriginalTransform.scale * multiplier;
	}
	else {
		glm::mat3 objectToWorld = m_OriginalTransform.GetMatrix();
		MeshUtil::ScaleFaces(*selectedMeshData, mainModule->selectedFaces, numericValue, m_ConstraintFrame, objectToWorld);
	}
}

void MeshEditingModule::Extrude(glm::vec3& objectDelta)
{
	if (mainModule->selectionMode == SelectionMode::Object)
		return;
	ft::Mesh* selectedMesh = mainModule->GetSelectedMesh();
	ft::MeshData* selectedMeshData = selectedMesh->GetData();
	MeshUtil::ExtrudeFaces(*selectedMeshData, mainModule->selectedFaces, objectDelta);
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
			m_CurrentVertexHighlight->UpdateModelMatrix();
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
		if (mainModule->selectedMeshID == 0)
			return false;
		switch (event.key)
		{
		case GLFW_KEY_X:
			if (editingMode == EditingMode::Select || editingMode == EditingMode::Cut)
				break;
			BuildConstraintFrame(ConstraintAxis::X, event.mods & GLFW_MOD_SHIFT);
			UpdateLockAxisMesh();
			break;
		case GLFW_KEY_Y:
			if (editingMode == EditingMode::Select || editingMode == EditingMode::Cut)
				break;
			BuildConstraintFrame(ConstraintAxis::Y, event.mods & GLFW_MOD_SHIFT);
			UpdateLockAxisMesh();
			break;
		case GLFW_KEY_Z:
			if (editingMode == EditingMode::Select || editingMode == EditingMode::Cut)
				break;
			BuildConstraintFrame(ConstraintAxis::Z, event.mods & GLFW_MOD_SHIFT);
			UpdateLockAxisMesh();
			break;
		default:
			break;
		}

		if (event.mods & GLFW_MOD_SHIFT)
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
					m_SelectedVertexHighlight->UpdateModelMatrix();
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

void MeshEditingModule::BuildConstraintFrame(ConstraintAxis input, bool extend)
{
	if (mainModule->selectedMeshID == 0)
		return;

	const ConstraintMode secondMode =
		mainModule->selectionMode == SelectionMode::Object
		? ConstraintMode::Local
		: ConstraintMode::Normal;

	// Cycle current mode
	if (m_ConstraintFrame.IsActive() && m_ConstraintFrame.CheckConstraint(input)) {
		if (m_ConstraintFrame.mode == ConstraintMode::World)
			m_ConstraintFrame.mode = secondMode;
		else {
			m_ConstraintFrame.ClearActive();
			m_ConstraintFrame.mode = ConstraintMode::None;
		}
	} 
	else {
		bool keepExisting = extend && m_ConstraintFrame.OneAxisActive();
		if (!keepExisting) {
			m_ConstraintFrame.ClearActive();
			m_ConstraintFrame.mode = ConstraintMode::World;
		}
		m_ConstraintFrame.SetConstraint(input);
	}

	switch (m_ConstraintFrame.mode) {
	case ConstraintMode::World:
		m_ConstraintFrame.SetCardinal();
		break;
	case ConstraintMode::Local:
		m_ConstraintFrame.SetFromTransform(m_OriginalTransform);
		break;
	case ConstraintMode::Normal: {
		glm::mat3 M = glm::mat3(m_OriginalTransform.GetMatrix());
		glm::mat3 normalMatrix = glm::inverseTranspose(M);
		glm::vec3 nObj = MeshUtil::GetAverageNormal(m_OriginalMeshData, mainModule->selectedFaces);
		glm::vec3 tObj = MeshUtil::GetTangent(m_OriginalMeshData, mainModule->selectedFaces, nObj, ft::Vector::X);
		if (glm::dot(tObj, tObj) < 1e-12f)
			tObj = ft::Vector::X;

		glm::vec3 normal = normalMatrix * nObj;
		glm::vec3 tangent = M * tObj;

		if (!m_ConstraintFrame.CalculateFromXZ(tangent, normal, ConstraintAxis::Z))
			m_ConstraintFrame.SetCardinal();
		break;
	}
	default:
		break;
	}
}

void MeshEditingModule::ConfirmEdit()
{
	editingMode = EditingMode::Select;
	m_ConstraintFrame.Clear();
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

	m_ConstraintFrame.Clear();
	UpdateLockAxisMesh();

	// Revert changes if switching modes
	if (editingMode != EditingMode::Select) {
		if (mainModule->selectionMode == SelectionMode::Object) {
			mainModule->GetSelectedMesh()->transform = m_OriginalTransform;
			mainModule->GetSelectedMesh()->UpdateModelMatrix();
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
		if (mainModule->selectionMode == SelectionMode::Face)
			m_OriginalMeshData = *mainModule->GetSelectedMesh()->GetData(); // Save current state
		m_OriginalTransform = mainModule->GetSelectedMesh()->transform;
		m_OriginalCameraFront = glm::normalize(ft::Renderer3D::GetCamera()->GetFront());
		m_StartMousePos = ft::Input::GetMousePosition();
		if (newMode != EditingMode::Cut) {
			m_MouseStartIndicator->SetPosition(m_StartMousePos - m_MouseStartIndicator->GetRect().Size() / 2.0f);
			m_MouseStartIndicator->EnableRender();
		}
		if (newMode == EditingMode::Extrude) {
			BuildConstraintFrame(ConstraintAxis::Z, false);
			BuildConstraintFrame(ConstraintAxis::Z, false);
			UpdateLockAxisMesh();
		}

		glm::vec3 toObject = mainModule->GetSelectedMesh()->transform.position - ft::Renderer3D::GetCamera()->GetPosition();
		m_StartObjectDepth = glm::dot(toObject, ft::Renderer3D::GetCamera()->GetFront());

		historyModule->BeginAction(GetActionNameForMode(newMode));
		historyModule->CaptureMeshState(mainModule->selectedMeshID, mainModule->selectionMode == SelectionMode::Face);
	}
}

void MeshEditingModule::UpdateLockAxisMesh()
{
	m_LockAxisMeshX->DisableRendering();
	m_LockAxisMeshY->DisableRendering();
	m_LockAxisMeshZ->DisableRendering();
	
	if (!m_ConstraintFrame.IsActive())
		return;
	if (mainModule->selectedMeshID == 0)
		return;

	glm::vec3 position;
	if (mainModule->selectionMode == SelectionMode::Object)
		position = m_OriginalTransform.position;
	else {
		glm::vec3 centroid = MeshUtil::GetCentroid(m_OriginalMeshData, mainModule->selectedFaces);
		position = m_OriginalTransform.TransformPoint(centroid);
	}
	if (m_ConstraintFrame.activeX) {
		ft::MeshData* dataX = m_LockAxisMeshX->GetData();
		glm::vec3 axisX = m_ConstraintFrame.GetAxisX();
		dataX->positions = { position - axisX * 100.0f, position + axisX * 100.0f };
		m_LockAxisMeshX->EnableRendering();
	}
	if (m_ConstraintFrame.activeY) {
		ft::MeshData* dataY = m_LockAxisMeshY->GetData();
		glm::vec3 axisY = m_ConstraintFrame.GetAxisY();
		dataY->positions = { position - axisY * 100.0f, position + axisY * 100.0f };
		m_LockAxisMeshY->EnableRendering();
	}
	if (m_ConstraintFrame.activeZ) {
		ft::MeshData* dataZ = m_LockAxisMeshZ->GetData();
		glm::vec3 axisZ = m_ConstraintFrame.GetAxisZ();
		dataZ->positions = { position - axisZ * 100.0f, position + axisZ * 100.0f };
		m_LockAxisMeshZ->EnableRendering();
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
