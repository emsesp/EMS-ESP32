import type { Translation } from '../i18n-types';

const hu: Translation = {
  LANGUAGE: 'Nyelv',
  RETRY: 'Újrapróbálás', // DeepL
  IS_REQUIRED: 'A {0} mező kitöltése kötelező', // DeepL
  SIGN_IN: 'Bejelentkezés',
  SIGN_IN_SUBTITLE: 'Átjáró az EMS fűtési rendszerekhez', // DeepL
  SIGN_OUT: 'Kijelentkezés',
  USERNAME: 'Felhasználónév',
  PASSWORD: 'Jelszó',
  APPLICATION_SETTINGS: 'Alkalmazásbeállítások',
  AP_SETTINGS: 'Hozzáférési pont beállításai',
  NETWORK_SETTINGS: 'Hálózati beállítások',
  NTP_SETTINGS: 'NTP-beállítások',
  MQTT_SETTINGS: 'MQTT-beállítások',
  EMS_BUS_SETTINGS: 'EMS busz beállításai',
  HELP: 'Súgó',
  LOGGED_IN: 'Bejelentkezett: {name}', // DeepL
  PLEASE_SIGNIN: 'Kérjük, jelentkezzen be a folytatáshoz', // DeepL
  UPLOAD_SUCCESSFUL: 'A feltöltés sikeresen megtörtént', // DeepL
  DOWNLOAD_SUCCESSFUL: 'Sikeres letöltés',
  INVALID_LOGIN: 'Helytelen felhasználónév vagy jelszó', // DeepL
  NETWORK: 'Hálózat',
  SECURITY: 'Biztonság',
  ONOFF_CAP: 'BE/KI',
  ONOFF: 'be/ki',
  TYPE: 'Típus',
  ENTITIES: 'entitások',
  EXPORT: 'Export', // DeepL
  FAVORITES: 'Kedvencek',
  CLIENT_ID: 'Kliensazonosító',
  DEVICE_ID: 'Eszközazonosító',
  PRODUCT_ID: 'Termékazonosító',
  ENTITY_ID: 'Entitásazonosító',
  SENSOR_ID: 'Szenzorazonosító',
  TYPE_ID: 'Típusazonosító',
  DEVICE: 'Eszköz',
  BRAND: 'Márka',
  VERSION: 'Verzió',
  ENTITY_NAME: 'Entitás neve',
  VALUE: 'Érték',
  DEVICES: 'Eszközök',
  SENSORS: 'Szenzorok',
  AUTOMATION: 'Automatizálás',
  CONNECTIVITY: 'Kapcsolódás',
  MAINTENANCE: 'Karbantartás',
  CHANGE_VALUE: 'Érték módosítása',
  CANCEL: 'Mégse', // DeepL
  REMOVE_ALL: 'Összes Eltávolítása', // DeepL
  APPLY_CHANGES: 'Változtatások alkalmazása ({0})',
  UPDATE: 'Frissítés',
  REMOVE: 'Törlés', // DeepL
  RESET: 'Visszaállítás', // DeepL
  PROBLEM_UPDATING: 'Frissítési probléma', // DeepL
  PROBLEM_LOADING: 'Betöltési hiba', // DeepL
  ANALOG_SENSORS: 'Analóg szenzorok',
  SETTINGS: 'Beállítások',
  SETTINGS_UPDATED: 'Beállítások frissítve',
  NAME_UPDATED: 'Név frissítve',
  SENSOR_UPDATED: 'Szenzor frissítve',
  ANALOG_SENSOR_UPDATED: 'Analóg szenzor frissítve',
  OFFSET: 'Eltolás', // DeepL
  FACTOR: 'Tényező', // DeepL
  FREQ: 'Gyakoriság', // DeepL
  DUTY_CYCLE: 'Működési ciklus', // DeepL
  UNIT: 'UoM', // DeepL
  STARTVALUE: 'Kezdőérték', // DeepL
  EDIT: 'Szerkesztés',
  TEMP_SENSORS: 'Hőmérséklet-szenzorok',
  WRITE_CMD_SENT: 'Parancs elküldve', // DeepL
  EMS_BUS_WARNING: 'Az EMS-busz kapcsolata megszakadt. Ha ez a figyelmeztetés néhány másodperc elteltével is fennáll, kérjük, ellenőrizze a beállításokat és a kártya profilját.', // DeepL
  CONNECTED: 'Csatlakoztatva', // DeepL
  TX_ISSUES: 'Adási problémák – ellenőrizze az adási módot', // DeepL
  DISCONNECTED: 'Nincs kapcsolat', // DeepL
  DATA_TRAFFIC: 'Adatforgalom',
  EMS_DEVICE: 'EMS-eszköz', // DeepL
  SUCCESS: 'SIKER',
  FAIL: 'HIBA', // DeepL
  QUALITY: 'MINŐSÉG',
  STATUS_NAMES: [
    'Fogadott EMS táviratok (Rx)',
    'EMS olvasások (Tx)',
    'EMS írások (Tx)',
    'Hőmérséklet-szenzor olvasások',
    'Analóg szenzor olvasások',
    'MQTT publikálások',
    'API hívások',
    'Syslog üzenetek'
  ],
  NUM_DAYS: '{num} nap{{s}}',
  NUM_SECONDS: '{num} másodperc{{s}}',
  NUM_HOURS: '{num} óra{{s}}',
  NUM_MINUTES: '{num} perc{{s}}',
  APPLICATION: 'Alkalmazás',
  CUSTOMIZATIONS: 'Egyedi beállítások',
  APPLICATION_RESTARTING: 'Az EMS-ESP újraindul',
  BOARD_PROFILE: 'Fórumprofil', // DeepL
  CUSTOM: 'Egyéni', // DeepL
  GPIO_OF: '{0} GPIO',
  BUTTON: 'Gomb',
  TEMPERATURE: 'Hőmérséklet',
  PHY_TYPE: 'Eth PHY típus', // DeepL
  DISABLED: 'letiltva',
  TX_MODE: 'EMS Tx üzemmód', // DeepL
  EMS_BUS: 'EMS busz',
  EMS_BUS_ID: 'EMS busz azonosító',
  HARDWARE: 'Hardver',
  LANGUAGE_ENTITIES: 'Nyelv (eszközök esetében)', // DeepL
  HIDE_LED: 'LED elrejtése',
  ENABLE_TELNET: 'Telnet-konzol engedélyezése', // DeepL
  ENABLE_ANALOG: 'Analóg szenzorok engedélyezése',
  CONVERT_FAHRENHEIT: 'A hőmérsékleti értékeket alakítsa át Fahrenheit-re', // DeepL
  READONLY: 'Olvasási mód engedélyezése (minden kimenő EMS Tx írási parancsot blokkol)', // DeepL
  WRITE_PROTECTION: 'Védje az EMS-eszközöket az ismételt és gyakori írási műveletektől', // DeepL
  WRITE_LIMIT: 'Maximális írási műveletek száma percenként (0 = nincs korlát)', // DeepL
  WRITE_LIMIT_REFUSE: 'A korlátot meghaladó szövegek elutasítása (ellenkező esetben csak figyelmeztetés kerül naplózásra)', // DeepL
  UNDERCLOCK_CPU: 'A CPU sebességének csökkentése', // DeepL
  REMOTE_TIMEOUT: 'Távoli időkorlát', // DeepL
  REMOTE_TIMEOUT_EN: 'A távvezérlés letiltása hiányzó szobahőmérséklet esetén', // DeepL
  HEATINGOFF: 'Indítsa el a kazánt kikapcsolt kényszerfűtés mellett', // DeepL
  MIN_DURATION: 'Várakozási idő',
  ENABLE_SHOWER_TIMER: 'Zuhanyidőzítő engedélyezése',
  ENABLE_SHOWER_ALERT: 'Zuhanyozási figyelmeztetés engedélyezése', // DeepL
  TRIGGER_TIME: 'Kiváltási idő', // DeepL
  COLD_SHOT_DURATION: 'Hidegindítás időtartama', // DeepL
  FORMATTING_OPTIONS: 'Formázási beállítások', // DeepL
  BOOLEAN_FORMAT_DASHBOARD: 'Boole-formátumú web', // DeepL
  BOOLEAN_FORMAT_API: 'Boole-formátumú API/MQTT', // DeepL
  ENUM_FORMAT: 'Enum formátumú API/MQTT', // DeepL
  INDEX: 'Tartalomjegyzék', // DeepL
  ENABLE_PARASITE: 'Engedélyezze az 1-Wire parazita tápellátást', // DeepL
  LOG_HEX: 'Az EMS-telegramok hexadecimális formátumban történő naplózása', // DeepL
  ENABLE_SYSLOG: 'Syslog engedélyezése',
  LOG_LEVEL: 'Naplózási szint',
  MARK_INTERVAL: 'Jelölje meg az intervallumot', // DeepL
  SECONDS: 'másodperc',
  MINUTES: 'percek', // DeepL
  HOURS: 'órák', // DeepL
  RESTART: 'Újraindítás',
  RESTART_TEXT: 'A rendszerbeállítások módosításainak érvénybe lépéséhez az EMS-ESP-t újra kell indítani.', // DeepL
  RESTART_CONFIRM: 'Biztosan újra szeretné indítani az EMS-ESP-t?', // DeepL
  COMMAND: 'Parancsok', // DeepL
  CUSTOMIZATIONS_RESTART: 'Minden egyedi beállítást eltávolítottunk. Újraindítás…', // DeepL
  CUSTOMIZATIONS_FULL: 'A kiválasztott entitások száma meghaladta a korlátot. Kérjük, többszörös lépésekben mentse el őket!', // DeepL
  CUSTOMIZATIONS_SAVED: 'Egyedi beállítások frissítve', // DeepL
  CUSTOMIZATIONS_HELP_1: 'Válasszon ki egy eszközt, és testreszabja az entitások beállításait', // DeepL
  CUSTOMIZATIONS_HELP_2: 'kedvencnek jelölés', // DeepL
  CUSTOMIZATIONS_HELP_3: 'írási művelet letiltása', // DeepL
  CUSTOMIZATIONS_HELP_4: 'kizárás a MQTT-ből és az API-ból', // DeepL
  CUSTOMIZATIONS_HELP_5: 'elrejtés az Eszközökből', // DeepL
  CUSTOMIZATIONS_HELP_6: 'törlés a memóriából', // DeepL
  SELECT_DEVICE: 'Válasszon ki egy eszközt', // DeepL
  MODIFIED: 'Módosítva',
  RESET_SELECTED: 'A kijelölt visszaállítása', // DeepL
  OPTIONS: 'Beállítások', // DeepL
  NAME: 'Név',
  CUSTOMIZATIONS_RESET: 'Biztosan el akarja távolítani az összes egyedi beállítást?', // DeepL
  SYSTEM_INFORMATION: 'Rendszerinformációk',
  HELP_INFORMATION_1: 'Látogasson el az online wikire, ahol megtalálja az EMS-ESP beállításához szükséges utasításokat.', // DeepL
  HELP_INFORMATION_2: 'Ha élő közösségi csevegésben szeretnél részt venni, csatlakozz a Discord-szerverünkhöz', // DeepL
  HELP_INFORMATION_3: 'Funkció igénylése vagy hiba jelentése', // DeepL
  HELP_INFORMATION_4: 'Probléma bejelentésekor adja meg a rendszer adatait a gyorsabb válasz érdekében', // DeepL
  UPLOAD: 'Feltöltés',
  DOWNLOAD: 'Letöltés',
  INSTALL: 'Telepítés',
  REINSTALL: 'Újratelepítés',
  REINSTALL_CONFIRM: 'Biztosan újra szeretné telepíteni a(z) {0} verziót?',
  FAILED: 'sikertelen',
  SYSTEM: 'Rendszer',
  SYSTEM_LOG: 'Rendszernapló',
  STATUS: 'Állapot',
  DOWNLOAD_UPLOAD: 'Letöltés/Feltöltés',
  CLOSE: 'Bezárás',
  COMMAND_GUEST: 'A vendégek futtathatják ezt a parancsot', // DeepL
  TOKENS_RESET: 'Ez a frissítés az hozzáférési tokeneket egy új eszközkulccsal írja alá, így a meglévő API-hozzáférési tokenek már nem működnek. Hozzon létre egy API-kulcsot minden olyan integrációhoz, amely az API-t használja (például a Home Assistant), és azt használja a régi token helyett.', // DeepL
  API_KEYS: 'API-kulcsok',
  API_KEYS_TEXT: 'Az API-kulcsok lehetővé teszik, hogy olyan integrációk, mint a Home Assistant és a szkriptek, használhassák az API-t. A kulcsot az „Authorization: Bearer <KULCS>” fejlécben kell elküldeni. Az olvasási kulcsok értékeket olvashatnak és olyan parancsokat futtathatnak, amelyeket a vendégek is futtathatnak, míg az olvasási és írási kulcsok mindent megtehetnek, amit egy rendszergazda az API-n keresztül megtehet.', // DeepL
  API_KEY_ACCESS: 'Hozzáférés',
  API_KEY_READ: 'Olvasás',
  API_KEY_READ_WRITE: 'Olvasás és Írás', // DeepL
  API_KEY_SHOW_ONCE: 'Másolja ki most ezt a kulcsot, és őrizze meg biztonságos helyen. Ezután már nem jelenik meg újra.', // DeepL
  CREATED: 'Létrehozva',
  COPY: 'Szöveg', // DeepL
  COPIED: 'Másolva',
  USE: 'Használja', // DeepL
  FACTORY_RESET: 'Gyári Beállítások', // DeepL
  SYSTEM_FACTORY_TEXT_DIALOG: 'Biztosan vissza szeretné állítani az EMS-ESP-t a gyári alapbeállításokra?', // DeepL
  STABLE: 'Stabil',
  DEVELOPMENT: 'Fejlesztés', // DeepL
  UPTIME: 'A rendszer rendelkezésre állási ideje', // DeepL
  LAST_RESTART: 'Utolsó újraindítás',
  POWER_AND_RESET: 'Bekapcsolás és visszaállítás', // DeepL
  FREE_MEMORY: 'Szabad memória',
  BUFFER_SIZE: 'Maximális pufferméret', // DeepL
  COMPACT: 'Tömör', // DeepL
  SUMMARY: 'Összefoglalás', // DeepL
  SHOW_MORE: '{n} további elem megjelenítése', // DeepL
  SHOW_LESS: 'Kevesebbet mutatni', // DeepL
  COREDUMP: 'Crash Dump', // DeepL
  DOWNLOAD_SYSTEM_BACKUP: 'Rendszermentés', // DeepL
  ERROR: 'Váratlan hiba történt, kérjük, próbálja meg újra', // DeepL
  TIME_SET: 'Beállított idő', // DeepL
  MANAGE_USERS: 'Felhasználók kezelése',
  USER_WARNING: 'Legalább egy rendszergazdai felhasználót kell beállítania', // DeepL
  ADD: 'Hozzáadás',
  TELNET_ENABLED: 'A Telnet-konzol engedélyezve van. A Telnet nem titkosított, ezért ha nem használja, kapcsolja ki az Alkalmazásbeállítások menüben.', // DeepL
  TOO_MANY_LOGINS: 'Túl sok sikertelen bejelentkezési kísérlet történt, próbálja meg újra néhány perc múlva', // DeepL
  PASSWORD_KEEP: 'Hagyja üresen, ha meg kívánja tartani a jelenlegi jelszót', // DeepL
  SETUP_TITLE: 'Állítsa be a jelszavait',
  SETUP_TEXT: 'Ez az eszköz még mindig a gyári jelszavakat használja. Mielőtt továbbmenne, állítsa be a saját jelszavát.', // DeepL
  SETUP_ADMIN_PASSWORD: '{name} új jelszava', // DeepL
  SETUP_AP_PASSWORD: 'Új hozzáférési pont jelszava', // DeepL
  SETUP_AP_REQUIRED: 'Még nincs beállítva Wi-Fi-hálózat, ezért az hozzáférési ponton keresztül lehet belépni. 8–63 karakter, az hozzáférési pont következő indításakor kerül felhasználásra.', // DeepL
  SETUP_AP_OPTIONAL: 'Ajánlott. 8–63 karakter, a hozzáférési pont következő indításakor lép érvénybe. Ha üresen hagyja, a jelenlegi beállítás marad érvényben.', // DeepL
  SETUP_AP_DEFAULT: 'A hozzáférési pont még mindig a gyári jelszót használja. Állítsa be a sajátját a Hozzáférési pont beállításaiban.', // DeepL
  SETUP_VIEW_USER: 'Opcionális: hozzon létre egy csak olvasási jogosultsággal rendelkező vendégfelhasználót.', // DeepL
  NETWORK_CORS_ORIGIN_HELP: 'A böngészőből az API-t meghívó webcím, például http://homeassistant.local:8123', // DeepL
  NETWORK_CORS_WILDCARD: 'A * jel használatával a hálózatodon lévő bármely, böngészőben megnyitott weboldal meghívhatja az API-t. Ehelyett inkább állítsd be a pontos címet.', // DeepL
  NETWORK_ALLOWED_HOSTS: 'Megengedett gazdagépnevek', // DeepL
  NETWORK_ALLOWED_HOSTS_HELP: 'Az eszköz böngészőben megnyitható további nevek, például EMS-ESP.fritz.box vagy egy fordított proxy neve. IP-címe, gazdagépneve és gazdagépnév.local mindig működnek.', // DeepL
  NOT_ENABLED: 'Nincs engedélyezve',
  MQTT_ERRORS: 'MQTT hibák',
  DISCONNECT_REASON: 'A kapcsolat megszakításának oka', // DeepL
  MQTT_BASE_TOPIC: 'Alapvető téma', // DeepL
  LAST_DISCONNECT: 'Utolsó lekapcsolás', // DeepL
  CONNECTED_SINCE: 'Csatlakozva:', // DeepL
  ENABLE_MQTT: 'MQTT engedélyezése',
  BROKER: 'Broker', // DeepL
  BASE_TOPIC: 'Alap', // DeepL
  OPTIONAL: 'opcionális', // DeepL
  MQTT_FORMAT: 'Téma/adatformátum', // DeepL
  MQTT_NEST_1: 'Egyetlen témakörbe ágyazva', // DeepL
  MQTT_NEST_2: 'Egyes témák', // DeepL
  MQTT_RESPONSE: 'A parancsok kimenetét tegyük közzé egy `response` témában', // DeepL
  MQTT_PUBLISH_TEXT_1: 'Az egyértékű témákat változáskor tegyük közzé', // DeepL
  MQTT_PUBLISH_TEXT_2: 'Közzététel parancsok témákban (ioBroker)', // DeepL
  MQTT_PUBLISH_TEXT_3: 'Engedélyezze az MQTT-felfedezést', // DeepL
  MQTT_PUBLISH_TEXT_4: 'Előtag a felfedezési témákhoz', // DeepL
  MQTT_PUBLISH_TEXT_5: 'Felfedezés típusa', // DeepL
  MQTT_INPUT_NUMBER_FORMAT: 'Számformátum bevitele', // DeepL
  MQTT_PUBLISH_INTERVALS: 'Közzétételi időközök', // DeepL
  MQTT_INT_BOILER: 'Kazánok és hőszivattyúk',
  MQTT_INT_THERMOSTATS: 'Termosztátok',
  MQTT_INT_SOLAR: 'Napelemek', // DeepL
  MQTT_INT_MIXER: 'Keverőmodulok', // DeepL
  MQTT_INT_WATER: 'Vízmodulok', // DeepL
  MQTT_QUEUE: 'MQTT-sor', // DeepL
  DEFAULT: 'Alapértelmezett',
  MQTT_ENTITY_FORMAT: 'Entitás-azonosító formátum', // DeepL
  MQTT_ENTITY_FORMAT_0: 'Egyetlen példány, hosszú név (v3.4)', // DeepL
  MQTT_ENTITY_FORMAT_1: 'Egyetlen példány, rövid név', // DeepL
  MQTT_ENTITY_FORMAT_2: 'Több példány, rövid név', // DeepL
  MQTT_CLEAN_SESSION: 'Tiszta munkamenet beállítása', // DeepL
  MQTT_RETAIN_FLAG: 'Mindig állítsa be a retain jelzőt', // DeepL
  MQTT_COMMANDS: 'MQTT-n keresztül fogadott parancsok', // DeepL
  MQTT_COMMANDS_0: 'Ki, csak értékek olvasása', // DeepL
  MQTT_COMMANDS_1: 'Értékek módosítása és parancsok végrehajtása', // DeepL
  MQTT_COMMANDS_2: 'Minden, beleértve a rendszerparancsokat is', // DeepL
  INACTIVE: 'Inaktív',
  ACTIVE: 'Aktív',
  UNKNOWN: 'Ismeretlen',
  SET_TIME: 'Idő Beállítása',
  SET_TIME_TEXT: 'Az idő beállításához írja be az alábbi mezőbe a helyi dátumot és időt', // DeepL
  LOCAL_TIME: 'Helyi idő',
  UTC_TIME: 'UTC-idő', // DeepL
  ENABLE_NTP: 'NTP engedélyezése',
  NTP_SERVER: 'NTP-kiszolgáló', // DeepL
  TIME_ZONE: 'Időzóna',
  ACCESS_POINT: 'Hozzáférési pont', // DeepL
  AP_PROVIDE: 'Hozzáférési pont engedélyezése',
  AP_PROVIDE_TEXT_2: 'Ha nincs hálózati kapcsolat', // DeepL
  AP_PROVIDE_TEXT_3: 'Soha',
  AP_PREFERRED_CHANNEL: 'Előnyben részesített csatorna',
  AP_HIDE_SSID: 'SSID elrejtése',
  AP_CLIENTS: 'AP-ügyfelek', // DeepL
  AP_MAX_CLIENTS: 'Maximális ügyfélszám', // DeepL
  AP_LOCAL_IP: 'Helyi IP-cím', // DeepL
  IDLE: 'Tétlen',
  SCAN_AGAIN: 'Újra beolvasás', // DeepL
  NETWORK_NO_WIFI: 'Nem található Wi-Fi-hálózat', // DeepL
  NETWORK_BLANK_BSSID: 'hagyja üresen, ha csak az SSID-t szeretné használni', // DeepL
  TX_POWER: 'Adási teljesítmény', // DeepL
  HOSTNAME: 'Gépnév', // DeepL
  NETWORK_DISABLE_SLEEP: 'A Wi-Fi alvó üzemmód letiltása', // DeepL
  NETWORK_LOW_BAND: 'Használjon alacsonyabb Wi-Fi sávszélességet', // DeepL
  NETWORK_USE_DNS: 'Engedélyezze az mDNS szolgáltatást', // DeepL
  NETWORK_ENABLE_CORS: 'Engedélyezze a CORS-t', // DeepL
  NETWORK_CORS_ORIGIN: 'CORS eredet',
  NETWORK_FIXED_IP: 'Használjon fix IP-címet', // DeepL
  NETWORK_GATEWAY: 'Átjáró', // DeepL
  NETWORK_SUBNET: 'Alhálózati maszk',
  NETWORK_DNS: 'DNS-kiszolgálók', // DeepL
  ADDRESS_OF: '{0} cím',
  ADMINISTRATOR: 'Rendszergazda',
  GUEST: 'Vendég',
  NEW: 'Új',
  NEW_ENTITY_NAME: 'Az entitás új neve',
  MIN: 'min',
  MAX: 'max',
  BLOCK_NAVIGATE_1: 'Vannak mentetlen módosításai', // DeepL
  BLOCK_NAVIGATE_2: 'Ha másik oldalra lép, a mentetlen módosítások elvesznek. Biztosan el akarja hagyni ezt az oldalt?', // DeepL
  UPLOAD_ABORTED: 'A feltöltés megszakadt', // DeepL
  BLOCK_BUSY_1: 'Még folyamatban', // DeepL
  BLOCK_BUSY_2: 'Ha most elhagyja ezt az oldalt, az alkalmazás leáll. Biztosan el akarja hagyni az oldalt?', // DeepL
  DISCARD: 'Elvetés',
  COMMANDS_UPDATED: 'Parancsok frissítve',
  V_REQUIRED: 'Kötelező',
  V_BETWEEN: 'Hosszuknak {min} és {max} között kell lennie', // DeepL
  V_MAX_CHARS: '{max} karakter vagy annál kevesebb lehet', // DeepL
  V_CHARS: 'Hossza {min}–{max} karakter között kell lennie', // DeepL
  V_IN_USE: 'Már használatban', // DeepL
  V_IP: 'IP-címnek kell lennie',
  V_HOSTNAME: 'Érvényes gazdagépnévnek kell lennie', // DeepL
  V_IP_OR_HOSTNAME: 'IP-címnek vagy gazdagépnévnek kell lennie', // DeepL
  V_NAME: 'Legfeljebb {max} karakter: betűk, számok vagy _', // DeepL
  V_USERNAME: 'Legfeljebb {max} karakter: betűk, számok, _ vagy .', // DeepL
  V_DAY: 'Válasszon ki legalább egy napot', // DeepL
  V_HEX: 'Hexadecimális számnak kell lennie',
  V_FINGERPRINT: 'SHA-1 ujjlenyomatnak kell lennie (40 hexadecimális számjegy)',
  V_EXAMPLE: 'Így kell kinéznie: {example}',
  V_NOT_DEFAULT: 'Válasszon másik jelszót', // DeepL
  ZERO_DISABLES: 'A 0 kikapcsolja', // DeepL
  MINUS_ONE_DISABLES: '-1 kikapcsolja',
  LED_TYPE: 'LED-típus', // DeepL
  PHY_POWER: 'PHY tápellátás',
  PHY_CLOCK: 'PHY óra', // DeepL
  AUTO: 'Auto',
  PORT: 'Port',
  TIMEOUT: 'Időkorlát', // DeepL
  HOST: 'Gazdagép', // DeepL
  SMTP_SERVER: 'SMTP-kiszolgáló', // DeepL
  KEEP_ALIVE: 'Tartsa életben', // DeepL
  MQTT_REFRESH: 'MQTT frissítése',
  MQTT_REFRESHED: 'MQTT frissítve',
  MQTT_R_USER: 'A felhasználó által leválasztva', // DeepL
  MQTT_R_TCP: 'A TCP-kapcsolat lezárult', // DeepL
  MQTT_R_PROTOCOL: 'A protokoll verziója nem elfogadott', // DeepL
  MQTT_R_ID: 'Ügyfél-azonosító elutasítva', // DeepL
  MQTT_R_SERVER: 'A szerver nem elérhető', // DeepL
  MQTT_R_CREDENTIALS: 'Hibás hitelesítő adatok',
  MQTT_R_AUTH: 'Nincs jogosultság',
  MQTT_R_FINGERPRINT: 'A TLS ujjlenyomat nem egyezik',
  GPIO_INVALID: 'Érvénytelen GPIO-beállítások',
  CONNECTED_VIA: 'Kapcsolat: {0}',
  NETWORK_NOT_FOUND: 'A hálózat nem található', // DeepL
  CONNECTION_FAILED: 'A csatlakozás nem sikerült', // DeepL
  CONNECTION_LOST: 'Megszakadt a kapcsolat',
  AP_LINGERING: 'Várakozás tétlen állapotig', // DeepL
  NONE: 'Nincs',
  LOG_NO_CONNECTION: 'Nincs kapcsolódás a naplózási szolgáltatással', // DeepL
  CHECK_SYSTEM_LOG: 'Ellenőrizze a rendszer naplóját az esetleges okok felderítése érdekében', // DeepL
  VALUE_TYPE: 'Értéktípus',
  DEFAULT_VALUE: 'Alapértelmezett érték',
  DEFAULT_NAME: 'Alapértelmezett név',
  ALWAYS_OFF: 'Mindig ki',
  ALWAYS_ON: 'Mindig be van kapcsolva', // DeepL
  NO_DATA: 'Még nincsenek kedvencek. Jelölje meg őket a {customizations} menüpontban, vagy tekintse meg az összes entitást a {devices} menüpontban.', // DeepL
  STAY: 'Maradjon', // DeepL
  LEAVE: 'Hagyja', // DeepL
  MENU: 'Menü',
  MORE: 'Tovább', // DeepL
  SHOW_PASSWORD: 'Jelszó megjelenítése',
  STATUS_RUNNING: 'Az EMS-ESP fut',
  NUM_ERRORS: '{num} hiba',
  NUM_WARNINGS: '{num} figyelmeztetés',
  HEAP: 'Heap',
  METER_USED: '{used} / {total} {unit} felhasználva', // DeepL
  MAX_BLOCK: '{num} KB maximális blokk', // DeepL
  MAX_BLOCK_HELP: 'A „Max block” az a legnagyobb heap-tömb, amelyet egyszerre lehet allokálni. 40 KB alatt a webes felület és a MQTT meghibásodhat.', // DeepL
  FLASH_CHIP: 'Flash-chip', // DeepL
  FILE_SYSTEM: 'Fájlrendszer',
  DETAILS: 'Részletek',
  RECONNECTS: 'Újracsatlakozás', // DeepL
  CONNECTS: 'Csatlakozik', // DeepL
  ACTIVITY_HELP: 'Néhány Rx-hiba normális. A sok Tx-hiba általában azt jelenti, hogy a kazánhoz nem megfelelő Tx-mód van beállítva.', // DeepL
  PAUSE: 'Szünet',
  RESUME: 'Önéletrajz', // DeepL
  LOG_PAUSED: 'Szüneteltetve, új sorok visszatartva: {num}', // DeepL
  FILTER_LINES: 'Szűrősorok', // DeepL
  SCHEDULER: 'Ütemező',
  SCHEDULER_HELP_1: 'Parancsokat automatizálhatja az alábbiakban megadott ütemezett események hozzáadásával', // DeepL
  SCHEDULER_HELP_2: 'indításkor',
  SCHEDULE: 'Ütemező', // DeepL
  TIME: 'Idő', // DeepL
  TIMER: 'Időzítő',
  ONCHANGE: 'Változás esetén', // DeepL
  CONDITION: 'Feltétel',
  SCHEDULE_UPDATED: 'Ütemező frissült', // DeepL
  CUSTOM_ENTITIES: 'Egyéni entitások', // DeepL
  ENTITIES_HELP_1: 'Egyéni EMS-entitások vagy dinamikus felhasználói változók definiálása', // DeepL
  ENTITIES_UPDATED: 'Frissített entitások', // DeepL
  WRITEABLE: 'Írható',
  SHOWING: 'Megjelenítés', // DeepL
  SEARCH: 'Keresés',
  CERT: 'CA-tanúsítvány (PEM)',
  ENABLE_TLS: 'TLS engedélyezése',
  TLS_CHECK: 'Ellenőrizze a brókert a', // DeepL
  TLS_CHECK_CA: 'CA-tanúsítvány',
  TLS_CHECK_FINGERPRINT: 'Tanúsítvány ujjlenyomata (SHA-1)',
  TLS_CHECK_NONE: 'Nincs, nem jelölve', // DeepL
  TLS_NOT_CHECKED: 'A kapcsolat titkosított, de a közvetítő hitelességét nem ellenőrzik, így a hálózaton lévő egy másik eszköz is eljátszhatja a szerepét.', // DeepL
  ON: 'A', // DeepL
  OFF: 'Ki',
  POLARITY: 'Polaritás',
  ACTIVEHIGH: 'Aktív magas', // DeepL
  ACTIVELOW: 'Aktív alacsony', // DeepL
  UNCHANGED: 'Változatlan',
  SECURITY_1: 'Felhasználók hozzáadása vagy eltávolítása',
  DOWNLOAD_UPLOAD_1: 'Beállítások letöltése és feltöltése',
  MODULES: 'Modulok',
  MODULES_1: 'Külső modulok aktiválása vagy deaktiválása',
  NETWORK_DESC: 'Wi-Fi, Ethernet és gépnév',
  AP_DESC: 'Saját hotspot a beállításhoz és tartalék hozzáféréshez',
  MQTT_DESC: 'Broker, témák és felderítés',
  APPLICATION_DESC: 'EMS busz, panel, szenzorok és formázás',
  NTP_DESC: 'Időszerver és időzóna',
  FIRMWARE_DESC: 'Verzió, frissítések és helyreállítás',
  MODULES_UPDATED: 'Frissített modulok', // DeepL
  MODULES_NONE: 'Nem észlelt külső modulokat', // DeepL
  RENAME: 'Átnevezés',
  ENABLE_MODBUS: 'Modbus engedélyezése',
  VIEW_LOG: 'A problémák diagnosztizálásához tekintse meg a naplófájlt', // DeepL
  SERVICES: 'Szolgáltatások',
  ALLVALUES: 'Minden érték', // DeepL
  WAIT_FIRMWARE: 'A firmware feltöltése és telepítése folyamatban van', // DeepL
  INSTALL_VERSION: 'Ez telepíti a következő verziót: {0}',
  UPDATE_AVAILABLE: 'frissítés elérhető', // DeepL
  LATEST_VERSION: 'A legújabb firmware-verziót használja ({0})',
  PLEASE_WAIT: 'Kérjük, várjon', // DeepL
  RESTARTING_PRE: 'Indítás',
  RESTARTING_POST: 'Előkészítés',
  AUTO_SCROLL: 'Automatikus görgetés',
  DASHBOARD: 'Vezérlőpult',
  DEVELOPER_MODE: 'Fejlesztői mód',
  DISABLE_RESET: 'Gyári beállítások visszaállítása letiltása', // DeepL
  AUTO_FW_CHECK: 'A firmware-frissítések automatikus ellenőrzése', // DeepL
  BYTES: 'Bájt', // DeepL
  BITMASK: 'Bitmaszk',
  DUPLICATE: 'Duplikátum', // DeepL
  DASHBOARD_1: 'Az alábbiakban az összes kedvenceként megjelölt aktív EMS-entitás, valamint az összes egyéni entitás, ütemezés és külső szenzor látható.', // DeepL
  NO_GPIO: 'Nincs elérhető GPIO', // DeepL
  PLATFORM: 'Platform',
  RELEASE_TYPE: 'Kiadás Típusa',
  FIRMWARE_VERSION_INFO: 'Firmware-verzió Információk', // DeepL
  USER_PROFILE: 'Felhasználói profil',
  ONLINE_HELP: 'online súgó',
  UPGRADE_IMPORTANT_MESSAGES: 'Fontos Frissítési Üzenetek', // DeepL
  UPGRADE_IMPORTANT_MESSAGES_2: 'Új főverzióra frissít. Célszerű először biztonsági másolatot készíteni a rendszerről. Ellenőrizze az online súgót is, hátha vannak olyan változások, amelyek a rendszer működését megzavarhatják.', // DeepL
  UPGRADE_IMPORTANT_MESSAGES_3: 'A 3.10-es verzió jelentős biztonsági változásokat tartalmaz. Először készítsen rendszermentést: a frissítés után a jelszavak hash-elt formában kerülnek tárolásra, és a régebbi verziók nem tudják azokat olvasni, ezért a visszaállításhoz szükség van erre a mentésre. Minden API-híváshoz API-kulcsra lesz szükség, ezért az olyan integrációkat, mint például a Home Assistant, frissíteni kell. Mielőtt továbbmenne, olvassa el a jelentős változásokat az online súgóban.', // DeepL
  WARNING_SYSTEM_BACKUP: 'Ezzel biztonsági másolatot készít a rendszer teljes konfigurációjáról és beállításairól. A jelszavak és kulcsok nem kerülnek bele, hacsak azokat alább nem adja meg; a biztonsági másolat visszaállítása során az eszközön már meglévő jelszavak megmaradnak. A parancsok mindig benne vannak, és tartalmazhatnak hitelesítő adatokat, ezért legyen óvatos a megosztásukkal. Folytatni szeretné?', // DeepL
  BACKUP_INCLUDE_SECRETS: 'Foglalja bele a jelszavakat és kulcsokat (amelyek szükségesek az új eszközre való visszaállításhoz)', // DeepL
  TEST_EMAIL_SUCCESSFUL: 'Teszt-e-mail elküldve', // DeepL
  SYSTEM_NAME: 'Rendszer neve',
  COMMANDS: 'Parancsok',
  COMMANDS_HELP_1: 'Az alábbiakban határozza meg az újrafelhasználható, névvel ellátott parancsokat. Ezeket a konzolról, API/MQTT-n keresztül lehet végrehajtani, vagy az Ütemező hivatkozhat rájuk.', // DeepL
  COMMAND_EXECUTED: 'Parancsok végrehajtva', // DeepL
  RUN: 'Futtatás',
  INTERNET_CONNECTION_CHECK: 'Az elérhető verziók online ellenőrzése', // DeepL
  UPLOAD_BACKUP_TEXT: 'Kattintson a kiválasztáshoz, vagy helyezzen el itt egy biztonsági másolatot (.json)', // DeepL
  UPLOAD_FIRMWARE_TEXT: 'Kattintson a kiválasztáshoz, vagy helyezzen el itt egy .bin/.md5 firmware-fájlt', // DeepL
  UPLOAD_MD5_RECEIVED: 'A kivonat beérkezett, most töltsd fel a firmware .bin fájlt', // DeepL
  UPLOAD_MD5_MATCHED: 'MD5-egyezés, firmware telepítése…', // DeepL
  UPLOAD_NOT_OFFICIAL: 'Ez nem egy hivatalos EMS-ESP-verzió. Csak olyan firmware-t telepítsen, amelyben megbízik.', // DeepL
  UPLOAD_NOT_OFFICIAL_HELP: 'A hivatalos és az egyéni összeállításokról', // DeepL
  UPLOAD_INSTALL_ANYWAY: 'Telepítsd mindenképp', // DeepL
  OFFICIAL_BUILD: 'Hivatalos Kiadás',
  CUSTOM_BUILD: 'Egyedi Kiadás', // DeepL
  USING_CUSTOM_FIRMWARE: 'Egyedi firmware-t használ', // DeepL
  PASSWORD_CONFIRM_TITLE: 'Írja be újra a jelszavát', // DeepL
  PASSWORD_CONFIRM_TEXT: 'Biztonsági okokból kérjük, adja meg jelszavát a módosítás előtt. 5 percig nem kell újra megadnia.', // DeepL
  PASSWORD_CONFIRM_BUTTON: 'Megerősítés',
  PASSWORD_WRONG: 'Helytelen jelszó', // DeepL
  PASSWORD_NOT_CONFIRMED: 'Mégsem, a jelszót nem adták meg', // DeepL
  RECOVERY_APP: 'Recovery App', // DeepL
  RECOVERY_NOT_INSTALLED: 'Nincs telepítve',
  RECOVERY_INSTALLED: 'A helyreállítási firmware telepítve', // DeepL
  RESTART_INTO_RECOVERY: 'Indítsa újra a Recovery App alkalmazást', // DeepL
  RECOVERY_NOT_FOUND: 'A Recovery App nem található az interneten', // DeepL
  RECOVERY_START_FAILED: 'A Recovery App nem indítható el, lehet, hogy elavult vagy sérült. Először telepítse a legújabb verziót', // DeepL
  PARTITION_START_FAILED: 'Ez a firmware nem indítható el, valószínűleg megsérült', // DeepL
  INSTALLED_VERSIONS: 'Telepített verziók',
  EMSESP_APP: 'EMS-ESP alkalmazás',
  NO_UPDATES_AVAILABLE: 'nincs elérhető frissítés',
  SWITCH_TO_DEV: 'Válts át a dev {0} verzióra', // DeepL
  SWITCH_TO_STABLE: 'Váltson a stabil kiadásra {0}', // DeepL
  SAVE: 'Mentés',
  NEEDS_RESTART: 'újraindításra van szükség', // DeepL
  CHANGED_SETTINGS: '{num} módosított beállítás',
  SAVED_RESTART: 'Mentve. Indítsa újra az EMS-ESP-t az új beállítások alkalmazásához.', // DeepL
  RESTART_NOW: 'Indítsa újra most', // DeepL
  COPY_FAILED: 'A másolás nem sikerült, jelölje ki a szöveget, és másolja át saját maga', // DeepL
  DOCUMENTATION: 'Dokumentáció',
  COMMUNITY_CHAT: 'Közösségi csevegés',
  REPORT_ISSUE: 'Hiba jelentése vagy funkciókérés', // DeepL
  GET_HELP: 'Segítség', // DeepL
  REPORT_PROBLEM: 'Probléma jelentése', // DeepL
  REPORT_STEP_1: 'A rendszerinformációk letöltése', // DeepL
  REPORT_STEP_1_TEXT: 'verzió, alaplap, EMS busz állapota, csatlakoztatott eszközök és beállítások, jelszavak nélkül', // DeepL
  REPORT_STEP_2: 'A legutóbbi napló mentése', // DeepL
  REPORT_STEP_2_TEXT: 'ha a probléma egy eszközzel kapcsolatos, először állítsa a naplószintet DEBUG-ra', // DeepL
  REPORT_STEP_3: 'Nyisson meg egy hibajelentést, vagy tegyen fel kérdést a Discordon', // DeepL
  REPORT_STEP_3_TEXT: 'Csatolja mindkét fájlt, vagy illessze be az alábbi rövid összefoglalót a csevegésbe', // DeepL
  NEW_ISSUE: 'Új probléma', // DeepL
  SHORT_SUMMARY: 'Rövid összefoglaló',
  LICENSE_TEXT: 'nyílt forráskódú, az LGPL-3.0 licenc alapján', // DeepL
  ADMIN_ACCESS_TEXT: 'teljes hozzáférés a beállításokhoz, az Eszközökhöz és az API-hoz', // DeepL
  GUEST_ACCESS_TEXT: 'megtekintheti az értékeket és végrehajthatja azokat a parancsokat, amelyeket a vendégek is végrehajthatnak', // DeepL
  LANGUAGE_BROWSER_ONLY: 'csak ebben a böngészőben változik', // DeepL
  LANGUAGE_ENTITIES_HINT: 'Az MQTT-ben és az API-ban használt eszköznevek nyelvét a következő helyen lehet beállítani:', // DeepL
  ADMINISTRATION: 'Adminisztráció',
  API_KEYS_SHORT: 'gombok a Home Assistanthez és szkriptekhez', // DeepL
  ADDRESSES: 'Címek',
  SEARCH_SETTINGS: 'Keresési beállítások', // DeepL
  NO_SETTINGS_MATCH: 'A keresésnek nem talált megfelelő beállítást', // DeepL
  GENERAL: 'Általános',
  BOARD: 'Fórum', // DeepL
  ADVANCED: 'Haladó', // DeepL
  EMS_BUS_DESC: 'Hogyan kommunikál az EMS-ESP a fűtési rendszerével?', // DeepL
  BOARD_DESC: 'A kimeneti csatlakozók a kártya profiljából származnak. Ha saját magad szeretnéd beállítani őket, kapcsold be a Fejlesztői módot, és válaszd az Egyéni lehetőséget.', // DeepL
  FORMAT_DESC: 'Hogyan jelennek meg az értékek a webes felhasználói felületen és az MQTT/API adatcsomagokban.', // DeepL
  BOARD_PINS: 'A profilból származó címkék', // DeepL
  ENABLE_EMAIL: 'E-mailes értesítések engedélyezése', // DeepL
  SEND_TEST_EMAIL: 'Teszt-e-mail küldése', // DeepL
  EMAIL_FROM: 'Forrás:', // DeepL
  EMAIL_TO: 'Hogy', // DeepL
  EMAIL_SUBJECT: 'Tárgy',
  WIFI_SCAN: 'Wi-Fi-letapogatás', // DeepL
  SCAN: 'Beolvasás', // DeepL
  NETWORKS_NEARBY: 'Közeli hálózatok',
  SIGNAL: 'Jel', // DeepL
  CHANNEL: 'Csatorna',
  WIFI_OPEN: 'nyitott', // DeepL
  WIFI_SAVED: 'mentve', // DeepL
  NETWORK_ETHERNET_INFO: 'Jelenleg Ethernet-kapcsolaton keresztül csatlakozik ({ip}). A Wi-Fi-t csak kábel nélkül használjuk.', // DeepL
  HOSTNAME_ACCESS: 'Gépnév és hozzáférés', // DeepL
  IP_ADDRESS: 'IP-cím',
  NETWORK_ETH_10MBIT: '10 Mbit-es kényszerített félduplex', // DeepL
  NETWORK_ETH_10MBIT_HELP: 'csak problémás kapcsolók vagy hosszú kábelek esetén', // DeepL
  NETWORK_DISABLE_SLEEP_HELP: 'megbízhatóbb, valamivel több energiát fogyaszt', // DeepL
  NETWORK_MDNS_HELP: '{host}.local néven érhető el mDNS-en keresztül', // DeepL
  NETWORK_FIXED_IP_HELP: 'ellenkező esetben a DHCP kerül használatra', // DeepL
  NETWORK_CORS_HELP: 'csak akkor szükséges, ha egy másik weboldal hívja meg az API-t', // DeepL
  NTP_TIME_ON_DEVICE: 'idő az EMS-ESP-n',
  NTP_TIME_SERVER: 'Időszerver',
  NTP_THERMOSTAT_CLOCK: 'Termosztát Óra', // DeepL
  NTP_THERMOSTAT_SYNC: 'Állítsa be a termosztát óráját', // DeepL
  NTP_THERMOSTAT_SYNC_HELP: 'beállítja a dátumot és az időt az EMS termosztáton',
  NTP_THERMOSTAT_SYNC_1: 'Be, EMS-ESP időzóna', // DeepL
  NTP_THERMOSTAT_SYNC_2: 'Be, saját időzóna',
  NTP_THERMOSTAT_TZ: 'A termosztát időzónája', // DeepL
  MQTT_PREVIEW: 'Mi kerül közzétételre',
  MQTT_PREVIEW_CMD: 'parancsként elfogadott', // DeepL
  MQTT_PREVIEW_RESPONSE: 'parancsok kimenete',
  MQTT_BEHAVIOUR: 'Viselkedés',
  MQTT_TOPICS: 'Témák és hasznos adatok', // DeepL
  MQTT_DISCOVERY: 'Felfedezés', // DeepL
  MQTT_INTERVALS_HELP: 'A közzétételek közötti idő másodpercben, 0 = minden változáskor közzététel.', // DeepL
  MQTT_SINGLE_NO_DISCOVERY: 'nem érhető el a felfedezés funkcióval', // DeepL
  MQTT_DISCOVERY_NO_SINGLE: 'nem érhető el egyértékű témáknál a', // DeepL
  MQTT_DISCOVERY_HELP: 'automatikusan létrehozza az entitásokat a Home Assistantben vagy a Domoticzban', // DeepL
  MQTT_ENTITY_FORMAT_WARN: 'ennek megváltoztatása a Home Assistant minden entitásának átnevezését eredményezi', // DeepL
  USERS: 'Felhasználók',
  USERS_TEXT: 'A rendszergazdák mindent módosíthatnak. A vendégek csak böngészhetnek, és a vendégek számára kijelölt parancsokat hajthatnak végre.', // DeepL
  ADD_USER: 'Felhasználó hozzáadása',
  EDIT_USER: 'Felhasználó szerkesztése',
  ROLE: 'Szerep', // DeepL
  ROLE_ADMIN: 'admin', // DeepL
  ROLE_GUEST: 'vendég',
  ADMIN: 'Admin', // DeepL
  YOU: 'te', // DeepL
  NEW_PASSWORD: 'Új jelszó',
  UNSAVED_CHANGES: '{num} nem mentett módosítás',
  CREATE_KEY: 'Kulcs Létrehozása', // DeepL
  NEW_API_KEY: 'Új API-kulcs',
  NO_API_KEYS: 'Még nincsenek API-kulcsok', // DeepL
  API_KEYS_LIMIT: 'Legfeljebb {num} gombot használhat.', // DeepL
  REVOKE: 'Visszavonás',
  REVOKE_KEY: 'API-kulcs Visszavonása',
  REVOKE_KEY_TEXT: 'Bármi, ami a „{name}” kifejezést tartalmazza, azonnal leáll.', // DeepL
  MODULES_TEXT: 'A firmware-be beépített opcionális funkciók. Itt kapcsolhatja be vagy ki őket.', // DeepL
  BY_AUTHOR: 'szerző: {name}',
  LICENSE_KEY: 'Licenckulcs',
  MODULE_ACTIVATED: 'aktiválva',
  MODULE_PENDING: 'aktiválásra vár',
  ENABLED: 'Engedélyezve',
  UNDO: 'Visszavonás',
  FW_UPDATES: 'Frissítések',
  FW_VERSION_AVAILABLE: 'A {version}-es verzió elérhető', // DeepL
  FW_RELEASE_NOTES: 'Kiadási megjegyzések',
  FW_UPDATES_OFFLINE: 'Nem sikerült online ellenőrizni az új verziókat. Ehhez az EMS-ESP-nek internetkapcsolatra van szüksége.', // DeepL
  FW_RESTART_RESET: 'Újraindítás és Visszaállítás',
  FW_RECOVERY_TEXT: 'Újraindítás a Recovery App alkalmazásba? Csatlakozik a hálózatához, és elindítja a saját hozzáférési pontját is, ahová feltöltheti a firmware-t.', // DeepL
  FW_SIZE: 'Méret',
  FW_INSTALL_DATE: 'Telepítve a', // DeepL
  FW_UPLOAD_FAILED: 'A feltöltés sikertelen: helytelen fájltípus vagy egy másik kártyához tartozó firmware', // DeepL
  FILE_TYPE_INVALID: 'Ez a fájltípus nem támogatott, válasszon egy {types} fájlt', // DeepL
  WHO_CAN_RUN: 'Ki futtathatja',
  CMD_NONE: 'Nem található parancs', // DeepL
  CMD_NONE_MATCH: 'A kereséshez nem találtak megfelelő parancsokat', // DeepL
  SCHEDULE_NONE: 'Nincs ütemezés', // DeepL
  SCHEDULE_NONE_MATCH: 'A keresésnek nem felel meg egyetlen ütemezés sem', // DeepL
  CE_NONE: 'Nem található elem', // DeepL
  CE_ALL: 'Minden', // DeepL
  CE_STORAGE: 'Tárolás',
  CE_SOURCE: 'Forrás',
  CE_EMS_SOURCE: 'eszköz {device} · telegram {type} · eltolás {offset}', // DeepL
  CE_RAM_TEXT: 'az újraindításig megmarad', // DeepL
  CE_NVS_TEXT: 'az újraindítások után is megmaradnak', // DeepL
  CE_EXCLUDED: 'nem az API/MQTT-ben', // DeepL
  SEARCH_SENSORS: 'Név, azonosító vagy GPIO keresése', // DeepL
  NO_SENSORS: 'Nem található senkivel sem', // DeepL
  NO_SENSORS_MATCH: 'A keresésnek nem felel meg egyetlen szensor sem', // DeepL
  TEMP_SENSORS_HELP: 'Automatikusan felismerve. Kattintson egy szenzorra annak átnevezéséhez vagy az eltolás beállításához', // DeepL
  FREE_GPIOS: 'Szabad GPIO-k',
  SYSTEM_SENSOR: 'Rendszer-szenzor', // DeepL
  MASK_FAVORITE: 'Kedvencek', // DeepL
  MASK_READONLY: 'Írási korlátozás', // DeepL
  MASK_NO_API: 'Nincs MQTT/API',
  MASK_HIDDEN: 'Rejtett',
  MASK_REMOVED: 'Eltávolítva',
  FILTER_ALL: 'Minden', // DeepL
  NUM_SELECTED: 'Kiválasztva: {num}', // DeepL
  ALL_SHOWN: 'Összes megjelenítése: {num}', // DeepL
  BULK_HINT: 'A gombok a bejelölt sorokat módosítják, illetve ha egyik sem bejelölve, akkor az összes megjelenített sort.', // DeepL
  CLEAR_SELECTION: 'Egyértelmű kiválasztás', // DeepL
  NO_ENTITIES_MATCH: 'Nincs találat', // DeepL
  LIMIT_HELP: 'hagyja üresen a beépített korlát miatt', // DeepL
  MIN_ABOVE_MAX: 'A minimum nem haladhatja meg a maximumot', // DeepL
  RENAME_DEVICE: 'Eszköz Átnevezése',
  WEEKLY: 'Heti',
  EVERY_DAY: 'Minden nap',
  SCHEDULER_NO_COMMANDS: 'Először adjon hozzá egy parancsot a Parancsok oldalon',
  DU_SETTINGS: 'EMS-ESP beállítások',
  DU_SETTINGS_TEXT: 'Az EMS-ESP beállításainak biztonsági mentése: hálózat, MQTT, egyedi beállítások, Ütemező, egyéni entitások és Modulok. A beállítások vagy a biztonsági mentés fájljának feltöltése újraindítja az EMS-ESP-t.', // DeepL
  HB_TITLE: 'Fűtési beállítások',
  HB_TEXT: 'Elmentek a kazán, a termosztát és más EMS-eszközök aktuális értékeit, így azokat később visszaállíthatja, például egy eszköz cseréje után. Az írásvédett entitások és parancsok soha nem változnak meg egy visszaállítás során.', // DeepL
  HB_BACKUP: 'Fűtési Beállítások Mentése', // DeepL
  HB_RESTORE: 'Fűtési beállítások visszaállítása',
  DATA_EXPORT: 'Adatexport', // DeepL
  DATA_EXPORT_TEXT: 'Támogatás és elemzés céljából. Ezeket a fájlokat nem lehet visszatölteni.', // DeepL
  NUM_VALUES: '{num} érték',
  HB_NOTHING: 'nincs mit alátámasztani', // DeepL
  FILE_NAME: 'Fájlnév',
  HB_NOTE: 'Megjegyzés (opcionális, a visszaállításkor újra megjelenik)', // DeepL
  HB_INCOMPLETE: 'Azok az értékek, amelyeket még nem olvastak ki a buszról, kimaradnak. Újraindítás után várjon néhány percet a teljes biztonsági mentés elkészültéig.', // DeepL
  HB_RESTORE_TEXT: 'Válasszon egy fűtési beállítások mentést. A változtatások ellenőrzése és megerősítése előtt semmi sem kerül rögzítésre.', // DeepL
  HB_DROP: 'Kattintson a kiválasztáshoz, vagy helyezzen el itt egy fűtési biztonsági másolatot (.json)', // DeepL
  HB_INVALID: 'Ez nem a fűtési beállítások biztonsági mentése', // DeepL
  HB_OTHER_FILE: 'Válasszon egy másik fájlt', // DeepL
  HB_SAME_DEVICE: 'ugyanaz az eszköz',
  HB_NOT_CONNECTED: 'nincs csatlakoztatva',
  HB_OTHER_DEVICE: 'másik eszköz (termékazonosító {backup} a tartalékban, {now} csatlakoztatva), kihagyva', // DeepL
  HB_UNIT_MISMATCH: 'Ez a biztonsági másolat a hőmérsékleteket {unit} mértékegységben tartalmazza. A visszaállítás előtt módosítsa a hőmérséklet mértékegységét az Alkalmazás beállításaiban.', // DeepL
  HB_CHANGED: 'Módosítva', // DeepL
  ALL: 'Minden', // DeepL
  HB_NOW: 'Most',
  HB_BACKUP_VALUE: 'Biztonsági másolat', // DeepL
  HB_NOT_AVAILABLE: 'nem elérhető',
  HB_SENDING: 'küldés',
  HB_WRITTEN: 'írva', // DeepL
  HB_RESTORE_NUM: '{num} érték visszaállítása',
  RESTORE: 'Visszaállítás',
  HB_CONFIRM: 'Kiírja a(z) {file} fájlból a(z) {num} értéket az EMS-eszközökre?',
  HB_CONFIRM_TEXT: 'Az értékeket egyenként küldjük el. Ha egy eszköz egy értéket elutasít, a többit akkor is elküldjük.', // DeepL
  HB_DONE: '{num} érték kiírva',
  HB_DONE_FAILED: '{num} hiba történt. Az eszközök gyakran elutasítják a telepítői szintű értékeket; a rendszernapló tartalmazza a részleteket.', // DeepL
  HB_NO_CHANGES: 'Semmi sem különbözik a biztonsági másolattól', // DeepL
  HB_DIFFERENT_DEVICE: 'különböző eszközök', // DeepL
  SELECT_ALL: 'Összes kijelölése',
  DASHBOARD_EMPTY: 'Még nincs mit megjeleníteni', // DeepL
  NO_DEVICES: 'Még nem találtak EMS-eszközöket', // DeepL
  ADD_COMMAND: 'Parancsok hozzáadása', // DeepL
  EDIT_COMMAND: 'Szerkesztés parancs', // DeepL
  RUN_COMMAND: 'Parancs futtatása',
  ADD_SCHEDULE: 'Ütemezés hozzáadása',
  EDIT_SCHEDULE: 'Ütemezés szerkesztése',
  ADD_ENTITY: 'Entitások hozzáadása', // DeepL
  EDIT_ENTITY: 'Entitás Szerkesztése', // DeepL
  ADD_ANALOG_SENSOR: 'Analóg szenzor hozzáadása',
  EDIT_ANALOG_SENSOR: 'Analóg szenzor szerkesztése',
  REMOVE_ANALOG_SENSOR: 'Analóg Szenzor Eltávolítása', // DeepL
  REMOVE_SENSOR_TEXT: 'A „{name}” szenzort azonnal eltávolítjuk.', // DeepL
  EDIT_TEMP_SENSOR: 'Hőmérséklet-érzékelő Szerkesztése', // DeepL
  INSTALL_FIRMWARE: 'Firmware Telepítése ({channel})',
  PARTITION: 'Partíció',
  PULSE: 'Pulse', // DeepL
  SYS_HOME: 'Főoldal', // draft, please review
  SYS_HOME_HELP: 'A legfontosabb dolgok egy pillantással. Minden más a Rendszer menüben található.', // draft, please review
  SYS_MENU: 'Rendszer menü', // draft, please review
  SYS_MENU_HELP: 'A rendszer összes értéke és beállítása, témák szerint rendezve.', // draft, please review
  SYS_FAV_HELP: 'A legfontosabb beállításaid. Az érték melletti csillaggal módosíthatod a listát.', // draft, please review
  SYS_FAV_NONE: 'Még nincs kedvenc. Jelölj meg értékeket a csillaggal.', // draft, please review
  SYS_FAV_REMOVED: 'Eltávolítva a kedvencek közül', // draft, please review
  SYS_GROUP_HEATING: 'Fűtés', // draft, please review
  SYS_GROUP_PLANT: 'Berendezés', // draft, please review
  SYS_GROUP_OVERVIEW: 'Áttekintés', // draft, please review
  SYS_LEVEL_MOVE_ALL: 'Csak az összes értéknél jelenjen meg', // draft, please review
  SYS_LEVEL_MOVE_NORMAL: 'Megjelenítés a normál nézetben', // draft, please review
  SYS_HC: 'Fűtési kör {0}', // draft, please review
  SYS_DHW: 'Melegvíz', // draft, please review
  SYS_AHS: 'Kiegészítő fűtés', // draft, please review
  SYS_HS: 'Hőforrás', // draft, please review
  SYS_SRC: 'Forrás', // draft, please review
  SYS_OTHER_DEVICES: 'További eszközök', // draft, please review
  SYS_SERVICE: 'Karbantartás és hibák', // draft, please review
  SYS_ENERGY: 'Energia és üzemidők', // draft, please review
  SYS_SEC_MODE: 'Üzemmód', // draft, please review
  SYS_SEC_TEMPS: 'Hőmérsékletek', // draft, please review
  SYS_SEC_STATUS: 'Állapot', // draft, please review
  SYS_SEC_ADVANCED: 'Fűtési görbe és telepítés', // draft, please review
  SYS_SEC_MEASURE: 'Mért értékek', // draft, please review
  SYS_SEC_OTHER: 'További értékek', // draft, please review
  SYS_LEVEL_NORMAL: 'Normál', // draft, please review
  SYS_NO_RESULTS: 'Nincs találat. Próbáld az összes értéket vagy másik kifejezést.', // draft, please review
  SYS_NOTHING_HERE: 'Normál szinten itt nincs megjeleníthető elem.', // draft, please review
  SYS_HIDDEN: 'Elrejtett szakértői értékek: {num}', // draft, please review
  SYS_SEARCH_RESULTS: 'Keresési találatok', // draft, please review
  SYS_CHANGED_WAS: 'módosítva (előtte: {0})', // draft, please review
  SYS_NO_DEVICES: 'Még nem található eszköz.', // draft, please review
  SYS_ROOM_TEMP: 'Szobahőmérséklet', // draft, please review
  SYS_WISH: 'Kívánt', // draft, please review
  SYS_CURRENT: 'Aktuális hőmérséklet', // draft, please review
  SYS_OUTDOOR: 'Külső hőmérséklet', // draft, please review
  SYS_FLOW: 'Előremenő', // draft, please review
  SYS_RETURN: 'Visszatérő', // draft, please review
  SYS_PRESSURE: 'Rendszernyomás', // draft, please review
  SYS_POWER: 'Teljesítmény', // draft, please review
  SYS_BURNER_ON: 'Az égő működik', // draft, please review
  SYS_BURNER_OFF: 'Az égő ki van kapcsolva', // draft, please review
  SYS_DHW_HEATING: 'Fűtés alatt', // draft, please review
  SYS_READY: 'Kész', // draft, please review
  SYS_ONE_TIME: 'Egyszeri töltés', // draft, please review
  SYS_NOTES: 'Megjegyzések', // draft, please review
  SYS_NOTES_SUB: 'Hibák és karbantartás', // draft, please review
  SYS_LAST_FAULT: 'Utolsó hiba', // draft, please review
  SYS_MAINT_MSG: 'Karbantartási üzenet', // draft, please review
  SYS_NEXT_MAINT: 'Következő karbantartás', // draft, please review
  SYS_FAULT_LAST: 'Utolsó hiba / üzenet', // draft, please review
  SYS_FAULT_CTRL: 'A szabályozó utolsó hibája', // draft, please review
  SYS_FAULT_SERVICE: 'Szervizkód', // draft, please review
  SYS_FAULT_ADD: 'Kiegészítő kód {0}', // draft, please review
  SYS_FAULT_RAW_ONLY: 'Ehhez a készülékhez nincs elérhető magyarázat.', // draft, please review
  SYS_FAULT_NONE: 'Nincs tárolt hiba.', // draft, please review
  SYS_FAULT_UNTIL_NOW: 'mostanáig', // draft, please review
  SYS_MAINTENANCE: 'Karbantartás', // draft, please review
};

export default hu;