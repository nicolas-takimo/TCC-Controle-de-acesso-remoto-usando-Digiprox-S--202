// (c) Michael Schoeffler 2018, http://www.mschoeffler.de
// Exemplo adaptado e comentado para leitura de quadros do leitor RFID RDM630/RDM6300
// Formato do quadro (14 bytes total):
// [0]  0x02 (HEAD / início)
// [1..10] 10 bytes ASCII HEX: 2 bytes "version" + 8 bytes "tag"
// [11..12] 2 bytes ASCII HEX: checksum (XOR de cada par de HEX nos 10 bytes de data)
// [13] 0x03 (TAIL / fim)

#include <SoftwareSerial.h>  // Biblioteca para porta serial por software (RX, TX em pinos digitais)
#include <stdlib.h>          // Para malloc/free e strtol

// Tamanhos fixos do protocolo do RDM630/RDM6300
const int BUFFER_SIZE = 14;       // 1 head, 10 data, 2 checksum, 1 tail = 14
const int DATA_SIZE = 10;         // bytes de dados (2 versão + 8 tag), ainda em ASCII HEX
const int DATA_VERSION_SIZE = 2;  // 2 bytes ASCII HEX correspondentes à "versão"
const int DATA_TAG_SIZE = 8;      // 8 bytes ASCII HEX do identificador (tag)
const int CHECKSUM_SIZE = 2;      // 2 bytes ASCII HEX do checksum

// Instancia uma serial por software:
//   RX no pino 6 (Arduino lê do leitor no pino 6)
//   TX no pino 8 (não é usado pelo leitor, mas precisa ser informado)
SoftwareSerial ssrfid = SoftwareSerial(6, 8);

// Buffer que armazena um quadro completo vindo do leitor
uint8_t buffer[BUFFER_SIZE];
// Índice atual de escrita no buffer (0..13)
int buffer_index = 0;

// Declarações antecipadas das funções usadas adiante
unsigned long extract_tag();                                      // Extrai e imprime campos do buffer; retorna a tag convertida
long hexstr_to_value(const uint8_t *str, unsigned int length);    // Converte uma sequência ASCII HEX (não terminada em '\0') para valor numérico

void setup() {
  Serial.begin(9600);     // Porta serial do Arduino para logs no Monitor Serial
  ssrfid.begin(9600);     // Porta serial por software para o leitor (RDM630 costuma usar 9600 8N1)
  ssrfid.listen();        // Garante que ssrfid é a porta escutada ativamente
  Serial.println("INIT DONE");  // Mensagem de inicialização
}

void loop() {
  // Só processa se há bytes disponíveis do leitor
  if (ssrfid.available() > 0) {
    bool call_extract_tag = false;  // Flag para indicar que o quadro terminou e devemos processar

    // Lê um byte da serial do leitor
    int ssvalue = ssrfid.read();
    if (ssvalue == -1) return;      // -1 significa nenhum dado válido lido (proteção adicional)

    // Detecta início e fim de quadro conforme o protocolo
    if (ssvalue == 2) {             // 0x02: HEAD (início do quadro)
      buffer_index = 0;             // Reinicia a escrita no buffer
    } else if (ssvalue == 3) {      // 0x03: TAIL (fim do quadro)
      call_extract_tag = true;      // Sinaliza que o quadro foi finalizado
    }

    // Proteção contra overflow do buffer (não esperado, mas seguro)
    if (buffer_index >= BUFFER_SIZE) {
      Serial.println("Error: Buffer overflow detected!");
      return;  // Aborta processamento; próximo HEAD deve sincronizar novamente
    }

    // Armazena o byte atual no buffer e avança índice
    buffer[buffer_index++] = (uint8_t)ssvalue;

    // Se detectamos o fim do quadro, checamos se o tamanho acumulado bate com 14
    if (call_extract_tag) {
      if (buffer_index == BUFFER_SIZE) {
        // Quadro completo: extrai informação (imprime campos e valida checksum)
        unsigned long tag = extract_tag();
        (void)tag; // Evita aviso de variável não utilizada se você não usar 'tag' fora da função
      } else {
        // Quadro com tamanho inesperado: descarta e aguarda próximo HEAD
        buffer_index = 0;
        return;
      }
    }
  }
}

