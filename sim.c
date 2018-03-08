#include "sim.h"
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

typedef struct bloco bloco;
typedef struct nivelCache nivelCache;

void exec(nivelCache *tester, struct cache * configs, int num_configs, uint32_t endereco, struct stats *estatistica, uint32_t mem_lat);
uint32_t hexEmDecimal(char *trace, int inicio, int final);
void escrita(bloco **vetor, uint32_t assoc, uint32_t num_blocks, uint32_t endereco, uint32_t block);
int leitura(bloco **vetor, uint32_t assoc, uint32_t num_blocks, uint32_t endereco, uint32_t block);
uint32_t indiceEnd(uint32_t endereco, uint32_t modulo);
void impressao(uint32_t i);
void exibeCache(bloco **vetor, uint32_t assoc, uint32_t num_blocks, uint32_t block);
uint32_t indiceEnd(uint32_t endereco, uint32_t modulo);
uint32_t rotuloEnd(uint32_t endereco, uint32_t modulo);
bloco** alocaBloco(uint32_t assoc, uint32_t num_blocks, uint32_t block);
uint32_t modulo(uint32_t assoc, uint32_t num_blocks);

/**
 * representa cada nivel da cache
 * */
struct nivelCache{
	bloco **nivel;
};

/**
 * bloco da cache
 * */
struct bloco{
	uint32_t bitValidade;
	uint32_t rotulo; //rotulo do bloco
	uint32_t *informacao; 
	uint32_t uso; //flag pra saber se foi o último a ser usado
};

/**
 * funcao a ser implementada
 * */
struct stats * sim(struct cache * configs, int num_configs, uint32_t mem_lat, char * filename, char * stream){
	struct stats *estatistica = (struct stats *) malloc (sizeof(struct stats));
	estatistica->cycles=0;
	estatistica->hits = (unsigned long *) malloc(sizeof(unsigned long) * num_configs);
	estatistica->misses = (unsigned long *) malloc(sizeof(unsigned long) * num_configs);
	nivelCache * tester = (nivelCache *) malloc (sizeof(nivelCache) * num_configs);


	for(int i=0; i<num_configs; i++){
		if(configs[i].num_blocks >= configs[i].assoc){
			tester[i].nivel = alocaBloco(configs[i].assoc, configs[i].num_blocks, configs[i].block);
		}else{
			tester[i].nivel = alocaBloco(configs[i].num_blocks, configs[i].assoc, configs[i].block);
			int temp = configs[i].num_blocks;
			configs[i].num_blocks = configs[i].assoc;
			configs[i].assoc = temp;
		}
	}
	
	
	//Leitura de arquivo
	if(filename==NULL){ //Se nao tiver arquivo usar o stream
		int i=0;
		int inicio = 0;
		int final = 0;
		int flag=0;
		int tamStream = strlen(stream);
		uint32_t hexDec;
		
		while(i < tamStream){
			if (stream[i]==' '){
				inicio = i+1;
			}
			
			if (stream[i]=='\n'){
				final = i;
				flag=1;
			}
			
			if(flag==1){
				hexDec = hexEmDecimal(stream, inicio, final);
				exec(tester, configs, num_configs, hexDec, estatistica, mem_lat);
				
				/**
				//imprime as caches
				for(int j=0; j<num_configs; j++){
					exibeCache(tester[j].nivel, configs[j].assoc, configs[j].num_blocks, configs[j].block);
					printf("\n");
				}**/
				
				flag=0;
			}
			i++;
		}
	}else{ //ler arquivo com trace
		char * url = filename;
		char palavra[16];
		uint32_t hexDec;
		FILE *arq;
	
		arq = fopen(url, "r");
		if (arq == NULL){
			printf("Erro de leitura de arquivo");
			return 0;
		}
	
		while(!feof(arq)){
			fgetc(arq);
			fscanf(arq, " %s\n", palavra);
			hexDec = hexEmDecimal(palavra, 0, 8);
			exec(tester, configs, num_configs, hexDec, estatistica, mem_lat);
			
			/**
			//imprime as caches
			for(int j=0; j<num_configs; j++){
				exibeCache(tester[j].nivel, configs[j].assoc, configs[j].num_blocks, configs[j].block);
				printf("\n");
			}**/
			
		}
	}
	
