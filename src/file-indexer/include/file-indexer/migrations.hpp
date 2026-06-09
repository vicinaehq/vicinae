#pragma once
#include <string_view>

namespace file_indexer {

inline constexpr int SCHEMA_VERSION = 3;

inline constexpr std::string_view INIT_SQL = R"sql(
CREATE TABLE IF NOT EXISTS scan_history (
		id INTEGER PRIMARY KEY AUTOINCREMENT,
		status INTEGER NOT NULL,
		created_at INT DEFAULT (unixepoch()),
		entrypoint TEXT NOT NULL,
		error TEXT,
		type INT NOT NULL, -- full/incremental
		indexed_file_count INT DEFAULT 0
);

CREATE TABLE IF NOT EXISTS indexed_file (
	id INTEGER PRIMARY KEY AUTOINCREMENT,
	path TEXT UNIQUE NOT NULL,
	last_modified_at INT,
	relevancy_score REAL NOT NULL
);

CREATE VIRTUAL TABLE IF NOT EXISTS unicode_idx USING fts5(
	path, content=indexed_file, tokenize='unicode61'
);

CREATE TRIGGER IF NOT EXISTS unicode_idx_ai AFTER INSERT ON indexed_file BEGIN
  INSERT INTO unicode_idx(rowid, path) VALUES (new.id, new.path);END;

CREATE TRIGGER IF NOT EXISTS unicode_idx_ad AFTER DELETE ON indexed_file BEGIN
  INSERT INTO unicode_idx(unicode_idx, rowid, path) VALUES('delete', old.id, old.path);END;
)sql";

}; // namespace file_indexer
