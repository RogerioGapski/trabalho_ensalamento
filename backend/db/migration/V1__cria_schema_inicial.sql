CREATE EXTENSION IF NOT EXISTS "pgcrypto";

CREATE TYPE papel_usuario AS ENUM ('admin', 'coord', 'prof', 'aluno');
CREATE TYPE tipo_sala AS ENUM ('sala_aula', 'laboratorio', 'auditorio', 'sala_reuniao', 'quadra', 'outro');
CREATE TYPE dia_semana AS ENUM ('segunda', 'terca', 'quarta', 'quinta', 'sexta', 'sabado', 'domingo');
CREATE TYPE status_alocacao AS ENUM ('pendente', 'ativa', 'cancelada', 'conflito');
CREATE TYPE tipo_operacao AS ENUM ('insert', 'update', 'delete');

CREATE FUNCTION set_atualizado_em()
RETURNS TRIGGER AS $$
BEGIN
    NEW.atualizado_em = now();
    RETURN NEW;
END;
$$ LANGUAGE plpgsql;

CREATE TABLE usuarios (
    id UUID PRIMARY KEY DEFAULT gen_random_uuid(),
    nome VARCHAR(255) NOT NULL,
    email VARCHAR(255) NOT NULL UNIQUE,
    senha_hash TEXT NOT NULL,
    papel papel_usuario NOT NULL DEFAULT 'aluno',
    ativo BOOLEAN NOT NULL DEFAULT TRUE,
    criado_em TIMESTAMPTZ NOT NULL DEFAULT now(),
    atualizado_em TIMESTAMPTZ NOT NULL DEFAULT now()
);

CREATE TRIGGER trg_usuarios_atualizado_em
BEFORE UPDATE ON usuarios
FOR EACH ROW
EXECUTE FUNCTION set_atualizado_em();

CREATE TABLE periodos_letivos (
    id UUID PRIMARY KEY DEFAULT gen_random_uuid(),
    nome VARCHAR(100) NOT NULL,
    data_inicio DATE NOT NULL,
    data_fim DATE NOT NULL,
    ativo BOOLEAN NOT NULL DEFAULT TRUE,
    criado_em TIMESTAMPTZ NOT NULL DEFAULT now(),
    atualizado_em TIMESTAMPTZ NOT NULL DEFAULT now(),
    CONSTRAINT chk_periodo_datas CHECK (data_fim > data_inicio)
);

CREATE TRIGGER trg_periodos_letivos_atualizado_em
BEFORE UPDATE ON periodos_letivos
FOR EACH ROW
EXECUTE FUNCTION set_atualizado_em();

CREATE TABLE campi (
    id UUID PRIMARY KEY DEFAULT gen_random_uuid(),
    nome VARCHAR(150) NOT NULL,
    endereco VARCHAR(255),
    cidade VARCHAR(100),
    estado CHAR(2),
    criado_em TIMESTAMPTZ NOT NULL DEFAULT now(),
    atualizado_em TIMESTAMPTZ NOT NULL DEFAULT now()
);

CREATE TRIGGER trg_campi_atualizado_em
BEFORE UPDATE ON campi
FOR EACH ROW
EXECUTE FUNCTION set_atualizado_em();

CREATE TABLE predios (
    id UUID PRIMARY KEY DEFAULT gen_random_uuid(),
    campus_id UUID NOT NULL REFERENCES campi(id) ON DELETE CASCADE,
    nome VARCHAR(150) NOT NULL,
    criado_em TIMESTAMPTZ NOT NULL DEFAULT now(),
    atualizado_em TIMESTAMPTZ NOT NULL DEFAULT now(),
    CONSTRAINT uq_predio_campus_nome UNIQUE (campus_id, nome)
);

CREATE INDEX idx_predios_campus_id ON predios(campus_id);

CREATE TRIGGER trg_predios_atualizado_em
BEFORE UPDATE ON predios
FOR EACH ROW
EXECUTE FUNCTION set_atualizado_em();

CREATE TABLE salas (
    id UUID PRIMARY KEY DEFAULT gen_random_uuid(),
    predio_id UUID NOT NULL REFERENCES predios(id) ON DELETE CASCADE,
    nome VARCHAR(100) NOT NULL,
    tipo tipo_sala NOT NULL DEFAULT 'sala_aula',
    capacidade INTEGER NOT NULL,
    acessivel BOOLEAN NOT NULL DEFAULT FALSE,
    recursos JSONB NOT NULL DEFAULT '[]'::jsonb,
    ativo BOOLEAN NOT NULL DEFAULT TRUE,
    criado_em TIMESTAMPTZ NOT NULL DEFAULT now(),
    atualizado_em TIMESTAMPTZ NOT NULL DEFAULT now(),
    CONSTRAINT chk_sala_capacidade CHECK (capacidade > 0),
    CONSTRAINT uq_sala_predio_nome UNIQUE (predio_id, nome)
);

CREATE INDEX idx_salas_predio_id ON salas(predio_id);
CREATE INDEX idx_salas_tipo ON salas(tipo);
CREATE INDEX idx_salas_recursos ON salas USING GIN (recursos);

CREATE TRIGGER trg_salas_atualizado_em
BEFORE UPDATE ON salas
FOR EACH ROW
EXECUTE FUNCTION set_atualizado_em();

