#ifndef __WORD_HPP__
#define __WORD_HPP__

#include <string>

class Kanji;

class Word
{
public:
	Word() = delete;
	Word(const Word&) = delete;
	Word(Word&&) = delete;
	~Word() = default;

	auto	operator = (const Word&)->Word & = delete;
	auto	operator = (Word&&)->Word & = delete;

protected:

private:
	Kanji** kanjiList = nullptr;
	int8_t	kanjiCount = 0;
};


#endif /*__WORD_HPP__*/
