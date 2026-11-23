#include <algorithm>
#include <cstdint>
#include <format>
#include <ranges>
#include <span>
#include <QDateTime>
#include <QUuid>
#include "notes-tools.hpp"
#include "notes-search.hpp"
#include "services/notes/notes-store.hpp"

// not anonymous: glaze can't reflect types with internal linkage
namespace notes_tools {

enum class WriteMode : std::uint8_t { Replace, Append };

} // namespace notes_tools

template <> struct glz::meta<notes_tools::WriteMode> {
  using enum notes_tools::WriteMode;
  static constexpr auto value = glz::enumerate("replace", Replace, "append", Append);
};

namespace notes_tools {

constexpr int DEFAULT_LIMIT = 10;
constexpr int MAX_LIMIT = 30;
constexpr std::size_t MAX_PREVIEW_BYTES = 200;
constexpr std::size_t MAX_NOTE_BYTES = 64 * 1024;
constexpr std::size_t REVISION_LENGTH = 16;

struct SearchInput {
  std::string query;
  std::optional<int> limit;
};

struct SearchResult {
  std::string path;
  std::string title;
  std::string modified_at;
  std::string preview;
  bool read_only;
};

struct ReadInput {
  std::string path;
};

struct ReadOutput {
  std::string path;
  std::string title;
  std::string revision;
  bool read_only;
  bool truncated;
  std::string content;
};

struct WriteInput {
  std::optional<std::string> path;
  std::optional<std::string> title;
  std::string content;
  std::optional<WriteMode> mode;
  std::optional<std::string> revision;
};

struct WriteOutput {
  std::string path;
  std::string title;
  std::string revision;
};

using RawToolResult = AI::AbstractTool::RawToolResult;
using NoteList = Notes::Result<std::vector<Notes::Note>>;

AI::AbstractTool::RawToolTask ready(RawToolResult result) {
  return {QtFuture::makeReadyValueFuture(std::move(result)), {}};
}

template <typename T> RawToolResult serialize(const T &value) {
  std::string out;
  if (const auto error = glz::write_json(value, out)) return std::unexpected(glz::format_error(error));
  return AI::ToolOutput{.content = std::move(out)};
}

std::string revision(const Notes::Note &note) { return note.revision.substr(0, REVISION_LENGTH); }

// Only notes the store lists can be addressed, so a path never reaches an arbitrary file.
std::expected<Notes::Note, std::string> findNote(const NoteList &notes, std::string_view path) {
  if (!notes) return std::unexpected(notes.error());
  const auto it = std::ranges::find(*notes, path, &Notes::Note::path);
  if (it == notes->end())
    return std::unexpected(std::format("There is no note at {}. Use notes_search to find notes.", path));
  return *it;
}

RawToolResult written(const Notes::Result<Notes::Note> &note) {
  if (!note) return std::unexpected(note.error());
  return serialize(WriteOutput{.path = note->path, .title = note->title, .revision = revision(*note)});
}

} // namespace notes_tools

using namespace notes_tools;

std::string SearchNotesTool::description() const {
  return "Search the user's Markdown notes by title and content. An empty query lists the most recently "
         "modified notes. Returns the path of each note with a short preview. Use notes_read with a path "
         "to get the full content.";
}

std::string SearchNotesTool::generateInputSchema() const {
  return R"({"type":"object","properties":{"query":{"type":"string","description":"Text to search for. Empty to list recent notes."},"limit":{"type":"integer","minimum":1,"maximum":30,"description":"Maximum number of notes. Defaults to 10."}},"required":["query"],"additionalProperties":false})";
}

std::optional<std::string> SearchNotesTool::invocationSummary(std::string_view arguments) const {
  SearchInput input;
  if (glz::read_json(input, arguments) || input.query.empty()) return std::nullopt;
  return input.query;
}

AI::AbstractTool::RawToolTask SearchNotesTool::runRaw(std::string_view arguments) {
  SearchInput input;
  if (const auto error = glz::read_json(input, arguments))
    return ready(std::unexpected(glz::format_error(error, arguments)));

  const auto limit = std::clamp(input.limit.value_or(DEFAULT_LIMIT), 1, MAX_LIMIT);
  auto future = m_store.list().then(
      &m_store, [query = std::move(input.query), limit](const NoteList &notes) -> RawToolResult {
        if (!notes) return std::unexpected(notes.error());
        std::vector<Scored<int>> matches;
        fuzzy::fuzzyFilter(std::span<const Notes::Note>(*notes), query, matches);

        std::vector<SearchResult> results;
        results.reserve(std::min<std::size_t>(limit, matches.size()));
        for (const auto &match : matches | std::views::take(limit)) {
          const auto &note = (*notes)[match.data];
          results.emplace_back(SearchResult{
              .path = note.path,
              .title = note.title,
              .modified_at =
                  QDateTime::fromMSecsSinceEpoch(note.updatedAt).toString(Qt::ISODate).toStdString(),
              .preview = std::string(AI::truncateUtf8(note.markdown, MAX_PREVIEW_BYTES).text),
              .read_only = note.readOnly,
          });
        }
        return serialize(results);
      });
  return {std::move(future), {}};
}