	free(tester);
	
	return estatistica;
}

bloco** alocaBloco(uint32_t assoc, uint32_t num_blocks, uint32_t block){
	int temp = num_blocks / assoc;
	int assoc1 = assoc;
	int qtWordPorBloco = block/4; 
	
	bloco **vetor = (bloco **) malloc(sizeof(bloco *) * temp);
	for(int i=0; i<temp; i++){
		vetor[i] = (bloco *) malloc (sizeof(bloco)*assoc);
		for(int j=0; j<assoc1; j++){
			vetor[i][j].bitValidade = 0;
			vetor[i][j].rotulo = 0;
			vetor[i][j].informacao = (uint32_t *) malloc (sizeof(uint32_t) * qtWordPorBloco);
			for(int k=0; k<qtWordPorBloco; k++){
				vetor[i][j].informacao[k]=0;
			}
		}
	}
	return vetor;
}

/**
 * retorna quantos bits sao necessarios para representar (modulo do numero)
 * */
uint32_t modulo(uint32_t assoc, uint32_t num_blocks){
	uint32_t div = num_blocks / assoc;
	uint32_t mod = 0;
	
	while(div>1){
		div /= 2;
		mod++;
	}	
	
	return mod;
}

/**
 * converte endereço hexadecimal em endereço decimal
* */
uint32_t hexEmDecimal(char *trace, int inicio, int final){
	int valor=0;
	int expoente=7;
	uint32_t resultado=0;
	for(int l = inicio; l < final; l++){
		switch(trace[l]){
			case '0': valor=0; break;
			case '1': valor=1; break;
			case '2': valor=2; break;
			case '3': valor=3; break;
			case '4': valor=4; break;
			case '5': valor=5; break;
			case '6': valor=6; break;
			case '7': valor=7; break;
			case '8': valor=8; break;
			case '9': valor=9; break;
			case 'A': case 'a': valor=10; break;
			case 'B': case 'b': valor=11; break;
			case 'C': case 'c': valor=12; break;
			case 'D': case 'd': valor=13; break;
			case 'E': case 'e': valor=14; break;
			case 'F': case 'f': valor=15; break;
			default: printf("Erro: endereço inválido\n"); exit(1);
		}
		resultado += valor * pow(16, expoente);
		expoente--;
	}
	return resultado;
}

/**
 * Busca na cache
 * **/
int leitura(bloco **vetor, uint32_t assoc, uint32_t num_blocks, uint32_t endereco, uint32_t block){
	int mod = modulo(assoc, num_blocks);
	int qtWordPorBloco = block/4;
	int indice = indiceEnd(endereco/qtWordPorBloco, mod);
	uint32_t rotulo = rotuloEnd(endereco/qtWordPorBloco, mod);
	int flag=0;
	int assoc1 = assoc;
	
	for(int j=0; j<assoc1; j++){
		if(vetor[indice][j].bitValidade == 0){ //FALTA COMPULSÓRIA NAO ESTA NA CACHE
			//printf("FALTA COMPULSORIA - TRATAR FALTA\n");		
			return 0;
		}
		else if(vetor[indice][j].rotulo == rotulo){ //HIT
			//printf("HIT\n");
			if(assoc1!=1){
				for(int k=0; k<assoc1; k++){ //zera todos os bits de uso (usado pra saber quem foi o último usado).
					vetor[indice][k].uso = 0;
				}
				vetor[indice][j].uso = 1; //ativa o bit de uso (usado mais recentemente).
			}
			return 1;
		}else{
			flag=1;
		}
	}
	
	//colisao
	if(flag==1){
		//printf("FALTA - Colisao\n");
		return 0;
	}
	return 0;
}

