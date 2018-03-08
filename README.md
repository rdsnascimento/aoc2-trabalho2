# Trabalho 2 - Arquitetura e Organização de Computadores II - 2017/2

## Descrição

Neste segundo trabalho, um simulador de hierarquia de memória orientado a traços será implementado. 

Um simulador orientado a traços (ou _traces_) recebe como entrada uma configuração de hierarquia de memória e uma lista de endereços acessados (ou traço). A resposta consiste de uma série de estatísticas sobre os acessos realizados, como taxa de acertos e falhas em cada nível, número de ciclos necessários para todos os acessos, tipos de falhas, etc.

## Formato do traço

O arquivo de traço terá entradas no seguinte formato:

        R 00000000

O primeiro campo indica se há uma leitura (caracter 'R') ou uma escrita ('W'). Os segundo campo contém o endereço em formato hexadecimal. Por exemplo,

        R 00000001
        W 000000FF

Correspondem a, respectivamente, leitura do endereço 1 e escrita no endereço 255. 

O separador é sempre um espaço em branco, o fim de linha é marcado por '\n'.

## Entrada

A entrada é composta por um vetor da estrutura com os campos abaixo, um elemento por nível de _cache_:

        struct cache{
            uint32_t assoc; /* associatividade, 1 para map. direto */
            uint32_t block; /* número de bytes por bloco */
            uint32_t num_blocks; /* número de blocos por conjunto */
            uint32_t lat; /* latência de acesso em ciclos */
        };

A assinatura da função a ser implementada é:

        struct stats * sim(struct cache * configs, int num_configs, 
            uint32_t mem_lat, char * filename, char * stream);

Onde _mem\_lat_ é a latência de acesso à memória e _filename_ é o nome do arquivo de traço. Caso _filename_ seja _null_, _stream_ contém o traço no mesmo formato do arquivo.

## Saída

O resultado da simulação deve ser retornado como uma estrutura com os seguintes campos:

        struct stats{
            unsigned long *hits; /* vetor para cada nível de cache */
            unsigned long *misses; /* idem */
            unsigned long cycles; /* número total de ciclos da simulação */
        };

Para cada nível de _cache_, um elemento nos vetores _hits_ e _misses_ deve ser alocado. Por exemplo, se houver dois níveis, deve ser possível acessar _hits[0]_ e _hits[1]_.

Em caso de erro (por exemplo, uma configuração impossível), a saída será _null_.

## Detalhes importantes

- Todas as _caches_ implementam a política de substituição LRU (_Least Recently Used_).
- A política de escrita é sempre _write-through_.
- A política de alocação em escrita é _allocate-on-write_.
- Todos os níveis são unificados (i.e. não existe _cache_ separada de instruções e dados).
- Todos os acessos são de 32 bits e são também alinhados (i.e. endereço é sempre um múltiplo de 4 bytes e nunca está em dois blocos da cache).
- Cuidado com a função de indexação.

## Produtos

* Implementação (*sim.c*)
* Casos de teses do(a) aluno(a) (*mytests.c* e traço em *mytrace.txt*)