std::string ReadNoteTool::description() const {
  return "Read a note, using a path returned by notes_search. Returns the Markdown content and the "
         "revision that notes_write needs to replace it. Content is limited to 64 KiB: a truncated note "
         "can only be appended to.";
}

std::string ReadNoteTool::generateInputSchema() const {
  return R"({"type":"object","properties":{"path":{"type":"string","description":"Path of the note."}},"required":["path"],"additionalProperties":false})";
}

std::optional<std::string> ReadNoteTool::invocationSummary(std::string_view arguments) const {
  ReadInput input;
  if (glz::read_json(input, arguments)) return std::nullopt;
  return input.path;
}

AI::AbstractTool::RawToolTask ReadNoteTool::runRaw(std::string_view arguments) {
  ReadInput input;
  if (const auto error = glz::read_json(input, arguments))
    return ready(std::unexpected(glz::format_error(error, arguments)));

  auto future =
      m_store.list().then(&m_store, [path = std::move(input.path)](const NoteList &notes) -> RawToolResult {
        const auto note = findNote(notes, path);
        if (!note) return std::unexpected(note.error());
        const auto [text, truncated] = AI::truncateUtf8(note->markdown, MAX_NOTE_BYTES);
        return serialize(ReadOutput{.path = note->path,
                                    .title = note->title,
                                    .revision = revision(*note),
                                    .read_only = note->readOnly,
                                    .truncated = truncated,
                                    .content = std::string(text)});
      });
  return {std::move(future), {}};
}

std::string WriteNoteTool::description() const {
  return "Create a note or change an existing one. Without a path, creates a note from title and content "
         "in the user's default notes directory. With a path, mode \"replace\" (the default) overwrites "
         "the whole content and requires the revision returned by notes_read, while mode \"append\" adds "
         "content at the end. Passing a title with a path renames the note. Content is Markdown. Returns "
         "the path of the note, which changes when it is renamed.";
}

std::string WriteNoteTool::generateInputSchema() const {
  return R"({"type":"object","properties":{"path":{"type":"string","description":"Path of the note to change. Omit to create a note."},"title":{"type":"string","description":"Title of the note. Required when creating a note."},"content":{"type":"string","description":"Markdown content."},"mode":{"type":"string","enum":["replace","append"],"description":"How content is applied to an existing note. Defaults to replace."},"revision":{"type":"string","description":"Revision returned by notes_read. Required to replace the content of a note."}},"required":["content"],"additionalProperties":false})";
}

bool WriteNoteTool::requiresApproval(std::string_view arguments) const {
  WriteInput input;
  if (glz::read_json(input, arguments)) return false;
  return input.path && input.mode != WriteMode::Append;
}

std::optional<std::string> WriteNoteTool::invocationSummary(std::string_view arguments) const {
  WriteInput input;
  if (glz::read_json(input, arguments)) return std::nullopt;
  return std::format("{}\n\n{}", input.path ? *input.path : input.title.value_or(std::string{}),
                     input.content);
}

AI::AbstractTool::RawToolTask WriteNoteTool::runRaw(std::string_view arguments) {
  WriteInput input;
  if (const auto error = glz::read_json(input, arguments))
    return ready(std::unexpected(glz::format_error(error, arguments)));

  if (!input.path) {
    if (!input.title || input.title->empty())
      return ready(std::unexpected(std::string("A title is required to create a note.")));
    if (input.mode == WriteMode::Append)
      return ready(std::unexpected(std::string("Appending requires the path of an existing note.")));
    Notes::Note note{.id = QUuid::createUuid().toString(QUuid::WithoutBraces).toStdString(),
                     .title = std::move(*input.title),
                     .markdown = std::move(input.content)};
    return {m_store.save(std::move(note), true).then(&m_store, written), {}};
  }

  auto future =
      m_store.list()
          .then(
              &m_store,
              [&store = m_store, input = std::move(input)](const NoteList &notes) -> QFuture<RawToolResult> {
                const auto fail = [](std::string message) {
                  return QtFuture::makeReadyValueFuture<RawToolResult>(std::unexpected(std::move(message)));
                };
                auto note = findNote(notes, *input.path);
                if (!note) return fail(std::move(note.error()));
                if (note->readOnly) return fail("This note is read-only.");

                if (input.mode == WriteMode::Append) {
                  if (!note->markdown.empty() && !note->markdown.ends_with('\n')) note->markdown += '\n';
                  note->markdown += input.content;
                } else {
                  if (note->markdown.size() > MAX_NOTE_BYTES)
                    return fail("This note is too large to be replaced. Append to it instead.");
                  if (input.revision != revision(*note))
                    return fail("The revision is missing or the note changed since it was read. Read it "
                                "again with notes_read before replacing its content.");
                  note->markdown = input.content;
                }
                if (input.title) note->title = *input.title;
                return store.save(std::move(*note), false).then(&store, written);
              })
          .unwrap();
  return {std::move(future), {}};
}