unsigned long extract_tag() {
  // Mapeia ponteiros para cada campo dentro do buffer já preenchido
  uint8_t msg_head = buffer[0];             // Byte 0: HEAD (0x02)
  uint8_t *msg_data = buffer + 1;           // Bytes 1..10: dados ASCII HEX (2 ver + 8 tag)
  uint8_t *msg_data_version = msg_data;     // Bytes 1..2: versão (ASCII HEX)
  uint8_t *msg_data_tag = msg_data + 2;     // Bytes 3..10: tag (ASCII HEX, 8 bytes)
  uint8_t *msg_checksum = buffer + 11;      // Bytes 11..12: checksum (ASCII HEX, 2 bytes)
  uint8_t msg_tail = buffer[13];            // Byte 13: TAIL (0x03)

  // ---- Log detalhado do quadro recebido ----
  Serial.println("--------");
  Serial.print("Message-Head: ");
  Serial.println(msg_head);  // Mostra valor bruto do head (deve ser 2)

  Serial.println("Message-Data (HEX): ");
  // Imprime os 2 bytes de versão como caracteres (devem ser dígitos HEX '0'..'9','A'..'F')
  for (int i = 0; i < DATA_VERSION_SIZE; ++i) Serial.print(char(msg_data_version[i]));
  Serial.println(" (version)");
  // Imprime os 8 bytes da tag como caracteres HEX
  for (int i = 0; i < DATA_TAG_SIZE; ++i) Serial.print(char(msg_data_tag[i]));
  Serial.println(" (tag)");

  // Imprime os 2 bytes do checksum como caracteres HEX
  Serial.print("Message-Checksum (HEX): ");
  for (int i = 0; i < CHECKSUM_SIZE; ++i) Serial.print(char(msg_checksum[i]));
  Serial.println();

  Serial.print("Message-Tail: ");
  Serial.println(msg_tail);  // Deve ser 3
  Serial.println("--");

  // Converte os 8 chars HEX da tag para valor numérico (unsigned long)
  // Observação: o retorno é decimal no println; use println(tag, HEX) para exibir em HEX.
  unsigned long tag = (unsigned long)hexstr_to_value(msg_data_tag, DATA_TAG_SIZE);
  Serial.print("Extracted Tag: ");
  Serial.println(tag); // Imprime em decimal; prefira Serial.println(tag, HEX) se quiser em hexadecimal

  // Calcula o checksum: XOR dos valores numéricos de cada par (2 bytes HEX) dentro dos 10 bytes de dados
  // Percorremos a área de dados (10 bytes ASCII HEX) de 2 em 2
  long checksum = 0;
  for (int i = 0; i < DATA_SIZE; i += CHECKSUM_SIZE) {
    long val = hexstr_to_value(msg_data + i, CHECKSUM_SIZE);  // Converte 2 chars HEX para valor (0x00..0xFF)
    checksum ^= val;                                          // XOR acumulado
  }

  // Mostra o checksum calculado em hexadecimal
  Serial.print("Extracted Checksum (HEX): ");
  Serial.print(checksum, HEX);

  // Compara com o checksum transmitido (também convertido de ASCII HEX para número)
  if (checksum == hexstr_to_value(msg_checksum, CHECKSUM_SIZE)) {
    Serial.print(" (OK)");      // Bateu com o recebido → quadro íntegro
  } else {
    Serial.print(" (NOT OK)");  // Divergência → ruído/erro na transmissão
  }

  Serial.println();
  Serial.println("--------");

  // Retorna a tag convertida (útil se você quiser usar fora desta função)
  return tag;
}

long hexstr_to_value(const uint8_t *str, unsigned int length) {
  // Converte um trecho de 'length' bytes ASCII HEX (sem '\0') para valor numérico (base 16).
  // Ex.: ['4','1'] -> 0x41 -> decimal 65.
  // Como strtol exige string terminada em '\0', fazemos uma cópia local terminada.

  char *copy = (char*)malloc(length + 1);  // Aloca 'length' chars + 1 para o terminador NUL
  if (!copy) return 0;
}
