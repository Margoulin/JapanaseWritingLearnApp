#include "Database.h"

#include "JapaneseTypes.h"

#include <sqlite3.h>
#include <String.hpp>

struct KanjisFillData
{
	Kanji** kanjis = nullptr;
	uint8_t count = 0;
};

auto	Database::OpenDatabaseFile(const char* filename) -> void
{
	if (initialized)
		CloseDatabase();
	int res = sqlite3_open(filename, &database);
	initialized = res == 0;
}

auto	Database::CloseDatabase() -> void
{
	if (initialized)
	{
		sqlite3_close(database);
		initialized = false;
	}
}

#include <iostream>

auto	Database::AddNewKanji(Kanji* kanji) -> void
{
	MString kanjiStr = MString::FromUInt((unsigned int)kanji->GetKanji());
	MString kunyomiStr = kanji->SerializeKunyomi();
	MString onyomiStr = kanji->SerializeOnyomi();
	MString insertKanjiRequest("INSERT INTO KANJI VALUES(" + kanjiStr + ",'" + kunyomiStr + "', '" + onyomiStr + "');");
	char* msgError = nullptr;
	int result = sqlite3_exec(database, insertKanjiRequest.Str(), nullptr, nullptr, &msgError);
	if (result != SQLITE_OK)
	{
		std::cout << "Err : " << msgError << std::endl;
		sqlite3_free(msgError);
	}
	expandKanjiArray();
	kanjis[kanjiCount] = kanji;
	kanjiCount++;
}

auto	Database::LoadKanji() -> void
{
	if (!initialized)
		return;
	MString getKanjiCountRequest("SELECT COUNT(*) FROM KANJI");
	int result = sqlite3_exec(database, getKanjiCountRequest.Str(), GetKanjiCountCallback, 
		(void*)&kanjiCount, nullptr);
	kanjis = new Kanji*[kanjiCount];
	MString getKanjisRq("SELECT * FROM KANJI;");
	KanjisFillData data;
	data.kanjis = kanjis;
	result = sqlite3_exec(database, getKanjisRq.Str(), GetKanjisCallback, 
		(void*)&data, nullptr);
}

static int GetKanjiCountCallback(void* data, int argc, char** argv, char** azColName)
{
	uint8_t* outCount = (uint8_t*)data;
	*outCount = std::stoi(argv[0]);
	return 0;
}

static int GetKanjisCallback(void* data, int argc, char** argv, char** azColName)
{
	KanjisFillData* kanjiData = (KanjisFillData*)data;

	wchar_t kanji = L' ';
	MString kun(true);
	MString on(true);

	for (unsigned int i = 0; i < argc; i++)
	{
			printf("%s = %s\n", azColName[i], argv[i] ? argv[i] : "NULL");
		if (strcmp(azColName[i], "Kanji") == 0)
		{
			kanji = std::stoi(argv[0]);
		}
		else if (strcmp(azColName[i], "Kunyomi") == 0)
		{
			if (argv[i])
				kun = argv[i];
		}
		else if (strcmp(azColName[i], "Onyomi") == 0)
		{
			if (argv[i])
				on = argv[i];
		}
	}
	kanjiData->kanjis[kanjiData->count] = new Kanji(kanji);
	if (kun.Count() > 0)
		kanjiData->kanjis[kanjiData->count]->DeserializeKunyomiFromJson(kun);
	if (on.Count() > 0)
		kanjiData->kanjis[kanjiData->count]->DeserializeOnyomiFromJson(on);
	kanjiData->count++;
	return 0;
}
	
auto	Database::expandKanjiArray() -> void
{
	auto* tempArray = new Kanji*[kanjiCount + 1];
	for (unsigned int pos = 0; pos < kanjiCount; pos++)
		tempArray[pos] = kanjis[pos];
	delete[] kanjis;
	kanjis = tempArray;
}

#include <imgui.h>

char kanjiBuffer[32]{"\0"};
Kanji* tempKanji = nullptr;
char yomiBuffer[32]{"\0"};
int yomiIdx = 0;
int tempKunyomiCount = 0;
int tempOnyomiCount = 0;
bool isEditingKunyomi = true;

auto	Database::ImGuiUpdate() -> void
{
	if (ImGui::CollapsingHeader("Kanjis"))
	{
		ImGui::Indent();
		for (unsigned int pos = 0; pos < kanjiCount; pos++)
			kanjis[pos]->ImGuiUpdate();
		ImGui::Unindent();
	}
	ImGui::Spacing();

	if (ImGui::CollapsingHeader("Add new Kanji"))
	{
		ImGui::Indent();
		if (tempKanji == nullptr)
		{
			ImGui::InputText("Kanji", kanjiBuffer, 32);
			ImGui::InputInt("Kunyomi Count", &tempKunyomiCount);
			ImGui::InputInt("Onyomi Count", &tempOnyomiCount);
			if (ImGui::Button("Create data"))
			{
				tempKanji = new Kanji(MWString::FromUTF8(kanjiBuffer)[0], tempKunyomiCount, tempOnyomiCount);
				tempKunyomiCount = 0;
				tempOnyomiCount = 0;
			}
		}
		else
		{
			tempKanji->ImGuiUpdate();
			if (ImGui::RadioButton("Editing Kunyomi", isEditingKunyomi))
				isEditingKunyomi = !isEditingKunyomi;
			ImGui::InputInt(isEditingKunyomi?"Kunyomi Idx":"Onyomi Idx", &yomiIdx);
			ImGui::InputText(isEditingKunyomi?"Kunyomi" : "Onyomi", yomiBuffer, 32);
			if (ImGui::Button(isEditingKunyomi ? "Set Kunyomi" : "Set Onyomi"))
			{
				if (isEditingKunyomi)
					tempKanji->SetKunyomi(yomiIdx, MWString::FromUTF8(yomiBuffer));
				else	
					tempKanji->SetOnyomi(yomiIdx, MWString::FromUTF8(yomiBuffer));
			}
			if (ImGui::Button("Add To Database"))
			{
				AddNewKanji(tempKanji);

				tempKanji = nullptr;
				yomiIdx = 0;
			}
		}
		ImGui::Unindent();
	}
}