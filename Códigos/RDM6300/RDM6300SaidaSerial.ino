	// (c) Michael Schoeffler 2018, http://www.mschoeffler.de
// Leitura do RDM630/RDM6300 e saída serial apenas da TAG em HEX (ex.: "TAG: 1A2B3C4D")

#include <SoftwareSerial.h>
#include <stdlib.h>

// Protocolo do RDM630/RDM6300
const int BUFFER_SIZE = 14;       // 1 head (0x02), 10 data (2 ver + 8 tag), 2 checksum, 1 tail (0x03)
const int DATA_SIZE = 10;         // 10 bytes (ASCII HEX) => 2 "version" + 8 "tag"
const int DATA_VERSION_SIZE = 2;  // 2 bytes ASCII HEX
const int DATA_TAG_SIZE = 8;      // 8 bytes ASCII HEX (TAG)
const int CHECKSUM_SIZE = 2;      // 2 bytes ASCII HEX

// RX no pino 6, TX no pino 8 (TX não usado pelo leitor)
SoftwareSerial ssrfid = SoftwareSerial(6, 8);

uint8_t buffer[BUFFER_SIZE];
int buffer_index = 0;

// --- Helpers: conversão de HEX (ASCII) para valor numérico, sem malloc/strtol ---
int hexNibble(uint8_t c) {
  if (c >= '0' && c <= '9') return c - '0';
  if (c >= 'A' && c <= 'F') return c - 'A' + 10;
  if (c >= 'a' && c <= 'f') return c - 'a' + 10;
  return -1; // inválido
}

// Converte sequência ASCII-HEX (length nibbles) em inteiro (long)
long hexstr_to_value(const uint8_t *str, unsigned int length) {
  long v = 0;
  for (unsigned int i = 0; i < length; ++i) {
    int nib = hexNibble(str[i]);
    if (nib < 0) return 0; // caractere inválido -> 0 (fallback simples)
    v = (v << 4) | nib;
  }
  return v;
}

// Imprime "TAG: <8-hex>" com base nos 8 bytes ASCII presentes no quadro
void printTagHex(const uint8_t *tag_ascii) {
  char out[DATA_TAG_SIZE + 1];
  for (int i = 0; i < DATA_TAG_SIZE; ++i) out[i] = (char)tag_ascii[i];
  out[DATA_TAG_SIZE] = '\0';
  Serial.print("TAG: ");
  Serial.println(out);
}

// Processa o quadro completo no buffer e, se checksum confere, imprime a TAG
void process_frame_and_print() {
  // Mapeia campos
  uint8_t *msg_data         = buffer + 1;      // [1..10]
  uint8_t *msg_data_tag     = msg_data + 2;    // [3..10] => 8 ASCII HEX
  uint8_t *msg_checksum     = buffer + 11;     // [11..12]

  // Calcula checksum = XOR de cada par (2 chars HEX) dentro dos 10 bytes de dados
  long checksum = 0;
  for (int i = 0; i < DATA_SIZE; i += CHECKSUM_SIZE) {
    long val = hexstr_to_value(msg_data + i, CHECKSUM_SIZE);
    checksum ^= val;
  }

  // Compara com checksum recebido (2 chars HEX)
  long chk_rx = hexstr_to_value(msg_checksum, CHECKSUM_SIZE);

  if (checksum == chk_rx) {
    // Quadro válido: imprime somente a TAG (como veio, em ASCII-HEX)
    printTagHex(msg_data_tag);
  }
  // Se inválido, não imprime nada (silencioso). Ajuste conforme sua necessidade.
}

void setup() {
  Serial.begin(9600);    // Saída para outro dispositivo/PC
  ssrfid.begin(9600);    // Porta do leitor
  ssrfid.listen();
}

void loop() {
  if (ssrfid.available() > 0) {
    bool call_process = false;

    int ssvalue = ssrfid.read();
    if (ssvalue == -1) return;

    if (ssvalue == 2) {           // HEAD
      buffer_index = 0;
    } else if (ssvalue == 3) {    // TAIL
      call_process = true;
    }

    if (buffer_index >= BUFFER_SIZE) {
      // Overflow: descarta e espera novo HEAD
      buffer_index = 0;
      return;
    }

    buffer[buffer_index++] = (uint8_t)ssvalue;

    if (call_process) {
      if (buffer_index == BUFFER_SIZE) {
        process_frame_and_print();
      }
      // Sempre reseta para aguardar próximo quadro
      buffer_index = 0;
    }
  }
}
