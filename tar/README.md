# tar (.tar)

Tar é um formato de empacotamento, dependendo do caso, pode não ser um dos mais adequados de se utilizar, como não tem nenhum formato de compressão e também devido a como funciona sua estrutura, ele vai acabar sendo maior que um .zip por exemplo, em compensação é um formato extremamente fácil de se ler.

O formato é lido em blocos de 512 bytes, então toda a estrutura funciona em cima disso, inclusive o cabeçalho tem esse mesmo tamanho. Caso um arquivo não utilize todos os 512 bytes do bloco, então é preenchido com zero, e caso ultrapasse os 512 bytes, um novo bloco vai ser alocado.

A estrutura é mais ou menos assim:
|                 |           |
|-----------------|-----------|
|   cabeçalho     | 512 bytes |
|   conteúdo      | 512 bytes |
|   cabeçalho     | 512 bytes |
|   conteúdo      | 512 bytes |
|   conteúdo      | 512 bytes |
|     ...         |    ...    |



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