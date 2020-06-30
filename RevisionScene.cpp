#include "RevisionScene.h"

#include "D3D11Renderer.h"
#include "JapaneseTypes.h"
#include "Application.h"
#include "Database.h"
#include "MainMenuScene.h"

auto	RevisionScene::Initialize() -> void
{
	Database* db = Application::Instance->GetDatabase();
	std::vector<Kanji*> last = db->Review_GetLast20Kanjis();
	std::vector<Kanji*> failed = db->Review_GetFailedKanjis();
	std::vector<Kanji*> old = db->Review_GetOldKanjis();

	for (auto&& l : last)
		kanjiList.push_back(l);
	for (auto&& fail : failed)
		kanjiList.push_back(fail);
	for (auto&& o : old)
		kanjiList.push_back(o);
}

auto	RevisionScene::Shutdown() -> void
{
	for (auto&& kanji : kanjiList)
		delete kanji;
}

auto	RevisionScene::Update() -> void
{
}

auto	RevisionScene::Render(D3D11Renderer* rend) -> void
{
	if (showSolution)
	{
		MWString wideKanji = MWString(kanjiList[currentKanjiIndex]->GetKanjiStr());
		rend->DrawString(wideKanji.ToUTF8String(), Vector2F(250.0f, 70.0f), Vector4F::one, 3.0f);
	}
	rend->DrawString("Revision Scene", Vector2F(250.0f, 40.0f), ColorWhite, 0.5f);
}

#include <imgui.h>

auto	RevisionScene::ImGuiUpdate() -> void
{
	ImGui::Begin("Revision Session");
	ImGui::SetWindowSize({ 700.0f, 460.0f });
	ImGui::SetWindowPos({ 0.0f, 260.0f });

	if (revisionStarted)
	{
		ImGui::Text("Kanji %d/%d", currentKanjiIndex + 1, kanjiList.size());
	
		if (!currentKanjiRevised)
		{
			if (kanjiList.size() > 0 && currentKanjiIndex < kanjiList.size() && kanjiList[currentKanjiIndex])
				kanjiImGui();
			if (showSolution)
				kanjiList[currentKanjiIndex]->ImGuiUpdateOpen();
		}
		else
		{
			if (currentKanjiIndex == kanjiList.size() - 1)
			{ 
				showSolution = false;
				resultImGui();
			}
		}
	}

	ImGui::End();

	ImGui::Begin("Revision Command");

	ImGui::SetWindowSize({200.0f, 260.0f});
	ImGui::SetWindowPos({0.0f, 0.0f});

	if (revisionStarted)
	{
		if (!currentKanjiRevised)
		{
			if (showSolution)
			{
				if (ImGui::Button("Validate", { 200.0f, 50.0f }))
					validatekanji();
				if (ImGui::Button("Negate", { 200.0f, 50.0f }))
					markKanjiAsWrong();
			}
			else
			{
				if (ImGui::Button("Show Solution", { 200.0f, 50.0f }))
					showSolution = true;
			}
		}
		else
		{
			if (currentKanjiIndex == kanjiList.size() - 1)
			{
				showSolution = false;
				if (ImGui::Button("End Session", { 200.0f, 50.0f }))
				{
					returnMainMenu();
				}
			}
		}
	}
	else
	{
		if (ImGui::Button("Start Revision", { 200.0f, 50.0f }))
			startRevision();
	}

	ImGui::End();
}

auto	RevisionScene::validatekanji() -> void
{
	Database* db = Application::Instance->GetDatabase();
	db->Review_KanjiAddCorrect(kanjiList[currentKanjiIndex]);
	if (currentKanjiIndex == kanjiList.size() - 1)
		currentKanjiRevised = true;
	else
	{
		showSolution = false;
		currentKanjiIndex++;
	}
}

auto	RevisionScene::markKanjiAsWrong() -> void
{
	Database* db = Application::Instance->GetDatabase();
	db->Review_KanjiAddWrong(kanjiList[currentKanjiIndex]);
	if (currentKanjiIndex == kanjiList.size() - 1)
		currentKanjiRevised = true;
	else
	{
		showSolution = false;
		currentKanjiIndex++;
	}
}

auto	RevisionScene::returnMainMenu() -> void
{
	Application::Instance->LoadScene(new MainMenuScene());
}

auto	RevisionScene::startRevision() -> void
{
	revisionStarted = true;
}

auto	RevisionScene::kanjiImGui() -> void
{
	MString kunTr = kanjiList[currentKanjiIndex]->GetKunyomiTranslation();
	MString onTr = kanjiList[currentKanjiIndex]->GetOnyomiTranslation();

	std::vector<MString>	kunList;
	bool couldSplit = false;
	do
	{
		MString left, right;
		couldSplit = kunTr.Split(",", left, right);
		if (couldSplit)
		{
			kunList.push_back(left);
			kunTr = right;
		}
	} while (couldSplit);
	kunList.push_back(kunTr);
	if (kunList.size() > 0 && ImGui::CollapsingHeader("Kunyomi", ImGuiTreeNodeFlags_DefaultOpen))
	{
		for (auto& tr : kunList)
			ImGui::Text(tr.Str());
	}
	std::vector<MString>	onList;
	couldSplit = false;
	do
	{
		MString left, right;
		couldSplit = onTr.Split(",", left, right);
		if (couldSplit)
		{
			onList.push_back(left);
			onTr = right;
		}
	} while (couldSplit);
	onList.push_back(onTr);
	if (onList.size() > 0 && ImGui::CollapsingHeader("Onyomi", ImGuiTreeNodeFlags_DefaultOpen))
	{
		for (auto& tr : onList)
			ImGui::Text(tr.Str());
	}
}

auto	RevisionScene::resultImGui() -> void
{
	Database* db = Application::Instance->GetDatabase();
	for (auto& kanji : kanjiList)
	{
		auto ntm = db->Review_KanjiGetReviewStats(kanji);
		ImGui::Text("%s -> Attemps : %d, Percentage : %f", MWString(kanji->GetKanjiStr()).ToUTF8String().Str(), ntm.Attempts, ntm.GetCorrectPercentage());
	}
}