CREATE TABLE turmas (
    id UUID PRIMARY KEY DEFAULT gen_random_uuid(),
    periodo_letivo_id UUID NOT NULL REFERENCES periodos_letivos(id) ON DELETE CASCADE,
    professor_id UUID REFERENCES usuarios(id) ON DELETE SET NULL,
    codigo VARCHAR(50) NOT NULL,
    disciplina VARCHAR(255) NOT NULL,
    quantidade_alunos INTEGER NOT NULL,
    necessidades_obrigatorias JSONB NOT NULL DEFAULT '[]'::jsonb,
    preferencias JSONB NOT NULL DEFAULT '[]'::jsonb,
    ativo BOOLEAN NOT NULL DEFAULT TRUE,
    criado_em TIMESTAMPTZ NOT NULL DEFAULT now(),
    atualizado_em TIMESTAMPTZ NOT NULL DEFAULT now(),
    CONSTRAINT chk_turma_quantidade_alunos CHECK (quantidade_alunos >= 0),
    CONSTRAINT uq_turma_periodo_codigo UNIQUE (periodo_letivo_id, codigo)
);

CREATE INDEX idx_turmas_periodo_letivo_id ON turmas(periodo_letivo_id);
CREATE INDEX idx_turmas_professor_id ON turmas(professor_id);
CREATE INDEX idx_turmas_necessidades_obrigatorias ON turmas USING GIN (necessidades_obrigatorias);
CREATE INDEX idx_turmas_preferencias ON turmas USING GIN (preferencias);

CREATE TRIGGER trg_turmas_atualizado_em
BEFORE UPDATE ON turmas
FOR EACH ROW
EXECUTE FUNCTION set_atualizado_em();

CREATE TABLE encontros (
    id UUID PRIMARY KEY DEFAULT gen_random_uuid(),
    turma_id UUID NOT NULL REFERENCES turmas(id) ON DELETE CASCADE,
    dia_semana dia_semana NOT NULL,
    hora_inicio TIME NOT NULL,
    hora_fim TIME NOT NULL,
    criado_em TIMESTAMPTZ NOT NULL DEFAULT now(),
    atualizado_em TIMESTAMPTZ NOT NULL DEFAULT now(),
    CONSTRAINT chk_encontro_horario CHECK (hora_fim > hora_inicio)
);

CREATE INDEX idx_encontros_turma_id ON encontros(turma_id);
CREATE INDEX idx_encontros_dia_semana ON encontros(dia_semana);

CREATE TRIGGER trg_encontros_atualizado_em
BEFORE UPDATE ON encontros
FOR EACH ROW
EXECUTE FUNCTION set_atualizado_em();

CREATE TABLE alocacoes (
    id UUID PRIMARY KEY DEFAULT gen_random_uuid(),
    encontro_id UUID NOT NULL REFERENCES encontros(id) ON DELETE CASCADE,
    sala_id UUID NOT NULL REFERENCES salas(id) ON DELETE RESTRICT,
    status status_alocacao NOT NULL DEFAULT 'pendente',
    criado_por UUID REFERENCES usuarios(id) ON DELETE SET NULL,
    observacoes TEXT,
    criado_em TIMESTAMPTZ NOT NULL DEFAULT now(),
    atualizado_em TIMESTAMPTZ NOT NULL DEFAULT now(),
    CONSTRAINT uq_alocacao_encontro_sala UNIQUE (encontro_id, sala_id)
);

CREATE INDEX idx_alocacoes_encontro_id ON alocacoes(encontro_id);
CREATE INDEX idx_alocacoes_sala_id ON alocacoes(sala_id);
CREATE INDEX idx_alocacoes_status ON alocacoes(status);

CREATE TRIGGER trg_alocacoes_atualizado_em
BEFORE UPDATE ON alocacoes
FOR EACH ROW
EXECUTE FUNCTION set_atualizado_em();

CREATE TABLE historico_alteracoes (
    id UUID PRIMARY KEY DEFAULT gen_random_uuid(),
    tabela_afetada VARCHAR(100) NOT NULL,
    registro_id UUID NOT NULL,
    tipo_operacao tipo_operacao NOT NULL,
    dados_anteriores JSONB,
    dados_novos JSONB,
    usuario_id UUID REFERENCES usuarios(id) ON DELETE SET NULL,
    criado_em TIMESTAMPTZ NOT NULL DEFAULT now()
);

CREATE INDEX idx_historico_alteracoes_tabela_registro ON historico_alteracoes(tabela_afetada, registro_id);
CREATE INDEX idx_historico_alteracoes_usuario_id ON historico_alteracoes(usuario_id);
CREATE INDEX idx_historico_alteracoes_criado_em ON historico_alteracoes(criado_em);

CREATE TABLE revisoes_versao (
    id UUID PRIMARY KEY DEFAULT gen_random_uuid(),
    entidade VARCHAR(100) NOT NULL,
    entidade_id UUID NOT NULL,
    versao INTEGER NOT NULL,
    dados JSONB NOT NULL,
    usuario_id UUID REFERENCES usuarios(id) ON DELETE SET NULL,
    criado_em TIMESTAMPTZ NOT NULL DEFAULT now(),
    CONSTRAINT uq_revisao_entidade_versao UNIQUE (entidade, entidade_id, versao)
);

CREATE INDEX idx_revisoes_versao_entidade_id ON revisoes_versao(entidade, entidade_id);
CREATE INDEX idx_revisoes_versao_usuario_id ON revisoes_versao(usuario_id);