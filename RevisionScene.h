#ifndef __REVISIONSCENE_HPP__
#define __REVISIONSCENE_HPP__

#include "Scene.h"

#include <vector>

class Kanji;

class RevisionScene : public Scene
{
public:
	RevisionScene() = default;
	RevisionScene(const RevisionScene&) = delete;
	RevisionScene(RevisionScene&&) = delete;
	~RevisionScene() = default;

	virtual auto Initialize() -> void override;
	virtual auto Shutdown() -> void override;

	virtual auto	Update() -> void override;
	virtual auto	Render(D3D11Renderer*) -> void override;

	virtual auto	ImGuiUpdate() -> void override;

	auto	operator = (const RevisionScene&)->RevisionScene & = delete;
	auto	operator = (RevisionScene&&)->RevisionScene & = delete;

protected:

private:
	auto	validatekanji() -> void;
	auto	markKanjiAsWrong() -> void;
	auto	returnMainMenu() -> void;

	auto	startRevision() -> void;
	auto	kanjiImGui() -> void;
	auto	resultImGui() -> void;
private:
	std::vector<Kanji*>	kanjiList;

	int		currentKanjiIndex = 0;
	bool	revisionStarted = false;
	bool	currentKanjiRevised = false;
	bool	showSolution = false;
};


#endif /*__REVISIONSCENE_HPP__*/