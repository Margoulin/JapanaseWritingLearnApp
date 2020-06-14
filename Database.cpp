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
	//MString insertKanjiRequest("INSERT INTO KANJI VALUES(" + kanjiStr + ",'" + kunyomiStr + "', '" + onyomiStr + "');");
	MString insertKanjiRequest(
		"INSERT INTO Kanji (Kanji, Kunyomi, Onyomi, KunTranslation, OnTranslation)\
		VALUES(" + kanjiStr + ",'" + kunyomiStr + "', '" + onyomiStr + "', '" + kanji->GetKunyomiTranslation().Str() + "', '" + kanji->GetOnyomiTranslation().Str() + "');");
	MString insertReviewRequest(
		"INSERT INTO LearnSession (UpdateDate)\
		VALUES(DATE('now'));");
	char* msgError = nullptr;
	int result = sqlite3_exec(database, insertKanjiRequest.Str(), nullptr, nullptr, &msgError);
	if (result != SQLITE_OK)
	{
		std::cout << "Err : " << msgError << std::endl;
		sqlite3_free(msgError);
	}
	result = sqlite3_exec(database, insertReviewRequest.Str(), nullptr, nullptr, &msgError);
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
	MString getKanjiCountRequest("SELECT COUNT(*) FROM Kanji");
	int result = sqlite3_exec(database, getKanjiCountRequest.Str(), GetKanjiCountCallback, 
		(void*)&kanjiCount, nullptr);
	kanjis = new Kanji*[kanjiCount];
	MString getKanjisRq("SELECT * FROM Kanji;");
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

static void fillKanji(Kanji** kanjiToFill, int argc, char** argv, char** azColName)
{
	wchar_t kanji = L' ';
	MString kun(true);
	MString on(true);
	MString kunTr(true);
	MString onTr(true);

	for (unsigned int i = 0; i < argc; i++)
	{
		//printf("%s = %s\n", azColName[i], argv[i] ? argv[i] : "NULL");
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
		else if (strcmp(azColName[i], "KunTranslation") == 0)
		{
			if (argv[i])
				kunTr = argv[i];
		}
		else if (strcmp(azColName[i], "OnTranslation") == 0)
		{
			if (argv[i])
				onTr = argv[i];
		}
	}
	(*kanjiToFill) = new Kanji(kanji);
	if (kun.Count() > 0)
		(*kanjiToFill)->DeserializeKunyomiFromJson(kun);
	if (on.Count() > 0)
		(*kanjiToFill)->DeserializeOnyomiFromJson(on);
	if (kunTr.Count() > 0)
		(*kanjiToFill)->SetKunyomiTranslation(kunTr);
	if (onTr.Count() > 0)
		(*kanjiToFill)->SetOnyomiTranslation(onTr);
}

static int GetKanjisCallback(void* data, int argc, char** argv, char** azColName)
{
	KanjisFillData* kanjiData = (KanjisFillData*)data;

	fillKanji(&(kanjiData->kanjis[kanjiData->count]), argc, argv, azColName);
	kanjiData->count++;
	return 0;
}

static int GetKanjisToListCallback(void* data, int argc, char** argv, char** azColName)
{
	std::vector<Kanji*>* vec = (std::vector<Kanji*>*)data;
	for (unsigned int i = 0; i < argc; i++)
	{
		printf("LAST 20 : %s = %s\n", azColName[i], argv[i] ? argv[i] : "NULL");
	}
	Kanji* temp = nullptr;
	fillKanji(&temp, argc, argv, azColName);
	vec->push_back(temp);
	return 0;
}

static int GetKanjisReviewStatsCallback(void* data, int argc, char** argv, char** azColName)
{
	KanjiReviewStats* outStats = (KanjiReviewStats*)data;

	for (unsigned int i = 0; i < argc; i++)
	{
		if (strcmp(azColName[i], "Success") == 0)
			outStats->Correct = std::stoi(argv[i]);
		else if (strcmp(azColName[i], "Fail") == 0)
			outStats->Wrong = std::stoi(argv[i]);
	}
	outStats->Attempts = outStats->Correct + outStats->Wrong;

	return 0;
}

