# tar (.tar)

Tar é um formato de empacotamento de dados. Dependendo do caso, pode não ser um dos mais adequados de se utilizar, pois apesar de ser um formato muito fácil de ler, não tem compressão (apesar de ser possível, mas não vou abordar isso aqui), e soma isso com como funciona a própria estrutura do formato, de separar todo o arquivo em blocos de 512 bytes. Vamos por partes.

Como falei, o formato é lido em blocos de 512 bytes, então toda a estrutura funciona em cima disso, inclusive o cabeçalho tem esse mesmo tamanho. Caso um arquivo não utilize todos os 512 bytes do bloco, então é preenchido com zero, e caso ultrapasse os 512 bytes, um novo bloco vai ser alocado. Então rola um certo desperdício de espaço, por isso como falei, é bom avaliar se vai ser útil pro seu projeto.

A grande vantagem é que é extremamente simples de implementar, então você pode modificar sua estrutura para ser mais eficiente pro seu projeto, eliminando esse desperdício de espaço, por exemplo. Removendo dados do cabeçalho que não serão úteis, aplicando algum sistema de compressão, enfim, o céu é o limite.

No código eu implemento a estrutura padrão do Tar, fazendo direitinho nós conseguimos facilmente ver ele funcionando com programas de gerenciamento de pacotes, como o 7zip e o WinRar.

A estrutura é mais ou menos assim:
|                 |           |
|-----------------|-----------|
|   cabeçalho     | 512 bytes |
|   conteúdo      | 512 bytes |
|   cabeçalho     | 512 bytes |
|   conteúdo      | 512 bytes |
|   conteúdo      | 512 bytes |
|     ...         |    ...    |

E no final do arquivo ainda são adicionados mais dois blocos de 512 bytes vazios (ou seja, preenchidos com 0x0).

O cabeçalho segue o padrão posix, alguns dos valores guardados nele são uma strings que representam um valor em octal, então pra utilizar é necessário converter essas strings pra de fato um número octal e aí sim converter para decimal:

|     Campo     |  Tamanho  |  Tipo  | Descrição |
|---------------|-----------|--------|-----------|
| Nome          | 100 bytes | String | Nome do arquivo |
| Modo          | 8 bytes   | Octal  | Modo do arquivo (leitura, escrita e execução) |
| UID           | 8 bytes   | Octal  | ID do usuário ao qual o arquivo pertence |
| GID           | 8 bytes   | Octal  | ID do grupo ao qual o arquivo pertence |
| Tamanho       | 12 bytes  | Octal  | Tamanho do arquivo |
| Modificado    | 12 bytes  | Octal  | Ultima vez que o arquivo foi modificado |
| Checksum      | 8 bytes   | Octal  | Basicamente é a soma dos bytes do cabeçalho |
| Tipo          | 1 byte    | | Tipo de arquivo, se é comum, se é diretório, link simbólico ... |
| Nome do link  | 100 bytes | String | Caso seja um link simbólico, contem o caminho para o arquivo ao qual ele aponta |
| Magic         | 6 bytes   | String | Só para validação, geralmente é 'ustar ' |
| Versão        | 2 bytes   | Octal  | |
| UNAME         | 32 bytes  | String | Nome do usuário ao qual o arquivo pertence | 
| GNAME         | 32 bytes  | String | Nome do grupo ao qual o arquivo pertence | 
| Dev Minor     | 8 bytes   | Octal  | |
| Dev Major     | 8 bytes   | Octal  | |
| Prefixo       | 155 bytes | | |
| Sem uso       | 12 bytes | | |
