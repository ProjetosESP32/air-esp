// by Marcus Vinícius
#define IR_RECEIVE_PIN 26 // D15
#define IR_SEND_PIN 4     // D4
// Os três próximos #define alteram alguns valores padrão da lib que não são funcionam com protocolos de ar condicionado
#define RAW_BUFFER_LENGTH 750   // bits do protocolo IR, um protocolo de 100 bits gera um array de 200 elementos ou seja 2x
#define RECORD_GAP_MICROS 52000 // Tempo do gap em micros da gravação
#define MARK_EXCESS_MICROS 20   // Sugerido pela lib para o módulo VS1838
#include <IRremote.hpp>
#include "LITTLEFS.h" // Sistema pra gravar na flash do esp

uint8_t Onn[750];
uint8_t Off[750];
uint8_t temp1[750];
uint8_t temp2[750];
uint8_t temp3[750];
uint8_t temp4[750];
uint8_t temp5[750];

bool isRec, isFirst = true;
int idRaw;
int sizeraws = 750;
int khz = 32; // frequência inicial vai até 38

void setup()
{
    Serial.begin(115200);               // Inicia comunicação Serial
    Serial.setTimeout(10);              // Alterando tempo de espera Serial
    LITTLEFS.begin();                   // Inicia comunicação
    IrSender.begin(IR_SEND_PIN, false); // Inicia o Transmissor IR
    // readData();
    readData("/rawON.txt", Onn); // Traz os dados da memória flash para a ram no seu respctivo Arquivo/array
    readData("/rawOFF.txt", Off);
    readData("/rawMacro1.txt", temp1);
    readData("/rawMacro2.txt", temp2);
    readData("/rawMacro3.txt", temp3);
    readData("/rawMacro4.txt", temp4);
    readData("/rawMacro5.txt", temp5);
    nextline(10); // Pula dez linhas
    Serial.println("COMANDOS SERIAL DO PROGRAMA");
    Serial.println("1  |     LIGA");
    Serial.println("2  |    DESLIGA");
    Serial.println("3  |     TEMP1");
    Serial.println("4  |     TEMP2");
    Serial.println("5  |     TEMP3");
    Serial.println("6  |     TEMP4");
    Serial.println("7  |     TEMP5");
    Serial.println("8  | Inicia Gravação");
    Serial.println("9  |   Lê Arquivos");
}
void loop()
{
    int command = Serial.parseInt(); // Passa leitura Serial que é em char para int
    switch (command)
    {
    case 1:
        SendRaw(Onn);
        break; // Manda sinal IR de acordo com o comando desejado
    case 2:
        SendRaw(Off);
        break;
    case 3:
        SendRaw(temp1);
        break;
    case 4:
        SendRaw(temp2);
        break;
    case 5:
        SendRaw(temp3);
        break;
    case 6:
        SendRaw(temp4);
        break;
    case 7:
        SendRaw(temp5);
        break;
    case 9:
        listdir();
        break;
    }
    if (command == 8)
    {
        command = 0;
        if (isFirst)
        { // Controle para gravar somente uma vez para não deixar a bibloteca que utiliza interrupções funcionando todo tempo
            isFirst = false;
            Rec(); // Inicia procedimento de gravação -> Utilizado função nesse caso para facilitar a leitura do código
        }
        else
        {
            Serial.println("Pressione reset para grvar novamente");
        }
    }
    if (isRec)
    { // Ativado pela função Rec() após as instruções
        if (IrReceiver.decode())
        { // Caso lido um sinal IR, retorna true
            // IrReceiver.printIRResultShort(&Serial); // use para debug caso o comando do ar não esteja sendo bem executado
            switch (idRaw)
            { // Grava o sinal IR nos seus respectivos arrays
            case 0:
                registerRaw(Off); // Função que grava o raw
                Serial.print("Comando ligar:");
                break;
            case 1:
                registerRaw(Onn);
                IrReceiver.compensateAndStoreIRResultInArray(Onn);
                Serial.print("temp1:");
                break;
            case 2:
                registerRaw(temp1);
                IrReceiver.compensateAndStoreIRResultInArray(temp1);
                Serial.print("temp2:");
                break;
            case 3:
                registerRaw(temp2);
                Serial.print("temp3:");
                break;
            case 4:
                registerRaw(temp3);
                IrReceiver.compensateAndStoreIRResultInArray(temp3);
                Serial.print("temp4:");
                break;
            case 5:
                registerRaw(temp4);
                IrReceiver.compensateAndStoreIRResultInArray(temp4);
                Serial.print("temp5:");
                break;
            case 6:
                registerRaw(temp5);
                Serial.println();
                isRec = false;                // Bloquei a leitura pois todos os sinais pedidos já foram enviados
                IrReceiver.stop();            // Desliga recpção IR
                delay(10);                    // boa prática
                writeData("/rawON.txt", Onn); // Grava o raw presenta na ram para a flash no seu respectivo Arquivo
                writeData("/rawOFF.txt", Off);
                writeData("/rawMacro1.txt", temp1);
                writeData("/rawMacro2.txt", temp2);
                writeData("/rawMacro3.txt", temp3);
                writeData("/rawMacro4.txt", temp4);
                writeData("/rawMacro5.txt", temp5);
                delay(10); // boa prática
                debug();   // Inicia o test de todos
                break;
            }
            idRaw++;
            IrReceiver.resume(); // Após receber um sinal deve ser executado para a lib poder receber outro
        }
    }
}

