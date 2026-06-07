# Ghostgram Desktop

**Ghostgram Desktop** is a privacy- and automation-focused fork of [Telegram Desktop][tdesktop] (tdesktop). It keeps full Telegram functionality and adds a layer of client-side stealth, message recovery, anti-restriction and quality-of-life features — all toggleable from a single **Ghostgram** settings section.

It is cross-platform (Windows / macOS / Linux). Pre-built binaries for this fork are produced by the GitHub Actions workflow in this repo; you can also build it yourself with the instructions below.

> Based on Telegram Desktop. Source is published under **GPLv3 with the OpenSSL exception** (see [LICENSE](LICENSE)). Ghostgram is an unofficial fork and is **not** affiliated with Telegram FZ-LLC.

## 📥 Download

- **Telegram channel: [@ghostgram](https://t.me/ghostgram)** — latest builds & announcements.
- **[GitHub Releases](https://github.com/ichmagmaus111/GhostgramPC/releases)** — `Ghostgram.exe` (Windows x64) and the signed `.dmg` (macOS, Apple Silicon).

> The macOS build is signed with Developer ID but not notarised — on first launch use **right-click → Open** (or run `xattr -dr com.apple.quarantine Ghostgram.app`).

---

## Privacy model — what stays local vs. what the server sees

Most features are **purely local** and never leave your client. A few intentionally change what the Telegram server sees (because that is the point of the feature).

- **Local-only (never sent to Telegram):** saved deleted/edited messages, view-once / ephemeral media capture, fake Premium, fake Stars, square avatars, archive opacity, custom font, hidden reactions/comments, edit-history viewer, deleted-message preservation in chat, the round app icon.
- **Server-visible behaviour we modify:** online / typing / read status (suppressed, or forced online via *Eternal online*), outgoing message text (translated / anti-caps / auto-formatted), forward bypass (sent as fresh messages from you), send-delay (Telegram-side `schedule_date`), and *Read to here*.

All Ghostgram state is stored locally next to your account data and is never uploaded:

```
tdata/ghostgram_config.json          # all toggles
tdata/ghostgram_archive.json         # deleted + edited messages
tdata/ghostgram_media_archive.json   # view-once / ephemeral metadata
tdata/ghostgram_media/               # captured view-once / voice / round files
```

---

## Features

### 👻 Ghost mode

A master toggle that gates the stealth sub-options:

- **Hide online status** — others never see you online (`account.updateStatus` is suppressed).
- **Always appear offline** — force offline regardless of activity.
- **Hide typing indicator** — never broadcast the “typing…/recording…” state.
- **Don't mark messages as read** — read receipts are not sent (read history is intercepted client-side).
- **Don't mark stories as viewed** — story views are not reported.
- **Mark read when replying or reacting** — optional exception: replying/reacting marks the chat read.
- **Read to here** — context-menu action to mark a chat read up to a specific message, on demand, even with read-suppression on.

### 🟢 Online status

- **Eternal online** — keep yourself permanently “online” by continuously re-asserting online status; applies immediately when toggled.

### 🗂️ Message recovery & history

- **Save deleted messages** — deleted messages stay in the chat (dimmed, with a trash badge) instead of disappearing; persisted locally.
- **Edit history** — view every previous version of an edited message.
- **Reply to a deleted message** — hitting *Reply* on a recovered message quotes its author + text into the composer.
- **View-once / ephemeral capture** — view-once photos/videos and voice/round notes are saved locally so they don't vanish; a timer badge marks ephemeral media.

### 🚫 Anti-restriction

- **Bypass protected content** — save/forward from chats that disable saving (forwards are re-sent as fresh messages/media from you).
- **Block ads** — sponsored messages are dropped before display.

### 🎭 Fakes & UI

- **Fake Premium** — show yourself as Premium locally (propagates everywhere the client checks `isPremium`).
- **Fake Stars** — show an arbitrary local Stars balance (configurable amount).
- **Square avatars** — square user-pics and single-segment story rings.
- **Hide reactions** / **Hide comments**.
- **Archive opacity** — dim recovered/deleted items to taste.

### ✍️ Outgoing text pipeline

Applied to every outgoing message before it leaves the client:

- **Translate outgoing** — auto-translate your message to a target language (Google translate endpoint, no API key).
- **Anti-caps** — normalise SHOUTING text.
- **Auto-format** — automatic formatting pass.

### ⏱️ Send delay

- **Delay sending** — text/media are sent with a configurable delay using Telegram's server-side scheduling (`schedule_date`), with independent text/media durations.

### 🎨 Appearance

- **Custom font** — load a `.ttf` and use it across the UI.
- **Round app icon** — circular taskbar / dock / tray / `.exe` icon.

All of the above live under **Settings → Ghostgram**.

---

## Building

API credentials (`TDESKTOP_API_ID` / `TDESKTOP_API_HASH`) are required to build. Get your own from <https://my.telegram.org>. **Never commit them.**

### Windows (x64, Release) — via GitHub Actions

The `.exe` is produced by [`.github/workflows/ghostgram-windows.yml`](.github/workflows/ghostgram-windows.yml). It runs on `windows-latest`, builds all native dependencies (Qt 6, tg_owt, …), caches them, then builds `Ghostgram.exe` in Release. Push to the `ghostgram` branch (or run the workflow manually via *workflow_dispatch*) and download the `Ghostgram-Windows-x64-Release` artifact.

Key build flags used by CI:

```bat
configure.bat x64 qt6 ^
  -D TDESKTOP_API_ID=<id> -D TDESKTOP_API_HASH=<hash> ^
  -D CMAKE_CONFIGURATION_TYPES=Release ^
  -D DESKTOP_APP_DISABLE_AUTOUPDATE=ON ^
  -D DESKTOP_APP_DISABLE_CRASH_REPORTS=ON
cmake --build ..\out --config Release --parallel 2
```

### macOS (Apple Silicon, Xcode)

The Xcode project is generated by CMake into `out/`. Build the native dependencies once (Qt 6.2.13, tg_owt, rnnoise, tde2e), then:

```bash
# Regenerate the project after editing CMakeLists.txt:
cd out && cmake .

# Debug:
xcodebuild -project out/Telegram.xcodeproj -target Telegram \
  -configuration Debug ONLY_ACTIVE_ARCH=YES -arch arm64

# Release (always pass -arch arm64):
xcodebuild -project out/Telegram.xcodeproj -target Telegram \
  -configuration Release ONLY_ACTIVE_ARCH=YES -arch arm64
```

Output is `out/Debug/Ghostgram.app` or `out/Release/Ghostgram.app`. New source files must be added to `Telegram/CMakeLists.txt` and the project regenerated with `cmake .` — never add files through Xcode's project navigator (the project is fully generated).

### Linux / general

The upstream tdesktop build process applies. See upstream's [build instructions](https://github.com/telegramdesktop/tdesktop/tree/dev/docs) for the toolchain; the Ghostgram sources live under `Telegram/SourceFiles/ghostgram/`.

---

## Architecture

All fork code is isolated under `Telegram/SourceFiles/ghostgram/` in namespace `Ghostgram`, reached through one singleton:

```cpp
Ghostgram::Manager::Instance().config()        // toggles, persisted JSON
Ghostgram::Manager::Instance().ghostMode()     // online/typing/read suppression
Ghostgram::Manager::Instance().archive()       // deleted + edited messages
Ghostgram::Manager::Instance().mediaArchive()  // view-once / ephemeral media
```

Features integrate by branching on `Manager::Instance().config().xxx()` directly inside the relevant upstream code path — there is no separate event bus.

---

## Contributors

- [@ichmagmaus111](https://github.com/ichmagmaus111) — author & maintainer

---

## Credits & license

Ghostgram Desktop is built on [Telegram Desktop][tdesktop] by Telegram FZ-LLC and contributors. The original and this fork are licensed under **GPLv3 with the OpenSSL exception**. This is an independent, unofficial project.

[tdesktop]: https://github.com/telegramdesktop/tdesktop
