DROP TRIGGER selection_ad;
DROP TRIGGER selection_auk;

CREATE VIRTUAL TABLE selection_fts_new USING fts5(
	content,
	selection_id UNINDEXED,
	tokenize='trigram remove_diacritics 1'
);

INSERT INTO selection_fts_new (selection_id, content)
SELECT selection_id, substr(content, 1, 65536) FROM selection_fts;

DROP TABLE selection_fts;
ALTER TABLE selection_fts_new RENAME TO selection_fts;

CREATE TRIGGER selection_ad AFTER DELETE ON selection BEGIN
  DELETE FROM selection_fts WHERE selection_id = old.id;END;

CREATE TRIGGER selection_auk AFTER UPDATE OF keywords ON selection BEGIN
DELETE FROM selection_fts WHERE selection_id = old.id AND content = old.keywords; INSERT INTO selection_fts (selection_id, content) VALUES (new.id, new.keywords); END;
