/*
This file is part of Telegram Desktop,
the official desktop application for the Telegram messaging service.

For license and copyright information please follow this link:
https://github.com/telegramdesktop/tdesktop/blob/master/LEGAL
*/
#include "settings/sections/settings_ghostgram.h"

#include "core/application.h"
#include "core/file_utilities.h"
#include "ghostgram/ghostgram.h"
#include "ghostgram/ghostgram_config.h"
#include "ghostgram/i18n.h"
#include "settings/settings_common.h"
#include "settings/settings_common_session.h"
#include "boxes/abstract_box.h"
#include "ui/boxes/confirm_box.h"
#include "ui/widgets/continuous_sliders.h"
#include "ui/widgets/fields/input_field.h"
#include "ui/layers/generic_box.h"
#include "ui/vertical_list.h"
#include "ui/widgets/buttons.h"
#include "ui/widgets/labels.h"
#include "ui/wrap/slide_wrap.h"
#include "ui/wrap/vertical_layout.h"
#include "window/window_session_controller.h"
#include "styles/style_layers.h"
#include "styles/style_settings.h"

#include <QtCore/QFileInfo>
#include <QtGui/QFontDatabase>

namespace Settings {
namespace {

[[nodiscard]] inline rpl::producer<QString> Tr(QString en, QString ru) {
	return ::Ghostgram::I18n::TrValue(std::move(en), std::move(ru));
}

[[nodiscard]] inline QString Ts(const QString &en, const QString &ru) {
	return ::Ghostgram::I18n::Tr(en, ru);
}

void AddToggle(
		not_null<Ui::VerticalLayout*> container,
		rpl::producer<QString> title,
		rpl::producer<bool> value,
		Fn<void(bool)> onToggled) {
	const auto button = container->add(object_ptr<Ui::SettingsButton>(
		container,
		std::move(title),
		st::settingsButtonNoIcon));
	button->toggleOn(std::move(value));
	button->toggledChanges(
	) | rpl::on_next(std::move(onToggled), button->lifetime());
}

class Ghostgram : public Section<Ghostgram> {
public:
	Ghostgram(
		QWidget *parent,
		not_null<Window::SessionController*> controller);

