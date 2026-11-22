import type { Translation } from '../i18n-types';

const it: Translation = {
  LANGUAGE: 'Lingua',
  RETRY: 'Riprova', // DeepL
  IS_REQUIRED: '{0} è obbligatorio', // DeepL
  SIGN_IN: 'Accedi', // DeepL
  SIGN_IN_SUBTITLE: 'Gateway per impianti di riscaldamento EMS',
  SIGN_OUT: 'Esci', // DeepL
  USERNAME: 'Nome utente', // DeepL
  PASSWORD: 'Password',
  APPLICATION_SETTINGS: 'Impostazioni applicazione',
  AP_SETTINGS: 'Impostazioni punto di accesso',
  NETWORK_SETTINGS: 'Impostazioni di rete',
  NTP_SETTINGS: 'Impostazioni NTP',
  MQTT_SETTINGS: 'Impostazioni MQTT',
  EMS_BUS_SETTINGS: 'Impostazioni bus EMS',
  HELP: 'Aiuto',
  LOGGED_IN: 'Hai effettuato l\'accesso come {name}', // DeepL
  PLEASE_SIGNIN: 'Accedi per continuare', // DeepL
  UPLOAD_SUCCESSFUL: 'Caricamento completato con successo', // DeepL
  DOWNLOAD_SUCCESSFUL: 'Scarica con successo', // DeepL
  INVALID_LOGIN: 'Nome utente o password errati', // DeepL
  NETWORK: 'Rete',
  SECURITY: 'Sicurezza',
  ONOFF_CAP: 'ON/OFF',
  ONOFF: 'on/off',
  TYPE: 'Tipo',
  ENTITIES: 'entità',
  EXPORT: 'Esporta',
  FAVORITES: "Preferiti",
  CLIENT_ID: 'ID client',
  DEVICE_ID: 'ID dispositivo',
  PRODUCT_ID: 'ID prodotto',
  ENTITY_ID: 'ID entità',
  SENSOR_ID: 'ID sensore',
  TYPE_ID: 'ID tipo',
  DEVICE: 'Dispositivo',
  BRAND: 'Marchio', // DeepL
  VERSION: 'Versione',
  ENTITY_NAME: 'Nome Entità',
  VALUE: 'Valore',
  DEVICES: 'Dispositivi',
  SENSORS: 'Sensori',
  AUTOMATION: 'Automazione',
  CONNECTIVITY: 'Connettività',
  MAINTENANCE: 'Manutenzione',
  CHANGE_VALUE: 'Modifica valore', // DeepL
  CANCEL: 'Annulla',
  REMOVE_ALL: 'Elimina Tutto', // DeepL
  APPLY_CHANGES: 'Applica le modifiche ({0})', // DeepL
  UPDATE: 'Aggiornamento', // DeepL
  REMOVE: 'Elimina',
  RESET: 'Reimposta',
  PROBLEM_UPDATING: 'Problema di aggiornamento', // DeepL
  PROBLEM_LOADING: 'Problema di caricamento', // DeepL
  ANALOG_SENSORS: 'Sensori Analogici',
  SETTINGS: 'Impostazioni',
  SETTINGS_UPDATED: 'Impostazioni aggiornate',
  NAME_UPDATED: 'Nome aggiornato',
  SENSOR_UPDATED: 'Sensore aggiornato',
  ANALOG_SENSOR_UPDATED: 'Sensore analogico aggiornato',
  OFFSET: 'Offset',
  FACTOR: 'Fattore',
  FREQ: 'Frequenza',
  DUTY_CYCLE: 'Ciclo di funzionamento', // DeepL
  UNIT: 'UoM',
  STARTVALUE: 'Valore iniziale', // DeepL
  EDIT: 'Modifica',
  TEMP_SENSORS: 'Sensori Temperatura',
  WRITE_CMD_SENT: 'Comando "scrivi" inviato', // DeepL
  EMS_BUS_WARNING: 'Bus EMS scollegato. Se questo avviso persiste dopo qualche secondo, controlla le impostazioni e il profilo della scheda', // DeepL
  CONNECTED: 'Connesso',
  TX_ISSUES: 'Problemi di trasmissione - controlla la modalità di trasmissione', // DeepL
  DISCONNECTED: 'Disconnesso',
  DATA_TRAFFIC: 'Traffico dati',
  EMS_DEVICE: 'Dispositivo EMS', // DeepL
  SUCCESS: 'SUCCESSO',
  FAIL: 'ERRORE', // DeepL
  QUALITY: 'QUALITÀ', // DeepL
  STATUS_NAMES: [
    'Telegrammi EMS Ricevuti (Rx)',
    'EMS Letti (Tx)',
    'EMS Scritti (Tx)',
    'Letture Sensori Temperatura',
    'Letture Sensori Analogici',
    'Pubblicazioni MQTT',
    'Chiamate API',
    'Messaggi Syslog'
  ],
  NUM_DAYS: '{num} giorni {{s}}',
  NUM_SECONDS: '{num} secondi {{s}}',
  NUM_HOURS: '{num} ore {{s}}',
  NUM_MINUTES: '{num} minuti {{s}}',
  APPLICATION: 'Applicazione',
  CUSTOMIZATIONS: 'Personalizzazione',
  APPLICATION_RESTARTING: 'EMS-ESP si sta riavviando', // DeepL
  BOARD_PROFILE: 'Profilo della scheda', // DeepL
  CUSTOM: 'Personalizzato', // DeepL
  GPIO_OF: '{0} GPIO', // DeepL
  BUTTON: 'Pulsante',
  TEMPERATURE: 'Temperatura',
  PHY_TYPE: 'Tipo di PHY Eth', // DeepL
  DISABLED: 'disabilitato', // DeepL
  TX_MODE: 'Modalità EMS Tx', // DeepL
  EMS_BUS: 'EMS Bus',
  EMS_BUS_ID: 'ID EMS Bus',
  HARDWARE: 'Hardware',
  LANGUAGE_ENTITIES: 'Lingua (per le entità dei dispositivi)', // DeepL
  HIDE_LED: 'Nascondi LED',
  ENABLE_TELNET: 'Abilita la console Telnet', // DeepL
  ENABLE_ANALOG: 'Abilita i sensori analogici', // DeepL
  CONVERT_FAHRENHEIT: 'Converti i valori di temperatura in gradi Fahrenheit', // DeepL
  READONLY: 'Attiva la modalità di sola lettura (blocca tutti i comandi di scrittura in uscita EMS Tx)', // DeepL
  WRITE_PROTECTION: 'Proteggi i dispositivi EMS da scritture ripetute e frequenti',
  WRITE_LIMIT: 'Numero massimo di scritture al minuto (0 = nessun limite)', // DeepL
  WRITE_LIMIT_REFUSE: 'Non superare il limite di lunghezza (altrimenti viene registrato solo un avviso)', // DeepL
  UNDERCLOCK_CPU: 'Riduci la velocità della CPU', // DeepL
  REMOTE_TIMEOUT: 'Timeout remoto', // DeepL
  REMOTE_TIMEOUT_EN: 'Disattiva il controllo remoto in caso di temperatura ambiente mancante', // DeepL
  HEATINGOFF: 'Avvia la caldaia con il riscaldamento forzato disattivato', // DeepL
  MIN_DURATION: 'Tempo di attesa', // DeepL
  ENABLE_SHOWER_TIMER: 'Attiva il timer della doccia', // DeepL
  ENABLE_SHOWER_ALERT: 'Attiva l\'avviso doccia', // DeepL
  TRIGGER_TIME: 'Ora di attivazione', // DeepL
  COLD_SHOT_DURATION: 'Durata del "cold shot"', // DeepL
  FORMATTING_OPTIONS: 'Opzioni di formattazione',
  BOOLEAN_FORMAT_DASHBOARD: 'Formato booleano web', // DeepL
  BOOLEAN_FORMAT_API: 'Formato booleano API/MQTT',
  ENUM_FORMAT: 'Formato Enum API/MQTT', // DeepL
  INDEX: 'Indice',
  ENABLE_PARASITE: 'Abilita l\'alimentazione parassita 1-Wire', // DeepL
  LOG_HEX: 'Registra i telegrammi EMS in formato esadecimale', // DeepL
  ENABLE_SYSLOG: 'Abilita Syslog',
  LOG_LEVEL: 'Livello di log', // DeepL
  MARK_INTERVAL: 'Segna l\'intervallo', // DeepL
  SECONDS: 'secondi',
  MINUTES: 'minuti',
  HOURS: 'ore',
  RESTART: 'Riavvia',
  RESTART_TEXT: 'Per applicare le modifiche alle impostazioni di sistema, è necessario riavviare EMS-ESP', // DeepL
  RESTART_CONFIRM: 'Sei sicuro di voler riavviare EMS-ESP?',
  COMMAND: 'Comando',
  CUSTOMIZATIONS_RESTART: 'Tutte le personalizzazioni sono state rimosse. Riavvio in corso…', // DeepL
  CUSTOMIZATIONS_FULL: 'Il numero di entità selezionate ha superato il limite. Salva in lotti', // DeepL
  CUSTOMIZATIONS_SAVED: 'Personalizzazioni aggiornate', // DeepL
  CUSTOMIZATIONS_HELP_1: 'Seleziona un dispositivo e personalizza le opzioni delle entità', // DeepL
  CUSTOMIZATIONS_HELP_2: 'aggiungi ai Preferiti', // DeepL
  CUSTOMIZATIONS_HELP_3: 'disabilita l\'azione di scrittura', // DeepL
  CUSTOMIZATIONS_HELP_4: 'Escludili da MQTT e dall\'API', // DeepL
  CUSTOMIZATIONS_HELP_5: 'nascondi dai Dispositivi', // DeepL
  CUSTOMIZATIONS_HELP_6: 'rimuovi dalla memoria',
  SELECT_DEVICE: 'Seleziona un dispositivo',
  MODIFIED: 'Modificato',
  RESET_SELECTED: 'Ripristina selezione', // DeepL
  OPTIONS: 'Opzioni',
  NAME: 'Nome',
  CUSTOMIZATIONS_RESET: 'Sei sicuro di voler rimuovere tutte le personalizzazioni?',
  SYSTEM_INFORMATION: 'Informazioni di sistema', // DeepL
  HELP_INFORMATION_1: 'Visita il wiki online per trovare le istruzioni su come configurare EMS-ESP', // DeepL
  HELP_INFORMATION_2: 'Per partecipare alla chat live della community, unisciti al nostro server Discord', // DeepL
  HELP_INFORMATION_3: 'Per richiedere una funzionalità o segnalare un bug', // DeepL
  HELP_INFORMATION_4: 'Quando segnali un problema, includi le informazioni sul tuo sistema per ottenere una risposta più rapida', // DeepL
  UPLOAD: 'Carica',
  DOWNLOAD: 'Scarica',
  INSTALL: 'Installa', // DeepL
  REINSTALL: 'Reinstalla', // DeepL
  REINSTALL_CONFIRM: 'Sei sicuro di voler reinstallare la versione {0}?', // DeepL
  FAILED: 'non riuscito', // DeepL
  SYSTEM: 'Sistema',
  SYSTEM_LOG: 'Registro di sistema',
  STATUS: 'Stato',
  DOWNLOAD_UPLOAD: 'Scarica/Carica', // DeepL
  CLOSE: 'Chiudi', // DeepL
  COMMAND_GUEST: 'Gli ospiti possono eseguire questo comando',
  TOKENS_RESET: 'Questo aggiornamento firma i token di accesso con una nuova chiave del dispositivo, quindi i token di accesso API esistenti non funzionano più. Crea una chiave API per ogni integrazione che utilizza l\'API, come Home Assistant, e usala al posto del vecchio token.', // DeepL
  API_KEYS: 'Chiavi API',
  API_KEYS_TEXT: 'Le chiavi API consentono a integrazioni come Home Assistant e agli script di utilizzare l\'API. Invia la chiave nell\'intestazione "Authorization: Bearer <CHIAVE>". Le chiavi di lettura possono leggere valori ed eseguire comandi che possono essere eseguiti dagli ospiti, mentre le chiavi di lettura e scrittura possono fare tutto ciò che un amministratore può fare tramite l\'API.', // DeepL
  API_KEY_ACCESS: 'Accesso',
  API_KEY_READ: 'Leggi', // DeepL
  API_KEY_READ_WRITE: 'Lettura e Scrittura', // DeepL
  API_KEY_SHOW_ONCE: 'Copia subito questa chiave e conservala in un posto sicuro. Non verrà più visualizzata.', // DeepL
  CREATED: 'Creato', // DeepL
  COPY: 'Testo', // DeepL
  COPIED: 'Copiato',
  USE: 'Usa',
  FACTORY_RESET: 'Ripristino di Fabbrica', // DeepL
  SYSTEM_FACTORY_TEXT_DIALOG: 'Sei sicuro di voler ripristinare le impostazioni di fabbrica di EMS-ESP?', // DeepL
  STABLE: 'Stabile',
  DEVELOPMENT: 'Sviluppo',
  UPTIME: 'Tempo di funzionamento del sistema', // DeepL
  LAST_RESTART: 'Ultimo riavvio',
  POWER_AND_RESET: 'Accensione e Reset', // DeepL
  FREE_MEMORY: 'Memoria libera', // DeepL
  BUFFER_SIZE: 'Dimensione massima del buffer', // DeepL
  COMPACT: 'Conciso', // DeepL
  SUMMARY: 'Riepilogo',
  SHOW_MORE: 'Mostra altri {n}',
  SHOW_LESS: 'Mostra meno',
  COREDUMP: 'Crash Dump', // DeepL
  DOWNLOAD_SYSTEM_BACKUP: 'Backup di Sistema', // DeepL
  ERROR: 'Errore imprevisto, prova di nuovo', // DeepL
  TIME_SET: 'Ora impostata', // DeepL
  MANAGE_USERS: 'Gestisci utenti', // DeepL
  USER_WARNING: 'Devi avere almeno un utente amministratore configurato', // DeepL
  ADD: 'Aggiungi',
  TELNET_ENABLED: 'La console Telnet è abilitata. Telnet non è crittografato, quindi disattivalo nelle Impostazioni dell\'applicazione se non lo usi.', // DeepL
  TOO_MANY_LOGINS: 'Troppi tentativi di accesso falliti, riprova tra qualche minuto', // DeepL
  PASSWORD_KEEP: 'Lascia vuoto per mantenere la password attuale',
  SETUP_TITLE: 'Imposta le tue password',
  SETUP_TEXT: 'Questo dispositivo utilizza ancora le password predefinite di fabbrica. Imposta le tue prima di continuare.', // DeepL
  SETUP_ADMIN_PASSWORD: 'Nuova password per {name}',
  SETUP_AP_PASSWORD: 'Nuova password del punto di accesso', // DeepL
  SETUP_AP_REQUIRED: 'Non c\'è ancora nessuna rete Wi-Fi configurata, quindi l\'accesso avviene tramite l\'access point. 8-63 caratteri, verrà utilizzato al prossimo avvio dell\'access point.', // DeepL
  SETUP_AP_OPTIONAL: 'Consigliato: da 8 a 63 caratteri; verrà applicato al prossimo avvio del punto di accesso. Lascia vuoto per mantenere quello attuale.', // DeepL
  SETUP_AP_DEFAULT: 'L\'access point utilizza ancora la password predefinita. Imposta la tua password nelle impostazioni dell\'access point.', // DeepL
  SETUP_VIEW_USER: 'Opzionale: aggiungi un utente ospite in sola lettura.', // DeepL
  NETWORK_CORS_ORIGIN_HELP: 'L\'indirizzo web che permette di richiamare l\'API da un browser, ad esempio http://homeassistant.local:8123', // DeepL
  NETWORK_CORS_WILDCARD: 'Con * qualsiasi sito web aperto in un browser sulla tua rete può richiamare l’API. Imposta invece l’indirizzo esatto.', // DeepL
  NETWORK_ALLOWED_HOSTS: 'Nomi host consentiti',
  NETWORK_ALLOWED_HOSTS_HELP: 'Nomi alternativi: questo dispositivo può essere aperto in un browser, ad esempio come ems-esp.fritz.box o con il nome di un proxy inverso. Il suo indirizzo IP, il nome host e il nome host.local funzionano sempre', // DeepL
  NOT_ENABLED: 'Non abilitato',
  MQTT_ERRORS: 'Errori MQTT',
  DISCONNECT_REASON: 'Motivo della disconnessione', // DeepL
  MQTT_BASE_TOPIC: 'Argomento di base', // DeepL
  LAST_DISCONNECT: 'Ultima disconnessione',
  CONNECTED_SINCE: 'Connesso da', // DeepL
  ENABLE_MQTT: 'Abilita MQTT',
  BROKER: 'Broker',
  BASE_TOPIC: 'Base',
  OPTIONAL: 'facoltativo', // DeepL
  MQTT_FORMAT: 'Formato argomento/payload', // DeepL
  MQTT_NEST_1: 'Raggruppati in un unico argomento', // DeepL
  MQTT_NEST_2: 'Come singoli argomenti', // DeepL
  MQTT_RESPONSE: 'Pubblica l\'output dei comandi su un argomento `response`', // DeepL
  MQTT_PUBLISH_TEXT_1: 'Pubblica argomenti con un singolo valore quando c\'è un cambiamento', // DeepL
  MQTT_PUBLISH_TEXT_2: 'Pubblica negli argomenti di comando (ioBroker)', // DeepL
  MQTT_PUBLISH_TEXT_3: 'Abilita il rilevamento MQTT', // DeepL
  MQTT_PUBLISH_TEXT_4: 'Prefisso per gli argomenti di rilevamento', // DeepL
  MQTT_PUBLISH_TEXT_5: 'Tipo di scoperta', // DeepL
  MQTT_INPUT_NUMBER_FORMAT: 'Formato dei numeri inseriti', // DeepL
  MQTT_PUBLISH_INTERVALS: 'Intervalli di pubblicazione', // DeepL
  MQTT_INT_BOILER: 'Caldaie e pompe di calore', // DeepL
  MQTT_INT_THERMOSTATS: 'Termostati',
  MQTT_INT_SOLAR: 'Moduli solari',
  MQTT_INT_MIXER: 'Moduli miscelatori', // DeepL
  MQTT_INT_WATER: 'Moduli acqua', // DeepL
  MQTT_QUEUE: 'Coda MQTT',
  DEFAULT: 'Impostazione predefinita', // DeepL
  MQTT_ENTITY_FORMAT: 'Formato dell\'ID entità', // DeepL
  MQTT_ENTITY_FORMAT_0: 'Unica occorrenza, nome lungo (v3.4)', // DeepL
  MQTT_ENTITY_FORMAT_1: 'Unica occorrenza, nome breve', // DeepL
  MQTT_ENTITY_FORMAT_2: 'Istanze multiple, nome breve',
  MQTT_CLEAN_SESSION: 'Imposta una sessione pulita', // DeepL
  MQTT_RETAIN_FLAG: 'Imposta sempre il flag di conservazione', // DeepL
  MQTT_COMMANDS: 'Comandi ricevuti tramite MQTT', // DeepL
  MQTT_COMMANDS_0: 'Spento, solo lettura dei valori', // DeepL
  MQTT_COMMANDS_1: 'Modifica i valori ed esegui i comandi', // DeepL
  MQTT_COMMANDS_2: 'Tutto, compresi i comandi di sistema', // DeepL
  INACTIVE: 'Inattivo',
  ACTIVE: 'Attivo',
  UNKNOWN: 'Sconosciuto',
  SET_TIME: 'Imposta Ora', // DeepL
  SET_TIME_TEXT: 'Inserisci qui sotto la data e l\'ora locali per impostare l\'ora', // DeepL
  LOCAL_TIME: 'Ora locale',
  UTC_TIME: 'Ora UTC',
  ENABLE_NTP: 'Abilita NTP',
  NTP_SERVER: 'Server NTP',
  TIME_ZONE: 'Fuso orario',
  ACCESS_POINT: 'Punto di accesso', // DeepL
  AP_PROVIDE: 'Abilita l\'Access Point', // DeepL
  AP_PROVIDE_TEXT_2: 'Quando non si trova la connessione di rete', // DeepL
  AP_PROVIDE_TEXT_3: 'Mai', // DeepL
  AP_PREFERRED_CHANNEL: 'Canale preferito',
  AP_HIDE_SSID: 'Nascondi SSID',
  AP_CLIENTS: 'Clienti AP',
  AP_MAX_CLIENTS: 'Numero massimo di client', // DeepL
  AP_LOCAL_IP: 'IP locale', // DeepL
  IDLE: 'Inattivo',
  SCAN_AGAIN: 'Riprova la scansione', // DeepL
  NETWORK_NO_WIFI: 'Nessuna rete Wi-Fi trovata', // DeepL
  NETWORK_BLANK_BSSID: 'lascia vuoto per usare solo l\'SSID', // DeepL
  TX_POWER: 'Potenza di trasmissione', // DeepL
  HOSTNAME: 'Nome host', // DeepL
  NETWORK_DISABLE_SLEEP: 'Disattiva la modalità di sospensione Wi-Fi', // DeepL
  NETWORK_LOW_BAND: 'Usa una larghezza di banda Wi-Fi inferiore', // DeepL
  NETWORK_USE_DNS: 'Abilita il servizio mDNS', // DeepL
  NETWORK_ENABLE_CORS: 'Abilita CORS',
  NETWORK_CORS_ORIGIN: 'Origine CORS', // DeepL
  NETWORK_FIXED_IP: 'Usa un indirizzo IP fisso', // DeepL
  NETWORK_GATEWAY: 'Gateway',
  NETWORK_SUBNET: 'Maschera di sottorete', // DeepL
  NETWORK_DNS: 'Server DNS',
  ADDRESS_OF: 'Indirizzo {0}',
  ADMINISTRATOR: 'Amministratore',
  GUEST: 'Ospite',
  NEW: 'Nuovo',
  NEW_ENTITY_NAME: 'Nuovo nome entità',
  MIN: 'min',
  MAX: 'max',
  BLOCK_NAVIGATE_1: 'Hai delle modifiche non salvate', // DeepL
  BLOCK_NAVIGATE_2: 'Se vai su un\'altra pagina, le modifiche non salvate andranno perse. Sei sicuro di voler uscire da questa pagina?', // DeepL
  UPLOAD_ABORTED: 'Caricamento interrotto',
  BLOCK_BUSY_1: 'Ancora in fase di elaborazione', // DeepL
  BLOCK_BUSY_2: 'Se esci da questa pagina ora, l\'operazione verrà interrotta. Sei sicuro di voler uscire?', // DeepL
  DISCARD: 'Elimina', // DeepL
  COMMANDS_UPDATED: 'Comandi aggiornati',
  V_REQUIRED: 'Obbligatorio',
  V_BETWEEN: 'Devono essere compresi tra {min} e {max}', // DeepL
  V_MAX_CHARS: 'Devono contenere al massimo {max} caratteri', // DeepL
  V_CHARS: 'Devono contenere da {min} a {max} caratteri', // DeepL
  V_IN_USE: 'Già in uso',
  V_IP: 'Deve essere un indirizzo IP',
  V_HOSTNAME: 'Deve essere un nome host valido',
  V_IP_OR_HOSTNAME: 'Deve essere un indirizzo IP o un nome host',
  V_NAME: 'Fino a {max} caratteri: lettere, cifre o _',
  V_USERNAME: 'Fino a {max} caratteri: lettere, cifre, _ o .',
  V_DAY: 'Scegli almeno un giorno',
  V_HEX: 'Deve essere un numero esadecimale',
  V_FINGERPRINT: 'Deve essere un fingerprint SHA-1 (40 cifre esadecimali)', // DeepL
  V_EXAMPLE: 'Devono avere questo aspetto: {example}', // DeepL
  V_NOT_DEFAULT: 'Scegli una password diversa', // DeepL
  ZERO_DISABLES: '0 lo spegne', // DeepL
  MINUS_ONE_DISABLES: '-1 lo spegne', // DeepL
  LED_TYPE: 'Tipo di LED',
  PHY_POWER: 'Alimentazione PHY',
  PHY_CLOCK: 'Orologio PHY', // DeepL
  AUTO: 'Auto',
  PORT: 'Porta',
  TIMEOUT: 'Timeout',
  HOST: 'Host',
  SMTP_SERVER: 'Server SMTP',
  KEEP_ALIVE: 'Rimani attivo', // DeepL
  MQTT_REFRESH: 'Aggiorna MQTT',
  MQTT_REFRESHED: 'MQTT aggiornato',
  MQTT_R_USER: 'Disconnesso dall\'utente',
  MQTT_R_TCP: 'Connessione TCP chiusa',
  MQTT_R_PROTOCOL: 'Versione del protocollo non accettata',
  MQTT_R_ID: 'ID cliente non valido', // DeepL
  MQTT_R_SERVER: 'Server non disponibile',
  MQTT_R_CREDENTIALS: 'Credenziali non valide',
  MQTT_R_AUTH: 'Non autorizzato',
  MQTT_R_FINGERPRINT: 'L\'impronta TLS non corrisponde',
  GPIO_INVALID: 'Impostazioni GPIO non valide',
  CONNECTED_VIA: 'Connesso tramite {0}',
  NETWORK_NOT_FOUND: 'Rete non trovata',
  CONNECTION_FAILED: 'Connessione fallita', // DeepL
  CONNECTION_LOST: 'Connessione persa',
  AP_LINGERING: 'Attesa in stato di inattività', // DeepL
  NONE: 'Nessuno',
  LOG_NO_CONNECTION: 'Nessuna connettività al servizio di log', // DeepL
  CHECK_SYSTEM_LOG: 'Controlla il log di sistema per individuare le possibili cause', // DeepL
  VALUE_TYPE: 'Tipo di valore',
  DEFAULT_VALUE: 'Valore predefinito',
  DEFAULT_NAME: 'Nome predefinito',
  ALWAYS_OFF: 'Sempre spento',
  ALWAYS_ON: 'Sempre attivo', // DeepL
  NO_DATA: 'Non ci sono ancora entità preferite. Contrassegnale in {customizations} oppure visualizza tutte le entità in {devices}.', // DeepL
  STAY: 'Resta', // DeepL
  LEAVE: 'Lascia', // DeepL
  MENU: 'Menu',
  MORE: 'Altro',
  SHOW_PASSWORD: 'Mostra password',
  STATUS_RUNNING: 'EMS-ESP è in esecuzione', // DeepL
  NUM_ERRORS: '{num} error{{e|i}}',
  NUM_WARNINGS: '{num} avvis{{o|i}}',
  HEAP: 'Heap',
  METER_USED: '{used} / {total} {unit} utilizzata', // DeepL
  MAX_BLOCK: 'Blocco massimo di {num} KB', // DeepL
  MAX_BLOCK_HELP: 'Il "Max block" è la porzione massima di heap che può essere allocata in una sola volta. Se scende sotto i 40 KB, l\'interfaccia web e MQTT potrebbero smettere di funzionare.', // DeepL
  FLASH_CHIP: 'Chip Flash', // DeepL
  FILE_SYSTEM: 'Sistema di file', // DeepL
  DETAILS: 'Dettagli',
  RECONNECTS: 'Si riconnette', // DeepL
  CONNECTS: 'Si collega', // DeepL
  ACTIVITY_HELP: 'È normale che si verifichino alcuni errori di ricezione (Rx). Se gli errori di trasmissione (Tx) sono molti, di solito significa che la modalità di trasmissione impostata non è quella giusta per la tua caldaia.', // DeepL
  PAUSE: 'Pausa',
  RESUME: 'Curriculum', // DeepL
  LOG_PAUSED: 'In pausa, nuove righe trattenute: {num}', // DeepL
  FILTER_LINES: 'Filtra le righe', // DeepL
  SCHEDULER: 'Programma eventi',
  SCHEDULER_HELP_1: 'Automatizza i comandi aggiungendo qui sotto gli eventi programmati', // DeepL
  SCHEDULER_HELP_2: "all'avvio",
  SCHEDULE: 'Programma eventi', // DeepL
  TIME: 'Ora',
  TIMER: 'Timer', // DeepL
  ONCHANGE: 'In caso di modifica', // DeepL
  CONDITION: 'Condizione',
  SCHEDULE_UPDATED: 'Programma eventi aggiornato', // DeepL
  CUSTOM_ENTITIES: 'Entità personalizzate',
  ENTITIES_HELP_1: 'Definisci entità EMS personalizzate o variabili utente dinamiche', // DeepL
  ENTITIES_UPDATED: 'Entità aggiornate',
  WRITEABLE: 'Modificabile', // DeepL
  SHOWING: 'Visualizzazione', // DeepL
  SEARCH: 'Cerca', // DeepL
  CERT: 'Certificato CA (PEM)',
  ENABLE_TLS: 'Abilita TLS',
  TLS_CHECK: 'Controlla il broker con', // DeepL
  TLS_CHECK_CA: 'Certificato CA',
  TLS_CHECK_FINGERPRINT: 'Impronta del certificato (SHA-1)',
  TLS_CHECK_NONE: 'Niente, non selezionato', // DeepL
  TLS_NOT_CHECKED: 'La connessione è crittografata, ma il broker non viene verificato, quindi un altro dispositivo sulla rete potrebbe fingersi il broker stesso', // DeepL
  ON: 'Su', // DeepL
  OFF: 'Off',
  POLARITY: 'Polarità', // DeepL
  ACTIVEHIGH: 'Alto attivo', // DeepL
  ACTIVELOW: 'Attivo basso', // DeepL
  UNCHANGED: 'Inalterato', // DeepL
  SECURITY_1: 'Aggiungi o rimuovi utenti',
  DOWNLOAD_UPLOAD_1: 'Scarica e carica le impostazioni',
  MODULES: 'Module',
  MODULES_1: 'Attiva o disattiva i moduli esterni',
  NETWORK_DESC: 'Wi-Fi, Ethernet e nome host',
  AP_DESC: 'Hotspot integrato per la configurazione e l\'accesso di riserva',
  MQTT_DESC: 'Broker, topic e discovery',
  APPLICATION_DESC: 'Bus EMS, scheda, sensori e formattazione',
  NTP_DESC: 'Server dell\'ora e fuso orario',
  FIRMWARE_DESC: 'Versione, aggiornamenti e ripristino',
  MODULES_UPDATED: 'Moduli aggiornati',
  MODULES_NONE: 'Nessun modulo esterno rilevato',
  RENAME: 'Rinomina', // DeepL
  ENABLE_MODBUS: 'Abilita Modbus',
  VIEW_LOG: 'Controlla il log per individuare eventuali problemi', // DeepL
  SERVICES: 'Servizi',
  ALLVALUES: 'Tutti i valori',
  WAIT_FIRMWARE: 'Il firmware è in fase di caricamento e installazione', // DeepL
  INSTALL_VERSION: 'Verrà installata la versione {0}',
  UPDATE_AVAILABLE: 'aggiornamento disponibile',
  LATEST_VERSION: 'Stai utilizzando l\'ultima versione del firmware ({0})',
  PLEASE_WAIT: 'Attendi, per favore', // DeepL
  RESTARTING_PRE: 'Avvio', // DeepL
  RESTARTING_POST: 'Preparazione',
  AUTO_SCROLL: 'Scorrimento automatico',
  DASHBOARD: 'Pannello di controllo',
  DEVELOPER_MODE: 'Modalità sviluppatore',
  DISABLE_RESET: 'Disattiva il ripristino delle impostazioni di fabbrica', // DeepL
  AUTO_FW_CHECK: 'Verifica automaticamente la presenza di aggiornamenti del firmware', // DeepL
  BYTES: 'Byte',
  BITMASK: 'Maschera di bit', // DeepL
  DUPLICATE: 'Duplicato',
  DASHBOARD_1: 'Di seguito vengono visualizzate tutte le entità EMS attive contrassegnate come preferite, oltre a tutte le entità personalizzate, le programmazioni e i Sensori esterni.', // DeepL
  NO_GPIO: 'Nessun GPIO disponibile', // DeepL
  PLATFORM: 'Piattaforma',
  RELEASE_TYPE: 'Tipo di Versione', // DeepL
  FIRMWARE_VERSION_INFO: 'Informazioni sulla Versione del Firmware',
  USER_PROFILE: 'Profilo utente',
  ONLINE_HELP: 'Guida in linea', // DeepL
  UPGRADE_IMPORTANT_MESSAGES: 'Messaggi Importanti di Aggiornamento', // DeepL
  UPGRADE_IMPORTANT_MESSAGES_2: 'Stai effettuando l\'aggiornamento a una nuova versione principale. È consigliabile eseguire prima un backup del sistema. Controlla anche la guida in linea per eventuali modifiche che potrebbero causare problemi di compatibilità.', // DeepL
  UPGRADE_IMPORTANT_MESSAGES_3: 'La versione 3.10 presenta modifiche significative alla sicurezza. Fai prima un backup del sistema: dopo l\'aggiornamento le password vengono memorizzate in forma hash e le versioni precedenti non riescono a leggerle, quindi per tornare indietro è necessario questo backup. Ogni chiamata API richiede poi una chiave API, quindi le integrazioni come Home Assistant devono essere aggiornate. Leggi le modifiche significative nella guida online prima di continuare.', // DeepL
  WARNING_SYSTEM_BACKUP: 'Questo crea un backup completo della configurazione e delle impostazioni del tuo sistema. Le password e le chiavi non vengono incluse a meno che tu non le inserisca qui sotto; il ripristino del backup mantiene le password già presenti sul dispositivo. I comandi sono sempre inclusi e potrebbero contenere credenziali, quindi fai attenzione a chi li condividi. Vuoi continuare?', // DeepL
  BACKUP_INCLUDE_SECRETS: 'Includi password e chiavi (necessarie per il ripristino su un nuovo Dispositivo)', // DeepL
  TEST_EMAIL_SUCCESSFUL: 'Email di prova inviata', // DeepL
  SYSTEM_NAME: 'Nome del sistema',
  COMMANDS: 'Comandi',
  COMMANDS_HELP_1: 'Definisci qui sotto i comandi con nome riutilizzabili. Questi possono essere eseguiti dalla console, tramite API/MQTT o richiamati dal Programma eventi', // DeepL
  COMMAND_EXECUTED: 'Comando eseguito', // DeepL
  RUN: 'Esegui', // DeepL
  INTERNET_CONNECTION_CHECK: 'Verifica online le versioni disponibili', // DeepL
  UPLOAD_BACKUP_TEXT: 'Clicca per selezionare o trascina qui un file di backup (.json)', // DeepL
  UPLOAD_FIRMWARE_TEXT: 'Clicca per selezionare o trascina qui un file firmware .bin/.md5', // DeepL
  UPLOAD_MD5_RECEIVED: 'Digest ricevuto, ora carica il file .bin del firmware', // DeepL
  UPLOAD_MD5_MATCHED: 'Verifica MD5, installazione del firmware…', // DeepL
  UPLOAD_NOT_OFFICIAL: 'Questa non è una versione ufficiale di EMS-ESP. Installa solo firmware di cui ti fidi', // DeepL
  UPLOAD_NOT_OFFICIAL_HELP: 'Informazioni sulle versioni ufficiali e sulla personalizzazione', // DeepL
  UPLOAD_INSTALL_ANYWAY: 'Installa comunque',
  OFFICIAL_BUILD: 'Versione Ufficiale', // DeepL
  CUSTOM_BUILD: 'Configurazione Personalizzata', // DeepL
  USING_CUSTOM_FIRMWARE: 'Stai utilizzando un firmware personalizzato', // DeepL
  PASSWORD_CONFIRM_TITLE: 'Inserisci nuovamente la tua password', // DeepL
  PASSWORD_CONFIRM_TEXT: 'Per la tua sicurezza, inserisci la password prima di apportare questa modifica. Non ti verrà richiesta nuovamente per 5 minuti.', // DeepL
  PASSWORD_CONFIRM_BUTTON: 'Conferma',
  PASSWORD_WRONG: 'Password errata',
  PASSWORD_NOT_CONFIRMED: 'Operazione annullata, la password non è stata inserita', // DeepL
  RECOVERY_APP: 'App di ripristino',
  RECOVERY_NOT_INSTALLED: 'Non installato',
  RECOVERY_INSTALLED: 'Firmware di ripristino installato',
  RESTART_INTO_RECOVERY: 'Riavvia nell\'app di ripristino',
  RECOVERY_NOT_FOUND: 'App di ripristino non trovata online',
  RECOVERY_START_FAILED: 'L\'app Recovery non si avvia: potrebbe essere obsoleta o danneggiata. Installa prima l\'ultima versione', // DeepL
  PARTITION_START_FAILED: 'Questo firmware non può essere avviato, potrebbe essere danneggiato',
  INSTALLED_VERSIONS: 'Versioni installate',
  EMSESP_APP: 'App EMS-ESP',
  NO_UPDATES_AVAILABLE: 'nessun aggiornamento disponibile',
  SWITCH_TO_DEV: 'Passa al ramo dev {0}', // DeepL
  SWITCH_TO_STABLE: 'Passa alla versione stabile {0}', // DeepL
  SAVE: 'Salva',
  NEEDS_RESTART: 'È necessario un riavvio', // DeepL
  CHANGED_SETTINGS: '{num} {{impostazione modificata|impostazioni modificate}}',
  SAVED_RESTART: 'Salvato. Riavvia EMS-ESP per applicare le nuove impostazioni.', // DeepL
  RESTART_NOW: 'Riavvia ora',
  COPY_FAILED: 'Impossibile copiare: seleziona il testo e copialo tu stesso', // DeepL
  DOCUMENTATION: 'Documentazione',
  COMMUNITY_CHAT: 'Chat della community',
  REPORT_ISSUE: 'Segnala un bug o richiedi una funzionalità', // DeepL
  GET_HELP: 'Chiedi aiuto', // DeepL
  REPORT_PROBLEM: 'Segnala un problema', // DeepL
  REPORT_STEP_1: 'Scarica le informazioni di sistema',
  REPORT_STEP_1_TEXT: 'versione, scheda, stato del bus EMS, dispositivi collegati e impostazioni, senza password', // DeepL
  REPORT_STEP_2: 'Salva il registro recente',
  REPORT_STEP_2_TEXT: 'Se il problema riguarda un dispositivo, imposta prima il livello di log su DEBUG', // DeepL
  REPORT_STEP_3: 'Apri una segnalazione o chiedi su Discord',
  REPORT_STEP_3_TEXT: 'allega entrambi i file, oppure incolla il breve riassunto qui sotto in una chat', // DeepL
  NEW_ISSUE: 'Nuovo problema', // DeepL
  SHORT_SUMMARY: 'Breve riassunto', // DeepL
  LICENSE_TEXT: 'open source con licenza LGPL-3.0',
  ADMIN_ACCESS_TEXT: 'accesso completo alle impostazioni, ai Dispositivi e all\'API', // DeepL
  GUEST_ACCESS_TEXT: 'puoi visualizzare i valori ed eseguire i comandi che gli ospiti possono eseguire', // DeepL
  LANGUAGE_BROWSER_ONLY: 'cambia solo questo browser', // DeepL
  LANGUAGE_ENTITIES_HINT: 'La lingua per i nomi delle entità dei dispositivi in MQTT e nell\'API è impostata in', // DeepL
  ADMINISTRATION: 'Amministrazione',
  API_KEYS_SHORT: 'chiavi per Home Assistant e script',
  ADDRESSES: 'Indirizzi',
  SEARCH_SETTINGS: 'Impostazioni di ricerca', // DeepL
  NO_SETTINGS_MATCH: 'Nessuna impostazione corrisponde alla tua ricerca', // DeepL
  GENERAL: 'Informazioni generali', // DeepL
  BOARD: 'Scheda',
  ADVANCED: 'Avanzato', // DeepL
  EMS_BUS_DESC: 'Come EMS-ESP comunica con il tuo impianto di riscaldamento.', // DeepL
  BOARD_DESC: 'I pin provengono dal profilo della scheda. Per impostarli tu stesso, attiva la modalità Sviluppatore e seleziona Personalizzato.', // DeepL
  FORMAT_DESC: 'Come appaiono i valori nell\'interfaccia utente web e nei payload MQTT/API.', // DeepL
  BOARD_PINS: 'Pin dal profilo',
  ENABLE_EMAIL: 'Attiva le notifiche via e-mail', // DeepL
  SEND_TEST_EMAIL: 'Invia un\'e-mail di prova', // DeepL
  EMAIL_FROM: 'Da',
  EMAIL_TO: 'Per', // DeepL
  EMAIL_SUBJECT: 'Oggetto',
  WIFI_SCAN: 'Scansione Wi-Fi', // DeepL
  SCAN: 'Scansiona', // DeepL
  NETWORKS_NEARBY: 'Reti nelle vicinanze', // DeepL
  SIGNAL: 'Segnale',
  CHANNEL: 'Canale',
  WIFI_OPEN: 'apri', // DeepL
  WIFI_SAVED: 'salvato', // DeepL
  NETWORK_ETHERNET_INFO: 'Ora connesso via Ethernet ({ip}). Il Wi-Fi si usa solo senza cavo.', // DeepL
  HOSTNAME_ACCESS: 'Nome host e accesso',
  IP_ADDRESS: 'Indirizzo IP',
  NETWORK_ETH_10MBIT: 'Forza 10 Mbit half-duplex', // DeepL
  NETWORK_ETH_10MBIT_HELP: 'solo per interruttori difettosi o cavi lunghi', // DeepL
  NETWORK_DISABLE_SLEEP_HELP: 'più affidabile, consuma un po’ più di energia', // DeepL
  NETWORK_MDNS_HELP: 'Puoi accedervi come {host}.local tramite mDNS', // DeepL
  NETWORK_FIXED_IP_HELP: 'altrimenti si usa il DHCP',
  NETWORK_CORS_HELP: 'necessario solo quando un’altra pagina web chiama l’API', // DeepL
  NTP_TIME_ON_DEVICE: 'Tempo su EMS-ESP', // DeepL
  NTP_TIME_SERVER: 'Server di tempo', // DeepL
  NTP_THERMOSTAT_CLOCK: 'Orologio del termostato',
  NTP_THERMOSTAT_SYNC: 'Imposta l\'orologio del termostato',
  NTP_THERMOSTAT_SYNC_HELP: 'imposta la data e l\'ora sul termostato EMS', // DeepL
  NTP_THERMOSTAT_SYNC_1: 'On, fuso orario di EMS-ESP', // DeepL
  NTP_THERMOSTAT_SYNC_2: 'Acceso, fuso orario locale', // DeepL
  NTP_THERMOSTAT_TZ: 'Fuso orario del termostato',
  MQTT_PREVIEW: 'Cosa viene pubblicato',
  MQTT_PREVIEW_CMD: 'accettato come comando',
  MQTT_PREVIEW_RESPONSE: 'output dei comandi',
  MQTT_BEHAVIOUR: 'Comportamento',
  MQTT_TOPICS: 'Argomenti e contenuti', // DeepL
  MQTT_DISCOVERY: 'Scoperta', // DeepL
  MQTT_INTERVALS_HELP: 'Intervallo in secondi tra una pubblicazione e l\'altra, 0 = pubblica ad ogni modifica.', // DeepL
  MQTT_SINGLE_NO_DISCOVERY: 'non disponibile con la funzione di ricerca su', // DeepL
  MQTT_DISCOVERY_NO_SINGLE: 'non disponibile con argomenti a valore singolo su', // DeepL
  MQTT_DISCOVERY_HELP: 'crea automaticamente le entità in Home Assistant o Domoticz',
  MQTT_ENTITY_FORMAT_WARN: 'modificando questo, si rinominano tutte le entità in Home Assistant', // DeepL
  USERS: 'Utenti',
  USERS_TEXT: 'Gli amministratori possono modificare tutto. Gli ospiti possono solo visualizzare i contenuti ed eseguire i comandi contrassegnati come accessibili agli ospiti.', // DeepL
  ADD_USER: 'Aggiungi utente',
  EDIT_USER: 'Modifica utente',
  ROLE: 'Ruolo',
  ROLE_ADMIN: 'admin',
  ROLE_GUEST: 'ospite',
  ADMIN: 'Amministratore',
  YOU: 'tu',
  NEW_PASSWORD: 'Nuova password',
  UNSAVED_CHANGES: '{num} {{modifica non salvata|modifiche non salvate}}',
  CREATE_KEY: 'Crea Chiave',
  NEW_API_KEY: 'Nuova Chiave API',
  NO_API_KEYS: 'Non ci sono ancora chiavi API', // DeepL
  API_KEYS_LIMIT: 'Puoi avere al massimo {num} tasti.', // DeepL
  REVOKE: 'Revoca',
  REVOKE_KEY: 'Revoca Chiave API',
  REVOKE_KEY_TEXT: 'Qualsiasi cosa che contenga "{name}" smette subito di funzionare.', // DeepL
  MODULES_TEXT: 'Funzionalità opzionali integrate in questo firmware. Attivale o disattivale qui.', // DeepL
  BY_AUTHOR: 'di {name}',
  LICENSE_KEY: 'Chiave di licenza',
  MODULE_ACTIVATED: 'attivato',
  MODULE_PENDING: 'in attesa di attivazione',
  ENABLED: 'Abilitato',
  UNDO: 'Annulla',
  FW_UPDATES: 'Aggiornamenti',
  FW_VERSION_AVAILABLE: 'È disponibile la {version}', // DeepL
  FW_RELEASE_NOTES: 'Note di rilascio',
  FW_UPDATES_OFFLINE: 'Impossibile verificare online la presenza di nuove versioni. Per farlo, EMS-ESP ha bisogno di una connessione a Internet.', // DeepL
  FW_RESTART_RESET: 'Riavvio e Ripristino',
  FW_RECOVERY_TEXT: 'Riavviare nell’app di ripristino? Si connette alla tua rete e avvia anche un proprio punto di accesso, da cui puoi caricare il firmware.', // DeepL
  FW_SIZE: 'Dimensioni', // DeepL
  FW_INSTALL_DATE: 'Installato su', // DeepL
  FW_UPLOAD_FAILED: 'Caricamento non riuscito: tipo di file errato o firmware per un\'altra scheda',
  FILE_TYPE_INVALID: 'Questo tipo di file non è supportato, scegli un file {types}',
  WHO_CAN_RUN: 'Chi può eseguire',
  CMD_NONE: 'Nessun comando trovato',
  CMD_NONE_MATCH: 'Nessun comando corrisponde alla tua ricerca', // DeepL
  SCHEDULE_NONE: 'Nessuna programmazione trovata', // DeepL
  SCHEDULE_NONE_MATCH: 'Non ci sono programmi eventi che corrispondono alla tua ricerca', // DeepL
  CE_NONE: 'Nessuna entità trovata',
  CE_ALL: 'Tutto', // DeepL
  CE_STORAGE: 'Archiviazione', // DeepL
  CE_SOURCE: 'Fonte', // DeepL
  CE_EMS_SOURCE: 'dispositivo {device} · telegramma {type} · offset {offset}',
  CE_RAM_TEXT: 'mantenuto fino al riavvio',
  CE_NVS_TEXT: 'mantenuti anche dopo il riavvio', // DeepL
  CE_EXCLUDED: 'non in API/MQTT',
  SEARCH_SENSORS: 'Cerca per nome, ID o GPIO', // DeepL
  NO_SENSORS: 'Nessun sensore rilevato', // DeepL
  NO_SENSORS_MATCH: 'Nessun sensore corrisponde alla tua ricerca', // DeepL
  TEMP_SENSORS_HELP: 'Rilevati automaticamente. Clicca su un sensore per rinominarlo o impostare un offset', // DeepL
  FREE_GPIOS: 'GPIO liberi',
  SYSTEM_SENSOR: 'Sensore di sistema',
  MASK_FAVORITE: 'Preferiti', // DeepL
  MASK_READONLY: 'Scrivi "disabilitato"', // DeepL
  MASK_NO_API: 'Niente MQTT/API',
  MASK_HIDDEN: 'Nascosto',
  MASK_REMOVED: 'Rimosso',
  FILTER_ALL: 'Tutto', // DeepL
  NUM_SELECTED: 'Selezionato: {num}', // DeepL
  ALL_SHOWN: 'Tutti visualizzati: {num}', // DeepL
  BULK_HINT: 'I pulsanti modificano le righe selezionate oppure, se nessuna è selezionata, visualizzano tutte le righe.', // DeepL
  CLEAR_SELECTION: 'Selezione chiara', // DeepL
  NO_ENTITIES_MATCH: 'Nessuna entità corrispondente', // DeepL
  LIMIT_HELP: 'lascia vuoto per il limite predefinito', // DeepL
  MIN_ABOVE_MAX: 'Il valore minimo non deve superare quello massimo', // DeepL
  RENAME_DEVICE: 'Rinomina Dispositivo',
  WEEKLY: 'Settimanale',
  EVERY_DAY: 'Ogni giorno',
  SCHEDULER_NO_COMMANDS: 'Aggiungi prima un comando nella pagina Comandi',
  DU_SETTINGS: 'Impostazioni di EMS-ESP', // DeepL
  DU_SETTINGS_TEXT: 'Backup delle impostazioni di EMS-ESP: rete, MQTT, personalizzazioni, Programma eventi, entità e Module personalizzati. Caricare un file di impostazioni o di backup riavvia EMS-ESP.', // DeepL
  HB_TITLE: 'Impostazioni del riscaldamento', // DeepL
  HB_TEXT: 'Salva i valori attuali della tua caldaia, del termostato e degli altri dispositivi EMS, così potrai ripristinarli in seguito, ad esempio dopo aver sostituito un dispositivo. Le entità e i comandi di sola lettura non vengono mai modificati da un ripristino.', // DeepL
  HB_BACKUP: 'Backup Impostazioni di Riscaldamento', // DeepL
  HB_RESTORE: 'Ripristina le impostazioni di riscaldamento', // DeepL
  DATA_EXPORT: 'Esportazione dei dati', // DeepL
  DATA_EXPORT_TEXT: 'Per assistenza e analisi. Questi file non possono essere ricaricati.', // DeepL
  NUM_VALUES: '{num} valor{{e|i}}',
  HB_NOTHING: 'niente da salvare',
  FILE_NAME: 'Nome del file', // DeepL
  HB_NOTE: 'Nota (facoltativa, verrà visualizzata nuovamente al ripristino)', // DeepL
  HB_INCOMPLETE: 'I valori che non sono ancora stati letti dal bus vengono tralasciati. Aspetta qualche minuto dopo un riavvio per ottenere un backup completo.', // DeepL
  HB_RESTORE_TEXT: 'Scegli un salvataggio delle impostazioni di riscaldamento. Non verrà scritto nulla finché non avrai verificato le modifiche e confermato.', // DeepL
  HB_DROP: 'Clicca per selezionare o trascina qui un file di backup del riscaldamento (.json)', // DeepL
  HB_INVALID: 'Questo non è un backup delle impostazioni di riscaldamento',
  HB_OTHER_FILE: 'Scegli un altro file',
  HB_SAME_DEVICE: 'stesso dispositivo',
  HB_NOT_CONNECTED: 'non connesso', // DeepL
  HB_OTHER_DEVICE: 'Dispositivo diverso (ID prodotto {backup}in modalità backup, {now} collegato), saltato', // DeepL
  HB_UNIT_MISMATCH: 'Questo backup contiene le temperature in {unit}. Modifica l\'unità di misura della temperatura nelle impostazioni dell\'applicazione prima di ripristinarlo.', // DeepL
  HB_CHANGED: 'Modificato', // DeepL
  ALL: 'Tutto', // DeepL
  HB_NOW: 'Ora', // DeepL
  HB_BACKUP_VALUE: 'Backup',
  HB_NOT_AVAILABLE: 'non disponibile',
  HB_SENDING: 'invio',
  HB_WRITTEN: 'scritto',
  HB_RESTORE_NUM: 'Ripristina {num} valor{{e|i}}',
  RESTORE: 'Ripristina',
  HB_CONFIRM: 'Scrivere {num} valor{{e|i}} da {file} nei dispositivi EMS?',
  HB_CONFIRM_TEXT: 'I valori vengono inviati uno alla volta. Se un dispositivo ne rifiuta uno, gli altri vengono comunque inviati.',
  HB_DONE: '{num} {{valore scritto|valori scritti}}',
  HB_DONE_FAILED: '{num} ha dato errore. Spesso i Dispositivi rifiutano i valori impostati a livello di installatore; nel registro di sistema trovi i dettagli.', // DeepL
  HB_NO_CHANGES: 'Non c\'è alcuna differenza rispetto al backup', // DeepL
  HB_DIFFERENT_DEVICE: 'Dispositivo diverso', // DeepL
  SELECT_ALL: 'Seleziona tutto',
  DASHBOARD_EMPTY: 'Non c\'è ancora nulla da mostrare', // DeepL
  NO_DEVICES: 'Non sono stati ancora trovati dispositivi EMS', // DeepL
  ADD_COMMAND: 'Aggiungi comando',
  EDIT_COMMAND: 'Modifica comando',
  RUN_COMMAND: 'Esegui comando',
  ADD_SCHEDULE: 'Aggiungi programma',
  EDIT_SCHEDULE: 'Modifica programma',
  ADD_ENTITY: 'Aggiungi entità',
  EDIT_ENTITY: 'Modifica Entità',
  ADD_ANALOG_SENSOR: 'Aggiungi sensore analogico',
  EDIT_ANALOG_SENSOR: 'Modifica sensore analogico',
  REMOVE_ANALOG_SENSOR: 'Rimuovi Sensore Analogico', // DeepL
  REMOVE_SENSOR_TEXT: 'Il sensore «{name}» viene rimosso immediatamente.', // DeepL
  EDIT_TEMP_SENSOR: 'Modifica Sensore di Temperatura',
  INSTALL_FIRMWARE: 'Installa Firmware ({channel})', // DeepL
  PARTITION: 'Partizione',
  PULSE: 'Pulse', // DeepL
  SYS_HOME: 'Casa', // draft, please review
  SYS_HOME_HELP: 'Le cose più importanti a colpo d’occhio. Tutto il resto è nel menu Sistema.', // draft, please review
  SYS_MENU: 'Menu Sistema', // draft, please review
  SYS_MENU_HELP: 'Tutti i valori e le impostazioni del tuo impianto, ordinati per argomento.', // draft, please review
  SYS_FAV_HELP: 'Le tue impostazioni più importanti. Usa la stella accanto a un valore per modificare l’elenco.', // draft, please review
  SYS_FAV_NONE: 'Ancora nessun preferito. Contrassegna i valori con la stella.', // draft, please review
  SYS_GROUP_HEATING: 'Riscaldamento', // draft, please review
  SYS_GROUP_PLANT: 'Impianto', // draft, please review
  SYS_GROUP_OVERVIEW: 'Panoramica', // draft, please review
  SYS_GROUP_TOOLS: 'Sensori e automazione', // draft, please review
  SYS_LEVEL_MOVE_ALL: 'Mostra solo in tutti i valori', // draft, please review
  SYS_LEVEL_MOVE_NORMAL: 'Mostra nella vista normale', // draft, please review
  SYS_HC: 'Circuito di riscaldamento {0}', // draft, please review
  SYS_DHW: 'Acqua calda', // draft, please review
  SYS_AHS: 'Riscaldatore ausiliario', // draft, please review
  SYS_HS: 'Fonte di calore', // draft, please review
  SYS_SRC: 'Sorgente', // draft, please review
  SYS_OTHER_DEVICES: 'Altri dispositivi', // draft, please review
  SYS_SERVICE: 'Manutenzione e guasti', // draft, please review
  SYS_ENERGY: 'Energia e tempi di funzionamento', // draft, please review
  SYS_SEC_MODE: 'Funzionamento', // draft, please review
  SYS_SEC_TEMPS: 'Temperature', // draft, please review
  SYS_SEC_STATUS: 'Stato', // draft, please review
  SYS_SEC_ADVANCED: 'Curva di riscaldamento e installazione', // draft, please review
  SYS_SEC_MEASURE: 'Valori misurati', // draft, please review
  SYS_SEC_OTHER: 'Altri valori', // draft, please review
  SYS_LEVEL_NORMAL: 'Normale', // draft, please review
  SYS_NO_RESULTS: 'Nessun risultato. Prova con tutti i valori o un altro termine.', // draft, please review
  SYS_NOTHING_HERE: 'Nessun elemento da mostrare qui al livello Normale.', // draft, please review
  SYS_HIDDEN: 'Valori esperti nascosti: {num}', // draft, please review
  SYS_SEARCH_RESULTS: 'Risultati della ricerca', // draft, please review
  SYS_CHANGED_WAS: 'modificato (prima: {0})', // draft, please review
  SYS_NO_DEVICES: 'Nessun dispositivo trovato.', // draft, please review
  SYS_ROOM_TEMP: 'Temperatura ambiente', // draft, please review
  SYS_WISH: 'Obiettivo', // draft, please review
  SYS_CURRENT: 'Temperatura attuale', // draft, please review
  SYS_OUTDOOR: 'Temperatura esterna', // draft, please review
  SYS_FLOW: 'Mandata', // draft, please review
  SYS_RETURN: 'Ritorno', // draft, please review
  SYS_PRESSURE: 'Pressione dell’impianto', // draft, please review
  SYS_POWER: 'Potenza', // draft, please review
  SYS_BURNER_ON: 'Bruciatore in funzione', // draft, please review
  SYS_BURNER_OFF: 'Bruciatore spento', // draft, please review
  SYS_DHW_HEATING: 'In riscaldamento', // draft, please review
  SYS_READY: 'Pronto', // draft, please review
  SYS_ONE_TIME: 'Carica singola', // draft, please review
  SYS_NOTES: 'Note', // draft, please review
  SYS_NOTES_SUB: 'Guasti e manutenzione', // draft, please review
  SYS_LAST_FAULT: 'Ultimo guasto', // draft, please review
  SYS_MAINT_MSG: 'Messaggio di manutenzione', // draft, please review
  SYS_NEXT_MAINT: 'Prossima manutenzione', // draft, please review
  SYS_FAULT_LAST: 'Ultimo guasto / messaggio', // draft, please review
  SYS_FAULT_CTRL: 'Ultimo guasto del regolatore', // draft, please review
  SYS_FAULT_SERVICE: 'Codice di servizio', // draft, please review
  SYS_FAULT_ADD: 'Codice aggiuntivo {0}', // draft, please review
  SYS_FAULT_RAW_ONLY: 'Nessuna spiegazione disponibile per questo dispositivo.', // draft, please review
  SYS_FAULT_NONE: 'Nessun guasto memorizzato.', // draft, please review
  SYS_FAULT_UNTIL_NOW: 'fino ad ora', // draft, please review
  SYS_MAINTENANCE: 'Manutenzione', // draft, please review
};

export default it;
