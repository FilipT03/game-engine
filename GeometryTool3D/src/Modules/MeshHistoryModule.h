#pragma once

#include<GameEngine.h>
#include<deque>
#include<unordered_set>

class MainModule;
class MeshEditingModule;
struct MeshSnapshot {
    uint32_t meshID;
	std::optional<ft::Transform3D> transform;
    std::optional<ft::MeshData> data;
    glm::vec4 color;
	bool exists = true;
};

struct HistoryAction {
    std::string actionName;
    std::vector<MeshSnapshot> snapshots;
	std::unordered_set<uint32_t> capturedMeshes;

	HistoryAction(const std::string& name) : actionName(name) {}
};

class MeshHistoryModule : public ft::Module
{
public:
	void OnRegister() {};
	void OnDelete() {};
	void OnUpdate() {};
	bool OnEvent(const ft::Event& event) { return false; };
	bool OnKeyEvent(const ft::KeyEvent& event);
	bool OnMouseEvent(const ft::MouseEvent& event) { return false; };

	void BeginAction(const std::string& actionName);
	void CaptureMeshState(uint32_t meshID, bool captureData = true, bool nonexistantState = false);
	void CommitAction();
	void CancelAction();
	void RecordMeshAction(const std::string& actionName, uint32_t meshID, bool captureData = true, bool nonexistantState = false);
	void Undo();
	void Redo();
	void ApplySnapshot(const MeshSnapshot& snapshot);

	MainModule* mainModule;
	MeshEditingModule* editingModule;
	
private:
	std::optional<MeshSnapshot> CreateMeshSnapshot(uint32_t meshID, bool captureData = true, bool nonexistantState = false) const;

	std::deque<HistoryAction> m_UndoStack;
	std::deque<HistoryAction> m_RedoStack;

	const size_t MAX_UNDO_STEPS = 50;

	std::unique_ptr<HistoryAction> m_CurrentAction;
};