	[[nodiscard]] rpl::producer<QString> title() override;

private:
	void setupContent();

};

Ghostgram::Ghostgram(
	QWidget *parent,
	not_null<Window::SessionController*> controller)
: Section(parent, controller) {
	setupContent();
}

rpl::producer<QString> Ghostgram::title() {
	return Tr(u"Ghostgram Settings"_q, u"Настройки Ghostgram"_q);
}

void Ghostgram::setupContent() {
	const auto content = Ui::CreateChild<Ui::VerticalLayout>(this);

	auto &config = ::Ghostgram::Manager::Instance().config();

	Ui::AddSkip(content);
	Ui::AddSubsectionTitle(content, Tr(u"Ghost mode"_q, u"Режим призрака"_q));

	AddToggle(
		content,
		Tr(u"Enable Ghost Mode"_q, u"Включить режим призрака"_q),
		config.ghostModeValue(),
		[](bool v) {
			::Ghostgram::Manager::Instance().config().setGhostMode(v);
		});

	const auto sub = content->add(
		object_ptr<Ui::SlideWrap<Ui::VerticalLayout>>(
			content,
			object_ptr<Ui::VerticalLayout>(content)));
	const auto inner = sub->entity();

	AddToggle(
		inner,
		Tr(u"Hide typing indicator"_q, u"Скрыть индикатор набора"_q),
		config.suppressTypingValue(),
		[](bool v) {
			::Ghostgram::Manager::Instance().config().setSuppressTyping(v);
		});

	AddToggle(
		inner,
		Tr(u"Don't mark messages as read"_q,
			u"Не отмечать сообщения прочитанными"_q),
		config.suppressReadMessagesValue(),
		[](bool v) {
			::Ghostgram::Manager::Instance().config().setSuppressReadMessages(v);
		});

	AddToggle(
		inner,
		Tr(u"Hide online status"_q, u"Скрыть статус «в сети»"_q),
		config.hideOnlineValue(),
		[](bool v) {
			::Ghostgram::Manager::Instance().config().setHideOnline(v);
		});

	AddToggle(
		inner,
		Tr(u"Don't mark stories as viewed"_q,
			u"Не отмечать истории просмотренными"_q),
		config.suppressReadStoriesValue(),
		[](bool v) {
			::Ghostgram::Manager::Instance().config().setSuppressReadStories(v);
		});

	AddToggle(
		inner,
		Tr(u"Always appear offline"_q, u"Всегда оффлайн"_q),
		config.autoOfflineValue(),
		[](bool v) {
			::Ghostgram::Manager::Instance().config().setAutoOffline(v);
		});

	AddToggle(
		inner,
		Tr(u"Mark read when replying or reacting"_q,
			u"Прочитать при ответе или реакции"_q),
		config.readOnActionsValue(),
		[](bool v) {
			::Ghostgram::Manager::Instance().config().setReadOnActions(v);
		});

	AddToggle(
		inner,
		Tr(u"Delay sending"_q, u"Задержка отправки"_q),
		config.ghostSendDelayValue(),
		[](bool v) {
			::Ghostgram::Manager::Instance().config().setGhostSendDelay(v);
		});

	const auto addDelaySlider = [&](
			rpl::producer<QString> caption,
			int minSec,
			int maxSec,
			int currentSec,
			Fn<void(int)> save) {
		Ui::AddSubsectionTitle(inner, std::move(caption));
		const auto secWidthSample = Ts(u"60 sec"_q, u"60 сек"_q);
		auto sliderWithLabel = ::Settings::MakeSliderWithLabel(
			inner,
			st::settingsScale,
			st::settingsScaleLabel,
			st::normalFont->spacew * 2,
			st::settingsScaleLabel.style.font->width(secWidthSample));
		inner->add(
			std::move(sliderWithLabel.widget),
			st::settingsBigScalePadding);
		const auto slider = sliderWithLabel.slider;
		const auto label = sliderWithLabel.label;
		const auto suffix = Ts(u" sec"_q, u" сек"_q);
		const auto setLabel = [=](int v) {
			label->setText(QString::number(v) + suffix);
		};
		setLabel(currentSec);
		slider->setPseudoDiscrete(
			maxSec - minSec + 1,
			[=](int index) { return minSec + index; },
			currentSec,
			[=](int v) { setLabel(v); },
			[=](int v) { setLabel(v); save(v); });
	};

	addDelaySlider(
		Tr(u"Text delay"_q, u"Задержка текста"_q),
		12,
		60,
		std::clamp(config.ghostTextDelaySec(), 12, 60),
		[](int v) {
			::Ghostgram::Manager::Instance().config()
				.setGhostTextDelaySec(v);
		});

	addDelaySlider(
		Tr(u"Media delay"_q, u"Задержка медиа"_q),
		20,
		60,
		std::clamp(config.ghostMediaDelaySec(), 20, 60),
		[](int v) {
			::Ghostgram::Manager::Instance().config()
				.setGhostMediaDelaySec(v);
		});

	sub->toggleOn(config.ghostModeValue(), anim::type::normal);
	sub->finishAnimating();

	Ui::AddSkip(content);
	Ui::AddDividerText(
		content,
		Tr(u"Block read receipts, story views, online status and typing indicators. «Read on actions» allows reading to mark messages read when you reply or react."_q,
			u"Блокирует подтверждения прочтения, просмотры историй, статус «в сети» и индикатор набора. «Прочитать при ответе или реакции» разрешает отметку о прочтении, когда вы отвечаете или ставите реакцию."_q));

	Ui::AddSkip(content);
	Ui::AddSubsectionTitle(content, Tr(u"Online status"_q, u"Статус «в сети»"_q));

	AddToggle(
		content,
		Tr(u"Eternal online"_q, u"Вечный онлайн"_q),
		config.eternalOnlineValue(),
		[](bool v) {
			::Ghostgram::Manager::Instance().config().setEternalOnline(v);
		});

	Ui::AddSkip(content);
	Ui::AddDividerText(
		content,
		Tr(u"Keep your account marked as online indefinitely, even when Ghostgram is in background."_q,
			u"Аккаунт остаётся в сети бесконечно, даже когда Ghostgram свёрнут."_q));

	Ui::AddSkip(content);
	Ui::AddSubsectionTitle(content, Tr(u"Message archive"_q, u"Архив сообщений"_q));

	AddToggle(
		content,
		Tr(u"Store on disk"_q, u"Хранить на диске"_q),
		config.saveToDiskValue(),
		[](bool v) {
			::Ghostgram::Manager::Instance().config().setSaveToDisk(v);
		});

	AddToggle(
		content,
		Tr(u"Save deleted messages"_q, u"Сохранять удалённые сообщения"_q),
		config.saveDeletedMessagesValue(),
		[](bool v) {
			::Ghostgram::Manager::Instance().config().setSaveDeletedMessages(v);
		});

	AddToggle(
		content,
		Tr(u"Save edit history"_q, u"Сохранять историю изменений"_q),
		config.saveEditedMessagesValue(),
		[](bool v) {
			::Ghostgram::Manager::Instance().config().setSaveEditedMessages(v);
		});

	AddToggle(
		content,
		Tr(u"Save view-once media"_q, u"Сохранять временные медиа"_q),
		config.saveEphemeralMediaValue(),
		[](bool v) {
			::Ghostgram::Manager::Instance().config().setSaveEphemeralMedia(v);
		});

	Ui::AddSkip(content);
	Ui::AddDividerText(
		content,
		Tr(u"Keep local copies of deleted, edited and view-once messages. View-once voice/video notes appear as regular ones with a timer icon. «Store on disk» preserves data across restarts; off — only in memory."_q,
			u"Хранит локальные копии удалённых, изменённых и временных сообщений. Временные голосовые и видеосообщения отображаются как обычные, с иконкой таймера. «Хранить на диске» сохраняет данные между перезапусками; выкл. — только в памяти."_q));

	Ui::AddSkip(content);
	Ui::AddSubsectionTitle(content, Tr(u"Restrictions"_q, u"Ограничения"_q));

	AddToggle(
		content,
		Tr(u"Bypass protected content"_q, u"Обходить защиту контента"_q),
		config.bypassProtectedContentValue(),
		[](bool v) {
			::Ghostgram::Manager::Instance().config().setBypassProtectedContent(v);
		});

	AddToggle(
		content,
		Tr(u"Block sponsored messages"_q, u"Блокировать рекламу"_q),
		config.blockAdsValue(),
		[](bool v) {
			::Ghostgram::Manager::Instance().config().setBlockAds(v);
		});

	AddToggle(
		content,
		Tr(u"Hide reactions"_q, u"Скрыть реакции"_q),
		config.hideReactionsValue(),
		[](bool v) {
			::Ghostgram::Manager::Instance().config().setHideReactions(v);
		});

	AddToggle(
		content,
		Tr(u"Hide comments button"_q, u"Скрыть кнопку комментариев"_q),
		config.hideCommentsValue(),
		[](bool v) {
			::Ghostgram::Manager::Instance().config().setHideComments(v);
		});

	AddToggle(
		content,
		Tr(u"Square avatars"_q, u"Квадратные аватары"_q),
		config.squareUserpicsValue(),
		[](bool v) {
			::Ghostgram::Manager::Instance().config().setSquareUserpics(v);
		});

	Ui::AddSkip(content);
	Ui::AddDividerText(
		content,
		Tr(u"Toggle visibility of restrictions, ads, reactions, comments. «Square avatars» switches round avatars to rounded squares."_q,
			u"Переключает видимость ограничений, рекламы, реакций и комментариев. «Квадратные аватары» делает круглые аватары скруглёнными квадратами."_q));

	Ui::AddSkip(content);
	Ui::AddSubsectionTitle(content, Tr(u"Archive opacity"_q, u"Прозрачность архива"_q));

	auto sliderWithLabel = ::Settings::MakeSliderWithLabel(
		content,
		st::settingsScale,
		st::settingsScaleLabel,
		st::normalFont->spacew * 2,
		st::settingsScaleLabel.style.font->width(u"100%"_q));
	content->add(
		std::move(sliderWithLabel.widget),
		st::settingsBigScalePadding);
	const auto slider = sliderWithLabel.slider;
	const auto label = sliderWithLabel.label;
	const auto updateLabel = [=](double v) {
		label->setText(QString::number(int(base::SafeRound(v * 100))) + '%');
	};
	updateLabel(config.archiveOpacity());
	slider->setValue(config.archiveOpacity());
	slider->setChangeProgressCallback([=](double v) {
		updateLabel(v);
		::Ghostgram::Manager::Instance().config().setArchiveOpacity(v);
	});
	slider->setChangeFinishedCallback([=](double v) {
		::Ghostgram::Manager::Instance().config().setArchiveOpacity(v);
	});

	Ui::AddSkip(content);
	Ui::AddDividerText(
		content,
		Tr(u"Opacity for deleted messages and edit preview text. 0% — invisible, 100% — full color."_q,
			u"Прозрачность удалённых сообщений и текста-превью редактирования. 0% — невидимо, 100% — полный цвет."_q));

	Ui::AddSkip(content);
	Ui::AddSubsectionTitle(content, Tr(u"Composer"_q, u"Поле ввода"_q));

	AddToggle(
		content,
		Tr(u"Anti-caps"_q, u"Анти-капс"_q),
		config.antiCapsValue(),
		[](bool v) {
			::Ghostgram::Manager::Instance().config().setAntiCaps(v);
		});

	Ui::AddSkip(content);
	Ui::AddDividerText(
		content,
		Tr(u"If you send a message in ALL CAPS, it will be lowered automatically."_q,
			u"Если отправить сообщение КАПСОМ, оно будет автоматически переведено в нижний регистр."_q));

	Ui::AddSkip(content);
	Ui::AddSubsectionTitle(content, Tr(u"Local premium"_q, u"Локальный Premium"_q));

	AddToggle(
		content,
		Tr(u"Fake Telegram Premium"_q, u"Фейковый Telegram Premium"_q),
		config.fakePremiumValue(),
		[](bool v) {
			::Ghostgram::Manager::Instance().config().setFakePremium(v);
		});

	Ui::AddSkip(content);
	Ui::AddDividerText(
		content,
		Tr(u"Pretend that your account has Premium. Unlocks client-side features that are not enforced by the server. Restart required."_q,
			u"Притворяется, что у аккаунта есть Premium. Разблокирует клиентские функции, которые не проверяются сервером. Требуется перезапуск."_q));

	Ui::AddSkip(content);
	Ui::AddSubsectionTitle(content, Tr(u"Local stars"_q, u"Локальные звёзды"_q));

	AddToggle(
		content,
		Tr(u"Fake stars balance"_q, u"Фейковый баланс звёзд"_q),
		config.fakeStarsValue(),
		[](bool v) {
			::Ghostgram::Manager::Instance().config().setFakeStars(v);
		});

	const auto starsBtn = content->add(object_ptr<Ui::SettingsButton>(
		content,
		rpl::combine(
			config.fakeStarsAmountValue(),
			::Ghostgram::I18n::IsRussianValue()
		) | rpl::map([](int64 v, bool isRu) {
			return (isRu ? u"Звёзды: %1"_q : u"Stars: %1"_q).arg(v);
		}),
		st::settingsButtonNoIcon));
	starsBtn->setClickedCallback([] {
		const auto builder = [](not_null<Ui::GenericBox*> dialog) {
			dialog->setTitle(Tr(u"Stars balance"_q, u"Баланс звёзд"_q));
			const auto current = ::Ghostgram::Manager::Instance().config()
				.fakeStarsAmount();
			const auto field = dialog->addRow(object_ptr<Ui::InputField>(
				dialog,
				st::defaultInputField,
				Tr(u"Amount"_q, u"Сумма"_q),
				QString::number(current)));
			dialog->setFocusCallback([=] { field->setFocusFast(); });
			const auto save = [=] {
				bool ok = false;
				const auto parsed = field->getLastText().toLongLong(&ok);
				if (ok && parsed >= 0) {
					::Ghostgram::Manager::Instance().config()
						.setFakeStarsAmount(parsed);
				}
				dialog->closeBox();
			};
			field->submits() | rpl::on_next(save, field->lifetime());
			dialog->addButton(Tr(u"Save"_q, u"Сохранить"_q), save);
			dialog->addButton(Tr(u"Cancel"_q, u"Отмена"_q), [=] {
				dialog->closeBox();
			});
		};
		Ui::show(Box(builder));
	});

	Ui::AddSkip(content);
	Ui::AddDividerText(
		content,
		Tr(u"Local-only stars balance. The client will see the entered amount; server-side spending of stars will still be rejected if you don't actually have them."_q,
			u"Баланс звёзд только локально. Клиент видит указанную сумму, но реальные траты на сервере всё равно будут отклонены, если звёзд нет."_q));

	Ui::AddSkip(content);
	Ui::AddSubsectionTitle(content, Tr(u"Outgoing translation"_q, u"Перевод исходящих"_q));

	AddToggle(
		content,
		Tr(u"Translate before sending"_q, u"Переводить перед отправкой"_q),
		config.translateOutgoingValue(),
		[](bool v) {
			::Ghostgram::Manager::Instance().config().setTranslateOutgoing(v);
		});

	const auto langButton = content->add(object_ptr<Ui::SettingsButton>(
		content,
		rpl::combine(
			config.translateOutgoingLangValue(),
			::Ghostgram::I18n::IsRussianValue()
		) | rpl::map([](QString lang, bool isRu) {
			const auto prefix = isRu
				? u"Язык перевода: "_q
				: u"Target language: "_q;
			return prefix + (lang.isEmpty() ? u"en"_q : lang);
		}),
		st::settingsButtonNoIcon));
	langButton->setClickedCallback([] {
		const auto builder = [](not_null<Ui::GenericBox*> dialog) {
			dialog->setTitle(Tr(u"Target language"_q, u"Язык перевода"_q));
			const auto current = ::Ghostgram::Manager::Instance().config()
				.translateOutgoingLang();
			const auto field = dialog->addRow(object_ptr<Ui::InputField>(
				dialog,
				st::defaultInputField,
				Tr(u"Language code (en, ru, de, ...)"_q,
					u"Код языка (en, ru, de, ...)"_q),
				current));
			dialog->setFocusCallback([=] { field->setFocusFast(); });
			const auto save = [=] {
				const auto value = field->getLastText().trimmed().toLower();
				if (!value.isEmpty()) {
					::Ghostgram::Manager::Instance().config()
						.setTranslateOutgoingLang(value);
				}
				dialog->closeBox();
			};
			field->submits() | rpl::on_next(save, field->lifetime());
			dialog->addButton(Tr(u"Save"_q, u"Сохранить"_q), save);
			dialog->addButton(Tr(u"Cancel"_q, u"Отмена"_q), [=] {
				dialog->closeBox();
			});
		};
		Ui::show(Box(builder));
	});

	Ui::AddSkip(content);
	Ui::AddDividerText(
		content,
		Tr(u"Your messages will be translated through Google Translate to the chosen language right before sending. Network latency adds a small delay."_q,
			u"Сообщения будут переведены через Google Translate на выбранный язык прямо перед отправкой. Сетевая задержка добавит небольшую паузу."_q));

	Ui::AddSkip(content);
	Ui::AddSubsectionTitle(content, Tr(u"Auto-format"_q, u"Авто-форматирование"_q));

	using F = ::Ghostgram::AutoFormat;
	auto formats = std::vector<std::pair<rpl::producer<QString>, F>>{
		{ Tr(u"Off"_q, u"Выкл."_q), F::None },
		{ Tr(u"Bold"_q, u"Жирный"_q), F::Bold },
		{ Tr(u"Italic"_q, u"Курсив"_q), F::Italic },
		{ Tr(u"Underline"_q, u"Подчёркнутый"_q), F::Underline },
		{ Tr(u"Strikethrough"_q, u"Зачёркнутый"_q), F::Strike },
		{ rpl::single(u"Monospace"_q), F::Mono },
		{ rpl::single(u"Quote"_q), F::Quote },
		{ rpl::single(u"Spoiler"_q), F::Spoiler },
	};
	for (auto &[title, value] : formats) {
		const auto button = content->add(object_ptr<Ui::SettingsButton>(
			content,
			std::move(title),
			st::settingsButtonNoIcon));
		const auto v = value;
		button->toggleOn(config.autoFormatValue() | rpl::map([=](F f) {
			return (f == v);
		}));
		button->toggledChanges(
		) | rpl::on_next([=](bool on) {
			auto &cfg = ::Ghostgram::Manager::Instance().config();
			if (on) {
				cfg.setAutoFormat(v);
			} else if (cfg.autoFormat() == v) {
				cfg.setAutoFormat(F::None);
			}
		}, button->lifetime());
	}

	Ui::AddSkip(content);
	Ui::AddDividerText(
		content,
		Tr(u"Outgoing messages will be wrapped in the selected style automatically."_q,
			u"Исходящие сообщения автоматически обернутся в выбранный стиль."_q));

	Ui::AddSkip(content);
	Ui::AddSubsectionTitle(content, Tr(u"Custom font"_q, u"Свой шрифт"_q));

	const auto familyLabel = content->add(
		object_ptr<Ui::FlatLabel>(
			content,
			rpl::combine(
				config.customFontFamilyValue(),
				::Ghostgram::I18n::IsRussianValue()
			) | rpl::map([](QString f, bool isRu) {
				if (!f.isEmpty()) {
					return f;
				}
				return isRu ? u"Не выбран"_q : u"None"_q;
			}),
			st::boxLabel),
		st::settingsButton.padding);

	const auto loadBtn = content->add(object_ptr<Ui::SettingsButton>(
		content,
		Tr(u"Load .ttf / .otf"_q, u"Загрузить .ttf / .otf"_q),
		st::settingsButtonNoIcon));
	loadBtn->setClickedCallback([] {
		FileDialog::GetOpenPath(
			Core::App().getFileDialogParent(),
			"Custom font",
			"Fonts (*.ttf *.otf)",
			[](const FileDialog::OpenResult &result) {
				if (result.paths.isEmpty()) {
					return;
				}
				const auto path = result.paths.front();
				const auto id = QFontDatabase::addApplicationFont(path);
				if (id < 0) {
					Ui::show(Ui::MakeInformBox(Ts(
						u"Could not load font file."_q,
						u"Не удалось загрузить шрифт."_q)));
					return;
				}
				const auto families =
					QFontDatabase::applicationFontFamilies(id);
				if (families.isEmpty()) {
					QFontDatabase::removeApplicationFont(id);
					Ui::show(Ui::MakeInformBox(Ts(
						u"Font has no readable family."_q,
						u"У шрифта нет читаемого семейства."_q)));
					return;
				}
				::Ghostgram::Manager::Instance().config().setCustomFont(
					path,
					families.front());
				Ui::show(Ui::MakeInformBox(Ts(
					u"Font loaded. Restart Ghostgram to apply."_q,
					u"Шрифт загружен. Перезапустите Ghostgram, чтобы применить."_q)));
			});
	});

	const auto clearBtn = content->add(object_ptr<Ui::SettingsButton>(
		content,
		Tr(u"Reset to default"_q, u"Сбросить"_q),
		st::settingsButtonNoIcon));
	clearBtn->setClickedCallback([] {
		::Ghostgram::Manager::Instance().config().clearCustomFont();
		Ui::show(Ui::MakeInformBox(Ts(
			u"Custom font cleared. Restart Ghostgram to apply."_q,
			u"Шрифт сброшен. Перезапустите Ghostgram, чтобы применить."_q)));
	});

	Ui::AddSkip(content);
	Ui::AddDividerText(
		content,
		Tr(u"Load a .ttf or .otf font to use throughout the app. Restart required."_q,
			u"Загрузите .ttf или .otf шрифт для использования во всём приложении. Требуется перезапуск."_q));

	Ui::AddSkip(content);
	Ui::AddSubsectionTitle(content, Tr(u"Chat background media"_q, u"Медиа-фон чата"_q));

	const auto bgPathLabel = content->add(
		object_ptr<Ui::FlatLabel>(
			content,
			rpl::combine(
				config.chatBackgroundMediaPathValue(),
				::Ghostgram::I18n::IsRussianValue()
			) | rpl::map([](QString p, bool isRu) {
				if (!p.isEmpty()) {
					return QFileInfo(p).fileName();
				}
				return isRu ? u"Не выбрано"_q : u"None"_q;
			}),
			st::boxLabel),
		st::settingsButton.padding);

	const auto bgLoadBtn = content->add(object_ptr<Ui::SettingsButton>(
		content,
		Tr(u"Load .gif / .mp4 / .webm"_q, u"Загрузить .gif / .mp4 / .webm"_q),
		st::settingsButtonNoIcon));
	bgLoadBtn->setClickedCallback([] {
		FileDialog::GetOpenPath(
			Core::App().getFileDialogParent(),
			"Chat background media",
			"Media (*.gif *.mp4 *.webm *.mov *.m4v)",
			[](const FileDialog::OpenResult &result) {
				if (result.paths.isEmpty()) {
					return;
				}
				::Ghostgram::Manager::Instance().config()
					.setChatBackgroundMediaPath(result.paths.front());
			});
	});

	const auto bgClearBtn = content->add(object_ptr<Ui::SettingsButton>(
		content,
		Tr(u"Clear"_q, u"Очистить"_q),
		st::settingsButtonNoIcon));
	bgClearBtn->setClickedCallback([] {
		::Ghostgram::Manager::Instance().config()
			.setChatBackgroundMediaPath(QString());
	});

	Ui::AddSkip(content);
	Ui::AddSubsectionTitle(content, Tr(u"Background opacity"_q, u"Прозрачность фона"_q));

	auto bgSliderWithLabel = ::Settings::MakeSliderWithLabel(
		content,
		st::settingsScale,
		st::settingsScaleLabel,
		st::normalFont->spacew * 2,
		st::settingsScaleLabel.style.font->width(u"100%"_q));
	content->add(
		std::move(bgSliderWithLabel.widget),
		st::settingsBigScalePadding);
	const auto bgSlider = bgSliderWithLabel.slider;
	const auto bgLabel = bgSliderWithLabel.label;
	const auto bgUpdateLabel = [=](double v) {
		bgLabel->setText(
			QString::number(int(base::SafeRound(v * 100))) + '%');
	};
	bgUpdateLabel(config.chatBackgroundMediaOpacity());
	bgSlider->setValue(config.chatBackgroundMediaOpacity());
	bgSlider->setChangeProgressCallback([=](double v) {
		bgUpdateLabel(v);
		::Ghostgram::Manager::Instance().config()
			.setChatBackgroundMediaOpacity(v);
	});
	bgSlider->setChangeFinishedCallback([=](double v) {
		::Ghostgram::Manager::Instance().config()
			.setChatBackgroundMediaOpacity(v);
	});

	Ui::AddSkip(content);
	Ui::AddDividerText(
		content,
		Tr(u"Render a looping GIF/video over the chat background. 0% — invisible, 100% — fully opaque."_q,
			u"Зацикленный GIF или видео поверх фона чата. 0% — невидимо, 100% — полная непрозрачность."_q));

	Ui::ResizeFitChild(this, content);
}

} // namespace

Type GhostgramId() {
	return Ghostgram::Id();
}

} // namespace Settings
