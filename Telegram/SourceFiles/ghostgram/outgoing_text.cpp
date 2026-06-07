/*
This file is part of Ghostgram Desktop,
a fork of Telegram Desktop.
*/
#include "ghostgram/outgoing_text.h"

#include "ghostgram/ghostgram.h"
#include "ghostgram/ghostgram_config.h"
#include "ghostgram/gtranslate.h"
#include "ui/text/text_entity.h"
#include "ui/widgets/fields/input_field.h"

namespace Ghostgram {
namespace {

[[nodiscard]] bool IsAllUpper(const QString &text) {
	auto letters = 0;
	for (const auto ch : text) {
		if (!ch.isLetter()) {
			continue;
		}
		++letters;
		if (ch.toLower() == ch) {
			return false;
		}
	}
	return (letters >= 2);
}

[[nodiscard]] QString TagFor(AutoFormat format) {
	using F = AutoFormat;
	switch (format) {
	case F::Bold: return Ui::InputField::kTagBold;
	case F::Italic: return Ui::InputField::kTagItalic;
	case F::Underline: return Ui::InputField::kTagUnderline;
	case F::Strike: return Ui::InputField::kTagStrikeOut;
	case F::Mono: return Ui::InputField::kTagCode;
	case F::Quote: return Ui::InputField::kTagBlockquote;
	case F::Spoiler: return Ui::InputField::kTagSpoiler;
	case F::None: break;
	}
	return QString();
}

} // namespace

void ApplyOutgoingTextHooks(TextWithTags &textWithTags) {
	auto &text = textWithTags.text;
	auto &tags = textWithTags.tags;
	const auto &config = Manager::Instance().config();

	if (config.translateOutgoing()
		&& !text.trimmed().isEmpty()) {
		const auto lang = config.translateOutgoingLang();
		if (!lang.isEmpty()) {
			const auto translated = TranslateSync(text, lang);
			if (!translated.isEmpty() && translated != text) {
				text = translated;
				tags.clear();
			}
		}
	}

	if (config.antiCaps() && !text.isEmpty() && IsAllUpper(text)) {
		text = text.toLower();
	}

	const auto autoTag = TagFor(config.autoFormat());
	if (!autoTag.isEmpty() && !text.isEmpty()) {
		auto already = false;
		for (const auto &t : tags) {
			if (t.offset == 0
				&& t.length == text.size()
				&& t.id == autoTag) {
				already = true;
				break;
			}
		}
		if (!already) {
			tags.push_back({ 0, int(text.size()), autoTag });
		}
	}
}

} // namespace Ghostgram
