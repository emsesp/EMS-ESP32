import type { Translation } from '../i18n-types';

const cs: Translation = {
  LANGUAGE: 'Jazyk',
  RETRY: 'Zkusit znovu',
  IS_REQUIRED: 'Pole {0} je povinné', // DeepL
  SIGN_IN: 'Přihlásit se',
  SIGN_IN_SUBTITLE: 'Brána pro topné systémy EMS',
  SIGN_OUT: 'Odhlásit se',
  USERNAME: 'Uživatelské jméno',
  PASSWORD: 'Heslo',
  APPLICATION_SETTINGS: 'Nastavení aplikace',
  AP_SETTINGS: 'Nastavení přístupového bodu',
  NETWORK_SETTINGS: 'Nastavení sítě',
  NTP_SETTINGS: 'Nastavení NTP',
  MQTT_SETTINGS: 'Nastavení MQTT',
  EMS_BUS_SETTINGS: 'Nastavení sběrnice EMS',
  HELP: 'Nápověda',
  LOGGED_IN: 'Přihlášen jako {name}',
  PLEASE_SIGNIN: 'Přihlaste se, prosím, abyste mohli pokračovat', // DeepL
  UPLOAD_SUCCESSFUL: 'Nahrání proběhlo úspěšně', // DeepL
  DOWNLOAD_SUCCESSFUL: 'Stáhnout úspěšně', // DeepL
  INVALID_LOGIN: 'Nesprávné uživatelské jméno nebo heslo', // DeepL
  NETWORK: 'Síť',
  SECURITY: 'Zabezpečení',
  ONOFF_CAP: 'ZAP/VYP',
  ONOFF: 'zapnuto/vypnuto', // DeepL
  TYPE: 'Typ',
  ENTITIES: 'entity',
  EXPORT: 'Exportovat',
  FAVORITES: "Oblíbené",
  CLIENT_ID: 'ID klienta',
  DEVICE_ID: 'ID zařízení',
  PRODUCT_ID: 'ID produktu',
  ENTITY_ID: 'ID entity',
  SENSOR_ID: 'ID senzoru',
  TYPE_ID: 'ID typu',
  DEVICE: 'Zařízení',
  BRAND: 'Značka',
  VERSION: 'Verze',
  ENTITY_NAME: 'Název entity',
  VALUE: 'Hodnota',
  DEVICES: 'Zařízení',
  SENSORS: 'Senzory',
  AUTOMATION: 'Automatizace',
  CONNECTIVITY: 'Připojení',
  MAINTENANCE: 'Údržba',
  CHANGE_VALUE: 'Změnit hodnotu',
  CANCEL: 'Zrušit',
  REMOVE_ALL: 'Odebrat Vše',
  APPLY_CHANGES: 'Použít změny ({0})',
  UPDATE: 'Aktualizace', // DeepL
  REMOVE: 'Odstranit', // DeepL
  RESET: 'Obnovit',
  PROBLEM_UPDATING: 'Problém s aktualizací',
  PROBLEM_LOADING: 'Problém s načítáním',
  ANALOG_SENSORS: 'Analogové senzory',
  SETTINGS: 'Nastavení',
  SETTINGS_UPDATED: 'Nastavení aktualizováno',
  NAME_UPDATED: 'Název aktualizován',
  SENSOR_UPDATED: 'Senzor aktualizován',
  ANALOG_SENSOR_UPDATED: 'Analogový senzor aktualizován',
  OFFSET: 'Offset',
  FACTOR: 'Faktor',
  FREQ: 'Četnost', // DeepL
  DUTY_CYCLE: 'Pracovní cyklus',
  UNIT: 'UoM', // DeepL
  STARTVALUE: 'Počáteční hodnota',
  EDIT: 'Upravit',
  TEMP_SENSORS: 'Teplotní senzory',
  WRITE_CMD_SENT: 'Odeslán příkaz', // DeepL
  EMS_BUS_WARNING: 'Sběrnice EMS je odpojena. Pokud toto varování přetrvává i po několika sekundách, zkontrolujte prosím nastavení a profil desky.', // DeepL
  CONNECTED: 'Připojeno',
  TX_ISSUES: 'Problémy s vysíláním – zkontrolujte režim vysílání', // DeepL
  DISCONNECTED: 'Odpojeno',
  DATA_TRAFFIC: 'Datový provoz',
  EMS_DEVICE: 'Zařízení EMS', // DeepL
  SUCCESS: 'ÚSPĚCH',
  FAIL: 'NEÚSPĚCH', // DeepL
  QUALITY: 'KVALITA',
  STATUS_NAMES: [
    'Přijaté EMS telegramy (Rx)',
    'Čtení EMS (Tx)',
    'Zápisy EMS (Tx)',
    'Čtení teplotních senzorů',
    'Čtení analogových senzorů',
    'Publikace MQTT',
    'API volání',
    'Syslog zprávy'
  ],
  NUM_DAYS: '{num} d{{ní|en|ní|ní|ní|ní}}',
  NUM_SECONDS: '{num} sek{{und|unda|undy|undy|und|und}}',
  NUM_HOURS: '{num} hod{{in|ina|iny|iny|in|in}}',
  NUM_MINUTES: '{num} minu{{t|ta|ty|ty|t|t}}',
  APPLICATION: 'Aplikace',
  CUSTOMIZATIONS: 'Přizpůsobení',
  APPLICATION_RESTARTING: 'EMS-ESP se restartuje',
  BOARD_PROFILE: 'Profil desky',
  CUSTOM: 'Vlastní',
  GPIO_OF: '{0} GPIO', // DeepL
  BUTTON: 'Tlačítko',
  TEMPERATURE: 'Teplota',
  PHY_TYPE: 'Typ Eth PHY',
  DISABLED: 'zakázáno',
  TX_MODE: 'Režim EMS Tx', // DeepL
  EMS_BUS: 'EMS Bus',
  EMS_BUS_ID: 'ID EMS Bus',
  HARDWARE: 'Hardware',
  LANGUAGE_ENTITIES: 'Jazyk (pro entity zařízení)',
  HIDE_LED: 'Skrýt LED',
  ENABLE_TELNET: 'Zapnout konzoli Telnet', // DeepL
  ENABLE_ANALOG: 'Povolit analogové senzory',
  CONVERT_FAHRENHEIT: 'Převádějte hodnoty teploty na stupně Fahrenheita', // DeepL
  READONLY: 'Zapněte režim pouze pro čtení (blokuje všechny odchozí příkazy EMS Tx pro zápis)', // DeepL
  WRITE_PROTECTION: 'Chraňte zařízení EMS před opakovaným a častým zapisováním', // DeepL
  WRITE_LIMIT: 'Max. počet zápisů za minutu (0 = bez omezení)', // DeepL
  WRITE_LIMIT_REFUSE: 'Zápisy přesahující limit se odmítají (v opačném případě se zaznamená pouze varování)', // DeepL
  UNDERCLOCK_CPU: 'Snižte taktovací frekvenci procesoru', // DeepL
  REMOTE_TIMEOUT: 'Časový limit pro vzdálené připojení', // DeepL
  REMOTE_TIMEOUT_EN: 'Vypnout dálkové ovládání při chybějící teplotě v místnosti', // DeepL
  HEATINGOFF: 'Spustit kotel s vypnutým nuceným vytápěním', // DeepL
  MIN_DURATION: 'Doba čekání', // DeepL
  ENABLE_SHOWER_TIMER: 'Zapnout časovač sprchy', // DeepL
  ENABLE_SHOWER_ALERT: 'Zapnout upozornění na sprchu', // DeepL
  TRIGGER_TIME: 'Čas spuštění',
  COLD_SHOT_DURATION: 'Doba trvání studeného spouštění', // DeepL
  FORMATTING_OPTIONS: 'Možnosti formátování',
  BOOLEAN_FORMAT_DASHBOARD: 'Web ve formátu Boolean', // DeepL
  BOOLEAN_FORMAT_API: 'Boolovský formát API/MQTT', // DeepL
  ENUM_FORMAT: 'Formát Enum API/MQTT', // DeepL
  INDEX: 'Rejstřík', // DeepL
  ENABLE_PARASITE: 'Zapnout napájení z 1-Wire', // DeepL
  LOG_HEX: 'Zaznamenávejte telegramy EMS v hexadecimálním formátu', // DeepL
  ENABLE_SYSLOG: 'Zapnout Syslog', // DeepL
  LOG_LEVEL: 'Úroveň protokolování',
  MARK_INTERVAL: 'Označit interval', // DeepL
  SECONDS: 'sekund', // DeepL
  MINUTES: 'minut', // DeepL
  HOURS: 'hodiny',
  RESTART: 'Restartovat', // DeepL
  RESTART_TEXT: 'Aby se změna nastavení systému projevila, je třeba EMS-ESP restartovat', // DeepL
  RESTART_CONFIRM: 'Opravdu chcete restartovat EMS-ESP?', // DeepL
  COMMAND: 'Příkaz',
  CUSTOMIZATIONS_RESTART: 'Všechna přizpůsobení byla odstraněna. Restartuji…', // DeepL
  CUSTOMIZATIONS_FULL: 'Počet vybraných entit překročil limit. Ukládejte prosím po částech.', // DeepL
  CUSTOMIZATIONS_SAVED: 'Aktualizovány přizpůsobení', // DeepL
  CUSTOMIZATIONS_HELP_1: 'Vyberte zařízení a přizpůsobte si možnosti entit', // DeepL
  CUSTOMIZATIONS_HELP_2: 'označit jako oblíbené',
  CUSTOMIZATIONS_HELP_3: 'zakázat akci zápisu',
  CUSTOMIZATIONS_HELP_4: 'vyloučit z MQTT a API',
  CUSTOMIZATIONS_HELP_5: 'skrýt ze zařízení', // DeepL
  CUSTOMIZATIONS_HELP_6: 'vymazat z paměti', // DeepL
  SELECT_DEVICE: 'Vyberte zařízení',
  MODIFIED: 'Upraveno', // DeepL
  RESET_SELECTED: 'Vynulovat výběr', // DeepL
  OPTIONS: 'Možnosti',
  NAME: 'Název',
  CUSTOMIZATIONS_RESET: 'Opravdu chcete odstranit všechna přizpůsobení?',
  SYSTEM_INFORMATION: 'Informace o systému',
  HELP_INFORMATION_1: 'Navštivte online wiki, kde najdete pokyny k nastavení EMS-ESP', // DeepL
  HELP_INFORMATION_2: 'Chcete-li se zapojit do živého komunitního chatu, připojte se k našemu serveru Discord', // DeepL
  HELP_INFORMATION_3: 'Chcete-li požádat o novou funkci nebo nahlásit chybu', // DeepL
  HELP_INFORMATION_4: 'Při hlášení problému uveďte informace o svém systému, aby byla reakce rychlejší', // DeepL
  HELP_INFORMATION_5: 'EMS-ESP vytvářejí dobrovolníci. Spropitné na Ko-fi pomáhá projektu pokračovat',
  UPLOAD: 'Nahrát',
  DOWNLOAD: 'Stáhnout',
  INSTALL: 'Instalace', // DeepL
  REINSTALL: 'Přeinstalovat', // DeepL
  REINSTALL_CONFIRM: 'Opravdu chcete přeinstalovat verzi {0}?', // DeepL
  FAILED: 'selhalo', // DeepL
  SYSTEM: 'Systém',
  SYSTEM_LOG: 'Systémový protokol',
  STATUS: 'Stav',
  DOWNLOAD_UPLOAD: 'Stáhnout/Nahrát',
  CLOSE: 'Zavřít',
  COMMAND_GUEST: 'Hosté mohou spustit tento příkaz',
  TOKENS_RESET: 'Tato aktualizace podepisuje přístupové tokeny novým klíčem zařízení, takže stávající přístupové tokeny API již nefungují. Vytvořte klíč API pro každou integraci, která API využívá, například Home Assistant, a použijte jej namísto starého tokenu.', // DeepL
  API_KEYS: 'Klíče API',
  API_KEYS_TEXT: 'Díky API klíčům mohou integrace, jako je Home Assistant, a skripty využívat API. Klíč odešlete v hlavičce „Authorization: Bearer <KLÍČ>“. Klíče typu „Read“ mohou číst hodnoty a spouštět příkazy, které mohou spouštět hosté, zatímco klíče typu „Read“ a „Write“ mohou provádět vše, co může správce provádět prostřednictvím API.', // DeepL
  API_KEY_ACCESS: 'Přístup',
  API_KEY_READ: 'Číst', // DeepL
  API_KEY_READ_WRITE: 'Čtení a Zápis', // DeepL
  API_KEY_SHOW_ONCE: 'Zkopírujte si tento klíč hned teď a uschovejte jej na bezpečném místě. Znovu se již nezobrazí.', // DeepL
  CREATED: 'Vytvořeno',
  COPY: 'Text', // DeepL
  COPIED: 'Zkopírováno',
  USE: 'Použijte', // DeepL
  FACTORY_RESET: 'Tovární Reset',
  SYSTEM_FACTORY_TEXT_DIALOG: 'Opravdu chcete obnovit tovární nastavení EMS-ESP?', // DeepL
  STABLE: 'Stabilní',
  DEVELOPMENT: 'Vývoj', // DeepL
  UPTIME: 'Dostupnost systému', // DeepL
  LAST_RESTART: 'Poslední restart',
  POWER_AND_RESET: 'Napájení a reset', // DeepL
  FREE_MEMORY: 'Volná paměť',
  BUFFER_SIZE: 'Maximální velikost vyrovnávací paměti',
  COMPACT: 'Stručně', // DeepL
  SUMMARY: 'Shrnutí', // DeepL
  SHOW_MORE: 'Zobrazit dalších {n}',
  SHOW_LESS: 'Zobrazit méně',
  COREDUMP: 'Crash Dump', // DeepL
  DOWNLOAD_SYSTEM_BACKUP: 'Záloha Systému', // DeepL
  ERROR: 'Došlo k neočekávané chybě, zkuste to prosím znovu', // DeepL
  TIME_SET: 'Nastavený čas', // DeepL
  MANAGE_USERS: 'Správa uživatelů', // DeepL
  USER_WARNING: 'Musíte mít nakonfigurovaného alespoň jednoho uživatele s oprávněními správce', // DeepL
  ADD: 'Přidat',
  TELNET_ENABLED: 'Telnetová konzole je zapnutá. Telnet není šifrovaný, proto jej v nastavení aplikace vypněte, pokud jej nepoužíváte.', // DeepL
  TOO_MANY_LOGINS: 'Došlo k příliš mnoha neúspěšným pokusům o přihlášení, zkuste to znovu za několik minut', // DeepL
  PASSWORD_KEEP: 'Nechte pole prázdné, chcete-li zachovat aktuální heslo', // DeepL
  SETUP_TITLE: 'Nastavte si hesla',
  SETUP_TEXT: 'Toto zařízení stále používá výchozí hesla. Než budete pokračovat, nastavte si vlastní.', // DeepL
  SETUP_ADMIN_PASSWORD: 'Nové heslo pro {name}',
  SETUP_AP_PASSWORD: 'Nové heslo k přístupovému bodu', // DeepL
  SETUP_AP_REQUIRED: 'Wi-Fi síť zatím není nastavena, takže přístupovým bodem je vstupní brána. 8–63 znaků, použije se při příštím spuštění přístupového bodu.', // DeepL
  SETUP_AP_OPTIONAL: 'Doporučeno: 8–63 znaků, použije se při příštím spuštění přístupového bodu. Nechte pole prázdné, chcete-li zachovat aktuální hodnotu.', // DeepL
  SETUP_AP_DEFAULT: 'Přístupový bod stále používá tovární heslo. Nastavte si vlastní v nastavení přístupového bodu.',
  SETUP_VIEW_USER: 'Volitelně: přidejte hosta s právy pouze pro prohlížení.', // DeepL
  NETWORK_CORS_ORIGIN_HELP: 'Webová adresa umožňující volání API z prohlížeče, například http://homeassistant.local:8123', // DeepL
  NETWORK_CORS_WILDCARD: 'Pomocí * může API volat jakákoli webová stránka otevřená v prohlížeči ve vaší síti. Místo toho zadejte přesnou adresu.', // DeepL
  NETWORK_ALLOWED_HOSTS: 'Povolené názvy hostitelů', // DeepL
  NETWORK_ALLOWED_HOSTS_HELP: 'Další názvy, pod kterými lze toto zařízení otevřít v prohlížeči, jsou například ems-esp.fritz.box nebo název reverzního proxy serveru. Jeho IP adresa, název hostitele a hostname.local fungují vždy.', // DeepL
  NOT_ENABLED: 'Není povoleno', // DeepL
  MQTT_ERRORS: 'Chyby MQTT',
  DISCONNECT_REASON: 'Důvod odpojení',
  MQTT_BASE_TOPIC: 'Základní téma',
  LAST_DISCONNECT: 'Poslední odpojení',
  CONNECTED_SINCE: 'Připojeno od',
  ENABLE_MQTT: 'Povolit MQTT',
  BROKER: 'Broker',
  BASE_TOPIC: 'Základ', // DeepL
  OPTIONAL: 'volitelné',
  MQTT_FORMAT: 'Formát tématu/užitečného obsahu', // DeepL
  MQTT_NEST_1: 'Vnořeno do jediného tématu', // DeepL
  MQTT_NEST_2: 'Jako jednotlivá témata', // DeepL
  MQTT_RESPONSE: 'Zveřejňujte výstup příkazů do tématu `response`', // DeepL
  MQTT_PUBLISH_TEXT_1: 'Publikujte témata s jednou hodnotou při změně', // DeepL
  MQTT_PUBLISH_TEXT_2: 'Publikujte do témat příkazů (ioBroker)', // DeepL
  MQTT_PUBLISH_TEXT_3: 'Povolit vyhledávání MQTT', // DeepL
  MQTT_PUBLISH_TEXT_4: 'Předpona pro témata vyhledávání', // DeepL
  MQTT_PUBLISH_TEXT_5: 'Typ zjištění', // DeepL
  MQTT_INPUT_NUMBER_FORMAT: 'Formát zadávaných čísel', // DeepL
  MQTT_PUBLISH_INTERVALS: 'Intervaly publikování',
  MQTT_INT_BOILER: 'Kotle a tepelná čerpadla',
  MQTT_INT_THERMOSTATS: 'Termostaty',
  MQTT_INT_SOLAR: 'Solární moduly',
  MQTT_INT_MIXER: 'Moduly směšovačů', // DeepL
  MQTT_INT_WATER: 'Vodní moduly',
  MQTT_QUEUE: 'Fronta MQTT', // DeepL
  DEFAULT: 'Výchozí nastavení', // DeepL
  MQTT_ENTITY_FORMAT: 'Formát identifikátoru entity', // DeepL
  MQTT_ENTITY_FORMAT_0: 'Jedna instance, dlouhý název (v3.4)', // DeepL
  MQTT_ENTITY_FORMAT_1: 'Jedna instance, krátký název', // DeepL
  MQTT_ENTITY_FORMAT_2: 'Více instancí, krátký název',
  MQTT_CLEAN_SESSION: 'Nastavit čistou relaci',
  MQTT_RETAIN_FLAG: 'Vždy nastavte příznak zachování', // DeepL
  MQTT_COMMANDS: 'Příkazy přijaté přes MQTT',
  MQTT_COMMANDS_0: 'Vypnuto, pouze čtení hodnot',
  MQTT_COMMANDS_1: 'Změna hodnot a spouštění příkazů', // DeepL
  MQTT_COMMANDS_2: 'Vše, včetně systémových příkazů', // DeepL
  INACTIVE: 'Neaktivní',
  ACTIVE: 'Aktivní',
  UNKNOWN: 'Neznámé',
  SET_TIME: 'Nastavit Čas',
  SET_TIME_TEXT: 'Zadejte níže místní datum a čas pro nastavení času', // DeepL
  LOCAL_TIME: 'Místní čas',
  UTC_TIME: 'Čas UTC',
  ENABLE_NTP: 'Zapněte NTP', // DeepL
  NTP_SERVER: 'Server NTP', // DeepL
  TIME_ZONE: 'Časové pásmo', // DeepL
  ACCESS_POINT: 'Přístupový bod',
  AP_PROVIDE: 'Zapnout přístupový bod', // DeepL
  AP_PROVIDE_TEXT_2: 'Pokud není nalezeno síťové připojení', // DeepL
  AP_PROVIDE_TEXT_3: 'Nikdy',
  AP_PREFERRED_CHANNEL: 'Preferovaný kanál',
  AP_HIDE_SSID: 'Skrýt SSID',
  AP_CLIENTS: 'Klienti AP',
  AP_MAX_CLIENTS: 'Maximální počet klientů',
  AP_LOCAL_IP: 'Lokální IP adresa', // DeepL
  IDLE: 'Nečinnost', // DeepL
  SCAN_AGAIN: 'Zkontrolujte znovu', // DeepL
  NETWORK_NO_WIFI: 'Nebyly nalezeny žádné sítě Wi-Fi', // DeepL
  NETWORK_BLANK_BSSID: 'nechte prázdné, chcete-li použít pouze SSID', // DeepL
  TX_POWER: 'Výkon vysílače', // DeepL
  HOSTNAME: 'Název hostitele',
  NETWORK_DISABLE_SLEEP: 'Vypněte režim spánku Wi-Fi', // DeepL
  NETWORK_LOW_BAND: 'Využijte nižší šířku pásma Wi-Fi', // DeepL
  NETWORK_USE_DNS: 'Povolte službu mDNS', // DeepL
  NETWORK_ENABLE_CORS: 'Povolit CORS',
  NETWORK_CORS_ORIGIN: 'Původ CORS',
  NETWORK_FIXED_IP: 'Používejte pevnou IP adresu', // DeepL
  NETWORK_GATEWAY: 'Brána',
  NETWORK_SUBNET: 'Maska podsítě',
  NETWORK_DNS: 'DNS servery',
  ADDRESS_OF: 'Adresa {0}', // DeepL
  ADMINISTRATOR: 'Správce', // DeepL
  GUEST: 'Host',
  NEW: 'Nové', // DeepL
  NEW_ENTITY_NAME: 'Nový název entity',
  MIN: 'min',
  MAX: 'max',
  BLOCK_NAVIGATE_1: 'Máte neuložené změny',
  BLOCK_NAVIGATE_2: 'Pokud přejdete na jinou stránku, vaše neuložené změny budou ztraceny. Opravdu chcete tuto stránku opustit?', // DeepL
  UPLOAD_ABORTED: 'Nahrávání bylo přerušeno', // DeepL
  BLOCK_BUSY_1: 'Stále ve vývoji', // DeepL
  BLOCK_BUSY_2: 'Opuštění této stránky ji zastaví. Opravdu chcete stránku opustit?', // DeepL
  DISCARD: 'Zrušit', // DeepL
  COMMANDS_UPDATED: 'Aktualizované příkazy', // DeepL
  V_REQUIRED: 'Povinné',
  V_BETWEEN: 'Musí být v rozmezí {min} a {max}', // DeepL
  V_MAX_CHARS: 'Musí mít maximálně {max} znaků', // DeepL
  V_CHARS: 'Musí mít {min}–{max} znaků',
  V_IN_USE: 'Již v používání', // DeepL
  V_IP: 'Musí se jednat o IP adresu', // DeepL
  V_HOSTNAME: 'Musí se jednat o platný název hostitele', // DeepL
  V_IP_OR_HOSTNAME: 'Musí se jednat o IP adresu nebo název hostitele', // DeepL
  V_NAME: 'Maximálně {max} znaků: písmena, číslice nebo _', // DeepL
  V_USERNAME: 'Maximálně {max} znaků: písmena, číslice, _ nebo .', // DeepL
  V_DAY: 'Vyberte alespoň jeden den',
  V_HEX: 'Musí se jednat o hexadecimální číslo', // DeepL
  V_FINGERPRINT: 'Musí se jednat o otisk SHA-1 (40 hexadecimálních číslic)', // DeepL
  V_EXAMPLE: 'Musí vypadat takto: {example}', // DeepL
  V_NOT_DEFAULT: 'Zvolte jiné heslo',
  ZERO_DISABLES: '0 vypne',
  MINUS_ONE_DISABLES: '-1 vypne zařízení', // DeepL
  LED_TYPE: 'Typ LED',
  PHY_POWER: 'Napájení PHY',
  PHY_CLOCK: 'Hodiny PHY',
  AUTO: 'Auto',
  PORT: 'Port',
  TIMEOUT: 'Časový limit',
  HOST: 'Hostitel',
  SMTP_SERVER: 'SMTP server', // DeepL
  KEEP_ALIVE: 'Udržujte spojení', // DeepL
  MQTT_REFRESH: 'Obnovit MQTT',
  MQTT_REFRESHED: 'MQTT aktualizováno', // DeepL
  MQTT_R_USER: 'Odpojeno uživatelem',
  MQTT_R_TCP: 'TCP připojení uzavřeno', // DeepL
  MQTT_R_PROTOCOL: 'Verze protokolu není přijata', // DeepL
  MQTT_R_ID: 'ID klienta bylo odmítnuto', // DeepL
  MQTT_R_SERVER: 'Server není k dispozici', // DeepL
  MQTT_R_CREDENTIALS: 'Nesprávně zadané přihlašovací údaje', // DeepL
  MQTT_R_AUTH: 'Bez oprávnění', // DeepL
  MQTT_R_FINGERPRINT: 'Otisek TLS se neshoduje', // DeepL
  GPIO_INVALID: 'Neplatná nastavení GPIO', // DeepL
  CONNECTED_VIA: 'Připojeno přes {0}',
  NETWORK_NOT_FOUND: 'Síť nebyla nalezena', // DeepL
  CONNECTION_FAILED: 'Připojení se nezdařilo', // DeepL
  CONNECTION_LOST: 'Ztráta připojení', // DeepL
  AP_LINGERING: 'Čekání v nečinnosti', // DeepL
  NONE: 'Žádné',
  LOG_NO_CONNECTION: 'Žádné připojení ke službě protokolování', // DeepL
  CHECK_SYSTEM_LOG: 'Zkontrolujte systémový protokol, zda nenajdete možné příčiny', // DeepL
  VALUE_TYPE: 'Typ hodnoty',
  DEFAULT_VALUE: 'Výchozí hodnota',
  DEFAULT_NAME: 'Výchozí název',
  ALWAYS_OFF: 'Vždy vypnuto',
  ALWAYS_ON: 'Vždy zapnuto',
  NO_DATA: 'Zatím žádné oblíbené entity. Označte je v sekci {customizations} nebo si prohlédněte všechny entity v sekci {devices}.', // DeepL
  STAY: 'Zůstaňte', // DeepL
  LEAVE: 'Nechte', // DeepL
  MENU: 'Nabídka',
  MORE: 'Více',
  SHOW_PASSWORD: 'Zobrazit heslo',
  STATUS_RUNNING: 'EMS-ESP je spuštěno', // DeepL
  NUM_ERRORS: '{num} {{chyb|chyba|chyby|chyby|chyb|chyb}}',
  NUM_WARNINGS: '{num} varování',
  HEAP: 'Heap',
  METER_USED: '{used} / {total} {unit} spotřebováno', // DeepL
  MAX_BLOCK: 'Maximální velikost bloku {num} KB', // DeepL
  MAX_BLOCK_HELP: 'Max block je největší část paměti heap, kterou lze najednou alokovat. Při velikosti pod 40 KB může dojít k selhání webového rozhraní a protokolu MQTT.', // DeepL
  FLASH_CHIP: 'Čip Flash', // DeepL
  FILE_SYSTEM: 'Souborový systém',
  DETAILS: 'Podrobnosti',
  RECONNECTS: 'Opětovné připojení', // DeepL
  CONNECTS: 'Připojuje', // DeepL
  ACTIVITY_HELP: 'Několik selhání příjmu (Rx) je normální. Mnoho selhání vysílání (Tx) obvykle znamená, že je pro váš kotel nastaven nesprávný režim vysílání.', // DeepL
  PAUSE: 'Pauza', // DeepL
  RESUME: 'Životopis', // DeepL
  LOG_PAUSED: 'Pozastaveno, nové řádky zadrženy: {num}', // DeepL
  FILTER_LINES: 'Řádky filtru', // DeepL
  SCHEDULER: 'Plánovač',
  SCHEDULER_HELP_1: 'Automatizujte příkazy přidáním naplánovaných událostí níže',
  SCHEDULER_HELP_2: 'při spuštění', // DeepL
  SCHEDULE: 'Časový plán', // DeepL
  TIME: 'Čas',
  TIMER: 'Časovač',
  ONCHANGE: 'Při změně',
  CONDITION: 'Podmínka',
  SCHEDULE_UPDATED: 'Aktualizovaný rozvrh', // DeepL
  CUSTOM_ENTITIES: 'Vlastní entity',
  ENTITIES_HELP_1: 'Definujte vlastní entity EMS nebo dynamické uživatelské proměnné', // DeepL
  ENTITIES_UPDATED: 'Aktualizované entity', // DeepL
  WRITEABLE: 'Upravitelné', // DeepL
  SHOWING: 'Zobrazit', // DeepL
  SEARCH: 'Hledat',
  CERT: 'Certifikát CA (PEM)',
  ENABLE_TLS: 'Zapněte TLS', // DeepL
  TLS_CHECK: 'Zkontrolujte zprostředkovatele pomocí', // DeepL
  TLS_CHECK_CA: 'Certifikát CA',
  TLS_CHECK_FINGERPRINT: 'Otisk certifikátu (SHA-1)',
  TLS_CHECK_NONE: 'Nic, nezaškrtnuto', // DeepL
  TLS_NOT_CHECKED: 'Připojení je šifrované, ale zprostředkovatel není ověřován, takže se za něj může vydávat jiné zařízení v síti', // DeepL
  ON: 'Na', // DeepL
  OFF: 'Vypnuto',
  POLARITY: 'Polarita',
  ACTIVEHIGH: 'Aktivní vysoká úroveň', // DeepL
  ACTIVELOW: 'Aktivní nízká úroveň', // DeepL
  UNCHANGED: 'Beze změn', // DeepL
  SECURITY_1: 'Přidání nebo odebrání uživatelů', // DeepL
  DOWNLOAD_UPLOAD_1: 'Stahování a nahrávání nastavení', // DeepL
  MODULES: 'Moduly',
  MODULES_1: 'Aktivace nebo deaktivace externích modulů', // DeepL
  NETWORK_DESC: 'Wi-Fi, Ethernet a název hostitele',
  AP_DESC: 'Vlastní hotspot pro nastavení a záložní přístup',
  MQTT_DESC: 'Broker, témata a discovery',
  APPLICATION_DESC: 'Sběrnice EMS, deska, senzory a formátování',
  NTP_DESC: 'Časový server a časové pásmo',
  FIRMWARE_DESC: 'Verze, aktualizace a obnova',
  MODULES_UPDATED: 'Aktualizované moduly', // DeepL
  MODULES_NONE: 'Nebyly detekovány žádné externí moduly', // DeepL
  RENAME: 'Přejmenovat',
  ENABLE_MODBUS: 'Povolit Modbus',
  VIEW_LOG: 'Prohlédněte si protokol a diagnostikujte problémy', // DeepL
  SERVICES: 'Služby',
  ALLVALUES: 'Všechny hodnoty',
  WAIT_FIRMWARE: 'Probíhá nahrávání a instalace firmwaru', // DeepL
  INSTALL_VERSION: 'Tím se nainstaluje verze {0}',
  UPDATE_AVAILABLE: 'k dispozici je aktualizace', // DeepL
  LATEST_VERSION: 'Používáte nejnovější verzi firmwaru ({0})',
  PLEASE_WAIT: 'Prosím, počkejte', // DeepL
  RESTARTING_PRE: 'Spouštění', // DeepL
  RESTARTING_POST: 'Příprava',
  AUTO_SCROLL: 'Automatické posouvání', // DeepL
  DASHBOARD: 'Dashboard',
  DEVELOPER_MODE: 'Režim pro vývojáře', // DeepL
  DISABLE_RESET: 'Zakázat obnovení továrního nastavení',
  AUTO_FW_CHECK: 'Automaticky kontrolovat dostupnost aktualizací firmwaru', // DeepL
  BYTES: 'Bajty',
  BITMASK: 'Bitová maska', // DeepL
  DUPLICATE: 'Duplikát',
  DASHBOARD_1: 'Níže jsou zobrazeny všechny aktivní entity systému EMS označené jako oblíbené, plus všechny vlastní entity, časové plány a externí senzory.', // DeepL
  NO_GPIO: 'Nebylo nalezeno žádné dostupné GPIO', // DeepL
  PLATFORM: 'Platforma',
  RELEASE_TYPE: 'Typ Vydání', // DeepL
  FIRMWARE_VERSION_INFO: 'Informace o Verzi Firmwaru',
  USER_PROFILE: 'Profil uživatele', // DeepL
  ONLINE_HELP: 'online nápověda',
  UPGRADE_IMPORTANT_MESSAGES: 'Důležité Zprávy o Aktualizacích', // DeepL
  UPGRADE_IMPORTANT_MESSAGES_2: 'Provádíte aktualizaci na novou hlavní verzi. Doporučujeme nejprve vytvořit zálohu systému. Zkontrolujte také online nápovědu, zda neobsahuje nějaké zásadní změny.', // DeepL
  UPGRADE_IMPORTANT_MESSAGES_3: 'Verze 3.10 obsahuje zásadní změny v oblasti zabezpečení. Nejprve si vytvořte zálohu systému: po aktualizaci jsou hesla ukládána v hashované podobě a starší verze je nedokážou přečíst, takže pro návrat k původnímu stavu je tato záloha nezbytná. Každé volání API pak vyžaduje API klíč, takže je nutné aktualizovat integrace, jako je například Home Assistant. Než budete pokračovat, přečtěte si informace o zásadních změnách v online nápovědě.', // DeepL
  WARNING_SYSTEM_BACKUP: 'Tímto se vytvoří záloha celé konfigurace a nastavení vašeho systému. Hesla a klíče nejsou zahrnuty, pokud je níže neuvedete; při obnovení zálohy zůstanou hesla již uložená v zařízení zachována. Příkazy jsou vždy zahrnuty a mohou obsahovat přihlašovací údaje, proto buďte při sdílení opatrní. Chcete pokračovat?', // DeepL
  BACKUP_INCLUDE_SECRETS: 'Uveďte hesla a klíče (potřebné k obnovení na novém zařízení)', // DeepL
  TEST_EMAIL_SUCCESSFUL: 'Odeslán testovací e-mail', // DeepL
  SYSTEM_NAME: 'Název systému',
  COMMANDS: 'Příkazy',
  COMMANDS_HELP_1: 'Níže definujte opakovaně použitelné pojmenované příkazy. Ty lze spouštět z konzole, přes API/MQTT nebo na ně odkazovat v Plánovači.', // DeepL
  COMMAND_EXECUTED: 'Příkaz proveden', // DeepL
  RUN: 'Spustit', // DeepL
  INTERNET_CONNECTION_CHECK: 'Kontrola dostupných verzí online', // DeepL
  UPLOAD_BACKUP_TEXT: 'Kliknutím vyberte nebo sem přetáhněte záložní soubor (.json)', // DeepL
  UPLOAD_FIRMWARE_TEXT: 'Kliknutím vyberte nebo sem přetáhněte soubor firmwaru ve formátu .bin/.md5', // DeepL
  UPLOAD_MD5_RECEIVED: 'Shrnutí přijato, nyní nahrajte soubor firmwaru .bin', // DeepL
  UPLOAD_MD5_MATCHED: 'Shody MD5, instalace firmwaru…', // DeepL
  UPLOAD_NOT_OFFICIAL: 'Toto není oficiální verze EMS-ESP. Instalujte pouze firmware, kterému důvěřujete.', // DeepL
  UPLOAD_NOT_OFFICIAL_HELP: 'O oficiálních a vlastních sestaveních',
  UPLOAD_INSTALL_ANYWAY: 'Nainstalujte to i tak', // DeepL
  OFFICIAL_BUILD: 'Oficiální Sestavení',
  CUSTOM_BUILD: 'Vlastní Sestavení',
  USING_CUSTOM_FIRMWARE: 'Používáte vlastní firmware',
  PASSWORD_CONFIRM_TITLE: 'Zadejte heslo znovu', // DeepL
  PASSWORD_CONFIRM_TEXT: 'Z bezpečnostních důvodů zadejte před provedením této změny své heslo. Po dobu 5 minut již nebudete znovu vyzváni k jeho zadání.', // DeepL
  PASSWORD_CONFIRM_BUTTON: 'Potvrdit',
  PASSWORD_WRONG: 'Nesprávné heslo',
  PASSWORD_NOT_CONFIRMED: 'Zrušeno, heslo nebylo zadáno',
  RECOVERY_APP: 'Aplikace pro obnovení', // DeepL
  RECOVERY_NOT_INSTALLED: 'Není nainstalováno', // DeepL
  RECOVERY_INSTALLED: 'Nainstalován obnovovací firmware', // DeepL
  RESTART_INTO_RECOVERY: 'Restartovat do aplikace Recovery', // DeepL
  RECOVERY_NOT_FOUND: 'Aplikace Recovery nebyla nalezena online', // DeepL
  RECOVERY_START_FAILED: 'Aplikaci Recovery nelze spustit, může být zastaralá nebo poškozená. Nejprve nainstalujte nejnovější verzi', // DeepL
  PARTITION_START_FAILED: 'Tento firmware nelze spustit, může být poškozen', // DeepL
  INSTALLED_VERSIONS: 'Nainstalované verze',
  EMSESP_APP: 'Aplikace EMS-ESP',
  NO_UPDATES_AVAILABLE: 'nejsou k dispozici žádné aktualizace', // DeepL
  SWITCH_TO_DEV: 'Přepněte na dev {0}', // DeepL
  SWITCH_TO_STABLE: 'Přepněte na stabilní verzi {0}', // DeepL
  SAVE: 'Uložit',
  NEEDS_RESTART: 'je třeba restartovat', // DeepL
  CHANGED_SETTINGS: '{num} {{změněných nastavení|změněné nastavení|změněná nastavení|změněná nastavení|změněných nastavení|změněného nastavení}}',
  SAVED_RESTART: 'Uloženo. Restartujte EMS-ESP, aby se nová nastavení uplatnila.', // DeepL
  RESTART_NOW: 'Restartovat nyní',
  COPY_FAILED: 'Kopírování se nezdařilo, vyberte text a zkopírujte jej ručně', // DeepL
  DOCUMENTATION: 'Dokumentace',
  COMMUNITY_CHAT: 'Komunitní chat',
  REPORT_ISSUE: 'Nahlásit chybu nebo požádat o novou funkci', // DeepL
  SUPPORT_PROJECT: 'Podpořte projekt',
  GET_HELP: 'Nápověda', // DeepL
  REPORT_PROBLEM: 'Nahlásit problém', // DeepL
  REPORT_STEP_1: 'Stáhnout informace o systému', // DeepL
  REPORT_STEP_1_TEXT: 'verze, deska, stav sběrnice EMS, připojená zařízení a nastavení, bez hesel', // DeepL
  REPORT_STEP_2: 'Uložit poslední záznam', // DeepL
  REPORT_STEP_2_TEXT: 'Pokud se problém týká zařízení, nejprve nastavte úroveň protokolování na DEBUG', // DeepL
  REPORT_STEP_3: 'Otevřete ticket nebo se zeptejte na Discord', // DeepL
  REPORT_STEP_3_TEXT: 'připojte oba soubory, nebo vložte krátké shrnutí níže do chatu', // DeepL
  NEW_ISSUE: 'Nový problém', // DeepL
  SHORT_SUMMARY: 'Stručné shrnutí', // DeepL
  LICENSE_TEXT: 'open source pod licencí LGPL-3.0',
  ADMIN_ACCESS_TEXT: 'plný přístup k nastavením, zařízením a API', // DeepL
  GUEST_ACCESS_TEXT: 'mohou zobrazovat hodnoty a spouštět příkazy, které mohou spouštět hosté', // DeepL
  LANGUAGE_BROWSER_ONLY: 'tato změna se týká pouze tohoto prohlížeče', // DeepL
  LANGUAGE_ENTITIES_HINT: 'Jazyk pro názvy entit zařízení v MQTT a API se nastavuje v', // DeepL
  ADMINISTRATION: 'Správa',
  API_KEYS_SHORT: 'klíče pro Home Assistant a skripty',
  ADDRESSES: 'Adresy',
  SEARCH_SETTINGS: 'Nastavení vyhledávání', // DeepL
  NO_SETTINGS_MATCH: 'Vašemu vyhledávání neodpovídá žádné nastavení', // DeepL
  GENERAL: 'Obecné informace', // DeepL
  BOARD: 'Deska',
  ADVANCED: 'Pokročilé',
  EMS_BUS_DESC: 'Jak EMS-ESP komunikuje s vaším topným systémem.', // DeepL
  BOARD_DESC: 'Piny jsou převzaty z profilu desky. Chcete-li je nastavit sami, zapněte režim pro vývojáře a vyberte možnost Vlastní.', // DeepL
  FORMAT_DESC: 'Jak hodnoty vypadají ve webovém uživatelském rozhraní a v datových paketech MQTT/API.', // DeepL
  BOARD_PINS: 'Piny z profilu',
  ENABLE_EMAIL: 'Zapněte e-mailová upozornění', // DeepL
  SEND_TEST_EMAIL: 'Odeslat testovací e-mail',
  EMAIL_FROM: 'Z', // DeepL
  EMAIL_TO: 'K', // DeepL
  EMAIL_SUBJECT: 'Předmět',
  WIFI_SCAN: 'Skenování Wi-Fi', // DeepL
  SCAN: 'Prohlédnout', // DeepL
  NETWORKS_NEARBY: 'Sítě v okolí',
  SIGNAL: 'Signál',
  CHANNEL: 'Kanál',
  WIFI_OPEN: 'otevřít', // DeepL
  WIFI_SAVED: 'uloženo', // DeepL
  NETWORK_ETHERNET_INFO: 'Nyní připojeno přes Ethernet ({ip}). Wi-Fi se používá pouze bez kabelu.', // DeepL
  HOSTNAME_ACCESS: 'Název hostitele a přístup',
  IP_ADDRESS: 'IP Adresa',
  NETWORK_ETH_10MBIT: 'Nastavit 10 Mbit v režimu half-duplex', // DeepL
  NETWORK_ETH_10MBIT_HELP: 'pouze pro poruchové spínače nebo dlouhé kabely', // DeepL
  NETWORK_DISABLE_SLEEP_HELP: 'spolehlivější, spotřebovává o něco více energie', // DeepL
  NETWORK_MDNS_HELP: 'přístup k ní je možný jako {host}.local pomocí mDNS', // DeepL
  NETWORK_FIXED_IP_HELP: 'v opačném případě se používá DHCP', // DeepL
  NETWORK_CORS_HELP: 'potřebné pouze v případě, že API volá jiná webová stránka', // DeepL
  NTP_TIME_ON_DEVICE: 'čas strávený na EMS-ESP', // DeepL
  NTP_TIME_SERVER: 'Časový server',
  NTP_THERMOSTAT_CLOCK: 'Hodiny termostatu',
  NTP_THERMOSTAT_SYNC: 'Nastavte hodiny na termostatu', // DeepL
  NTP_THERMOSTAT_SYNC_HELP: 'nastaví datum a čas na termostatu EMS',
  NTP_THERMOSTAT_SYNC_1: 'Zapnuto, časové pásmo EMS-ESP', // DeepL
  NTP_THERMOSTAT_SYNC_2: 'Zapnuto, vlastní časové pásmo', // DeepL
  NTP_THERMOSTAT_TZ: 'Časové pásmo termostatu',
  MQTT_PREVIEW: 'Co se zveřejňuje', // DeepL
  MQTT_PREVIEW_CMD: 'přijato jako příkaz',
  MQTT_PREVIEW_RESPONSE: 'výstup příkazů',
  MQTT_BEHAVIOUR: 'Chování',
  MQTT_TOPICS: 'Témata a užitečná data', // DeepL
  MQTT_DISCOVERY: 'Objevování', // DeepL
  MQTT_INTERVALS_HELP: 'Interval v sekundách mezi publikacemi, 0 = publikovat při každé změně.', // DeepL
  MQTT_SINGLE_NO_DISCOVERY: 'není k dispozici při vyhledávání na', // DeepL
  MQTT_DISCOVERY_NO_SINGLE: 'není k dispozici u témat s jednou hodnotou na', // DeepL
  MQTT_DISCOVERY_HELP: 'automaticky vytvoří entity v Home Assistant nebo Domoticz',
  MQTT_ENTITY_FORMAT_WARN: 'Změna tohoto nastavení přejmenuje všechny entity v Home Assistant', // DeepL
  USERS: 'Uživatelé',
  USERS_TEXT: 'Správci mohou měnit vše. Hosté mohou prohlížet obsah a spouštět příkazy označené pro hosty.', // DeepL
  ADD_USER: 'Přidat uživatele',
  EDIT_USER: 'Upravit uživatele',
  ROLE: 'Role',
  ROLE_ADMIN: 'admin', // DeepL
  ROLE_GUEST: 'host',
  ADMIN: 'Admin', // DeepL
  YOU: 'vy',
  NEW_PASSWORD: 'Nové heslo',
  UNSAVED_CHANGES: '{num} {{neuložených změn|neuložená změna|neuložené změny|neuložené změny|neuložených změn|neuložené změny}}',
  CREATE_KEY: 'Vytvořit Klíč',
  NEW_API_KEY: 'Nový Klíč API',
  NO_API_KEYS: 'Zatím nejsou k dispozici žádné klíče API', // DeepL
  API_KEYS_LIMIT: 'Můžete mít maximálně {num} kláves.', // DeepL
  REVOKE: 'Zrušit', // DeepL
  REVOKE_KEY: 'Zrušit Klíč API', // DeepL
  REVOKE_KEY_TEXT: 'Cokoli, co obsahuje „{name}“, přestane okamžitě fungovat.', // DeepL
  MODULES_TEXT: 'Volitelné funkce zabudované do tohoto firmwaru. Zde je můžete zapnout nebo vypnout.', // DeepL
  BY_AUTHOR: 'autor: {name}',
  LICENSE_KEY: 'Licenční klíč',
  MODULE_ACTIVATED: 'aktivováno',
  MODULE_PENDING: 'čeká na aktivaci',
  ENABLED: 'Povoleno', // DeepL
  UNDO: 'Zpět', // DeepL
  FW_UPDATES: 'Aktualizace',
  FW_VERSION_AVAILABLE: 'K dispozici je {version}',
  FW_RELEASE_NOTES: 'Poznámky k vydání',
  FW_UPDATES_OFFLINE: 'Nelze online zkontrolovat, zda jsou k dispozici nové verze. EMS-ESP k tomu potřebuje připojení k internetu.', // DeepL
  FW_RESTART_RESET: 'Restart a Reset', // DeepL
  FW_RECOVERY_TEXT: 'Restartovat do aplikace Recovery? Aplikace se připojí k vaší síti a zároveň spustí vlastní přístupový bod, do kterého můžete nahrát firmware.', // DeepL
  FW_SIZE: 'Velikost',
  FW_INSTALL_DATE: 'Nainstalováno na', // DeepL
  FW_UPLOAD_FAILED: 'Nahrání se nezdařilo: nesprávný typ souboru nebo firmware pro jinou desku', // DeepL
  FILE_TYPE_INVALID: 'Tento typ souboru není podporován, vyberte soubor typu {types}', // DeepL
  WHO_CAN_RUN: 'Kdo může spustit',
  CMD_NONE: 'Nebyly nalezeny žádné příkazy', // DeepL
  CMD_NONE_MATCH: 'Vašemu vyhledávání neodpovídá žádný příkaz', // DeepL
  SCHEDULE_NONE: 'Nebyly nalezeny žádné časové plány', // DeepL
  SCHEDULE_NONE_MATCH: 'Vašemu vyhledávání neodpovídá žádný časový plán v Plánovači', // DeepL
  CE_NONE: 'Nebyly nalezeny žádné entity', // DeepL
  CE_ALL: 'Vše',
  CE_STORAGE: 'Úložiště',
  CE_SOURCE: 'Zdroj',
  CE_EMS_SOURCE: 'zařízení {device} · telegram {type} · offset {offset}',
  CE_RAM_TEXT: 'uchováno do restartu',
  CE_NVS_TEXT: 'zachovávají se i po restartu', // DeepL
  CE_EXCLUDED: 'ne v API/MQTT', // DeepL
  SEARCH_SENSORS: 'Vyhledat podle názvu, ID nebo GPIO', // DeepL
  NO_SENSORS: 'Nebyly nalezeny žádné senzory',
  NO_SENSORS_MATCH: 'Žádný senzor neodpovídá vašemu vyhledávání', // DeepL
  SENSORS_HELP_1: 'Spravujte teplotní a analogové senzory připojené k tomuto zařízení', // DeepL
  TEMP_SENSORS_HELP: 'Nalezeno automaticky. Kliknutím na senzor jej můžete přejmenovat nebo nastavit odchylku', // DeepL
  FREE_GPIOS: 'Volné GPIO',
  SYSTEM_SENSOR: 'Systémový senzor',
  MASK_FAVORITE: 'Oblíbené',
  MASK_READONLY: 'Psát s omezením', // DeepL
  MASK_NO_API: 'Žádné MQTT/API', // DeepL
  MASK_HIDDEN: 'Skryté',
  MASK_REMOVED: 'Odstraněno', // DeepL
  FILTER_ALL: 'Vše',
  NUM_SELECTED: 'Vybráno: {num}',
  ALL_SHOWN: 'Zobrazit vše: {num}', // DeepL
  BULK_HINT: 'Tlačítka mění zaškrtnuté řádky, případně zobrazují všechny řádky, pokud není zaškrtnut žádný.', // DeepL
  CLEAR_SELECTION: 'Jasný výběr', // DeepL
  NO_ENTITIES_MATCH: 'Nenašly se žádné odpovídající entity', // DeepL
  LIMIT_HELP: 'nechte prázdné pro vestavěný limit', // DeepL
  MIN_ABOVE_MAX: 'Minimální hodnota nesmí být vyšší než maximální hodnota', // DeepL
  RENAME_DEVICE: 'Přejmenovat Zařízení',
  WEEKLY: 'Týdenní', // DeepL
  EVERY_DAY: 'Každý den',
  SCHEDULER_NO_COMMANDS: 'Nejprve přidejte příkaz na stránce Příkazy',
  DU_SETTINGS: 'Nastavení EMS-ESP',
  DU_SETTINGS_TEXT: 'Zálohování nastavení EMS-ESP: síť, MQTT, Přizpůsobení, Plánovač, vlastní entity a Moduly. Nahrání souboru s nastavením nebo zálohou způsobí restart EMS-ESP.', // DeepL
  HB_TITLE: 'Nastavení vytápění',
  HB_TEXT: 'Ukládá aktuální hodnoty vašeho kotle, termostatu a dalších zařízení EMS, abyste je mohli později obnovit, například po výměně zařízení. Entity a Příkazy určené pouze ke čtení se při obnově nikdy nemění.', // DeepL
  HB_BACKUP: 'Zálohovat Nastavení Vytápění', // DeepL
  HB_RESTORE: 'Obnovit nastavení vytápění',
  DATA_EXPORT: 'Export dat',
  DATA_EXPORT_TEXT: 'Pro účely podpory a analýzy. Tyto soubory nelze nahrát zpět.', // DeepL
  NUM_VALUES: '{num} {{hodnot|hodnota|hodnoty|hodnoty|hodnot|hodnoty}}',
  HB_NOTHING: 'není co zálohovat', // DeepL
  FILE_NAME: 'Název souboru',
  HB_NOTE: 'Poznámka (volitelná, zobrazí se znovu při obnovení)', // DeepL
  HB_INCOMPLETE: 'Hodnoty, které ještě nebyly načteny ze sběrnice, nejsou zobrazeny. Po restartu počkejte několik minut, než se načte kompletní záloha.', // DeepL
  HB_RESTORE_TEXT: 'Vyberte si záložní nastavení vytápění. Dokud změny nezkontrolujete a nepotvrdíte, nic se nezapíše.', // DeepL
  HB_DROP: 'Kliknutím vyberte nebo sem přetáhněte záložní soubor topení (.json)', // DeepL
  HB_INVALID: 'Nejedná se o zálohu nastavení vytápění', // DeepL
  HB_OTHER_FILE: 'Vyberte jiný soubor', // DeepL
  HB_SAME_DEVICE: 'stejné zařízení',
  HB_NOT_CONNECTED: 'není připojeno', // DeepL
  HB_OTHER_DEVICE: 'jiné zařízení (ID produktu {backup} v záloze, {now} připojeno), přeskočeno', // DeepL
  HB_UNIT_MISMATCH: 'Tato záloha obsahuje teploty v jednotkách {unit}. Před obnovením zálohy změňte jednotku teploty v nastavení aplikace.', // DeepL
  HB_CHANGED: 'Změněno', // DeepL
  ALL: 'Vše',
  HB_NOW: 'Nyní',
  HB_BACKUP_VALUE: 'Záloha',
  HB_NOT_AVAILABLE: 'není k dispozici', // DeepL
  HB_SENDING: 'odesílání', // DeepL
  HB_WRITTEN: 'napsáno', // DeepL
  HB_RESTORE_NUM: 'Obnovit {num} {{hodnot|hodnotu|hodnoty|hodnoty|hodnot|hodnoty}}',
  RESTORE: 'Obnovit',
  HB_CONFIRM: 'Zapsat {num} {{hodnot|hodnotu|hodnoty|hodnoty|hodnot|hodnoty}} ze souboru {file} do zařízení EMS?',
  HB_CONFIRM_TEXT: 'Hodnoty se odesílají po jedné. Pokud zařízení jednu z nich odmítne, ostatní se i tak odešlou.', // DeepL
  HB_DONE: '{num} {{hodnot zapsáno|hodnota zapsána|hodnoty zapsány|hodnoty zapsány|hodnot zapsáno|hodnoty zapsáno}}',
  HB_DONE_FAILED: 'Došlo k chybě {num}. Zařízení často odmítají hodnoty na úrovni instalátora; podrobnosti najdete v systémovém protokolu.', // DeepL
  HB_NO_CHANGES: 'Nic se neliší od záložní verze', // DeepL
  HB_DIFFERENT_DEVICE: 'různá zařízení', // DeepL
  SELECT_ALL: 'Vybrat vše',
  DASHBOARD_EMPTY: 'Zatím není co zobrazit',
  NO_DEVICES: 'Zatím nebyla nalezena žádná zařízení EMS',
  ADD_COMMAND: 'Přidat příkaz',
  EDIT_COMMAND: 'Upravit příkaz',
  RUN_COMMAND: 'Spustit příkaz',
  ADD_SCHEDULE: 'Přidat časový plán', // DeepL
  EDIT_SCHEDULE: 'Upravit plán', // DeepL
  ADD_ENTITY: 'Přidat entitu',
  EDIT_ENTITY: 'Upravit Entitu',
  ADD_ANALOG_SENSOR: 'Přidat analogový senzor',
  EDIT_ANALOG_SENSOR: 'Upravit analogový senzor',
  REMOVE_ANALOG_SENSOR: 'Odstranit Analogový Senzor', // DeepL
  REMOVE_SENSOR_TEXT: 'Senzor „{name}“ je okamžitě odstraněn.', // DeepL
  EDIT_TEMP_SENSOR: 'Upravit Teplotní Senzor',
  INSTALL_FIRMWARE: 'Nainstalovat Firmware ({channel})', // DeepL
  PARTITION: 'Oddíl',
  PULSE: 'Pulse', // DeepL
};

export default cs;
