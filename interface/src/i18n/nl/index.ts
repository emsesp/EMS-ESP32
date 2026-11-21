import type { Translation } from '../i18n-types';

const nl: Translation = {
  LANGUAGE: 'Taal',
  RETRY: 'Opnieuw proberen',
  IS_REQUIRED: '{0} is verplicht',
  SIGN_IN: 'Inloggen',
  SIGN_IN_SUBTITLE: 'Gateway voor EMS-verwarmingssystemen',
  SIGN_OUT: 'Uitloggen',
  USERNAME: 'Gebruikersnaam',
  PASSWORD: 'Wachtwoord',
  APPLICATION_SETTINGS: 'Toepassingsinstellingen',
  AP_SETTINGS: 'Toegangspuntinstellingen',
  NETWORK_SETTINGS: 'Netwerkinstellingen',
  NTP_SETTINGS: 'NTP-instellingen',
  MQTT_SETTINGS: 'MQTT-instellingen',
  EMS_BUS_SETTINGS: 'EMS-businstellingen',
  HELP: 'Help',
  LOGGED_IN: 'Ingelogd als {name}',
  PLEASE_SIGNIN: 'Log in om verder te gaan',
  UPLOAD_SUCCESSFUL: 'Uploaden gelukt', // DeepL
  DOWNLOAD_SUCCESSFUL: 'Download succesvol', // DeepL
  INVALID_LOGIN: 'Onjuiste gebruikersnaam of wachtwoord', // DeepL
  NETWORK: 'Netwerk',
  SECURITY: 'Beveiliging',
  ONOFF_CAP: 'AAN/UIT',
  ONOFF: 'aan/uit',
  TYPE: 'Type',
  ENTITIES: 'entiteiten',
  EXPORT: 'Exporteren', // DeepL
  FAVORITES: "Favorieten",
  CLIENT_ID: 'Client-ID',
  DEVICE_ID: 'Apparaat-ID',
  PRODUCT_ID: 'Product-ID',
  ENTITY_ID: 'Entiteit-ID',
  SENSOR_ID: 'Sensor-ID',
  TYPE_ID: 'Type-ID',
  DEVICE: 'Apparaat',
  BRAND: 'Merk',
  VERSION: 'Versie',
  ENTITY_NAME: 'Entiteit',
  VALUE: 'Waarde',
  DEVICES: 'Apparaten',
  SENSORS: 'Sensoren',
  AUTOMATION: 'Automatisering',
  CONNECTIVITY: 'Connectiviteit',
  MAINTENANCE: 'Onderhoud',
  CHANGE_VALUE: 'Waarde wijzigen', // DeepL
  CANCEL: 'Annuleren',
  REMOVE_ALL: 'Alles Verwijderen', // DeepL
  APPLY_CHANGES: 'Wijzigingen toepassen ({0})', // DeepL
  UPDATE: 'Update',
  REMOVE: 'Verwijderen',
  RESET: 'Resetten',
  PROBLEM_UPDATING: 'Probleem bij het bijwerken', // DeepL
  PROBLEM_LOADING: 'Probleem bij het laden', // DeepL
  ANALOG_SENSORS: 'Analoge Sensoren',
  SETTINGS: 'Instellingen',
  SETTINGS_UPDATED: 'Instellingen bijgewerkt',
  NAME_UPDATED: 'Naam bijgewerkt',
  SENSOR_UPDATED: 'Sensor bijgewerkt',
  ANALOG_SENSOR_UPDATED: 'Analoge sensor bijgewerkt',
  OFFSET: 'Offset',
  FACTOR: 'Factor',
  FREQ: 'Frequentie',
  DUTY_CYCLE: 'Inschakelduur', // DeepL
  UNIT: 'UoM',
  STARTVALUE: 'Startwaarde',
  EDIT: 'Bewerken', // DeepL
  TEMP_SENSORS: 'Temperatuur Sensoren',
  WRITE_CMD_SENT: 'Commando verzonden', // DeepL
  EMS_BUS_WARNING: 'EMS-bus is losgekoppeld. Als deze waarschuwing na een paar seconden nog steeds verschijnt, controleer dan de instellingen en het bordprofiel', // DeepL
  CONNECTED: 'Verbonden',
  TX_ISSUES: 'Problemen met verzenden – controleer de verzendmodus', // DeepL
  DISCONNECTED: 'Geen verbinding', // DeepL
  DATA_TRAFFIC: 'Dataverkeer',
  EMS_DEVICE: 'EMS-apparaat', // DeepL
  SUCCESS: 'GESLAAGD', // DeepL
  FAIL: 'FAIL', // DeepL
  QUALITY: 'KWALITEIT',
  STATUS_NAMES: [
    'EMS Telegrammen ontvangen (Rx)',
    'EMS Leesopdrachten (Tx)',
    'EMS Schrijfopdrachten (Tx)',
    'Temperatuursensoren uitgelezen',
    'Analoge sensoren uitgelezen',
    'MQTT publicaties',
    'API calls',
    'Syslog berichten'
  ],
  NUM_DAYS: '{num} dag{{en}}',
  NUM_SECONDS: '{num} second{{en}}',
  NUM_HOURS: '{num} {{uur|uren}}',
  NUM_MINUTES: '{num} {{minuut|minuten}}',
  APPLICATION: 'Toepassing', // DeepL
  CUSTOMIZATIONS: 'Aanpassingen',
  APPLICATION_RESTARTING: 'EMS-ESP wordt opnieuw opgestart', // DeepL
  BOARD_PROFILE: 'Bordprofiel', // DeepL
  CUSTOM: 'Aangepast', // DeepL
  GPIO_OF: '{0} GPIO',
  BUTTON: 'Knop', // DeepL
  TEMPERATURE: 'Temperatuur',
  PHY_TYPE: 'Eth PHY-type', // DeepL
  DISABLED: 'uitgeschakeld', // DeepL
  TX_MODE: 'EMS Tx-modus', // DeepL
  EMS_BUS: 'EMS Bus',
  EMS_BUS_ID: 'EMS Bus-ID',
  HARDWARE: 'Hardware',
  LANGUAGE_ENTITIES: 'Taal (voor apparaatentiteiten)', // DeepL
  HIDE_LED: 'LED verbergen', // DeepL
  ENABLE_TELNET: 'Schakel de Telnet-console in', // DeepL
  ENABLE_ANALOG: 'Analoge Sensoren inschakelen', // DeepL
  CONVERT_FAHRENHEIT: 'Zet temperatuurwaarden om naar Fahrenheit', // DeepL
  READONLY: 'Schakel de alleen-lezenmodus in (blokkeert alle uitgaande EMS Tx-schrijfcommando\'s)', // DeepL
  WRITE_PROTECTION: 'Bescherm de EMS-apparaten tegen herhaaldelijk en veelvuldig schrijven', // DeepL
  WRITE_LIMIT: 'Max. aantal schrijfbewerkingen per minuut (0 = geen limiet)', // DeepL
  WRITE_LIMIT_REFUSE: 'Weiger teksten die de limiet overschrijden (anders wordt er alleen een waarschuwing geregistreerd)', // DeepL
  UNDERCLOCK_CPU: 'De CPU-snelheid verlagen', // DeepL
  REMOTE_TIMEOUT: 'Time-out bij bediening op afstand', // DeepL
  REMOTE_TIMEOUT_EN: 'Schakel bediening op afstand uit als de kamertemperatuur ontbreekt', // DeepL
  HEATINGOFF: 'Start de ketel met geforceerde verwarming uitgeschakeld', // DeepL
  MIN_DURATION: 'Wachttijd', // DeepL
  ENABLE_SHOWER_TIMER: 'Douchetimer inschakelen', // DeepL
  ENABLE_SHOWER_ALERT: 'Douchewaarschuwing inschakelen', // DeepL
  TRIGGER_TIME: 'Triggertijd', // DeepL
  COLD_SHOT_DURATION: 'Duur van de koude-injectie', // DeepL
  FORMATTING_OPTIONS: 'Opmaakopties', // DeepL
  BOOLEAN_FORMAT_DASHBOARD: 'Web in Booleaanse indeling', // DeepL
  BOOLEAN_FORMAT_API: 'Booleaanse indeling API/MQTT', // DeepL
  ENUM_FORMAT: 'Enum-formaat API/MQTT', // DeepL
  INDEX: 'Index',
  ENABLE_PARASITE: 'Schakel 1-Wire-parasietvoeding in', // DeepL
  LOG_HEX: 'Log EMS-telegrammen in hexadecimaal', // DeepL
  ENABLE_SYSLOG: 'Syslog inschakelen', // DeepL
  LOG_LEVEL: 'Logboekniveau', // DeepL
  MARK_INTERVAL: 'Markeer interval', // DeepL
  SECONDS: 'seconden',
  MINUTES: 'minuten',
  HOURS: 'uren',
  RESTART: 'Opnieuw opstarten', // DeepL
  RESTART_TEXT: 'EMS-ESP moet opnieuw worden opgestart om gewijzigde systeeminstellingen toe te passen', // DeepL
  RESTART_CONFIRM: 'Weet je zeker dat je EMS-ESP opnieuw wilt opstarten?', // DeepL
  COMMAND: 'Commando',
  CUSTOMIZATIONS_RESTART: 'Alle aanpassingen zijn verwijderd. Opnieuw opstarten…', // DeepL
  CUSTOMIZATIONS_FULL: 'Het aantal geselecteerde entiteiten is te hoog. Sla ze alsjeblieft in batches op', // DeepL
  CUSTOMIZATIONS_SAVED: 'Aanpassingen bijgewerkt', // DeepL
  CUSTOMIZATIONS_HELP_1: 'Kies een apparaat en pas de opties voor entiteiten aan', // DeepL
  CUSTOMIZATIONS_HELP_2: 'als favoriet markeren', // DeepL
  CUSTOMIZATIONS_HELP_3: 'schrijfactie uitschakelen', // DeepL
  CUSTOMIZATIONS_HELP_4: 'sluit uit van MQTT en API', // DeepL
  CUSTOMIZATIONS_HELP_5: 'verbergen voor Apparaten', // DeepL
  CUSTOMIZATIONS_HELP_6: 'uit het geheugen verwijderen', // DeepL
  SELECT_DEVICE: 'Kies een apparaat', // DeepL
  MODIFIED: 'Aangepast', // DeepL
  RESET_SELECTED: 'Geselecteerde reset', // DeepL
  OPTIONS: 'Opties',
  NAME: 'Naam',
  CUSTOMIZATIONS_RESET: 'Weet je zeker dat je alle aanpassingen wilt verwijderen?', // DeepL
  SYSTEM_INFORMATION: 'Systeeminformatie',
  HELP_INFORMATION_1: 'Kijk op de online wiki voor instructies over hoe je EMS-ESP kunt configureren', // DeepL
  HELP_INFORMATION_2: 'Kom naar onze Discord-server voor live community-chat', // DeepL
  HELP_INFORMATION_3: 'Om een functie aan te vragen of een bug te melden', // DeepL
  HELP_INFORMATION_4: 'Vermeld je systeemgegevens als je een probleem meldt, zodat je sneller antwoord krijgt', // DeepL
  UPLOAD: 'Uploaden', // DeepL
  DOWNLOAD: 'Download',
  INSTALL: 'Installeren',
  REINSTALL: 'Opnieuw installeren',
  REINSTALL_CONFIRM: 'Weet je zeker dat je versie {0} opnieuw wilt installeren?',
  FAILED: 'mislukt',
  SYSTEM: 'Systeem',
  SYSTEM_LOG: 'Systeemlogboek',
  STATUS: 'Status',
  DOWNLOAD_UPLOAD: 'Downloaden/Uploaden', // DeepL
  CLOSE: 'Sluiten',
  COMMAND_GUEST: 'Gasten kunnen deze opdracht uitvoeren', // DeepL
  TOKENS_RESET: 'Bij deze upgrade worden toegangstokens ondertekend met een nieuwe apparaatsleutel, waardoor bestaande API-toegangstokens niet meer werken. Maak voor elke integratie die de API gebruikt, zoals Home Assistant, een nieuwe API-sleutel aan en gebruik die in plaats van het oude token.', // DeepL
  API_KEYS: 'API-sleutels',
  API_KEYS_TEXT: 'Met API-sleutels kunnen integraties zoals Home Assistant en scripts de API gebruiken. Stuur de sleutel mee in de header „Authorization: Bearer <SLEUTEL>“. Lees-sleutels kunnen waarden uitlezen en Commando\'s uitvoeren die gasten ook mogen uitvoeren; lees- en schrijf-sleutels kunnen alles doen wat een beheerder via de API kan doen.', // DeepL
  API_KEY_ACCESS: 'Toegang',
  API_KEY_READ: 'Lees verder', // DeepL
  API_KEY_READ_WRITE: 'Lezen en Schrijven', // DeepL
  API_KEY_SHOW_ONCE: 'Kopieer deze sleutel nu en bewaar hem goed. Hij wordt niet meer getoond.', // DeepL
  CREATED: 'Gemaakt', // DeepL
  COPY: 'Tekst', // DeepL
  COPIED: 'Gekopieerd',
  USE: 'Gebruik',
  FACTORY_RESET: 'Fabrieksreset', // DeepL
  SYSTEM_FACTORY_TEXT_DIALOG: 'Weet je zeker dat je EMS-ESP wilt terugzetten naar de fabrieksinstellingen?', // DeepL
  STABLE: 'Stabiel', // DeepL
  DEVELOPMENT: 'Ontwikkeling', // DeepL
  UPTIME: 'Systeemup-time', // DeepL
  LAST_RESTART: 'Laatste herstart',
  POWER_AND_RESET: 'Aan/uit en Reset', // DeepL
  FREE_MEMORY: 'Vrij geheugen',
  BUFFER_SIZE: 'Maximale buffergrootte', // DeepL
  COMPACT: 'Beknopt', // DeepL
  SUMMARY: 'Samenvatting', // DeepL
  SHOW_MORE: '{n} meer weergeven', // DeepL
  SHOW_LESS: 'Minder weergeven', // DeepL
  COREDUMP: 'Crash-dump', // DeepL
  DOWNLOAD_SYSTEM_BACKUP: 'Systeemback-up',
  ERROR: 'Onverwachte fout, probeer het nog eens', // DeepL
  TIME_SET: 'Ingestelde tijd', // DeepL
  MANAGE_USERS: 'Gebruikers beheren', // DeepL
  USER_WARNING: 'Je moet minstens één beheerdersgebruiker hebben geconfigureerd', // DeepL
  ADD: 'Toevoegen',
  TELNET_ENABLED: 'De Telnet-console is ingeschakeld. Telnet is niet versleuteld, dus schakel het uit in de applicatie-instellingen als je het niet gebruikt.', // DeepL
  TOO_MANY_LOGINS: 'Te veel mislukte inlogpogingen, probeer het over een paar minuten nog eens', // DeepL
  PASSWORD_KEEP: 'Laat dit veld leeg om het huidige wachtwoord te behouden', // DeepL
  SETUP_TITLE: 'Stel je wachtwoorden in',
  SETUP_TEXT: 'Dit apparaat gebruikt nog steeds de standaardwachtwoorden. Stel je eigen wachtwoord in voordat je verdergaat.', // DeepL
  SETUP_ADMIN_PASSWORD: 'Nieuw wachtwoord voor {name}',
  SETUP_AP_PASSWORD: 'Nieuw wachtwoord voor het toegangspunt', // DeepL
  SETUP_AP_REQUIRED: 'Er is nog geen wifi-netwerk ingesteld, dus het toegangspunt is de toegang. 8-63 tekens, wordt gebruikt zodra het toegangspunt de volgende keer opstart.', // DeepL
  SETUP_AP_OPTIONAL: 'Aanbevolen: 8-63 tekens, wordt gebruikt zodra het toegangspunt de volgende keer opstart. Laat het leeg om de huidige instelling te behouden.', // DeepL
  SETUP_AP_DEFAULT: 'Het toegangspunt gebruikt nog steeds het fabriekswachtwoord. Stel je eigen wachtwoord in via de instellingen van het toegangspunt.', // DeepL
  SETUP_VIEW_USER: 'Optioneel: voeg een gastgebruiker toe die alleen kan kijken.', // DeepL
  NETWORK_CORS_ORIGIN_HELP: 'Het webadres waarmee je de API vanuit een browser kunt aanroepen, bijvoorbeeld http://homeassistant.local:8123', // DeepL
  NETWORK_CORS_WILDCARD: 'Met * kan elke website die in een browser op je netwerk is geopend de API aanroepen. Stel in plaats daarvan het exacte adres in.', // DeepL
  NETWORK_ALLOWED_HOSTS: 'Toegestane hostnamen',
  NETWORK_ALLOWED_HOSTS_HELP: 'Dit apparaat kan ook via een browser worden geopend onder verschillende namen, bijvoorbeeld ems-esp.fritz.box of een reverse proxy-naam. Het IP-adres, de hostnaam en hostnaam.local werken altijd', // DeepL
  NOT_ENABLED: 'Niet ingeschakeld', // DeepL
  MQTT_ERRORS: 'MQTT-fouten',
  DISCONNECT_REASON: 'Reden voor het verbreken van de verbinding', // DeepL
  MQTT_BASE_TOPIC: 'Basisonderwerp', // DeepL
  LAST_DISCONNECT: 'Laatste verbinding verbroken', // DeepL
  CONNECTED_SINCE: 'Verbonden sinds',
  ENABLE_MQTT: 'Schakel MQTT in', // DeepL
  BROKER: 'Broker',
  BASE_TOPIC: 'Basis', // DeepL
  OPTIONAL: 'optioneel',
  MQTT_FORMAT: 'Onderwerp/payload-formaat', // DeepL
  MQTT_NEST_1: 'Gegroepeerd onder één onderwerp', // DeepL
  MQTT_NEST_2: 'Per onderwerp', // DeepL
  MQTT_RESPONSE: 'Publiceer de uitvoer van commando\'s naar een `response`-topic', // DeepL
  MQTT_PUBLISH_TEXT_1: 'Publiceer onderwerpen met één waarde bij wijzigingen', // DeepL
  MQTT_PUBLISH_TEXT_2: 'Publiceer naar command-topics (ioBroker)', // DeepL
  MQTT_PUBLISH_TEXT_3: 'Schakel MQTT-detectie in', // DeepL
  MQTT_PUBLISH_TEXT_4: 'Voorvoegsel voor de discovery-onderwerpen', // DeepL
  MQTT_PUBLISH_TEXT_5: 'Type ontdekking', // DeepL
  MQTT_INPUT_NUMBER_FORMAT: 'Notatie van invoergetallen', // DeepL
  MQTT_PUBLISH_INTERVALS: 'Publicatie-intervallen', // DeepL
  MQTT_INT_BOILER: 'Cv-ketels en warmtepompen', // DeepL
  MQTT_INT_THERMOSTATS: 'Thermostaten',
  MQTT_INT_SOLAR: 'Zonnepanelen', // DeepL
  MQTT_INT_MIXER: 'Mixermodules', // DeepL
  MQTT_INT_WATER: 'Water Modules', // DeepL
  MQTT_QUEUE: 'MQTT-wachtrij',
  DEFAULT: 'Standaard',
  MQTT_ENTITY_FORMAT: 'Indeling van entiteits-ID’s', // DeepL
  MQTT_ENTITY_FORMAT_0: 'Eén exemplaar, lange naam (v3.4)', // DeepL
  MQTT_ENTITY_FORMAT_1: 'Eén exemplaar, korte naam', // DeepL
  MQTT_ENTITY_FORMAT_2: 'Meerdere exemplaren, korte naam', // DeepL
  MQTT_CLEAN_SESSION: 'Zet een schone sessie in', // DeepL
  MQTT_RETAIN_FLAG: 'Zet altijd de retentievlag aan', // DeepL
  MQTT_COMMANDS: 'Commando\'s die via MQTT worden ontvangen', // DeepL
  MQTT_COMMANDS_0: 'Uit, alleen waarden lezen',
  MQTT_COMMANDS_1: 'Waarden wijzigen en commando\'s uitvoeren', // DeepL
  MQTT_COMMANDS_2: 'Alles, ook systeemopdrachten', // DeepL
  INACTIVE: 'Inactief',
  ACTIVE: 'Actief',
  UNKNOWN: 'Onbekend',
  SET_TIME: 'Tijd Instellen',
  SET_TIME_TEXT: 'Voer hieronder de lokale datum en tijd in om de tijd in te stellen', // DeepL
  LOCAL_TIME: 'Lokale tijd', // DeepL
  UTC_TIME: 'UTC-tijd',
  ENABLE_NTP: 'Schakel NTP in', // DeepL
  NTP_SERVER: 'NTP-server', // DeepL
  TIME_ZONE: 'Tijdzone',
  ACCESS_POINT: 'Toegangspunt', // DeepL
  AP_PROVIDE: 'Access Point inschakelen', // DeepL
  AP_PROVIDE_TEXT_2: 'Als er geen connectiviteit wordt gevonden', // DeepL
  AP_PROVIDE_TEXT_3: 'Nooit', // DeepL
  AP_PREFERRED_CHANNEL: 'Voorkeurskanaal',
  AP_HIDE_SSID: 'SSID verbergen',
  AP_CLIENTS: 'AP-clients', // DeepL
  AP_MAX_CLIENTS: 'Max. aantal clients', // DeepL
  AP_LOCAL_IP: 'Lokaal IP-adres', // DeepL
  IDLE: 'Inactief',
  SCAN_AGAIN: 'Nog een keer scannen', // DeepL
  NETWORK_NO_WIFI: 'Er zijn geen wifi-netwerken gevonden', // DeepL
  NETWORK_BLANK_BSSID: 'Laat leeg om alleen SSID te gebruiken', // DeepL
  TX_POWER: 'Zendvermogen', // DeepL
  HOSTNAME: 'Hostnaam',
  NETWORK_DISABLE_SLEEP: 'Schakel de wifi-slaapstand uit', // DeepL
  NETWORK_LOW_BAND: 'Gebruik minder wifi-bandbreedte', // DeepL
  NETWORK_USE_DNS: 'Schakel de mDNS-service in', // DeepL
  NETWORK_ENABLE_CORS: 'Schakel CORS in', // DeepL
  NETWORK_CORS_ORIGIN: 'CORS-oorsprong', // DeepL
  NETWORK_FIXED_IP: 'Gebruik een vast IP-adres', // DeepL
  NETWORK_GATEWAY: 'Gateway',
  NETWORK_SUBNET: 'Subnetmasker',
  NETWORK_DNS: 'DNS-servers', // DeepL
  ADDRESS_OF: '{0}-adres', // DeepL
  ADMINISTRATOR: 'Beheerder',
  GUEST: 'Gast', // DeepL
  NEW: 'Nieuw', // DeepL
  NEW_ENTITY_NAME: 'Nieuwe entiteitnaam',
  MIN: 'min',
  MAX: 'max',
  BLOCK_NAVIGATE_1: 'Je hebt onopgeslagen wijzigingen', // DeepL
  BLOCK_NAVIGATE_2: 'Als je naar een andere pagina gaat, gaan je niet-opgeslagen wijzigingen verloren. Weet je zeker dat je deze pagina wilt verlaten?', // DeepL
  UPLOAD_ABORTED: 'Uploaden mislukt', // DeepL
  BLOCK_BUSY_1: 'Nog in ontwikkeling', // DeepL
  BLOCK_BUSY_2: 'Als je deze pagina nu verlaat, wordt het proces gestopt. Weet je zeker dat je wilt vertrekken?', // DeepL
  DISCARD: 'Verwijderen', // DeepL
  COMMANDS_UPDATED: 'Commando\'s bijgewerkt', // DeepL
  V_REQUIRED: 'Vereist', // DeepL
  V_BETWEEN: 'Moet tussen {min} en {max} liggen',
  V_MAX_CHARS: 'Moet uit {max} tekens of minder bestaan', // DeepL
  V_CHARS: 'Moet tussen {min} en {max} tekens lang zijn', // DeepL
  V_IN_USE: 'Al in gebruik',
  V_IP: 'Moet een IP-adres zijn',
  V_HOSTNAME: 'Moet een geldige hostnaam zijn',
  V_IP_OR_HOSTNAME: 'Moet een IP-adres of hostnaam zijn',
  V_NAME: 'Maximaal {max} tekens: letters, cijfers of _',
  V_USERNAME: 'Maximaal {max} tekens: letters, cijfers, _ of .',
  V_DAY: 'Kies minstens één dag',
  V_HEX: 'Moet een hexadecimaal getal zijn',
  V_FINGERPRINT: 'Moet een SHA-1-vingerafdruk zijn (40 hexadecimale cijfers)',
  V_EXAMPLE: 'Moet er zo uitzien: {example}', // DeepL
  V_NOT_DEFAULT: 'Kies een ander wachtwoord',
  ZERO_DISABLES: '0 schakelt het uit',
  MINUS_ONE_DISABLES: '-1 schakelt het uit',
  LED_TYPE: 'Type LED', // DeepL
  PHY_POWER: 'PHY-voeding',
  PHY_CLOCK: 'PHY-klok',
  AUTO: 'Auto',
  PORT: 'Poort',
  TIMEOUT: 'Time-out',
  HOST: 'Host',
  SMTP_SERVER: 'SMTP-server',
  KEEP_ALIVE: 'Blijf actief', // DeepL
  MQTT_REFRESH: 'MQTT vernieuwen',
  MQTT_REFRESHED: 'MQTT vernieuwd',
  MQTT_R_USER: 'Verbinding verbroken door gebruiker', // DeepL
  MQTT_R_TCP: 'TCP-verbinding verbroken', // DeepL
  MQTT_R_PROTOCOL: 'Protocolversie wordt niet geaccepteerd', // DeepL
  MQTT_R_ID: 'Client-ID afgewezen', // DeepL
  MQTT_R_SERVER: 'Server niet beschikbaar',
  MQTT_R_CREDENTIALS: 'Onjuiste inloggegevens', // DeepL
  MQTT_R_AUTH: 'Geen toestemming', // DeepL
  MQTT_R_FINGERPRINT: 'De TLS-vingerafdruk klopt niet', // DeepL
  GPIO_INVALID: 'Ongeldige GPIO-instellingen',
  CONNECTED_VIA: 'Verbonden via {0}',
  NETWORK_NOT_FOUND: 'Netwerk niet gevonden',
  CONNECTION_FAILED: 'Verbinding mislukt',
  CONNECTION_LOST: 'Verbinding verbroken',
  AP_LINGERING: 'Wachten tot er niets te doen is', // DeepL
  NONE: 'Geen',
  LOG_NO_CONNECTION: 'Geen connectiviteit met de logdienst', // DeepL
  CHECK_SYSTEM_LOG: 'Kijk in het systeemlogboek voor mogelijke oorzaken', // DeepL
  VALUE_TYPE: 'Type waarde', // DeepL
  DEFAULT_VALUE: 'Standaardwaarde',
  DEFAULT_NAME: 'Standaardnaam',
  ALWAYS_OFF: 'Altijd uit',
  ALWAYS_ON: 'Altijd aan',
  NO_DATA: 'Nog geen favoriete entiteiten. Markeer ze in {customizations}, of bekijk alle entiteiten in {devices}.', // DeepL
  STAY: 'Blijf', // DeepL
  LEAVE: 'Laat', // DeepL
  MENU: 'Menu',
  MORE: 'Meer',
  SHOW_PASSWORD: 'Wachtwoord weergeven', // DeepL
  STATUS_RUNNING: 'EMS-ESP draait', // DeepL
  NUM_ERRORS: '{num} fout{{en}}',
  NUM_WARNINGS: '{num} waarschuwing{{en}}',
  HEAP: 'Heap',
  METER_USED: '{used} / {total} {unit} gebruikt',
  MAX_BLOCK: 'Max. {num} KB per blok', // DeepL
  MAX_BLOCK_HELP: 'Max block is het grootste stuk heapgeheugen dat in één keer kan worden toegewezen. Onder de 40 KB kunnen de webinterface en MQTT uitvallen.', // DeepL
  FLASH_CHIP: 'Flash-chip', // DeepL
  FILE_SYSTEM: 'Bestandssysteem',
  DETAILS: 'Details',
  RECONNECTS: 'Opnieuw verbinden', // DeepL
  CONNECTS: 'Verbindt', // DeepL
  ACTIVITY_HELP: 'Een paar Rx-fouten zijn normaal. Veel Tx-fouten betekenen meestal dat de Tx-modus niet geschikt is voor jouw ketel.', // DeepL
  PAUSE: 'Pauze',
  RESUME: 'CV', // DeepL
  LOG_PAUSED: 'Gepauzeerd, nieuwe regels achtergehouden: {num}', // DeepL
  FILTER_LINES: 'Filterregels', // DeepL
  SCHEDULER: 'Scheduler',
  SCHEDULER_HELP_1: 'Automatiseer commando\'s door hieronder geplande gebeurtenissen toe te voegen', // DeepL
  SCHEDULER_HELP_2: 'bij het opstarten',
  SCHEDULE: 'Schema', // DeepL
  TIME: 'Tijd',
  TIMER: 'Timer',
  ONCHANGE: 'Bij wijzigingen', // DeepL
  CONDITION: 'Voorwaarde',
  SCHEDULE_UPDATED: 'Schema bijgewerkt',
  CUSTOM_ENTITIES: 'Aangepaste entiteiten', // DeepL
  ENTITIES_HELP_1: 'Definieer aangepaste EMS-entiteiten of dynamische gebruikersvariabelen', // DeepL
  ENTITIES_UPDATED: 'Bijgewerkte entiteiten', // DeepL
  WRITEABLE: 'Schrijfbaar', // DeepL
  SHOWING: 'Weergave', // DeepL
  SEARCH: 'Zoeken', // DeepL
  CERT: 'CA-certificaat (PEM)',
  ENABLE_TLS: 'Schakel TLS in', // DeepL
  TLS_CHECK: 'Controleer de broker met', // DeepL
  TLS_CHECK_CA: 'CA-certificaat',
  TLS_CHECK_FINGERPRINT: 'Certificaatvingerafdruk (SHA-1)', // DeepL
  TLS_CHECK_NONE: 'Niets, niet aangevinkt', // DeepL
  TLS_NOT_CHECKED: 'De verbinding is versleuteld, maar de broker wordt niet gecontroleerd, dus een ander apparaat op het netwerk zou zich ervoor kunnen uitgeven', // DeepL
  ON: 'Op', // DeepL
  OFF: 'Uit',
  POLARITY: 'Polariteit',
  ACTIVEHIGH: 'Actief hoog', // DeepL
  ACTIVELOW: 'Actief laag', // DeepL
  UNCHANGED: 'Ongewijzigd',
  SECURITY_1: 'Gebruikers toevoegen of verwijderen',
  DOWNLOAD_UPLOAD_1: 'Instellingen downloaden en uploaden', // DeepL
  MODULES: 'Module',
  MODULES_1: 'Externe modules in- of uitschakelen', // DeepL
  NETWORK_DESC: 'Wi-Fi, ethernet en hostnaam',
  AP_DESC: 'Eigen hotspot voor installatie en noodtoegang',
  MQTT_DESC: 'Broker, topics en discovery',
  APPLICATION_DESC: 'EMS-bus, board, sensoren en opmaak',
  NTP_DESC: 'Tijdserver en tijdzone',
  FIRMWARE_DESC: 'Versie, updates en herstel',
  MODULES_UPDATED: 'Modules bijgewerkt', // DeepL
  MODULES_NONE: 'Er zijn geen externe modules gedetecteerd', // DeepL
  RENAME: 'Hernoemen',
  ENABLE_MODBUS: 'Modbus inschakelen', // DeepL
  VIEW_LOG: 'Bekijk het logboek om problemen op te sporen', // DeepL
  SERVICES: 'Diensten', // DeepL
  ALLVALUES: 'Alle waarden', // DeepL
  WAIT_FIRMWARE: 'De firmware wordt geüpload en geïnstalleerd', // DeepL
  INSTALL_VERSION: 'Dit installeert versie {0}',
  UPDATE_AVAILABLE: 'update beschikbaar',
  LATEST_VERSION: 'Je gebruikt de nieuwste firmwareversie ({0})',
  PLEASE_WAIT: 'Even geduld a.u.b.', // DeepL
  RESTARTING_PRE: 'Opstarten', // DeepL
  RESTARTING_POST: 'Voorbereiding',
  AUTO_SCROLL: 'Automatisch scrollen', // DeepL
  DASHBOARD: 'Dashboard',
  DEVELOPER_MODE: 'Ontwikkelaarsmodus',
  DISABLE_RESET: 'Fabrieksinstellingen resetten uitschakelen', // DeepL
  AUTO_FW_CHECK: 'Controleer automatisch op firmware-updates', // DeepL
  BYTES: 'Bytes',
  BITMASK: 'Bitmasker', // DeepL
  DUPLICATE: 'Duplicaat',
  DASHBOARD_1: 'Alle actieve EMS-entiteiten die als favoriet zijn gemarkeerd, worden hieronder weergegeven, plus alle aangepaste entiteiten, schema’s en externe sensoren.', // DeepL
  NO_GPIO: 'Er is geen beschikbare GPIO gevonden',
  PLATFORM: 'Platform',
  RELEASE_TYPE: 'Release Type', // DeepL
  FIRMWARE_VERSION_INFO: 'Firmwareversie-informatie', // DeepL
  USER_PROFILE: 'Gebruikersprofiel',
  ONLINE_HELP: 'online hulp',
  UPGRADE_IMPORTANT_MESSAGES: 'Belangrijke Upgradeberichten', // DeepL
  UPGRADE_IMPORTANT_MESSAGES_2: 'Je gaat upgraden naar een nieuwe hoofdversie. Het is aan te raden om eerst een back-up van het systeem te maken. Kijk ook even in de online help of er belangrijke wijzigingen zijn.', // DeepL
  UPGRADE_IMPORTANT_MESSAGES_3: 'Versie 3.10 bevat ingrijpende beveiligingswijzigingen. Maak eerst een systeemback-up: na de upgrade worden de wachtwoorden gehashte opgeslagen en kunnen oudere versies ze niet meer lezen, dus om terug te gaan heb je deze back-up nodig. Elke API-aanroep heeft dan een API-sleutel nodig, dus integraties zoals Home Assistant moeten worden bijgewerkt. Lees de ingrijpende wijzigingen in de online help voordat je verdergaat.', // DeepL
  WARNING_SYSTEM_BACKUP: 'Hiermee maak je een back-up van je volledige systeemconfiguratie en instellingen. Wachtwoorden en sleutels worden weggelaten, tenzij je ze hieronder opgeeft; bij het terugzetten van de back-up blijven de wachtwoorden die al op het Apparaat staan behouden. Commando\'s worden altijd meegenomen en kunnen inloggegevens bevatten, dus wees voorzichtig met het delen ervan. Wil je doorgaan?', // DeepL
  BACKUP_INCLUDE_SECRETS: 'Vermeld ook wachtwoorden en sleutels (nodig om op een nieuw apparaat te herstellen)', // DeepL
  TEST_EMAIL_SUCCESSFUL: 'Testmail verzonden',
  SYSTEM_NAME: 'Systeemnaam',
  COMMANDS: 'Commando\'s',
  COMMANDS_HELP_1: 'Definieer hieronder herbruikbare, benoemde commando’s. Deze kun je uitvoeren via de console, API/MQTT, of ernaar verwijzen vanuit de Scheduler', // DeepL
  COMMAND_EXECUTED: 'Commando uitgevoerd',
  RUN: 'Uitvoeren', // DeepL
  INTERNET_CONNECTION_CHECK: 'Online kijken welke versies er beschikbaar zijn', // DeepL
  UPLOAD_BACKUP_TEXT: 'Klik om te selecteren, of sleep hier een back-upbestand (.json) naar toe', // DeepL
  UPLOAD_FIRMWARE_TEXT: 'Klik om te selecteren, of sleep hier een firmwarebestand (.bin/.md5) naar toe', // DeepL
  UPLOAD_MD5_RECEIVED: 'Digest ontvangen, upload nu het .bin-bestand met de firmware', // DeepL
  UPLOAD_MD5_MATCHED: 'MD5-vergelijking, firmware installeren…', // DeepL
  UPLOAD_NOT_OFFICIAL: 'Dit is geen officiële EMS-ESP-build. Installeer alleen firmware die je vertrouwt', // DeepL
  UPLOAD_NOT_OFFICIAL_HELP: 'Over officiële en aangepaste versies', // DeepL
  UPLOAD_INSTALL_ANYWAY: 'Installeer het toch maar', // DeepL
  OFFICIAL_BUILD: 'Officiële Build',
  CUSTOM_BUILD: 'Aangepaste Build', // DeepL
  USING_CUSTOM_FIRMWARE: 'U gebruikt een aangepaste firmware', // DeepL
  PASSWORD_CONFIRM_TITLE: 'Voer je wachtwoord nogmaals in', // DeepL
  PASSWORD_CONFIRM_TEXT: 'Voer voor je eigen veiligheid je wachtwoord in voordat je deze wijziging doorvoert. Je hoeft het de komende 5 minuten niet nog een keer in te voeren.', // DeepL
  PASSWORD_CONFIRM_BUTTON: 'Bevestig', // DeepL
  PASSWORD_WRONG: 'Verkeerd wachtwoord', // DeepL
  PASSWORD_NOT_CONFIRMED: 'Geannuleerd, het wachtwoord is niet ingevoerd',
  RECOVERY_APP: 'Recovery-app', // DeepL
  RECOVERY_NOT_INSTALLED: 'Niet geïnstalleerd',
  RECOVERY_INSTALLED: 'Herstelfirmware geïnstalleerd',
  RESTART_INTO_RECOVERY: 'Opnieuw opstarten in de Recovery-app', // DeepL
  RECOVERY_NOT_FOUND: 'Recovery-app niet online gevonden', // DeepL
  RECOVERY_START_FAILED: 'De Recovery-app kan niet worden gestart; deze is mogelijk verouderd of beschadigd. Installeer eerst de nieuwste versie', // DeepL
  PARTITION_START_FAILED: 'Deze firmware kan niet worden gestart; mogelijk is deze beschadigd', // DeepL
  INSTALLED_VERSIONS: 'Geïnstalleerde versies',
  EMSESP_APP: 'EMS-ESP-app', // DeepL
  NO_UPDATES_AVAILABLE: 'geen updates beschikbaar',
  SWITCH_TO_DEV: 'Schakel over naar dev {0}', // DeepL
  SWITCH_TO_STABLE: 'Schakel over naar de stabiele versie {0}', // DeepL
  SAVE: 'Opslaan',
  NEEDS_RESTART: 'moet opnieuw worden opgestart', // DeepL
  CHANGED_SETTINGS: '{num} gewijzigde instelling{{en}}',
  SAVED_RESTART: 'Opgeslagen. Start EMS-ESP opnieuw op om de nieuwe instellingen te gebruiken.', // DeepL
  RESTART_NOW: 'Nu opnieuw opstarten', // DeepL
  COPY_FAILED: 'Kon niet kopiëren, selecteer de tekst en kopieer hem zelf', // DeepL
  DOCUMENTATION: 'Documentatie',
  COMMUNITY_CHAT: 'Community-chat',
  REPORT_ISSUE: 'Meld een bug of vraag een functie aan', // DeepL
  GET_HELP: 'Hulp zoeken', // DeepL
  REPORT_PROBLEM: 'Een probleem melden',
  REPORT_STEP_1: 'Download de systeeminformatie',
  REPORT_STEP_1_TEXT: 'versie, printplaat, status van de EMS-bus, aangesloten apparaten en instellingen, zonder wachtwoorden', // DeepL
  REPORT_STEP_2: 'Sla het recente logboek op',
  REPORT_STEP_2_TEXT: 'Stel het logniveau eerst in op DEBUG als het probleem bij een apparaat ligt', // DeepL
  REPORT_STEP_3: 'Maak een issue aan of stel je vraag op Discord', // DeepL
  REPORT_STEP_3_TEXT: 'Voeg beide bestanden toe, of plak de korte samenvatting hieronder in een chat', // DeepL
  NEW_ISSUE: 'Nieuw probleem', // DeepL
  SHORT_SUMMARY: 'Korte samenvatting',
  LICENSE_TEXT: 'open source onder de LGPL-3.0-licentie',
  ADMIN_ACCESS_TEXT: 'volledige toegang tot instellingen, apparaten en de API',
  GUEST_ACCESS_TEXT: 'kun je waarden bekijken en de Commando\'s uitvoeren die gasten kunnen uitvoeren', // DeepL
  LANGUAGE_BROWSER_ONLY: 'dit geldt alleen voor deze browser', // DeepL
  LANGUAGE_ENTITIES_HINT: 'De taal voor de namen van apparaatentiteiten in MQTT en de API wordt ingesteld in', // DeepL
  ADMINISTRATION: 'Beheer',
  API_KEYS_SHORT: 'toetsen voor Home Assistant en scripts', // DeepL
  ADDRESSES: 'Adressen',
  SEARCH_SETTINGS: 'Zoekinstellingen', // DeepL
  NO_SETTINGS_MATCH: 'Er zijn geen instellingen die aan je zoekopdracht voldoen', // DeepL
  GENERAL: 'Algemeen',
  BOARD: 'Forum', // DeepL
  ADVANCED: 'Geavanceerd',
  EMS_BUS_DESC: 'Hoe EMS-ESP met je verwarmingssysteem communiceert.', // DeepL
  BOARD_DESC: 'De pinnen komen uit het bordprofiel. Om ze zelf in te stellen, schakel je de ontwikkelaarsmodus in en kies je ‘Aangepast’.', // DeepL
  FORMAT_DESC: 'Hoe waarden eruitzien in de webinterface en in MQTT/API-payloads.', // DeepL
  BOARD_PINS: 'Pins uit het profiel', // DeepL
  ENABLE_EMAIL: 'Schakel e-mailmeldingen in', // DeepL
  SEND_TEST_EMAIL: 'Stuur een testmail', // DeepL
  EMAIL_FROM: 'Van',
  EMAIL_TO: 'Naar', // DeepL
  EMAIL_SUBJECT: 'Onderwerp',
  WIFI_SCAN: 'Wi-Fi-scan', // DeepL
  SCAN: 'Scannen', // DeepL
  NETWORKS_NEARBY: 'Netwerken in de buurt',
  SIGNAL: 'Signaal',
  CHANNEL: 'Kanaal',
  WIFI_OPEN: 'open',
  WIFI_SAVED: 'opgeslagen',
  NETWORK_ETHERNET_INFO: 'Nu verbonden via Ethernet ({ip}). Wi-Fi wordt alleen gebruikt als er geen kabel is.', // DeepL
  HOSTNAME_ACCESS: 'Hostnaam en toegang',
  IP_ADDRESS: 'IP-adres',
  NETWORK_ETH_10MBIT: 'Forceer 10 Mbit half-duplex', // DeepL
  NETWORK_ETH_10MBIT_HELP: 'alleen voor probleemschakelaars of lange kabels', // DeepL
  NETWORK_DISABLE_SLEEP_HELP: 'betrouwbaarder, verbruikt iets meer stroom', // DeepL
  NETWORK_MDNS_HELP: 'Je kunt het bereiken via {host}.local met mDNS', // DeepL
  NETWORK_FIXED_IP_HELP: 'anders wordt DHCP gebruikt',
  NETWORK_CORS_HELP: 'alleen nodig als een andere webpagina de API aanroept',
  NTP_TIME_ON_DEVICE: 'tijd op EMS-ESP',
  NTP_TIME_SERVER: 'Tijdserver',
  NTP_THERMOSTAT_CLOCK: 'Thermostaatklok', // DeepL
  NTP_THERMOSTAT_SYNC: 'Stel de klok van de thermostaat in', // DeepL
  NTP_THERMOSTAT_SYNC_HELP: 'stelt de datum en tijd in op de EMS-thermostaat', // DeepL
  NTP_THERMOSTAT_SYNC_1: 'Aan, EMS-ESP-tijdzone', // DeepL
  NTP_THERMOSTAT_SYNC_2: 'Aan, eigen tijdzone',
  NTP_THERMOSTAT_TZ: 'Tijdzone van de thermostaat',
  MQTT_PREVIEW: 'Wat er wordt gepubliceerd', // DeepL
  MQTT_PREVIEW_CMD: 'wordt als een commando geaccepteerd', // DeepL
  MQTT_PREVIEW_RESPONSE: 'uitvoer van commando\'s',
  MQTT_BEHAVIOUR: 'Gedrag',
  MQTT_TOPICS: 'Onderwerpen en payloads', // DeepL
  MQTT_DISCOVERY: 'Ontdekking', // DeepL
  MQTT_INTERVALS_HELP: 'Aantal seconden tussen publicaties, 0 = publiceren bij elke wijziging.', // DeepL
  MQTT_SINGLE_NO_DISCOVERY: 'niet beschikbaar bij discovery op', // DeepL
  MQTT_DISCOVERY_NO_SINGLE: 'niet beschikbaar bij onderwerpen met één waarde op', // DeepL
  MQTT_DISCOVERY_HELP: 'maakt de entiteiten automatisch aan in Home Assistant of Domoticz',
  MQTT_ENTITY_FORMAT_WARN: 'Als je dit verandert, worden alle entiteiten in Home Assistant hernoemd', // DeepL
  USERS: 'Gebruikers',
  USERS_TEXT: 'Beheerders kunnen alles aanpassen. Gasten kunnen rondkijken en commando\'s uitvoeren die voor gasten zijn gemarkeerd.', // DeepL
  ADD_USER: 'Gebruiker toevoegen',
  EDIT_USER: 'Gebruiker bewerken',
  ROLE: 'Rol',
  ROLE_ADMIN: 'admin', // DeepL
  ROLE_GUEST: 'gast',
  ADMIN: 'Beheer', // DeepL
  YOU: 'jij',
  NEW_PASSWORD: 'Nieuw wachtwoord',
  UNSAVED_CHANGES: '{num} niet-opgeslagen wijziging{{en}}',
  CREATE_KEY: 'Sleutel Maken', // DeepL
  NEW_API_KEY: 'Nieuwe API-sleutel',
  NO_API_KEYS: 'Nog geen API-sleutels',
  API_KEYS_LIMIT: 'Je kunt maximaal {num} toetsen hebben.', // DeepL
  REVOKE: 'Intrekken',
  REVOKE_KEY: 'API-sleutel Intrekken',
  REVOKE_KEY_TEXT: 'Alles waarin „{name}“ voorkomt, werkt meteen niet meer.', // DeepL
  MODULES_TEXT: 'Optionele functies die in deze firmware zijn ingebouwd. Je kunt ze hier in- of uitschakelen.', // DeepL
  BY_AUTHOR: 'door {name}',
  LICENSE_KEY: 'Licentiesleutel',
  MODULE_ACTIVATED: 'geactiveerd',
  MODULE_PENDING: 'in afwachting van activering', // DeepL
  ENABLED: 'Ingeschakeld',
  UNDO: 'Ongedaan maken',
  FW_UPDATES: 'Updates',
  FW_VERSION_AVAILABLE: '{version} is beschikbaar',
  FW_RELEASE_NOTES: 'Release-opmerkingen', // DeepL
  FW_UPDATES_OFFLINE: 'Er kon niet online worden gecontroleerd of er nieuwe versies beschikbaar zijn. EMS-ESP heeft hiervoor een internetverbinding nodig.', // DeepL
  FW_RESTART_RESET: 'Opnieuw Opstarten en Reset', // DeepL
  FW_RECOVERY_TEXT: 'Opnieuw opstarten via de Recovery-app? Deze maakt verbinding met je netwerk en start ook een eigen toegangspunt, waar je firmware kunt uploaden.', // DeepL
  FW_SIZE: 'Grootte',
  FW_INSTALL_DATE: 'Geïnstalleerd op',
  FW_UPLOAD_FAILED: 'Uploaden mislukt: verkeerd bestandstype of firmware voor een ander bord', // DeepL
  FILE_TYPE_INVALID: 'Dit bestandstype wordt niet ondersteund, kies een {types}-bestand',
  WHO_CAN_RUN: 'Wie kan het gebruiken', // DeepL
  CMD_NONE: 'Geen commando\'s gevonden',
  CMD_NONE_MATCH: 'Er zijn geen resultaten gevonden voor je zoekopdracht', // DeepL
  SCHEDULE_NONE: 'Er zijn geen schema’s gevonden', // DeepL
  SCHEDULE_NONE_MATCH: 'Er zijn geen schema’s die aan je zoekopdracht voldoen', // DeepL
  CE_NONE: 'Geen entiteiten gevonden',
  CE_ALL: 'Alles', // DeepL
  CE_STORAGE: 'Opslag',
  CE_SOURCE: 'Bron',
  CE_EMS_SOURCE: 'apparaat {device} · telegram {type} · offset {offset}',
  CE_RAM_TEXT: 'blijft behouden tot de herstart', // DeepL
  CE_NVS_TEXT: 'blijft behouden bij het opnieuw opstarten', // DeepL
  CE_EXCLUDED: 'niet in API/MQTT',
  SEARCH_SENSORS: 'Zoek op naam, ID of GPIO', // DeepL
  NO_SENSORS: 'Geen Sensoren gevonden', // DeepL
  NO_SENSORS_MATCH: 'Er zijn geen Sensoren die aan je zoekopdracht voldoen', // DeepL
  TEMP_SENSORS_HELP: 'Automatisch gevonden. Klik op een Sensor om hem een andere naam te geven of een offset in te stellen', // DeepL
  FREE_GPIOS: 'Vrije GPIO\'s',
  SYSTEM_SENSOR: 'Systeemsensor',
  MASK_FAVORITE: 'Favorieten', // DeepL
  MASK_READONLY: 'Schrijf uitgeschakeld', // DeepL
  MASK_NO_API: 'Geen MQTT/API',
  MASK_HIDDEN: 'Verborgen',
  MASK_REMOVED: 'Verwijderd',
  FILTER_ALL: 'Alles', // DeepL
  NUM_SELECTED: 'Geselecteerd: {num}',
  ALL_SHOWN: 'Alles weergeven: {num}', // DeepL
  BULK_HINT: 'De knoppen wijzigen de aangevinkte rijen, of alle weergegeven rijen als er geen zijn aangevinkt.', // DeepL
  CLEAR_SELECTION: 'Duidelijke selectie', // DeepL
  NO_ENTITIES_MATCH: 'Er zijn geen overeenkomende entiteiten', // DeepL
  LIMIT_HELP: 'laat leeg voor de ingebouwde limiet', // DeepL
  MIN_ABOVE_MAX: 'Het minimum mag niet hoger zijn dan het maximum', // DeepL
  RENAME_DEVICE: 'Apparaat Hernoemen',
  WEEKLY: 'Wekelijks',
  EVERY_DAY: 'Elke dag',
  SCHEDULER_NO_COMMANDS: 'Voeg eerst een commando toe op de pagina Commando\'s',
  DU_SETTINGS: 'EMS-ESP-instellingen',
  DU_SETTINGS_TEXT: 'Back-up van de EMS-ESP-instellingen: netwerk, MQTT, Aanpassingen, Scheduler, aangepaste entiteiten en modules. Als je een instellingen- of back-upbestand uploadt, wordt EMS-ESP opnieuw opgestart.', // DeepL
  HB_TITLE: 'Verwarmingsinstellingen',
  HB_TEXT: 'Slaat de huidige waarden van je ketel, thermostaat en andere EMS-apparaten op, zodat je ze later weer kunt terugzetten, bijvoorbeeld na het vervangen van een apparaat. Alleen-lezen entiteiten en commando\'s worden nooit gewijzigd door een herstelbewerking.', // DeepL
  HB_BACKUP: 'Verwarmingsinstellingen Back-uppen', // DeepL
  HB_RESTORE: 'Verwarmingsinstellingen herstellen',
  DATA_EXPORT: 'Gegevens exporteren', // DeepL
  DATA_EXPORT_TEXT: 'Voor ondersteuning en analyse. Deze bestanden kunnen niet opnieuw worden geüpload.', // DeepL
  NUM_VALUES: '{num} waarde{{n}}',
  HB_NOTHING: 'niets om te back-uppen',
  FILE_NAME: 'Bestandsnaam',
  HB_NOTE: 'Opmerking (optioneel, wordt opnieuw weergegeven wanneer je de instellingen herstelt)', // DeepL
  HB_INCOMPLETE: 'Waarden die nog niet van de bus zijn gelezen, worden weggelaten. Wacht na een herstart een paar minuten op een volledige back-up.', // DeepL
  HB_RESTORE_TEXT: 'Kies een back-up voor je verwarmingsinstellingen. Er wordt niets opgeslagen totdat je de wijzigingen hebt gecontroleerd en bevestigd.', // DeepL
  HB_DROP: 'Klik om te selecteren, of sleep hier een back-upbestand van je verwarmingssysteem (.json) naar toe', // DeepL
  HB_INVALID: 'Dit is geen back-up van de verwarmingsinstellingen', // DeepL
  HB_OTHER_FILE: 'Kies een ander bestand', // DeepL
  HB_SAME_DEVICE: 'hetzelfde apparaat', // DeepL
  HB_NOT_CONNECTED: 'niet aangesloten', // DeepL
  HB_OTHER_DEVICE: 'ander apparaat (product-ID {backup} in de back-up, {now} aangesloten), overgeslagen', // DeepL
  HB_UNIT_MISMATCH: 'Deze back-up bevat temperaturen in {unit}. Pas de temperatuureenheid aan in de instellingen van de app voordat je de back-up terugzet.', // DeepL
  HB_CHANGED: 'Gewijzigd',
  ALL: 'Alles', // DeepL
  HB_NOW: 'Nu',
  HB_BACKUP_VALUE: 'Back-up',
  HB_NOT_AVAILABLE: 'niet beschikbaar',
  HB_SENDING: 'verzenden',
  HB_WRITTEN: 'geschreven',
  HB_RESTORE_NUM: '{num} waarde{{n}} herstellen',
  RESTORE: 'Terugzetten', // DeepL
  HB_CONFIRM: '{num} waarde{{n}} uit {file} naar de EMS-apparaten schrijven?',
  HB_CONFIRM_TEXT: 'Waarden worden één voor één verzonden. Als een apparaat er één afwijst, worden de rest toch verzonden.', // DeepL
  HB_DONE: '{num} waarde{{n}} geschreven',
  HB_DONE_FAILED: '{num} is mislukt. Apparaten weigeren vaak waarden op installateursniveau; in het systeemlogboek vind je de details.', // DeepL
  HB_NO_CHANGES: 'Er is niets anders dan in de back-up', // DeepL
  HB_DIFFERENT_DEVICE: 'ander apparaat',
  SELECT_ALL: 'Alles selecteren',
  DASHBOARD_EMPTY: 'Er is nog niets te zien', // DeepL
  NO_DEVICES: 'Er zijn nog geen EMS-apparaten gevonden', // DeepL
  ADD_COMMAND: 'Commando toevoegen',
  EDIT_COMMAND: 'Bewerk commando', // DeepL
  RUN_COMMAND: 'Commando uitvoeren',
  ADD_SCHEDULE: 'Schema toevoegen',
  EDIT_SCHEDULE: 'Schema bewerken', // DeepL
  ADD_ENTITY: 'Entiteit toevoegen',
  EDIT_ENTITY: 'Entiteit Bewerken', // DeepL
  ADD_ANALOG_SENSOR: 'Analoge Sensor toevoegen', // DeepL
  EDIT_ANALOG_SENSOR: 'Analoge Sensoren bewerken', // DeepL
  REMOVE_ANALOG_SENSOR: 'Analoge Sensor Verwijderen', // DeepL
  REMOVE_SENSOR_TEXT: 'De Sensor „{name}“ wordt meteen verwijderd.', // DeepL
  EDIT_TEMP_SENSOR: 'Temperatuursensor Bewerken', // DeepL
  INSTALL_FIRMWARE: 'Firmware Installeren ({channel})',
  PARTITION: 'Sectie', // DeepL
  PULSE: 'Pulse', // DeepL
};

export default nl;
