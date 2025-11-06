CREATE TABLE IF NOT EXISTS oauth_token_set (
	extension_id TEXT NOT NULL,
	provider_id TEXT NOT NULL DEFAULT '',
	access_token TEXT NOT NULL,
	refresh_token TEXT,
	id_token TEXT,
	scope TEXT,
	expires_in INT,
	updated_at INT NOT NULL,
	PRIMARY KEY (extension_id, provider_id)
);
