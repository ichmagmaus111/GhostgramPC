/*
This file is part of Ghostgram Desktop,
a fork of Telegram Desktop.
*/
#pragma once

#include "base/basic_types.h"

#include <QString>

namespace Ghostgram {

[[nodiscard]] QString EstimateAccountAge(int64 userId);

} // namespace Ghostgram
