#ifndef __JAPANESE_TYPES_HPP__
#define __JAPANESE_TYPES_HPP__

#include <String.hpp>

class Kanji
{
public:
	Kanji(wchar_t inKanji, uint8_t kunCount, uint8_t onCount);
	Kanji(wchar_t inKanji);
	Kanji(const Kanji& other) = delete;
	Kanji(Kanji&& other) = delete;
	~Kanji();

	auto	DeserializeKunyomiFromJson(MString const& json) -> void;
	auto	DeserializeOnyomiFromJson(MString const& json) -> void;

	auto	SerializeKunyomi() const -> MString;
	auto	SerializeOnyomi() const -> MString;

	auto	SetKunyomi(uint8_t idx, MWString const& text) { kunyomi[idx] = text; }
	auto	SetOnyomi(uint8_t idx, MWString const& text) { onyomi[idx] = text; }
	auto	SetKunyomiTranslation(MString const& inValue) { kunTranslation = inValue; }
	auto	SetOnyomiTranslation(MString const& inValue) { onTranslation = inValue; }

	auto	GetKanji() const -> const wchar_t { return kanji[0]; }
	auto	GetKanjiStr() const -> const wchar_t* { return kanji; }
	auto	GetKunyomi(unsigned int idx) const -> MWString const& { return kunyomi[idx]; }
	auto	GetOnyomi(unsigned int idx) const -> MWString const& { return onyomi[idx]; }
	auto	GetKunyomiTranslation() const -> MString const& { return kunTranslation; }
	auto	GetOnyomiTranslation() const -> MString const& { return onTranslation; }

	auto	GetKunyomiCount() const -> uint8_t { return kunyomiCount; }
	auto	GetOnyomiCount() const -> uint8_t { return onyomiCount; }

	auto	operator = (const Kanji&)->Kanji & = delete;
	auto	operator = (Kanji&&)->Kanji & = delete;

	auto	ImGuiUpdate() -> void;

protected:

private:
	wchar_t		kanji[2];
	MWString*	kunyomi = nullptr;
	MWString*	onyomi = nullptr;
	MString		kunTranslation;
	MString		onTranslation;
	uint8_t		kunyomiCount = 0;
	uint8_t		onyomiCount = 0;
};


#endif /*__JAPANESE_TYPES_HPP__*/