import type { Translation } from '../i18n-types';

const no: Translation = {
  LANGUAGE: 'Språk',
  RETRY: 'Prøv på nytt', // DeepL
  IS_REQUIRED: '{0} er obligatorisk', // DeepL
  SIGN_IN: 'Logg inn',
  SIGN_IN_SUBTITLE: 'Gateway for EMS-varmesystemer', // DeepL
  SIGN_OUT: 'Logg ut',
  USERNAME: 'Brukernavn',
  PASSWORD: 'Passord',
  APPLICATION_SETTINGS: 'Applikasjonsinnstillinger',
  AP_SETTINGS: 'Tilgangspunktinnstillinger',
  NETWORK_SETTINGS: 'Nettverksinnstillinger',
  NTP_SETTINGS: 'NTP-innstillinger',
  MQTT_SETTINGS: 'MQTT-innstillinger',
  EMS_BUS_SETTINGS: 'EMS-bussinnstillinger',
  HELP: 'Hjelp',
  LOGGED_IN: 'Logg inn som {name}', // DeepL
  PLEASE_SIGNIN: 'Logg inn for å fortsette', // DeepL
  UPLOAD_SUCCESSFUL: 'Opplastingen lyktes', // DeepL
  DOWNLOAD_SUCCESSFUL: 'Last ned vellykket', // DeepL
  INVALID_LOGIN: 'Feil brukernavn eller passord', // DeepL
  NETWORK: 'Nettverk',
  SECURITY: 'Sikkerhet',
  ONOFF_CAP: 'PÅ/AV',
  ONOFF: 'på/av',
  TYPE: 'Type',
  ENTITIES: 'Ojekter',
  EXPORT: 'Eksporter', // DeepL
  FAVORITES: "Favoritter",
  CLIENT_ID: 'Klient-ID',
  DEVICE_ID: 'Enhets-ID',
  PRODUCT_ID: 'Produkt-ID',
  ENTITY_ID: 'Entitets-ID',
  SENSOR_ID: 'Sensor-ID',
  TYPE_ID: 'Type-ID',
  DEVICE: 'Enhets',
  BRAND: 'Merke', // DeepL
  VERSION: 'Versjon',
  ENTITY_NAME: 'Objektsnavn',
  VALUE: 'Verdi',
  DEVICES: 'Enheter',
  SENSORS: 'Sensorer',
  AUTOMATION: 'Automatisering',
  CONNECTIVITY: 'Tilkobling',
  MAINTENANCE: 'Vedlikehold',
  CHANGE_VALUE: 'Endre verdi',
  CANCEL: 'Avbryt',
  REMOVE_ALL: 'Fjern Alt', // DeepL
  APPLY_CHANGES: 'Bruk endringer ({0})', // DeepL
  UPDATE: 'Oppdater',
  REMOVE: 'Fjern',
  RESET: 'Tilbakestill',
  PROBLEM_UPDATING: 'Problemer med oppdatering', // DeepL
  PROBLEM_LOADING: 'Problemer med innlasting', // DeepL
  ANALOG_SENSORS: 'Analoge Sensorer',
  SETTINGS: 'Innstillinger',
  SETTINGS_UPDATED: 'Innstillinger oppdatert',
  NAME_UPDATED: 'Navn oppdatert',
  SENSOR_UPDATED: 'Sensor oppdatert',
  ANALOG_SENSOR_UPDATED: 'Analog sensor oppdatert',
  OFFSET: 'Offset', // DeepL
  FACTOR: 'Faktor',
  FREQ: 'Hyppighet', // DeepL
  DUTY_CYCLE: 'Driftssyklus', // DeepL
  UNIT: 'UoM',
  STARTVALUE: 'Startverdi',
  EDIT: 'Rediger', // DeepL
  TEMP_SENSORS: 'Temperaturesensorer',
  WRITE_CMD_SENT: 'Kommando sendt', // DeepL
  EMS_BUS_WARNING: 'EMS-bussen er frakoblet. Hvis denne advarselen fortsatt vises etter noen sekunder, må du sjekke innstillingene og kortprofilen', // DeepL
  CONNECTED: 'Tilkoblet',
  TX_ISSUES: 'Tx-problemer – sjekk Tx-modus', // DeepL
  DISCONNECTED: 'Frakoblet',
  DATA_TRAFFIC: 'Datatrafikk', // DeepL
  EMS_DEVICE: 'EMS-enhet', // DeepL
  SUCCESS: 'SUCCESS', // DeepL
  FAIL: 'FEIL', // DeepL
  QUALITY: 'KVALITET',
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
  APPLICATION: 'Applikasjon', // DeepL
  CUSTOMIZATIONS: 'Tilpasninger',
  APPLICATION_RESTARTING: 'EMS-ESP starter på nytt', // DeepL
  BOARD_PROFILE: 'Brettprofil', // DeepL
  CUSTOM: 'Tilpasset', // DeepL
  GPIO_OF: '{0} GPIO',
  BUTTON: 'Knapp',
  TEMPERATURE: 'Temperatur',
  PHY_TYPE: 'Eth PHY-type', // DeepL
  DISABLED: 'deaktivert', // DeepL
  TX_MODE: 'EMS Tx-modus', // DeepL
  EMS_BUS: 'EMS Bus',
  EMS_BUS_ID: 'EMS Bus-ID',
  HARDWARE: 'Maskinvare', // DeepL
  LANGUAGE_ENTITIES: 'Språk (for enhetsojekter)', // DeepL
  HIDE_LED: 'Skjul LED',
  ENABLE_TELNET: 'Aktiver Telnet-konsoll', // DeepL
  ENABLE_ANALOG: 'Aktiver analoge Sensorer', // DeepL
  CONVERT_FAHRENHEIT: 'Konverter temperaturverdier til Fahrenheit', // DeepL
  READONLY: 'Aktiver skrivebeskyttet modus (blokkerer alle utgående EMS Tx-skrivekommandoer)', // DeepL
  WRITE_PROTECTION: 'Beskytt EMS-enhetene mot gjentatte og hyppige skrivoperasjoner', // DeepL
  WRITE_LIMIT: 'Maks. antall skrivninger per minutt (0 = ingen begrensning)', // DeepL
  WRITE_LIMIT_REFUSE: 'Avvis innskrivinger som overskrider grensen (ellers logges det kun en advarsel)', // DeepL
  UNDERCLOCK_CPU: 'Reduser CPU-hastigheten', // DeepL
  REMOTE_TIMEOUT: 'Tidsavbrudd ved fjernstyring', // DeepL
  REMOTE_TIMEOUT_EN: 'Deaktiver fjernstyring ved manglende romtemperatur', // DeepL
  HEATINGOFF: 'Start kjelen med tvungen oppvarming slått av', // DeepL
  MIN_DURATION: 'Ventetid',
  ENABLE_SHOWER_TIMER: 'Aktiver dusjtimer', // DeepL
  ENABLE_SHOWER_ALERT: 'Aktiver dusjvarsel', // DeepL
  TRIGGER_TIME: 'Utløsertid', // DeepL
  COLD_SHOT_DURATION: 'Varighet av kaldinnsprøytning', // DeepL
  FORMATTING_OPTIONS: 'Formateringsalternativer', // DeepL
  BOOLEAN_FORMAT_DASHBOARD: 'Web i boolsk format', // DeepL
  BOOLEAN_FORMAT_API: 'Boolsk format API/MQTT', // DeepL
  ENUM_FORMAT: 'Enum-format API/MQTT', // DeepL
  INDEX: 'Indeks',
  ENABLE_PARASITE: 'Aktiver 1-Wire-parasittstrøm', // DeepL
  LOG_HEX: 'Logg EMS-telegrammer i heksadesimalformat', // DeepL
  ENABLE_SYSLOG: 'Aktiver Syslog',
  LOG_LEVEL: 'Lognivå', // DeepL
  MARK_INTERVAL: 'Merk intervall', // DeepL
  SECONDS: 'sekunder',
  MINUTES: 'minutter',
  HOURS: 'timer',
  RESTART: 'Start på nytt', // DeepL
  RESTART_TEXT: 'EMS-ESP må startes på nytt for at endrede systeminnstillinger skal tre i kraft', // DeepL
  RESTART_CONFIRM: 'Er du sikker på at du vil starte EMS-ESP på nytt?', // DeepL
  COMMAND: 'Kommando',
  CUSTOMIZATIONS_RESTART: 'Alle tilpasninger er fjernet. Starter på nytt…', // DeepL
  CUSTOMIZATIONS_FULL: 'Antall valgte ojekter har overskredet grensen. Lagre i omganger', // DeepL
  CUSTOMIZATIONS_SAVED: 'Tilpasninger oppdatert', // DeepL
  CUSTOMIZATIONS_HELP_1: 'Velg en enhet og tilpass alternativene for ojektene', // DeepL
  CUSTOMIZATIONS_HELP_2: 'merk som favoritt',
  CUSTOMIZATIONS_HELP_3: 'deaktiver skrivehandling', // DeepL
  CUSTOMIZATIONS_HELP_4: 'ekskluder fra MQTT og API', // DeepL
  CUSTOMIZATIONS_HELP_5: 'skjul fra Enheter', // DeepL
  CUSTOMIZATIONS_HELP_6: 'fjern fra minnet',
  SELECT_DEVICE: 'Velg en enhet',
  MODIFIED: 'Endret',
  RESET_SELECTED: 'Tilbakestill valgt', // DeepL
  OPTIONS: 'Alternativer', // DeepL
  NAME: 'Navn',
  CUSTOMIZATIONS_RESET: 'Er du sikker på at du vil fjerne alle tilpasningene?', // DeepL
  SYSTEM_INFORMATION: 'Systeminformasjon',
  HELP_INFORMATION_1: 'Besøk nettwikien for å få instruksjoner om hvordan du konfigurerer EMS-ESP', // DeepL
  HELP_INFORMATION_2: 'For live-chat med fellesskapet, bli med på Discord-serveren vår', // DeepL
  HELP_INFORMATION_3: 'For å be om en funksjon eller rapportere en feil', // DeepL
  HELP_INFORMATION_4: 'Oppgi systeminformasjonen din når du rapporterer et problem, for å få raskere svar', // DeepL
  HELP_INFORMATION_5: 'EMS-ESP utvikles av frivillige. Et tips via Ko-fi hjelper prosjektet videre',
  UPLOAD: 'Last opp', // DeepL
  DOWNLOAD: 'Nedlasting',
  INSTALL: 'Installer',
  REINSTALL: 'Installer på nytt',
  REINSTALL_CONFIRM: 'Er du sikker på at du vil installere versjon {0} på nytt?',
  FAILED: 'mislyktes', // DeepL
  SYSTEM: 'System',
  SYSTEM_LOG: 'Systemlogg',
  STATUS: 'Status',
  DOWNLOAD_UPLOAD: 'Last ned/Last opp', // DeepL
  CLOSE: 'Lukk', // DeepL
  COMMAND_GUEST: 'Gjester kan kjøre denne kommandoen',
  TOKENS_RESET: 'Denne oppgraderingen signerer tilgangstokener med en ny enhetsnøkkel, slik at eksisterende API-tilgangstokener ikke lenger fungerer. Opprett en API-nøkkel for hver integrasjon som bruker API-et, for eksempel Home Assistant, og bruk den i stedet for det gamle tokenet.', // DeepL
  API_KEYS: 'API-nøkler',
  API_KEYS_TEXT: 'API-nøkler gjør det mulig for integrasjoner som Home Assistant og skript å bruke API-et. Send nøkkelen i overskriften «Authorization: Bearer <NØKKEL>». Les-nøkler kan lese verdier og utføre kommandoer som gjester kan utføre, mens lese- og skrive-nøkler kan gjøre alt det en administrator kan gjøre via API-et.', // DeepL
  API_KEY_ACCESS: 'Tilgang',
  API_KEY_READ: 'Les', // DeepL
  API_KEY_READ_WRITE: 'Les og Skriv', // DeepL
  API_KEY_SHOW_ONCE: 'Kopier denne nøkkelen nå og oppbevar den på et trygt sted. Den vil ikke vises igjen.', // DeepL
  CREATED: 'Opprettet',
  COPY: 'Tekst', // DeepL
  COPIED: 'Kopiert',
  USE: 'Bruk',
  FACTORY_RESET: 'Fabrikkreset', // DeepL
  SYSTEM_FACTORY_TEXT_DIALOG: 'Er du sikker på at du vil tilbakestille EMS-ESP til fabrikkinnstillingene?', // DeepL
  STABLE: 'Stabil',
  DEVELOPMENT: 'Utvikling',
  UPTIME: 'Systemets oppetid', // DeepL
  LAST_RESTART: 'Siste omstart',
  POWER_AND_RESET: 'Strøm og tilbakestilling', // DeepL
  FREE_MEMORY: 'Ledig minne', // DeepL
  BUFFER_SIZE: 'Maksimal bufferstørrelse', // DeepL
  COMPACT: 'Kortfattet', // DeepL
  SUMMARY: 'Sammendrag', // DeepL
  SHOW_MORE: 'Vis {n} flere', // DeepL
  SHOW_LESS: 'Vis mindre', // DeepL
  COREDUMP: 'Krasjlogg', // DeepL
  DOWNLOAD_SYSTEM_BACKUP: 'Systembackup', // DeepL
  ERROR: 'Uventet feil, prøv igjen', // DeepL
  TIME_SET: 'Tid innstilt', // DeepL
  MANAGE_USERS: 'Administrer brukere', // DeepL
  USER_WARNING: 'Du må ha konfigurert minst én administratorbruker', // DeepL
  ADD: 'Legg til',
  TELNET_ENABLED: 'Telnet-konsollen er aktivert. Telnet er ikke kryptert, så deaktiver den i programinnstillingene hvis du ikke bruker den.', // DeepL
  TOO_MANY_LOGINS: 'For mange mislykkede innloggingsforsøk. Prøv igjen om noen minutter.', // DeepL
  PASSWORD_KEEP: 'La feltet stå tomt for å beholde det nåværende passordet', // DeepL
  SETUP_TITLE: 'Angi passordene dine',
  SETUP_TEXT: 'Denne enheten bruker fortsatt fabrikkinnstilte passord. Angi ditt eget passord før du fortsetter.', // DeepL
  SETUP_ADMIN_PASSWORD: 'Nytt passord for {name}',
  SETUP_AP_PASSWORD: 'Nytt passord for tilgangspunktet',
  SETUP_AP_REQUIRED: 'Det er ikke konfigurert noe Wi-Fi-nettverk ennå, så tilgangspunktet er inngangsveien. 8–63 tegn, brukes neste gang tilgangspunktet starter.', // DeepL
  SETUP_AP_OPTIONAL: 'Anbefalt: 8–63 tegn, tas i bruk neste gang tilgangspunktet starter. La feltet stå tomt for å beholde den nåværende.', // DeepL
  SETUP_AP_DEFAULT: 'Tilgangspunktet bruker fortsatt fabrikkpassordet. Angi ditt eget passord i innstillingene for tilgangspunktet.', // DeepL
  SETUP_VIEW_USER: 'Valgfritt: Legg til en gjestebruker med lesetilgang.', // DeepL
  NETWORK_CORS_ORIGIN_HELP: 'Nettadressen som brukes til å kalle opp API-et fra en nettleser, for eksempel http://homeassistant.local:8123', // DeepL
  NETWORK_CORS_WILDCARD: 'Med * kan ethvert nettsted som er åpent i en nettleser på nettverket ditt, kalle opp API-et. Angi den eksakte adressen i stedet.', // DeepL
  NETWORK_ALLOWED_HOSTS: 'Tillatte vertsnavn',
  NETWORK_ALLOWED_HOSTS_HELP: 'Denne enheten kan åpnes i en nettleser under ulike navn, for eksempel ems-esp.fritz.box eller et omvendt proxy-navn. IP-adressen, vertsnavnet og vertsnavn.local fungerer alltid', // DeepL
  NOT_ENABLED: 'Ikke aktivert', // DeepL
  MQTT_ERRORS: 'MQTT-feil',
  DISCONNECT_REASON: 'Årsak til frakobling', // DeepL
  MQTT_BASE_TOPIC: 'Grunnleggende tema', // DeepL
  LAST_DISCONNECT: 'Siste frakobling',
  CONNECTED_SINCE: 'Tilkoblet siden',
  ENABLE_MQTT: 'Aktiver MQTT',
  BROKER: 'Broker',
  BASE_TOPIC: 'Grunnlag', // DeepL
  OPTIONAL: 'valgfritt',
  MQTT_FORMAT: 'Emne-/innholdsformat', // DeepL
  MQTT_NEST_1: 'Innebygd i ett enkelt emne', // DeepL
  MQTT_NEST_2: 'Som enkeltstående emner', // DeepL
  MQTT_RESPONSE: 'Publiser kommandoens utdata til et `response`-emne', // DeepL
  MQTT_PUBLISH_TEXT_1: 'Publiser emner med én verdi ved endring', // DeepL
  MQTT_PUBLISH_TEXT_2: 'Publiser til kommandoemner (ioBroker)', // DeepL
  MQTT_PUBLISH_TEXT_3: 'Aktiver MQTT-oppdagelse', // DeepL
  MQTT_PUBLISH_TEXT_4: 'Prefiks for oppdagelsestemaene', // DeepL
  MQTT_PUBLISH_TEXT_5: 'Oppdagelsestype', // DeepL
  MQTT_INPUT_NUMBER_FORMAT: 'Inndataformat for tall', // DeepL
  MQTT_PUBLISH_INTERVALS: 'Publiseringsintervaller', // DeepL
  MQTT_INT_BOILER: 'Varmekjeler og varmepumper', // DeepL
  MQTT_INT_THERMOSTATS: 'Termostater', // DeepL
  MQTT_INT_SOLAR: 'Solcellemodul', // DeepL
  MQTT_INT_MIXER: 'Blandermodul', // DeepL
  MQTT_INT_WATER: 'Vannmodul', // DeepL
  MQTT_QUEUE: 'MQTT-kø',
  DEFAULT: 'Standard',
  MQTT_ENTITY_FORMAT: 'Format for enhets-ID', // DeepL
  MQTT_ENTITY_FORMAT_0: 'Enkelt forekomst, langt navn (v3.4)', // DeepL
  MQTT_ENTITY_FORMAT_1: 'Enkelt forekomst, kort navn', // DeepL
  MQTT_ENTITY_FORMAT_2: 'Flere forekomster, kort navn', // DeepL
  MQTT_CLEAN_SESSION: 'Opprett ny økt', // DeepL
  MQTT_RETAIN_FLAG: 'Sett alltid «retain»-flagget', // DeepL
  MQTT_COMMANDS: 'Kommandoer mottatt via MQTT', // DeepL
  MQTT_COMMANDS_0: 'Av, kun leseverdier', // DeepL
  MQTT_COMMANDS_1: 'Endre verdier og utfør kommandoer', // DeepL
  MQTT_COMMANDS_2: 'Alt, også systemkommandoer',
  INACTIVE: 'Inaktiv', // DeepL
  ACTIVE: 'Aktiv',
  UNKNOWN: 'Ukjent',
  SET_TIME: 'Still Inn Tid', // DeepL
  SET_TIME_TEXT: 'Skriv inn lokal dato og klokkeslett nedenfor for å stille inn klokkeslettet', // DeepL
  LOCAL_TIME: 'Lokal tid', // DeepL
  UTC_TIME: 'UTC-tid',
  ENABLE_NTP: 'Aktiver NTP',
  NTP_SERVER: 'NTP-server', // DeepL
  TIME_ZONE: 'Tidssone',
  ACCESS_POINT: 'Tilgangspunkt', // DeepL
  AP_PROVIDE: 'Aktiver tilgangspunkt', // DeepL
  AP_PROVIDE_TEXT_2: 'Når nettverkstilkobling ikke blir funnet', // DeepL
  AP_PROVIDE_TEXT_3: 'Aldri', // DeepL
  AP_PREFERRED_CHANNEL: 'Foretrukket kanal',
  AP_HIDE_SSID: 'Skjul SSID',
  AP_CLIENTS: 'AP-klienter',
  AP_MAX_CLIENTS: 'Maks antall klienter', // DeepL
  AP_LOCAL_IP: 'Lokal IP', // DeepL
  IDLE: 'Inaktiv', // DeepL
  SCAN_AGAIN: 'Skann på nytt', // DeepL
  NETWORK_NO_WIFI: 'Ingen Wi-Fi-nettverk funnet', // DeepL
  NETWORK_BLANK_BSSID: 'La feltet stå tomt for å bruke kun SSID', // DeepL
  TX_POWER: 'Sendeeffekt', // DeepL
  HOSTNAME: 'Vertsnavn', // DeepL
  NETWORK_DISABLE_SLEEP: 'Deaktiver Wi-Fi-hvilemodus', // DeepL
  NETWORK_LOW_BAND: 'Bruk lavere Wi-Fi-båndbredde', // DeepL
  NETWORK_USE_DNS: 'Aktiver mDNS-tjenesten', // DeepL
  NETWORK_ENABLE_CORS: 'Aktiver CORS', // DeepL
  NETWORK_CORS_ORIGIN: 'CORS-opprinnelse', // DeepL
  NETWORK_FIXED_IP: 'Bruk fast IP-adresse', // DeepL
  NETWORK_GATEWAY: 'Gateway',
  NETWORK_SUBNET: 'Nettverksmaske',
  NETWORK_DNS: 'DNS-servere', // DeepL
  ADDRESS_OF: '{0}-adresse', // DeepL
  ADMINISTRATOR: 'Administrator',
  GUEST: 'Gjest',
  NEW: 'Ny',
  NEW_ENTITY_NAME: 'Nytt entitetsnavn',
  MIN: 'min',
  MAX: 'max',
  BLOCK_NAVIGATE_1: 'Du har ubehandlede endringer', // DeepL
  BLOCK_NAVIGATE_2: 'Hvis du går til en annen side, vil endringer du ikke har lagret gå tapt. Er du sikker på at du vil forlate denne siden?', // DeepL
  UPLOAD_ABORTED: 'Opplastingen ble avbrutt', // DeepL
  BLOCK_BUSY_1: 'Fortsatt under utvikling', // DeepL
  BLOCK_BUSY_2: 'Hvis du forlater denne siden nå, avsluttes den. Er du sikker på at du vil forlate siden?', // DeepL
  DISCARD: 'Kast', // DeepL
  COMMANDS_UPDATED: 'Kommandoer oppdatert',
  V_REQUIRED: 'Påkrevd',
  V_BETWEEN: 'Må være mellom {min} og {max}',
  V_MAX_CHARS: 'Må være {max} tegn eller færre', // DeepL
  V_CHARS: 'Må være {min}–{max} tegn',
  V_IN_USE: 'Allerede i bruk', // DeepL
  V_IP: 'Må være en IP-adresse',
  V_HOSTNAME: 'Må være et gyldig vertsnavn',
  V_IP_OR_HOSTNAME: 'Må være en IP-adresse eller et vertsnavn',
  V_NAME: 'Opptil {max} tegn: bokstaver, tall eller _', // DeepL
  V_USERNAME: 'Opptil {max} tegn: bokstaver, tall, _ eller .', // DeepL
  V_DAY: 'Velg minst én dag',
  V_HEX: 'Må være et heksadesimaltall', // DeepL
  V_FINGERPRINT: 'Må være et SHA-1-fingeravtrykk (40 heksadesimale siffer)', // DeepL
  V_EXAMPLE: 'Må se ut som {example}',
  V_NOT_DEFAULT: 'Velg et annet passord',
  ZERO_DISABLES: '0 slår den av', // DeepL
  MINUS_ONE_DISABLES: '-1 slår den av', // DeepL
  LED_TYPE: 'LED-type',
  PHY_POWER: 'PHY-strøm',
  PHY_CLOCK: 'PHY-klokke',
  AUTO: 'Auto',
  PORT: 'Port',
  TIMEOUT: 'Tidsavbrudd',
  HOST: 'Vert',
  SMTP_SERVER: 'SMTP-server',
  KEEP_ALIVE: 'Hold det i gang', // DeepL
  MQTT_REFRESH: 'Oppdater MQTT',
  MQTT_REFRESHED: 'MQTT oppdatert',
  MQTT_R_USER: 'Koblet fra av brukeren', // DeepL
  MQTT_R_TCP: 'TCP-forbindelsen er avsluttet', // DeepL
  MQTT_R_PROTOCOL: 'Protokollversjon godtas ikke', // DeepL
  MQTT_R_ID: 'Klient-ID avvist',
  MQTT_R_SERVER: 'Serveren er utilgjengelig',
  MQTT_R_CREDENTIALS: 'Feilformaterte påloggingsopplysninger', // DeepL
  MQTT_R_AUTH: 'Ikke autorisert',
  MQTT_R_FINGERPRINT: 'TLS-fingeravtrykket stemmer ikke overens', // DeepL
  GPIO_INVALID: 'Ugyldige GPIO-innstillinger',
  CONNECTED_VIA: 'Koblet til via {0}', // DeepL
  NETWORK_NOT_FOUND: 'Nettverk ikke funnet', // DeepL
  CONNECTION_FAILED: 'Tilkoblingen mislyktes',
  CONNECTION_LOST: 'Tilkoblingen ble brutt',
  AP_LINGERING: 'Venter til systemet er inaktivt', // DeepL
  NONE: 'Ingen',
  LOG_NO_CONNECTION: 'Ingen tilkobling til loggtjenesten',
  CHECK_SYSTEM_LOG: 'Sjekk systemloggen for mulige årsaker', // DeepL
  VALUE_TYPE: 'Verditype',
  DEFAULT_VALUE: 'Standardverdi',
  DEFAULT_NAME: 'Standardnavn',
  ALWAYS_OFF: 'Alltid av',
  ALWAYS_ON: 'Alltid på',
  NO_DATA: 'Ingen favoritter ennå. Merk dem i {customizations}, eller se alle ojekter i {devices}.', // DeepL
  STAY: 'Hold deg', // DeepL
  LEAVE: 'La være', // DeepL
  MENU: 'Meny',
  MORE: 'Mer',
  SHOW_PASSWORD: 'Vis passord',
  STATUS_RUNNING: 'EMS-ESP kjører',
  NUM_ERRORS: '{num} feil',
  NUM_WARNINGS: '{num} {{advarsel|advarsler}}',
  HEAP: 'Heap',
  METER_USED: '{used} / {total} {unit} brukt',
  MAX_BLOCK: '{num} KB maks. blokk', // DeepL
  MAX_BLOCK_HELP: '«Max block» er den største delen av heap-minnet som kan tildeles på én gang. Under 40 KB kan nettgrensesnittet og MQTT svikte.', // DeepL
  FLASH_CHIP: 'Flash-brikke', // DeepL
  FILE_SYSTEM: 'Filsystem',
  DETAILS: 'Detaljer',
  RECONNECTS: 'Kobler til på nytt', // DeepL
  CONNECTS: 'Kobler til', // DeepL
  ACTIVITY_HELP: 'Noen få Rx-feil er normalt. Mange Tx-feil betyr vanligvis at du har valgt feil Tx-modus for kjelen din.', // DeepL
  PAUSE: 'Pause',
  RESUME: 'CV', // DeepL
  LOG_PAUSED: 'Pauset, nye linjer holdt tilbake: {num}', // DeepL
  FILTER_LINES: 'Filterlinjer', // DeepL
  SCHEDULER: 'Planlegger',
  SCHEDULER_HELP_1: 'Automatiser kommandoer ved å legge til planlagte hendelser nedenfor', // DeepL
  SCHEDULER_HELP_2: 'ved oppstart',
  SCHEDULE: 'Tidsplan', // DeepL
  TIME: 'Tid',
  TIMER: 'Timer',
  ONCHANGE: 'Ved endring', // DeepL
  CONDITION: 'Tilstand', // DeepL
  SCHEDULE_UPDATED: 'Tidsplan oppdatert', // DeepL
  CUSTOM_ENTITIES: 'Egendefinerte ojekter', // DeepL
  ENTITIES_HELP_1: 'Definer egendefinerte EMS-ojekter eller dynamiske brukervariabler', // DeepL
  ENTITIES_UPDATED: 'Oppdaterede ojekter', // DeepL
  WRITEABLE: 'Skrivbar',
  SHOWING: 'Viser',
  SEARCH: 'Søk',
  CERT: 'CA-sertifikat (PEM)',
  ENABLE_TLS: 'Aktiver TLS', // DeepL
  TLS_CHECK: 'Sjekk megleren med', // DeepL
  TLS_CHECK_CA: 'CA-sertifikat',
  TLS_CHECK_FINGERPRINT: 'Sertifikatets fingeravtrykk (SHA-1)',
  TLS_CHECK_NONE: 'Ingenting, ikke merket av', // DeepL
  TLS_NOT_CHECKED: 'Forbindelsen er kryptert, men mellomleddet blir ikke sjekket, så en annen enhet på nettverket kan utgi seg for å være det', // DeepL
  ON: 'På',
  OFF: 'Av',
  POLARITY: 'Polaritet', // DeepL
  ACTIVEHIGH: 'Aktiv høy', // DeepL
  ACTIVELOW: 'Aktiv lav', // DeepL
  UNCHANGED: 'Uendret', // DeepL
  SECURITY_1: 'Legg til eller fjern brukere',
  DOWNLOAD_UPLOAD_1: 'Last ned og last opp innstillinger',
  MODULES: 'Modul',
  MODULES_1: 'Aktiver eller deaktiver eksterne moduler',
  NETWORK_DESC: 'Wi-Fi, Ethernet og vertsnavn',
  AP_DESC: 'Eget hotspot for oppsett og reservetilgang',
  MQTT_DESC: 'Broker, emner og discovery',
  APPLICATION_DESC: 'EMS-buss, kort, sensorer og formatering',
  NTP_DESC: 'Tidsserver og tidssone',
  FIRMWARE_DESC: 'Versjon, oppdateringer og gjenoppretting',
  MODULES_UPDATED: 'Modul oppdatert', // DeepL
  MODULES_NONE: 'Ingen eksterne moduler oppdaget', // DeepL
  RENAME: 'Gi nytt navn',
  ENABLE_MODBUS: 'Aktiver Modbus',
  VIEW_LOG: 'Se loggen for å diagnostisere problemer', // DeepL
  SERVICES: 'Tjenester',
  ALLVALUES: 'Alle verdier',
  WAIT_FIRMWARE: 'Firmware lastes opp og installeres', // DeepL
  INSTALL_VERSION: 'Dette installerer versjon {0}',
  UPDATE_AVAILABLE: 'oppdatering tilgjengelig',
  LATEST_VERSION: 'Du bruker den nyeste firmwareversjonen ({0})',
  PLEASE_WAIT: 'Vennligst vent',
  RESTARTING_PRE: 'Oppstart', // DeepL
  RESTARTING_POST: 'Forberedelser', // DeepL
  AUTO_SCROLL: 'Automatisk rulling',
  DASHBOARD: 'Dashboard',
  DEVELOPER_MODE: 'Utviklermodus', // DeepL
  DISABLE_RESET: 'Deaktiver tilbakestilling til fabrikkinnstillinger', // DeepL
  AUTO_FW_CHECK: 'Sjekk automatisk etter firmwareoppdateringer', // DeepL
  BYTES: 'Bytes',
  BITMASK: 'Bitmaske', // DeepL
  DUPLICATE: 'Duplikat',
  DASHBOARD_1: 'Alle aktive EMS-enheter som er merket som favoritter, vises nedenfor, i tillegg til alle tilpassede ojekter, tidsplaner og eksterne sensorer.', // DeepL
  NO_GPIO: 'Ingen tilgjengelige GPIO-porter funnet', // DeepL
  PLATFORM: 'Plattform',
  RELEASE_TYPE: 'Utgivelsestype', // DeepL
  FIRMWARE_VERSION_INFO: 'Informasjon om Firmwareversjon',
  USER_PROFILE: 'Brukerprofil',
  ONLINE_HELP: 'online hjelp',
  UPGRADE_IMPORTANT_MESSAGES: 'Viktige Oppgraderingsmeldinger', // DeepL
  UPGRADE_IMPORTANT_MESSAGES_2: 'Du oppgraderer til en ny hovedversjon. Det anbefales å ta en sikkerhetskopi av systemet først. Sjekk også den elektroniske hjelpen for eventuelle vesentlige endringer.', // DeepL
  UPGRADE_IMPORTANT_MESSAGES_3: 'Versjon 3.10 inneholder sikkerhetsendringer som medfører kompatibilitetsbrudd. Ta først en sikkerhetskopi av systemet: etter oppgraderingen lagres passordene i hashform, og eldre versjoner kan ikke lese dem, så du trenger denne sikkerhetskopien hvis du skal gå tilbake. Hvert API-kall krever da en API-nøkkel, så integrasjoner som Home Assistant må oppdateres. Les om endringene som medfører kompatibilitetsbrudd i den elektroniske hjelpen før du fortsetter.', // DeepL
  WARNING_SYSTEM_BACKUP: 'Dette oppretter en sikkerhetskopi av hele systemkonfigurasjonen og innstillingene dine. Passord og nøkler utelates med mindre du oppgir dem nedenfor; når du gjenoppretter sikkerhetskopien, beholdes passordene som allerede finnes på enheten. Kommandoer inkluderes alltid og kan inneholde påloggingsopplysninger, så vær forsiktig med å dele dem. Vil du fortsette?', // DeepL
  BACKUP_INCLUDE_SECRETS: 'Inkluder passord og nøkler (nødvendig for å gjenopprette på en ny enhet)', // DeepL
  TEST_EMAIL_SUCCESSFUL: 'Test-e-post sendt', // DeepL
  SYSTEM_NAME: 'Systemnavn',
  COMMANDS: 'Kommandoer',
  COMMANDS_HELP_1: 'Definer gjenbrukbare, navngitte kommandoer nedenfor. Disse kan utføres fra konsollen, API/MQTT eller refereres til av Planleggeren', // DeepL
  COMMAND_EXECUTED: 'Kommando utført', // DeepL
  RUN: 'Kjør',
  INTERNET_CONNECTION_CHECK: 'Sjekk på nettet for tilgjengelige versjoner', // DeepL
  UPLOAD_BACKUP_TEXT: 'Klikk for å velge, eller dra en sikkerhetskopifil (.json) hit', // DeepL
  UPLOAD_FIRMWARE_TEXT: 'Klikk for å velge, eller dra en firmwarefil (.bin/.md5) hit', // DeepL
  UPLOAD_MD5_RECEIVED: 'Sammendrag mottatt, last nå opp firmware-filen (.bin)', // DeepL
  UPLOAD_MD5_MATCHED: 'MD5-samsvar, installerer fastvare…', // DeepL
  UPLOAD_NOT_OFFICIAL: 'Dette er ikke en offisiell EMS-ESP-versjon. Installer kun fastvare du stoler på', // DeepL
  UPLOAD_NOT_OFFICIAL_HELP: 'Om offisielle og tilpassede versjoner', // DeepL
  UPLOAD_INSTALL_ANYWAY: 'Installer uansett', // DeepL
  OFFICIAL_BUILD: 'Offisiell Versjon',
  CUSTOM_BUILD: 'Tilpasset Versjon', // DeepL
  USING_CUSTOM_FIRMWARE: 'Du bruker en tilpasset firmware', // DeepL
  PASSWORD_CONFIRM_TITLE: 'Skriv inn passordet ditt på nytt', // DeepL
  PASSWORD_CONFIRM_TEXT: 'For din egen sikkerhet må du oppgi passordet ditt før denne endringen. Du vil ikke bli bedt om det igjen på 5 minutter.', // DeepL
  PASSWORD_CONFIRM_BUTTON: 'Bekreft',
  PASSWORD_WRONG: 'Feil passord',
  PASSWORD_NOT_CONFIRMED: 'Avbrutt, passordet ble ikke angitt', // DeepL
  RECOVERY_APP: 'Gjenopprettingsapp',
  RECOVERY_NOT_INSTALLED: 'Ikke installert',
  RECOVERY_INSTALLED: 'Gjenopprettingsfirmware installert',
  RESTART_INTO_RECOVERY: 'Start på nytt i gjenopprettingsappen',
  RECOVERY_NOT_FOUND: 'Recovery-appen ble ikke funnet på nettet', // DeepL
  RECOVERY_START_FAILED: 'Recovery-appen kan ikke startes; den kan være utdatert eller skadet. Installer først den nyeste versjonen', // DeepL
  PARTITION_START_FAILED: 'Denne fastvaren kan ikke startes, den kan være skadet',
  INSTALLED_VERSIONS: 'Installerte versjoner',
  EMSESP_APP: 'EMS-ESP-appen', // DeepL
  NO_UPDATES_AVAILABLE: 'ingen oppdateringer tilgjengelig',
  SWITCH_TO_DEV: 'Bytt til dev {0}',
  SWITCH_TO_STABLE: 'Bytt til stabil versjon {0}',
  SAVE: 'Lagre',
  NEEDS_RESTART: 'må startes på nytt', // DeepL
  CHANGED_SETTINGS: '{num} {{endret innstilling|endrede innstillinger}}',
  SAVED_RESTART: 'Lagret. Start EMS-ESP på nytt for å bruke de nye innstillingene.',
  RESTART_NOW: 'Start på nytt nå',
  COPY_FAILED: 'Kunne ikke kopiere, velg teksten og kopier den selv', // DeepL
  DOCUMENTATION: 'Dokumentasjon',
  COMMUNITY_CHAT: 'Fellesskapschat',
  REPORT_ISSUE: 'Rapporter en feil eller be om en funksjon',
  SUPPORT_PROJECT: 'Støtt prosjektet',
  GET_HELP: 'Få hjelp',
  REPORT_PROBLEM: 'Meld fra om et problem', // DeepL
  REPORT_STEP_1: 'Last ned systeminformasjonen',
  REPORT_STEP_1_TEXT: 'versjon, hovedkort, EMS-bussstatus, tilkoblede enheter og innstillinger, uten passord', // DeepL
  REPORT_STEP_2: 'Lagre den siste loggen',
  REPORT_STEP_2_TEXT: 'Sett først loggnivået til DEBUG hvis problemet ligger i en enhet', // DeepL
  REPORT_STEP_3: 'Opprett en sak eller spør på Discord',
  REPORT_STEP_3_TEXT: 'Legg ved begge filene, eller lim inn den korte oppsummeringen nedenfor i en chat', // DeepL
  NEW_ISSUE: 'Nytt problem', // DeepL
  SHORT_SUMMARY: 'Kort oppsummering', // DeepL
  LICENSE_TEXT: 'Åpen kildekode under LGPL-3.0-lisensen', // DeepL
  ADMIN_ACCESS_TEXT: 'full tilgang til innstillinger, enheter og API-et',
  GUEST_ACCESS_TEXT: 'kan vise verdier og utføre kommandoer som gjester kan utføre', // DeepL
  LANGUAGE_BROWSER_ONLY: 'endrer kun denne nettleseren', // DeepL
  LANGUAGE_ENTITIES_HINT: 'Språket for objektsnavnene i MQTT og API-et angis i', // DeepL
  ADMINISTRATION: 'Administrasjon',
  API_KEYS_SHORT: 'taster for Home Assistant og skript', // DeepL
  ADDRESSES: 'Adresser',
  SEARCH_SETTINGS: 'Søkeinnstillinger', // DeepL
  NO_SETTINGS_MATCH: 'Ingen innstillinger samsvarer med søket ditt', // DeepL
  GENERAL: 'Generelt',
  BOARD: 'Styret', // DeepL
  ADVANCED: 'Avansert',
  EMS_BUS_DESC: 'Slik kommuniserer EMS-ESP med varmesystemet ditt.', // DeepL
  BOARD_DESC: 'Pin-kodene hentes fra kortprofilen. For å angi dem selv må du slå på utviklermodus og velge «Custom».', // DeepL
  FORMAT_DESC: 'Hvordan verdiene vises i nettgrensesnittet og i MQTT/API-payloads.', // DeepL
  BOARD_PINS: 'Pins fra profilen', // DeepL
  ENABLE_EMAIL: 'Aktiver e-postvarsler',
  SEND_TEST_EMAIL: 'Send test-e-post',
  EMAIL_FROM: 'Fra',
  EMAIL_TO: 'Til',
  EMAIL_SUBJECT: 'Emne',
  WIFI_SCAN: 'Wi-Fi-skanning', // DeepL
  SCAN: 'Skann', // DeepL
  NETWORKS_NEARBY: 'Nettverk i nærheten',
  SIGNAL: 'Signal',
  CHANNEL: 'Kanal',
  WIFI_OPEN: 'åpen',
  WIFI_SAVED: 'lagret',
  NETWORK_ETHERNET_INFO: 'Nå tilkoblet via Ethernet ({ip}). Wi-Fi brukes kun uten kabel.', // DeepL
  HOSTNAME_ACCESS: 'Vertsnavn og tilgang',
  IP_ADDRESS: 'IP-adresse',
  NETWORK_ETH_10MBIT: 'Tving 10 Mbit halvdupleks', // DeepL
  NETWORK_ETH_10MBIT_HELP: 'kun for feilbrytere eller lange kabler', // DeepL
  NETWORK_DISABLE_SLEEP_HELP: 'mer pålitelig, bruker litt mer strøm',
  NETWORK_MDNS_HELP: 'Du når den via {host}.local med mDNS', // DeepL
  NETWORK_FIXED_IP_HELP: 'ellers brukes DHCP',
  NETWORK_CORS_HELP: 'kun nødvendig når en annen nettside påkaller API-et', // DeepL
  NTP_TIME_ON_DEVICE: 'tid på EMS-ESP',
  NTP_TIME_SERVER: 'Tidsserver',
  NTP_THERMOSTAT_CLOCK: 'Termostatklokke',
  NTP_THERMOSTAT_SYNC: 'Still inn klokken på termostaten', // DeepL
  NTP_THERMOSTAT_SYNC_HELP: 'stiller inn dato og klokkeslett på EMS-termostaten', // DeepL
  NTP_THERMOSTAT_SYNC_1: 'På, EMS-ESP-tidssone', // DeepL
  NTP_THERMOSTAT_SYNC_2: 'På, egen tidssone',
  NTP_THERMOSTAT_TZ: 'Termostatens tidssone',
  MQTT_PREVIEW: 'Hva blir publisert', // DeepL
  MQTT_PREVIEW_CMD: 'godkjent som en kommando', // DeepL
  MQTT_PREVIEW_RESPONSE: 'utdata fra kommandoer',
  MQTT_BEHAVIOUR: 'Atferd', // DeepL
  MQTT_TOPICS: 'Emner og nyttelast', // DeepL
  MQTT_DISCOVERY: 'Oppdagelse', // DeepL
  MQTT_INTERVALS_HELP: 'Sekunder mellom publiseringer, 0 = publiser ved hver endring.',
  MQTT_SINGLE_NO_DISCOVERY: 'ikke tilgjengelig med oppdagingsfunksjonen på', // DeepL
  MQTT_DISCOVERY_NO_SINGLE: 'ikke tilgjengelig med emner med én verdi på', // DeepL
  MQTT_DISCOVERY_HELP: 'oppretter ojektene i Home Assistant eller Domoticz automatisk', // DeepL
  MQTT_ENTITY_FORMAT_WARN: 'Hvis du endrer dette, vil alle ojekter i Home Assistant få nytt navn', // DeepL
  USERS: 'Brukere',
  USERS_TEXT: 'Administratorer kan endre alt. Gjester kan se på og utføre kommandoer som er merket for gjester.', // DeepL
  ADD_USER: 'Legg til bruker',
  EDIT_USER: 'Rediger bruker',
  ROLE: 'Rolle',
  ROLE_ADMIN: 'admin', // DeepL
  ROLE_GUEST: 'gjest',
  ADMIN: 'Admin', // DeepL
  YOU: 'du', // DeepL
  NEW_PASSWORD: 'Nytt passord',
  UNSAVED_CHANGES: '{num} {{ulagret endring|ulagrede endringer}}',
  CREATE_KEY: 'Opprett Nøkkel',
  NEW_API_KEY: 'Ny API-nøkkel',
  NO_API_KEYS: 'Ingen API-nøkler ennå',
  API_KEYS_LIMIT: 'Du kan ha maksimalt {num}-taster.', // DeepL
  REVOKE: 'Tilbakekall',
  REVOKE_KEY: 'Tilbakekall API-nøkkel',
  REVOKE_KEY_TEXT: 'Alt som bruker «{name}» slutter å fungere umiddelbart.', // DeepL
  MODULES_TEXT: 'Valgfrie funksjoner som er innebygd i denne fastvaren. Slå dem på eller av her.', // DeepL
  BY_AUTHOR: 'av {name}',
  LICENSE_KEY: 'Lisensnøkkel',
  MODULE_ACTIVATED: 'aktivert',
  MODULE_PENDING: 'venter på aktivering',
  ENABLED: 'Aktivert',
  UNDO: 'Angre',
  FW_UPDATES: 'Oppdateringer',
  FW_VERSION_AVAILABLE: '{version} er tilgjengelig',
  FW_RELEASE_NOTES: 'Utgivelsesnotater', // DeepL
  FW_UPDATES_OFFLINE: 'Kunne ikke sjekke om det finnes nye versjoner på nettet. EMS-ESP trenger en internettforbindelse for dette.', // DeepL
  FW_RESTART_RESET: 'Omstart og Tilbakestilling',
  FW_RECOVERY_TEXT: 'Skal du starte på nytt i Recovery-appen? Den kobler seg til nettverket ditt og starter også sitt eget tilgangspunkt, der du kan laste opp fastvare.', // DeepL
  FW_SIZE: 'Størrelse',
  FW_INSTALL_DATE: 'Installert på', // DeepL
  FW_UPLOAD_FAILED: 'Opplastingen mislyktes: feil filtype eller firmware for et annet kort', // DeepL
  FILE_TYPE_INVALID: 'Denne filtypen støttes ikke, velg en {types}-fil',
  WHO_CAN_RUN: 'Hvem kan kjøre',
  CMD_NONE: 'Ingen kommandoer funnet',
  CMD_NONE_MATCH: 'Ingen kommandoer samsvarer med søket ditt', // DeepL
  SCHEDULE_NONE: 'Ingen tidsplaner funnet',
  SCHEDULE_NONE_MATCH: 'Ingen tidsplaner samsvarer med søket ditt', // DeepL
  CE_NONE: 'Ingen ojekter funnet', // DeepL
  CE_ALL: 'Alle',
  CE_STORAGE: 'Lagring',
  CE_SOURCE: 'Kilde',
  CE_EMS_SOURCE: 'enhet {device} · telegram {type} · offset {offset}',
  CE_RAM_TEXT: 'bevares til omstart', // DeepL
  CE_NVS_TEXT: 'bevares ved omstart', // DeepL
  CE_EXCLUDED: 'ikke i API/MQTT',
  SEARCH_SENSORS: 'Søk etter navn, ID eller GPIO',
  NO_SENSORS: 'Ingen sensorer funnet',
  NO_SENSORS_MATCH: 'Ingen sensorer samsvarer med søket ditt', // DeepL
  SENSORS_HELP_1: 'Administrer temperaturfølere og analoge sensorer koblet til denne enheten', // DeepL
  TEMP_SENSORS_HELP: 'Funnet automatisk. Klikk på en sensor for å gi den nytt navn eller angi en forskyvning', // DeepL
  FREE_GPIOS: 'Ledige GPIO-er',
  SYSTEM_SENSOR: 'Systemsensor',
  MASK_FAVORITE: 'Favoritt',
  MASK_READONLY: 'Skriv for funksjonshemmede', // DeepL
  MASK_NO_API: 'Ingen MQTT/API', // DeepL
  MASK_HIDDEN: 'Skjult',
  MASK_REMOVED: 'Fjernet',
  FILTER_ALL: 'Alle',
  NUM_SELECTED: 'Valgt: {num}',
  ALL_SHOWN: 'Alle vises: {num}', // DeepL
  BULK_HINT: 'Knappene endrer de merkede radene, eller viser alle radene når ingen er merket.', // DeepL
  CLEAR_SELECTION: 'Tydelig valg', // DeepL
  NO_ENTITIES_MATCH: 'Ingen ojekter samsvarer', // DeepL
  LIMIT_HELP: 'La feltet stå tomt for den innebygde grensen', // DeepL
  MIN_ABOVE_MAX: 'Minimum må ikke være høyere enn maksimum', // DeepL
  RENAME_DEVICE: 'Gi Enheten Nytt Navn',
  WEEKLY: 'Ukentlig',
  EVERY_DAY: 'Hver dag',
  SCHEDULER_NO_COMMANDS: 'Legg først til en kommando på siden «Kommandoer»', // DeepL
  DU_SETTINGS: 'EMS-ESP-innstillinger',
  DU_SETTINGS_TEXT: 'Sikkerhetskopi av EMS-ESP-innstillingene: nettverk, MQTT, Tilpasninger, Planlegger, egendefinerte ojekter og moduler. Når du laster opp en innstillings- eller sikkerhetskopifil, startes EMS-ESP på nytt.', // DeepL
  HB_TITLE: 'Oppvarmingsinnstillinger', // DeepL
  HB_TEXT: 'Lagrer gjeldende verdier for kjelen, termostaten og andre EMS-enheter, slik at du kan gjenopprette dem senere, for eksempel etter at du har byttet ut en enhet. Skrivebeskyttede ojekter og kommandoer endres aldri ved en gjenoppretting.', // DeepL
  HB_BACKUP: 'Sikkerhetskopier Oppvarmingsinnstillingene', // DeepL
  HB_RESTORE: 'Gjenopprett oppvarmingsinnstillinger', // DeepL
  DATA_EXPORT: 'Dataeksport',
  DATA_EXPORT_TEXT: 'For support og analyse. Disse filene kan ikke lastes opp igjen.', // DeepL
  NUM_VALUES: '{num} {{verdi|verdier}}',
  HB_NOTHING: 'ingenting å sikkerhetskopiere',
  FILE_NAME: 'Filnavn',
  HB_NOTE: 'Merknad (valgfritt, vises igjen når du gjenoppretter)', // DeepL
  HB_INCOMPLETE: 'Verdier som ennå ikke er lest inn fra bussen, utelates. Vent noen minutter etter en omstart for å få en fullstendig sikkerhetskopi.', // DeepL
  HB_RESTORE_TEXT: 'Velg en sikkerhetskopi av oppvarmingsinnstillingene. Ingenting blir lagret før du har sjekket endringene og bekreftet dem.', // DeepL
  HB_DROP: 'Klikk for å velge, eller dra og slipp en reservefil for oppvarming (.json) her', // DeepL
  HB_INVALID: 'Dette er ikke en sikkerhetskopi av oppvarmingsinnstillingene', // DeepL
  HB_OTHER_FILE: 'Velg en annen fil',
  HB_SAME_DEVICE: 'samma enhet', // DeepL
  HB_NOT_CONNECTED: 'ikke tilkoblet',
  HB_OTHER_DEVICE: 'annen enhet (produkt-ID {backup} i backupen, {now} tilkoblet), hoppet over', // DeepL
  HB_UNIT_MISMATCH: 'Denne sikkerhetskopien inneholder temperaturer i {unit}. Endre temperaturenheten i applikasjonsinnstillingene før du gjenoppretter den.', // DeepL
  HB_CHANGED: 'Endret',
  ALL: 'Alle',
  HB_NOW: 'Nå',
  HB_BACKUP_VALUE: 'Sikkerhetskopi',
  HB_NOT_AVAILABLE: 'ikke tilgjengelig',
  HB_SENDING: 'sending', // DeepL
  HB_WRITTEN: 'skrevet',
  HB_RESTORE_NUM: 'Gjenopprett {num} {{verdi|verdier}}',
  RESTORE: 'Gjenopprett',
  HB_CONFIRM: 'Skrive {num} {{verdi|verdier}} fra {file} til EMS-enhetene?',
  HB_CONFIRM_TEXT: 'Verdiene sendes én om gangen. Hvis en enhet avviser én, sendes resten likevel.',
  HB_DONE: '{num} {{verdi|verdier}} skrevet',
  HB_DONE_FAILED: '{num} mislyktes. Enheter avviser ofte verdier på installatørnivå; systemloggen inneholder detaljene.', // DeepL
  HB_NO_CHANGES: 'Ingenting skiller seg fra sikkerhetskopien', // DeepL
  HB_DIFFERENT_DEVICE: 'annen enhet',
  SELECT_ALL: 'Velg alt', // DeepL
  DASHBOARD_EMPTY: 'Ingenting å vise ennå',
  NO_DEVICES: 'Ingen EMS-enhet funnet ennå', // DeepL
  ADD_COMMAND: 'Legg til kommando',
  EDIT_COMMAND: 'Rediger kommando',
  RUN_COMMAND: 'Kjør kommando',
  ADD_SCHEDULE: 'Legg til tidsplan', // DeepL
  EDIT_SCHEDULE: 'Rediger tidsplan', // DeepL
  ADD_ENTITY: 'Legg til enhet', // DeepL
  EDIT_ENTITY: 'Rediger Enhet', // DeepL
  ADD_ANALOG_SENSOR: 'Legg til analog sensor',
  EDIT_ANALOG_SENSOR: 'Rediger analog sensor',
  REMOVE_ANALOG_SENSOR: 'Fjern Analog Sensor',
  REMOVE_SENSOR_TEXT: 'Sensorer «{name}» fjernes umiddelbart.', // DeepL
  EDIT_TEMP_SENSOR: 'Rediger Temperatursensor',
  INSTALL_FIRMWARE: 'Installer Fastvare ({channel})',
  PARTITION: 'Partisjon',
  PULSE: 'Pulse', // DeepL
  SYS_HOME: 'Hjem', // draft, please review
  SYS_HOME_HELP: 'De viktigste tingene på et øyeblikk. Alt annet finner du i systemmenyen.', // draft, please review
  SYS_MENU: 'Systemmeny', // draft, please review
  SYS_MENU_HELP: 'Alle verdier og innstillinger for anlegget ditt, sortert etter tema.', // draft, please review
  SYS_FAV_HELP: 'Dine viktigste innstillinger. Bruk stjernen ved en verdi for å endre listen.', // draft, please review
  SYS_FAV_NONE: 'Ingen favoritter ennå. Merk verdier med stjernen.', // draft, please review
  SYS_FAV_REMOVED: 'Fjernet fra favoritter', // draft, please review
  SYS_GROUP_HEATING: 'Oppvarming', // draft, please review
  SYS_GROUP_PLANT: 'Anlegg', // draft, please review
  SYS_GROUP_OVERVIEW: 'Oversikt', // draft, please review
  SYS_LEVEL_MOVE_ALL: 'Vis bare under alle verdier', // draft, please review
  SYS_LEVEL_MOVE_NORMAL: 'Vis i normalvisning', // draft, please review
  SYS_HC: 'Varmekrets {0}', // draft, please review
  SYS_DHW: 'Varmtvann', // draft, please review
  SYS_AHS: 'Tilleggsvarme', // draft, please review
  SYS_HS: 'Varmekilde', // draft, please review
  SYS_SRC: 'Kilde', // draft, please review
  SYS_OTHER_DEVICES: 'Andre enheter', // draft, please review
  SYS_SERVICE: 'Vedlikehold og feil', // draft, please review
  SYS_ENERGY: 'Energi og driftstider', // draft, please review
  SYS_SEC_MODE: 'Drift', // draft, please review
  SYS_SEC_TEMPS: 'Temperaturer', // draft, please review
  SYS_SEC_STATUS: 'Status', // draft, please review
  SYS_SEC_ADVANCED: 'Varmekurve og installasjon', // draft, please review
  SYS_SEC_MEASURE: 'Måleverdier', // draft, please review
  SYS_SEC_OTHER: 'Andre verdier', // draft, please review
  SYS_LEVEL_NORMAL: 'Normal', // draft, please review
  SYS_NO_RESULTS: 'Ingen treff. Prøv alle verdier eller et annet søkeord.', // draft, please review
  SYS_SEARCH_PLACES: 'Sider og moduler', // draft, please review
  SYS_NOTHING_HERE: 'Ingenting å vise her på nivå Normal.', // draft, please review
  SYS_HIDDEN: 'Skjulte ekspertverdier: {num}', // draft, please review
  SYS_SEARCH_RESULTS: 'Søkeresultater', // draft, please review
  SYS_CHANGED_WAS: 'endret (var: {0})', // draft, please review
  SYS_NO_DEVICES: 'Ingen enheter funnet ennå.', // draft, please review
  SYS_ROOM_TEMP: 'Romtemperatur', // draft, please review
  SYS_WISH: 'Ønsket', // draft, please review
  SYS_CURRENT: 'Gjeldende temperatur', // draft, please review
  SYS_OUTDOOR: 'Utetemperatur', // draft, please review
  SYS_FLOW: 'Fremløp', // draft, please review
  SYS_RETURN: 'Retur', // draft, please review
  SYS_PRESSURE: 'Anleggstrykk', // draft, please review
  SYS_POWER: 'Effekt', // draft, please review
  SYS_BURNER_ON: 'Brenneren går', // draft, please review
  SYS_BURNER_OFF: 'Brenneren av', // draft, please review
  SYS_DHW_HEATING: 'Varmes opp', // draft, please review
  SYS_READY: 'Klar', // draft, please review
  SYS_ONE_TIME: 'Engangsopplading', // draft, please review
  SYS_NOTES: 'Merknader', // draft, please review
  SYS_NOTES_SUB: 'Feil og vedlikehold', // draft, please review
  SYS_LAST_FAULT: 'Siste feil', // draft, please review
  SYS_MAINT_MSG: 'Vedlikeholdsmelding', // draft, please review
  SYS_NEXT_MAINT: 'Neste vedlikehold', // draft, please review
  SYS_FAULT_LAST: 'Siste feil / melding', // draft, please review
  SYS_FAULT_CTRL: 'Siste regulatorfeil', // draft, please review
  SYS_FAULT_SERVICE: 'Servicekode', // draft, please review
  SYS_FAULT_ADD: 'Tilleggskode {0}', // draft, please review
  SYS_FAULT_RAW_ONLY: 'Ingen forklaring tilgjengelig for denne enheten.', // draft, please review
  SYS_FAULT_NONE: 'Ingen lagret feil.', // draft, please review
  SYS_FAULT_UNTIL_NOW: 'til nå', // draft, please review
  SYS_MAINTENANCE: 'Vedlikehold', // draft, please review
};

export default no;