auto	Database::Review_GetLast20Kanjis() -> std::vector<Kanji*>
{
	std::vector<Kanji*> ret;
	if (!initialized)
		return ret;
	MString rq("\
		SELECT * \
		FROM Kanji\
		INNER JOIN LearnSession ON LearnSession.Id = Kanji.Id\
		ORDER BY LearnSession.UpdateDate DESC\
		LIMIT 20");
	char* msgError = nullptr;
	int result = sqlite3_exec(database, rq.Str(), GetKanjisToListCallback, &ret, &msgError);
	if (result != SQLITE_OK)
	{
		std::cout << "Err : " << msgError << std::endl;
		sqlite3_free(msgError);
	}
	return ret;
}

auto	Database::Review_GetFailedKanjis() -> std::vector<Kanji*>
{
	std::vector<Kanji*> ret;
	if (!initialized)
		return ret;
	MString rq("\
		SELECT * \
		FROM Kanji\
		INNER JOIN LearnSession ON LearnSession.Id = Kanji.Id\
		WHERE LearnSession.Success + LearnSession.Fail > 20 AND (LearnSession.Fail / (LearnSession.Success + LearnSession.Fail)) > 0.5\
		ORDER BY LearnSession.UpdateDate ASC");
	char* msgError = nullptr;
	int result = sqlite3_exec(database, rq.Str(), GetKanjisToListCallback, &ret, &msgError);
	if (result != SQLITE_OK)
	{
		std::cout << "Err : " << msgError << std::endl;
		sqlite3_free(msgError);
	}
	return ret;
}

auto	Database::Review_GetOldKanjis() -> std::vector<Kanji*>
{
	std::vector<Kanji*> ret;
	if (!initialized)
		return ret;
	MString rq("\
		SELECT * \
		FROM Kanji\
		INNER JOIN LearnSession ON LearnSession.Id = Kanji.Id\
		WHERE (LearnSession.Success + LearnSession.Fail) > 20 AND \
			(LearnSession.UpdateDate BETWEEN Date('now', '-2 year') AND Date('now', '-14 day')) AND \
			LearnSession.Fail / (LearnSession.Success + LearnSession.Fail) > 0.25");
	char* msgError = nullptr;
	int result = sqlite3_exec(database, rq.Str(), GetKanjisToListCallback, &ret, &msgError);
	if (result != SQLITE_OK)
	{
		std::cout << "Err : " << msgError << std::endl;
		sqlite3_free(msgError);
	}
	MString rq2("\
		SELECT * \
		FROM Kanji\
		INNER JOIN LearnSession ON LearnSession.Id = Kanji.Id\
		WHERE (LearnSession.Success + LearnSession.Fail) > 20 AND \
			(LearnSession.UpdateDate BETWEEN Date('now', '-2 year') AND Date('now', '-1 month')) AND \
			LearnSession.Fail / (LearnSession.Success + LearnSession.Fail) > 0.2");
	msgError = nullptr;
	result = sqlite3_exec(database, rq2.Str(), GetKanjisToListCallback, &ret, &msgError);
	if (result != SQLITE_OK)
	{
		std::cout << "Err : " << msgError << std::endl;
		sqlite3_free(msgError);
	}
	MString rq3("\
		SELECT * \
		FROM Kanji\
		INNER JOIN LearnSession ON LearnSession.Id = Kanji.Id\
		WHERE (LearnSession.Success + LearnSession.Fail) > 20 AND \
			(LearnSession.UpdateDate BETWEEN Date('now', '-2 year') AND Date('now', '-2 month')) AND \
			LearnSession.Fail / (LearnSession.Success + LearnSession.Fail) > 0.1");
	msgError = nullptr;
	result = sqlite3_exec(database, rq3.Str(), GetKanjisToListCallback, &ret, &msgError);
	if (result != SQLITE_OK)
	{
		std::cout << "Err : " << msgError << std::endl;
		sqlite3_free(msgError);
	}
	MString rq4("\
		SELECT * \
		FROM Kanji\
		INNER JOIN LearnSession ON LearnSession.Id = Kanji.Id\
		WHERE (LearnSession.Success + LearnSession.Fail) > 20 AND \
			(LearnSession.UpdateDate BETWEEN Date('now', '-2 year') AND Date('now', '-4 month'))");
	msgError = nullptr;
	result = sqlite3_exec(database, rq4.Str(), GetKanjisToListCallback, &ret, &msgError);
	if (result != SQLITE_OK)
	{
		std::cout << "Err : " << msgError << std::endl;
		sqlite3_free(msgError);
	}
	return ret;
}

