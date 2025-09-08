# Sistema de Controle de Acesso com RFID Integrado ao Firebase

Este repositório contém o código LaTeX do Trabalho de Conclusão de Curso (TCC) de Engenharia de Computação da UFMT.

## Autor

- **Nícolas Gabriel Meneses de Jesus**
- **Orientador:** Prof. Dr. Fabrício Carvalho

## Sobre o Projeto

Este trabalho apresenta o desenvolvimento de um sistema de controle de acesso com RFID integrado ao Firebase, demonstrando uma solução viável para modernização de sistemas legados sem comprometer sua funcionalidade original. A solução utiliza interceptação paralela dos sinais RFID para adicionar conectividade e capacidades de monitoramento remoto.

## Pré-requisitos para Compilação

### Sistema Linux (Ubuntu/Debian)

```bash
sudo apt update
sudo apt install texlive-full
sudo apt install texlive-lang-portuguese
sudo apt install texlive-fonts-recommended
sudo apt install texlive-latex-extra
```

### Sistema Windows

- Instale o [MiKTeX](https://miktex.org/download) ou [TeX Live](https://www.tug.org/texlive/)
- Certifique-se de que os pacotes em português estão instalados

### Sistema macOS

```bash
# Instale o MacTeX
brew install --cask mactex
```

## Estrutura do Projeto

```
TCCNICOLAS/
├── main.tex                    # Arquivo principal
├── bibliografia/
│   ├── bibliografia.bib       # Referências bibliográficas
│   └── portugues.bib          # Referências em português
├── lib/                       # Bibliotecas e estilos LaTeX
├── pre-textuais/             # Elementos pré-textuais
│   ├── figuras/              # Imagens e diagramas
│   ├── rosto.tex             # Folha de rosto
│   ├── preambulo.tex         # Agradecimentos
│   └── resumos.tex           # Resumo e abstract
├── textuais/                 # Capítulos do trabalho
│   ├── 01-introducao/
│   ├── 02-teoria/
│   ├── 03-trabalhos_relacionados/
│   ├── 04-problema/
│   ├── 05-implementacao/
│   ├── 06-experimentos_e_resultados/
│   ├── 07-conclusao/
│   └── apendice/
└── README.md                 # Este arquivo
```

## Como Compilar

### Compilação Completa (Recomendada)

Para gerar o PDF final com todas as referências e numeração correta:

```bash
cd /caminho/para/TCCNICOLAS
pdflatex main.tex
bibtex main
pdflatex main.tex
pdflatex main.tex
```

### Compilação Rápida (Para testes)

Para compilação rápida durante edição:

```bash
pdflatex main.tex
```

### Usando Makefile (se disponível)

```bash
make
```

## Arquivos Gerados

Após a compilação, os seguintes arquivos serão gerados:

- `main.pdf` - Documento final
- `main.aux` - Arquivo auxiliar
- `main.bbl` - Bibliografia processada
- `main.lof` - Lista de figuras
- `main.lot` - Lista de tabelas
- `main.toc` - Sumário
- `main.log` - Log de compilação

## Figuras

As figuras estão localizadas em `pre-textuais/figuras/` e incluem:

- Diagramas de arquitetura do sistema
- Fotos dos componentes utilizados
- Esquemas de conexão
- Resultados experimentais

## Dependências LaTeX

O projeto utiliza os seguintes pacotes principais:

- `babel` (português)
- `inputenc` (UTF-8)
- `graphicx` (figuras)
- `amsmath` (matemática)
- `hyperref` (links)
- `cite` (citações)

## Troubleshooting

### Erro: "File not found"

- Verifique se todas as figuras estão no diretório correto
- Confirme que os caminhos nos arquivos `.tex` estão corretos

### Erro: "Bibliography not found"

- Execute `bibtex main` após a primeira compilação
- Compile novamente com `pdflatex main.tex`

### Caracteres especiais não aparecem

- Certifique-se de que o arquivo está salvo em UTF-8
- Verifique se o pacote `inputenc` está carregado

### Referências não aparecem

- Execute a sequência completa: pdflatex → bibtex → pdflatex → pdflatex

## Contato

Para dúvidas sobre o projeto:

- **Autor:** Nícolas Gabriel Meneses de Jesus
- **Orientador:** Prof. Dr. Fabrício Carvalho
- **Instituição:** UFMT - Universidade Federal de Mato Grosso

## Licença

Este trabalho está licenciado sob os termos acadêmicos da UFMT para Trabalhos de Conclusão de Curso.
