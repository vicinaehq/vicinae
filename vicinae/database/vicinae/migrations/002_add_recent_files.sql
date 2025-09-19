CREATE TABLE IF NOT EXISTS recent_files (
	path TEXT PRIMARY KEY,
	access_count INT DEFAULT 1,
	last_accessed_at INT DEFAULT (unixepoch())
);
