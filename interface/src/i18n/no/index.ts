import type { Translation } from '../i18n-types';

const no: Translation = {
  LANGUAGE: 'Språk',
  RETRY: 'Forsøk igjen',
  LOADING: 'Laster',
  IS_REQUIRED: '{0} er nødvendig',
  SIGN_IN: 'Logg inn',
  SIGN_OUT: 'Logg ut',
  USERNAME: 'Brukernavn',
  PASSWORD: 'Passord',
  SU_PASSWORD: 'su Passord',
  SETTINGS_OF: '{0} Innstillinger',
  HELP: 'Hjelp',
  LOGGED_IN: 'Logget in som {name}',
  PLEASE_SIGNIN: 'Venligst logge inn for å fortsetta',
  UPLOAD_SUCCESSFUL: 'Opplasting lykkes',
  DOWNLOAD_SUCCESSFUL: 'Nedlasting lykkes',
  INVALID_LOGIN: 'Ugyldig innlogging',
  NETWORK: 'Nettverk',
  SECURITY: 'Sikkerhet',
  ONOFF_CAP: 'PÅ/AV',
  ONOFF: 'på/av',
  TYPE: 'Type',
  DESCRIPTION: 'Beskrivelse',
  ENTITIES: 'Ojekter',
  REFRESH: 'Oppdater',
  EXPORT: 'Eksport',
  FAVORITES: "Favoritter",
  DEVICE_DETAILS: 'Enhetsdetaljer',
  ID_OF: '{0}-ID',
  DEVICE: 'Enhets',
  PRODUCT: 'Produkt',
  VERSION: 'Versjon',
  BRAND: 'Fabrikat',
  ENTITY_NAME: 'Objektsnavn',
  VALUE: '{{verdi|Verdi}}',
  DEVICES: 'Enheter',
  SENSORS: 'Sensorer',
  RUN_COMMAND: 'Kjør kommando',
  CHANGE_VALUE: 'Endre Verdi',
  CANCEL: 'Avbryt',
  RESET: 'Nullstill',
  APPLY_CHANGES: 'Utfør endringer({0})',
  UPDATE: 'Oppdater',
  EXECUTE: 'Execute', // TODO translate
  REMOVE: 'Fjern',
  PROBLEM_UPDATING: 'Problem med oppdatering',
  PROBLEM_LOADING: 'Problem med opplasting',
  ANALOG_SENSOR: 'Analog Sensor',
  ANALOG_SENSORS: 'Analoge Sensorer',
  SETTINGS: 'Innstillinger',
  UPDATED_OF: '{0} Oppdatert',
  UPDATE_OF: '{0} Oppdater',
  REMOVED_OF: '{0} Slettet',
  DELETION_OF: '{0} Sletting',
  OFFSET: 'Kompensering',
  FACTOR: 'Faktor',
  FREQ: 'Frekvens',
  DUTY_CYCLE: 'Duty Cycle',
  UNIT: 'UoM',
  STARTVALUE: 'Startverdi',
  WARN_GPIO: 'Advarsel: vær forsiktig ved aktivering av GPIO!',
  EDIT: 'Endre',
  SENSOR: 'Sensor',
  TEMP_SENSOR: 'Temperatursensor',
  TEMP_SENSORS: 'Temperaturesensorer',
  WRITE_CMD_SENT: 'Skriv kommando sent',
  EMS_BUS_WARNING: 'EMS bussen koblet ned. Hvis denne advarselen fortsetter etter noen f¨sekunder sjekk instillinger og  prosessorkort',
  EMS_BUS_SCANNING: 'Søker etter EMS enheter...',
  CONNECTED: 'Tilkoblet',
  TX_ISSUES: 'Tx problemer - prøv en annen Tx Modus',
  DISCONNECTED: 'Frakoblet',
  EMS_SCAN: 'Er du sikker på du vil starte full søking av EMS bussen?',
  DATA_TRAFFIC: 'Data Traffic', // TODO translate
  EMS_DEVICE: 'EMS Enhet',
  SUCCESS: 'VELLYKKET',
  FAIL: 'MISLYKKET',
  QUALITY: 'KVALITET',
  SCAN: 'Søk',
  STATUS_NAMES: [
    'EMS Telegrammer Mottatt (Rx)',
    'EMS Lest (Tx)',
    'EMS Skrevet (Tx)',
    'Temperatur Sensor Lest',
    'Analog Sensor Lest',
    'MQTT Publiseringer',
    'API Anrop',
    'Syslog Meldinger'
  ],
  NUM_DAYS: '{num} Dag{{er}}',
  NUM_SECONDS: '{num} Sekund{{er}}',
  NUM_HOURS: '{num} Time{{r}}',
  NUM_MINUTES: '{num} Minutt{{er}}',
  APPLICATION: 'Søknad',
  CUSTOMIZATIONS: 'Tilpasninger',
  APPLICATION_RESTARTING: 'EMS-ESP restarter',
  BOARD_PROFILE: 'Prosessor Profil',
  CUSTOM: 'Custom',
  GPIO_OF: '{0} GPIO',
  BUTTON: 'Knapp',
  TEMPERATURE: 'Temperatur',
  PHY_TYPE: 'Eth PHY Type',
  DISABLED: 'avslått',
  TX_MODE: 'EMS Tx Mode',
  HARDWARE: 'Hardware',
  EMS_BUS: '{{BUS|EMS BUS}}',
  GENERAL_OPTIONS: 'Generelle Innstillinger',
  LANGUAGE_ENTITIES: 'Språk (for objekter)',
  HIDE_LED: 'Skjul LED',
  ENABLE_TELNET: 'Aktiver Telnet',
  ENABLE_ANALOG: 'Aktiver Analoge Sensorer',
  CONVERT_FAHRENHEIT: 'Konverter temperatur til Fahrenheit',
  BYPASS_TOKEN: 'Utelat Aksess Token authorisering av API kall',
  READONLY: 'Aktiver read-only modus (blokker all EMS Tx Skriving)',
  UNDERCLOCK_CPU: 'Underklokking av prosessorhastighet',
  REMOTE_TIMEOUT: 'Remote timeout',
  REMOTE_TIMEOUT_EN: 'Disable remote control on missing room temperature', // TODO translate
  HEATINGOFF: 'Start boiler with forced heating off', // TODO translate
  MIN_DURATION: 'Wait time',
  ENABLE_SHOWER_TIMER: 'Aktiver Dusjtimer',
  ENABLE_SHOWER_ALERT: 'Aktiver Dusj-varsling',
  TRIGGER_TIME: 'Aktiveringstid',
  COLD_SHOT_DURATION: 'Tid på kaldt vann',
  FORMATTING_OPTIONS: 'Formatteringsalternativs',
  BOOLEAN_FORMAT_DASHBOARD: 'Bool Format Web',
  BOOLEAN_FORMAT_API: 'Bool Format API/MQTT',
  ENUM_FORMAT: 'Enum Format API/MQTT',
  INDEX: 'Indeks',
  ENABLE_PARASITE: 'Aktiver 1-wire parasitt strømforsyning',
  LOGGING: 'Logging',
  LOG_HEX: 'Logg EMS telegrammer i hexadesimal',
  ENABLE_SYSLOG: 'Aktiver Syslog',
  LOG_LEVEL: 'Log Level',
  MARK_INTERVAL: 'Oppdateringsintervall',
  SECONDS: 'sekunder',
  MINUTES: 'minutter',
  HOURS: 'timer',
  RESTART: 'Omstart',
  RESTART_TEXT: 'EMS-ESP må omstartes for å iverksette endrede systeminstillinger',
  RESTART_CONFIRM: 'Er du sikker på at du vil omstarte EMS-ESP?',
  COMMAND: 'Kommando',
  CUSTOMIZATIONS_RESTART: 'Alle tilpasninger har blitt slettet. Restarter...',
  CUSTOMIZATIONS_FULL: 'Antall valgte objekter for høyt. Largre i mindre antall om gangen',
  CUSTOMIZATIONS_SAVED: 'Tilpasninger lagret',
  CUSTOMIZATIONS_HELP_1: 'Velg en enhet og tilpass underenheter med hjelp av alternativer eller velg å gi nytt navn',
  CUSTOMIZATIONS_HELP_2: 'merk som favoritt',
  CUSTOMIZATIONS_HELP_3: 'inaktiviser skriving',
  CUSTOMIZATIONS_HELP_4: 'ekskludere fra MQTT og API',
  CUSTOMIZATIONS_HELP_5: 'skjul fra enheter',
  CUSTOMIZATIONS_HELP_6: 'fjern fra minnet',
  SELECT_DEVICE: 'Velg en enhet',
  SET_ALL: 'sett alle',
  OPTIONS: 'Alternativ',
  NAME: 'Navn',
  CUSTOMIZATIONS_RESET: 'Er du sikker på att du vil fjerne tilpassninger inkludert innstillinger for Temperatur og Analoge sensorer?',
  SUPPORT_INFORMATION: 'Supportinformasjon',
  HELP_INFORMATION_1: 'Besøk wiki for instruksjoner for å konfigurere EMS-ESP',
  HELP_INFORMATION_2: 'For community-support besøk vår Discord-server',
  HELP_INFORMATION_3: 'For å be om en ny funksjon eller melde feil',
  HELP_INFORMATION_4: 'Husk å laste ned og legg ved din systeminformasjon for en raskere respons når du rapporterer et problem',
  UPLOAD: 'Opplasning',
  DOWNLOAD: '{{N|n|n}}edlasting',
  INSTALL: 'Installer',
  ABORTED: 'avbrutt',
  FAILED: 'feilet',
  SUCCESSFUL: 'vellykket',
  SYSTEM: 'System',
  LOG_OF: '{0} Logg',
  STATUS_OF: '{0} Status',
  DOWNLOAD_UPLOAD: 'Nedlasting/Opp',
  CLOSE: 'Steng',
  USE: 'Bruk',
  FACTORY_RESET: 'Sett tilbake til fabrikkinstilling',
  SYSTEM_FACTORY_TEXT: 'Enhet har blitt satt tilbake til fabrikkinstilling og vil restarte',
  SYSTEM_FACTORY_TEXT_DIALOG: 'Er du sikker på at du vil resette enheten til fabrikkinstillinger?',
  AVAILABLE_VERSION: 'Latest Available Versions', // TODO translate
  STABLE: 'Stable', // TODO translate
  DEVELOPMENT: 'Development',
  UPTIME: 'System Oppetid',
  FREE_MEMORY: 'Ledig Memory',
  PSRAM: 'PSRAM (Størrelse / Ledig)',
  FLASH: 'Flash Chip (Størrelse , Hastighet)',
  APPSIZE: 'Applikasjon (Partition: Brukt / Ledig)',
  FILESYSTEM: 'File System (Brukt / Ledig)',
  BUFFER_SIZE: 'Max Buffer Størrelse',
  COMPACT: 'Komprimere',
  DOWNLOAD_SETTINGS_TEXT: 'Create a backup of your configuration and settings', // TODO translate
  UPLOAD_TEXT: 'Upload a new firmware file (.bin) or a backup file (.json)', // TODO translate
  UPLOAD_DROP_TEXT: 'Drop a firmware .bin file or click here', // TODO translate
  ERROR: 'Ukjent feil, prøv igjen',
  TIME_SET: 'Still in tid',
  MANAGE_USERS: 'Administrer Brukere',
  IS_ADMIN: 'er Admin',
  USER_WARNING: 'Du må ha minst en admin bruker konfigurert',
  ADD: 'Legg til',
  ACCESS_TOKEN_FOR: 'Aksess Token for',
  ACCESS_TOKEN_TEXT: 'Token nedenfor benyttes med REST API-kall som krever autorisering. Den kan sendes med enten som en Bearer token i Authorization-headern eller i access_token URL query-parameter.',
  GENERATING_TOKEN: 'Generer token',
  USER: 'Bruker',
  MODIFY: 'Endre',
  SU_TEXT: 'su brukeren (super user) passord benyttes for å signere autentiserings token samt å tillate admin privileger i konsoll modus.',
  NOT_ENABLED: 'Ikke aktiv',
  ERRORS_OF: '{0} Feil',
  DISCONNECT_REASON: 'Årsak til nedkobling',
  ENABLE_MQTT: 'Aktiver MQTT',
  BROKER: 'Broker',
  CLIENT: 'Client',
  BASE_TOPIC: 'Base',
  OPTIONAL: 'valgfritt',
  FORMATTING: 'Formatering',
  MQTT_FORMAT: 'Topic/Payload Format',
  MQTT_NEST_1: 'Nestet i en topic',
  MQTT_NEST_2: 'Som individuelle topics',
  MQTT_RESPONSE: 'Publiser kommandoer til en `response` topic',
  MQTT_PUBLISH_TEXT_1: 'Publiser singel verdi topics ved endringer',
  MQTT_PUBLISH_TEXT_2: 'Publiser til kommando topics (ioBroker)',
  MQTT_PUBLISH_TEXT_3: 'Aktiver MQTT Discovery',
  MQTT_PUBLISH_TEXT_4: 'Prefiks for Discovery topics',
  MQTT_PUBLISH_TEXT_5: 'Discovery type',
  MQTT_PUBLISH_INTERVALS: 'Publiseringsintervall',
  MQTT_INT_BOILER: 'Fyr/Varmepumpe',
  MQTT_INT_THERMOSTATS: 'Termostat',
  MQTT_INT_SOLAR: 'Solpaneler',
  MQTT_INT_MIXER: 'Blandeventil',
  MQTT_INT_WATER: 'Water Modules', // TODO translate
  MQTT_QUEUE: 'MQTT Queue',
  DEFAULT: 'Standard',
  MQTT_ENTITY_FORMAT: 'Enhets ID format',
  MQTT_ENTITY_FORMAT_0: 'Enkel instans, fullt nane (v3.4)',
  MQTT_ENTITY_FORMAT_1: 'Enkel instans, kort nave',
  MQTT_ENTITY_FORMAT_2: 'Multiple instanser, kort name',
  MQTT_CLEAN_SESSION: 'Benytt Clean Session',
  MQTT_RETAIN_FLAG: 'Alltid sett Retain flag',
  INACTIVE: 'Innaktiv',
  ACTIVE: 'Aktiv',
  UNKNOWN: 'Ukjent',
  SET_TIME: 'Sett Tid',
  SET_TIME_TEXT: 'Skriv inn dato og klokke nedenfor',
  LOCAL_TIME: 'Lokaltid',
  UTC_TIME: 'UTC Tid',
  ENABLE_NTP: 'Aktiver NTP',
  NTP_SERVER: 'NTP Server',
  TIME_ZONE: 'Tidssone',
  ACCESS_POINT: 'Aksesspunkt',
  AP_PROVIDE: 'Aktiver Aksesspunkt',
  AP_PROVIDE_TEXT_1: 'alltid',
  AP_PROVIDE_TEXT_2: 'når WiFi er utilgjengelig',
  AP_PROVIDE_TEXT_3: 'aldri',
  AP_PREFERRED_CHANNEL: 'Foretrukket kanal',
  AP_HIDE_SSID: 'Skjul SSID',
  AP_CLIENTS: 'AP Clients',
  AP_MAX_CLIENTS: 'Max Clients',
  AP_LOCAL_IP: 'Local IP',
  NETWORK_SCAN: 'Søk etter trådløst nettverk',
  IDLE: 'Klar',
  LOST: 'Mistet',
  SCANNING: 'Søker',
  SCAN_AGAIN: 'Søk igjen',
  NETWORK_SCANNER: 'Nettverk Scanner',
  NETWORK_NO_WIFI: 'Ingen trådløse nett funnet',
  NETWORK_BLANK_SSID: 'la feltet være blankt for å deaktivisere trådløst nettverk',
  NETWORK_BLANK_BSSID: 'leave blank to use only SSID', // TODO translate
  TX_POWER: 'Tx Effekt',
  HOSTNAME: 'Hostname',
  NETWORK_DISABLE_SLEEP: 'Hindre at trådløst nettverk går i Sleep Mode',
  NETWORK_LOW_BAND: 'Benytt smalere båndbredde på trådløst nettverk',
  NETWORK_USE_DNS: 'Aktiviser mDNS Service',
  NETWORK_ENABLE_CORS: 'Aktiviser CORS',
  NETWORK_CORS_ORIGIN: 'CORS origin',
  NETWORK_FIXED_IP: 'Benytt statisk IP adresse',
  NETWORK_GATEWAY: 'Gateway',
  NETWORK_SUBNET: 'Nettverksmaske',
  NETWORK_DNS: 'DNS Servers',
  ADDRESS_OF: '{0} Address',
  ADMINISTRATOR: 'Administrator',
  GUEST: 'Gjest',
  NEW: 'Ny',
  NEW_NAME_OF: 'Bytt navn {0}',
  ENTITY: 'Entitet',
  MIN: 'min',
  MAX: 'max',
  BLOCK_NAVIGATE_1: 'You har ulagrede endringer',
  BLOCK_NAVIGATE_2: 'Hvis du navigerer til en annen side blir dine ikke lagrede endringer gå tapt. Are du sikker på at du vil forlate denne siden ?',
  STAY: 'Bli her',
  LEAVE: 'Forlat',
  SCHEDULER: 'Planlegger',
  SCHEDULER_HELP_1: 'Automatiser kommandoer ved å legge til skedulerte hendelser nedenfor. Sett et unikt navn for å slå på/av aktivering via API/MQTT',
  SCHEDULER_HELP_2: 'Bruk 00:00 for å kjøre en gang ved oppstart',
  SCHEDULE: 'Planlegg',
  TIME: 'Tid',
  TIMER: 'Timer',
  ONCHANGE: 'På endring',
  CONDITION: 'Betingelse',
  IMMEDIATE: 'Umiddelbar',
  SCHEDULE_UPDATED: 'Planlegger er oppdatert',
  SCHEDULE_TIMER_1: 'ved oppstart',
  SCHEDULE_TIMER_2: 'hvert minutt',
  SCHEDULE_TIMER_3: 'hver time',
  CUSTOM_ENTITIES: 'Custom Entities', // TODO translate
  ENTITIES_HELP_1: 'Fetch custom entities from the EMS bus', // TODO translate
  ENTITIES_UPDATED: 'Entities Updated', // TODO translate
  WRITEABLE: 'Writeable', // TODO translate
  SHOWING: 'Showing', // TODO translate
  SEARCH: 'Search', // TODO translate
  CERT: 'TLS root certificate (leave blank for insecure)', // TODO translate
  ENABLE_TLS: 'Aktiviser TLS',
  ON: 'On', // TODO translate
  OFF: 'Off', // TODO translate
  POLARITY: 'Polarity', // TODO translate
  ACTIVEHIGH: 'Active High', // TODO translate
  ACTIVELOW: 'Active Low', // TODO translate
  UNCHANGED: 'Unchanged', // TODO translate
  ALWAYS: 'Always', // TODO translate
  ACTIVITY: 'Activity', // TODO translate
  CONFIGURE: 'Configure {0}', // TODO translate
  SYSTEM_MEMORY: 'System Memory', // TODO translate
  APPLICATION_SETTINGS_1: 'Modify EMS-ESP Application Settings', // TODO translate
  SECURITY_1: 'Add or remove users', // TODO translate
  DOWNLOAD_UPLOAD_1: 'Download and Upload Settings and Firmware', // TODO translate
  MODULES: 'Module', // TODO translate
  MODULES_1: 'Aktiver eller deaktiver eksterne moduler',
  MODULES_UPDATED: 'Modules updated', // TODO translate
  MODULES_DESCRIPTION: 'Click on the Module to activate or de-activate EMS-ESP library modules', // TODO translate
  MODULES_NONE: 'No external modules detected', // TODO translate
  RENAME: 'Rename', // TODO translate 
  ENABLE_MODBUS: 'Aktiver Modbus',
  VIEW_LOG: 'View log to diagnose issues', // TODO translate
  UPLOAD_DRAG: 'drag and drop a file here or click to select one', // TODO translate
  SERVICES: 'Services', // TODO translate
  ALLVALUES: 'All Values', // TODO translate
  SPECIAL_FUNCTIONS: 'Special Functions', // TODO translate
  WAIT_FIRMWARE: 'Firmware is uploading and installing', // TODO translate
  INSTALL_VERSION: 'This will install version {0}. Are you sure?', // TODO translate
  UPGRADE_AVAILABLE: 'There is a firmware upgrade available!', // TODO translate
  LATEST_VERSION: 'You are using the latest firmware version', // TODO translate
  PLEASE_WAIT: 'Please wait', // TODO translate
  RESTARTING_PRE: 'Initializing', // TODO translate
  RESTARTING_POST: 'Preparing', // TODO translate
  AUTO_SCROLL: 'Auto Scroll', // TODO translate
  DASHBOARD: 'Dashboard', // TODO translate
  DEVELOPER_MODE: 'Developer Mode', // TODO translate
  BYTES: 'Bytes', // TODO translate
  BITMASK: 'Bit Mask',// TODO translate
  DUPLICATE: 'Duplicate', // TODO translate
  UPGRADE: 'Upgrade', // TODO translate
  DASHBOARD_1: 'All EMS entities that are active and marked as Favorite, plus all Custom Entities, Schedules and external Sensors data are displayed below.', // TODO translate
  NO_DATA_1: 'No favorite EMS entities found yet. Use the', // TODO translate
  NO_DATA_2: 'module to mark them.', // TODO translate
  NO_DATA_3: 'To see all available entities go to', // TODO translate
  THIS_VERSION: 'This Version', // TODO translate
  PLATFORM: 'Platform', // TODO translate
  RELEASE_TYPE: 'Release Type', // TODO translate
  REINSTALL: 'Re-install', // TODO translate
  INTERNET_CONNECTION_REQUIRED: 'Internet connection required for automatic version checking and upgrading',
};

export default no;
