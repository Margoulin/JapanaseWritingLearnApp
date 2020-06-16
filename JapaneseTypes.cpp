#include "JapaneseTypes.h"

#include <rapidjson/document.h>
#include <rapidjson/stringbuffer.h>
#include <rapidjson/prettywriter.h>

using namespace rapidjson;

Kanji::Kanji(wchar_t inKanji, uint8_t kunCount, uint8_t onCount)
{
	kanji[0] = inKanji;
	kanji[1] = '\0';
	kunyomiCount = kunCount;
	onyomiCount = onCount;

	kunyomi = new MWString[kunCount];
	onyomi = new MWString[onCount];
}

Kanji::Kanji(wchar_t inKanji)
{
	kanji[0] = inKanji;
	kanji[1] = '\0';
}

Kanji::~Kanji()
{
	delete[] kunyomi;
	delete[] onyomi;
}

auto	Kanji::DeserializeKunyomiFromJson(MString const& json) -> void
{
	Document doc;
	doc.Parse(json.Str());
	auto kunyomiArray = doc["Kunyomi"].GetArray();
	kunyomiCount = kunyomiArray.Size();
	kunyomi = new MWString[kunyomiCount];
	for (unsigned int idx = 0; idx < kunyomiCount; idx++)
	{
		auto wcharArray = kunyomiArray[idx].GetArray();
		for (unsigned int pos = 0; pos < wcharArray.Size(); pos++)
			kunyomi[idx].Append(wchar_t(wcharArray[pos].GetUint()));
	}
}

auto	Kanji::DeserializeOnyomiFromJson(MString const& json) -> void
{
	Document doc;
	doc.Parse(json.Str());
	auto onyomiArray = doc["Onyomi"].GetArray();
	onyomiCount = onyomiArray.Size();
	onyomi = new MWString[onyomiCount];
	for (unsigned int idx = 0; idx < onyomiCount; idx++)
	{
		auto wcharArray = onyomiArray[idx].GetArray();
		for (unsigned int pos = 0; pos < wcharArray.Size(); pos++)
			onyomi[idx].Append(wchar_t(wcharArray[pos].GetUint()));
	}
}

auto	Kanji::SerializeKunyomi() const ->MString
{
	StringBuffer sb;
	PrettyWriter<StringBuffer> writer(sb);
	writer.SetFormatOptions(PrettyFormatOptions::kFormatSingleLineArray);
	writer.StartObject();

	writer.String("Kunyomi");
	writer.StartArray();
	for (unsigned int pos = 0; pos < kunyomiCount; pos++)
	{
		writer.StartArray();
		for (unsigned int idx = 0; idx < kunyomi[pos].Count(); idx++)
			writer.Uint(unsigned int(kunyomi[pos][idx]));
		writer.EndArray();
	}
	writer.EndArray();

	writer.EndObject();

	return MString(sb.GetString());
}

auto	Kanji::SerializeOnyomi() const -> MString
{
	StringBuffer sb;
	PrettyWriter<StringBuffer> writer(sb);
	writer.SetFormatOptions(PrettyFormatOptions::kFormatSingleLineArray);
	writer.StartObject();

	writer.String("Onyomi");
	writer.StartArray();
	for (unsigned int pos = 0; pos < onyomiCount; pos++)
	{
		writer.StartArray();
		for (unsigned int idx = 0; idx < onyomi[pos].Count(); idx++)
			writer.Uint(unsigned int(onyomi[pos][idx]));
		writer.EndArray();
	}
	writer.EndArray();

	writer.EndObject();

	return MString(sb.GetString());
}

#include <imgui.h>

auto	Kanji::ImGuiUpdate() -> void
{
	if (ImGui::CollapsingHeader(MWString(kanji).ToUTF8String().Str()))
	{
		if (kunyomiCount > 0)
		{
			ImGui::Indent();
			if (ImGui::CollapsingHeader(MString("Kunyomi : " + MString::FromUInt(kunyomiCount)).Str()))
			{
				ImGui::Indent();
				for (unsigned int idx = 0; idx < kunyomiCount; idx++)
					ImGui::Text(kunyomi[idx].ToUTF8String().Str());
				ImGui::Unindent();
			}
			ImGui::Unindent();
		}

		if (onyomiCount > 0)
		{
			ImGui::Indent();
			if (ImGui::CollapsingHeader(MString("Onyomi : " + MString::FromUInt(onyomiCount)).Str()))
			{
				ImGui::Indent();
				for (unsigned int idx = 0; idx < onyomiCount; idx++)
					ImGui::Text(onyomi[idx].ToUTF8String().Str());
				ImGui::Unindent();
			}
			ImGui::Unindent();
		}
		if (kunTranslation.Count() > 0)
			ImGui::Text(MString("Kunyomi Translation : " + kunTranslation).Str());
		if (onTranslation.Count() > 0)
			ImGui::Text(MString("Onyomi Translation : " + onTranslation).Str());
	}
}

auto	Kanji::ImGuiUpdateOpen() -> void
{
	if (ImGui::CollapsingHeader(MWString(kanji).ToUTF8String().Str(), ImGuiTreeNodeFlags_DefaultOpen))
	{
		if (kunyomiCount > 0)
		{
			ImGui::Indent();
			if (ImGui::CollapsingHeader(MString("Kunyomi : " + MString::FromUInt(kunyomiCount)).Str(), ImGuiTreeNodeFlags_DefaultOpen))
			{
				ImGui::Indent();
				for (unsigned int idx = 0; idx < kunyomiCount; idx++)
					ImGui::Text(kunyomi[idx].ToUTF8String().Str());
				ImGui::Unindent();
			}
			ImGui::Unindent();
		}

		if (onyomiCount > 0)
		{
			ImGui::Indent();
			if (ImGui::CollapsingHeader(MString("Onyomi : " + MString::FromUInt(onyomiCount)).Str(), ImGuiTreeNodeFlags_DefaultOpen))
			{
				ImGui::Indent();
				for (unsigned int idx = 0; idx < onyomiCount; idx++)
					ImGui::Text(onyomi[idx].ToUTF8String().Str());
				ImGui::Unindent();
			}
			ImGui::Unindent();
		}
		if (kunTranslation.Count() > 0)
			ImGui::Text(MString("Kunyomi Translation : " + kunTranslation).Str());
		if (onTranslation.Count() > 0)
			ImGui::Text(MString("Onyomi Translation : " + onTranslation).Str());
	}
}