#include "sim.h"
#include "simplegrade.h"

void imprimeResultado(struct stats * resultado, int assoc){
	for(int i=0; i<assoc; i++){
		printf("Nivel: %d\tQuant de Miss: %lu\tQuant de Hit: %lu\t\tCiclos: %lu\n", i+1, resultado->misses[i], resultado->hits[i], resultado->cycles);
	}
}

void teste_l1(){
	char * trace = "R 00000010\nR 00000028\nR 00000018\nR 0000000C\n";
	struct stats * resultado;
	
	DESCRIBE("Mapeamento direto 4 bytes por bloco");
	IF("Tenho quatro ops com miss compulsório e latência 100");
	THEN("Espero que resultado seja 404");
	
	struct cache cacheTemp = {1, 4, 8, 1};
	resultado = sim(&cacheTemp, 1, 100, NULL, trace);
	//imprimeResultado(resultado, 1);
	isEqual(resultado->cycles, 404, 1);
	free(resultado);
	
	trace = "R 00000018\nR 0000000C\nR 00000018\nR 0000000C\nR 00000010\n";
	IF("Se tenho cinco ops, com 2 hits");
	THEN("Espero que resultado seja 35");
	resultado = sim(&cacheTemp, 1, 10, NULL, trace);
	//imprimeResultado(resultado, 1);
	isEqual(resultado->cycles, 35, 1);
	free(resultado);
	
	struct cache cacheTemp2 = {1, 8, 8, 1};
	DESCRIBE("Mapeamento direto com 8 bytes por bloco");
	trace = "R 0000001C\nR 00000020\nR 00000024\nR 0000000C\nR 00000008\nR 00000004\nR 00000024\n";
	IF("Se tenho 7 ops, com 4 miss e 3 hits");
	THEN("Espero que resultado seja 47");
	resultado = sim(&cacheTemp2, 1, 10, NULL, trace);
	//imprimeResultado(resultado, 1);
	isEqual(resultado->cycles, 47, 1);
	free(resultado);
	
	struct cache cacheTemp3 = {1, 16, 8, 1};
	DESCRIBE("Mapeamento direto com 16 bytes por bloco");
	trace = "R 00000010\nR 00000018\nR 00000014\nR 0000001C\n";
	IF("Se tenho 4 ops, com 1 miss e 3 hits");
	THEN("Espero que resultado seja 14");
	resultado = sim(&cacheTemp3, 1, 10, NULL, trace);
	//imprimeResultado(resultado, 1);
	isEqual(resultado->cycles, 14, 1);
	free(resultado);
	
	struct cache cacheTemp4 = {2, 4, 4, 1};
	DESCRIBE("Mapeamento 2 Associativo com 4 bytes por bloco");
	trace = "R 00000010\nR 00000020\nR 00000010\nR 00000018\nR 00000020\n";
	IF("Se tenho 5 ops, com 4 miss e 1 hits");
	THEN("Espero que resultado seja 45");
	resultado = sim(&cacheTemp4, 1, 10, NULL, trace);
	//imprimeResultado(resultado, 1);
	isEqual(resultado->cycles, 45, 1);
	free(resultado);
	
	struct cache cacheTemp5 = {2, 16, 4, 1};
	DESCRIBE("Mapeamento 2 Associativo com 16 bytes por bloco");
	IF("Se tenho 5 ops, com 2 miss e 3 hits");
	THEN("Espero que resultado seja 25");
	resultado = sim(&cacheTemp5, 1, 10, NULL, trace);
	//imprimeResultado(resultado, 1);
	isEqual(resultado->cycles, 25, 1);
	free(resultado);
	
	struct cache cacheTemp6 = {4, 16, 4, 1};
	DESCRIBE("Mapeamento 4 Associativo com 16 bytes por bloco");
	trace = "R 00000010\nR 00000020\nR 00000010\nR 00000018\nR 00000020\nR 0000012C\nR 0000012C\n";
	IF("Se tenho 7 ops, com 3 miss e 4 hits");
	THEN("Espero que resultado seja 37");
	resultado = sim(&cacheTemp6, 1, 10, NULL, trace);
	//imprimeResultado(resultado, 1);
	isEqual(resultado->cycles, 37, 1);
	free(resultado);
}

