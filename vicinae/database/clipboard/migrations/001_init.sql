-- We use a separate SQLite database for clipboard history as it can become
-- a very big thing on its own and will benefit from database-level specific optimisations.

CREATE TABLE IF NOT EXISTS selection (
	id INTEGER PRIMARY KEY AUTOINCREMENT,
	hash_md5 TEXT NOT NULL,
	preferred_mime_type TEXT NOT NULL,
	source TEXT,
	offer_count TEXT,
	created_at INTEGER DEFAULT (unixepoch()),
	pinned_at INTEGER,
	tags JSON DEFAULT '[]'
);

CREATE TABLE IF NOT EXISTS data_offer (
	id INTEGER PRIMARY KEY AUTOINCREMENT,
	mime_type TEXT NOT NULL,
	text_preview TEXT,
	content_hash_md5 TEXT NOT NULL,
	encryption_type TEXT NOT NULL DEFAULT 'none', -- 'none' | 'local' | 'sync' (none can only happen if no keychain backend is available)
	selection_id INTEGER,
	FOREIGN KEY(selection_id)
	REFERENCES selection(id)
	ON DELETE CASCADE
);

CREATE VIRTUAL TABLE IF NOT EXISTS selection_fts USING fts5(
	content,
	selection_id,
	tokenize='porter'
);

CREATE INDEX IF NOT EXISTS idx_selection_pinned_created 
ON selection(
	pinned_at DESC, 
	created_at DESC
);

CREATE INDEX IF NOT EXISTS idx_data_offer_selection_id 
ON data_offer(
	selection_id, 
	mime_type
);

CREATE INDEX IF NOT EXISTS idx_selection_preferred_mime 
ON selection(
	preferred_mime_type
);
