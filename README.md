<h1>Gerador Dinâmico de Funções</h1>

Trabalho de Software Básico (INF1018)

<br>
<p align="center">
 <a href="#objetivo">Objetivo </a> |
 <a href="#dados">Dados </a> 
</p>


### Objetivo

O objetivo deste trabalho é implementar em C uma função cria_func, que recebe o endereço de uma função f e a descrição de um conjunto de parâmetros. A função cria_func deverá gerar, dinamicamente, o código de uma "nova versão" de f, e retornar o endereço do código gerado.

Também deve ser implementada uma função libera_func, que é responsável por liberar a memória alocada para o código criado dinamicamente.


### Dados 

   * Linguagem utilizada: ```C ```
   * Definição da funcao: ```void* cria_func (void* f, DescParam params[], int n);```
   * Enunciado em: ```enunciado.pdf```
   * Observações: considerar que o número mínimo de parâmetros é 1, e o máximo é 3