auto	Database::Review_KanjiAddCorrect(Kanji* kanji) -> void
{
	unsigned int kanjiChar = (unsigned int)kanji->GetKanji();
	MString rq("UPDATE LearnSession SET Success = Success + 1, UpdateDate = Date('now')\
				WHERE (SELECT id\
				FROM Kanji\
				WHERE Kanji.Kanji = ");
	rq += MString::FromUInt(kanjiChar).Str();
	rq +=		") = LearnSession.ID";
	char* msgError = nullptr;
	int result = sqlite3_exec(database, rq.Str(), nullptr, nullptr, &msgError);
	if (result != SQLITE_OK)
	{
		std::cout << "Err : " << msgError << std::endl;
		sqlite3_free(msgError);
	}
}

auto	Database::Review_KanjiAddWrong(Kanji* kanji) -> void
{
	unsigned int kanjiChar = (unsigned int)kanji->GetKanji();
	MString rq("UPDATE LearnSession SET Fail = Fail + 1, UpdateDate = Date('now')\
				WHERE (SELECT id\
				FROM Kanji\
				WHERE Kanji.Kanji = ");
	rq += MString::FromUInt(kanjiChar).Str();
	rq += ") = LearnSession.ID";
	char* msgError = nullptr;
	int result = sqlite3_exec(database, rq.Str(), nullptr, nullptr, &msgError);
	if (result != SQLITE_OK)
	{
		std::cout << "Err : " << msgError << std::endl;
		sqlite3_free(msgError);
	}
}

auto	Database::Review_KanjiGetReviewStats(Kanji* kanji) -> KanjiReviewStats
{
	KanjiReviewStats ret;
	MString rq("SELECT Success, Fail\
				FROM LearnSession\
				WHERE (SELECT id\
				FROM Kanji\
				WHERE Kanji.Kanji = ");
	rq += MString::FromUInt((unsigned int)kanji->GetKanji()).Str();
	rq += ") = LearnSession.ID";
	char* msgError = nullptr;
	int result = sqlite3_exec(database, rq.Str(), GetKanjisReviewStatsCallback, &ret, &msgError);
	if (result != SQLITE_OK)
	{
		std::cout << "Err : " << msgError << std::endl;
		sqlite3_free(msgError);
	}
	return ret;
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
char translationKunBuffer[256]{"\0"};
char translationOnBuffer[256]{"\0"};
Kanji* tempKanji = nullptr;
char yomiBuffer[128]{"\0"};
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
			ImGui::InputText("Kunyomi Translation", translationKunBuffer, 256);
			ImGui::InputText("Onyomi Translation", translationOnBuffer, 256);
			if (ImGui::Button("Create data"))
			{
				tempKanji = new Kanji(MWString::FromUTF8(kanjiBuffer)[0], tempKunyomiCount, tempOnyomiCount);
				tempKanji->SetOnyomiTranslation(translationOnBuffer);
				tempKanji->SetKunyomiTranslation(translationKunBuffer);
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
			ImGui::InputText(isEditingKunyomi?"Kunyomi" : "Onyomi", yomiBuffer, 128);
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