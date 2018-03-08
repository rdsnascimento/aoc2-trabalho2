#include "sim.h"
#include "simplegrade.h"


/* Função auxiliar para imprimir estatísticas no fim da simulação */
void print_stats(struct stats * res, int levels){
	for(int i=0; i<levels; i++){
		printf("L%d\t misses %lu\t hits %lu\t hit rate %.2f%%\n", i+1, res->misses[i], 
			res->hits[i],100*res->hits[i]/(float)(res->hits[0]+res->misses[i]));
	}
	printf("Cycles: %lu\n",res->cycles);
}

/* Macro para não dar segfault quando resultado é NULL */
#ifndef RES
#define RES(a, b, c); if(a){isEqual(a->cycles, b, c); free(a);}\
				  	else {isNotNull(a,c);}
#endif

void teste_hash(){
	struct stats * res ;
	char * trace = "R 00000000\nR 10000000\nR 00000000\n";

	DESCRIBE("Testa apenas a função hash");
	IF("Tenho tres ops com lat 10");
	THEN("Espero que o resultado seja 30 sem cache");

	res = sim(NULL, 0, 10, NULL, trace);
	RES(res,30,1);


	IF("Tenho quatro ops");
	THEN("Espero que o resultado seja 24 se não houver colisão e uma cache");

	struct cache mycache = {1, 4, 2, 1};
	char * trace2 = "R 00000000\nR 00000000\nR 00000004\nR 00000000\n";
	res = sim(&mycache, 1, 10, NULL, trace2);
	RES(res,24,4);


	IF("Tenho quatro ops");
	THEN("Espero que o resultado seja 44 se todas colidirem.");

	struct cache mycache2 = {1, 8, 2, 1};
	char * trace3 = "R 00000000\nR 01000000\nR 00000000\nR 01000000\n";
	res = sim(&mycache2, 1, 10, NULL, trace3);
	RES(res,44,1);

	IF("Tenho quatro ops");
	THEN("Espero que o resultado seja 11+1+11 ");

	char * trace4 = "R 00000000\nW 00000004\nR 00000008\n";
	res = sim(&mycache2, 1, 10, NULL, trace4);
	print_stats(res,1);
	RES(res,23,1);
}



void teste_so_mem(){
	DESCRIBE("Teste com memória");
	IF("Tenho um traço com 3K ops");
	THEN("Espero que o resultado seja 3K * 10 = 30K");

	struct stats * res = sim(NULL, 0, 10, "trace_small.txt", NULL);
	RES(res,30000,1);

}


void teste_l1_simples(){
	DESCRIBE("Teste com l1 e duas operações");
	char * trace = "R 12345678\nW 90ABCDEF\n";
	IF("Tenho duas ops com lat 11");
	THEN("Espero que o resultado seja 22");
	struct cache mycache = {1, 4, 2, 1};

	struct stats * res = sim(&mycache, 1, 10, NULL, trace);
	print_stats(res,1);
	RES(res,22,1);


	trace = "R 12345678\nR 12345678\n";
	IF("Tenho duas ops com lat 11 e 1");
	THEN("Espero que o resultado seja 12");

	res = sim(&mycache, 1, 10, NULL, trace);
	print_stats(res,1);

	RES(res,12,1);
}

void teste_l2_simples(){
	struct stats * res; 

	DESCRIBE("Teste com l2");
	IF("Tenho 2 ops com miss compulsório");
	THEN("Espero que o resultado seja 222");
	/* Fancy initializing structs! */
	struct cache mycache[] = {{1, 4, 1, 1}, {1, 8, 16, 10}}; 
	char * trace = "R 12345678\nW 00000000\n";
	res = sim(mycache, 2, 100, NULL, trace);
	print_stats(res,1);
	RES(res,222,1);


	trace = "R 12345678\nR 12345678\n";
	IF("Tenho duas ops, uma com hit");
	THEN("Espero que o resultado seja 112");

	res = sim(mycache, 2, 100, NULL, trace);
	print_stats(res,1);
	RES(res,112,1);
}

void teste_l3_simples(){
	struct stats * res; 

	DESCRIBE("Teste com l3");
	IF("Tenho 2 ops com miss compulsório");
	THEN("Espero que o resultado seja 262");
	/* Fancy initializing structs! */
	struct cache mycache[] = {{1, 4, 1, 1}, {1, 8, 16, 10}, {1, 8, 32, 20}}; 
	char * trace = "R 12345678\nW 00000000\n";
	res = sim(mycache, 3, 100, NULL, trace);
	print_stats(res,1);
	RES(res,262,4);


	trace = "R 12345678\nR 12345678\n";
	IF("Tenho duas ops, uma com hit");
	THEN("Espero que o resultado seja 112");

	res = sim(mycache, 3, 100, NULL, trace);
	print_stats(res,1);
	RES(res,132,4);


}



void teste_short_trace_l1(){
	struct stats * res;

	DESCRIBE("Teste com traço 6K");
	IF("Tenho 6K ops e uma cache l1 1:4:1:1 (4 B)");
	THEN("Espero que o resultado seja 6K*1+6K*10 - 40 (só quatro hits)");
	
	struct cache mycache = {1, 4, 1, 1};
	res = sim(&mycache, 1, 10, "trace_small_2.txt", NULL);
	print_stats(res, 1);
	RES(res,65960,5);

	IF("Tenho 6K ops e uma cache l1 1:16:16:1 (256 B)");
	THEN("Espero que haja mais hits por localidade espacial (mesma linha)");
	
	struct cache mycache2 = {1, 16, 16, 1};
	res = sim(&mycache2, 1, 10, "trace_small_2.txt", NULL);
	print_stats(res, 1);
	RES(res,32600,5);

	IF("Tenho 6K ops e uma cache l1 1:16:1024:1 (16KiB)");
	THEN("Espero que diminua os misses de capacidade (e conflito)");
	
	struct cache mycache3 = {1, 16, 1024, 1};
	res = sim(&mycache3, 1, 10, "trace_small_2.txt", NULL);
	print_stats(res, 1);
	RES(res,6810,5);


	IF("Tenho 6K ops e uma cache l1 1024:16:1:1 (16KiB, CAM)");
	THEN("Espero que diminua os misses de capacidade (e conflito)");
	
	struct cache mycache4 = {1024, 16, 1, 1};
	res = sim(&mycache4, 1, 10, "trace_small_2.txt", NULL);
	print_stats(res, 1);
	RES(res,6810,5);

}



int main(int argc __attribute__ ((unused)), char ** argv __attribute__ ((unused))){

	DESCRIBE("Teste do simulador de cache");

	teste_hash();
	teste_so_mem();
	teste_l1_simples();
	teste_l2_simples();
	teste_l3_simples();
	teste_short_trace_l1();

	
	GRADEME();

	if (grade==maxgrade)
		return 0;
	else return grade;

}
