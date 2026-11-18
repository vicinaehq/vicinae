CREATE TABLE message_part_new (
    id INTEGER PRIMARY KEY,
    message_id INTEGER NOT NULL REFERENCES message(id) ON DELETE CASCADE,
    position INTEGER NOT NULL,
    kind TEXT NOT NULL CHECK(kind IN ('text', 'image', 'resource', 'tool_call', 'tool_result')),
    UNIQUE(message_id, position)
);

INSERT INTO message_part_new SELECT * FROM message_part;
DROP TABLE message_part;
ALTER TABLE message_part_new RENAME TO message_part;

CREATE TABLE resource_part (
    part_id INTEGER PRIMARY KEY REFERENCES message_part(id) ON DELETE CASCADE,
    id TEXT NOT NULL UNIQUE,
    name TEXT,
    mime_type TEXT,
    uri TEXT,
    path TEXT,
    status INTEGER NOT NULL
);

ALTER TABLE turn ADD COLUMN reasoning_effort TEXT;
