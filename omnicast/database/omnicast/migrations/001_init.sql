CREATE TABLE IF NOT EXISTS bookmarks (
		id INTEGER PRIMARY KEY AUTOINCREMENT,
		name TEXT NOT NULL,
		icon TEXT NOT NULL,
		url TEXT NOT NULL,
		app TEXT NOT NULL,
		open_count INTEGER DEFAULT 0,
		created_at INTEGER DEFAULT (unixepoch()),
		last_used_at INTEGER
);

CREATE TABLE IF NOT EXISTS root_provider (
	id TEXT PRIMARY KEY,
	preference_values JSON DEFAULT '{}',
	enabled INT DEFAULT 1
);

CREATE TABLE IF NOT EXISTS root_provider_item (
	id TEXT PRIMARY KEY,
	provider_id TEXT,
	preference_values JSON DEFAULT '{}',
	enabled INT DEFAULT 1,
	fallback INT DEFAULT 0,
	fallback_position INT DEFAULT -1,
	alias TEXT DEFAULT '',
	visit_count INT DEFAULT 0,
	last_visited_at INT,
	FOREIGN KEY(provider_id)
	REFERENCES root_provider(id)
	ON DELETE CASCADE
);

CREATE TABLE IF NOT EXISTS storage_data_item (
	id INTEGER PRIMARY KEY AUTOINCREMENT,
	namespace_id TEXT NOT NULL,
	value_type INT NOT NULL,
	key TEXT NOT NULL,
	value TEXT NOT NULL,
	UNIQUE(namespace_id, key)
);

CREATE TABLE IF NOT EXISTS calculator_history (
	id INTEGER PRIMARY KEY AUTOINCREMENT,
	expression TEXT NOT NULL,
	result TEXT NOT NULL,
	created_at INTEGER DEFAULT (unixepoch())
);

CREATE TABLE IF NOT EXISTS ai_provider_config (
	id TEXT PRIMARY KEY,
	data JSON,
	enabled INT DEFAULT 1,
	created_at INTEGER DEFAULT (unixepoch()),
	updated_at INTEGER
);
