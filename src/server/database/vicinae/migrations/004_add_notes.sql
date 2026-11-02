CREATE TABLE IF NOT EXISTS note (
	id TEXT PRIMARY KEY,
	title TEXT NOT NULL,
	body TEXT NOT NULL DEFAULT '',
	pinned_at INTEGER, -- if NULL, not pinned
	created_at INTEGER NOT NULL,
	updated_at INTEGER NOT NULL,
	last_used_at INTEGER
);

CREATE INDEX IF NOT EXISTS idx_note_pinned_updated
ON note(
	pinned_at DESC,
	updated_at DESC
);
