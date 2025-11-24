#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_PRODUTOS 100

struct Produto {
    char nome[50];
    float custo;
    float despesas;
    float lucro_desejado;
    float preco_ideal;
    float valor_lucro;
    float imposto;
};

// Função para calcular preço ideal e lucro (lucro sobre o preço final)
float calcularPreco(float custo, float despesas, float lucro_percent, float imposto_porcent, float *valor_lucro) {
    float total = custo + despesas;
    float preco = total / (1 - ((lucro_percent +imposto_porcent) /100.0));
    *valor_lucro = preco - total;
    return preco;
}

//ARQUIVO RANDOMICO/DIRETO
void carregarArquivos(struct Produto produtos[], int *qtd){

    FILE *arquivo = fopen("produtos.dat", "rb");
        if(arquivo == NULL){
            *qtd = 0;
            return;
        }
        *qtd = 0;
        while(fread(&produtos[*qtd], sizeof(struct Produto), 1, arquivo)==1){
            (*qtd)++;
        }
        fclose(arquivo);
}
void salvarEmArquivo(struct Produto produto[], int qtd){

     FILE *arquivo = fopen("produtos.dat", "wb");
           if(arquivo == NULL){
            printf("Erro ao abrir arquivo!\n");
            return;
           }
           for(int i=0; i<qtd; i++){
            fwrite(&produto[i], sizeof(struct Produto), 1, arquivo);
           }
           fclose(arquivo);

           printf("\nDados salvos com sucesso em 'produtos.dat'!\n");
}
// Função para cadastrar produto
void cadastrarProduto(struct Produto produtos[], int *qtd) {
    if (*qtd >= MAX_PRODUTOS) {
        printf("Limite de produtos atingido!\n");
        return;
    }

    struct Produto p;
    char entrada[50];

    printf("\nNome do produto: ");
    fgets(p.nome, sizeof(p.nome), stdin);
    p.nome[strcspn(p.nome, "\n")] = '\0';

    printf("Custo de producao (R$): ");
    fgets(entrada, sizeof(entrada), stdin);
    p.custo = atof(entrada);

    printf("Despesas adicionais (R$): ");
    fgets(entrada, sizeof(entrada), stdin);
    p.despesas = atof(entrada);

    printf("Imposto aplicado (%%): ");
    fgets(entrada, sizeof(entrada), stdin);
    p.imposto = atof(entrada);

    printf("Lucro desejado (%%): ");
    fgets(entrada, sizeof(entrada), stdin);
    p.lucro_desejado = atof(entrada);

    p.preco_ideal = calcularPreco(p.custo, p.despesas, p.lucro_desejado, p.imposto, &p.valor_lucro);

    produtos[*qtd] = p;

    //SALVA O ARQUIVO DIRETO NA POSICAO CORRETA
    FILE *arquivo = fopen("produtos.dat", "rb");
        if(arquivo == NULL){
            arquivo = fopen("produtos.dat", "wb");
        }
        fseek(arquivo, (*qtd) * sizeof(struct Produto), SEEK_SET);
        fwrite(&p, sizeof(struct Produto), 1, arquivo);
        fclose(arquivo);
        (*qtd)++;

    printf("\nPreco ideal de venda: R$ %.2f\n", p.preco_ideal);
    printf("Lucro estimado: R$ %.2f\n", p.valor_lucro);
    printf("--------------------------------------------\n");
}

// Função para listar produtos
void listarProdutos(struct Produto produtos[], int qtd) {

    if (qtd == 0) {
        printf("\nNenhum produto cadastrado ainda.\n");
        return;
    }

    printf("\n=== LISTA DE PRODUTOS ===\n");
    for (int i = 0; i < qtd; i++) {
        printf("%d. %s\n", i + 1, produtos[i].nome);
        printf("   Custo: R$ %.2f | Despesas: R$ %.2f\n", produtos[i].custo, produtos[i].despesas);
        printf("   Lucro desejado: %.1f%% | Imposto: %.1f%%\n", produtos[i].lucro_desejado, produtos[i].imposto);
        printf("   Lucro em R$: %.2f\n", produtos[i].valor_lucro);
        printf("   Preco ideal de venda: R$ %.2f\n", produtos[i].preco_ideal);
        printf("--------------------------------------------\n");
    }

}

