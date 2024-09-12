import type { Translation } from '../i18n-types';

const sv: Translation = {
  LANGUAGE: 'Språk',
  RETRY: 'Försök igen',
  LOADING: 'Laddar',
  IS_REQUIRED: '{0} Krävs',
  SIGN_IN: 'Logga In',
  SIGN_OUT: 'Logga Ut',
  USERNAME: 'Användarnamn',
  PASSWORD: 'Lösenord',
  SU_PASSWORD: 'su Lösenord',
  SETTINGS_OF: '{0} Inställningar',
  HELP: 'Hjälp',
  LOGGED_IN: 'Inloggad som {name}',
  PLEASE_SIGNIN: 'Vänligen logga in för att fortsätta',
  UPLOAD_SUCCESSFUL: 'Uppladdning lyckades',
  DOWNLOAD_SUCCESSFUL: 'Nedladdning lyckades',
  INVALID_LOGIN: 'Ogiltig login',
  NETWORK: 'Nätverk',
  SECURITY: 'Säkerhet',
  ONOFF_CAP: 'PÅ/AV',
  ONOFF: 'på/av',
  TYPE: 'Typ',
  DESCRIPTION: 'Beskrivning',
  ENTITIES: 'entiteter',
  REFRESH: 'Uppdatera',
  EXPORT: 'Exportera',
  DEVICE_DETAILS: 'Enhetsdetaljer',
  ID_OF: '{0}-ID',
  DEVICE: 'Enhets',
  PRODUCT: 'Produkt',
  VERSION: 'Version',
  BRAND: 'Fabrikat',
  ENTITY_NAME: 'Entitetsnamn',
  VALUE: '{{värde|Värde}}',
  DEVICES: 'Enheter',
  SENSORS: 'Sensorer',
  RUN_COMMAND: 'Kör Kommando',
  CHANGE_VALUE: 'Ändra Värde',
  CANCEL: 'Avbryt',
  RESET: 'Nollställ',
  APPLY_CHANGES: 'Apply Changes ({0})', // TODO translate
  UPDATE: 'Update', // TODO translate
  EXECUTE: 'Execute', // TODO translate
  REMOVE: 'Ta bort',
  PROBLEM_UPDATING: 'Problem vid uppdatering',
  PROBLEM_LOADING: 'Problem vid hämtning',
  ANALOG_SENSOR: 'Analog Sensor',
  ANALOG_SENSORS: 'Analoga Sensorer',
  SETTINGS: 'Inställningar',
  UPDATED_OF: '{0} Uppdaterad',
  UPDATE_OF: '{0} Uppdatera',
  REMOVED_OF: '{0} Raderad',
  DELETION_OF: '{0} Radering',
  OFFSET: 'Kompensering',
  FACTOR: 'Faktor',
  FREQ: 'Frekvens',
  DUTY_CYCLE: 'Duty Cycle',
  UNIT: 'UoM',
  STARTVALUE: 'Startvärde',
  WARN_GPIO: 'Varning: Var försiktig vid aktivering av GPIO!',
  EDIT: 'Ändra',
  SENSOR: 'Sensor',
  TEMP_SENSOR: 'Temperatursensor',
  TEMP_SENSORS: 'Temperatursensorer',
  WRITE_CMD_SENT: 'Skrivkommandon skickade',
  EMS_BUS_WARNING: 'EMS-buss nedkopplad. Om denna varning kvarstår efter några sekunder, kontrollera inställningar och enhets-profil.',
  EMS_BUS_SCANNING: 'Söker efter EMS-enheter...',
  CONNECTED: 'Ansluten',
  TX_ISSUES: 'Sändfel - Prova ett annat TX-läge',
  DISCONNECTED: 'Nedkopplad',
  EMS_SCAN: 'Är du säker att du vill initiera en full genomsökning av EMS-bussen?',
  DATA_TRAFFIC: 'Data Traffic', // TODO Translate
  EMS_DEVICE: 'EMS Enhet',
  SUCCESS: 'Lyckades',
  FAIL: 'Misslyckades',
  QUALITY: 'Kvalitet',
  SCAN: 'Sök',
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
  APPLICATION: 'Apliká',
  CUSTOMIZATIONS: 'Anpassningr',
  APPLICATION_RESTARTING: 'EMS-ESP startar om',
  BOARD_PROFILE: 'Hårdvarutyp',
  CUSTOM: 'Anpassa',
  GPIO_OF: '{0} GPIO',
  BUTTON: 'Knapp',
  TEMPERATURE: 'Temperatur',
  PHY_TYPE: 'Eth PHY-typ',
  DISABLED: 'inaktiverad',
  TX_MODE: 'EMS Tx-läge',
  HARDWARE: 'Hårdvara',
  EMS_BUS: '{{BUSS|EMS-BUSS}}',
  GENERAL_OPTIONS: 'Allmänna Inställningar',
  LANGUAGE_ENTITIES: 'Språk (för entiteter)',
  HIDE_LED: 'Inaktivera LED',
  ENABLE_TELNET: 'Aktivera Telnet',
  ENABLE_ANALOG: 'Aktivera Analoga Sensorer',
  CONVERT_FAHRENHEIT: 'Konvertera temperaturer till Fahrenheit',
  BYPASS_TOKEN: 'Inaktivera Token-autensiering för API-anrop',
  READONLY: 'Aktivera read-only (blockerar alla utgående skrivkommandon mot EMS-bussen)',
  UNDERCLOCK_CPU: 'Nedklocka Processorhastighet',
  REMOTE_TIMEOUT: 'Remote timeout',
  REMOTE_TIMEOUT_EN: 'Disable remote on missing roomtemperature',
  HEATINGOFF: 'Start boiler with forced heating off', // TODO translate
  MIN_DURATION: 'Wait time',
  ENABLE_SHOWER_TIMER: 'Aktivera Dusch-timer',
  ENABLE_SHOWER_ALERT: 'Aktivera Dusch-varning',
  TRIGGER_TIME: 'Aktiveringstid',
  COLD_SHOT_DURATION: 'Längd på kalldusch',
  FORMATTING_OPTIONS: 'Formatteringsalternativ',
  BOOLEAN_FORMAT_DASHBOARD: 'Bool-format Kontrollpanel',
  BOOLEAN_FORMAT_API: 'Bool-format API/MQTT',
  ENUM_FORMAT: 'Enum-format API/MQTT',
  INDEX: 'Index',
  ENABLE_PARASITE: 'Aktivera 1-wire parasitström',
  LOGGING: 'Loggning',
  LOG_HEX: 'Logga EMS-telegram i hexadecimal',
  ENABLE_SYSLOG: 'Aktivera Syslog',
  LOG_LEVEL: 'Loggnivå',
  MARK_INTERVAL: 'Markerings-interval',
  SECONDS: 'sekunder',
  MINUTES: 'minuter',
  HOURS: 'timmar',
  RESTART: 'Starta om',
  RESTART_TEXT: 'EMS-ESP kräver en omstart för att applicera förändrade systeminställningar',
  RESTART_CONFIRM: 'Är du säker på att du vill starta om EMS-ESP?',
  COMMAND: 'Kommando',
  CUSTOMIZATIONS_RESTART: 'Alla anpassningr har raderats. Startar om...',
  CUSTOMIZATIONS_FULL: 'Antal valda enheter för högt. Vänligen spara i mindre antal åt gången.',
  CUSTOMIZATIONS_SAVED: 'Anpassningar sparade',
  CUSTOMIZATIONS_HELP_1: 'Välj en enhet och anpassa underenheter med hjälp av alternativen',
  CUSTOMIZATIONS_HELP_2: 'Favorit',
  CUSTOMIZATIONS_HELP_3: 'Inaktivera skrivningar',
  CUSTOMIZATIONS_HELP_4: 'Exkludera från MQTT & API',
  CUSTOMIZATIONS_HELP_5: 'Göm från Kontrollpanel',
  CUSTOMIZATIONS_HELP_6: 'remove from memory', // TODO translate
  SELECT_DEVICE: 'Välj en enhet',
  SET_ALL: 'ställ in alla',
  OPTIONS: 'Alternativ',
  NAME: 'Namn',
  CUSTOMIZATIONS_RESET: 'Är du säker på att du vill ta bort alla anpassningar inklusive inställningar för Temperatur och Analoga sensorer?',
  SUPPORT_INFORMATION: 'Supportinformation',
  HELP_INFORMATION_1: 'Besök Wikin för instruktioner för hur du kan konfigurera EMS-ESP',
  HELP_INFORMATION_2: 'För community-support besök vår Discord-server',
  HELP_INFORMATION_3: 'Önska en ny funktion eller rapportera en bugg',
  HELP_INFORMATION_4: 'Bifoga din systeminformation för snabbare hantering när du rapporterar ett problem',
  HELP_INFORMATION_5: 'EMS-ESP är gratis och är öppen källkod. Bidra till utvecklingen genom att ge oss en stjärna på GitHub!',
  UPLOAD: 'Uppladdning',
  DOWNLOAD: '{{N|n|n}}edladdning',
  INSTALL: 'Installera {0}',
  ABORTED: 'Avbruten',
  FAILED: 'Misslyckades',
  SUCCESSFUL: 'Lyckades',
  SYSTEM: 'System',
  LOG_OF: '{0} Logg',
  STATUS_OF: '{0} Status',
  DOWNLOAD_UPLOAD: 'Nedladdning/Upp',
  CLOSE: 'Stäng',
  USE: 'Använd',
  FACTORY_RESET: 'Fabriksåterställning',
  SYSTEM_FACTORY_TEXT: 'Enheten har blivit fabriksåterställd och startar nu om',
  SYSTEM_FACTORY_TEXT_DIALOG: 'Är du säker att du vill fabriksåterställa enheten?',
  STABLE: 'Stable', // TODO translate
  DEVELOPMENT: 'Utveckling',
  RELEASE_NOTES: 'release-logg',
  EMS_ESP_VER: 'Firmware Version', // TODO translate
  UPTIME: 'Systemets Upptid',
  FREE_MEMORY: 'Ledigt Memory',
  PSRAM: 'PSRAM (Storlek / Ledigt)',
  FLASH: 'Flashminne (Storlek , Hastighet)',
  APPSIZE: 'Applikationer (Partition: Använt / Ledigt)',
  FILESYSTEM: 'Filsystem (Använt / Ledigt)',
  BUFFER_SIZE: 'Max Bufferstorlek',
  COMPACT: 'Komprimera',
  DOWNLOAD_SETTINGS_TEXT: 'Create a backup of your configuration and settings', // TODO translate
  UPLOAD_TEXT: 'Upload a new firmware (.bin) file or a backup file (.json)', // TODO translate
  UPLOAD_DROP_TEXT: 'Släpp fil eller klicka här',
  ERROR: 'Okänt Fel, var god försök igen',
  TIME_SET: 'Ställ in tid',
  MANAGE_USERS: 'Användare',
  IS_ADMIN: 'Admin',
  USER_WARNING: 'Du måste ha minst en admin konfigurerad',
  ADD: 'Lägg till',
  ACCESS_TOKEN_FOR: 'Access Token för',
  ACCESS_TOKEN_TEXT: 'Nedan Token används med REST API-anrop som kräver auktorisering. Den kan skickas med antingen som en Bearer token i Authorization-headern eller i access_token URL query-parametern.',
  GENERATING_TOKEN: 'Genererar token',
  USER: 'Användare',
  MODIFY: 'Ändra',
  SU_TEXT: 'SU-användarens (super user) lösenord används för att signera autensierings-tokens samt för att aktivera administratörsprivilegier i Console-läge',
  NOT_ENABLED: 'Ej aktiv',
  ERRORS_OF: '{0} fel',
  DISCONNECT_REASON: 'Anledning till nedkoppling',
  ENABLE_MQTT: 'Aktivera MQTT',
  BROKER: 'Broker',
  CLIENT: 'Client',
  BASE_TOPIC: 'Base',
  OPTIONAL: 'valfritt',
  FORMATTING: 'Formatering',
  MQTT_FORMAT: 'Topic/Payload Format',
  MQTT_NEST_1: 'Nestlat i en topic.',
  MQTT_NEST_2: 'Som individuella topics',
  MQTT_RESPONSE: 'Publish-kommando som ett `response` topic',
  MQTT_PUBLISH_TEXT_1: 'Publicera single value topics vid värdeförändring',
  MQTT_PUBLISH_TEXT_2: 'Publicera till kommando-topics (ioBroker)',
  MQTT_PUBLISH_TEXT_3: 'Aktivera MQTT Discovery',
  MQTT_PUBLISH_TEXT_4: 'Prefix för  Discovery topics',
  MQTT_PUBLISH_TEXT_5: 'Discovery type', // TODO translate
  MQTT_PUBLISH_INTERVALS: 'Publiceringsintervall',
  MQTT_INT_BOILER: 'Värmepump/panna',
  MQTT_INT_THERMOSTATS: 'Termostater',
  MQTT_INT_SOLAR: 'Solpaneler',
  MQTT_INT_MIXER: 'Blandningsventiler',
  MQTT_INT_WATER: 'Water Modules', // TODO translate
  MQTT_QUEUE: 'MQTT-kö',
  DEFAULT: 'Standard',
  MQTT_ENTITY_FORMAT: 'Entitets-ID format',
  MQTT_ENTITY_FORMAT_0: 'Singel-instans, långt namn(v3.4)',
  MQTT_ENTITY_FORMAT_1: 'Singel-instans, kort name',
  MQTT_ENTITY_FORMAT_2: 'Multi-instans, kort name',
  MQTT_CLEAN_SESSION: 'Använd "Clean Session"-flaggan',
  MQTT_RETAIN_FLAG: 'Använd "Always Retain"-flaggan',
  INACTIVE: 'Inaktiv',
  ACTIVE: 'Aktiv',
  UNKNOWN: 'Okänt',
  SET_TIME: 'Ställ in klockan',
  SET_TIME_TEXT: 'Ange lokal datum och tid nedan för att ställa in klockan',
  LOCAL_TIME: 'Tid (lokal)',
  UTC_TIME: 'Tid (UTC)',
  ENABLE_NTP: 'Aktivera NTP',
  NTP_SERVER: 'NTP-server',
  TIME_ZONE: 'Tidszon',
  ACCESS_POINT: 'Accesspunkt',
  AP_PROVIDE: 'Aktivera Accesspunkt',
  AP_PROVIDE_TEXT_1: 'alltid',
  AP_PROVIDE_TEXT_2: 'när WiFi är nedkopplat',
  AP_PROVIDE_TEXT_3: 'aldrig',
  AP_PREFERRED_CHANNEL: 'Kanal',
  AP_HIDE_SSID: 'Göm SSID',
  AP_CLIENTS: 'AP-klienter',
  AP_MAX_CLIENTS: 'Max Klienter',
  AP_LOCAL_IP: 'Lokalt IP',
  NETWORK_SCAN: 'Sök efter WiFi-nätverk',
  IDLE: 'Vilande',
  LOST: 'Förlorad',
  SCANNING: 'Söker',
  SCAN_AGAIN: 'Sök igen',
  NETWORK_SCANNER: 'Hittade nätverk',
  NETWORK_NO_WIFI: 'Inga WiFi-nätverk hittades',
  NETWORK_BLANK_SSID: 'lämna blankt för att inaktivera WiFi', // and enable ETH // TODO translate
  NETWORK_BLANK_BSSID: 'leave blank to use only SSID', // TODO translate
  TX_POWER: 'Tx Effekt',
  HOSTNAME: 'Värdnamn',
  NETWORK_DISABLE_SLEEP: 'Inaktivera sömnläge',
  NETWORK_LOW_BAND: 'Använd lägre bandbredd',
  NETWORK_USE_DNS: 'Aktivera mDNS-tjänsten',
  NETWORK_ENABLE_CORS: 'Aktivera CORS',
  NETWORK_CORS_ORIGIN: 'CORS origin',
  NETWORK_FIXED_IP: 'Använd statiskt IP',
  NETWORK_GATEWAY: 'Gateway',
  NETWORK_SUBNET: 'Subnätmask',
  NETWORK_DNS: 'DNS-Server',
  ADDRESS_OF: '{0} Adress',
  ADMINISTRATOR: 'Administrator',
  GUEST: 'Gäst',
  NEW: 'Ny',
  NEW_NAME_OF: 'Byt namn {0}',
  ENTITY: 'Entitet',
  MIN: 'min',
  MAX: 'max',
  BLOCK_NAVIGATE_1: 'You have unsaved changes', // TODO translate
  BLOCK_NAVIGATE_2: 'If you navigate to a different page, your unsaved changes will be lost. Are you sure you want to leave this page?', // TODO translate
  STAY: 'Stay', // TODO translate
  LEAVE: 'Leave', // TODO translate
  SCHEDULER: 'Scheduler', // TODO translate
  SCHEDULER_HELP_1: 'Automate commands by adding scheduled events below. Set a unique Name to enable/disable activation via API/MQTT.', // TODO translate
  SCHEDULER_HELP_2: 'Use 00:00 to trigger once on start-up', // TODO translate
  SCHEDULE: 'Schedule', // TODO translate
  TIME: 'Time', // TODO translate
  TIMER: 'Timer', // TODO translate
  ONCHANGE: 'På förändring', // TODO translate
  CONDITION: 'Skick', // TODO translate
  IMMEDIATE: 'Omedelbar', // TODO translate
  SCHEDULE_UPDATED: 'Schedule updated', // TODO translate
  SCHEDULE_TIMER_1: 'on startup', // TODO translate
  SCHEDULE_TIMER_2: 'every minute', // TODO translate
  SCHEDULE_TIMER_3: 'every hour', // TODO translate
  CUSTOM_ENTITIES: 'Custom Entities', // TODO translate
  ENTITIES_HELP_1: 'Fetch custom entities from the EMS bus', // TODO translate
  ENTITIES_UPDATED: 'Entities Updated', // TODO translate
  WRITEABLE: 'Writeable', // TODO translate
  SHOWING: 'Showing', // TODO translate
  SEARCH: 'Search', // TODO translate
  CERT: 'TLS root certificate (leave blank for insecure)', // TODO translate
  ENABLE_TLS: 'Aktivera TLS',
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
  MODULES_1: 'Aktivera eller avaktivera externa moduler', // TODO translate
  MODULES_UPDATED: 'Modules updated', // TODO translate
  MODULES_DESCRIPTION: 'Click on the Module to activate or de-activate EMS-ESP library modules', // TODO translate
  MODULES_NONE: 'No external modules detected', // TODO translate
  RENAME: 'Rename', // TODO translate
  ENABLE_MODBUS: 'Aktivera Modbus',
  VIEW_LOG: 'View log to diagnose issues', // TODO translate
  UPLOAD_DRAG: 'drag and drop a file here or click to select one', // TODO translate
  SERVICES: 'Services', // TODO translate
  ALLVALUES: 'All Values', // TODO translate
  SPECIAL_FUNCTIONS: 'Special Functions', // TODO translate
  WAIT_FIRMWARE: 'Firmware is uploading and installing', // TODO translate
  INSTALL_VERSION: 'This will install version {0}. Are you sure?', // TODO translate
  SWITCH_DEV: 'switch to the development version', // TODO translate
  UPGRADE_AVAILABLE: 'There is a firmware upgrade available!', // TODO translate
  LATEST_VERSION: 'You are using the latest firmware version.', // TODO translate
  PLEASE_WAIT: 'Please wait', // TODO translate
  RESTARTING_PRE: 'Initializing', // TODO translate
  RESTARTING_POST: 'Preparing', // TODO translate
  AUTO_SCROLL: 'Auto Scroll' // TODO translate
};

export default sv;