void teste_l2(){
	char * trace = "R 00000000\nR 00000020\nR 00000000\nR 00000018\nR 00000020\n";
	struct stats * resultado;
	
	IF("Tenho cinco ops com 8 miss e 2 hits");
	THEN("Espero que resultado seja 56");
	struct cache cacheTemp[] = {{1, 8, 4, 1}, {2,16,8, 1}};
	resultado = sim(cacheTemp, 2, 10, NULL, trace);
	//imprimeResultado(resultado, 2);
	isEqual(resultado->cycles, 56, 1);
	free(resultado);
	
	IF("Tenho cinco ops com 7 miss e 2 hits");
	THEN("Espero que resultado seja 56");
	trace = "R 00000190\nR 00000194\nR 00000198\nR 000001B8\nR 00000C94\n";
	struct cache cacheTemp1[] = {{2, 8, 8, 1}, {2,16,16, 1}};
	resultado = sim(cacheTemp1, 2, 10, NULL, trace);
	//imprimeResultado(resultado, 2);
	isEqual(resultado->cycles, 47, 1);
	free(resultado);
	
}

void teste_l3(){
	char * trace = "R 00001310\nR 000012CC\nR 000012C8\nR 00001450\n";
	struct stats * resultado;
	
	IF("Tenho quatro ops com 10 miss e 1 hits");
	THEN("Espero que resultado seja 49");
	struct cache cacheTemp[] = {{1, 4, 8, 1}, {2,8,16, 1}, {2,16,16,1}};
	resultado = sim(cacheTemp, 3, 10, NULL, trace);
	//imprimeResultado(resultado, 3);
	isEqual(resultado->cycles, 49, 1);
	free(resultado);
}

void teste_arquivo(){
	struct stats * resultado;
	
	DESCRIBE("Mapeamento direto com 16 bytes por bloco e apenas 8 blocos");
	IF("Tenho 500 ops com 323 miss e 177 hits");
	THEN("Espero que resultado seja 3730");
	struct cache cacheTemp = {1, 16, 8, 1};
	resultado = sim(&cacheTemp, 1, 10, "mytrace.txt", NULL);
	//imprimeResultado(resultado, 1);
	isEqual(resultado->cycles, 3730, 1);
	free(resultado);
	
	DESCRIBE("Com dois níveis de cache");
	IF("Tenho 500 ops");
	THEN("Espero que resultado seja 4060");
	struct cache cacheTemp2[] = {{1, 4, 8, 1}, {2, 16, 64, 1}};
	resultado = sim(cacheTemp2, 2, 10, "mytrace.txt", NULL);
	//imprimeResultado(resultado, 2);
	isEqual(resultado->cycles, 4060, 1);
	free(resultado);
}

void teste_semCache(){
	struct stats * resultado;
	char * trace = "R 00000010\nR 00000028\nR 00000018\nR 0000000C\n";

	DESCRIBE("Sem nenhum nível de cache");
	IF("Testa quatro operacoes com latencia 100");
	THEN("Espero que resultado seja 400");
	resultado = sim(NULL, 0, 100, NULL, trace);
	isEqual(resultado->cycles, 400, 1);
	free(resultado);

	trace = "R 00000010\nR 00000028\nR 00000018\n";
	IF("Testa três operacoes com latencia 10");
	THEN("Espero que resultado seja 30");
	resultado = sim(NULL, 0, 10, NULL, trace);
	isEqual(resultado->cycles, 30, 1);
	free(resultado);

	DESCRIBE("Testando arquivo sem nenhuma cache");
	IF("Tenho 500 ops com latência 10");
 	THEN("Espero que resultado seja 10*500");
	resultado = sim(NULL, 0, 10, "mytrace.txt", NULL);
	//imprimeResultado(resultado, 2);
	isEqual(resultado->cycles, 5000, 1);
	free(resultado);
}

int main(int argc __attribute__ ((unused)), char ** argv __attribute__ ((unused))){

	DESCRIBE("SIMULADOR DE CACHE\n");
	DESCRIBE("Testando com um nível da cache\n");
	teste_l1();
	
	DESCRIBE("\nTestando com dois níveis da cache\n");
	teste_l2();
	
	DESCRIBE("\nTestando com três níveis da cache\n");
	teste_l3();
	
	DESCRIBE("\nTestando com arquivo de entrada níveis da cache\n");
	teste_arquivo();

	DESCRIBE("\nTestando sem nenhum nível de cache\n");
	teste_semCache();

	GRADEME();
	if (grade==maxgrade)
		return 0;
	else return grade;

}
