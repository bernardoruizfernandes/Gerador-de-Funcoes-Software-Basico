/* 

TRABALHO DE SOFTWARE BASICO

GERADOR DE FUNCOES

Bernardo Ruiz Fernandes 

*/


#include <stdio.h>
#include <stdlib.h>
#include "cria_func.h"


void libera_func (void* func){
	free(func);
	return;
}

void* cria_func (void* f, DescParam params[], int n){
	
	int k = 0, j, i;
	
	unsigned char inic_pilha[] = {0x55, 0x48, 0x89, 0xE5};		//comandos para inicar a pilha
	unsigned char fim_pilha[] = {0xC9, 0xC3};					//comandos para dar leave e ret
	
	unsigned char salvadores[] = {0xBF, 0xBE, 0xBA};			//edi, esi e edx
	unsigned char r_num[] = {0xB9, 0xBA, 0xBB};					//r9, r10 e r11
	unsigned char movs[] = {0x39, 0x32, 0x13};					//movs com os comandos selecionados acima

	unsigned long func = (unsigned long) f;						//guarda p endereco da funcao passada para nao se perder
	unsigned long end_var;										//variavel para quando precisar passar um endereço

	unsigned int val_int;										//variavel para quando precisar passar um inteiro

	unsigned char *codigo = (unsigned char *) malloc (sizeof(unsigned char)*512);



	j = 0;
	while(j<4){
		codigo[k++] = inic_pilha[j];							//inicando o codigo com os comandos de inicar uma pilha
		j++;
	}


	/*

	o codigo e separado em dois momentos, inicialmente sao tratados os casos de PARAM, pois sao mais simples
	em seguida os casos mais complicados, aonde temos os casos amarrados 

	Quando temos n == 1 (caso minimo) nao precisamos fazer nenhuma interacao pois
	ele ja esta nas posicoes necessarias para serem chamados
	
	*/
																			
	if (n == 2){												//caso tenhamos 2 parametros

		if (params[0].orig_val != PARAM && params[1].orig_val == PARAM){

			if(params[1].tipo_val == PTR_PAR) 
				codigo[k++] = 0x48;								//o 0x48 e necessario para movs de 8 bytes (em todos os casos)
			codigo[k++] = 0x89;									//movendo do 1 para o 2 parametro
			codigo[k++] = 0xfe;
		}
	}


// Com 3 parametros temos que fazer mais interacoes do tipo mov para varios tipos possiveis de eventos
																			
	if (n == 3){															

		if(params[2].orig_val == PARAM){

			if(params[2].tipo_val == PTR_PAR)					//todos os casos que vamos validar aqui teremos que ter esse mov
				codigo[k++] = 0x48;
			codigo[k++] = 0x89;

			if(params[0].orig_val != PARAM && params[1].orig_val != PARAM)
				codigo[k++] = 0xfa;								//%edi -> %edx

			else if(params[0].orig_val != PARAM && params[1].orig_val == PARAM){
				codigo[k++] = 0xf2;								//%esi -> %edx

				if(params[1].tipo_val == PTR_PAR) 
					codigo[k++] = 0x48;
				codigo[k++] = 0x89;
				codigo[k++] = 0xfe;								//%edi -> %esi
			}

			else if(params[0].orig_val == PARAM && params[1].orig_val != PARAM)
				codigo[k++] = 0xf2;								//%esi -> %edx

		}
		
		//unico caso que tem mov quando nao tem que mover o 3 parametro
		else if(params[0].orig_val != PARAM && params[1].orig_val == PARAM){
			if(params[1].tipo_val == PTR_PAR) 
				codigo[k++] = 0x48;
			codigo[k++] = 0x89;
			codigo[k++] = 0xfe;									//%edi -> %esi
		}
	}


	
	/*

	Realizando a parte de chamada de enderecos dos parametros passados
	Vale lembra, que quando as variveis sao parametros nao fazemos interacdoes agora
	pois ja foram feitas nas linhas anteriores

	*/
	i = 0;
	while(i<n){

		if(params[i].orig_val == FIX){

			if(params[i].tipo_val == INT_PAR){

				val_int = (unsigned int) params[i].valor.v_int;

				codigo[k++] = salvadores[i];					// chamo o (edi,esi,edx) com base na interacao que estou ja que estamos tratando de um int

				j = 0;
				while(j<32){									// while para fazer o shift de 8 em 8 no endereco de int
					codigo[k++] = (val_int >> j) & 0xFF;		// pegando de 8 em 8 com 0xFF para certificar					
					j+=8;
				}
			}

			else if (params[i].tipo_val == PTR_PAR){
	
				end_var = (unsigned long) params[i].valor.v_ptr;

				codigo[k++] = 0x48;								// temos o 0x48 por que agora vai ser (rdi, rsi, rdx) ja que estamos tratando de um long
				codigo[k++] = salvadores[i];

				j = 0;
				while(j<64){									//while para fazer o shift de 8 em 8 no endereco passado
					codigo[k++] = (end_var >> j) & 0xFF;		// passa o endereco
					j+=8;
				}
			}
		}

		else if (params[i].orig_val == IND){

			codigo[k++] = 0x49;
			codigo[k++] = r_num[i];								// pegamos (r9, r10, r11) por conta do 0x49, tratando long novamente

			end_var = (unsigned long) params[i].valor.v_ptr;

			j = 0;
			while(j<64){
				codigo[k++] = (end_var >> j) & 0xFF;			// passa o endereco				
				j+=8;
			}

			if (params[i].tipo_val == INT_PAR)
				codigo[k++] = 0x41;								// (%registrador da vez) -> %edi

			else if (params[i].tipo_val == PTR_PAR)
				codigo[k++] = 0x49;								// (%registrador da vez) -> %rdi

			codigo[k++] = 0x8B;
			codigo[k++] = movs[i];								// usando o registrador da vez
		}

	++i;
	}


	codigo[k++] = 0x48;											// movq $constrante, %rax 
	codigo[k++] = 0xB8;

	j = 0;
	while(j<64){
		codigo[k++] = (func >> j) & 0xFF;						//percorrendo o endereco da funcao e colocando no codigo
		j+=8;
	}

	codigo[k++] = 0xFF;											// call de %rax
	codigo[k++] = 0xD0;


	j = 0;
	while(j<2){
		codigo[k++] = fim_pilha[j];								//finalizando o codigo com os comandos de leave e ret
		j++;
	}

	return codigo;

}
