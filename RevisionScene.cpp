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
		rend->DrawString(wideKanji.ToUTF8String(), Vector2F(500.0f, 300.0f), Vector4F::one, 3.0f);
	}
	rend->DrawString("Revision Scene", Vector2F(550.0f, 100.0f), ColorWhite, 0.5f);
}

#include <imgui.h>

auto	RevisionScene::ImGuiUpdate() -> void
{
	ImGui::Begin("Revision Session");
	ImGui::SetWindowSize({ 600.0f, 200.0f });
	ImGui::SetWindowPos({ 340.0f, 520.0f });

	if (revisionStarted)
	{
		ImGui::Text("Kanji %d/%d", currentKanjiIndex + 1, kanjiList.size());
	
		if (!currentKanjiRevised)
		{
			if (kanjiList.size() > 0 && currentKanjiIndex < kanjiList.size() && kanjiList[currentKanjiIndex])
				kanjiImGui();
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
				resultImGui();
				if (ImGui::Button("End Session", { 200.0f, 50.0f }))
				{
					returnMainMenu();
				}
			}
			else
			{
				if (ImGui::Button("Next", { 200.0f, 50.0f }))
				{
					showSolution = false;
					currentKanjiRevised = false;
					currentKanjiIndex++;
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

#include <iostream>
auto	RevisionScene::validatekanji() -> void
{
	Database* db = Application::Instance->GetDatabase();
	db->Review_KanjiAddCorrect(kanjiList[currentKanjiIndex]);
	std::cout << "oergnoengoenon" << std::endl;
	currentKanjiRevised = true;
}

auto	RevisionScene::markKanjiAsWrong() -> void
{
	Database* db = Application::Instance->GetDatabase();
	db->Review_KanjiAddWrong(kanjiList[currentKanjiIndex]);
	currentKanjiRevised = true;
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
	for (auto& tr : kunList)
	{
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