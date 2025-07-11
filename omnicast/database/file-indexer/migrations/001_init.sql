CREATE TABLE IF NOT EXISTS scan_history (
		id INTEGER PRIMARY KEY AUTOINCREMENT,
		status INTEGER NOT NULL,
		created_at INT DEFAULT (unixepoch())
);

CREATE TABLE IF NOT EXISTS indexed_file (
	id INTEGER PRIMARY KEY AUTOINCREMENT,
	path TEXT UNIQUE NOT NULL,
	name TEXT NOT NULL,
	last_modified_at INT,
	relevancy_score REAL NOT NULL
);

CREATE VIRTUAL TABLE IF NOT EXISTS unicode_idx USING fts5(
	name, content=indexed_file, tokenize='unicode61', prefix='2 3 4 5 6'
);

-- Triggers to keep the FTS index up to date.

CREATE TRIGGER unicode_idx_ai AFTER INSERT ON indexed_file BEGIN
  INSERT INTO unicode_idx(rowid, name) VALUES (new.id, new.name);END;

CREATE TRIGGER unicode_idx_ad AFTER DELETE ON indexed_file BEGIN
  INSERT INTO unicode_idx(unicode_idx, rowid, name) VALUES('delete', old.id, old.name);END; 

CREATE INDEX idx_indexed_file_covering 
ON indexed_file(id, relevancy_score DESC, path);
