# bitmap (.bmp)

Bitmap é um formato usado pra armazenar pixels de uma imagem, o arquivo basicamente contem dois cabeçalhos, um de 14 bytes onde são guardadas informações mais relacionadas ao arquivo em si, como bytes de validação do formato, tamanho e a partir de onde começa os dados de pixel da imagem por exemplo, e outro cabeçalho de 40 bytes (que pode variar de tamanho, mas explico mais abaixo) que contem informações mais específicas em relação a imagem a resolução da imagem e a quantidade de bits utilizada para armazenar um pixel. Por ultimo, logo depois dos cabeçalhos vem os dados de pixels da imagem.

**Cabeçalho de Arquivo (14 bytes)**

| Campo | Bytes | Descrição |
| ----- | ----- | --------- |
| Tipo  |   2   | esse campo é utilizado somente pra validar a imagem como um bitmap, é sempre preenchido com 'BM' (ou 0x424D em hexadecimal) |
| Tamanho |  4  | Tamanho do arquivo em bytes |
| Reservado |  2  | |
| Reservado |  2  | |
| Offset    |  4  | A partir de onde começar a ler os dados de pixels da imagem |

**Cabeçalho de Dados do Bitmap(40 bytes)**
| Campo | Bytes | Descrição |
| ----- | ----- | --------- |
| Tamanho |  4  | Tamanho do cabeçalho, geralmente é 40 |
| Largura |  4  | Largura da imagem |
| Altura  |  4  | Altura da imagem |
| Planos  |  2  | Não sei pra que serve, mas geralmente é 1 |
| Bits por pixel |  4  | Quantidade de bits utilizada para armzenar um pixel, pode ser 1, 2, 4, 8, 16, 32 |
| Compressão |  4  | Tipo de compressão utilizada, geralmente é zero |
| Tamanho da imagem |  4  | Tamanho da imagem em bytes |
| X Resolution |  4  | |
| Y Resolution |  4  | |
| Cores utilizadas |  4  | Geralmente é zero, mas pode variar caso a imagem utilize um número de cores limitado por paleta |
| Cores importantes |  4  | Mesmo esquema |
| Paleta | 4 * N (pode variar) | Caso a imagem possua uma paleta de cores, os dados vão aqui, esse campo só é utilizado quando o campo de `Bits por pixel` é menor ou igual a 8 (<= 8) |

O `N` na paleta de cores são o número de cores que a paleta tem, como falei, ele pode variar dependendo do número de `Bits por pixel`:

- 1: N = 2
- 4: N = 16
- 8: N = 256
- 16: N = 0 (RGB565)
- 24: N = 0 (RGB)
