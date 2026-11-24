-- Keywords written by a model do not count as a user modification of the selection.
ALTER TABLE selection ADD COLUMN keywords_generated INTEGER NOT NULL DEFAULT 0;
