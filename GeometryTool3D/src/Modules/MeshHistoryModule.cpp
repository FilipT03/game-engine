#include "MeshHistoryModule.h"
#include "MainModule.h"

#include <GLFW/glfw3.h>


bool MeshHistoryModule::OnKeyEvent(const ft::KeyEvent& event)
{
	if (event.type == ft::EventType::KeyPress || event.type == ft::EventType::KeyRepeat) {
		if ((event.mods & GLFW_MOD_CONTROL) && editingModule->editingMode == EditingMode::Select) {
			if (event.key == GLFW_KEY_Z) {
				Undo();
				return true;
			}
			else if (event.key == GLFW_KEY_Y) {
				Redo();
				return true;
			}
		}
	}
	return false;
}

void MeshHistoryModule::BeginAction(const std::string& actionName)
{
	if (m_CurrentAction) {
		FT_WARN("Previous action not committed.");
	}
	m_CurrentAction = std::make_unique<HistoryAction>(actionName);
}

void MeshHistoryModule::CaptureMeshState(uint32_t meshID, bool captureData, bool nonexistantState)
{
	if (!m_CurrentAction) {
		FT_TRACE("No active action to capture mesh state.");
		return;
	}
	if (m_CurrentAction->capturedMeshes.contains(meshID)) {
		return; // Already captured this mesh in current action
	}

	auto snapshotOpt = CreateMeshSnapshot(meshID, captureData, nonexistantState);
	if (snapshotOpt.has_value()) {
		m_CurrentAction->snapshots.push_back(*snapshotOpt);
		m_CurrentAction->capturedMeshes.insert(meshID);
	}
}

void MeshHistoryModule::CommitAction()
{
	if (m_CurrentAction) {
		if (m_CurrentAction->snapshots.empty())
			return;

		m_UndoStack.push_back(std::move(*m_CurrentAction));

		if (m_UndoStack.size() > MAX_UNDO_STEPS)
			m_UndoStack.pop_front();

		m_CurrentAction = nullptr;
		m_RedoStack.clear();
	}
}

void MeshHistoryModule::CancelAction()
{
	m_CurrentAction = nullptr;
}

void MeshHistoryModule::RecordMeshAction(const std::string& actionName, uint32_t meshID, bool captureData, bool nonexistantState)
{
	BeginAction(actionName);
	CaptureMeshState(meshID, captureData, nonexistantState);
	CommitAction();
}

std::optional<MeshSnapshot> MeshHistoryModule::CreateMeshSnapshot(uint32_t meshID, bool captureData, bool nonexistantState) const
{
	if (meshID == 0) return std::nullopt;

	MeshSnapshot snapshot;
	snapshot.meshID = meshID;
	if (!mainModule->meshes.contains(meshID) || nonexistantState) {
		snapshot.transform = std::nullopt;
		snapshot.data = std::nullopt;
		snapshot.color = glm::vec4(1.0f);
		snapshot.exists = false;
	}
	else {
		const ft::Mesh* mesh = mainModule->meshes[meshID];
		snapshot.transform = mesh->transform;
		if (captureData)
			snapshot.data = *mesh->GetData();
		else
			snapshot.data = std::nullopt;
		snapshot.color = mesh->color;
		snapshot.exists = true;
	}

	return snapshot;
}

void MeshHistoryModule::Undo() 
{
    if (m_UndoStack.empty()) {
        FT_TRACE("Nothing to undo.");
        return;
    }

    HistoryAction lastAction = std::move(m_UndoStack.back());
    m_UndoStack.pop_back();

    FT_TRACE("Undoing: {}", lastAction.actionName);

	HistoryAction redoAction(lastAction.actionName);

    for (const auto& snapshot : lastAction.snapshots) {
		bool captureData = snapshot.data.has_value() || !snapshot.exists;
		auto currentSnapshotOpt = CreateMeshSnapshot(snapshot.meshID, captureData);
		if (currentSnapshotOpt.has_value()) {
			redoAction.snapshots.push_back(*currentSnapshotOpt);
		}
		ApplySnapshot(snapshot);
    }

    mainModule->ClearSelectionMesh();

    m_RedoStack.push_back(std::move(redoAction));
}

void MeshHistoryModule::Redo() 
{
	if (m_RedoStack.empty()) {
		FT_TRACE("Nothing to redo.");
		return;
	}
	
	HistoryAction nextAction = std::move(m_RedoStack.back());
	m_RedoStack.pop_back();

	FT_TRACE("Redoing: {}", nextAction.actionName);

	HistoryAction undoAction(nextAction.actionName);

	for (const auto& snapshot : nextAction.snapshots) {
		bool captureData = snapshot.data.has_value() || !snapshot.exists;
		auto currentSnapshotOpt = CreateMeshSnapshot(snapshot.meshID, captureData);
		if (currentSnapshotOpt.has_value()) {
			undoAction.snapshots.push_back(*currentSnapshotOpt);
		}
		ApplySnapshot(snapshot);
	}

	mainModule->ClearSelectionMesh();

	m_UndoStack.push_back(std::move(undoAction));
}

void MeshHistoryModule::ApplySnapshot(const MeshSnapshot& snapshot)
{
	if (mainModule->meshes.contains(snapshot.meshID)) {
		ft::Mesh* mesh = mainModule->meshes[snapshot.meshID];

		if (!snapshot.exists) { // Mesh exist now, but not in snapshot, delete it
			mainModule->RemoveMesh(snapshot.meshID);
		}
		else {
			if (snapshot.transform.has_value()) {
				mesh->transform = *snapshot.transform;
				mesh->UpdateModelMatrix();
			}
			if (snapshot.data.has_value())
				mesh->SetData(*snapshot.data);
			mesh->color = snapshot.color;
		}
	}
	else {
		// Mesh doesn't exist now, but exist in snapshot, create it
		if (!snapshot.exists)
			return;
		ft::Mesh newMesh = ft::Mesh(*snapshot.data, *snapshot.transform, snapshot.color);
		newMesh.SetID(snapshot.meshID);
		mainModule->AddMesh(std::move(newMesh));
	}
}