void debug()
{ // Testa todos os sinais gravados
    Serial.println();
    Serial.println("Testando Comandos, aponte o esp para o ar condicionado.");
    Serial.print("Iniciando em: ");
    timerSerial(5, 1000); // função de temporização 5 contagens/tempo entre contagens
    Serial.println();
    Serial.println("Zerando sequência");
    SendRaw(Off);

    timerSerial(3, 1000);
    Serial.print("-> ");
    Serial.println("Ar On");
    SendRaw(Onn);

    timerSerial(3, 1000);
    Serial.print("-> ");
    Serial.println("Ar Off");
    SendRaw(Off);

    timerSerial(3, 1000);
    Serial.print("-> ");
    Serial.println("Temp1");
    SendRaw(temp1);

    timerSerial(3, 1000);
    Serial.print("-> ");
    Serial.println("Temp2");
    SendRaw(temp2);

    timerSerial(3, 1000);
    Serial.print("-> ");
    Serial.println("Temp3");
    SendRaw(temp3);

    timerSerial(3, 1000);
    Serial.print("-> ");
    Serial.println("Temp4");
    SendRaw(temp4);

    timerSerial(3, 1000);
    Serial.print("-> ");
    Serial.println("Temp5");
    SendRaw(temp5);

    Serial.println();
    Serial.println();
    Serial.println("Gravação finalizada, para gravar novamente reinicie o esp");
    Serial.println("by: Marcus Vinícius");
}

void SendRaw(uint8_t *raw)
{ // Função que envia sinal para o ar condicionado -> Utiliza ponteiros pois o array é muito grande
    for (int i = 0; i < 7; i++)
    {
        IrSender.sendRaw(raw, sizeraws, khz + i); // Array do sinal, tamanho do array, frequência do sinal
        delay(5);
    }
}

void writeData(String path, uint8_t *raw)
{                                                // Usando ponteiro pois o array é muito grande
    File file = LITTLEFS.open(path, FILE_WRITE); // Cria ou abre um arquivo com o nome definido no modo escrita
    for (int i = 0; i < sizeraws; i++)
    {                             // laço for que passa cada elemento do array para a flash
        file.println(*(raw + i)); // Escreve cada valor por linha e incrementa endereço do ponteiro
    }
    file.close(); // fecha o arquivo
}

void readData(String path, uint8_t *raw)
{                                               // Lê arquivos -> utilizando ponteiros para ser o mudar a variável e não sua cópia
    String buffer_read;                         // String intermediária pois os dados são gravados em texto, não em int, sendo necessário conversão
    File file = LITTLEFS.open(path, FILE_READ); // Abre aquivo com o nome definido no modo leitura
    while (file.available())
    {                                             // Enquando houver informação para ser lida retorna verdadeiro
        buffer_read = file.readStringUntil('\n'); // Lê o arquivo até \n pra saber que é o fim da linha
        *raw = buffer_read.toInt();               // passa o arquivo de String para int
        raw++;                                    // incrementa endereço do ponteiro
    }
    file.close(); // Fecha arquivo
}

