CREATE TABLE note (
    id TEXT PRIMARY KEY NOT NULL,
    title TEXT NOT NULL,
    markdown TEXT NOT NULL,
    created_at INTEGER NOT NULL,
    updated_at INTEGER NOT NULL
);
CREATE INDEX note_updated_at ON note(updated_at DESC);
