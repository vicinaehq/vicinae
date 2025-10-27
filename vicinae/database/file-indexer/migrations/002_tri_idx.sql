
CREATE VIRTUAL TABLE IF NOT EXISTS tri_idx USING fts5(name, content='indexed_file',
  tokenize='trigram');

INSERT INTO tri_idx(tri_idx) VALUES('rebuild');

-- Triggers to keep the FTS index up to date.

CREATE TRIGGER tri_idx_ai AFTER INSERT ON indexed_file BEGIN
  INSERT INTO tri_idx(rowid, name) VALUES (new.id, new.name);END;

CREATE TRIGGER tri_idx_ad AFTER DELETE ON indexed_file BEGIN
  INSERT INTO tri_idx(tri_idx, rowid, name) VALUES('delete', old.id, old.name);END; 
