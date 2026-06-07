/*
This file is part of Ghostgram Desktop,
a fork of Telegram Desktop.
*/
#include "ghostgram/i18n.h"

#include "lang/lang_instance.h"
#include "lang/lang_keys.h"

#include <rpl/map.h>
#include <rpl/then.h>

namespace Ghostgram::I18n {
namespace {

[[nodiscard]] bool DetectRussian(const QString &id) {
	return id.startsWith(u"ru"_q, Qt::CaseInsensitive);
}

} // namespace

bool IsRussian() {
	return DetectRussian(Lang::Id());
}

rpl::producer<bool> IsRussianValue() {
	return rpl::single(
		Lang::Id()
	) | rpl::then(
		Lang::GetInstance().idChanges()
	) | rpl::map([](const QString &id) {
		return DetectRussian(id);
	});
}

QString Tr(const QString &en, const QString &ru) {
	return IsRussian() ? ru : en;
}

rpl::producer<QString> TrValue(QString en, QString ru) {
	return IsRussianValue(
	) | rpl::map([en = std::move(en), ru = std::move(ru)](bool isRu) {
		return isRu ? ru : en;
	});
}

} // namespace Ghostgram::I18n
