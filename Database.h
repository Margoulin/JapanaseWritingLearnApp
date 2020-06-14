#pragma once

#include <cstdint>
#include <vector>

class sqlite3;
class Kanji;

struct KanjiReviewStats
{
	unsigned int Attempts = 0;
	unsigned int Correct = 0;
	unsigned int Wrong = 0;

	float	GetCorrectPercentage()
	{ 
		if (Attempts != 0)
		{
			float q = (float)Correct / (float)Attempts;
			return q * 100.0f;
		}
		return 0.0f;
		//return Attempts == 0 ? 0.0f : (Correct / Attempts) * 100.0f; 
	}
};

class Database
{
public:
	Database() = default;
	Database(const Database&) = delete;
	Database(Database&&) = delete;
	~Database() = default;

	auto	OpenDatabaseFile(const char* filename) -> void;
	auto	CloseDatabase() -> void;

	auto	AddNewKanji(Kanji* inKanji) -> void;

	auto	LoadKanji() -> void;

	auto	IsInitialized() const -> bool { return initialized; }

	auto	GetKanji(uint8_t pos) const -> Kanji* { return kanjis[pos]; }
	auto	GetKanjiCount() const -> uint8_t { return kanjiCount; }

	auto	Review_GetLast20Kanjis() -> std::vector<Kanji*>;
	auto	Review_GetFailedKanjis() -> std::vector<Kanji*>;
	auto	Review_GetOldKanjis() -> std::vector<Kanji*>;
	auto	Review_KanjiAddCorrect(Kanji* kanji) -> void;
	auto	Review_KanjiAddWrong(Kanji* kanji) -> void;
	auto	Review_KanjiGetReviewStats(Kanji* kanji) -> KanjiReviewStats;

	auto	operator = (const Database&)->Database & = delete;
	auto	operator = (Database&&)->Database & = delete;

	auto	ImGuiUpdate() -> void;

private:
	auto	expandKanjiArray() -> void;

protected:

private:
	Kanji**		kanjis = nullptr;
	sqlite3*	database = nullptr;
	uint8_t		kanjiCount = 0;
	bool		initialized = false;
};

static int GetKanjiCountCallback(void* data, int argc, char** argv, char** azColName);
static int GetKanjisCallback(void* data, int argc, char** argv, char** azColName);
