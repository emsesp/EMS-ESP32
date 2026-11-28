import type { Translation } from '../i18n-types';

const sk: Translation = {
  LANGUAGE: 'Jazyk',
  RETRY: 'Skúste to znova', // DeepL
  IS_REQUIRED: '{0} je povinné', // DeepL
  SIGN_IN: 'Prihlásiť sa',
  SIGN_IN_SUBTITLE: 'Brána pre vykurovacie systémy EMS',
  SIGN_OUT: 'Odhlásiť sa',
  USERNAME: 'Používateľské meno', // DeepL
  PASSWORD: 'Heslo',
  APPLICATION_SETTINGS: 'Nastavenia aplikácie',
  AP_SETTINGS: 'Nastavenia prístupového bodu',
  NETWORK_SETTINGS: 'Nastavenia siete',
  NTP_SETTINGS: 'Nastavenia NTP',
  MQTT_SETTINGS: 'Nastavenia MQTT',
  EMS_BUS_SETTINGS: 'Nastavenia zbernice EMS',
  HELP: 'Pomoc',
  LOGGED_IN: 'Prihlásený ako {name}',
  PLEASE_SIGNIN: 'Prihláste sa, prosím, aby ste mohli pokračovať', // DeepL
  UPLOAD_SUCCESSFUL: 'Nahratie bolo úspešné', // DeepL
  DOWNLOAD_SUCCESSFUL: 'Stiahnuť úspešné', // DeepL
  INVALID_LOGIN: 'Nesprávne používateľské meno alebo heslo', // DeepL
  NETWORK: 'Sieť',
  SECURITY: 'Bezpečnosť', // DeepL
  ONOFF_CAP: 'ZAP/VYPN', // DeepL
  ONOFF: 'zapnuté/vypnuté', // DeepL
  TYPE: 'Typ',
  ENTITIES: 'entity',
  EXPORT: 'Export',
  FAVORITES: "Obľúbené",
  CLIENT_ID: 'ID klienta',
  DEVICE_ID: 'ID zariadenia',
  PRODUCT_ID: 'ID produktu',
  ENTITY_ID: 'ID entity',
  SENSOR_ID: 'ID snímača',
  TYPE_ID: 'ID typu',
  DEVICE: 'Zariadenie',
  BRAND: 'Značka',
  VERSION: 'Verzia',
  ENTITY_NAME: 'Názov entity',
  VALUE: 'Hodnota',
  DEVICES: 'Zariadenia',
  SENSORS: 'Snímače',
  AUTOMATION: 'Automatizácia',
  CONNECTIVITY: 'Pripojenie',
  MAINTENANCE: 'Údržba',
  CHANGE_VALUE: 'Zmeniť hodnotu', // DeepL
  CANCEL: 'Zrušiť',
  REMOVE_ALL: 'Odstrániť Všetko', // DeepL
  APPLY_CHANGES: 'Použiť zmeny ({0})', // DeepL
  UPDATE: 'Aktualizácia', // DeepL
  REMOVE: 'Odstrániť',
  RESET: 'Resetovať', // DeepL
  PROBLEM_UPDATING: 'Problém s aktualizáciou',
  PROBLEM_LOADING: 'Problém s načítaním',
  ANALOG_SENSORS: 'Analógové snímače',
  SETTINGS: 'Nastavenia',
  SETTINGS_UPDATED: 'Nastavenia aktualizované',
  NAME_UPDATED: 'Názov aktualizovaný',
  SENSOR_UPDATED: 'Snímač aktualizovaný',
  ANALOG_SENSOR_UPDATED: 'Analógový snímač aktualizovaný',
  OFFSET: 'Posun', // DeepL
  FACTOR: 'Faktor',
  FREQ: 'Frekvencia',
  DUTY_CYCLE: 'Pracovný cyklus',
  UNIT: 'UoM',
  STARTVALUE: 'Počiatočná hodnota',
  EDIT: 'Upraviť', // DeepL
  TEMP_SENSORS: 'Snímače teploty',
  WRITE_CMD_SENT: 'Odoslaný príkaz', // DeepL
  EMS_BUS_WARNING: 'Zbernica EMS je odpojená. Ak toto varovanie pretrváva aj po niekoľkých sekundách, skontrolujte prosím nastavenia a profil dosky.', // DeepL
  CONNECTED: 'Pripojené',
  TX_ISSUES: 'Problémy s Tx – skontrolujte režim Tx', // DeepL
  DISCONNECTED: 'Odpojené',
  DATA_TRAFFIC: 'Dátový tok', // DeepL
  EMS_DEVICE: 'Zariadenie EMS', // DeepL
  SUCCESS: 'ÚSPECH', // DeepL
  FAIL: 'NEÚSPECH', // DeepL
  QUALITY: 'KVALITA',
  STATUS_NAMES: [
    'EMS Telegramy prijaté (Rx)',
    'EMS Čítania (Tx)',
    'EMS Zápisy (Tx)',
    'Čítanie snímačov teploty',
    'Čítanie analógových snímačov',
    'MQTT Publikovanie',
    'Externé API volania',
    'Syslog správy'
  ],
  NUM_DAYS: '{num} d{{ní|eň|ní|ní|ní|ní}}',
  NUM_SECONDS: '{num} sek{{únd|unda|undy|undy|únd|únd}}',
  NUM_HOURS: '{num} hod{{ín|ina|iny|iny|ín|ín}}',
  NUM_MINUTES: '{num} minú{{t|ta|ty|ty|t|t}}',
  APPLICATION: 'Aplikácia',
  CUSTOMIZATIONS: 'Prispôsobenia',
  APPLICATION_RESTARTING: 'EMS-ESP sa reštartuje',
  BOARD_PROFILE: 'Profil dosky',
  CUSTOM: 'Vlastné',
  GPIO_OF: '{0} GPIO',
  BUTTON: 'Tlačidlo',
  TEMPERATURE: 'Teplota',
  PHY_TYPE: 'Typ Eth PHY', // DeepL
  DISABLED: 'vypnuté', // DeepL
  TX_MODE: 'Režim EMS Tx', // DeepL
  EMS_BUS: 'EMS Bus',
  EMS_BUS_ID: 'ID EMS Bus',
  HARDWARE: 'Hardvér', // DeepL
  LANGUAGE_ENTITIES: 'Jazyk (pre entity zariadení)', // DeepL
  HIDE_LED: 'Skryť LED',
  ENABLE_TELNET: 'Zapnúť konzolu Telnet', // DeepL
  ENABLE_ANALOG: 'Aktivujte analógové snímače', // DeepL
  CONVERT_FAHRENHEIT: 'Preveďte hodnoty teploty na stupne Fahrenheita', // DeepL
  READONLY: 'Zapnite režim len na čítanie (blokuje všetky odchádzajúce príkazy zápisu EMS Tx)', // DeepL
  WRITE_PROTECTION: 'Chráňte zariadenia EMS pred opakovaným a častým zapisovaním', // DeepL
  WRITE_LIMIT: 'Maximálny počet zápisov za minútu (0 = bez obmedzenia)', // DeepL
  WRITE_LIMIT_REFUSE: 'Zakážte prepisovanie nad limit (inak sa zaznamená len varovanie)', // DeepL
  UNDERCLOCK_CPU: 'Znížte frekvenciu procesora', // DeepL
  REMOTE_TIMEOUT: 'Časový limit vzdialeného pripojenia', // DeepL
  REMOTE_TIMEOUT_EN: 'Vypnúť diaľkové ovládanie pri chýbajúcej teplote v miestnosti', // DeepL
  HEATINGOFF: 'Spustiť kotol s vypnutým núteným kúrením', // DeepL
  MIN_DURATION: 'Čas čakania', // DeepL
  ENABLE_SHOWER_TIMER: 'Zapnúť časovač sprchy', // DeepL
  ENABLE_SHOWER_ALERT: 'Zapnúť upozornenie na sprchovanie', // DeepL
  TRIGGER_TIME: 'Čas spustenia',
  COLD_SHOT_DURATION: 'Trvanie studenej fázy', // DeepL
  FORMATTING_OPTIONS: 'Možnosti formátovania',
  BOOLEAN_FORMAT_DASHBOARD: 'Web v booleovskom formáte', // DeepL
  BOOLEAN_FORMAT_API: 'Booleovský formát API/MQTT', // DeepL
  ENUM_FORMAT: 'Formát Enum API/MQTT', // DeepL
  INDEX: 'Zoznam', // DeepL
  ENABLE_PARASITE: 'Aktivovať napájanie z vedľajších zdrojov 1-Wire', // DeepL
  LOG_HEX: 'Zaznamenávajte telegramy EMS v hexadecimálnom formáte', // DeepL
  ENABLE_SYSLOG: 'Zapnúť Syslog', // DeepL
  LOG_LEVEL: 'Úroveň protokolovania', // DeepL
  MARK_INTERVAL: 'Označte interval', // DeepL
  SECONDS: 'sekúnd', // DeepL
  MINUTES: 'minúty',
  HOURS: 'hodiny',
  RESTART: 'Reštartovať', // DeepL
  RESTART_TEXT: 'Aby sa zmenené nastavenia systému uplatnili, je potrebné reštartovať EMS-ESP', // DeepL
  RESTART_CONFIRM: 'Naozaj chcete reštartovať EMS-ESP?', // DeepL
  COMMAND: 'Príkaz',
  CUSTOMIZATIONS_RESTART: 'Všetky prispôsobenia boli odstránené. Spúšťam sa znova…', // DeepL
  CUSTOMIZATIONS_FULL: 'Počet vybraných entit prekročil limit. Uložte ich prosím po častiach.', // DeepL
  CUSTOMIZATIONS_SAVED: 'Aktualizované prispôsobenia', // DeepL
  CUSTOMIZATIONS_HELP_1: 'Vyberte zariadenie a prispôsobte si možnosti entít', // DeepL
  CUSTOMIZATIONS_HELP_2: 'označiť ako Obľúbené', // DeepL
  CUSTOMIZATIONS_HELP_3: 'zakázať akciu zápisu',
  CUSTOMIZATIONS_HELP_4: 'vylúčiť z MQTT a API',
  CUSTOMIZATIONS_HELP_5: 'skryť zo Zariadení',
  CUSTOMIZATIONS_HELP_6: 'vymazať z pamäte', // DeepL
  SELECT_DEVICE: 'Vyberte zariadenie', // DeepL
  MODIFIED: 'Upravené',
  RESET_SELECTED: 'Vynulovať výber', // DeepL
  OPTIONS: 'Možnosti',
  NAME: 'Názov',
  CUSTOMIZATIONS_RESET: 'Naozaj chcete odstrániť všetky vlastné prispôsobenia?', // DeepL
  SYSTEM_INFORMATION: 'Informácie o systéme',
  HELP_INFORMATION_1: 'Navštívte online wiki, kde nájdete pokyny na konfiguráciu EMS-ESP',
  HELP_INFORMATION_2: 'Ak sa chcete zapojiť do živého komunitného chatu, pripojte sa k nášmu serveru Discord', // DeepL
  HELP_INFORMATION_3: 'Ak chcete požiadať o novú funkciu alebo nahlásiť chybu', // DeepL
  HELP_INFORMATION_4: 'Pri hlásení problému uveďte informácie o vašom systéme, aby sme mohli rýchlejšie reagovať', // DeepL
  HELP_INFORMATION_5: 'EMS-ESP vytvárajú dobrovoľníci. Prepitné na Ko-fi pomáha projektu pokračovať',
  UPLOAD: 'Nahrať',
  DOWNLOAD: 'Stiahnuť',
  INSTALL: 'Inštalácia', // DeepL
  REINSTALL: 'Preinštalovať', // DeepL
  REINSTALL_CONFIRM: 'Naozaj chcete preinštalovať verziu {0}?', // DeepL
  FAILED: 'neúspešné', // DeepL
  SYSTEM: 'Systém',
  SYSTEM_LOG: 'Systémový protokol',
  STATUS: 'Stav',
  DOWNLOAD_UPLOAD: 'Stiahnuť/Nahrať',
  CLOSE: 'Zatvoriť',
  COMMAND_GUEST: 'Hostia môžu spustiť tento príkaz',
  TOKENS_RESET: 'Táto aktualizácia podpisuje prístupové tokeny novým kľúčom zariadenia, takže existujúce prístupové tokeny API už nefungujú. Vytvorte kľúč API pre každú integráciu, ktorá používa API, napríklad Home Assistant, a používajte ho namiesto starého tokenu.', // DeepL
  API_KEYS: 'Kľúče API',
  API_KEYS_TEXT: 'Kľúče API umožňujú integráciám, ako je Home Assistant, a skriptom využívať API. Kľúč odošlite v hlavičke „Authorization: Bearer <KEY>“. Kľúče typu „Read“ môžu čítať hodnoty a spúšťať príkazy, ktoré môžu spúšťať hostia, kľúče typu „Read“ a „Write“ môžu robiť všetko, čo môže robiť správca prostredníctvom API.', // DeepL
  API_KEY_ACCESS: 'Prístup',
  API_KEY_READ: 'Prečítajte si', // DeepL
  API_KEY_READ_WRITE: 'Čítanie a Zapisovanie', // DeepL
  API_KEY_SHOW_ONCE: 'Tento kľúč si teraz skopírujte a uschovajte na bezpečnom mieste. Už sa viac nezobrazí.', // DeepL
  CREATED: 'Vytvorené',
  COPY: 'Text', // DeepL
  COPIED: 'Skopírované',
  USE: 'Používajte', // DeepL
  FACTORY_RESET: 'Továrenský Reset', // DeepL
  SYSTEM_FACTORY_TEXT_DIALOG: 'Naozaj chcete obnoviť továrenské nastavenia EMS-ESP?', // DeepL
  STABLE: 'Stabilné', // DeepL
  DEVELOPMENT: 'Vývoj', // DeepL
  UPTIME: 'Doba prevádzky systému', // DeepL
  LAST_RESTART: 'Posledný reštart',
  POWER_AND_RESET: 'Napájanie a resetovanie', // DeepL
  FREE_MEMORY: 'Voľná pamäť',
  BUFFER_SIZE: 'Maximálna veľkosť vyrovnávacej pamäte', // DeepL
  COMPACT: 'Stručne', // DeepL
  SUMMARY: 'Zhrnutie', // DeepL
  SHOW_MORE: 'Zobraziť ďalších {n}',
  SHOW_LESS: 'Zobraziť menej',
  COREDUMP: 'Crash Dump', // DeepL
  DOWNLOAD_SYSTEM_BACKUP: 'Záloha Systému', // DeepL
  ERROR: 'Došlo k neočakávanej chybe, skúste to prosím znova', // DeepL
  TIME_SET: 'Nastavený čas',
  MANAGE_USERS: 'Správa používateľov',
  USER_WARNING: 'Musíte mať nakonfigurovaného aspoň jedného používateľa s oprávneniami správcu', // DeepL
  ADD: 'Pridať',
  TELNET_ENABLED: 'Konzola Telnet je zapnutá. Telnet nie je šifrovaný, preto ho v nastaveniach aplikácie vypnite, ak ho nepoužívate.', // DeepL
  TOO_MANY_LOGINS: 'Príliš veľa neúspešných pokusov o prihlásenie, skúste to znovu o pár minút', // DeepL
  PASSWORD_KEEP: 'Nechajte prázdne, ak chcete zachovať aktuálne heslo', // DeepL
  SETUP_TITLE: 'Nastavte si heslá',
  SETUP_TEXT: 'Toto zariadenie stále používa továrenské heslá. Pred pokračovaním si nastavte vlastné.', // DeepL
  SETUP_ADMIN_PASSWORD: 'Nové heslo pre {name}',
  SETUP_AP_PASSWORD: 'Nové heslo pre prístupový bod', // DeepL
  SETUP_AP_REQUIRED: 'Zatiaľ nie je nastavená žiadna sieť Wi-Fi, takže prístupovým bodom je vstup. 8–63 znakov, použije sa pri najbližšom spustení prístupového bodu.', // DeepL
  SETUP_AP_OPTIONAL: 'Odporúčané: 8–63 znakov, použije sa pri najbližšom spustení prístupového bodu. Ak necháte pole prázdne, zachová sa aktuálny text.', // DeepL
  SETUP_AP_DEFAULT: 'Prístupový bod stále používa továrenské heslo. Nastavte si vlastné v nastaveniach prístupového bodu.', // DeepL
  SETUP_VIEW_USER: 'Voliteľné: pridajte hosťovského používateľa s právami len na prehliadanie.', // DeepL
  NETWORK_CORS_ORIGIN_HELP: 'Webová adresa umožňujúca volanie API z prehliadača, napríklad http://homeassistant.local:8123', // DeepL
  NETWORK_CORS_WILDCARD: 'Pomocou * môže API volať akákoľvek webová stránka otvorená v prehliadači vo vašej sieti. Namiesto toho nastavte presnú adresu.', // DeepL
  NETWORK_ALLOWED_HOSTS: 'Povolené názvy hostiteľov', // DeepL
  NETWORK_ALLOWED_HOSTS_HELP: 'Toto zariadenie je možné otvoriť v prehliadači aj pod inými názvami, napríklad EMS-ESP.fritz.box alebo názov reverzného proxy servera. Jeho IP adresa, názov hostiteľa a názov hostiteľa.local vždy fungujú', // DeepL
  NOT_ENABLED: 'Nie je povolené',
  MQTT_ERRORS: 'Chyby MQTT',
  DISCONNECT_REASON: 'Dôvod odpojenia',
  MQTT_BASE_TOPIC: 'Základná téma',
  LAST_DISCONNECT: 'Posledné odpojenie',
  CONNECTED_SINCE: 'Pripojené od',
  ENABLE_MQTT: 'Aktivujte MQTT', // DeepL
  BROKER: 'Broker',
  BASE_TOPIC: 'Základ', // DeepL
  OPTIONAL: 'voliteľné',
  MQTT_FORMAT: 'Formát témy/užitočného obsahu', // DeepL
  MQTT_NEST_1: 'Zahrnuté v jednej téme', // DeepL
  MQTT_NEST_2: 'Ako jednotlivé témy',
  MQTT_RESPONSE: 'Zverejňujte výstup príkazu do témy `response`', // DeepL
  MQTT_PUBLISH_TEXT_1: 'Zverejňujte témy s jednou hodnotou pri zmene', // DeepL
  MQTT_PUBLISH_TEXT_2: 'Zverejňujte do tém príkazov (ioBroker)', // DeepL
  MQTT_PUBLISH_TEXT_3: 'Povoliť vyhľadávanie MQTT', // DeepL
  MQTT_PUBLISH_TEXT_4: 'Predpona pre témy vyhľadávania', // DeepL
  MQTT_PUBLISH_TEXT_5: 'Typ objavu', // DeepL
  MQTT_INPUT_NUMBER_FORMAT: 'Formát zadávania čísel', // DeepL
  MQTT_PUBLISH_INTERVALS: 'Intervaly zverejňovania',
  MQTT_INT_BOILER: 'Kotly a tepelné čerpadlá',
  MQTT_INT_THERMOSTATS: 'Termostaty',
  MQTT_INT_SOLAR: 'Solárne moduly',
  MQTT_INT_MIXER: 'Moduly zmiešavačov', // DeepL
  MQTT_INT_WATER: 'Vodné moduly', // DeepL
  MQTT_QUEUE: 'Fronta MQTT',
  DEFAULT: 'Predvolené nastavenie', // DeepL
  MQTT_ENTITY_FORMAT: 'Formát identifikátora entity', // DeepL
  MQTT_ENTITY_FORMAT_0: 'Jedna inštancia, dlhý názov (v3.4)',
  MQTT_ENTITY_FORMAT_1: 'Jedna inštancia, krátky názov',
  MQTT_ENTITY_FORMAT_2: 'Viac inštancií, krátky názov', // DeepL
  MQTT_CLEAN_SESSION: 'Nastaviť čistú reláciu', // DeepL
  MQTT_RETAIN_FLAG: 'Vždy nastavte príznak zachovania', // DeepL
  MQTT_COMMANDS: 'Príkazy prijaté cez MQTT',
  MQTT_COMMANDS_0: 'Vypnuté, iba čítanie hodnôt',
  MQTT_COMMANDS_1: 'Zmena hodnôt a spúšťanie príkazov', // DeepL
  MQTT_COMMANDS_2: 'Všetko, vrátane systémových príkazov', // DeepL
  INACTIVE: 'Neaktívne',
  ACTIVE: 'Aktívne',
  UNKNOWN: 'Neznáme',
  SET_TIME: 'Nastaviť Čas',
  SET_TIME_TEXT: 'Do nižšie uvedeného poľa zadajte miestny dátum a čas, aby ste nastavili čas', // DeepL
  LOCAL_TIME: 'Miestny čas', // DeepL
  UTC_TIME: 'Čas UTC', // DeepL
  ENABLE_NTP: 'Zapnúť NTP', // DeepL
  NTP_SERVER: 'Server NTP', // DeepL
  TIME_ZONE: 'Časové pásmo', // DeepL
  ACCESS_POINT: 'Prístupový bod',
  AP_PROVIDE: 'Aktivovať prístupový bod', // DeepL
  AP_PROVIDE_TEXT_2: 'Keď sa nepodarilo nájsť sieťové pripojenie', // DeepL
  AP_PROVIDE_TEXT_3: 'Nikdy', // DeepL
  AP_PREFERRED_CHANNEL: 'Preferovaný kanál',
  AP_HIDE_SSID: 'Skryť SSID',
  AP_CLIENTS: 'Klienti AP', // DeepL
  AP_MAX_CLIENTS: 'Maximálny počet klientov', // DeepL
  AP_LOCAL_IP: 'Lokálna IP adresa', // DeepL
  IDLE: 'Nečinnosť', // DeepL
  SCAN_AGAIN: 'Skontrolujte to znovu', // DeepL
  NETWORK_NO_WIFI: 'Nenašli sa žiadne Wi-Fi siete', // DeepL
  NETWORK_BLANK_BSSID: 'nechajte prázdne, ak chcete používať iba SSID', // DeepL
  TX_POWER: 'Výkon vysielača', // DeepL
  HOSTNAME: 'Názov hostiteľa', // DeepL
  NETWORK_DISABLE_SLEEP: 'Vypnite režim spánku Wi-Fi', // DeepL
  NETWORK_LOW_BAND: 'Používajte nižšiu šírku pásma Wi-Fi', // DeepL
  NETWORK_USE_DNS: 'Aktivujte službu mDNS', // DeepL
  NETWORK_ENABLE_CORS: 'Povoliť CORS',
  NETWORK_CORS_ORIGIN: 'Pôvod CORS', // DeepL
  NETWORK_FIXED_IP: 'Používajte pevnú IP adresu', // DeepL
  NETWORK_GATEWAY: 'Brána',
  NETWORK_SUBNET: 'Maska podsiete',
  NETWORK_DNS: 'DNS servery',
  ADDRESS_OF: 'Adresa {0}', // DeepL
  ADMINISTRATOR: 'Správca', // DeepL
  GUEST: 'Hosť',
  NEW: 'Nové', // DeepL
  NEW_ENTITY_NAME: 'Nový názov entity',
  MIN: 'min',
  MAX: 'max',
  BLOCK_NAVIGATE_1: 'Máte neuložené zmeny',
  BLOCK_NAVIGATE_2: 'Ak prejdete na inú stránku, vaše neuložené zmeny sa stratia. Naozaj chcete opustiť túto stránku?', // DeepL
  UPLOAD_ABORTED: 'Nahrávanie bolo prerušené', // DeepL
  BLOCK_BUSY_1: 'Stále v procese', // DeepL
  BLOCK_BUSY_2: 'Opustením tejto stránky sa proces zastaví. Naozaj chcete stránku opustiť?', // DeepL
  DISCARD: 'Zrušiť', // DeepL
  COMMANDS_UPDATED: 'Aktualizované príkazy', // DeepL
  V_REQUIRED: 'Povinné',
  V_BETWEEN: 'Dĺžka musí byť v rozmedzí {min} a {max}', // DeepL
  V_MAX_CHARS: 'Musí mať najviac {max} znakov', // DeepL
  V_CHARS: 'Musí mať {min} – {max} znakov', // DeepL
  V_IN_USE: 'Už sa používa',
  V_IP: 'Musí to byť IP adresa', // DeepL
  V_HOSTNAME: 'Musí to byť platný názov hostiteľa', // DeepL
  V_IP_OR_HOSTNAME: 'Musí to byť IP adresa alebo názov hostiteľa', // DeepL
  V_NAME: 'Maximálne {max} znakov: písmená, číslice alebo _', // DeepL
  V_USERNAME: 'Maximálne {max} znakov: písmená, číslice, _ alebo .', // DeepL
  V_DAY: 'Vyberte si aspoň jeden deň', // DeepL
  V_HEX: 'Musí to byť šesťnástkové číslo', // DeepL
  V_FINGERPRINT: 'Musí ísť o SHA-1 odtlačok (40 hexadecimálnych číslic)', // DeepL
  V_EXAMPLE: 'Musia vyzerať takto: {example}', // DeepL
  V_NOT_DEFAULT: 'Zvoľte iné heslo',
  ZERO_DISABLES: '0 to vypne', // DeepL
  MINUS_ONE_DISABLES: '-1 vypne zariadenie', // DeepL
  LED_TYPE: 'Typ LED',
  PHY_POWER: 'Napájanie PHY',
  PHY_CLOCK: 'Hodiny PHY',
  AUTO: 'Auto',
  PORT: 'Port',
  TIMEOUT: 'Časový limit',
  HOST: 'Hostiteľ',
  SMTP_SERVER: 'SMTP server', // DeepL
  KEEP_ALIVE: 'Udržujte ich aktuálne', // DeepL
  MQTT_REFRESH: 'Obnoviť MQTT',
  MQTT_REFRESHED: 'MQTT aktualizované', // DeepL
  MQTT_R_USER: 'Odpojené používateľom',
  MQTT_R_TCP: 'TCP spojenie bolo uzavreté', // DeepL
  MQTT_R_PROTOCOL: 'Verzia protokolu nebola akceptovaná', // DeepL
  MQTT_R_ID: 'ID klienta bolo odmietnuté', // DeepL
  MQTT_R_SERVER: 'Server nie je dostupný', // DeepL
  MQTT_R_CREDENTIALS: 'Nesprávne zadané prihlasovacie údaje', // DeepL
  MQTT_R_AUTH: 'Neautorizované',
  MQTT_R_FINGERPRINT: 'Otlačok TLS sa nezhoduje', // DeepL
  GPIO_INVALID: 'Neplatné nastavenia GPIO', // DeepL
  CONNECTED_VIA: 'Pripojené cez {0}',
  NETWORK_NOT_FOUND: 'Sieť nebola nájdená', // DeepL
  CONNECTION_FAILED: 'Pripojenie sa nepodarilo', // DeepL
  CONNECTION_LOST: 'Stratené pripojenie', // DeepL
  AP_LINGERING: 'Čakanie, kým sa systém dostane do nečinnosti', // DeepL
  NONE: 'Žiadne',
  LOG_NO_CONNECTION: 'Žiadne pripojenie so službou protokolovania', // DeepL
  CHECK_SYSTEM_LOG: 'Skontrolujte systémový protokol, či nenájdete možné príčiny', // DeepL
  VALUE_TYPE: 'Typ hodnoty',
  DEFAULT_VALUE: 'Predvolená hodnota',
  DEFAULT_NAME: 'Predvolený názov',
  ALWAYS_OFF: 'Vždy vypnuté',
  ALWAYS_ON: 'Vždy zapnuté',
  NO_DATA: 'Zatiaľ nie sú žiadne obľúbené entity. Označte ich v sekcii {customizations} alebo si prezrite všetky entity v sekcii {devices}.', // DeepL
  STAY: 'Zostaňte', // DeepL
  LEAVE: 'Nechajte', // DeepL
  MENU: 'Ponuka',
  MORE: 'Viac',
  SHOW_PASSWORD: 'Zobraziť heslo',
  STATUS_RUNNING: 'EMS-ESP beží',
  NUM_ERRORS: '{num} {{chýb|chyba|chyby|chyby|chýb|chýb}}',
  NUM_WARNINGS: '{num} upozorneni{{í|e|a|a|í|í}}',
  HEAP: 'Heap',
  METER_USED: '{used} / {total} {unit} spotrebované', // DeepL
  MAX_BLOCK: 'Maximálny blok {num} KB', // DeepL
  MAX_BLOCK_HELP: 'Max block je najväčšia časť pamäte, ktorú je možné naraz alokovať. Pri hodnote nižšej ako 40 KB môže dôjsť k zlyhaniu webového rozhrania a protokolu MQTT.', // DeepL
  FLASH_CHIP: 'Čip Flash', // DeepL
  FILE_SYSTEM: 'Súborový systém',
  DETAILS: 'Podrobnosti',
  RECONNECTS: 'Opätovné pripojenie', // DeepL
  CONNECTS: 'Pripojenia',
  ACTIVITY_HELP: 'Niekoľko neúspešných pokusov o prijímanie (Rx) je normálne. Mnoho neúspešných pokusov o odosielanie (Tx) zvyčajne znamená, že je pre váš kotol nastavený nesprávny režim odosielania (Tx).', // DeepL
  PAUSE: 'Pauza', // DeepL
  RESUME: 'Zhrnutie', // DeepL
  LOG_PAUSED: 'Pozastavené, nové riadky zadržané: {num}', // DeepL
  FILTER_LINES: 'Filtračné riadky', // DeepL
  SCHEDULER: 'Plánovač',
  SCHEDULER_HELP_1: 'Automatizujte príkazy pridaním naplánovaných udalostí nižšie',
  SCHEDULER_HELP_2: 'pri spustení', // DeepL
  SCHEDULE: 'Rozvrh', // DeepL
  TIME: 'Čas',
  TIMER: 'Časovač',
  ONCHANGE: 'Pri zmene',
  CONDITION: 'Podmienka',
  SCHEDULE_UPDATED: 'Aktualizovaný harmonogram', // DeepL
  CUSTOM_ENTITIES: 'Vlastné entity',
  ENTITIES_HELP_1: 'Definujte vlastné entity EMS alebo dynamické užívateľské premenné', // DeepL
  ENTITIES_UPDATED: 'Aktualizované entity',
  WRITEABLE: 'Možnosť zápisu', // DeepL
  SHOWING: 'Zobrazenie',
  SEARCH: 'Vyhľadávanie', // DeepL
  CERT: 'Certifikát CA (PEM)',
  ENABLE_TLS: 'Zapnúť TLS', // DeepL
  TLS_CHECK: 'Skontrolujte sprostredkovateľa pomocou', // DeepL
  TLS_CHECK_CA: 'Certifikát CA',
  TLS_CHECK_FINGERPRINT: 'Otlačok certifikátu (SHA-1)', // DeepL
  TLS_CHECK_NONE: 'Nič, nezaškrtnuté', // DeepL
  TLS_NOT_CHECKED: 'Pripojenie je šifrované, ale sprostredkovateľ nie je overovaný, takže iné zariadenie v sieti by sa mohlo vydávať za neho', // DeepL
  TLS_CA_NEEDS_TIME: 'Kontrola dátumu certifikátu vyžaduje správny čas zariadenia — zapnite NTP, alebo nastavte čas ručne', // DeepL
  ON: 'Na', // DeepL
  OFF: 'Vypnúť', // DeepL
  POLARITY: 'Polarita',
  ACTIVEHIGH: 'Aktívna vysoká', // DeepL
  ACTIVELOW: 'Aktívny nízky', // DeepL
  UNCHANGED: 'Bez zmien', // DeepL
  SECURITY_1: 'Pridať alebo odstrániť používateľov', // DeepL
  DOWNLOAD_UPLOAD_1: 'Stiahnutie a nahratie nastavení', // DeepL
  MODULES: 'Moduly',
  MODULES_1: 'Aktivácia alebo deaktivácia externých modulov', // DeepL
  NETWORK_DESC: 'Wi-Fi, Ethernet a názov hostiteľa',
  AP_DESC: 'Vlastný hotspot na nastavenie a záložný prístup',
  MQTT_DESC: 'Broker, témy a discovery',
  APPLICATION_DESC: 'Zbernica EMS, doska, snímače a formátovanie',
  NTP_DESC: 'Časový server a časové pásmo',
  FIRMWARE_DESC: 'Verzia, aktualizácie a obnova',
  MODULES_UPDATED: 'Aktualizované moduly',
  MODULES_NONE: 'Neboli zistené žiadne externé moduly',
  RENAME: 'Pomenovať', // DeepL
  ENABLE_MODBUS: 'Aktivovať Modbus', // DeepL
  VIEW_LOG: 'Prezrite si protokol na diagnostiku problémov', // DeepL
  SERVICES: 'Služby',
  ALLVALUES: 'Všetky hodnoty',
  WAIT_FIRMWARE: 'Firmware sa nahráva a inštaluje', // DeepL
  INSTALL_VERSION: 'Týmto sa nainštaluje verzia {0}',
  UPDATE_AVAILABLE: 'k dispozícii je aktualizácia', // DeepL
  LATEST_VERSION: 'Používate najnovšiu verziu firmvéru ({0})',
  PLEASE_WAIT: 'Prosím, počkajte', // DeepL
  RESTARTING_PRE: 'Spúšťanie', // DeepL
  RESTARTING_POST: 'Príprava',
  AUTO_SCROLL: 'Automatické posúvanie', // DeepL
  DASHBOARD: 'Panel',
  DEVELOPER_MODE: 'Režim pre vývojárov', // DeepL
  DISABLE_RESET: 'Zakázať obnovenie továrenských nastavení', // DeepL
  AUTO_FW_CHECK: 'Automaticky kontrolovať dostupnosť aktualizácií firmvéru', // DeepL
  BYTES: 'Bajty', // DeepL
  BITMASK: 'Bitová maska',
  DUPLICATE: 'Duplikát', // DeepL
  DASHBOARD_1: 'Nižšie sa zobrazujú všetky aktívne entity EMS označené ako obľúbené, ako aj všetky vlastné entity, časové plány a externé snímače.', // DeepL
  NO_GPIO: 'Nenašiel sa žiadny dostupný GPIO', // DeepL
  PLATFORM: 'Platforma',
  RELEASE_TYPE: 'Typ Vydania',
  FIRMWARE_VERSION_INFO: 'Informácie o Verzii Firmvéru', // DeepL
  USER_PROFILE: 'Profil používateľa',
  ONLINE_HELP: 'online pomoc',
  UPGRADE_IMPORTANT_MESSAGES: 'Dôležité Správy o Aktualizáciách', // DeepL
  UPGRADE_IMPORTANT_MESSAGES_2: 'Aktualizujete na novú hlavnú verziu. Odporúča sa najskôr vytvoriť zálohu systému. Skontrolujte tiež online pomoc, či neobsahuje žiadne zásadné zmeny.', // DeepL
  UPGRADE_IMPORTANT_MESSAGES_3: 'Verzia 3.10 obsahuje zásadné zmeny týkajúce sa bezpečnosti. Najskôr si vytvorte zálohu systému: po aktualizácii sa heslá ukladajú v hashovanej podobe a staršie verzie ich nedokážu prečítať, takže na návrat k predchádzajúcej verzii budete potrebovať túto zálohu. Každé volanie API potom vyžaduje kľúč API, takže integrácie, ako napríklad Home Assistant, je potrebné aktualizovať. Pred pokračovaním si prečítajte informácie o zásadných zmenách v online nápovede.', // DeepL
  WARNING_SYSTEM_BACKUP: 'Týmto sa vytvorí záloha vašej kompletnej konfigurácie a nastavení systému. Heslá a kľúče sa do zálohy nezahrnú, pokiaľ ich neuvádzate nižšie; obnovenie zálohy zachová heslá, ktoré sú už v zariadení. Príkazy sú vždy zahrnuté a môžu obsahovať prihlasovacie údaje, preto buďte opatrní pri ich zdieľaní. Chcete pokračovať?', // DeepL
  BACKUP_INCLUDE_SECRETS: 'Zahrňte heslá a kľúče (potrebné na obnovenie na novom zariadení)', // DeepL
  TEST_EMAIL_SUCCESSFUL: 'Odoslaný testovací e-mail', // DeepL
  SYSTEM_NAME: 'Názov systému',
  COMMANDS: 'Príkazy',
  COMMANDS_HELP_1: 'Nižšie definujte opakovane použiteľné pomenované príkazy. Tieto príkazy je možné spúšťať z konzoly, cez API/MQTT alebo na ne odkazovať v Plánovači.', // DeepL
  COMMAND_EXECUTED: 'Príkaz vykonaný', // DeepL
  RUN: 'Spustiť', // DeepL
  INTERNET_CONNECTION_CHECK: 'Online kontrola dostupných verzií', // DeepL
  UPLOAD_BACKUP_TEXT: 'Kliknutím vyberte alebo sem presuňte záložný súbor (.json)', // DeepL
  UPLOAD_FIRMWARE_TEXT: 'Kliknutím vyberte alebo sem presuňte súbor firmvéru .bin/.md5', // DeepL
  UPLOAD_MD5_RECEIVED: 'Súhrn prijatý, teraz nahrajte súbor firmvéru .bin', // DeepL
  UPLOAD_MD5_MATCHED: 'Zhoda MD5, inštalácia firmvéru…', // DeepL
  UPLOAD_NOT_OFFICIAL: 'Toto nie je oficiálna verzia EMS-ESP. Inštalujte iba firmware, ktorému dôverujete.', // DeepL
  UPLOAD_NOT_OFFICIAL_HELP: 'O oficiálnych a vlastných zostaveniach',
  UPLOAD_INSTALL_ANYWAY: 'Nainštalujte to aj tak', // DeepL
  OFFICIAL_BUILD: 'Oficiálna Verzia', // DeepL
  CUSTOM_BUILD: 'Vlastná Konfigurácia', // DeepL
  USING_CUSTOM_FIRMWARE: 'Používate vlastný firmvér', // DeepL
  PASSWORD_CONFIRM_TITLE: 'Zadajte heslo ešte raz', // DeepL
  PASSWORD_CONFIRM_TEXT: 'Pre vašu bezpečnosť zadajte heslo pred vykonaním tejto zmeny. Počas nasledujúcich 5 minút nebudete opäť vyzvaní na jeho zadanie.', // DeepL
  PASSWORD_CONFIRM_BUTTON: 'Potvrdiť',
  PASSWORD_WRONG: 'Nesprávne heslo',
  PASSWORD_NOT_CONFIRMED: 'Zrušené, heslo nebolo zadané',
  RECOVERY_APP: 'Aplikácia na obnovenie', // DeepL
  RECOVERY_NOT_INSTALLED: 'Nenainštalované',
  RECOVERY_INSTALLED: 'Nainštalovaný obnovovací firmvér', // DeepL
  RESTART_INTO_RECOVERY: 'Reštartujte aplikáciu do režimu obnovenia', // DeepL
  RECOVERY_NOT_FOUND: 'Aplikácia na obnovenie nebola nájdená online', // DeepL
  RECOVERY_START_FAILED: 'Aplikáciu Recovery App nie je možné spustiť, môže byť zastaraná alebo poškodená. Najskôr nainštalujte najnovšiu verziu', // DeepL
  PARTITION_START_FAILED: 'Tento firmvér sa nedá spustiť, môže byť poškodený', // DeepL
  INSTALLED_VERSIONS: 'Inštalované verzie', // DeepL
  EMSESP_APP: 'Aplikácia EMS-ESP',
  NO_UPDATES_AVAILABLE: 'nie sú k dispozícii žiadne aktualizácie', // DeepL
  SWITCH_TO_DEV: 'Prepnúť na dev {0}',
  SWITCH_TO_STABLE: 'Prejsť na stabilnú verziu {0}', // DeepL
  SAVE: 'Uložiť',
  NEEDS_RESTART: 'je potrebné reštartovať', // DeepL
  CHANGED_SETTINGS: '{num} {{zmenených nastavení|zmenené nastavenie|zmenené nastavenia|zmenené nastavenia|zmenených nastavení|zmeneného nastavenia}}',
  SAVED_RESTART: 'Uložené. Na použitie nových nastavení reštartujte EMS-ESP.', // DeepL
  RESTART_NOW: 'Reštartovať teraz',
  COPY_FAILED: 'Kopírovanie sa nepodarilo, vyberte text a skopírujte ho sami', // DeepL
  DOCUMENTATION: 'Dokumentácia',
  COMMUNITY_CHAT: 'Komunitný chat',
  REPORT_ISSUE: 'Nahláste chybu alebo požiadajte o novú funkciu', // DeepL
  SUPPORT_PROJECT: 'Podporte projekt',
  GET_HELP: 'Pomoc', // DeepL
  REPORT_PROBLEM: 'Nahlásiť problém', // DeepL
  REPORT_STEP_1: 'Stiahnite si informácie o systéme', // DeepL
  REPORT_STEP_1_TEXT: 'verzia, doska, stav zbernice EMS, pripojené zariadenia a nastavenia, bez hesiel', // DeepL
  REPORT_STEP_2: 'Uložte posledný protokol', // DeepL
  REPORT_STEP_2_TEXT: 'ak je problém so zariadením, najskôr nastavte úroveň protokolovania na DEBUG', // DeepL
  REPORT_STEP_3: 'Otvorte hlásenie alebo sa opýtajte na Discorde', // DeepL
  REPORT_STEP_3_TEXT: 'priložte oba súbory alebo vložte krátke zhrnutie nižšie do chatu',
  NEW_ISSUE: 'Nový problém', // DeepL
  SHORT_SUMMARY: 'Stručné zhrnutie', // DeepL
  LICENSE_TEXT: 'otvorený zdroj pod licenciou LGPL-3.0', // DeepL
  ADMIN_ACCESS_TEXT: 'plný prístup k nastaveniam, zariadeniam a API',
  GUEST_ACCESS_TEXT: 'môžu zobraziť hodnoty a spúšťať príkazy, ktoré môžu spúšťať hostia', // DeepL
  LANGUAGE_BROWSER_ONLY: 'toto sa týka len tohto prehliadača', // DeepL
  LANGUAGE_ENTITIES_HINT: 'Jazyk pre názvy entít zariadení v protokole MQTT a v API sa nastavuje v', // DeepL
  ADMINISTRATION: 'Správa',
  API_KEYS_SHORT: 'kľúče pre Home Assistant a skripty',
  ADDRESSES: 'Adresy',
  SEARCH_SETTINGS: 'Nastavenia vyhľadávania', // DeepL
  NO_SETTINGS_MATCH: 'Žiadne nastavenia nezodpovedajú vášmu vyhľadávaniu', // DeepL
  GENERAL: 'Všeobecné informácie', // DeepL
  BOARD: 'Doska',
  ADVANCED: 'Pokročilé',
  EMS_BUS_DESC: 'Ako EMS-ESP komunikuje s vaším vykurovacím systémom.', // DeepL
  BOARD_DESC: 'Piny sú prevzaté z profilu dosky. Ak ich chcete nastaviť sami, zapnite režim pre vývojárov a vyberte možnosť Vlastné.', // DeepL
  FORMAT_DESC: 'Ako hodnoty vyzerajú vo webovom rozhraní a v dátových paketoch MQTT/API.', // DeepL
  BOARD_PINS: 'Piny z profilu',
  ENABLE_EMAIL: 'Zapnite e-mailové upozornenia', // DeepL
  SEND_TEST_EMAIL: 'Odoslať testovací e-mail',
  EMAIL_FROM: 'Z', // DeepL
  EMAIL_TO: 'K', // DeepL
  EMAIL_SUBJECT: 'Predmet',
  WIFI_SCAN: 'Vyhľadávanie Wi-Fi sietí', // DeepL
  SCAN: 'Prečítať', // DeepL
  NETWORKS_NEARBY: 'Siete v okolí',
  SIGNAL: 'Signál',
  CHANNEL: 'Kanál',
  WIFI_OPEN: 'otvoriť', // DeepL
  WIFI_SAVED: 'uložené', // DeepL
  NETWORK_ETHERNET_INFO: 'Teraz je zariadenie pripojené cez Ethernet ({ip}). Wi-Fi sa používa len bez kábla.', // DeepL
  HOSTNAME_ACCESS: 'Názov hostiteľa a prístup',
  IP_ADDRESS: 'IP Adresa',
  NETWORK_ETH_10MBIT: 'Vynútiť 10 Mbit poloduplex', // DeepL
  NETWORK_ETH_10MBIT_HELP: 'len v prípade problémových vypínačov alebo dlhých káblov', // DeepL
  NETWORK_DISABLE_SLEEP_HELP: 'spoľahlivejšie, spotrebuje o niečo viac energie', // DeepL
  NETWORK_MDNS_HELP: 'prístup k nemu je cez {host}.local pomocou mDNS', // DeepL
  NETWORK_FIXED_IP_HELP: 'inak sa používa DHCP', // DeepL
  NETWORK_CORS_HELP: 'potrebné len vtedy, keď iná webová stránka volá API', // DeepL
  NTP_TIME_ON_DEVICE: 'čas strávený v EMS-ESP', // DeepL
  NTP_TIME_SERVER: 'Časový server',
  NTP_THERMOSTAT_CLOCK: 'Hodiny termostatu',
  NTP_THERMOSTAT_SYNC: 'Nastavte hodiny na termostate', // DeepL
  NTP_THERMOSTAT_SYNC_HELP: 'nastaví dátum a čas na termostate EMS',
  NTP_THERMOSTAT_SYNC_1: 'Zapnuté, časové pásmo EMS-ESP', // DeepL
  NTP_THERMOSTAT_SYNC_2: 'Zapnuté, vlastné časové pásmo', // DeepL
  NTP_THERMOSTAT_TZ: 'Časové pásmo termostatu',
  MQTT_PREVIEW: 'Čo sa zverejňuje', // DeepL
  MQTT_PREVIEW_CMD: 'prijaté ako príkaz',
  MQTT_PREVIEW_RESPONSE: 'výstup príkazov',
  MQTT_BEHAVIOUR: 'Správanie',
  MQTT_TOPICS: 'Témy a obsah', // DeepL
  MQTT_DISCOVERY: 'Objavovanie', // DeepL
  MQTT_INTERVALS_HELP: 'Interval v sekundách medzi publikovaniami, 0 = publikovať pri každej zmene.', // DeepL
  MQTT_SINGLE_NO_DISCOVERY: 'nie je k dispozícii pri vyhľadávaní na', // DeepL
  MQTT_DISCOVERY_NO_SINGLE: 'nie je k dispozícii pri témach s jednou hodnotou na', // DeepL
  MQTT_DISCOVERY_HELP: 'automaticky vytvorí entity v Home Assistant alebo Domoticz',
  MQTT_ENTITY_FORMAT_WARN: 'Zmena tohto nastavenia premenuje všetky entity v Home Assistant', // DeepL
  USERS: 'Používatelia',
  USERS_TEXT: 'Správcovia môžu meniť všetko. Hostia môžu prehliadať obsah a spúšťať príkazy označené pre hostí.', // DeepL
  ADD_USER: 'Pridať používateľa',
  EDIT_USER: 'Upraviť používateľa',
  ROLE: 'Úloha', // DeepL
  ROLE_ADMIN: 'admin', // DeepL
  ROLE_GUEST: 'hosť',
  ADMIN: 'Správca',
  YOU: 'vy',
  NEW_PASSWORD: 'Nové heslo',
  UNSAVED_CHANGES: '{num} {{neuložených zmien|neuložená zmena|neuložené zmeny|neuložené zmeny|neuložených zmien|neuloženej zmeny}}',
  CREATE_KEY: 'Vytvoriť Kľúč', // DeepL
  NEW_API_KEY: 'Nový Kľúč API',
  NO_API_KEYS: 'Zatiaľ nie sú k dispozícii žiadne kľúče API', // DeepL
  API_KEYS_LIMIT: 'Môžete mať maximálne {num} klávesov.', // DeepL
  REVOKE: 'Zrušiť', // DeepL
  REVOKE_KEY: 'Zrušiť Kľúč API', // DeepL
  REVOKE_KEY_TEXT: 'Všetko, čo obsahuje „{name}“, prestane okamžite fungovať.', // DeepL
  MODULES_TEXT: 'Voliteľné funkcie zabudované v tomto firmvéri. Tu ich môžete zapnúť alebo vypnúť.', // DeepL
  BY_AUTHOR: 'autor: {name}',
  LICENSE_KEY: 'Licenčný kľúč',
  MODULE_ACTIVATED: 'aktivované',
  MODULE_PENDING: 'čaká na aktiváciu',
  ENABLED: 'Povolené', // DeepL
  UNDO: 'Vrátiť späť', // DeepL
  FW_UPDATES: 'Aktualizácie',
  FW_VERSION_AVAILABLE: 'K dispozícii je verzia {version}', // DeepL
  FW_RELEASE_NOTES: 'Poznámky k vydaniu',
  FW_UPDATES_OFFLINE: 'Nepodarilo sa overiť dostupnosť nových verzií online. Na tento účel potrebuje EMS-ESP pripojenie k internetu.', // DeepL
  FW_RESTART_RESET: 'Reštart a Reset', // DeepL
  FW_RECOVERY_TEXT: 'Reštartovať do aplikácie Recovery? Pripojí sa k vašej sieti a zároveň spustí vlastný prístupový bod, kde môžete nahrať firmvér.', // DeepL
  FW_SIZE: 'Veľkosť',
  FW_INSTALL_DATE: 'Inštalované na', // DeepL
  FW_UPLOAD_FAILED: 'Nahratie sa nepodarilo: nesprávny typ súboru alebo firmware pre inú dosku', // DeepL
  FILE_TYPE_INVALID: 'Tento typ súboru nie je podporovaný, vyberte súbor typu {types}', // DeepL
  WHO_CAN_RUN: 'Kto môže spustiť',
  CMD_NONE: 'Nenašli sa žiadne príkazy',
  CMD_NONE_MATCH: 'Vaše vyhľadávanie nezodpovedá žiadnym príkazom', // DeepL
  SCHEDULE_NONE: 'Nenašli sa žiadne časové plány', // DeepL
  SCHEDULE_NONE_MATCH: 'Žiadne plány nezodpovedajú vášmu vyhľadávaniu', // DeepL
  CE_NONE: 'Nenašli sa žiadne entity',
  CE_ALL: 'Všetko',
  CE_STORAGE: 'Ukladanie', // DeepL
  CE_SOURCE: 'Zdroj',
  CE_EMS_SOURCE: 'zariadenie {device} · telegram {type} · posun {offset}', // DeepL
  CE_RAM_TEXT: 'zachované až do reštartu', // DeepL
  CE_NVS_TEXT: 'zachovávajú sa aj po reštartovaní', // DeepL
  CE_EXCLUDED: 'nie v API/MQTT', // DeepL
  SEARCH_SENSORS: 'Vyhľadávanie podľa názvu, ID alebo GPIO', // DeepL
  NO_SENSORS: 'Nenašli sa žiadne snímače',
  NO_SENSORS_MATCH: 'Žiadne snímače nezodpovedajú vášmu vyhľadávaniu', // DeepL
  SENSORS_HELP_1: 'Spravujte teplotné a analógové senzory pripojené k tomuto zariadeniu', // DeepL
  TEMP_SENSORS_HELP: 'Nájdené automaticky. Kliknutím na snímač ho môžete premenovať alebo nastaviť posun', // DeepL
  FREE_GPIOS: 'Voľné GPIO',
  SYSTEM_SENSOR: 'Systémový snímač',
  MASK_FAVORITE: 'Obľúbené',
  MASK_READONLY: 'Písanie zakázané', // DeepL
  MASK_NO_API: 'Žiadne MQTT/API', // DeepL
  MASK_HIDDEN: 'Skryté',
  MASK_REMOVED: 'Odstránené',
  FILTER_ALL: 'Všetko', // DeepL
  NUM_SELECTED: 'Vybrané: {num}',
  ALL_SHOWN: 'Všetko zobraziť: {num}', // DeepL
  BULK_HINT: 'Tlačidlá menia zaškrtnuté riadky alebo všetky zobrazené riadky, ak nie je zaškrtnutý žiadny.', // DeepL
  CLEAR_SELECTION: 'Jasný výber', // DeepL
  NO_ENTITIES_MATCH: 'Nenašli sa žiadne zodpovedajúce entity', // DeepL
  LIMIT_HELP: 'nechajte prázdne pre prednastavený limit', // DeepL
  MIN_ABOVE_MAX: 'Minimálna hodnota nesmie prekročiť maximálnu hodnotu', // DeepL
  RENAME_DEVICE: 'Premenovať Zariadenie', // DeepL
  WEEKLY: 'Týždenne',
  EVERY_DAY: 'Každý deň',
  SCHEDULER_NO_COMMANDS: 'Najskôr pridajte príkaz na stránke Príkazy', // DeepL
  DU_SETTINGS: 'Nastavenia EMS-ESP',
  DU_SETTINGS_TEXT: 'Záloha nastavení EMS-ESP: sieť, MQTT, Prispôsobenia, Plánovač, vlastné entity a Moduly. Nahraním súboru s nastaveniami alebo zálohy sa EMS-ESP reštartuje.', // DeepL
  HB_TITLE: 'Nastavenia kúrenia',
  HB_TEXT: 'Ukladá aktuálne hodnoty vášho kotla, termostatu a ďalších zariadení EMS, aby ste ich mohli neskôr obnoviť, napríklad po výmene zariadenia. Entity a príkazy určené len na čítanie sa pri obnovení nikdy nemenia.', // DeepL
  HB_BACKUP: 'Zálohovať Nastavenia Vykurovania', // DeepL
  HB_RESTORE: 'Obnoviť nastavenia vykurovania', // DeepL
  DATA_EXPORT: 'Export údajov',
  DATA_EXPORT_TEXT: 'Na účely podpory a analýzy. Tieto súbory nie je možné nahrať späť.', // DeepL
  NUM_VALUES: '{num} {{hodnôt|hodnota|hodnoty|hodnoty|hodnôt|hodnoty}}',
  HB_NOTHING: 'nie je čo zálohovať', // DeepL
  FILE_NAME: 'Názov súboru',
  HB_NOTE: 'Poznámka (voliteľná, zobrazí sa znova pri obnovení)', // DeepL
  HB_INCOMPLETE: 'Hodnoty, ktoré ešte neboli načítané zo zbernice, sa nezobrazujú. Po reštarte počkajte niekoľko minút, kým sa vytvorí kompletná záloha.', // DeepL
  HB_RESTORE_TEXT: 'Vyberte si zálohu nastavení vykurovania. Nič sa nezapíše, kým nezkontrolujete zmeny a nepotvrdíte ich.', // DeepL
  HB_DROP: 'Kliknutím vyberte alebo sem presuňte záložný súbor vykurovania (.json)', // DeepL
  HB_INVALID: 'Toto nie je záloha nastavení vykurovania', // DeepL
  HB_OTHER_FILE: 'Vyberte iný súbor', // DeepL
  HB_SAME_DEVICE: 'rovnaké zariadenie',
  HB_NOT_CONNECTED: 'nie je pripojené', // DeepL
  HB_OTHER_DEVICE: 'iné zariadenie (ID produktu {backup} v zálohe, {now} pripojené), preskočené', // DeepL
  HB_UNIT_MISMATCH: 'Táto záloha obsahuje teploty v jednotkách {unit}. Pred obnovením zálohy zmeňte jednotku teploty v nastaveniach aplikácie.', // DeepL
  HB_CHANGED: 'Zmenené',
  ALL: 'Všetko', // DeepL
  HB_NOW: 'Teraz',
  HB_BACKUP_VALUE: 'Záloha',
  HB_NOT_AVAILABLE: 'nie je k dispozícii', // DeepL
  HB_SENDING: 'odosielanie', // DeepL
  HB_WRITTEN: 'napísané', // DeepL
  HB_RESTORE_NUM: 'Obnoviť {num} {{hodnôt|hodnotu|hodnoty|hodnoty|hodnôt|hodnoty}}',
  RESTORE: 'Obnoviť',
  HB_CONFIRM: 'Zapísať {num} {{hodnôt|hodnotu|hodnoty|hodnoty|hodnôt|hodnoty}} zo súboru {file} do zariadení EMS?',
  HB_CONFIRM_TEXT: 'Hodnoty sa odosielajú po jednej. Ak zariadenie jednu odmietne, ostatné sa aj tak odošlú.', // DeepL
  HB_DONE: '{num} {{hodnôt zapísaných|hodnota zapísaná|hodnoty zapísané|hodnoty zapísané|hodnôt zapísaných|hodnoty zapísanej}}',
  HB_DONE_FAILED: '{num} zlyhalo. Zariadenia často odmietajú hodnoty na úrovni inštalatéra; podrobnosti nájdete v systémovom protokole.', // DeepL
  HB_NO_CHANGES: 'Nič sa nelíši od záložnej kópie', // DeepL
  HB_DIFFERENT_DEVICE: 'rôzne zariadenia', // DeepL
  SELECT_ALL: 'Vybrať všetko',
  DASHBOARD_EMPTY: 'Zatiaľ nie je čo zobraziť',
  NO_DEVICES: 'Zatiaľ neboli nájdené žiadne zariadenia EMS', // DeepL
  ADD_COMMAND: 'Pridať príkaz',
  EDIT_COMMAND: 'Príkaz na úpravu', // DeepL
  RUN_COMMAND: 'Spustiť príkaz',
  ADD_SCHEDULE: 'Pridať časový plán', // DeepL
  EDIT_SCHEDULE: 'Upraviť harmonogram', // DeepL
  ADD_ENTITY: 'Pridať entitu',
  EDIT_ENTITY: 'Upraviť Entitu',
  ADD_ANALOG_SENSOR: 'Pridať analógový snímač',
  EDIT_ANALOG_SENSOR: 'Úprava analógového snímače', // DeepL
  REMOVE_ANALOG_SENSOR: 'Odstrániť Analógový Snímač', // DeepL
  REMOVE_SENSOR_TEXT: 'Snímač „{name}“ sa ihneď odstráni.', // DeepL
  EDIT_TEMP_SENSOR: 'Upraviť Snímač Teploty', // DeepL
  INSTALL_FIRMWARE: 'Inštalácia Firmvéru ({channel})', // DeepL
  PARTITION: 'Oddiel',
  PULSE: 'Pulse', // DeepL
  SYS_HOME: 'Domov', // draft, please review
  SYS_HOME_HELP: 'Najdôležitejšie veci na prvý pohľad. Všetko ostatné nájdete v systémovej ponuke.', // draft, please review
  SYS_SENSORS_HELP: 'Senzory pripojené k bráne. Ak ich chcete zmeniť, otvorte Senzory v hlavnej ponuke.', // draft, please review
  SYS_MENU: 'Systémová ponuka', // draft, please review
  SYS_MENU_HELP: 'Všetky hodnoty {name} vo vašom systéme.', // draft, please review
  SYS_SEARCH_HELP: 'Zodpovedajúce hodnoty všetkých zariadení.', // draft, please review
  SYS_FAV_HELP: 'Vaše najdôležitejšie entity zariadení. Hviezdičkou pri hodnote zmeníte tento zoznam.', // draft, please review
  SYS_FAV_NONE: 'Zatiaľ žiadne obľúbené. Označte hodnoty hviezdičkou.', // draft, please review
  SYS_FAV_REMOVED: 'Odstránené z obľúbených', // draft, please review
  SYS_GROUP_HEATING: 'Kúrenie', // draft, please review
  SYS_GROUP_PLANT: 'Zariadenie', // draft, please review
  SYS_GROUP_OVERVIEW: 'Prehľad', // draft, please review
  SYS_LEVEL_MOVE_ALL: 'Zobraziť len vo všetkých hodnotách', // draft, please review
  SYS_LEVEL_MOVE_NORMAL: 'Zobraziť v normálnom zobrazení', // draft, please review
  SYS_LEVEL_NOW_ALL: 'Teraz sa zobrazuje len vo všetkých hodnotách', // draft, please review
  SYS_LEVEL_NOW_NORMAL: 'Teraz sa zobrazuje aj v normálnom zobrazení', // draft, please review
  SYS_HC: 'Vykurovací okruh {0}', // draft, please review
  SYS_DHW: 'Teplá voda', // draft, please review
  SYS_AHS: 'Prídavné kúrenie', // draft, please review
  SYS_HS: 'Zdroj tepla', // draft, please review
  SYS_SRC: 'Zdroj', // draft, please review
  SYS_OTHER_DEVICES: 'Ďalšie zariadenia', // draft, please review
  SYS_SERVICE: 'Údržba a poruchy', // draft, please review
  SYS_ENERGY: 'Energia a prevádzkové časy', // draft, please review
  SYS_SEC_MODE: 'Prevádzka', // draft, please review
  SYS_SEC_TEMPS: 'Teploty', // draft, please review
  SYS_SEC_STATUS: 'Stav', // draft, please review
  SYS_SEC_ADVANCED: 'Vykurovacia krivka a inštalácia', // draft, please review
  SYS_SEC_MEASURE: 'Namerané hodnoty', // draft, please review
  SYS_SEC_OTHER: 'Ďalšie hodnoty', // draft, please review
  SYS_LEVEL_NORMAL: 'Bežné', // draft, please review
  SYS_NO_RESULTS: 'Nič sa nenašlo. Skúste všetky hodnoty alebo iný výraz.', // draft, please review
  SYS_SEARCH_PLACES: 'Stránky a moduly', // draft, please review
  SYS_NOTHING_HERE: 'V úrovni Bežné sa tu nič nezobrazuje.', // draft, please review
  SYS_HIDDEN: 'Skryté expertné hodnoty: {num}', // draft, please review
  SYS_SEARCH_RESULTS: 'Výsledky vyhľadávania', // draft, please review
  SYS_CHANGED_WAS: 'zmenené (predtým: {0})', // draft, please review
  SYS_NO_DEVICES: 'Zatiaľ sa nenašli žiadne zariadenia.', // draft, please review
  SYS_ROOM_TEMP: 'Izbová teplota', // draft, please review
  SYS_WISH: 'Cieľová', // draft, please review
  SYS_CURRENT: 'Aktuálna teplota', // draft, please review
  SYS_OUTDOOR: 'Vonkajšia teplota', // draft, please review
  SYS_FLOW: 'Prívod', // draft, please review
  SYS_RETURN: 'Spiatočka', // draft, please review
  SYS_PRESSURE: 'Tlak v systéme', // draft, please review
  SYS_POWER: 'Výkon', // draft, please review
  SYS_BURNER_ON: 'Horák beží', // draft, please review
  SYS_BURNER_OFF: 'Horák vypnutý', // draft, please review
  SYS_DHW_HEATING: 'Prebieha ohrev', // draft, please review
  SYS_READY: 'Pripravené', // draft, please review
  SYS_ONE_TIME: 'Jednorazový ohrev', // draft, please review
  SYS_NOTES: 'Upozornenia', // draft, please review
  SYS_NOTES_SUB: 'Poruchy a údržba', // draft, please review
  SYS_LAST_FAULT: 'Posledná porucha', // draft, please review
  SYS_MAINT_MSG: 'Hlásenie údržby', // draft, please review
  SYS_NEXT_MAINT: 'Ďalšia údržba', // draft, please review
  SYS_FAULT_LAST: 'Posledná porucha / hlásenie', // draft, please review
  SYS_FAULT_CTRL: 'Posledná porucha regulátora', // draft, please review
  SYS_FAULT_SERVICE: 'Servisný kód', // draft, please review
  SYS_FAULT_ADD: 'Doplnkový kód {0}', // draft, please review
  SYS_FAULT_RAW_ONLY: 'Pre toto zariadenie nie je k dispozícii vysvetlenie.', // draft, please review
  SYS_FAULT_NONE: 'Žiadna uložená porucha.', // draft, please review
  SYS_FAULT_UNTIL_NOW: 'do teraz', // draft, please review
  SYS_MAINTENANCE: 'Údržba', // draft, please review
};

export default sk;