bool listdir()
{                                   // Lista todos os arquivos que estão disponíveis
    File root = LITTLEFS.open("/"); // Abre o "diretório" onde estão os arquivos na LITTLEFS

    if (!root) // root retorna false caso haja erro
    {
        Serial.println(" - falha ao abrir o diretório");
        return false;
    }
    File file = root.openNextFile(); // Abre o próximo arquivo
    int qtdFiles = 0;                // variável que armazena a quantidade de arquivos que
    //                    há no diretório informado.
    while (file)
    { // Enquanto houver arquivos no "diretório" que não foram vistos,
        //                executa o laço de repetição.
        Serial.print("FILE : ");
        Serial.print(file.name()); // Imprime o nome do arquivo
        Serial.print("\tSIZE : ");
        Serial.println(file.size()); // Imprime o tamanho do arquivo
        qtdFiles++;                  // Incrementa a variável de quantidade de arquivos
        file = root.openNextFile();  // Relata o próximo arquivo do diretório e
                                     //                              passa o retorno para a variável
                                     //                              do tipo File.
    }
    if (qtdFiles == 0) // Se após a visualização de todos os arquivos do diretório
                       //                      não houver algum arquivo, ...
    {
        // Avisa o usuário que não houve nenhum arquivo para ler e retorna false.
        Serial.println("Nenhuma Gravação Disponível");
        return false;
    }
    return true; // retorna true se não houver nenhum erro
}

void nextline(uint8_t number_line)
{ // pula linhas determinadas
    for (int nline = 0; nline <= number_line; nline++)
    {
        Serial.println();
    }
}

void timerSerial(uint8_t time_sec, int interval)
{ // Imprime um temporizador no serial
    for (int temp = time_sec; temp > 0; temp--)
    {
        Serial.print(temp);
        Serial.print(" ");
        delay(interval); // tempo dfinido pelo usuário
    }
}

void registerRaw(uint8_t *raw)
{
    IrReceiver.compensateAndStoreIRResultInArray(raw); // Salva raw no seu respectivo array
    Serial.print(" Salvo");
    Serial.print(" // ");
}
void Rec()
{ // Procedimento antes da gravação
    nextline(20);
    Serial.println("                CUIDADOS DURANTE A GRAVAÇÃO");
    Serial.println("        !!!NÃO EMITA NENHUM SINAL IR ALÉM DO PEDIDO!!");
    Serial.println("     !!!NÃO MOVIMENTE O CONTROLE AO ENVIAR O COMANDO!!!");
    Serial.println("TODOS OS CUIDADOS DEVEM SER TOMADOS, CASO CONTRÁRIO A GRAVAÇÃO FALHARÁ");
    Serial.println();
    Serial.println();
    Serial.println("                 PASSOS ANTES DA GRAVAÇÃO");
    Serial.println("1 - Ligue o ar condicionado");
    Serial.println("2 - Certifique-se que está em modo COOL/FRIO");
    Serial.println("3 - Defina direção desejada (estará presente em todos os comandos gravados)");
    Serial.println("4 - Coloque na temperatura mínima");
    Serial.println("A partir de agora todos os comandos devem ser direcionados para sensor IR");
    Serial.println("- Envie '1' para iniciar a gravação");
    int command;
    while (command != 1)
    { // Aguarda o usuário mandar 1 para continuar
        command = Serial.parseInt();
    }
    Serial.println();
    Serial.println();
    Serial.println("!!!NÃO ALTERE MAIS NENHUMA CONFIGURAÇÃO DO CONTROLE DO AR!!!");
    Serial.print("Gravação iniciando em: ");
    timerSerial(5, 1000); // Temporizador
    Serial.println();
    Serial.print("Comando Desligar:");
    IrReceiver.begin(IR_RECEIVE_PIN, 0); // Inicia o recptor
    isRec = true;                        // Valida variável que inicia a gravação
}
