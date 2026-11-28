import type { Translation } from '../i18n-types';

const pl: Translation = {
  LANGUAGE: 'Język',
  RETRY: 'Spróbuj ponownie', // DeepL
  IS_REQUIRED: 'Pole {0} jest obowiązkowe', // DeepL
  SIGN_IN: 'Zaloguj się',
  SIGN_IN_SUBTITLE: 'Brama dla systemów grzewczych EMS', // DeepL
  SIGN_OUT: 'Wyloguj się',
  USERNAME: '{{Użytkownik|Nazwa użytkownika|}}',
  PASSWORD: 'Hasło',
  APPLICATION_SETTINGS: 'Ustawienia aplikacji',
  AP_SETTINGS: 'Ustawienia punktu dostępowego',
  NETWORK_SETTINGS: 'Ustawienia sieci',
  NTP_SETTINGS: 'Ustawienia NTP',
  MQTT_SETTINGS: 'Ustawienia MQTT',
  EMS_BUS_SETTINGS: 'Ustawienia magistrali EMS',
  HELP: 'Pomoc',
  LOGGED_IN: 'Zalogowano się jako {name}', // DeepL
  PLEASE_SIGNIN: 'Zaloguj się, aby kontynuować', // DeepL
  UPLOAD_SUCCESSFUL: 'Przesłanie zakończone sukcesem', // DeepL
  DOWNLOAD_SUCCESSFUL: 'Pobierz udane', // DeepL
  INVALID_LOGIN: 'Nieprawidłowa nazwa użytkownika lub hasło', // DeepL
  NETWORK: '{{Sieć|sieci|}}',
  SECURITY: '{{B|b|}}ezpieczeństw{{o|a|}}',
  ONOFF_CAP: 'WŁ./WYŁ.', // DeepL
  ONOFF: 'włącz/wyłącz', // DeepL
  TYPE: '{{T|t|}}yp{{|u|}}',
  ENTITIES: 'encj{{i|a|e|e|i}}',
  EXPORT: 'Eksportuj',
  FAVORITES: "Ulubione",
  CLIENT_ID: 'ID klienta',
  DEVICE_ID: 'ID urządzenia',
  PRODUCT_ID: 'ID produktu',
  ENTITY_ID: 'ID encji',
  SENSOR_ID: 'ID czujnika',
  TYPE_ID: 'ID typu',
  DEVICE: 'urządzenia',
  BRAND: 'Marka',
  VERSION: 'Wersja',
  ENTITY_NAME: '{{N|n|}}azwa encji',
  VALUE: 'Wartość',
  DEVICES: 'Urządzenia',
  SENSORS: 'Czujniki',
  AUTOMATION: 'Automatyzacja',
  CONNECTIVITY: 'Łączność',
  MAINTENANCE: 'Konserwacja',
  CHANGE_VALUE: 'Zmień wartość', // DeepL
  CANCEL: 'Anuluj',
  REMOVE_ALL: 'Usuń Wszystkie', // DeepL
  APPLY_CHANGES: 'Zastosuj zmiany ({0})', // DeepL
  UPDATE: 'Aktualizacja', // DeepL
  REMOVE: 'Usuń',
  RESET: 'Resetuj',
  PROBLEM_UPDATING: 'Problem z aktualizacją', // DeepL
  PROBLEM_LOADING: 'Problem z ładowaniem', // DeepL
  ANALOG_SENSORS: 'Urządzenia podłączone do EMS-ESP',
  SETTINGS: '{{U|u|}}stawienia',
  SETTINGS_UPDATED: 'Zaktualizowano ustawienia',
  NAME_UPDATED: 'Zaktualizowano nazwę',
  SENSOR_UPDATED: 'Zaktualizowano czujnik',
  ANALOG_SENSOR_UPDATED: 'Zaktualizowano czujnik analogowy',
  OFFSET: 'Przesunięcie', // DeepL
  FACTOR: 'Czynnik', // DeepL
  FREQ: 'Częstotliwość',
  DUTY_CYCLE: 'Cykl pracy', // DeepL
  UNIT: 'Jednostka miary', // DeepL
  STARTVALUE: 'Wartość początkowa',
  EDIT: 'Edytuj', // DeepL
  TEMP_SENSORS: 'Czujniki temperatury 1-Wire®',
  WRITE_CMD_SENT: 'Wysłano polecenie', // DeepL
  EMS_BUS_WARNING: 'Rozłączono magistralę EMS. Jeśli to ostrzeżenie nadal się pojawia po kilku sekundach, sprawdź ustawienia i profil płyty.', // DeepL
  CONNECTED: '{{połączono|połączenie|}}',
  TX_ISSUES: 'Problemy z transmisją – sprawdź tryb transmisji', // DeepL
  DISCONNECTED: 'Brak połączenia', // DeepL
  DATA_TRAFFIC: 'Ruch danych', // DeepL
  EMS_DEVICE: 'Urządzenie EMS',
  SUCCESS: 'SUKCES', // DeepL
  FAIL: 'NIEZGODNOŚĆ', // DeepL
  QUALITY: 'JAKOŚĆ', // DeepL
  STATUS_NAMES: [
    'EMS, telegramy odebrane (Rx)',
    'EMS, wysłane telegramy "odczyt" (Tx)',
    'EMS, wysłane telegramy "zapis" (Tx)',
    'Odczyty czujników temperatury 1-Wire®',
    'Odczyty czujników analogowych i cyfrowych',
    'Publikacje MQTT',
    'Wywołania API',
    'Wpisy w SysLog'
  ],
  NUM_DAYS: '{num} d{{ni|zień|ni|ni|ni}}',
  NUM_SECONDS: '{num} sekun{{d|da|dy|dy|d}}',
  NUM_HOURS: '{num} godzi{{n|na|ny|ny|n}}',
  NUM_MINUTES: '{num} minu{{t|ta|ty|ty|t}}',
  APPLICATION: 'Aplikacja', // DeepL
  CUSTOMIZATIONS: 'Personalizacja',
  APPLICATION_RESTARTING: 'EMS-ESP uruchamia się ponownie', // DeepL
  BOARD_PROFILE: 'Profil płyty głównej', // DeepL
  CUSTOM: 'Niestandardowe', // DeepL
  GPIO_OF: '{0} GPIO', // DeepL
  BUTTON: 'Przycisk', // DeepL
  TEMPERATURE: 'Temperatura', // DeepL
  PHY_TYPE: 'Typ Eth PHY', // DeepL
  DISABLED: '{{wyłączono|brak|}}',
  TX_MODE: 'Tryb EMS Tx', // DeepL
  EMS_BUS: 'Magistrala EMS',
  EMS_BUS_ID: 'ID magistrali EMS',
  HARDWARE: 'Sprzęt', // DeepL
  LANGUAGE_ENTITIES: 'Język (dla elementów urządzeń)', // DeepL
  HIDE_LED: 'Ukryj diodę LED', // DeepL
  ENABLE_TELNET: 'Włącz konsolę Telnet', // DeepL
  ENABLE_ANALOG: 'Włącz czujniki analogowe', // DeepL
  CONVERT_FAHRENHEIT: 'Przelicz wartości temperatury na stopnie Fahrenheita', // DeepL
  READONLY: 'Włącz tryb tylko do odczytu (blokuje wszystkie wychodzące komendy zapisu EMS Tx)', // DeepL
  WRITE_PROTECTION: 'Należy chronić urządzenia EMS przed powtarzającym się i częstym zapisem danych', // DeepL
  WRITE_LIMIT: 'Maksymalna liczba operacji zapisu na minutę (0 = bez ograniczeń)', // DeepL
  WRITE_LIMIT_REFUSE: 'Odrzucaj wpisy przekraczające limit (w przeciwnym razie rejestrowane jest jedynie ostrzeżenie)', // DeepL
  UNDERCLOCK_CPU: 'Zmniejsz taktowanie procesora', // DeepL
  REMOTE_TIMEOUT: 'Limit czasu połączenia zdalnego', // DeepL
  REMOTE_TIMEOUT_EN: 'Wyłącz zdalne sterowanie w przypadku braku danych o temperaturze w pomieszczeniu', // DeepL
  HEATINGOFF: 'Uruchom kocioł przy wyłączonym ogrzewaniu wymuszonym', // DeepL
  MIN_DURATION: 'Czas oczekiwania', // DeepL
  ENABLE_SHOWER_TIMER: 'Włącz licznik czasu prysznica', // DeepL
  ENABLE_SHOWER_ALERT: 'Włącz powiadomienie o prysznicu', // DeepL
  TRIGGER_TIME: 'Czas wyzwolenia', // DeepL
  COLD_SHOT_DURATION: 'Czas trwania zimnego strzału', // DeepL
  FORMATTING_OPTIONS: 'Opcje formatowania',
  BOOLEAN_FORMAT_DASHBOARD: 'Format boolowski w sieci', // DeepL
  BOOLEAN_FORMAT_API: 'Format boolowski API/MQTT', // DeepL
  ENUM_FORMAT: 'Format enum API/MQTT', // DeepL
  INDEX: 'Indeks', // DeepL
  ENABLE_PARASITE: 'Włącz zasilanie pasożytnicze 1-Wire', // DeepL
  LOG_HEX: 'Rejestruj telegramy EMS w formacie szesnastkowym', // DeepL
  ENABLE_SYSLOG: 'Włącz Syslog', // DeepL
  LOG_LEVEL: 'Poziom logowania',
  MARK_INTERVAL: 'Zaznacz przedział', // DeepL
  SECONDS: 'sekund',
  MINUTES: 'minuty', // DeepL
  HOURS: 'godziny', // DeepL
  RESTART: 'Uruchom ponownie', // DeepL
  RESTART_TEXT: 'Aby zastosować wprowadzone zmiany, interfejs EMS-ESP {{musi zostać|zostanie|}} uruchomiony ponowni{{e.|e|}}',
  RESTART_CONFIRM: 'Czy na pewno chcesz ponownie uruchomić EMS-ESP?', // DeepL
  COMMAND: '{{Komenda|KOMENDA|}}',
  CUSTOMIZATIONS_RESTART: 'Wszystka personalizacja została usunięta. Uruchamianie od nowa…', // DeepL
  CUSTOMIZATIONS_FULL: 'Liczba wybranych elementów przekroczyła limit. Proszę zapisywać partiami.', // DeepL
  CUSTOMIZATIONS_SAVED: 'Zaktualizowano personalizację', // DeepL
  CUSTOMIZATIONS_HELP_1: 'Wybierz urządzenie i dostosuj opcje elementów', // DeepL
  CUSTOMIZATIONS_HELP_2: 'dodaj do Ulubionych', // DeepL
  CUSTOMIZATIONS_HELP_3: 'wyłącz akcję zapisu', // DeepL
  CUSTOMIZATIONS_HELP_4: 'wyklucz z MQTT i API',
  CUSTOMIZATIONS_HELP_5: 'ukryj z poziomu Urządzeń', // DeepL
  CUSTOMIZATIONS_HELP_6: 'usunąć z pamięci', // DeepL
  SELECT_DEVICE: 'Wybierz urządzenie', // DeepL
  MODIFIED: 'Zmodyfikowano', // DeepL
  RESET_SELECTED: 'Zresetuj zaznaczone', // DeepL
  OPTIONS: 'Opcje',
  NAME: '{{Nazwa|nazwa|}}',
  CUSTOMIZATIONS_RESET: 'Czy na pewno chcesz usunąć wszystkie personalizacje?', // DeepL
  SYSTEM_INFORMATION: '{{I|i|}}nformacj{{e|i|}} o systemie',
  HELP_INFORMATION_1: 'Odwiedź wiki online, aby uzyskać instrukcje dotyczące konfiguracji EMS-ESP', // DeepL
  HELP_INFORMATION_2: 'Aby dołączyć do czatu społeczności na żywo, dołącz do naszego serwera Discord', // DeepL
  HELP_INFORMATION_3: 'Aby zgłosić prośbę o dodanie funkcji lub zgłosić błąd', // DeepL
  HELP_INFORMATION_4: 'Podaj informacje o swoim systemie podczas zgłaszania problemu, aby uzyskać szybszą odpowiedź', // DeepL
  HELP_INFORMATION_5: 'EMS-ESP jest tworzony przez wolontariuszy. Napiwek na Ko-fi pomaga w dalszym rozwoju projektu',
  UPLOAD: 'Prześlij', // DeepL
  DOWNLOAD: 'Pobieranie',
  INSTALL: 'Zainstaluj', // DeepL
  REINSTALL: 'Zainstaluj ponownie', // DeepL
  REINSTALL_CONFIRM: 'Czy na pewno chcesz ponownie zainstalować wersję {0}?', // DeepL
  FAILED: 'nie powiodł{{o|a|}} się!',
  SYSTEM: '{{S|s||s}}yste{{m|mu||mowy}}',
  SYSTEM_LOG: 'Dziennik systemowy',
  STATUS: 'Stan',
  DOWNLOAD_UPLOAD: 'Pobierz/Prześlij', // DeepL
  CLOSE: 'Zamknij',
  COMMAND_GUEST: 'Goście mogą wykonać tę komendę', // DeepL
  TOKENS_RESET: 'Ta aktualizacja podpisuje tokeny dostępu nowym kluczem urządzenia, więc dotychczasowe tokeny dostępu do API przestały działać. Utwórz klucz API dla każdej integracji korzystającej z API, takiej jak Home Assistant, i używaj go zamiast starego tokenu.', // DeepL
  API_KEYS: 'Klucze API',
  API_KEYS_TEXT: 'Klucze API umożliwiają integrację z takimi rozwiązaniami jak Home Assistant oraz skryptami korzystającym z API. Klucz należy przesłać w nagłówku „Authorization: Bearer <KLUCZ>”. Klucze typu „Read” umożliwiają odczyt wartości i wykonywanie komend, które mogą wykonywać goście, natomiast klucze typu „Read” i „Write” umożliwiają wykonywanie wszystkich czynności, które administrator może wykonać za pośrednictwem API.', // DeepL
  API_KEY_ACCESS: 'Dostęp',
  API_KEY_READ: 'Przeczytaj', // DeepL
  API_KEY_READ_WRITE: 'Odczyt i Zapis', // DeepL
  API_KEY_SHOW_ONCE: 'Skopiuj ten klucz teraz i przechowuj go w bezpiecznym miejscu. Nie zostanie on ponownie wyświetlony.', // DeepL
  CREATED: 'Utworzono',
  COPY: 'Tekst', // DeepL
  COPIED: 'Skopiowano',
  USE: 'Używaj', // DeepL
  FACTORY_RESET: 'Reset Fabryczny', // DeepL
  SYSTEM_FACTORY_TEXT_DIALOG: 'Czy na pewno chcesz przywrócić ustawienia fabryczne EMS-ESP?', // DeepL
  STABLE: 'Stabilny', // DeepL
  DEVELOPMENT: 'Rozwój', // DeepL
  UPTIME: 'Czas sprawności systemu', // DeepL
  LAST_RESTART: 'Ostatnie ponowne uruchomienie', // DeepL
  POWER_AND_RESET: 'Zasilanie i reset', // DeepL
  FREE_MEMORY: 'Wolna pamięć', // DeepL
  BUFFER_SIZE: 'Maksymalny rozmiar bufora', // DeepL
  COMPACT: 'Zwięzły', // DeepL
  SUMMARY: 'Podsumowanie',
  SHOW_MORE: 'Pokaż {n} więcej', // DeepL
  SHOW_LESS: 'Pokaż mniej',
  COREDUMP: 'Zrzut pamięci po awarii', // DeepL
  DOWNLOAD_SYSTEM_BACKUP: 'Kopia Zapasowa Systemu',
  ERROR: 'Wystąpił nieoczekiwany błąd, spróbuj ponownie', // DeepL
  TIME_SET: 'Ustawiony czas', // DeepL
  MANAGE_USERS: 'Zarządzanie użytkownikami',
  USER_WARNING: 'Należy skonfigurować co najmniej jednego użytkownika z uprawnieniami administratora', // DeepL
  ADD: 'Doda{{j|wanie|}}',
  TELNET_ENABLED: 'Konsola Telnet jest włączona. Telnet nie jest szyfrowany, więc jeśli go nie używasz, wyłącz go w ustawieniach aplikacji.', // DeepL
  TOO_MANY_LOGINS: 'Zbyt wiele nieudanych prób logowania. Spróbuj ponownie za kilka minut.', // DeepL
  PASSWORD_KEEP: 'Pozostaw puste, aby zachować aktualne hasło', // DeepL
  SETUP_TITLE: 'Ustaw hasła', // DeepL
  SETUP_TEXT: 'To urządzenie nadal korzysta z haseł fabrycznych. Przed kontynuowaniem należy ustawić własne hasło.', // DeepL
  SETUP_ADMIN_PASSWORD: 'Nowe hasło dla {name}',
  SETUP_AP_PASSWORD: 'Nowe hasło do punktu dostępowego', // DeepL
  SETUP_AP_REQUIRED: 'Sieć Wi-Fi nie została jeszcze skonfigurowana, więc dostęp odbywa się poprzez punkt dostępowy. 8–63 znaków; zostanie użyte przy następnym uruchomieniu punktu dostępowego.', // DeepL
  SETUP_AP_OPTIONAL: 'Zalecane: 8–63 znaków; zostanie zastosowane przy następnym uruchomieniu punktu dostępowego. Pozostaw puste, aby zachować bieżącą wartość.', // DeepL
  SETUP_AP_DEFAULT: 'Punkt dostępowy nadal korzysta z hasła fabrycznego. Ustaw własne hasło w ustawieniach punktu dostępowego.', // DeepL
  SETUP_VIEW_USER: 'Opcjonalnie: dodaj użytkownika-gościa z uprawnieniami wyłącznie do przeglądania.', // DeepL
  NETWORK_CORS_ORIGIN_HELP: 'Adres internetowy umożliwiający wywołanie interfejsu API z poziomu przeglądarki, na przykład http://homeassistant.local:8123', // DeepL
  NETWORK_CORS_WILDCARD: 'Z * każda strona internetowa otwarta w przeglądarce w Twojej sieci może wywołać API. Zamiast tego ustaw dokładny adres.', // DeepL
  NETWORK_ALLOWED_HOSTS: 'Dopuszczalne nazwy hostów', // DeepL
  NETWORK_ALLOWED_HOSTS_HELP: 'Dodatkowe nazwy, pod którymi urządzenie to można otworzyć w przeglądarce, to na przykład ems-esp.fritz.box lub nazwa odwrotnego serwera proxy. Jego adres IP, nazwa hosta oraz nazwa hosta.local zawsze działają', // DeepL
  NOT_ENABLED: 'Nie włączono', // DeepL
  MQTT_ERRORS: 'Błędy MQTT',
  DISCONNECT_REASON: 'Przyczyna odłączenia', // DeepL
  MQTT_BASE_TOPIC: 'Temat podstawowy', // DeepL
  LAST_DISCONNECT: 'Ostatnie odłączenie', // DeepL
  CONNECTED_SINCE: 'Podłączony od', // DeepL
  ENABLE_MQTT: 'Włącz MQTT', // DeepL
  BROKER: 'Broker', // DeepL
  BASE_TOPIC: 'Podstawa', // DeepL
  OPTIONAL: 'opcjonalnie', // DeepL
  MQTT_FORMAT: 'Format tematu/danych użytkowych', // DeepL
  MQTT_NEST_1: 'Zagnieżdżone w jednym temacie', // DeepL
  MQTT_NEST_2: 'Poszczególne tematy', // DeepL
  MQTT_RESPONSE: 'Publikuj wyniki komend w temacie `response`', // DeepL
  MQTT_PUBLISH_TEXT_1: 'Publikuj tematy o pojedynczej wartości po wystąpieniu zmiany', // DeepL
  MQTT_PUBLISH_TEXT_2: 'Publikuj w tematach komend (ioBroker)', // DeepL
  MQTT_PUBLISH_TEXT_3: 'Włącz wykrywanie urządzeń MQTT', // DeepL
  MQTT_PUBLISH_TEXT_4: 'Prefiks dla tematów wykrywania', // DeepL
  MQTT_PUBLISH_TEXT_5: 'Typ odkrycia', // DeepL
  MQTT_INPUT_NUMBER_FORMAT: 'Format liczb wprowadzanych', // DeepL
  MQTT_PUBLISH_INTERVALS: 'Częstotliwość publikacji', // DeepL
  MQTT_INT_BOILER: 'Kotły i pompy ciepła',
  MQTT_INT_THERMOSTATS: 'Termostaty',
  MQTT_INT_SOLAR: 'Moduły słoneczne', // DeepL
  MQTT_INT_MIXER: 'Moduły mieszające', // DeepL
  MQTT_INT_WATER: 'Moduły wodne', // DeepL
  MQTT_QUEUE: 'Kolejka MQTT',
  DEFAULT: '{{Pozostałe|Domyślna|}}',
  MQTT_ENTITY_FORMAT: 'Format identyfikatora encji', // DeepL
  MQTT_ENTITY_FORMAT_0: 'Pojedynczy egzemplarz, długa nazwa (wersja 3.4)', // DeepL
  MQTT_ENTITY_FORMAT_1: 'Pojedyncza instancja, krótka nazwa', // DeepL
  MQTT_ENTITY_FORMAT_2: 'Wiele instancji, krótka nazwa', // DeepL
  MQTT_CLEAN_SESSION: 'Ustaw czystą sesję', // DeepL
  MQTT_RETAIN_FLAG: 'Zawsze należy ustawić flagę zachowania', // DeepL
  MQTT_COMMANDS: 'Komendy odbierane przez protokół MQTT', // DeepL
  MQTT_COMMANDS_0: 'Wyłączone, tylko odczyt wartości',
  MQTT_COMMANDS_1: 'Zmiana wartości i uruchamianie komend', // DeepL
  MQTT_COMMANDS_2: 'Wszystko, w tym komendy systemowe', // DeepL
  INACTIVE: 'nieaktywn{{y|a|}}',
  ACTIVE: 'Aktywne', // DeepL
  UNKNOWN: 'Nieznane', // DeepL
  SET_TIME: '{{Ustaw Zegar|Ustawianie Zegara|}}',
  SET_TIME_TEXT: 'Wprowadź poniżej lokalną datę i godzinę, aby ustawić czas', // DeepL
  LOCAL_TIME: '{{Czas|czasu|}} lokaln{{y|ego|}}',
  UTC_TIME: 'Czas UTC',
  ENABLE_NTP: 'Włącz NTP', // DeepL
  NTP_SERVER: 'Serwer NTP',
  TIME_ZONE: 'Strefa czasowa',
  ACCESS_POINT: '{{Punkt|punktu|}} {{dostępowy|dostępowego|}}',
  AP_PROVIDE: 'Włącz punkt dostępowy', // DeepL
  AP_PROVIDE_TEXT_2: 'W przypadku braku łączności sieciowej', // DeepL
  AP_PROVIDE_TEXT_3: 'Nigdy', // DeepL
  AP_PREFERRED_CHANNEL: 'Preferowany kanał',
  AP_HIDE_SSID: 'Ukryj identyfikator SSID', // DeepL
  AP_CLIENTS: 'Klienci AP', // DeepL
  AP_MAX_CLIENTS: 'Maksymalna liczba klientów',
  AP_LOCAL_IP: 'Lokalny adres IP',
  IDLE: 'Bezczynność', // DeepL
  SCAN_AGAIN: 'Przeskanuj ponownie', // DeepL
  NETWORK_NO_WIFI: 'Nie znaleziono sieci Wi-Fi', // DeepL
  NETWORK_BLANK_BSSID: 'pozostaw puste, aby używać wyłącznie identyfikatora SSID', // DeepL
  TX_POWER: 'Moc nadawcza', // DeepL
  HOSTNAME: 'Nazwa hosta', // DeepL
  NETWORK_DISABLE_SLEEP: 'Wyłącz tryb uśpienia Wi-Fi', // DeepL
  NETWORK_LOW_BAND: 'Należy korzystać z niższej przepustowości sieci Wi-Fi', // DeepL
  NETWORK_USE_DNS: 'Włącz usługę mDNS', // DeepL
  NETWORK_ENABLE_CORS: 'Włącz CORS', // DeepL
  NETWORK_CORS_ORIGIN: 'Źródło CORS', // DeepL
  NETWORK_FIXED_IP: 'Należy używać stałego adresu IP', // DeepL
  NETWORK_GATEWAY: 'Brama',
  NETWORK_SUBNET: 'Maska podsieci',
  NETWORK_DNS: 'Serwery DNS',
  ADDRESS_OF: 'Adres {0}',
  ADMINISTRATOR: 'Administrator', // DeepL
  GUEST: 'Gość', // DeepL
  NEW: 'nowe{{go|j|}}',
  NEW_ENTITY_NAME: 'Nowa nazwa encji',
  MIN: 'min', // DeepL
  MAX: 'max', // DeepL
  BLOCK_NAVIGATE_1: 'Masz niezapisane zmiany', // DeepL
  BLOCK_NAVIGATE_2: 'Jeśli przejdziesz do innej strony, niezapisane zmiany zostaną utracone. Czy na pewno chcesz opuścić tę stronę?', // DeepL
  UPLOAD_ABORTED: 'Przerwano przesyłanie', // DeepL
  BLOCK_BUSY_1: 'W trakcie opracowywania', // DeepL
  BLOCK_BUSY_2: 'Opuszczenie tej strony spowoduje jej zamknięcie. Czy na pewno chcesz ją opuścić?', // DeepL
  DISCARD: 'Odrzuć',
  COMMANDS_UPDATED: 'Zaktualizowane komendy', // DeepL
  V_REQUIRED: 'Wymagane',
  V_BETWEEN: 'Długość musi mieścić się w przedziale od {min} do {max}', // DeepL
  V_MAX_CHARS: 'Musi mieć maksymalnie {max} znaków', // DeepL
  V_CHARS: 'Długość musi wynosić od {min} do {max} znaków', // DeepL
  V_IN_USE: 'Już w użyciu',
  V_IP: 'Musi to być adres IP', // DeepL
  V_HOSTNAME: 'Musi to być poprawna nazwa hosta', // DeepL
  V_IP_OR_HOSTNAME: 'Musi to być adres IP lub nazwa hosta', // DeepL
  V_NAME: 'Maksymalnie {max} znaków: litery, cyfry lub znak _', // DeepL
  V_USERNAME: 'Maksymalnie {max} znaków: litery, cyfry, znak „_” lub „.”', // DeepL
  V_DAY: 'Wybierz co najmniej jeden dzień',
  V_HEX: 'Musi to być liczba szesnastkowa', // DeepL
  V_FINGERPRINT: 'Musi to być odcisk SHA-1 (40 cyfr szesnastkowych)', // DeepL
  V_EXAMPLE: 'Powinny wyglądać tak: {example}', // DeepL
  V_NOT_DEFAULT: 'Wybierz inne hasło',
  ZERO_DISABLES: '0 wyłącza urządzenie', // DeepL
  MINUS_ONE_DISABLES: '-1 wyłącza urządzenie', // DeepL
  LED_TYPE: 'Typ diody LED', // DeepL
  PHY_POWER: 'Zasilanie PHY',
  PHY_CLOCK: 'Zegar PHY',
  AUTO: 'Auto',
  PORT: 'Port',
  TIMEOUT: 'Limit czasu',
  HOST: 'Host',
  SMTP_SERVER: 'Serwer SMTP',
  KEEP_ALIVE: 'Utrzymuj połączenie', // DeepL
  MQTT_REFRESH: 'Odśwież MQTT',
  MQTT_REFRESHED: 'Aktualizacja MQTT', // DeepL
  MQTT_R_USER: 'Odłączone przez użytkownika', // DeepL
  MQTT_R_TCP: 'Połączenie TCP zostało zamknięte', // DeepL
  MQTT_R_PROTOCOL: 'Wersja protokołu nie została zaakceptowana', // DeepL
  MQTT_R_ID: 'Odrzucono identyfikator klienta', // DeepL
  MQTT_R_SERVER: 'Serwer niedostępny',
  MQTT_R_CREDENTIALS: 'Nieprawidłowe dane uwierzytelniające', // DeepL
  MQTT_R_AUTH: 'Brak uprawnień', // DeepL
  MQTT_R_FINGERPRINT: 'Odcisk TLS nie pasuje',
  GPIO_INVALID: 'Nieprawidłowe ustawienia GPIO',
  CONNECTED_VIA: 'Połączono za pośrednictwem {0}', // DeepL
  NETWORK_NOT_FOUND: 'Nie znaleziono sieci',
  CONNECTION_FAILED: 'Nie udało się nawiązać połączenia', // DeepL
  CONNECTION_LOST: 'Utracono połączenie',
  AP_LINGERING: 'Oczekiwanie do momentu bezczynności', // DeepL
  NONE: 'Brak',
  LOG_NO_CONNECTION: 'Brak łączności z usługą logowania', // DeepL
  CHECK_SYSTEM_LOG: 'Sprawdź dziennik systemowy pod kątem możliwych przyczyn', // DeepL
  VALUE_TYPE: 'Typ wartości',
  DEFAULT_VALUE: 'Wartość domyślna',
  DEFAULT_NAME: 'Nazwa domyślna',
  ALWAYS_OFF: 'Zawsze wyłączone',
  ALWAYS_ON: 'Zawsze włączone',
  NO_DATA: 'Nie ma jeszcze żadnych ulubionych elementów. Zaznacz je w sekcji {customizations} lub przejrzyj wszystkie elementy w sekcji {devices}.', // DeepL
  STAY: 'Pozostań',
  LEAVE: 'Zostaw', // DeepL
  MENU: 'Menu',
  MORE: 'Więcej',
  SHOW_PASSWORD: 'Pokaż hasło',
  STATUS_RUNNING: 'EMS-ESP działa',
  NUM_ERRORS: '{num} {{błędów|błąd|błędy|błędy|błędów|błędu}}',
  NUM_WARNINGS: '{num} ostrzeże{{ń|nie|nia|nia|ń|nia}}',
  HEAP: 'Heap',
  METER_USED: '{used} / {total} {unit} zużyte', // DeepL
  MAX_BLOCK: 'Maksymalna długość bloku: {num} KB', // DeepL
  MAX_BLOCK_HELP: 'Max block to największy fragment pamięci dynamicznej, jaki można przydzielić jednorazowo. Przy wartości poniżej 40 KB interfejs internetowy i protokół MQTT mogą przestać działać.', // DeepL
  FLASH_CHIP: 'Układ Flash', // DeepL
  FILE_SYSTEM: 'System plików',
  DETAILS: 'Szczegóły',
  RECONNECTS: 'Ponowne połączenie', // DeepL
  CONNECTS: 'Łączy', // DeepL
  ACTIVITY_HELP: 'Kilka nieudanych prób odbioru (Rx) jest normalne. Liczne nieudane próby wysyłania (Tx) zazwyczaj oznaczają, że tryb wysyłania (Tx) jest nieodpowiedni dla danego kotła.', // DeepL
  PAUSE: 'Pauza', // DeepL
  RESUME: 'Wznowienie', // DeepL
  LOG_PAUSED: 'Wstrzymane, nowe wiersze wstrzymane: {num}', // DeepL
  FILTER_LINES: 'Wiersze filtru', // DeepL
  SCHEDULER: 'Harmonogram',
  SCHEDULER_HELP_1: 'Zautomatyzuj komendy, dodając poniżej zaplanowane zdarzenia w Harmonogramie', // DeepL
  SCHEDULER_HELP_2: 'przy uruchomieniu', // DeepL
  SCHEDULE: '{{H|h|}}armonogram{{|u|}}',
  TIME: '{{Czas|Godzina|}}',
  TIMER: '{{m|M|}}inutnik',
  ONCHANGE: 'W przypadku zmian', // DeepL
  CONDITION: 'Warunek', // DeepL
  SCHEDULE_UPDATED: 'Harmonogram zaktualizowany', // DeepL
  CUSTOM_ENTITIES: '{{N|n|}}iestandardowe{{|j|}} encj{{e|i|}}',
  ENTITIES_HELP_1: 'Zdefiniuj niestandardowe jednostki EMS lub dynamiczne zmienne użytkownika', // DeepL
  ENTITIES_UPDATED: 'Zaktualizowane elementy', // DeepL
  WRITEABLE: 'Możliwość edycji', // DeepL
  SHOWING: 'Wyświetlanie', // DeepL
  SEARCH: 'Wyszukiwanie', // DeepL
  CERT: 'Certyfikat CA (PEM)',
  ENABLE_TLS: 'Włącz TLS', // DeepL
  TLS_CHECK: 'Sprawdź brokera za pomocą', // DeepL
  TLS_CHECK_CA: 'Certyfikat CA',
  TLS_CHECK_FINGERPRINT: 'Odcisk certyfikatu (SHA-1)',
  TLS_CHECK_NONE: 'Brak, niezaznaczone', // DeepL
  TLS_NOT_CHECKED: 'Połączenie jest szyfrowane, ale broker nie jest weryfikowany, więc inne urządzenie w sieci mogłoby podszywać się pod niego', // DeepL
  ON: 'W', // DeepL
  OFF: 'Wyłączone', // DeepL
  POLARITY: 'Polarność', // DeepL
  ACTIVEHIGH: 'Aktywny wysoki', // DeepL
  ACTIVELOW: 'Aktywny niski poziom', // DeepL
  UNCHANGED: 'Bez zmian', // DeepL
  SECURITY_1: 'Dodawanie lub usuwanie użytkowników', // DeepL
  DOWNLOAD_UPLOAD_1: 'Pobieranie i wysyłanie ustawień', // DeepL
  MODULES: 'Moduł',
  MODULES_1: 'Włączanie lub wyłączanie modułów zewnętrznych', // DeepL
  NETWORK_DESC: 'Wi-Fi, Ethernet i nazwa hosta',
  AP_DESC: 'Własny hotspot do konfiguracji i dostępu awaryjnego',
  MQTT_DESC: 'Broker, tematy i wykrywanie',
  APPLICATION_DESC: 'Magistrala EMS, płytka, czujniki i formatowanie',
  NTP_DESC: 'Serwer czasu i strefa czasowa',
  FIRMWARE_DESC: 'Wersja, aktualizacje i odzyskiwanie',
  MODULES_UPDATED: 'Zaktualizowane moduły', // DeepL
  MODULES_NONE: 'Nie wykryto żadnych modułów zewnętrznych', // DeepL
  RENAME: 'Zmień nazwę',
  ENABLE_MODBUS: 'Włącz Modbus', // DeepL
  VIEW_LOG: 'Przejrzyj dziennik, aby zdiagnozować problemy', // DeepL
  SERVICES: 'Usługi',
  ALLVALUES: 'Wszystkie wartości',
  WAIT_FIRMWARE: 'Trwa przesyłanie i instalacja oprogramowania sprzętowego', // DeepL
  INSTALL_VERSION: 'Zostanie zainstalowana wersja {0}',
  UPDATE_AVAILABLE: 'dostępna aktualizacja', // DeepL
  LATEST_VERSION: 'Korzystasz z najnowszej wersji oprogramowania sprzętowego ({0})',
  PLEASE_WAIT: 'Proszę czekać',
  RESTARTING_PRE: 'Uruchamianie',
  RESTARTING_POST: 'Przygotowanie',
  AUTO_SCROLL: 'Automatyczne przewijanie', // DeepL
  DASHBOARD: 'Pulpit',
  DEVELOPER_MODE: 'Tryb programisty',
  DISABLE_RESET: 'Wyłącz przywracanie ustawień fabrycznych', // DeepL
  AUTO_FW_CHECK: 'Automatyczne sprawdzanie dostępności aktualizacji oprogramowania sprzętowego', // DeepL
  BYTES: 'Bajty',
  BITMASK: 'Maska bitowa', // DeepL
  DUPLICATE: 'Duplikat', // DeepL
  DASHBOARD_1: 'Poniżej wyświetlane są wszystkie aktywne elementy systemu EMS oznaczone jako ulubione, a także wszystkie elementy niestandardowe, harmonogramy i czujniki zewnętrzne.', // DeepL
  NO_GPIO: 'Nie znaleziono dostępnych pinów GPIO',
  PLATFORM: 'Platforma',
  RELEASE_TYPE: 'Typ Wydania',
  FIRMWARE_VERSION_INFO: 'Informacje o Wersji Oprogramowania', // DeepL
  USER_PROFILE: 'Profil użytkownika',
  ONLINE_HELP: 'pomoc online',
  UPGRADE_IMPORTANT_MESSAGES: 'Ważne Komunikaty Aktualizacji', // DeepL
  UPGRADE_IMPORTANT_MESSAGES_2: 'Przeprowadzasz aktualizację do nowej głównej wersji. Zaleca się najpierw wykonać kopię zapasową systemu. Sprawdź również pomoc online pod kątem ewentualnych zmian powodujących niekompatybilność.', // DeepL
  UPGRADE_IMPORTANT_MESSAGES_3: 'Wersja 3.10 zawiera istotne zmiany dotyczące bezpieczeństwa. Najpierw należy wykonać kopię zapasową systemu: po aktualizacji hasła są przechowywane w postaci zaszyfrowanej, a starsze wersje nie są w stanie ich odczytać, więc powrót do poprzedniej wersji wymaga tej kopii zapasowej. Każde wywołanie API wymaga teraz klucza API, więc integracje takie jak Home Assistant muszą zostać zaktualizowane. Przed kontynuowaniem należy zapoznać się z informacjami o istotnych zmianach w pomocy online.', // DeepL
  WARNING_SYSTEM_BACKUP: 'Spowoduje to utworzenie kopii zapasowej pełnej konfiguracji i ustawień systemu. Hasła i klucze nie są uwzględniane, chyba że zostaną podane poniżej; przywrócenie kopii zapasowej zachowuje hasła już zapisane w urządzeniu. Komendy są zawsze uwzględniane i mogą zawierać dane uwierzytelniające, dlatego należy zachować ostrożność przy udostępnianiu. Czy chcesz kontynuować?', // DeepL
  BACKUP_INCLUDE_SECRETS: 'Należy uwzględnić hasła i klucze (niezbędne do przywrócenia ustawień na nowym urządzeniu)', // DeepL
  TEST_EMAIL_SUCCESSFUL: 'Wysłano wiadomość e-mail testową', // DeepL
  SYSTEM_NAME: 'Nazwa systemu',
  COMMANDS: 'Komendy',
  COMMANDS_HELP_1: 'Poniżej zdefiniuj nazwane komendy wielokrotnego użytku. Można je wykonywać z konsoli, za pośrednictwem API/MQTT lub odwoływać się do nich w harmonogramie.', // DeepL
  COMMAND_EXECUTED: 'Komenda wykonana', // DeepL
  RUN: 'Uruchom', // DeepL
  INTERNET_CONNECTION_CHECK: 'Sprawdzanie dostępnych wersji online', // DeepL
  UPLOAD_BACKUP_TEXT: 'Kliknij, aby wybrać, lub przeciągnij tutaj plik kopii zapasowej (.json)', // DeepL
  UPLOAD_FIRMWARE_TEXT: 'Kliknij, aby wybrać, lub upuść tutaj plik oprogramowania układowego .bin/.md5', // DeepL
  UPLOAD_MD5_RECEIVED: 'Otrzymano podsumowanie, teraz należy przesłać plik oprogramowania układowego .bin', // DeepL
  UPLOAD_MD5_MATCHED: 'Zgodność MD5, instalacja oprogramowania układowego…', // DeepL
  UPLOAD_NOT_OFFICIAL: 'To nie jest oficjalna wersja EMS-ESP. Instaluj wyłącznie oprogramowanie, któremu ufasz', // DeepL
  UPLOAD_NOT_OFFICIAL_HELP: 'Informacje o wersjach oficjalnych i niestandardowych', // DeepL
  UPLOAD_INSTALL_ANYWAY: 'Zainstaluj mimo wszystko', // DeepL
  OFFICIAL_BUILD: 'Oficjalna Kompilacja', // DeepL
  CUSTOM_BUILD: 'Kompilacja Niestandardowa', // DeepL
  USING_CUSTOM_FIRMWARE: 'Używasz niestandardowego oprogramowania układowego', // DeepL
  PASSWORD_CONFIRM_TITLE: 'Wprowadź hasło ponownie', // DeepL
  PASSWORD_CONFIRM_TEXT: 'Dla własnego bezpieczeństwa wprowadź hasło przed dokonaniem tej zmiany. Przez 5 minut nie będziesz ponownie proszony o jego podanie.', // DeepL
  PASSWORD_CONFIRM_BUTTON: 'Potwierdź',
  PASSWORD_WRONG: 'Nieprawidłowe hasło',
  PASSWORD_NOT_CONFIRMED: 'Operacja została anulowana, hasło nie zostało wprowadzone', // DeepL
  RECOVERY_APP: 'Aplikacja do przywracania danych', // DeepL
  RECOVERY_NOT_INSTALLED: 'Nie zainstalowano',
  RECOVERY_INSTALLED: 'Zainstalowano oprogramowanie naprawcze', // DeepL
  RESTART_INTO_RECOVERY: 'Uruchom ponownie aplikację w trybie odzyskiwania', // DeepL
  RECOVERY_NOT_FOUND: 'Nie znaleziono aplikacji Recovery w Internecie', // DeepL
  RECOVERY_START_FAILED: 'Nie można uruchomić aplikacji Recovery App – może być ona nieaktualna lub uszkodzona. Najpierw zainstaluj najnowszą wersję', // DeepL
  PARTITION_START_FAILED: 'Nie można uruchomić tego oprogramowania sprzętowego, może być uszkodzone', // DeepL
  INSTALLED_VERSIONS: 'Zainstalowane wersje',
  EMSESP_APP: 'Aplikacja EMS-ESP',
  NO_UPDATES_AVAILABLE: 'brak dostępnych aktualizacji',
  SWITCH_TO_DEV: 'Przejdź do gałęzi dev {0}', // DeepL
  SWITCH_TO_STABLE: 'Przejdź do stabilnej wersji {0}', // DeepL
  SAVE: 'Zapisz',
  NEEDS_RESTART: 'wymagane jest ponowne uruchomienie', // DeepL
  CHANGED_SETTINGS: '{num} {{zmienionych ustawień|zmienione ustawienie|zmienione ustawienia|zmienione ustawienia|zmienionych ustawień|zmienionego ustawienia}}',
  SAVED_RESTART: 'Zapisano. Uruchom ponownie EMS-ESP, aby zastosować nowe ustawienia.', // DeepL
  RESTART_NOW: 'Uruchom ponownie teraz',
  COPY_FAILED: 'Nie udało się skopiować – zaznacz tekst i skopiuj go samodzielnie', // DeepL
  DOCUMENTATION: 'Dokumentacja',
  COMMUNITY_CHAT: 'Czat społecznościowy', // DeepL
  REPORT_ISSUE: 'Zgłoś błąd lub poproś o dodanie funkcji', // DeepL
  SUPPORT_PROJECT: 'Wesprzyj projekt',
  GET_HELP: 'Pomoc', // DeepL
  REPORT_PROBLEM: 'Zgłoś problem', // DeepL
  REPORT_STEP_1: 'Pobierz informacje o systemie',
  REPORT_STEP_1_TEXT: 'wersja, płyta główna, stan magistrali EMS, podłączone urządzenia i ustawienia, bez haseł', // DeepL
  REPORT_STEP_2: 'Zapisz ostatni dziennik',
  REPORT_STEP_2_TEXT: 'jeśli problem dotyczy urządzeń, najpierw ustaw poziom logowania na DEBUG', // DeepL
  REPORT_STEP_3: 'Zgłoś problem lub zadaj pytanie na Discordzie', // DeepL
  REPORT_STEP_3_TEXT: 'załącz oba pliki lub wklej poniższe krótkie streszczenie na czacie', // DeepL
  NEW_ISSUE: 'Nowa sprawa', // DeepL
  SHORT_SUMMARY: 'Krótkie podsumowanie',
  LICENSE_TEXT: 'oprogramowanie typu open source na licencji LGPL-3.0', // DeepL
  ADMIN_ACCESS_TEXT: 'pełny dostęp do ustawień, urządzeń i API',
  GUEST_ACCESS_TEXT: 'można wyświetlać wartości i uruchamiać komendy, które mogą uruchamiać goście', // DeepL
  LANGUAGE_BROWSER_ONLY: 'zmiany dotyczą wyłącznie tej przeglądarki', // DeepL
  LANGUAGE_ENTITIES_HINT: 'Język nazw encji urządzeń w protokole MQTT i interfejsie API jest określony w', // DeepL
  ADMINISTRATION: 'Administracja',
  API_KEYS_SHORT: 'klucze dla Home Assistant i skryptów',
  ADDRESSES: 'Adresy',
  SEARCH_SETTINGS: 'Ustawienia wyszukiwania', // DeepL
  NO_SETTINGS_MATCH: 'Nie znaleziono ustawień odpowiadających Twojemu wyszukiwaniu', // DeepL
  GENERAL: 'Ogólne',
  BOARD: 'Płytka drukowana', // DeepL
  ADVANCED: 'Zaawansowane',
  EMS_BUS_DESC: 'W jaki sposób EMS-ESP komunikuje się z Twoim systemem grzewczym.', // DeepL
  BOARD_DESC: 'Piny pochodzą z profilu płytki. Aby ustawić je samodzielnie, włącz tryb programisty i wybierz opcję „Niestandardowe”.', // DeepL
  FORMAT_DESC: 'Jak wartości wyglądają w interfejsie użytkownika oraz w danych przesyłanych przez MQTT/API.', // DeepL
  BOARD_PINS: 'Pinezki z profilu', // DeepL
  ENABLE_EMAIL: 'Włącz powiadomienia e-mailowe', // DeepL
  SEND_TEST_EMAIL: 'Wyślij e-mail testowy', // DeepL
  EMAIL_FROM: 'Źródło:', // DeepL
  EMAIL_TO: 'Do',
  EMAIL_SUBJECT: 'Temat',
  WIFI_SCAN: 'Skanowanie sieci Wi-Fi', // DeepL
  SCAN: 'Skanuj',
  NETWORKS_NEARBY: 'Sieci w pobliżu',
  SIGNAL: 'Sygnał',
  CHANNEL: 'Kanał',
  WIFI_OPEN: 'otwórz', // DeepL
  WIFI_SAVED: 'zapisano', // DeepL
  NETWORK_ETHERNET_INFO: 'Obecnie podłączono przez Ethernet ({ip}). Wi-Fi jest używane wyłącznie w przypadku braku kabla.', // DeepL
  HOSTNAME_ACCESS: 'Nazwa hosta i dostęp', // DeepL
  IP_ADDRESS: 'Adres IP',
  NETWORK_ETH_10MBIT: 'Wymuszony tryb półdupleksowy 10 Mbit', // DeepL
  NETWORK_ETH_10MBIT_HELP: 'tylko w przypadku wadliwych przełączników lub długich kabli', // DeepL
  NETWORK_DISABLE_SLEEP_HELP: 'bardziej niezawodny, zużywa nieco więcej energii', // DeepL
  NETWORK_MDNS_HELP: 'dostęp do niej pod adresem {host}.local za pomocą mDNS', // DeepL
  NETWORK_FIXED_IP_HELP: 'w przeciwnym razie wykorzystywany jest protokół DHCP', // DeepL
  NETWORK_CORS_HELP: 'wymagane tylko wtedy, gdy inna strona internetowa wywołuje API', // DeepL
  NTP_TIME_ON_DEVICE: 'czas spędzony w EMS-ESP', // DeepL
  NTP_TIME_SERVER: 'Serwer czasu',
  NTP_THERMOSTAT_CLOCK: 'Zegar termostatu',
  NTP_THERMOSTAT_SYNC: 'Ustaw zegar termostatu', // DeepL
  NTP_THERMOSTAT_SYNC_HELP: 'ustawia datę i godzinę na termostacie EMS',
  NTP_THERMOSTAT_SYNC_1: 'Włącz, strefa czasowa EMS-ESP', // DeepL
  NTP_THERMOSTAT_SYNC_2: 'Włączone, własna strefa czasowa', // DeepL
  NTP_THERMOSTAT_TZ: 'Strefa czasowa termostatu',
  MQTT_PREVIEW: 'Co jest publikowane',
  MQTT_PREVIEW_CMD: 'akceptowane jako polecenie', // DeepL
  MQTT_PREVIEW_RESPONSE: 'wyniki komend', // DeepL
  MQTT_BEHAVIOUR: 'Zachowanie',
  MQTT_TOPICS: 'Tematy i dane użytkowe', // DeepL
  MQTT_DISCOVERY: 'Odkrycie', // DeepL
  MQTT_INTERVALS_HELP: 'Liczba sekund między publikacjami, 0 = publikuj przy każdej zmianie.', // DeepL
  MQTT_SINGLE_NO_DISCOVERY: 'niedostępne przy włączonej funkcji wykrywania', // DeepL
  MQTT_DISCOVERY_NO_SINGLE: 'niedostępne w przypadku tematów o pojedynczej wartości w', // DeepL
  MQTT_DISCOVERY_HELP: 'automatycznie tworzy obiekty w Home Assistant lub Domoticz', // DeepL
  MQTT_ENTITY_FORMAT_WARN: 'Zmiana tego spowoduje zmianę nazw wszystkich elementów w Home Assistant', // DeepL
  USERS: 'Użytkownicy',
  USERS_TEXT: 'Administratorzy mogą zmieniać wszystko. Goście mogą przeglądać zawartość oraz uruchamiać komendy przeznaczone dla gości.', // DeepL
  ADD_USER: 'Dodaj użytkownika',
  EDIT_USER: 'Edytuj użytkownika',
  ROLE: 'Rola',
  ROLE_ADMIN: 'admin', // DeepL
  ROLE_GUEST: 'gość',
  ADMIN: 'Admin', // DeepL
  YOU: 'ty',
  NEW_PASSWORD: 'Nowe hasło',
  UNSAVED_CHANGES: '{num} {{niezapisanych zmian|niezapisana zmiana|niezapisane zmiany|niezapisane zmiany|niezapisanych zmian|niezapisanej zmiany}}',
  CREATE_KEY: 'Utwórz Klucz',
  NEW_API_KEY: 'Nowy Klucz API',
  NO_API_KEYS: 'Brak kluczy API',
  API_KEYS_LIMIT: 'Możesz mieć maksymalnie {num} klawiszy.', // DeepL
  REVOKE: 'Cofnij', // DeepL
  REVOKE_KEY: 'Cofnij Klucz API', // DeepL
  REVOKE_KEY_TEXT: 'Wszelkie fragmenty zawierające „{name}” natychmiast przestają działać.', // DeepL
  MODULES_TEXT: 'Opcjonalne funkcje wbudowane w to oprogramowanie sprzętowe. Tutaj można je włączyć lub wyłączyć.', // DeepL
  BY_AUTHOR: 'autor: {name}',
  LICENSE_KEY: 'Klucz licencyjny',
  MODULE_ACTIVATED: 'aktywowane', // DeepL
  MODULE_PENDING: 'oczekuje na aktywację',
  ENABLED: 'Włączone', // DeepL
  UNDO: 'Cofnij',
  FW_UPDATES: 'Aktualizacje',
  FW_VERSION_AVAILABLE: 'Dostępna jest wersja {version}',
  FW_RELEASE_NOTES: 'Informacje o wydaniu',
  FW_UPDATES_OFFLINE: 'Nie udało się sprawdzić dostępności nowych wersji online. EMS-ESP potrzebuje do tego połączenia z Internetem.', // DeepL
  FW_RESTART_RESET: 'Ponowne Uruchomienie i Reset', // DeepL
  FW_RECOVERY_TEXT: 'Czy chcesz uruchomić ponownie w trybie aplikacji odzyskiwania? Aplikacja łączy się z Twoją siecią i uruchamia własny punkt dostępowy, za pomocą którego można wgrać oprogramowanie sprzętowe.', // DeepL
  FW_SIZE: 'Rozmiar',
  FW_INSTALL_DATE: 'Zainstalowano na', // DeepL
  FW_UPLOAD_FAILED: 'Przesyłanie nie powiodło się: nieprawidłowy typ pliku lub oprogramowanie układowe przeznaczone dla innej płytki', // DeepL
  FILE_TYPE_INVALID: 'Ten typ pliku nie jest obsługiwany, wybierz plik {types}',
  WHO_CAN_RUN: 'Kto może uruchomić',
  CMD_NONE: 'Nie znaleziono żadnych komend', // DeepL
  CMD_NONE_MATCH: 'Nie znaleziono komend pasujących do wyszukiwania', // DeepL
  SCHEDULE_NONE: 'Nie znaleziono harmonogramów',
  SCHEDULE_NONE_MATCH: 'Nie znaleziono harmonogramów pasujących do Twojego wyszukiwania', // DeepL
  CE_NONE: 'Nie znaleziono żadnych elementów', // DeepL
  CE_ALL: 'Wszystkie',
  CE_STORAGE: 'Przechowywanie',
  CE_SOURCE: 'Źródło',
  CE_EMS_SOURCE: 'urządzenie {device} · telegram {type} · przesunięcie {offset}',
  CE_RAM_TEXT: 'zachowane do momentu ponownego uruchomienia', // DeepL
  CE_NVS_TEXT: 'zachowywane po ponownym uruchomieniu', // DeepL
  CE_EXCLUDED: 'nie dotyczy API/MQTT', // DeepL
  SEARCH_SENSORS: 'Wyszukaj nazwę, identyfikator lub GPIO', // DeepL
  NO_SENSORS: 'Nie znaleziono czujników',
  NO_SENSORS_MATCH: 'Żadne czujniki nie pasują do Twojego wyszukiwania', // DeepL
  SENSORS_HELP_1: 'Zarządzaj czujnikami temperatury i analogowymi podłączonymi do tego urządzenia', // DeepL
  TEMP_SENSORS_HELP: 'Wykryto automatycznie. Kliknij czujnik, aby zmienić jego nazwę lub ustawić przesunięcie', // DeepL
  FREE_GPIOS: 'Wolne piny GPIO', // DeepL
  SYSTEM_SENSOR: 'Czujnik systemowy',
  MASK_FAVORITE: 'Ulubione',
  MASK_READONLY: 'Pisanie zablokowane', // DeepL
  MASK_NO_API: 'Brak MQTT/API', // DeepL
  MASK_HIDDEN: 'Ukryte',
  MASK_REMOVED: 'Usunięto', // DeepL
  FILTER_ALL: 'Wszystkie',
  NUM_SELECTED: 'Wybrano: {num}', // DeepL
  ALL_SHOWN: 'Wyświetl wszystkie: {num}', // DeepL
  BULK_HINT: 'Przyciski powodują zmianę zaznaczonych wierszy lub wyświetlanie wszystkich wierszy, gdy żaden nie jest zaznaczony.', // DeepL
  CLEAR_SELECTION: 'Przejrzysty wybór', // DeepL
  NO_ENTITIES_MATCH: 'Nie znaleziono żadnych wyników', // DeepL
  LIMIT_HELP: 'pozostaw puste, aby zachować domyślne ograniczenie', // DeepL
  MIN_ABOVE_MAX: 'Wartość minimalna nie może przekraczać wartości maksymalnej', // DeepL
  RENAME_DEVICE: 'Zmień Nazwę Urządzenia',
  WEEKLY: 'Co tydzień', // DeepL
  EVERY_DAY: 'Codziennie',
  SCHEDULER_NO_COMMANDS: 'Najpierw dodaj polecenie na stronie Komendy', // DeepL
  DU_SETTINGS: 'Ustawienia EMS-ESP',
  DU_SETTINGS_TEXT: 'Kopia zapasowa ustawień EMS-ESP: sieć, MQTT, Personalizacja, Harmonogram, niestandardowe encje i moduły. Przesłanie pliku ustawień lub kopii zapasowej powoduje ponowne uruchomienie EMS-ESP.', // DeepL
  HB_TITLE: 'Ustawienia ogrzewania',
  HB_TEXT: 'Zapisuje aktualne wartości kotła, termostatu i innych urządzeń EMS, dzięki czemu można je później przywrócić, na przykład po wymianie urządzenia. Elementy i komendy tylko do odczytu nigdy nie są modyfikowane podczas przywracania.', // DeepL
  HB_BACKUP: 'Kopia Zapasowa Ustawień Ogrzewania', // DeepL
  HB_RESTORE: 'Przywróć ustawienia ogrzewania',
  DATA_EXPORT: 'Eksport danych',
  DATA_EXPORT_TEXT: 'Do celów wsparcia technicznego i analizy. Pliki te nie mogą zostać ponownie przesłane.', // DeepL
  NUM_VALUES: '{num} {{wartości|wartość|wartości|wartości|wartości|wartości}}',
  HB_NOTHING: 'nie ma co uzupełniać', // DeepL
  FILE_NAME: 'Nazwa pliku',
  HB_NOTE: 'Uwaga (opcjonalna, wyświetlana ponownie po przywróceniu)', // DeepL
  HB_INCOMPLETE: 'Wartości, które nie zostały jeszcze odczytane z magistrali, są pomijane. Po ponownym uruchomieniu należy odczekać kilka minut, aby uzyskać pełną kopię zapasową.', // DeepL
  HB_RESTORE_TEXT: 'Wybierz kopię zapasową ustawień ogrzewania. Żadne zmiany nie zostaną zapisane, dopóki ich nie sprawdzisz i nie potwierdzisz.', // DeepL
  HB_DROP: 'Kliknij, aby wybrać, lub przeciągnij tutaj plik rezerwowy systemu grzewczego (.json)', // DeepL
  HB_INVALID: 'Nie jest to kopia zapasowa ustawień ogrzewania', // DeepL
  HB_OTHER_FILE: 'Wybierz inny plik',
  HB_SAME_DEVICE: 'to samo urządzenie',
  HB_NOT_CONNECTED: 'brak połączenia', // DeepL
  HB_OTHER_DEVICE: 'inne urządzenie (identyfikator produktu {backup} w trybie rezerwowym, {now} podłączone), pominięto', // DeepL
  HB_UNIT_MISMATCH: 'Ta kopia zapasowa zawiera temperatury w {unit}. Przed przywróceniem kopii zapasowej zmień jednostkę temperatury w ustawieniach aplikacji.', // DeepL
  HB_CHANGED: 'Zmieniono', // DeepL
  ALL: 'Wszystkie',
  HB_NOW: 'Teraz',
  HB_BACKUP_VALUE: 'Kopia zapasowa', // DeepL
  HB_NOT_AVAILABLE: 'niedostępne',
  HB_SENDING: 'wysyłanie',
  HB_WRITTEN: 'napisane', // DeepL
  HB_RESTORE_NUM: 'Przywróć {num} {{wartości|wartość|wartości|wartości|wartości|wartości}}',
  RESTORE: 'Przywróć',
  HB_CONFIRM: 'Zapisać {num} {{wartości|wartość|wartości|wartości|wartości|wartości}} z {file} do urządzeń EMS?',
  HB_CONFIRM_TEXT: 'Wartości są wysyłane pojedynczo. Jeśli urządzenie odrzuci jedną z nich, pozostałe nadal zostaną wysłane.', // DeepL
  HB_DONE: '{num} {{wartości zapisano|wartość zapisana|wartości zapisane|wartości zapisane|wartości zapisano|wartości zapisano}}',
  HB_DONE_FAILED: 'Wystąpił błąd {num}. Urządzenia często odrzucają wartości ustawione na poziomie instalatora; szczegóły znajdują się w dzienniku systemowym.', // DeepL
  HB_NO_CHANGES: 'Nie różni się nic od wersji zapasowej', // DeepL
  HB_DIFFERENT_DEVICE: 'inne urządzenia', // DeepL
  SELECT_ALL: 'Zaznacz wszystko',
  DASHBOARD_EMPTY: 'Na razie nie ma nic do wyświetlenia',
  NO_DEVICES: 'Nie znaleziono jeszcze żadnych urządzeń EMS', // DeepL
  ADD_COMMAND: 'Dodaj komendę',
  EDIT_COMMAND: 'Komenda edycji', // DeepL
  RUN_COMMAND: 'Uruchom polecenie', // DeepL
  ADD_SCHEDULE: 'Dodaj harmonogram',
  EDIT_SCHEDULE: 'Edytuj harmonogram',
  ADD_ENTITY: 'Dodaj element', // DeepL
  EDIT_ENTITY: 'Edytuj Element', // DeepL
  ADD_ANALOG_SENSOR: 'Dodaj czujnik analogowy',
  EDIT_ANALOG_SENSOR: 'Edytuj czujnik analogowy',
  REMOVE_ANALOG_SENSOR: 'Usuń Czujnik Analogowy',
  REMOVE_SENSOR_TEXT: 'Czujnik „{name}” jest natychmiast usuwany.', // DeepL
  EDIT_TEMP_SENSOR: 'Edytuj Czujnik Temperatury',
  INSTALL_FIRMWARE: 'Zainstaluj Oprogramowanie ({channel})', // DeepL
  PARTITION: 'Podział', // DeepL
  PULSE: 'Pulse', // DeepL
  SYS_HOME: 'Dom', // draft, please review
  SYS_HOME_HELP: 'Najważniejsze rzeczy na pierwszy rzut oka. Wszystko inne znajdziesz w menu systemowym.', // draft, please review
  SYS_SENSORS_HELP: 'Czujniki podłączone do bramy. Aby je zmienić, otwórz Czujniki w menu głównym.', // draft, please review
  SYS_MENU: 'Menu systemowe', // draft, please review
  SYS_MENU_HELP: 'Wszystkie wartości {name} w Twojej instalacji.', // draft, please review
  SYS_SEARCH_HELP: 'Pasujące wartości ze wszystkich urządzeń.', // draft, please review
  SYS_FAV_HELP: 'Twoje najważniejsze encje urządzeń. Gwiazdką przy wartości zmienisz tę listę.', // draft, please review
  SYS_FAV_NONE: 'Brak ulubionych. Oznacz wartości gwiazdką.', // draft, please review
  SYS_FAV_REMOVED: 'Usunięto z ulubionych', // draft, please review
  SYS_GROUP_HEATING: 'Ogrzewanie', // draft, please review
  SYS_GROUP_PLANT: 'Instalacja', // draft, please review
  SYS_GROUP_OVERVIEW: 'Przegląd', // draft, please review
  SYS_LEVEL_MOVE_ALL: 'Pokaż tylko we wszystkich wartościach', // draft, please review
  SYS_LEVEL_MOVE_NORMAL: 'Pokaż w widoku normalnym', // draft, please review
  SYS_LEVEL_NOW_ALL: 'Teraz tylko we wszystkich wartościach', // draft, please review
  SYS_LEVEL_NOW_NORMAL: 'Teraz także w widoku normalnym', // draft, please review
  SYS_HC: 'Obieg grzewczy {0}', // draft, please review
  SYS_DHW: 'Ciepła woda', // draft, please review
  SYS_AHS: 'Ogrzewanie dodatkowe', // draft, please review
  SYS_HS: 'Źródło ciepła', // draft, please review
  SYS_SRC: 'Źródło', // draft, please review
  SYS_OTHER_DEVICES: 'Pozostałe urządzenia', // draft, please review
  SYS_SERVICE: 'Konserwacja i błędy', // draft, please review
  SYS_ENERGY: 'Energia i czasy pracy', // draft, please review
  SYS_SEC_MODE: 'Praca', // draft, please review
  SYS_SEC_TEMPS: 'Temperatury', // draft, please review
  SYS_SEC_STATUS: 'Stan', // draft, please review
  SYS_SEC_ADVANCED: 'Krzywa grzewcza i instalacja', // draft, please review
  SYS_SEC_MEASURE: 'Wartości pomiarowe', // draft, please review
  SYS_SEC_OTHER: 'Pozostałe wartości', // draft, please review
  SYS_LEVEL_NORMAL: 'Normalny', // draft, please review
  SYS_NO_RESULTS: 'Nic nie znaleziono. Spróbuj wszystkich wartości lub innego terminu.', // draft, please review
  SYS_SEARCH_PLACES: 'Strony i moduły', // draft, please review
  SYS_NOTHING_HERE: 'Na poziomie Normalny nie ma tu nic do pokazania.', // draft, please review
  SYS_HIDDEN: 'Ukryte wartości eksperckie: {num}', // draft, please review
  SYS_SEARCH_RESULTS: 'Wyniki wyszukiwania', // draft, please review
  SYS_CHANGED_WAS: 'zmieniono (było: {0})', // draft, please review
  SYS_NO_DEVICES: 'Nie znaleziono jeszcze urządzeń.', // draft, please review
  SYS_ROOM_TEMP: 'Temperatura pokojowa', // draft, please review
  SYS_WISH: 'Zadana', // draft, please review
  SYS_CURRENT: 'Temperatura bieżąca', // draft, please review
  SYS_OUTDOOR: 'Temperatura zewnętrzna', // draft, please review
  SYS_FLOW: 'Zasilanie', // draft, please review
  SYS_RETURN: 'Powrót', // draft, please review
  SYS_PRESSURE: 'Ciśnienie w instalacji', // draft, please review
  SYS_POWER: 'Moc', // draft, please review
  SYS_BURNER_ON: 'Palnik pracuje', // draft, please review
  SYS_BURNER_OFF: 'Palnik wyłączony', // draft, please review
  SYS_DHW_HEATING: 'Trwa podgrzewanie', // draft, please review
  SYS_READY: 'Gotowe', // draft, please review
  SYS_ONE_TIME: 'Jednorazowe ładowanie', // draft, please review
  SYS_NOTES: 'Uwagi', // draft, please review
  SYS_NOTES_SUB: 'Błędy i konserwacja', // draft, please review
  SYS_LAST_FAULT: 'Ostatni błąd', // draft, please review
  SYS_MAINT_MSG: 'Komunikat konserwacyjny', // draft, please review
  SYS_NEXT_MAINT: 'Następna konserwacja', // draft, please review
  SYS_FAULT_LAST: 'Ostatnia usterka / komunikat', // draft, please review
  SYS_FAULT_CTRL: 'Ostatni błąd regulatora', // draft, please review
  SYS_FAULT_SERVICE: 'Kod serwisowy', // draft, please review
  SYS_FAULT_ADD: 'Kod dodatkowy {0}', // draft, please review
  SYS_FAULT_RAW_ONLY: 'Brak objaśnienia dla tego urządzenia.', // draft, please review
  SYS_FAULT_NONE: 'Brak zapisanych usterek.', // draft, please review
  SYS_FAULT_UNTIL_NOW: 'do teraz', // draft, please review
  SYS_MAINTENANCE: 'Konserwacja', // draft, please review
};

export default pl;