/**
 * Escrita na cache
 * **/
void escrita(bloco **vetor, uint32_t assoc, uint32_t num_blocks, uint32_t endereco, uint32_t block){
	uint32_t mod = modulo(assoc, num_blocks);
	int qtWordPorBloco = block/4;
	uint32_t indice = indiceEnd(endereco/qtWordPorBloco, mod);
	uint32_t rotulo = rotuloEnd(endereco/qtWordPorBloco, mod);
	int assoc1 = assoc;
	
	int flag = 0;
	for(int j=0; j<assoc1; j++){
		if((vetor[indice][j].rotulo == rotulo)&&(vetor[indice][j].bitValidade!=0)){
			//printf("HIT já está na cache\n");
			flag=0;
			break;
		}
		
		else if((vetor[indice][j].uso == 0)&&(vetor[indice][j].bitValidade==0)){ //se espaco nao estiver sendo usado e bit de validade for 0
			flag=0;
			vetor[indice][j].bitValidade = 1; 
			vetor[indice][j].rotulo = rotulo;
			
			int endWord = (endereco/qtWordPorBloco)*qtWordPorBloco; //endereço inicial da palavra que será colocada no bloco
			for(int l=0; l<qtWordPorBloco; l++){
				vetor[indice][j].informacao[l] = endWord;
				endWord++;
			}
			
			if(assoc!=1){
				for(int k=0; k<assoc1; k++){ //zera todos os bits de uso (usado pra saber quem foi o ultimo usado).
					vetor[indice][k].uso = 0;
				}
				vetor[indice][j].uso = 1; //ativa o bit de uso (usado mais recentemente).
			}
			break;
		}
		flag=1;
	}
	
	if(flag==1){ //bit de validade é igual a 1 tem alguma coisa dentro
		for(int j=0; j<assoc1; j++){
			if((vetor[indice][j].uso == 0)){ //se uso for igual a zero, significa que nao foi o ultimo a ser usado entao escrever nessa posicao
				vetor[indice][j].bitValidade = 1; 
				vetor[indice][j].rotulo = rotulo;
				
				int endWord = (endereco/qtWordPorBloco)*qtWordPorBloco; //endereço inicial da palavra que será colocada no bloco
				for(int l=0; l<qtWordPorBloco; l++){
					vetor[indice][j].informacao[l] = endWord;
					endWord++;
				}
				
				if(assoc!=1){
					for(int k=0; k<assoc1; k++){ //zera todos os bits de uso (usado pra saber quem foi o último usado).
						vetor[indice][k].uso = 0;
					}
					vetor[indice][j].uso = 1; //ativa o bit de uso (usado mais recentemente).
				}
				break;
			}
		}
	}
}

/**
 * imprime numero decimal em binario
 * */
void impressao(uint32_t i){
	if(i / 2 != 0){
		impressao (i / 2);
	}
	printf("%d", i % 2);
} 

/**
 * imprime a cache
* */
void exibeCache(bloco **vetor, uint32_t assoc, uint32_t num_blocks, uint32_t block){
	int temp = num_blocks / assoc;
	int qtWordPorBloco = block/4;
	int assoc1 = assoc;
	
	for(int i=0; i<temp; i++){
		for(int j=0; j<assoc1; j++){
			printf("[%d %d]: \tUso: %d\tBit de Val: %d\tInformação", i, j, vetor[i][j].bitValidade, vetor[i][j].uso);
			for(int k=0; k<qtWordPorBloco; k++){
				printf("\t[%d]: %d", k, vetor[i][j].informacao[k]*4);
			}
			printf("\n");
		}
	}
}

