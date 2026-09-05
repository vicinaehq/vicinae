#include <windows.h>
#include <Everything3.h>
#include <cstddef>
#include <string_view>
#include <qlogging.h>
#include "everything-pipe-client.hpp"

namespace {

constexpr std::size_t PATH_BUFFER_BYTES = 32768;

struct SearchStateDeleter {
  void operator()(EVERYTHING3_SEARCH_STATE *state) const { Everything3_DestroySearchState(state); }
};

struct ResultListDeleter {
  void operator()(EVERYTHING3_RESULT_LIST *list) const { Everything3_DestroyResultList(list); }
};

} // namespace

std::unique_ptr<EverythingPipeClient> EverythingPipeClient::connect(const std::wstring &instanceName) {
  EVERYTHING3_CLIENT *client = Everything3_ConnectW(instanceName.empty() ? nullptr : instanceName.c_str());

  if (!client) { return nullptr; }

  return std::make_unique<EverythingPipeClient>(client);
}

EverythingPipeClient::EverythingPipeClient(_everything3_client_s *client) : m_client(client) {}

EverythingPipeClient::~EverythingPipeClient() { Everything3_DestroyClient(m_client); }

bool EverythingPipeClient::isConnected() { return Everything3_GetIPCPipeVersion(m_client) != 0; }

std::optional<std::vector<WinFileCandidate>> EverythingPipeClient::search(const EverythingSearch &search) {
  std::unique_ptr<EVERYTHING3_SEARCH_STATE, SearchStateDeleter> state{Everything3_CreateSearchState()};

  if (!state) { return std::nullopt; }

  Everything3_SetSearchTextW(state.get(), search.text.c_str());
  Everything3_SetSearchRegex(state.get(), search.regex ? TRUE : FALSE);
  Everything3_AddSearchSort(state.get(), EVERYTHING3_PROPERTY_ID_DATE_RECENTLY_CHANGED, FALSE);
  Everything3_SetSearchViewportOffset(state.get(), 0);
  Everything3_SetSearchViewportCount(state.get(), static_cast<EVERYTHING3_SIZE_T>(search.maxResults));
  Everything3_AddSearchPropertyRequest(state.get(), EVERYTHING3_PROPERTY_ID_PATH_AND_NAME);

  std::unique_ptr<EVERYTHING3_RESULT_LIST, ResultListDeleter> results{
      Everything3_Search(m_client, state.get())};

  if (!results) {
    qWarning() << "EverythingPipeClient: search failed" << Qt::hex << Everything3_GetLastError();
    return std::nullopt;
  }

  EVERYTHING3_SIZE_T const count = Everything3_GetResultListViewportCount(results.get());
  std::vector<WinFileCandidate> candidates;
  std::string buffer(PATH_BUFFER_BYTES, '\0');

  candidates.reserve(count);

  for (EVERYTHING3_SIZE_T i = 0; i < count; ++i) {
    EVERYTHING3_SIZE_T const length = Everything3_GetResultPropertyTextUTF8(
        results.get(), i, EVERYTHING3_PROPERTY_ID_PATH_AND_NAME,
        reinterpret_cast<EVERYTHING3_UTF8 *>(buffer.data()), buffer.size());

    if (length == 0 || length >= buffer.size()) { continue; }

    candidates.emplace_back(
        WinFileCandidate{.path = std::string_view{buffer.data(), length},
                         .isDirectory = Everything3_IsFolderResult(results.get(), i) != FALSE});
  }

  return candidates;
}
