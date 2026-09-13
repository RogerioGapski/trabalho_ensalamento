CREATE TABLE sessoes (
    id UUID PRIMARY KEY DEFAULT gen_random_uuid(),
    usuario_id UUID NOT NULL REFERENCES usuarios(id) ON DELETE CASCADE,
    token_hash TEXT NOT NULL UNIQUE,
    criado_em TIMESTAMPTZ NOT NULL DEFAULT now(),
    expira_em TIMESTAMPTZ NOT NULL,
    revogada_em TIMESTAMPTZ
);

CREATE INDEX idx_sessoes_usuario_id ON sessoes(usuario_id);
CREATE INDEX idx_sessoes_token_hash ON sessoes(token_hash);