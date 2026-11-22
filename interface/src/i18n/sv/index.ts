import type { Translation } from '../i18n-types';

const sv: Translation = {
  LANGUAGE: 'Språk',
  RETRY: 'Försök igen',
  IS_REQUIRED: '{0} är obligatoriskt', // DeepL
  SIGN_IN: 'Logga in',
  SIGN_IN_SUBTITLE: 'Gateway för EMS-värmesystem',
  SIGN_OUT: 'Logga ut',
  USERNAME: 'Användarnamn',
  PASSWORD: 'Lösenord',
  APPLICATION_SETTINGS: 'Applikationsinställningar',
  AP_SETTINGS: 'Åtkomstpunktsinställningar',
  NETWORK_SETTINGS: 'Nätverksinställningar',
  NTP_SETTINGS: 'NTP-inställningar',
  MQTT_SETTINGS: 'MQTT-inställningar',
  EMS_BUS_SETTINGS: 'EMS-bussinställningar',
  HELP: 'Hjälp',
  LOGGED_IN: 'Inloggad som {name}',
  PLEASE_SIGNIN: 'Logga in för att fortsätta', // DeepL
  UPLOAD_SUCCESSFUL: 'Uppladdningen lyckades', // DeepL
  DOWNLOAD_SUCCESSFUL: 'Ladda ner framgångsrikt', // DeepL
  INVALID_LOGIN: 'Felaktigt användarnamn eller lösenord', // DeepL
  NETWORK: 'Nätverk',
  SECURITY: 'Säkerhet',
  ONOFF_CAP: 'PÅ/AV',
  ONOFF: 'på/av',
  TYPE: 'Typ',
  ENTITIES: 'entiteter',
  EXPORT: 'Exportera',
  FAVORITES: "Favoriter",
  CLIENT_ID: 'Klient-ID',
  DEVICE_ID: 'Enhets-ID',
  PRODUCT_ID: 'Produkt-ID',
  ENTITY_ID: 'Entitets-ID',
  SENSOR_ID: 'Sensor-ID',
  TYPE_ID: 'Typ-ID',
  DEVICE: 'Enhet',
  BRAND: 'Varumärke', // DeepL
  VERSION: 'Version',
  ENTITY_NAME: 'Entitetsnamn',
  VALUE: 'Värde',
  DEVICES: 'Enheter',
  SENSORS: 'Sensorer',
  AUTOMATION: 'Automatisering',
  CONNECTIVITY: 'Anslutning',
  MAINTENANCE: 'Underhåll',
  CHANGE_VALUE: 'Ändra värde',
  CANCEL: 'Avbryt',
  REMOVE_ALL: 'Ta Bort Alla', // DeepL
  APPLY_CHANGES: 'Verkställ ändringar ({0})', // DeepL
  UPDATE: 'Uppdatera',
  REMOVE: 'Ta bort',
  RESET: 'Återställ',
  PROBLEM_UPDATING: 'Problem med uppdatering', // DeepL
  PROBLEM_LOADING: 'Problem med inläsningen', // DeepL
  ANALOG_SENSORS: 'Analoga sensorer',
  SETTINGS: 'Inställningar',
  SETTINGS_UPDATED: 'Inställningar uppdaterade',
  NAME_UPDATED: 'Namn uppdaterat',
  SENSOR_UPDATED: 'Sensor uppdaterad',
  ANALOG_SENSOR_UPDATED: 'Analog sensor uppdaterad',
  OFFSET: 'Offset',
  FACTOR: 'Faktor',
  FREQ: 'Frekvens',
  DUTY_CYCLE: 'Driftcykel', // DeepL
  UNIT: 'UoM', // DeepL
  STARTVALUE: 'Startvärde',
  EDIT: 'Redigera', // DeepL
  TEMP_SENSORS: 'Temperatursensorer',
  WRITE_CMD_SENT: 'Skriv kommando skickat', // DeepL
  EMS_BUS_WARNING: 'EMS-bussen är frånkopplad. Om denna varning kvarstår efter några sekunder, kontrollera inställningarna och kortprofilen', // DeepL
  CONNECTED: 'Ansluten',
  TX_ISSUES: 'Sändningsproblem – kontrollera sändningsläget', // DeepL
  DISCONNECTED: 'Frånkopplad', // DeepL
  DATA_TRAFFIC: 'Datatrafik',
  EMS_DEVICE: 'EMS-enhet', // DeepL
  SUCCESS: 'FRAMGÅNG', // DeepL
  FAIL: 'FEL', // DeepL
  QUALITY: 'KVALITET', // DeepL
  STATUS_NAMES: [
    'EMS-telegram (Rx)',
    'EMS-läsningar (Tx)',
    'EMS-skrivningar (Tx)',
    'Temperatursensor-läsningar',
    'Analog Sensor-läsningar',
    'MQTT-publiceringar',
    'API-anrop',
    'Syslog-meddelanden'
  ],
  NUM_DAYS: '{num} dag{{ar}}',
  NUM_SECONDS: '{num} sekund{{er}}',
  NUM_HOURS: '{num} timmar',
  NUM_MINUTES: '{num} minut{{er}}',
  APPLICATION: 'Applikation',
  CUSTOMIZATIONS: 'Anpassningar',
  APPLICATION_RESTARTING: 'EMS-ESP startar om',
  BOARD_PROFILE: 'Kortprofil', // DeepL
  CUSTOM: 'Anpassad', // DeepL
  GPIO_OF: '{0} GPIO',
  BUTTON: 'Knapp',
  TEMPERATURE: 'Temperatur',
  PHY_TYPE: 'Eth PHY-typ',
  DISABLED: 'inaktiverad',
  TX_MODE: 'EMS Tx-läge',
  EMS_BUS: 'EMS-Buss',
  EMS_BUS_ID: 'EMS-Buss-ID',
  HARDWARE: 'Hårdvara',
  LANGUAGE_ENTITIES: 'Språk (för enhetsentiteter)', // DeepL
  HIDE_LED: 'Dölj LED', // DeepL
  ENABLE_TELNET: 'Aktivera Telnet-konsolen', // DeepL
  ENABLE_ANALOG: 'Aktivera analoga sensorer', // DeepL
  CONVERT_FAHRENHEIT: 'Omvandla temperaturvärden till Fahrenheit', // DeepL
  READONLY: 'Aktivera skrivskyddat läge (blockerar alla utgående EMS Tx-skrivkommandon)', // DeepL
  WRITE_PROTECTION: 'Skydda EMS-enheterna mot upprepade och frekventa skrivoperationer', // DeepL
  WRITE_LIMIT: 'Max antal skrivningar per minut (0 = ingen gräns)',
  WRITE_LIMIT_REFUSE: 'Avvisa skrivningar som överskrider gränsen (annars loggas endast en varning)', // DeepL
  UNDERCLOCK_CPU: 'Sänk CPU-hastigheten', // DeepL
  REMOTE_TIMEOUT: 'Tidsgräns för fjärranslutning', // DeepL
  REMOTE_TIMEOUT_EN: 'Inaktivera fjärrstyrning vid saknad rumstemperatur', // DeepL
  HEATINGOFF: 'Starta värmepannan med tvångsuppvärmning avstängd', // DeepL
  MIN_DURATION: 'Väntetid',
  ENABLE_SHOWER_TIMER: 'Aktivera dusch-timern', // DeepL
  ENABLE_SHOWER_ALERT: 'Aktivera duschvarning', // DeepL
  TRIGGER_TIME: 'Utlösningstid', // DeepL
  COLD_SHOT_DURATION: 'Varaktighet för kallstart', // DeepL
  FORMATTING_OPTIONS: 'Formateringsalternativ',
  BOOLEAN_FORMAT_DASHBOARD: 'Webb i booleskt format', // DeepL
  BOOLEAN_FORMAT_API: 'Booleskt format API/MQTT', // DeepL
  ENUM_FORMAT: 'Enum-format API/MQTT',
  INDEX: 'Index',
  ENABLE_PARASITE: 'Aktivera 1-Wire-parasitström', // DeepL
  LOG_HEX: 'Logga EMS-telegram i hexadecimal form', // DeepL
  ENABLE_SYSLOG: 'Aktivera Syslog',
  LOG_LEVEL: 'Logg nivå', // DeepL
  MARK_INTERVAL: 'Markera intervall', // DeepL
  SECONDS: 'sekunder',
  MINUTES: 'minuter',
  HOURS: 'timmar',
  RESTART: 'Starta om',
  RESTART_TEXT: 'EMS-ESP måste startas om för att ändrade systeminställningar ska träda i kraft', // DeepL
  RESTART_CONFIRM: 'Är du säker på att du vill starta om EMS-ESP?',
  COMMAND: 'Kommando',
  CUSTOMIZATIONS_RESTART: 'Alla anpassningar har tagits bort. Startar om…', // DeepL
  CUSTOMIZATIONS_FULL: 'Antalet valda entiteter överskrider gränsen. Spara i omgångar', // DeepL
  CUSTOMIZATIONS_SAVED: 'Anpassningar uppdaterade', // DeepL
  CUSTOMIZATIONS_HELP_1: 'Välj en enhet och anpassa inställningarna för entiteterna', // DeepL
  CUSTOMIZATIONS_HELP_2: 'markera som favorit', // DeepL
  CUSTOMIZATIONS_HELP_3: 'inaktivera skrivåtgärd', // DeepL
  CUSTOMIZATIONS_HELP_4: 'uteslut från MQTT och API', // DeepL
  CUSTOMIZATIONS_HELP_5: 'dölj från Enheter', // DeepL
  CUSTOMIZATIONS_HELP_6: 'ta bort från minnet', // DeepL
  SELECT_DEVICE: 'Välj en enhet',
  MODIFIED: 'Ändrat', // DeepL
  RESET_SELECTED: 'Återställ valda', // DeepL
  OPTIONS: 'Alternativ',
  NAME: 'Namn',
  CUSTOMIZATIONS_RESET: 'Är du säker på att du vill ta bort alla anpassningar?',
  SYSTEM_INFORMATION: 'Systeminformation',
  HELP_INFORMATION_1: 'Besök online-wikin för att få instruktioner om hur du konfigurerar EMS-ESP', // DeepL
  HELP_INFORMATION_2: 'För livechatt med communityn, gå med i vår Discord-server', // DeepL
  HELP_INFORMATION_3: 'För att begära en funktion eller rapportera ett fel', // DeepL
  HELP_INFORMATION_4: 'Ange din systeminformation när du rapporterar ett problem för att få ett snabbare svar', // DeepL
  HELP_INFORMATION_5: 'EMS-ESP utvecklas av frivilliga. Ett tips via Ko-fi hjälper projektet att fortsätta',
  UPLOAD: 'Ladda upp', // DeepL
  DOWNLOAD: 'Nedladdning',
  INSTALL: 'Installera',
  REINSTALL: 'Installera om',
  REINSTALL_CONFIRM: 'Är du säker på att du vill installera om version {0}?',
  FAILED: 'misslyckades', // DeepL
  SYSTEM: 'System',
  SYSTEM_LOG: 'Systemlogg',
  STATUS: 'Status',
  DOWNLOAD_UPLOAD: 'Ladda ner/Ladda upp', // DeepL
  CLOSE: 'Stäng',
  COMMAND_GUEST: 'Gäster kan köra detta kommando', // DeepL
  TOKENS_RESET: 'Denna uppgradering signerar åtkomsttoken med en ny enhetsnyckel, vilket innebär att befintliga API-åtkomsttoken inte längre fungerar. Skapa en API-nyckel för varje integration som använder API:et, till exempel Home Assistant, och använd den istället för det gamla tokenet.', // DeepL
  API_KEYS: 'API-nycklar',
  API_KEYS_TEXT: 'API-nycklar gör det möjligt för integrationer som Home Assistant och skript att använda API:et. Skicka nyckeln i rubriken ”Authorization: Bearer <NYCKEL>”. Läsnycklar kan läsa av värden och köra kommandon som gäster kan köra, medan läs- och skrivnycklar kan göra allt som en administratör kan göra via API:et.', // DeepL
  API_KEY_ACCESS: 'Åtkomst',
  API_KEY_READ: 'Läs', // DeepL
  API_KEY_READ_WRITE: 'Läs och Skriv', // DeepL
  API_KEY_SHOW_ONCE: 'Kopiera den här nyckeln nu och förvara den på ett säkert ställe. Den kommer inte att visas igen.', // DeepL
  CREATED: 'Skapad',
  COPY: 'Text', // DeepL
  COPIED: 'Kopierat', // DeepL
  USE: 'Använd',
  FACTORY_RESET: 'Fabriksåterställning', // DeepL
  SYSTEM_FACTORY_TEXT_DIALOG: 'Är du säker på att du vill återställa EMS-ESP till fabriksinställningarna?', // DeepL
  STABLE: 'Stabil',
  DEVELOPMENT: 'Utveckling',
  UPTIME: 'Systemets drifttid', // DeepL
  LAST_RESTART: 'Senaste omstart',
  POWER_AND_RESET: 'Ström och återställning', // DeepL
  FREE_MEMORY: 'Ledigt minne',
  BUFFER_SIZE: 'Maximal buffertstorlek', // DeepL
  COMPACT: 'Kortfattat', // DeepL
  SUMMARY: 'Sammanfattning', // DeepL
  SHOW_MORE: 'Visa {n} fler', // DeepL
  SHOW_LESS: 'Visa mindre', // DeepL
  COREDUMP: 'Kraschdump',
  DOWNLOAD_SYSTEM_BACKUP: 'Systembackup', // DeepL
  ERROR: 'Oväntat fel, försök igen', // DeepL
  TIME_SET: 'Tidsinställning', // DeepL
  MANAGE_USERS: 'Hantera användare', // DeepL
  USER_WARNING: 'Du måste ha minst en administratörsanvändare konfigurerad', // DeepL
  ADD: 'Lägg till',
  TELNET_ENABLED: 'Telnet-konsolen är aktiverad. Telnet är inte krypterat, så stäng av det i programinställningarna om du inte använder det.', // DeepL
  TOO_MANY_LOGINS: 'För många misslyckade inloggningsförsök, försök igen om några minuter', // DeepL
  PASSWORD_KEEP: 'Lämna fältet tomt för att behålla det nuvarande lösenordet', // DeepL
  SETUP_TITLE: 'Ställ in dina lösenord', // DeepL
  SETUP_TEXT: 'Denna enhet använder fortfarande fabriksinställda lösenord. Ställ in ditt eget lösenord innan du fortsätter.', // DeepL
  SETUP_ADMIN_PASSWORD: 'Nytt lösenord för {name}',
  SETUP_AP_PASSWORD: 'Nytt lösenord för åtkomstpunkt', // DeepL
  SETUP_AP_REQUIRED: 'Inget Wi-Fi-nätverk har konfigurerats ännu, så åtkomstpunkten är ingången. 8–63 tecken, används nästa gång åtkomstpunkten startas.', // DeepL
  SETUP_AP_OPTIONAL: 'Rekommenderat: 8–63 tecken, används nästa gång åtkomstpunkten startas. Lämna fältet tomt för att behålla den nuvarande inställningen.', // DeepL
  SETUP_AP_DEFAULT: 'Åtkomstpunkten använder fortfarande fabriksinställt lösenord. Ställ in ditt eget i inställningarna för åtkomstpunkten.', // DeepL
  SETUP_VIEW_USER: 'Valfritt: lägg till en gästanvändare med endast läsbehörighet.', // DeepL
  NETWORK_CORS_ORIGIN_HELP: 'Webbadressen som används för att anropa API:et från en webbläsare, till exempel http://homeassistant.local:8123', // DeepL
  NETWORK_CORS_WILDCARD: 'Med * kan vilken webbplats som helst som är öppen i en webbläsare i ditt nätverk anropa API:et. Ange istället den exakta adressen.', // DeepL
  NETWORK_ALLOWED_HOSTS: 'Tillåtna värdnamn',
  NETWORK_ALLOWED_HOSTS_HELP: 'Alternativa namn som enheten kan öppnas med i en webbläsare, till exempel ems-ESP.fritz.box eller ett omvänd proxynamn. Dess IP-adress, värdnamn och värdnamn.local fungerar alltid', // DeepL
  NOT_ENABLED: 'Inte aktiverat', // DeepL
  MQTT_ERRORS: 'MQTT-fel',
  DISCONNECT_REASON: 'Orsak till frånkoppling', // DeepL
  MQTT_BASE_TOPIC: 'Grundläggande ämne', // DeepL
  LAST_DISCONNECT: 'Senaste frånkoppling', // DeepL
  CONNECTED_SINCE: 'Ansluten sedan',
  ENABLE_MQTT: 'Aktivera MQTT',
  BROKER: 'Broker',
  BASE_TOPIC: 'Grundläggande', // DeepL
  OPTIONAL: 'valfritt',
  MQTT_FORMAT: 'Ämne/innehållsformat', // DeepL
  MQTT_NEST_1: 'Inbäddat i ett enda ämne', // DeepL
  MQTT_NEST_2: 'Som enskilda ämnen', // DeepL
  MQTT_RESPONSE: 'Publicera kommandoutdata till ett `response`-ämne', // DeepL
  MQTT_PUBLISH_TEXT_1: 'Publicera ämnen med ett enda värde vid förändring', // DeepL
  MQTT_PUBLISH_TEXT_2: 'Publicera till kommandoämnen (ioBroker)', // DeepL
  MQTT_PUBLISH_TEXT_3: 'Aktivera MQTT-upptäckt', // DeepL
  MQTT_PUBLISH_TEXT_4: 'Prefix för upptäcktsämnena', // DeepL
  MQTT_PUBLISH_TEXT_5: 'Typ av upptäckt', // DeepL
  MQTT_INPUT_NUMBER_FORMAT: 'Format för inmatning av tal', // DeepL
  MQTT_PUBLISH_INTERVALS: 'Publiceringsintervall',
  MQTT_INT_BOILER: 'Värmepannor och värmepumpar', // DeepL
  MQTT_INT_THERMOSTATS: 'Termostater',
  MQTT_INT_SOLAR: 'Solcellsmoduler', // DeepL
  MQTT_INT_MIXER: 'Mixermoduler', // DeepL
  MQTT_INT_WATER: 'Vattenmoduler', // DeepL
  MQTT_QUEUE: 'MQTT-kö',
  DEFAULT: 'Standard',
  MQTT_ENTITY_FORMAT: 'Format för entitets-ID', // DeepL
  MQTT_ENTITY_FORMAT_0: 'Enstaka förekomst, långt namn (v3.4)', // DeepL
  MQTT_ENTITY_FORMAT_1: 'Enstaka förekomst, kort namn', // DeepL
  MQTT_ENTITY_FORMAT_2: 'Flera instanser, kort namn', // DeepL
  MQTT_CLEAN_SESSION: 'Ställ in ren session', // DeepL
  MQTT_RETAIN_FLAG: 'Ställ alltid in flaggan för att behålla', // DeepL
  MQTT_COMMANDS: 'Kommandon som tas emot via MQTT',
  MQTT_COMMANDS_0: 'Av, endast läsvärden', // DeepL
  MQTT_COMMANDS_1: 'Ändra värden och utför kommandon', // DeepL
  MQTT_COMMANDS_2: 'Allt, även systemkommandon',
  INACTIVE: 'Inaktiv',
  ACTIVE: 'Aktiv',
  UNKNOWN: 'Okänt',
  SET_TIME: 'Ställ In Tid', // DeepL
  SET_TIME_TEXT: 'Ange lokalt datum och tid nedan för att ställa in tiden', // DeepL
  LOCAL_TIME: 'Lokal tid', // DeepL
  UTC_TIME: 'UTC-tid', // DeepL
  ENABLE_NTP: 'Aktivera NTP',
  NTP_SERVER: 'NTP-server',
  TIME_ZONE: 'Tidszon',
  ACCESS_POINT: 'Åtkomstpunkt', // DeepL
  AP_PROVIDE: 'Aktivera åtkomstpunkt', // DeepL
  AP_PROVIDE_TEXT_2: 'När nätverksanslutning inte hittas', // DeepL
  AP_PROVIDE_TEXT_3: 'Aldrig', // DeepL
  AP_PREFERRED_CHANNEL: 'Föredragen kanal', // DeepL
  AP_HIDE_SSID: 'Dölj SSID', // DeepL
  AP_CLIENTS: 'AP-klienter',
  AP_MAX_CLIENTS: 'Max antal klienter',
  AP_LOCAL_IP: 'Lokal IP', // DeepL
  IDLE: 'Inaktiv', // DeepL
  SCAN_AGAIN: 'Skanna igen', // DeepL
  NETWORK_NO_WIFI: 'Inga Wi-Fi-nätverk hittades', // DeepL
  NETWORK_BLANK_BSSID: 'Lämna fältet tomt för att endast använda SSID', // DeepL
  TX_POWER: 'Sändningseffekt', // DeepL
  HOSTNAME: 'Värdnamn',
  NETWORK_DISABLE_SLEEP: 'Inaktivera Wi-Fi-viloläge', // DeepL
  NETWORK_LOW_BAND: 'Använd lägre Wi-Fi-bandbredd', // DeepL
  NETWORK_USE_DNS: 'Aktivera mDNS-tjänsten',
  NETWORK_ENABLE_CORS: 'Aktivera CORS',
  NETWORK_CORS_ORIGIN: 'CORS-ursprung', // DeepL
  NETWORK_FIXED_IP: 'Använd en fast IP-adress', // DeepL
  NETWORK_GATEWAY: 'Gateway',
  NETWORK_SUBNET: 'Subnätmask',
  NETWORK_DNS: 'DNS-servrar', // DeepL
  ADDRESS_OF: '{0}-adress', // DeepL
  ADMINISTRATOR: 'Administratör',
  GUEST: 'Gäst',
  NEW: 'Ny', // DeepL
  NEW_ENTITY_NAME: 'Nytt entitetsnamn',
  MIN: 'min',
  MAX: 'max',
  BLOCK_NAVIGATE_1: 'Du har osparade ändringar',
  BLOCK_NAVIGATE_2: 'Om du navigerar till en annan sida kommer dina osparade ändringar att gå förlorade. Är du säker på att du vill lämna denna sida?', // DeepL
  UPLOAD_ABORTED: 'Uppladdningen avbröts',
  BLOCK_BUSY_1: 'Arbetet pågår fortfarande', // DeepL
  BLOCK_BUSY_2: 'Om du lämnar den här sidan nu avbryts processen. Är du säker på att du vill lämna sidan?', // DeepL
  DISCARD: 'Kasta', // DeepL
  COMMANDS_UPDATED: 'Kommandon uppdaterade',
  V_REQUIRED: 'Obligatoriskt',
  V_BETWEEN: 'Måste ligga mellan {min} och {max}', // DeepL
  V_MAX_CHARS: 'Måste vara {max} tecken eller färre', // DeepL
  V_CHARS: 'Måste vara {min}–{max} tecken',
  V_IN_USE: 'Används redan',
  V_IP: 'Måste vara en IP-adress',
  V_HOSTNAME: 'Måste vara ett giltigt värdnamn',
  V_IP_OR_HOSTNAME: 'Måste vara en IP-adress eller ett värdnamn',
  V_NAME: 'Upp till {max} tecken: bokstäver, siffror eller _', // DeepL
  V_USERNAME: 'Upp till {max} tecken: bokstäver, siffror, _ eller .', // DeepL
  V_DAY: 'Välj minst en dag',
  V_HEX: 'Måste vara ett hexadecimalt tal',
  V_FINGERPRINT: 'Måste vara ett SHA-1-fingeravtryck (40 hexadecimala siffror)',
  V_EXAMPLE: 'Ska se ut som {example}', // DeepL
  V_NOT_DEFAULT: 'Välj ett annat lösenord',
  ZERO_DISABLES: '0 stänger av den', // DeepL
  MINUS_ONE_DISABLES: '-1 stänger av den', // DeepL
  LED_TYPE: 'LED-typ',
  PHY_POWER: 'PHY-strömförsörjning', // DeepL
  PHY_CLOCK: 'PHY-klocka',
  AUTO: 'Auto',
  PORT: 'Port',
  TIMEOUT: 'Timeout', // DeepL
  HOST: 'Värd',
  SMTP_SERVER: 'SMTP-server',
  KEEP_ALIVE: 'Håll liv i', // DeepL
  MQTT_REFRESH: 'Uppdatera MQTT',
  MQTT_REFRESHED: 'MQTT uppdaterat',
  MQTT_R_USER: 'Frånkopplat av användaren', // DeepL
  MQTT_R_TCP: 'TCP-anslutningen har stängts', // DeepL
  MQTT_R_PROTOCOL: 'Protokollversionen accepteras inte',
  MQTT_R_ID: 'Kund-ID avvisat', // DeepL
  MQTT_R_SERVER: 'Servern är inte tillgänglig',
  MQTT_R_CREDENTIALS: 'Felaktiga inloggningsuppgifter',
  MQTT_R_AUTH: 'Ej auktoriserad', // DeepL
  MQTT_R_FINGERPRINT: 'TLS-fingeravtrycket stämmer inte överens', // DeepL
  GPIO_INVALID: 'Ogiltiga GPIO-inställningar',
  CONNECTED_VIA: 'Ansluten via {0}',
  NETWORK_NOT_FOUND: 'Nätverket hittades inte',
  CONNECTION_FAILED: 'Anslutningen misslyckades',
  CONNECTION_LOST: 'Anslutningen har brutits', // DeepL
  AP_LINGERING: 'Väntar tills systemet är inaktivt', // DeepL
  NONE: 'Inga', // DeepL
  LOG_NO_CONNECTION: 'Ingen anslutning till loggtjänsten',
  CHECK_SYSTEM_LOG: 'Kontrollera systemloggen för möjliga orsaker', // DeepL
  VALUE_TYPE: 'Värdetyp',
  DEFAULT_VALUE: 'Standardvärde',
  DEFAULT_NAME: 'Standardnamn',
  ALWAYS_OFF: 'Alltid av',
  ALWAYS_ON: 'Alltid på',
  NO_DATA: 'Inga favoriter ännu. Markera dem i {customizations}, eller se alla entiteter i {devices}.', // DeepL
  STAY: 'Håll dig', // DeepL
  LEAVE: 'Lämna',
  MENU: 'Meny',
  MORE: 'Mer',
  SHOW_PASSWORD: 'Visa lösenord',
  STATUS_RUNNING: 'EMS-ESP körs',
  NUM_ERRORS: '{num} fel',
  NUM_WARNINGS: '{num} varning{{ar}}',
  HEAP: 'Heap',
  METER_USED: '{used} / {total} {unit} förbrukad', // DeepL
  MAX_BLOCK: '{num} KB max per block', // DeepL
  MAX_BLOCK_HELP: 'Max block är den största mängd minne som kan allokeras på en gång. Under 40 KB kan webbgränssnittet och MQTT sluta fungera.', // DeepL
  FLASH_CHIP: 'Flash-chip', // DeepL
  FILE_SYSTEM: 'Filsystem',
  DETAILS: 'Detaljer',
  RECONNECTS: 'Återansluter', // DeepL
  CONNECTS: 'Ansluter', // DeepL
  ACTIVITY_HELP: 'Några få Rx-fel är normalt. Många Tx-fel innebär oftast att fel Tx-läge har valts för din värmepanna.', // DeepL
  PAUSE: 'Paus', // DeepL
  RESUME: 'CV', // DeepL
  LOG_PAUSED: 'Pausat, nya rader hålls tillbaka: {num}', // DeepL
  FILTER_LINES: 'Filterrader', // DeepL
  SCHEDULER: 'Schemaläggning',
  SCHEDULER_HELP_1: 'Automatisera kommandon genom att lägga till schemalagda händelser nedan', // DeepL
  SCHEDULER_HELP_2: 'vid uppstart',
  SCHEDULE: 'Schema', // DeepL
  TIME: 'Tid',
  TIMER: 'Timer',
  ONCHANGE: 'Vid ändring', // DeepL
  CONDITION: 'Villkor',
  SCHEDULE_UPDATED: 'Schemat har uppdaterats', // DeepL
  CUSTOM_ENTITIES: 'Anpassade enheter', // DeepL
  ENTITIES_HELP_1: 'Definiera anpassade EMS-entiteter eller dynamiska användarvariabler', // DeepL
  ENTITIES_UPDATED: 'Uppdaterade entiteter', // DeepL
  WRITEABLE: 'Skrivbar', // DeepL
  SHOWING: 'Visar',
  SEARCH: 'Sök',
  CERT: 'CA-certifikat (PEM)',
  ENABLE_TLS: 'Aktivera TLS',
  TLS_CHECK: 'Kontrollera mäklaren med', // DeepL
  TLS_CHECK_CA: 'CA-certifikat',
  TLS_CHECK_FINGERPRINT: 'Certifikatets fingeravtryck (SHA-1)',
  TLS_CHECK_NONE: 'Inget, inte markerat', // DeepL
  TLS_NOT_CHECKED: 'Anslutningen är krypterad, men mäklaren kontrolleras inte, vilket innebär att en annan enhet i nätverket skulle kunna utge sig för att vara den.', // DeepL
  ON: 'På',
  OFF: 'Av',
  POLARITY: 'Polaritet',
  ACTIVEHIGH: 'Aktiv hög', // DeepL
  ACTIVELOW: 'Aktiv låg', // DeepL
  UNCHANGED: 'Oförändrat', // DeepL
  SECURITY_1: 'Lägg till eller ta bort användare',
  DOWNLOAD_UPLOAD_1: 'Ladda ner och ladda upp inställningar', // DeepL
  MODULES: 'Moduler',
  MODULES_1: 'Aktivera eller inaktivera externa moduler', // DeepL
  NETWORK_DESC: 'Wi-Fi, Ethernet och värdnamn',
  AP_DESC: 'Egen hotspot för installation och reservåtkomst',
  MQTT_DESC: 'Broker, ämnen och discovery',
  APPLICATION_DESC: 'EMS-buss, kort, sensorer och formatering',
  NTP_DESC: 'Tidsserver och tidszon',
  FIRMWARE_DESC: 'Version, uppdateringar och återställning',
  MODULES_UPDATED: 'Moduler uppdaterade', // DeepL
  MODULES_NONE: 'Inga externa moduler har upptäckts', // DeepL
  RENAME: 'Byt namn',
  ENABLE_MODBUS: 'Aktivera Modbus',
  VIEW_LOG: 'Visa loggen för att diagnostisera problem', // DeepL
  SERVICES: 'Tjänster',
  ALLVALUES: 'Alla värden',
  WAIT_FIRMWARE: 'Firmware laddas upp och installeras',
  INSTALL_VERSION: 'Detta installerar version {0}',
  UPDATE_AVAILABLE: 'uppdatering tillgänglig',
  LATEST_VERSION: 'Du använder den senaste firmwareversionen ({0})',
  PLEASE_WAIT: 'Vänta', // DeepL
  RESTARTING_PRE: 'Uppstart', // DeepL
  RESTARTING_POST: 'Förberedelser', // DeepL
  AUTO_SCROLL: 'Automatisk rullning', // DeepL
  DASHBOARD: 'Kontrollpanel',
  DEVELOPER_MODE: 'Utvecklingsläge', // DeepL
  DISABLE_RESET: 'Inaktivera fabriksåterställning',
  AUTO_FW_CHECK: 'Sök automatiskt efter uppdateringar av firmware', // DeepL
  BYTES: 'Bytes',
  BITMASK: 'Bitmask',
  DUPLICATE: 'Duplikat', // DeepL
  DASHBOARD_1: 'Alla aktiva EMS-entiteter som är markerade som Favoriter visas nedan, tillsammans med alla anpassade entiteter, scheman och externa sensorer.', // DeepL
  NO_GPIO: 'Ingen tillgänglig GPIO hittades', // DeepL
  PLATFORM: 'Plattform',
  RELEASE_TYPE: 'Utgåvetyp', // DeepL
  FIRMWARE_VERSION_INFO: 'Firmwareversionsinformation',
  USER_PROFILE: 'Användarprofil',
  ONLINE_HELP: 'onlinehjälp', // DeepL
  UPGRADE_IMPORTANT_MESSAGES: 'Viktiga Uppdateringsmeddelanden', // DeepL
  UPGRADE_IMPORTANT_MESSAGES_2: 'Du uppgraderar till en ny huvudversion. Det rekommenderas att du först gör en säkerhetskopia av systemet. Kontrollera även onlinehjälpen för eventuella avgörande ändringar.', // DeepL
  UPGRADE_IMPORTANT_MESSAGES_3: 'Version 3.10 innehåller säkerhetsändringar som medför bakåtkompatibilitetsproblem. Gör först en säkerhetskopia av systemet: efter uppgraderingen lagras lösenorden i hashform och äldre versioner kan inte läsa dem, så för att återgå till en tidigare version krävs denna säkerhetskopia. Varje API-anrop kräver då en API-nyckel, så integrationer som Home Assistant måste uppdateras. Läs om de bakåtkompatibilitetsproblem som medförs i onlinehjälpen innan du fortsätter.', // DeepL
  WARNING_SYSTEM_BACKUP: 'Detta skapar en säkerhetskopia av hela din systemkonfiguration och dina inställningar. Lösenord och nycklar utelämnas om du inte anger dem nedan; när säkerhetskopian återställs behålls de lösenord som redan finns på enheten. Kommandon inkluderas alltid och kan innehålla inloggningsuppgifter, så var försiktig med att dela dem. Vill du fortsätta?', // DeepL
  BACKUP_INCLUDE_SECRETS: 'Inkludera lösenord och nycklar (som behövs för att återställa till en ny enhet)', // DeepL
  TEST_EMAIL_SUCCESSFUL: 'Testmejl skickat', // DeepL
  SYSTEM_NAME: 'Systemnamn',
  COMMANDS: 'Kommandon',
  COMMANDS_HELP_1: 'Definiera återanvändbara namngivna kommandon nedan. Dessa kan köras från konsolen, via API/MQTT eller refereras till under schemaläggning', // DeepL
  COMMAND_EXECUTED: 'Kommandon har utförts', // DeepL
  RUN: 'Kör', // DeepL
  INTERNET_CONNECTION_CHECK: 'Kontrollera tillgängliga versioner online', // DeepL
  UPLOAD_BACKUP_TEXT: 'Klicka för att välja, eller släpp en säkerhetskopia (.json) här', // DeepL
  UPLOAD_FIRMWARE_TEXT: 'Klicka för att välja, eller släpp en firmwarefil (.bin/.md5) här', // DeepL
  UPLOAD_MD5_RECEIVED: 'Sammanfattning mottagen, ladda nu upp firmwarefilen (.bin)', // DeepL
  UPLOAD_MD5_MATCHED: 'MD5-matchning, installation av firmware…', // DeepL
  UPLOAD_NOT_OFFICIAL: 'Detta är inte en officiell version av EMS-ESP. Installera endast firmware som du litar på', // DeepL
  UPLOAD_NOT_OFFICIAL_HELP: 'Om officiella och anpassade versioner', // DeepL
  UPLOAD_INSTALL_ANYWAY: 'Installera ändå',
  OFFICIAL_BUILD: 'Officiell Version',
  CUSTOM_BUILD: 'Anpassad Version', // DeepL
  USING_CUSTOM_FIRMWARE: 'Du använder en anpassad firmware', // DeepL
  PASSWORD_CONFIRM_TITLE: 'Ange ditt lösenord igen',
  PASSWORD_CONFIRM_TEXT: 'För din säkerhet, ange ditt lösenord innan denna ändring. Du kommer inte att bli ombedd att ange det igen på 5 minuter.', // DeepL
  PASSWORD_CONFIRM_BUTTON: 'Bekräfta',
  PASSWORD_WRONG: 'Felaktigt lösenord', // DeepL
  PASSWORD_NOT_CONFIRMED: 'Avbrutet, lösenordet angavs inte',
  RECOVERY_APP: 'Återställningsapp',
  RECOVERY_NOT_INSTALLED: 'Ej installerad', // DeepL
  RECOVERY_INSTALLED: 'Återställningsfirmware installerad',
  RESTART_INTO_RECOVERY: 'Starta om i återställningsappen',
  RECOVERY_NOT_FOUND: 'Återställningsappen hittades inte online',
  RECOVERY_START_FAILED: 'Recovery-appen går inte att starta; den kan vara föråldrad eller skadad. Installera först den senaste versionen', // DeepL
  PARTITION_START_FAILED: 'Denna firmware kan inte startas, den kan vara skadad', // DeepL
  INSTALLED_VERSIONS: 'Installerade versioner',
  EMSESP_APP: 'EMS-ESP-appen', // DeepL
  NO_UPDATES_AVAILABLE: 'inga uppdateringar tillgängliga',
  SWITCH_TO_DEV: 'Byt till dev {0}',
  SWITCH_TO_STABLE: 'Byt till den stabila versionen {0}', // DeepL
  SAVE: 'Spara',
  NEEDS_RESTART: 'måste startas om', // DeepL
  CHANGED_SETTINGS: '{num} {{ändrad inställning|ändrade inställningar}}',
  SAVED_RESTART: 'Sparat. Starta om EMS-ESP för att använda de nya inställningarna.',
  RESTART_NOW: 'Starta om nu',
  COPY_FAILED: 'Det gick inte att kopiera, markera texten och kopiera den själv', // DeepL
  DOCUMENTATION: 'Dokumentation',
  COMMUNITY_CHAT: 'Community-chatt', // DeepL
  REPORT_ISSUE: 'Rapportera ett fel eller begär en funktion', // DeepL
  SUPPORT_PROJECT: 'Stöd projektet',
  GET_HELP: 'Få hjälp',
  REPORT_PROBLEM: 'Rapportera ett problem',
  REPORT_STEP_1: 'Ladda ner systeminformationen',
  REPORT_STEP_1_TEXT: 'version, kretskort, EMS-bussstatus, anslutna enheter och inställningar, utan lösenord', // DeepL
  REPORT_STEP_2: 'Spara den senaste loggen',
  REPORT_STEP_2_TEXT: 'Ställ först in loggnivån på DEBUG om problemet gäller en enhet', // DeepL
  REPORT_STEP_3: 'Skapa ett ärende eller ställ en fråga på Discord', // DeepL
  REPORT_STEP_3_TEXT: 'Bifoga båda filerna, eller klistra in den korta sammanfattningen nedan i chatten', // DeepL
  NEW_ISSUE: 'Nytt ärende',
  SHORT_SUMMARY: 'Kort sammanfattning',
  LICENSE_TEXT: 'Öppen källkod under LGPL-3.0-licensen', // DeepL
  ADMIN_ACCESS_TEXT: 'fullständig åtkomst till inställningar, enheter och API:et', // DeepL
  GUEST_ACCESS_TEXT: 'kan visa värden och utföra de kommandon som gäster kan utföra', // DeepL
  LANGUAGE_BROWSER_ONLY: 'gäller endast denna webbläsare', // DeepL
  LANGUAGE_ENTITIES_HINT: 'Språket för entitetsnamn i MQTT och API:et anges i', // DeepL
  ADMINISTRATION: 'Administration',
  API_KEYS_SHORT: 'tangenter för Home Assistant och skript', // DeepL
  ADDRESSES: 'Adresser',
  SEARCH_SETTINGS: 'Sökinställningar', // DeepL
  NO_SETTINGS_MATCH: 'Inga inställningar matchar din sökning', // DeepL
  GENERAL: 'Allmänt',
  BOARD: 'Styrelse', // DeepL
  ADVANCED: 'Avancerat',
  EMS_BUS_DESC: 'Så här kommunicerar EMS-ESP med ditt värmesystem.', // DeepL
  BOARD_DESC: 'Stiften hämtas från kortprofilen. För att ställa in dem själv, aktivera utvecklarläget och välj Anpassad.', // DeepL
  FORMAT_DESC: 'Hur värdena ser ut i webbgränssnittet och i MQTT/API-payloads.', // DeepL
  BOARD_PINS: 'Pinnar från profilen', // DeepL
  ENABLE_EMAIL: 'Aktivera e-postaviseringar',
  SEND_TEST_EMAIL: 'Skicka testmejl',
  EMAIL_FROM: 'Från',
  EMAIL_TO: 'Till',
  EMAIL_SUBJECT: 'Ämne',
  WIFI_SCAN: 'Wi-Fi-skanning', // DeepL
  SCAN: 'Skanna', // DeepL
  NETWORKS_NEARBY: 'Nätverk i närheten',
  SIGNAL: 'Signal',
  CHANNEL: 'Kanal',
  WIFI_OPEN: 'öppen',
  WIFI_SAVED: 'sparat', // DeepL
  NETWORK_ETHERNET_INFO: 'Ansluten via Ethernet just nu ({ip}). Wi-Fi används endast utan kabel.', // DeepL
  HOSTNAME_ACCESS: 'Värdnamn och åtkomst',
  IP_ADDRESS: 'IP-adress',
  NETWORK_ETH_10MBIT: 'Tvinga 10 Mbit halvduplex', // DeepL
  NETWORK_ETH_10MBIT_HELP: 'endast för felaktiga strömbrytare eller långa kablar', // DeepL
  NETWORK_DISABLE_SLEEP_HELP: 'mer tillförlitlig, förbrukar lite mer ström', // DeepL
  NETWORK_MDNS_HELP: 'Nås via {host}.local med mDNS', // DeepL
  NETWORK_FIXED_IP_HELP: 'i annat fall används DHCP', // DeepL
  NETWORK_CORS_HELP: 'behövs endast när en annan webbsida anropar API:et', // DeepL
  NTP_TIME_ON_DEVICE: 'tid på EMS-ESP',
  NTP_TIME_SERVER: 'Tidsserver',
  NTP_THERMOSTAT_CLOCK: 'Termostatklocka', // DeepL
  NTP_THERMOSTAT_SYNC: 'Ställ in termostatens klocka', // DeepL
  NTP_THERMOSTAT_SYNC_HELP: 'ställer in datum och tid på EMS-termostaten', // DeepL
  NTP_THERMOSTAT_SYNC_1: 'På, EMS-ESP-tidszon', // DeepL
  NTP_THERMOSTAT_SYNC_2: 'På, egen tidszon',
  NTP_THERMOSTAT_TZ: 'Termostatens tidszon',
  MQTT_PREVIEW: 'Vad som publiceras',
  MQTT_PREVIEW_CMD: 'godkänns som ett kommando', // DeepL
  MQTT_PREVIEW_RESPONSE: 'utdata från kommandon',
  MQTT_BEHAVIOUR: 'Beteende',
  MQTT_TOPICS: 'Ämnen och nyttolaster', // DeepL
  MQTT_DISCOVERY: 'Upptäckt', // DeepL
  MQTT_INTERVALS_HELP: 'Sekunder mellan publiceringar, 0 = publicera vid varje ändring.',
  MQTT_SINGLE_NO_DISCOVERY: 'inte tillgängligt med upptäckt på', // DeepL
  MQTT_DISCOVERY_NO_SINGLE: 'finns inte tillgängligt med ämnen med ett enda värde på', // DeepL
  MQTT_DISCOVERY_HELP: 'skapar entiteterna i Home Assistant eller Domoticz automatiskt', // DeepL
  MQTT_ENTITY_FORMAT_WARN: 'Om du ändrar detta kommer alla entiteter i Home Assistant att byta namn', // DeepL
  USERS: 'Användare',
  USERS_TEXT: 'Administratörer kan ändra allt. Gäster kan titta och utföra kommandon som är markerade för gäster.', // DeepL
  ADD_USER: 'Lägg till användare',
  EDIT_USER: 'Redigera användare',
  ROLE: 'Roll',
  ROLE_ADMIN: 'admin', // DeepL
  ROLE_GUEST: 'gäst',
  ADMIN: 'Admin', // DeepL
  YOU: 'du',
  NEW_PASSWORD: 'Nytt lösenord',
  UNSAVED_CHANGES: '{num} {{osparad ändring|osparade ändringar}}',
  CREATE_KEY: 'Skapa Nyckel',
  NEW_API_KEY: 'Ny API-nyckel',
  NO_API_KEYS: 'Inga API-nycklar ännu',
  API_KEYS_LIMIT: 'Du kan ha högst {num}-tangenter.', // DeepL
  REVOKE: 'Återkalla',
  REVOKE_KEY: 'Återkalla API-nyckel',
  REVOKE_KEY_TEXT: 'Allt som använder ”{name}” slutar fungera omedelbart.', // DeepL
  MODULES_TEXT: 'Valfria funktioner som är inbyggda i denna firmware. Aktivera eller inaktivera dem här.', // DeepL
  BY_AUTHOR: 'av {name}',
  LICENSE_KEY: 'Licensnyckel',
  MODULE_ACTIVATED: 'aktiverad',
  MODULE_PENDING: 'väntar på aktivering',
  ENABLED: 'Aktiverad',
  UNDO: 'Ångra',
  FW_UPDATES: 'Uppdateringar',
  FW_VERSION_AVAILABLE: '{version} är tillgänglig',
  FW_RELEASE_NOTES: 'Releaseanteckningar', // DeepL
  FW_UPDATES_OFFLINE: 'Det gick inte att söka efter nya versioner online. EMS-ESP behöver en internetanslutning för detta.', // DeepL
  FW_RESTART_RESET: 'Omstart och Återställning',
  FW_RECOVERY_TEXT: 'Ska du starta om i återställningsappen? Den ansluter till ditt nätverk och startar även en egen åtkomstpunkt, där du kan ladda upp firmware.', // DeepL
  FW_SIZE: 'Storlek',
  FW_INSTALL_DATE: 'Installerad på', // DeepL
  FW_UPLOAD_FAILED: 'Uppladdningen misslyckades: fel filtyp eller firmware för ett annat kort',
  FILE_TYPE_INVALID: 'Den här filtypen stöds inte, välj en {types}-fil',
  WHO_CAN_RUN: 'Vem kan köra', // DeepL
  CMD_NONE: 'Inga kommandon hittades',
  CMD_NONE_MATCH: 'Inga kommandon matchar din sökning', // DeepL
  SCHEDULE_NONE: 'Inga scheman hittades',
  SCHEDULE_NONE_MATCH: 'Inga scheman matchar din sökning', // DeepL
  CE_NONE: 'Inga entiteter hittades',
  CE_ALL: 'Alla',
  CE_STORAGE: 'Lagring',
  CE_SOURCE: 'Källa',
  CE_EMS_SOURCE: 'enhet {device} · telegram {type} · offset {offset}',
  CE_RAM_TEXT: 'bevaras tills omstart', // DeepL
  CE_NVS_TEXT: 'bevaras vid omstart', // DeepL
  CE_EXCLUDED: 'inte i API/MQTT',
  SEARCH_SENSORS: 'Sök på namn, ID eller GPIO', // DeepL
  NO_SENSORS: 'Inga sensorer hittades',
  NO_SENSORS_MATCH: 'Inga sensorer matchar din sökning', // DeepL
  TEMP_SENSORS_HELP: 'Hittas automatiskt. Klicka på en Sensor för att byta namn på den eller ställa in en förskjutning', // DeepL
  FREE_GPIOS: 'Lediga GPIO-portar', // DeepL
  SYSTEM_SENSOR: 'Systemsensor',
  MASK_FAVORITE: 'Favorit',
  MASK_READONLY: 'Skriv funktionshindrade', // DeepL
  MASK_NO_API: 'Ingen MQTT/API',
  MASK_HIDDEN: 'Dold',
  MASK_REMOVED: 'Borttagen',
  FILTER_ALL: 'Alla',
  NUM_SELECTED: 'Valt: {num}', // DeepL
  ALL_SHOWN: 'Alla visas: {num}', // DeepL
  BULK_HINT: 'Knapparna ändrar de markerade raderna, eller alla rader som visas när ingen är markerad.', // DeepL
  CLEAR_SELECTION: 'Tydligt val', // DeepL
  NO_ENTITIES_MATCH: 'Inga entiteter matchar',
  LIMIT_HELP: 'Lämna tomt för det inbyggda gränsvärdet', // DeepL
  MIN_ABOVE_MAX: 'Minimivärdet får inte överstiga maximivärdet', // DeepL
  RENAME_DEVICE: 'Byt Namn på Enheten', // DeepL
  WEEKLY: 'Veckovis',
  EVERY_DAY: 'Varje dag',
  SCHEDULER_NO_COMMANDS: 'Lägg först till ett kommando på sidan Kommandon',
  DU_SETTINGS: 'EMS-ESP-inställningar',
  DU_SETTINGS_TEXT: 'Säkerhetskopiering av EMS-ESP-inställningarna: nätverk, MQTT, anpassningar, Schemaläggning, anpassade entiteter och moduler. När du laddar upp en inställnings- eller säkerhetskopieringsfil startas EMS-ESP om.', // DeepL
  HB_TITLE: 'Värmeinställningar',
  HB_TEXT: 'Sparar de aktuella värdena för din värmepanna, termostat och andra EMS-enheter, så att du kan återställa dem senare, till exempel efter att du bytt ut en enhet. Skrivskyddade entiteter och kommandon ändras aldrig vid en återställning.', // DeepL
  HB_BACKUP: 'Säkerhetskopiera Värmeinställningarna', // DeepL
  HB_RESTORE: 'Återställ värmeinställningar',
  DATA_EXPORT: 'Dataexport',
  DATA_EXPORT_TEXT: 'För support och analys. Dessa filer kan inte laddas upp igen.',
  NUM_VALUES: '{num} {{värde|värden}}',
  HB_NOTHING: 'inget att säkerhetskopiera',
  FILE_NAME: 'Filnamn',
  HB_NOTE: 'Anmärkning (valfritt, visas igen när du återställer)', // DeepL
  HB_INCOMPLETE: 'Värden som ännu inte har lästs av från bussen utelämnas. Vänta några minuter efter en omstart för att få en fullständig säkerhetskopia.', // DeepL
  HB_RESTORE_TEXT: 'Välj en säkerhetskopia för värmeinställningarna. Ingenting skrivs förrän du har granskat ändringarna och bekräftat dem.', // DeepL
  HB_DROP: 'Klicka för att välja, eller släpp en reservfil för uppvärmningen (.json) här', // DeepL
  HB_INVALID: 'Detta är inte en säkerhetskopia av värmeinställningarna', // DeepL
  HB_OTHER_FILE: 'Välj en annan fil',
  HB_SAME_DEVICE: 'samma enhet',
  HB_NOT_CONNECTED: 'inte ansluten',
  HB_OTHER_DEVICE: 'annan enhet (produkt-ID {backup} i reservsystemet, {now} ansluten), hoppades över', // DeepL
  HB_UNIT_MISMATCH: 'Denna säkerhetskopia innehåller temperaturer i {unit}. Ändra temperaturenheten i applikationsinställningarna innan du återställer den.', // DeepL
  HB_CHANGED: 'Ändrat', // DeepL
  ALL: 'Alla',
  HB_NOW: 'Nu',
  HB_BACKUP_VALUE: 'Säkerhetskopia',
  HB_NOT_AVAILABLE: 'ej tillgängligt', // DeepL
  HB_SENDING: 'sändning', // DeepL
  HB_WRITTEN: 'skrivet',
  HB_RESTORE_NUM: 'Återställ {num} {{värde|värden}}',
  RESTORE: 'Återställ',
  HB_CONFIRM: 'Skriva {num} {{värde|värden}} från {file} till EMS-enheterna?',
  HB_CONFIRM_TEXT: 'Värdena skickas ett i taget. Om en enhet avvisar ett värde skickas resten ändå.', // DeepL
  HB_DONE: '{num} {{värde skrivet|värden skrivna}}',
  HB_DONE_FAILED: '{num} misslyckades. Enheterna accepterar ofta inte värden på installatörsnivå; detaljerna finns i systemloggen.', // DeepL
  HB_NO_CHANGES: 'Inget skiljer sig från säkerhetskopian',
  HB_DIFFERENT_DEVICE: 'olika enheter', // DeepL
  SELECT_ALL: 'Markera allt', // DeepL
  DASHBOARD_EMPTY: 'Inget att visa ännu',
  NO_DEVICES: 'Inga EMS-enheter har hittats ännu', // DeepL
  ADD_COMMAND: 'Lägg till kommando',
  EDIT_COMMAND: 'Redigeringskommando', // DeepL
  RUN_COMMAND: 'Kör kommando',
  ADD_SCHEDULE: 'Lägg till schema',
  EDIT_SCHEDULE: 'Redigera schema',
  ADD_ENTITY: 'Lägg till enhet', // DeepL
  EDIT_ENTITY: 'Redigera Entitet', // DeepL
  ADD_ANALOG_SENSOR: 'Lägg till analog sensor',
  EDIT_ANALOG_SENSOR: 'Redigera analoga sensorer', // DeepL
  REMOVE_ANALOG_SENSOR: 'Ta Bort Analog Sensor',
  REMOVE_SENSOR_TEXT: 'Sensor ”{name}” tas bort direkt.', // DeepL
  EDIT_TEMP_SENSOR: 'Redigera Temperaturgivare', // DeepL
  INSTALL_FIRMWARE: 'Installera Firmware ({channel})',
  PARTITION: 'Avsnitt', // DeepL
  PULSE: 'Pulse', // DeepL
};

export default sv;