/**
 * retorna indice do endereco
* */
uint32_t indiceEnd(uint32_t endereco, uint32_t modulo){
	int cont=1;
	uint32_t indice;
	int mod = modulo;
	
	for(int i=0; i < mod; i++){		
		cont = cont * 2;
	}
	cont=cont-1;
	indice = endereco & cont;
	
	return indice;
}

/**
 * retorna o rotulo do endereco
* */
uint32_t rotuloEnd(uint32_t endereco, uint32_t modulo){
	return endereco>>modulo;
}

/**
 * exibe endereço hexadecimal lido da stream
* */
void exibirEndHex(char *stream, int inicio, int final){
	char palavra[16] = ""; 
	uint32_t y=0;
	for(int l = inicio; l < final; l++){
		palavra[y] = stream[l];
		y++;
	}
	palavra[strlen(palavra)]='\0';
	printf("%s ", palavra);
}

void exec(nivelCache *tester, struct cache * configs, int num_configs, uint32_t endereco, struct stats *estatistica, uint32_t mem_lat){
	endereco /= 4;
	int i=0;
	uint32_t deuHit=0;
	
	//printf("-- Endereco %d --\n", endereco*4);
	
	while(i<num_configs){
		if(leitura(tester[i].nivel, configs[i].assoc, configs[i].num_blocks, endereco, configs[i].block)==1){
			deuHit=1;
			estatistica->hits[i] += 1;
			break;
		}
		i++;
	}
	
	//nao tem nenhum nível de cache
	if((deuHit==0)&&(num_configs<1)){
		//printf("\n-- Laço 1 --\nNao tem cache");
		estatistica->cycles += mem_lat;
		estatistica->misses[0] += 1;
	}
	
	//nao esta em nenhum nivel da cache
	else if((deuHit==0)&&(i!=0)){
		//printf("\n-- Laco 2 --\nNao esta em nenhum nivel de cache\nEscreve em todos os níveis da cache\n");
		int latenciaTemp=0;
		for(int j=0; j<i; j++){
			escrita(tester[j].nivel, configs[j].assoc, configs[j].num_blocks, endereco, configs[j].block);
			estatistica->misses[j] += 1;
			latenciaTemp += configs[j].lat;
		}
		//estatistica->cycles += configs->lat + (i * 10 - 10) + mem_lat;
		estatistica->cycles += latenciaTemp + mem_lat;
	}
	
	//deu hit no primeiro nivel da cache
	else if((deuHit==1)&&(i==0)){
		//printf("\n--Laco 3 --\nDeu hit no primeiro nivel de cache\n");
		estatistica->cycles += configs->lat;
	}
	
	//deu hit mas nao foi no primeiro nivel da cache
	else if((deuHit==1)&&(i!=0)){
		//printf("\n--Laco 4 --\nDeu hit mas nao foi no primeiro nivel da cache\nHit foi na cache[%d]\n\n", i+1);
		for(int j=0; j<i; j++){
			escrita(tester[j].nivel, configs[j].assoc, configs[j].num_blocks, endereco, configs[j].block);
			estatistica->cycles += mem_lat;
			estatistica->misses[j] += 1;
		}
	}
}

/**
// Função auxiliar para imprimir estatísticas no fim da simulação
void print_stats(struct stats * res, int levels){
	for(int i=0; i<levels; i++){
		printf("L%d\t misses %lu\t hits %lu\t hit rate %.2f%%\n", i+1, res->misses[i], 
			res->hits[i],100*res->hits[i]/(float)(res->hits[0]+res->misses[i]));
	}
	printf("Cycles: %lu\n",res->cycles);
}

int main(){
	struct cache cacheTemp[] = {{1024, 16, 1, 1}};
	int num_configs = 1;
	uint32_t mem_lat = 10; 
	char * filename = NULL; 
	char * stream = "R 000000000\nR 00000020\nR 000000000\nR 00000018\nR 00000020\n";
	
	struct stats * res  = sim(cacheTemp, num_configs, mem_lat, filename, stream);
	
	print_stats(res,1);
}**/
