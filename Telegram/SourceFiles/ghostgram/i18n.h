/*
This file is part of Ghostgram Desktop,
a fork of Telegram Desktop.
*/
#pragma once

#include <QtCore/QString>
#include <rpl/producer.h>

namespace Ghostgram::I18n {

[[nodiscard]] bool IsRussian();
[[nodiscard]] rpl::producer<bool> IsRussianValue();

[[nodiscard]] QString Tr(const QString &en, const QString &ru);
[[nodiscard]] rpl::producer<QString> TrValue(QString en, QString ru);

} // namespace Ghostgram::I18n
