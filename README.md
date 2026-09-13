# Projeto Ensalamento

Sistema de **ensalamento** (alocação de turmas/aulas em salas físicas) com backend em **C++** e frontend web para gestão e visualização das alocações.

> **Status do projeto:** este repositório está em fase inicial de estruturação. As instruções de execução abaixo descrevem a arquitetura-alvo do projeto (backend em C++ com CMake, frontend em Node.js/npm, orquestração via Docker Compose). À medida que o código de cada camada for adicionado nos próximos passos, os comandos e arquivos referenciados aqui (`CMakeLists.txt`, `package.json`, `Dockerfile`, `docker-compose.yml`) serão criados de acordo com o descrito.

---

## Sumário

- [Visão geral](#visão-geral)
- [Estrutura de diretórios](#estrutura-de-diretórios)
- [Pré-requisitos](#pré-requisitos)
- [Execução local](#execução-local)
  - [Backend (C++)](#backend-c)
  - [Frontend](#frontend)
- [Execução com Docker](#execução-com-docker)
  - [Build e execução com Docker Compose](#build-e-execução-com-docker-compose)
  - [Build e execução manual de cada container](#build-e-execução-manual-de-cada-container)
- [Deploy](#deploy)
  - [Deploy em servidor próprio (VPS)](#deploy-em-servidor-próprio-vps)
  - [Deploy em serviços de nuvem](#deploy-em-serviços-de-nuvem)
  - [Variáveis de ambiente](#variáveis-de-ambiente)
- [Testes](#testes)
- [Contribuindo](#contribuindo)
- [Licença](#licença)

---

## Visão geral

O **Ensalamento** tem como objetivo automatizar e apoiar o processo de alocação de turmas, disciplinas e eventos em salas físicas de uma instituição de ensino, considerando restrições como:

- Capacidade da sala x quantidade de alunos da turma;
- Disponibilidade de horários (choque de horários);
- Requisitos especiais de sala (laboratório, auditório, acessibilidade, equipamentos);
- Preferências e restrições de professores;
- Otimização de deslocamento entre salas/blocos.

O sistema é dividido em duas grandes partes:

- **Backend (`backend/`)**: escrito em **C++**, responsável pela lógica de negócio, pelos algoritmos de alocação/otimização de salas e pela exposição de uma API para o frontend consumir.
- **Frontend (`frontend/`)**: aplicação web responsável pela interface de cadastro de salas, turmas, horários e pela visualização do ensalamento gerado.

Documentação complementar (regras de negócio, decisões de arquitetura, diagramas, manuais) fica centralizada em `docs/`.

---

## Estrutura de diretórios

```
ensalamento/
├── backend/            # Código-fonte do backend em C++
├── frontend/           # Código-fonte do frontend web
├── docs/               # Documentação do projeto (arquitetura, manuais, diagramas)
├── .gitignore          # Regras de exclusão de arquivos do controle de versão
├── LICENSE             # Licença MIT do projeto, com justificativa detalhada
└── README.md           # Este arquivo
```

À medida que o projeto evoluir, a estrutura interna de `backend/` e `frontend/` será detalhada em documentação própria dentro de `docs/`.

---

## Pré-requisitos

Para rodar o projeto **localmente, sem Docker**, você vai precisar de:

- **Backend (C++)**
  - Compilador C++ compatível com C++17 ou superior (GCC ≥ 9, Clang ≥ 10 ou MSVC ≥ 2019);
  - [CMake](https://cmake.org/) ≥ 3.16;
  - Ferramenta de build (Make, Ninja ou equivalente);
  - Gerenciador de dependências C++ (vcpkg ou Conan), caso o projeto passe a depender de bibliotecas externas (ex.: framework HTTP, driver de banco de dados).

- **Frontend**
  - [Node.js](https://nodejs.org/) ≥ 18 LTS;
  - npm ≥ 9 (ou Yarn/pnpm, conforme definido futuramente no `package.json`).

Para rodar via **Docker**, você vai precisar apenas de:

- [Docker](https://docs.docker.com/get-docker/) ≥ 24;
- [Docker Compose](https://docs.docker.com/compose/) (já incluso no Docker Desktop e nas versões recentes do Docker Engine, via `docker compose`).

---

## Execução local

### Backend (C++)

1. Acesse a pasta do backend:

```bash
   cd backend
```

2. Crie o diretório de build e gere os arquivos de build com o CMake:

```bash
   mkdir -p build
   cd build
   cmake -DCMAKE_BUILD_TYPE=Release ..
```

3. Compile o projeto:

```bash
   cmake --build . --config Release
```

4. Execute o binário gerado (o nome exato do executável será definido no `CMakeLists.txt` do backend):

```bash
   ./ensalamento_backend
```

5. Por padrão, a API do backend ficará disponível em:

```
   http://localhost:8080
```

   (a porta poderá ser configurada via variável de ambiente `BACKEND_PORT`, conforme seção [Variáveis de ambiente](#variáveis-de-ambiente)).

### Frontend

1. Acesse a pasta do frontend:

```bash
   cd frontend
```

2. Instale as dependências:

```bash
   npm install
```

3. Configure a URL da API do backend, criando um arquivo `.env` local a partir do `.env.example` (quando disponível):

```bash
   cp .env.example .env
```

4. Execute o servidor de desenvolvimento:

```bash
   npm run dev
```

5. Acesse a aplicação no navegador em:

```
   http://localhost:3000
```

6. Para gerar a build de produção do frontend:

```bash
   npm run build
```

---

## Execução com Docker

A forma recomendada de rodar o projeto completo (backend + frontend) de maneira reprodutível é via **Docker Compose**.

### Build e execução com Docker Compose

1. Na raiz do repositório, execute:

```bash
   docker compose up --build
```

2. Esse comando irá:
   - Construir a imagem do backend (C++), compilando o código dentro do container;
   - Construir a imagem do frontend, instalando dependências e gerando a build de produção (ou subindo o servidor de desenvolvimento, dependendo do ambiente configurado);
   - Subir ambos os serviços interligados na mesma rede Docker.

3. Após a subida dos containers, a aplicação estará disponível em:
   - Frontend: `http://localhost:3000`
   - Backend/API: `http://localhost:8080`

4. Para rodar em segundo plano (modo *detached*):

```bash
   docker compose up --build -d
```

5. Para visualizar os logs dos serviços:

```bash
   docker compose logs -f
```

6. Para parar e remover os containers:

```bash
   docker compose down
```

### Build e execução manual de cada container

Caso prefira construir e rodar cada serviço manualmente, sem Docker Compose:

**Backend:**

```bash
cd backend
docker build -t ensalamento-backend:latest .
docker run --rm -p 8080:8080 --name ensalamento-backend ensalamento-backend:latest
```

**Frontend:**

```bash
cd frontend
docker build -t ensalamento-frontend:latest .
docker run --rm -p 3000:3000 --name ensalamento-frontend ensalamento-frontend:latest
```

---

## Deploy

### Deploy em servidor próprio (VPS)

1. Instale Docker e Docker Compose no servidor (Ubuntu/Debian como exemplo):

```bash
   curl -fsSL https://get.docker.com | sh
   sudo usermod -aG docker $USER
```

2. Clone o repositório no servidor:

```bash
   git clone https://github.com/<sua-organizacao>/ensalamento.git
   cd ensalamento
```

3. Configure as variáveis de ambiente de produção (ver seção [Variáveis de ambiente](#variáveis-de-ambiente)), criando um arquivo `.env` na raiz do projeto.

4. Suba os containers em modo produção:

```bash
   docker compose -f docker-compose.yml -f docker-compose.prod.yml up --build -d
```

5. Configure um proxy reverso (Nginx, Caddy ou Traefik) na frente dos containers para expor as portas 80/443 com HTTPS (recomenda-se uso do [Let's Encrypt](https://letsencrypt.org/) via Certbot ou integração nativa do Traefik/Caddy).

6. Configure um serviço de *systemd* (opcional) ou dependa do `restart: always` do Docker Compose para garantir que os containers subam automaticamente após reinicializações do servidor.

### Deploy em serviços de nuvem

O projeto, por ser containerizado, é compatível com qualquer provedor que suporte containers Docker, incluindo (mas não se limitando a):

- **AWS**: ECS (Elastic Container Service), Fargate ou EC2 com Docker;
- **Google Cloud**: Cloud Run ou GKE (Google Kubernetes Engine);
- **Azure**: Azure Container Apps ou AKS (Azure Kubernetes Service);
- **Railway / Render / Fly.io**: plataformas PaaS com suporte nativo a Dockerfile/Docker Compose, indicadas para deploys mais simples e rápidos.

Passos gerais para esses provedores:

1. Publicar as imagens de backend e frontend em um *container registry* (Docker Hub, GitHub Container Registry, Amazon ECR, etc.):

```bash
   docker build -t <registry>/ensalamento-backend:latest ./backend
   docker push <registry>/ensalamento-backend:latest

   docker build -t <registry>/ensalamento-frontend:latest ./frontend
   docker push <registry>/ensalamento-frontend:latest
```

2. Configurar o serviço de nuvem escolhido para consumir essas imagens, definindo as variáveis de ambiente necessárias e mapeando as portas 8080 (backend) e 3000 (frontend, ou a porta final definida para servir os arquivos estáticos de produção).

3. Configurar domínio e certificado HTTPS conforme a plataforma escolhida.

### Variáveis de ambiente

As principais variáveis de ambiente utilizadas pelo projeto (a serem detalhadas em arquivos `.env.example` no `backend/` e no `frontend/` conforme o código for implementado):

| Variável            | Descrição                                             | Exemplo                        |
|---------------------|--------------------------------------------------------|---------------------------------|
| `BACKEND_PORT`       | Porta em que a API do backend será exposta             | `8080`                          |
| `FRONTEND_PORT`      | Porta em que o frontend será exposto                    | `3000`                          |
| `DATABASE_URL`       | String de conexão com o banco de dados (quando houver)  | `postgresql://user:pass@db:5432/ensalamento` |
| `NEXT_PUBLIC_API_URL`/`VITE_API_URL` | URL pública da API consumida pelo frontend | `http://localhost:8080`         |
| `NODE_ENV`           | Ambiente de execução do frontend                        | `development` / `production`    |

---

## Testes

As instruções detalhadas de testes (unitários e de integração) para backend e frontend serão adicionadas em `docs/` e neste README conforme os frameworks de teste forem definidos e incorporados ao projeto (ex.: GoogleTest/Catch2 para o backend em C++, Jest/Vitest para o frontend).

---

## Contribuindo

Contribuições são bem-vindas. Sugestão de fluxo:

1. Abra uma *issue* descrevendo o problema ou a melhoria proposta;
2. Crie um *branch* a partir de `main` seguindo o padrão `feature/nome-da-feature` ou `fix/nome-do-bug`;
3. Abra um *Pull Request* referenciando a issue correspondente, descrevendo as mudanças realizadas.

Diretrizes de estilo de código, convenções de commit e demais padrões do projeto serão documentados em `docs/CONTRIBUTING.md` conforme o projeto evoluir.

---

## Licença

Este projeto está licenciado sob os termos da **licença MIT**. Veja o arquivo [LICENSE](./LICENSE) para o texto completo da licença e para a justificativa detalhada da escolha dessa licença para o Projeto Ensalamento.