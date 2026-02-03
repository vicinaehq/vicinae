-- We use a separate SQLite database for clipboard history as it can become
-- a very big thing on its own and will benefit from database-level specific optimisations.

CREATE TABLE IF NOT EXISTS selection (
	id TEXT PRIMARY KEY,
	hash_md5 TEXT NOT NULL,
	preferred_mime_type TEXT NOT NULL,
	source TEXT,
	offer_count INTEGER,
	created_at INTEGER NOT NULL,
	updated_at INTEGER NOT NULL, -- updated when the same selection is reselected
	pinned_at INTEGER,
	kind INTEGER,
	keywords TEXT DEFAULT '' -- additional user defined text to index
);

CREATE TABLE IF NOT EXISTS data_offer (
	id TEXT PRIMARY KEY,
	selection_id TEXT,
	mime_type TEXT NOT NULL,
	text_preview TEXT,
	content_hash_md5 TEXT NOT NULL,
	size INTEGER NOT NULL,
	encryption_type INT NOT NULL, 
	kind INT NOT NULL,
	url_host TEXT, -- only applicable to web url types: used to quickly fetch favicon
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

-- always delete indexed content when a selection gets deleted
-- content insertion is done programatically as it depends on the actual selection content
-- which is not stored in database.
CREATE TRIGGER selection_ad AFTER DELETE ON selection BEGIN
  DELETE FROM selection_fts WHERE selection_id = old.id;END;

-- keep keyword index in sync
CREATE TRIGGER selection_auk AFTER UPDATE OF keywords ON selection BEGIN
DELETE FROM selection_fts WHERE selection_id = old.id AND content = old.keywords; INSERT INTO selection_fts (selection_id, content) VALUES (new.id, new.keywords); END;
