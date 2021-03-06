#include "transmissor.hpp"
#include "../utils/utils.hpp"

#define TIPO_CONTROLE_ERRO 2
#define TAMANHO_PARIDADE 8
#define CHAVE "1101"

// Camadas transmissoras
vector<int> AplicacaoTransmissora();
vector<int> CamadaDeAplicacaoTransmissora(string mensagem);
vector<int> CamadaEnlaceDadosTransmissora(vector<int> &quadro);

// Controle de erros
void CamadaEnlaceDadosTransmissoraControleDeErroBitParidade(vector<int> &quadro, int _paridade);
void CamadaEnlaceDadosTransmissoraControleDeErroCRC(vector<int> &quadro);

// Utilidades
vector<int> stringToBinary(string words);

//CRC
string encodeData(string data, string key);

// Le a entrada do usuario
vector<int> AplicacaoTransmissora() {

    // Le a entrada do usuario
    string mensagem;
    cout << "Digite uma mensagem: " << endl;
    cin >> mensagem;

    // Chama a proxima camada
    return CamadaDeAplicacaoTransmissora(mensagem);

}

// Converte a entrada do usuario para binario
vector<int> CamadaDeAplicacaoTransmissora(string mensagem) {

    // Converte a mensagem do usuario para um vetor binario
    vector<int> quadro = stringToBinary(mensagem);

    // Chama a proxima camada
    return CamadaEnlaceDadosTransmissora(quadro);

}

// Adiciona bits para controle de erros
vector<int> CamadaEnlaceDadosTransmissora(vector<int> &quadro) {

    // Seleciona o tipo de controle de erro
    switch( TIPO_CONTROLE_ERRO ) {

        case 0:
        
            CamadaEnlaceDadosTransmissoraControleDeErroBitParidade(quadro, 0);
        
        break;

        case 1:
        
            CamadaEnlaceDadosTransmissoraControleDeErroBitParidade(quadro, 1);
        
        break;

        case 2:

            CamadaEnlaceDadosTransmissoraControleDeErroCRC(quadro);

        break;

    }

    return quadro;

}

// Adiciona o metodo de paridade para controle de erros
void CamadaEnlaceDadosTransmissoraControleDeErroBitParidade(vector<int> &quadro, int _paridade) {

    // Inicializa o vetor auxiliar que armazena os dados com bits de controle de erros
    vector<int> quadroFinal;

    // Inicializa as variaveis que armazenam as somas das linhas e colunas
    int horizontalSum = 0;
    vector<int> linhaFinal(TAMANHO_PARIDADE+1);

    // Le o comprimento do vetor de dados
    int length = quadro.size();

    // Realiza uma iteracao pelo vetor de dados
    for( int i=0; i<length; i++ ) {

        // Le o bit atual
        int bit = quadro[i];

        // Soma o bit atual e o insere no vetor auxiliar
        horizontalSum += bit;
        quadroFinal.push_back(bit);

        // Ao acabar um byte (8 bits), 
        if( (i+1) % TAMANHO_PARIDADE == 0 ) {

            // Verifica se a linha eh par ou impar
            int paridade = horizontalSum % 2 == _paridade;

            // Insere o bit de paridade no vetor auxiliar
            quadroFinal.push_back( paridade );
            horizontalSum = 0;

            // Atualiza o vetor de soma da linha final
            linhaFinal[TAMANHO_PARIDADE] += paridade;

        }

        // Atualiza o vetor de soma da linha final
        linhaFinal[ i%TAMANHO_PARIDADE ] += bit;

    }

    // Atualiza o vetor da linha final, defininindo os bits de paridade com base nas somas das colunas
    for( int i=0; i<(TAMANHO_PARIDADE+1); i++ ) {

        linhaFinal[ i ] = linhaFinal[ i ] % 2 == _paridade;

    }

    // Concatena o vetor auxiliar ao vetor da linha final
    quadroFinal.insert( quadroFinal.end(), linhaFinal.begin(), linhaFinal.end() );

    // Atualiza o vetor de dados
    quadro = quadroFinal;

    // Imprime os dados antes do envio ao ponto B
    cout << endl << "Dados antes do envio:" << endl;

    length = quadroFinal.size();
    for( int i=0; i<length; i++ ) {

        cout << quadroFinal[i] << ' ';

        if( (i+1) % (TAMANHO_PARIDADE+1) == 0 ) cout << endl;

    }

    cout << endl;

}

// Adiciona o metodo CRC para controle de erros
void CamadaEnlaceDadosTransmissoraControleDeErroCRC(vector<int> &quadro) {
    string str = vecToStr(quadro);
    str = encodeData(str, CHAVE);
    quadro = strToVec(str);
    return;
}

// Converte caracteres para binario
vector<int> stringToBinary(string words) {

    // Converte a string para uma string binaria
    string binaryString = "";
    
    for (char& _char : words) {

        binaryString += bitset<8>(_char).to_string();
    
    }

    // Converte a string para um vetor binario
    int stringLength = binaryString.length();
    vector<int> binaryData;

    int i = 0;
    for(i=0; i < stringLength; i++) {

        binaryData.push_back( binaryString[i] == '1' );

    }

    return binaryData;
}

string encodeData(string data, string key)
{
    int l_key = key.length();
     
    // Appends n-1 zeroes at end of data
    string appended_data = (data +
                            std::string(
                                l_key - 1, '0'));
     
    string remainder = mod2div(appended_data, key);
     
    // Append remainder in the original data
    string codeword = data + remainder;
    return codeword;
}