// Função para editar produto
void editarProduto(struct Produto produtos[], int qtd) {
    if (qtd == 0) {
        printf("Nenhum produto para editar.\n");
        return;
    }

    listarProdutos(produtos, qtd);
    int indice;
    printf("\nDigite o numero do produto que deseja editar: ");
    scanf("%d", &indice);
    while (getchar() != '\n');

    if (indice < 1 || indice > qtd) {
        printf("Produto invalido.\n");
        return;
    }

    indice--;
    struct Produto *p = &produtos[indice];
    char entrada[50];

    printf("\nEditando produto: %s\n", p->nome);
    printf("Novo nome (ou Enter para manter): ");
    fgets(entrada, sizeof(entrada), stdin);
    if (entrada[0] != '\n') {
        entrada[strcspn(entrada, "\n")] = '\0';
        strcpy(p->nome, entrada);
    }

    printf("Novo custo (atual: %.2f): ", p->custo);
    fgets(entrada, sizeof(entrada), stdin);
    if (entrada[0] != '\n') p->custo = atof(entrada);

    printf("Novas despesas (atual: %.2f): ", p->despesas);
    fgets(entrada, sizeof(entrada), stdin);
    if (entrada[0] != '\n') p->despesas = atof(entrada);

    printf("Novo lucro desejado (%%) (atual: %.1f): ", p->lucro_desejado);
    fgets(entrada, sizeof(entrada), stdin);
    if (entrada[0] != '\n') p->lucro_desejado = atof(entrada);

    printf("Novo imposto (%%) (atual: %.1f): ", p->imposto);
    fgets(entrada, sizeof(entrada), stdin);
    if(entrada[0] != '\n') p->imposto = atof(entrada);

    p->preco_ideal = calcularPreco(p->custo, p->despesas, p->lucro_desejado, p->imposto, &p->valor_lucro);

    //salvar somente o novo registro no arquivo
    FILE *arquivo = fopen("produtos.dat", "rb");
    fseek(arquivo, indice * sizeof(struct Produto), SEEK_SET);
    fwrite(p, sizeof(struct Produto), 1, arquivo);
    fclose(arquivo);

    printf("\nProduto atualizado com sucesso!\n");
}

// Função para excluir produto
void excluirProduto(struct Produto produtos[], int *qtd) {
    if (*qtd == 0) {
        printf("Nenhum produto para excluir.\n");
        return;
    }

    listarProdutos(produtos, *qtd);
    int indice;
    printf("\nDigite o número do produto que deseja excluir: ");
    scanf("%d", &indice);
    while (getchar() != '\n');

    if (indice < 1 || indice > *qtd) {
        printf("Produto invalido.\n");
        return;
    }

    indice--;

    for (int i = indice; i < *qtd - 1; i++) {
        produtos[i] = produtos[i + 1];
    }
    (*qtd)--;
    //compactar arquivo reescrevendo tudo
    salvarEmArquivo(produtos, *qtd);

    printf("Produto excluido com sucesso!\n");
}

int main() {
    struct Produto produtos[MAX_PRODUTOS];
    int qtd = 0;
    int opcao;

    do {
        printf("\n=== SIPRI - Calculadora de Precificacao Inteligente ===\n");
        printf("1 - Cadastrar produto\n");
        printf("2 - Listar produtos\n");
        printf("3 - Editar produto\n");
        printf("4 - Excluir produto\n");
        printf("5 - Sair\n");
        printf("Escolha uma opcao: ");
        scanf("%d", &opcao);
        while (getchar() != '\n');

        switch (opcao) {
            case 1: cadastrarProduto(produtos, &qtd); break;
            case 2: listarProdutos(produtos, qtd); break;
            case 3: editarProduto(produtos, qtd); break;
            case 4: excluirProduto(produtos, &qtd); break;
            case 5: printf("Encerrando o programa...\n"); break;
            default: printf("Opcao invalida!\n");
        }
    } while (opcao != 5);

    return 0;
}
