CREATE TABLE IF NOT EXISTS oauth_token_set (
	id TEXT PRIMARY KEY,
	access_token TEXT NOT NULL,
	refresh_token TEXT,
	id_token TEXT,
	scope TEXT,
	expires_in INT,
	updated_at INT
);
