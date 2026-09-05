#include <windows.h>
#include <cstddef>
#include <cstdint>
#include <cstring>
#include <span>
#include <qlogging.h>
#include "everything-window-client.hpp"

namespace {

constexpr const wchar_t *EVERYTHING_WINDOW_CLASS = L"EVERYTHING_TASKBAR_NOTIFICATION";
constexpr const wchar_t *REPLY_WINDOW_CLASS = L"VicinaeEverythingReply";
constexpr DWORD COPYDATA_QUERY2W = 18;
constexpr DWORD REPLY_COPYDATA_ID = 0x56494349;
constexpr DWORD REQUEST_FULL_PATH_AND_NAME = 0x00000004;
constexpr DWORD SEARCH_REGEX = 0x00000008;
constexpr DWORD SORT_DATE_RECENTLY_CHANGED_DESCENDING = 22;
constexpr DWORD ITEM_FOLDER = 0x00000001;
constexpr DWORD REPLY_TIMEOUT_MS = 5000;

#pragma pack(push, 1)
struct Query2Header {
  DWORD replyHwnd;
  DWORD replyCopydataMessage;
  DWORD searchFlags;
  DWORD offset;
  DWORD maxResults;
  DWORD requestFlags;
  DWORD sortType;
};

struct List2Header {
  DWORD totalItems;
  DWORD numItems;
  DWORD offset;
  DWORD requestFlags;
  DWORD sortType;
};

struct Item2 {
  DWORD flags;
  DWORD dataOffset;
};
#pragma pack(pop)

struct ReplyState {
  std::vector<std::byte> data;
  bool received = false;
};

LRESULT CALLBACK replyWndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {
  if (msg == WM_COPYDATA) {
    auto *cds = reinterpret_cast<COPYDATASTRUCT *>(lParam);
    auto *state = reinterpret_cast<ReplyState *>(GetWindowLongPtrW(hwnd, GWLP_USERDATA));

    if (cds && state && cds->dwData == REPLY_COPYDATA_ID) {
      auto *bytes = static_cast<const std::byte *>(cds->lpData);
      state->data.assign(bytes, bytes + cds->cbData);
      state->received = true;
      return TRUE;
    }
  }

  return DefWindowProcW(hwnd, msg, wParam, lParam);
}

bool registerReplyClass() {
  static const bool registered = [] {
    WNDCLASSEXW wc{.cbSize = sizeof(WNDCLASSEXW),
                   .lpfnWndProc = replyWndProc,
                   .hInstance = GetModuleHandleW(nullptr),
                   .lpszClassName = REPLY_WINDOW_CLASS};

    return RegisterClassExW(&wc) != 0 || GetLastError() == ERROR_CLASS_ALREADY_EXISTS;
  }();

  return registered;
}

bool waitForReply(const ReplyState &state) {
  ULONGLONG const deadline = GetTickCount64() + REPLY_TIMEOUT_MS;

  while (!state.received) {
    ULONGLONG const now = GetTickCount64();

    if (now >= deadline) { return false; }

    DWORD const wait = MsgWaitForMultipleObjectsEx(0, nullptr, static_cast<DWORD>(deadline - now),
                                                   QS_ALLINPUT, MWMO_INPUTAVAILABLE);

    if (wait == WAIT_TIMEOUT || wait == WAIT_FAILED) { return false; }

    MSG msg;

    while (PeekMessageW(&msg, nullptr, 0, 0, PM_REMOVE)) {
      TranslateMessage(&msg);
      DispatchMessageW(&msg);
    }
  }

  return true;
}

template <typename T> std::optional<T> readAt(std::span<const std::byte> data, size_t offset) {
  if (offset > data.size() || data.size() - offset < sizeof(T)) { return std::nullopt; }

  T value;
  std::memcpy(&value, data.data() + offset, sizeof(T));
  return value;
}

std::vector<WinFileCandidate> parseList(std::span<const std::byte> data) {
  std::vector<WinFileCandidate> candidates;
  auto header = readAt<List2Header>(data, 0);

  if (!header) { return candidates; }

  candidates.reserve(header->numItems);

  for (DWORD i = 0; i < header->numItems; ++i) {
    auto item = readAt<Item2>(data, sizeof(List2Header) + static_cast<size_t>(i) * sizeof(Item2));

    if (!item) { break; }

    auto length = readAt<DWORD>(data, item->dataOffset);

    if (!length) { continue; }

    size_t const textOffset = item->dataOffset + sizeof(DWORD);
    size_t const textBytes = static_cast<size_t>(*length) * sizeof(wchar_t);

    if (textOffset > data.size() || data.size() - textOffset < textBytes) { continue; }

    std::wstring path(*length, L'\0');
    std::memcpy(path.data(), data.data() + textOffset, textBytes);

    candidates.emplace_back(
        WinFileCandidate{.path = std::move(path), .isDirectory = (item->flags & ITEM_FOLDER) != 0});
  }

  return candidates;
}

} // namespace

std::unique_ptr<EverythingWindowClient> EverythingWindowClient::connect() {
  if (!FindWindowW(EVERYTHING_WINDOW_CLASS, nullptr)) { return nullptr; }
  return std::make_unique<EverythingWindowClient>();
}

bool EverythingWindowClient::isConnected() {
  return FindWindowW(EVERYTHING_WINDOW_CLASS, nullptr) != nullptr;
}

std::optional<std::vector<WinFileCandidate>> EverythingWindowClient::search(const EverythingSearch &search) {
  HWND const everything = FindWindowW(EVERYTHING_WINDOW_CLASS, nullptr);

  if (!everything || !registerReplyClass()) { return std::nullopt; }

  ReplyState state;
  HWND const reply = CreateWindowExW(0, REPLY_WINDOW_CLASS, L"", 0, 0, 0, 0, 0, HWND_MESSAGE, nullptr,
                                     GetModuleHandleW(nullptr), nullptr);

  if (!reply) { return std::nullopt; }

  SetWindowLongPtrW(reply, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(&state));

  size_t const textBytes = (search.text.size() + 1) * sizeof(wchar_t);
  std::vector<std::byte> query(sizeof(Query2Header) + textBytes);
  Query2Header const header{.replyHwnd = static_cast<DWORD>(reinterpret_cast<std::uintptr_t>(reply)),
                            .replyCopydataMessage = REPLY_COPYDATA_ID,
                            .searchFlags = search.regex ? SEARCH_REGEX : 0,
                            .offset = 0,
                            .maxResults = static_cast<DWORD>(search.maxResults),
                            .requestFlags = REQUEST_FULL_PATH_AND_NAME,
                            .sortType = SORT_DATE_RECENTLY_CHANGED_DESCENDING};

  std::memcpy(query.data(), &header, sizeof(header));
  std::memcpy(query.data() + sizeof(header), search.text.c_str(), textBytes);

  COPYDATASTRUCT cds{
      .dwData = COPYDATA_QUERY2W, .cbData = static_cast<DWORD>(query.size()), .lpData = query.data()};
  bool const ok = SendMessageW(everything, WM_COPYDATA, reinterpret_cast<WPARAM>(reply),
                               reinterpret_cast<LPARAM>(&cds)) != 0 &&
                  waitForReply(state);

  DestroyWindow(reply);

  if (!ok) {
    qWarning() << "EverythingWindowClient: no reply from Everything";
    return std::nullopt;
  }

  return parseList(state.data);
}
