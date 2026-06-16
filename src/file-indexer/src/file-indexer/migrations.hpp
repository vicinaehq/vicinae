#pragma once
#include <string_view>

namespace file_indexer {

inline constexpr int SCHEMA_VERSION = 8;

inline constexpr std::string_view INIT_SQL = R"sql(
CREATE TABLE IF NOT EXISTS scan_history (
		id INTEGER PRIMARY KEY AUTOINCREMENT,
		status INTEGER NOT NULL,
		created_at INT DEFAULT (unixepoch()),
		finished_at INT, -- null while the scan is running
		entrypoint TEXT NOT NULL,
		error TEXT,
		type INT NOT NULL, -- full/incremental
		indexed_file_count INT DEFAULT 0
);

CREATE TABLE IF NOT EXISTS indexed_file (
	id INTEGER PRIMARY KEY AUTOINCREMENT,
	path TEXT UNIQUE NOT NULL,
	parent_id INT,
	last_modified_at INT,
	type INT NOT NULL DEFAULT 0 -- 0 file, 1 directory
);

CREATE INDEX IF NOT EXISTS indexed_file_parent_id_idx ON indexed_file(parent_id);

-- serves the recently-active-directories query backing the dynamic watch set
CREATE INDEX IF NOT EXISTS indexed_file_dir_mtime_idx
	ON indexed_file(last_modified_at DESC) WHERE type = 1;

CREATE VIRTUAL TABLE IF NOT EXISTS unicode_idx USING fts5(
	path, content=indexed_file, tokenize='fuzzy_trigram'
);

CREATE TRIGGER IF NOT EXISTS unicode_idx_ai AFTER INSERT ON indexed_file BEGIN
  INSERT INTO unicode_idx(rowid, path) VALUES (new.id, new.path);END;

CREATE TRIGGER IF NOT EXISTS unicode_idx_ad AFTER DELETE ON indexed_file BEGIN
  INSERT INTO unicode_idx(unicode_idx, rowid, path) VALUES('delete', old.id, old.path);END;

-- trigram index over abbreviation skeletons ('dwnlds', 'cfg'); the tokenizer
-- skeletonizes both documents and queries.
CREATE VIRTUAL TABLE IF NOT EXISTS skeleton_idx USING fts5(
	path, content='', contentless_delete=1, tokenize='fuzzy_trigram skeleton 1 skipgrams 1'
);

CREATE TRIGGER IF NOT EXISTS skeleton_idx_ai AFTER INSERT ON indexed_file BEGIN
  INSERT INTO skeleton_idx(rowid, path) VALUES (new.id, new.path);END;

CREATE TRIGGER IF NOT EXISTS skeleton_idx_ad AFTER DELETE ON indexed_file BEGIN
  DELETE FROM skeleton_idx WHERE rowid = old.id;END;

-- spellfix1 typo-correction vocabulary, fully rebuilt from indexed_file basenames
-- after successful scans. Derived data: no schema version bump needed.
CREATE VIRTUAL TABLE IF NOT EXISTS spellfix_vocab USING spellfix1;
)sql";

}; // namespace file_indexer
