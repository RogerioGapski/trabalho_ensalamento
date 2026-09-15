ALTER TABLE salas ADD COLUMN andar INTEGER NOT NULL DEFAULT 0;
ALTER TABLE predios ADD COLUMN instrucoes_acesso TEXT;

CREATE INDEX idx_salas_andar ON salas(predio_id, andar);