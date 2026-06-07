/*
This file is part of Ghostgram Desktop,
a fork of Telegram Desktop.
*/
#pragma once

#include <QString>

namespace Ghostgram {

[[nodiscard]] QString TranslateSync(
	const QString &text,
	const QString &targetLang,
	int timeoutMs = 5000);

} // namespace Ghostgram
