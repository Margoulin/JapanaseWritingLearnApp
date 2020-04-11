#ifndef __REVISIONSCENE_HPP__
#define __REVISIONSCENE_HPP__

#include "Scene.h"

#include <vector>

class Kanji;

class RevisionScene : public Scene
{
public:
	RevisionScene() = delete;
	RevisionScene(const RevisionScene&) = delete;
	RevisionScene(RevisionScene&&) = delete;
	~RevisionScene() = default;

	virtual auto Initialize() -> void;
	virtual auto Shutdown() -> void;

	virtual auto	Update() -> void;
	virtual auto	Render(D3D11Renderer*) -> void;

	auto	operator = (const RevisionScene&)->RevisionScene & = delete;
	auto	operator = (RevisionScene&&)->RevisionScene & = delete;

protected:

private:
	auto	validatekanji() -> void;
	auto	markKanjiAsWrong() -> void;
	auto	returnMainMenu();

private:
	std::vector<Kanji*>	kanjiList;
};


#endif /*__REVISIONSCENE_HPP__*/