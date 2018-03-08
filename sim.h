#ifndef _SIM_H_
#define _SIM_H_


#include <stdint.h>

struct cache{
    uint32_t assoc; /* associatividade, 1 para map. direto */
    uint32_t block; /* número de bytes por bloco */
    uint32_t num_blocks; /* número de blocos por conjunto */
    uint32_t lat; /* latência de acesso em ciclos */
};


struct stats{
    unsigned long *hits; /* vetor para cada nível de cache */
    unsigned long *misses; /* idem */
    unsigned long cycles; /* número total de ciclos da simulação */
};

/** Recebe um traço de endereços de memória acessados e simula hierarquia de memória
  * @param configs vetor de configurações de caches
  * @param num_configs número de níveis de cache
  * @param mem_lat latência em ciclos de acesso à memória
  * @param filename nome do arquivo de traço (ou null)
  * @param stream stream com traço (se filename for null)
  * @return estatísticas de simulação
  */
struct stats * sim(struct cache * configs, int num_configs, 
            uint32_t mem_lat, char * filename, char * stream);


#endif // _SIM_H_
