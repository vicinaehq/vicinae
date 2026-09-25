CREATE TABLE conversation (
    id TEXT PRIMARY KEY NOT NULL,
    title TEXT NOT NULL,
    model TEXT,
    created_at INTEGER NOT NULL,
    updated_at INTEGER NOT NULL
);

CREATE INDEX conversation_recent ON conversation(updated_at DESC, id);

CREATE TABLE turn (
    id TEXT PRIMARY KEY NOT NULL,
    conversation_id TEXT NOT NULL REFERENCES conversation(id) ON DELETE CASCADE,
    position INTEGER NOT NULL,
    query TEXT NOT NULL,
    status TEXT NOT NULL CHECK(status IN ('running', 'completed', 'failed', 'cancelled', 'interrupted')),
    model TEXT,
    model_label TEXT NOT NULL,
    thinking INTEGER NOT NULL,
    max_steps INTEGER NOT NULL,
    started_at INTEGER NOT NULL,
    finished_at INTEGER,
    error TEXT,
    UNIQUE(conversation_id, position)
);

CREATE TABLE turn_tool (
    turn_id TEXT NOT NULL REFERENCES turn(id) ON DELETE CASCADE,
    name TEXT NOT NULL,
    PRIMARY KEY(turn_id, name)
);

CREATE TABLE attachment (
    id TEXT PRIMARY KEY NOT NULL,
    turn_id TEXT NOT NULL REFERENCES turn(id) ON DELETE CASCADE,
    position INTEGER NOT NULL,
    name TEXT NOT NULL,
    mime_type TEXT NOT NULL,
    original_path TEXT NOT NULL UNIQUE,
    image_path TEXT,
    image_mime_type TEXT,
    text TEXT,
    UNIQUE(turn_id, position)
);

CREATE TABLE message (
    id INTEGER PRIMARY KEY,
    turn_id TEXT NOT NULL REFERENCES turn(id) ON DELETE CASCADE,
    position INTEGER NOT NULL,
    role TEXT NOT NULL CHECK(role IN ('user', 'assistant', 'tool')),
    UNIQUE(turn_id, position)
);

CREATE TABLE message_part (
    id INTEGER PRIMARY KEY,
    message_id INTEGER NOT NULL REFERENCES message(id) ON DELETE CASCADE,
    position INTEGER NOT NULL,
    kind TEXT NOT NULL CHECK(kind IN ('text', 'image', 'tool_call', 'tool_result')),
    UNIQUE(message_id, position)
);

CREATE TABLE text_part (
    part_id INTEGER PRIMARY KEY REFERENCES message_part(id) ON DELETE CASCADE,
    text TEXT NOT NULL
);

CREATE TABLE image_part (
    part_id INTEGER PRIMARY KEY REFERENCES message_part(id) ON DELETE CASCADE,
    attachment_id TEXT NOT NULL REFERENCES attachment(id)
);

CREATE TABLE tool_call (
    part_id INTEGER PRIMARY KEY REFERENCES message_part(id) ON DELETE CASCADE,
    provider_id TEXT NOT NULL,
    name TEXT NOT NULL,
    arguments TEXT NOT NULL,
    summary TEXT,
    status TEXT NOT NULL CHECK(status IN ('queued', 'running', 'succeeded', 'failed', 'cancelled', 'interrupted')),
    duration_ms INTEGER
);

CREATE TABLE tool_result (
    part_id INTEGER PRIMARY KEY REFERENCES message_part(id) ON DELETE CASCADE,
    call_part_id INTEGER NOT NULL UNIQUE REFERENCES tool_call(part_id) ON DELETE CASCADE,
    content TEXT NOT NULL,
    display_text TEXT,
    status_text TEXT,
    failed INTEGER NOT NULL CHECK(failed IN (0, 1))
);
