/*
This file is part of Ghostgram Desktop,
a fork of Telegram Desktop.

Anchor data — real samples from
https://github.com/Jobians/telegram-id-age/blob/main/dataset.json
(filtered to monotonic-in-date entries).
*/
#include "ghostgram/account_age.h"

#include "ghostgram/i18n.h"

#include <QtCore/QDate>
#include <QtCore/QDateTime>
#include <QtCore/QLocale>

#include <array>

namespace Ghostgram {
namespace {

struct Anchor {
	int64 id;
	int year;
	int month;
	int day;
};

constexpr auto kAnchors = std::to_array<Anchor>({
	{             0, 2013,  8, 14 },
	{       2768409, 2013, 11,  1 },
	{       7679610, 2013, 12, 31 },
	{      11538514, 2014,  2,  1 },
	{      15835244, 2014,  2, 20 },
	{      23646077, 2014,  2, 26 },
	{      38015510, 2014,  3,  1 },
	{      44634663, 2014,  5,  6 },
	{      46145305, 2014,  5, 15 },
	{      54845238, 2014,  9, 20 },
	{      63263518, 2014, 10, 27 },
	{     101260938, 2015,  3,  6 },
	{     101323197, 2015,  3, 13 },
	{     103151531, 2015,  6,  3 },
	{     109393468, 2015,  8,  8 },
	{     112594714, 2015,  8, 15 },
	{     124872445, 2015,  8, 17 },
	{     125828524, 2015, 10,  5 },
	{     133909606, 2015, 10,  7 },
	{     143445125, 2015, 12,  1 },
	{     148670295, 2016,  1,  8 },
	{     152079341, 2016,  1, 22 },
	{     171295414, 2016,  3,  9 },
	{     181783990, 2016,  4, 10 },
	{     222021233, 2016,  6,  8 },
	{     225034354, 2016,  6, 18 },
	{     278941742, 2016,  9, 10 },
	{     285253072, 2016, 10, 18 },
	{     294851037, 2016, 11, 19 },
	{     297621225, 2016, 12, 16 },
	{     328594461, 2017,  1, 28 },
	{     337808429, 2017,  2, 21 },
	{     341546272, 2017,  2, 22 },
	{     352940995, 2017,  2, 24 },
	{     369669043, 2017,  3, 31 },
	{     400169472, 2017,  7, 31 },
	{     805158066, 2019,  7, 15 },
	{    1974255900, 2021, 10, 12 },
	{    5022636255, 2021, 12, 10 },
	{    5045293264, 2022,  1, 13 },
	{    5047148663, 2022,  4,  5 },
	{    5144324763, 2022,  4,  6 },
	{    5159326926, 2022,  6, 16 },
	{    5340744210, 2022, 10, 26 },
	{    5472518401, 2022, 11,  2 },
	{    5559594088, 2022, 11, 10 },
	{    5567880858, 2023,  1, 25 },
	{    5738347976, 2023,  4, 25 },
	{    5854845236, 2023,  4, 27 },
	{    5891297818, 2023,  5,  4 },
	{    5904140174, 2023,  5, 20 },
	{    6000582627, 2023,  5, 30 },
	{    6188508923, 2023,  6, 22 },
	{    6326011828, 2023,  7,  7 },
	{    6401027363, 2023, 11, 25 },
	{    6451891234, 2023, 12,  2 },
	{    6513268158, 2023, 12,  2 },
	{    6536173556, 2024,  1, 11 },
	{    6559717847, 2024,  2, 25 },
	{    6606876583, 2024,  3, 17 },
	{    6651640269, 2024,  5, 18 },
	{    6872061796, 2024,  5, 25 },
	{    7078066115, 2024,  9,  8 },
	{    7273085448, 2024, 11, 21 },
	{    7342300216, 2025,  1, 16 },
	{    7591351660, 2025,  3, 21 },
	{    7708562823, 2025,  5, 10 },
	{    7817256746, 2025,  6, 18 },
	{    7834356221, 2025,  9,  1 },
	{    7912577935, 2025,  9, 15 },
	{    8017192943, 2025, 10,  5 },
	{    8117852491, 2025, 10, 14 },
	{    8209194945, 2025, 10, 26 },
	{    8480708838, 2025, 11,  5 },
	{    8559682245, 2025, 11, 11 },
});

[[nodiscard]] qint64 ToUnix(const Anchor &a) {
	return QDateTime(
		QDate(a.year, a.month, a.day),
		QTime(0, 0),
		Qt::UTC).toSecsSinceEpoch();
}

[[nodiscard]] QString FormatDate(qint64 unix) {
	const auto date = QDateTime::fromSecsSinceEpoch(unix, Qt::UTC).date();
	return QLocale::c().toString(date, u"d MMM yyyy"_q);
}

} // namespace

QString EstimateAccountAge(int64 userId) {
	if (userId <= 0) {
		return QString();
	}
	const auto &first = kAnchors.front();
	const auto &last = kAnchors.back();
	if (userId <= first.id) {
		return u"~ "_q + FormatDate(ToUnix(first));
	}
	if (userId >= last.id) {
		return u"~ "_q
			+ FormatDate(ToUnix(last))
			+ I18n::Tr(u" or later"_q, u" или позже"_q);
	}
	for (auto i = std::size_t(1); i != kAnchors.size(); ++i) {
		const auto &hi = kAnchors[i];
		if (userId > hi.id) {
			continue;
		}
		const auto &lo = kAnchors[i - 1];
		const auto loUnix = ToUnix(lo);
		const auto hiUnix = ToUnix(hi);
		const auto span = double(hi.id - lo.id);
		const auto pos = double(userId - lo.id) / span;
		const auto unix = qint64(loUnix + pos * (hiUnix - loUnix));
		return u"~ "_q + FormatDate(unix);
	}
	return QString();
}

} // namespace Ghostgram
