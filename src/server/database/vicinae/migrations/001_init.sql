CREATE TABLE IF NOT EXISTS shortcut (
		id TEXT PRIMARY KEY,
		name TEXT NOT NULL,
		icon TEXT NOT NULL,
		url TEXT NOT NULL,
		app TEXT NOT NULL,
		open_count INTEGER DEFAULT 0,
		created_at INTEGER NOT NULL,
		updated_at INTEGER NOT NULL,
		last_used_at INTEGER
);

-- no used yet, but planned
CREATE TABLE IF NOT EXISTS shortcut_tag (
	id TEXT PRIMARY KEY,
	name TEXT NOT NULL,
	color INT NOT NULL
);

CREATE TABLE IF NOT EXISTS shortcut_tag_shortcut (
	shortcut_id TEXT NOT NULL,
	tag_id TEXT NOT NULL,
	PRIMARY KEY (shortcut_id, tag_id),
	FOREIGN KEY (shortcut_id) REFERENCES shortcut(id),
	FOREIGN KEY (tag_id) REFERENCES shortcut_tag(id)
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
	fallback INT DEFAULT 0, -- not used anymore, we only look at fallback_position
	fallback_position INT DEFAULT -1,
	alias TEXT DEFAULT '',
	favorite INT DEFAULT 0,
	visit_count INT DEFAULT 0,
	last_visited_at INT,
	-- ranking variants are used for frecency sorting and can be reset
	-- at the user's demand
	rank_visit_count INT DEFAULT 0,
	rank_last_visited_at INT,
	FOREIGN KEY(provider_id)
	REFERENCES root_provider(id)
	ON DELETE CASCADE
);

-- local storage
CREATE TABLE IF NOT EXISTS storage_data_item (
	namespace_id TEXT NOT NULL,
	value_type INT NOT NULL,
	key TEXT NOT NULL,
	value TEXT NOT NULL,
	PRIMARY KEY(namespace_id, key)
);

CREATE TABLE IF NOT EXISTS calculator_history (
	id TEXT PRIMARY KEY,
	type_hint INTEGER NOT NULL, -- unit conversion / regular arithmetic
	question TEXT NOT NULL,
	answer TEXT NOT NULL,
	created_at INTEGER NOT NULL,
	pinned_at INTEGER
);

CREATE TABLE IF NOT EXISTS visited_emoji (
	emoji TEXT PRIMARY KEY,
	pinned_at INTEGER, -- if NULL, not pinned
	last_visited_at INTEGER,
	visit_count INTEGER DEFAULT 0,
	custom_keywords TEXT
);
