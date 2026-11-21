import type { Translation } from '../i18n-types';

const de: Translation = {
  LANGUAGE: 'Sprache',
  RETRY: 'Erneut versuchen', // DeepL
  IS_REQUIRED: '{0} ist erforderlich',
  SIGN_IN: 'Anmelden', // DeepL
  SIGN_IN_SUBTITLE: 'Gateway für EMS-Heizungsanlagen',
  SIGN_OUT: 'Abmelden', // DeepL
  USERNAME: 'Benutzername', // DeepL
  PASSWORD: 'Passwort',
  APPLICATION_SETTINGS: 'Anwendungseinstellungen',
  AP_SETTINGS: 'Access-Point-Einstellungen',
  NETWORK_SETTINGS: 'Netzwerkeinstellungen',
  NTP_SETTINGS: 'NTP-Einstellungen',
  MQTT_SETTINGS: 'MQTT-Einstellungen',
  EMS_BUS_SETTINGS: 'EMS-Bus-Einstellungen',
  HELP: 'Hilfe',
  LOGGED_IN: 'Angemeldet als {name}', // DeepL
  PLEASE_SIGNIN: 'Bitte melden Sie sich an, um fortzufahren', // DeepL
  UPLOAD_SUCCESSFUL: 'Upload erfolgreich', // DeepL
  DOWNLOAD_SUCCESSFUL: 'Erfolgreicher Download', // DeepL
  INVALID_LOGIN: 'Falscher Benutzername oder falsches Passwort', // DeepL
  NETWORK: 'Netzwerk',
  SECURITY: 'Sicherheit',
  ONOFF_CAP: 'EIN/AUS', // DeepL
  ONOFF: 'Ein/Aus', // DeepL
  TYPE: 'Typ',
  ENTITIES: 'Entitäten',
  EXPORT: 'Exportieren',
  FAVORITES: "Favoriten",
  CLIENT_ID: 'Client-ID',
  DEVICE_ID: 'Geräte-ID',
  PRODUCT_ID: 'Produkt-ID',
  ENTITY_ID: 'Entitäts-ID',
  SENSOR_ID: 'Sensor-ID',
  TYPE_ID: 'Typ-ID',
  DEVICE: 'Geräte',
  BRAND: 'Marke',
  VERSION: 'Version',
  ENTITY_NAME: 'Entitätsname',
  VALUE: 'Wert',
  DEVICES: 'Geräte',
  SENSORS: 'Sensoren',
  AUTOMATION: 'Automatisierung',
  CONNECTIVITY: 'Konnektivität',
  MAINTENANCE: 'Wartung',
  CHANGE_VALUE: 'Wert ändern',
  CANCEL: 'Abbrechen',
  REMOVE_ALL: 'Alle entfernen', // DeepL
  APPLY_CHANGES: 'Änderungen übernehmen ({0})', // DeepL
  UPDATE: 'Aktualisierung', // DeepL
  REMOVE: 'Entfernen',
  RESET: 'Zurücksetzen',
  PROBLEM_UPDATING: 'Problem bei der Aktualisierung', // DeepL
  PROBLEM_LOADING: 'Problem beim Laden',
  ANALOG_SENSORS: 'Analogsensoren',
  SETTINGS: 'Einstellungen',
  SETTINGS_UPDATED: 'Einstellungen aktualisiert',
  NAME_UPDATED: 'Name aktualisiert',
  SENSOR_UPDATED: 'Sensor aktualisiert',
  ANALOG_SENSOR_UPDATED: 'Analogsensor aktualisiert',
  OFFSET: 'Versatz', // DeepL
  FACTOR: 'Faktor',
  FREQ: 'Häufigkeit', // DeepL
  DUTY_CYCLE: 'Einschaltdauer', // DeepL
  UNIT: 'Maßeinheit',
  STARTVALUE: 'Startwert',
  EDIT: 'Bearbeiten', // DeepL
  TEMP_SENSORS: 'Temperatursensoren',
  WRITE_CMD_SENT: 'Befehl „Write“ gesendet', // DeepL
  EMS_BUS_WARNING: 'EMS-Bus getrennt. Sollte diese Warnung nach einigen Sekunden weiterhin bestehen, überprüfen Sie bitte die Einstellungen und das Board-Profil.', // DeepL
  CONNECTED: 'Verbunden',
  TX_ISSUES: 'Probleme bei der Übertragung – Übertragungsmodus überprüfen', // DeepL
  DISCONNECTED: 'Keine Verbindung', // DeepL
  DATA_TRAFFIC: 'Datenverkehr',
  EMS_DEVICE: 'EMS-Gerät', // DeepL
  SUCCESS: 'ERFOLG',
  FAIL: 'FEHLER',
  QUALITY: 'QUALITÄT',
  STATUS_NAMES: [
    'EMS-Telegramme empfangen (Rx)',
    'EMS-Telegramme gelesen (Tx)',
    'EMS-Telegramme geschrieben (Tx)',
    'Temperatursensoren gelesen',
    'Analogsensoren gelesen',
    'MQTT-Nachrichten gesendet',
    'API-Aufrufe',
    'Syslog-Mitteilungen'
  ],
  NUM_DAYS: '{num} Tag{{e}}',
  NUM_SECONDS: '{num} Sekunde{{n}}',
  NUM_HOURS: '{num} Stunde{{n}}',
  NUM_MINUTES: '{num} Minute{{n}}',
  APPLICATION: 'Anwendung',
  CUSTOMIZATIONS: 'Anpassungen',
  APPLICATION_RESTARTING: 'EMS-ESP wird neu gestartet', // DeepL
  BOARD_PROFILE: 'Board-Profil',
  CUSTOM: 'Benutzerdefiniert',
  GPIO_OF: '{0} GPIO',
  BUTTON: 'Taster',
  TEMPERATURE: 'Temperatur',
  PHY_TYPE: 'Eth-PHY-Typ', // DeepL
  DISABLED: 'deaktiviert',
  TX_MODE: 'EMS-Tx-Modus', // DeepL
  EMS_BUS: 'EMS Bus',
  EMS_BUS_ID: 'ID EMS Bus',
  HARDWARE: 'Hardware',
  LANGUAGE_ENTITIES: 'Sprache (für Geräte-Entitäten)', // DeepL
  HIDE_LED: 'LED ausblenden',
  ENABLE_TELNET: 'Telnet-Konsole aktivieren', // DeepL
  ENABLE_ANALOG: 'Analogsensoren aktivieren', // DeepL
  CONVERT_FAHRENHEIT: 'Rechnen Sie Temperaturwerte in Fahrenheit um', // DeepL
  READONLY: 'Schalten Sie den schreibgeschützten Modus ein (blockiert alle ausgehenden EMS-Tx-Schreibbefehle)', // DeepL
  WRITE_PROTECTION: 'Schützen Sie die EMS-Geräte vor wiederholten und häufigen Schreibvorgängen', // DeepL
  WRITE_LIMIT: 'Max. Anzahl von Schreibvorgängen pro Minute (0 = keine Begrenzung)', // DeepL
  WRITE_LIMIT_REFUSE: 'Schreiben über die zulässige Länge hinaus ist nicht zulässig (andernfalls wird lediglich eine Warnung protokolliert)', // DeepL
  UNDERCLOCK_CPU: 'CPU-Taktfrequenz herabsetzen', // DeepL
  REMOTE_TIMEOUT: 'Zeitlimit für Fernzugriff', // DeepL
  REMOTE_TIMEOUT_EN: 'Fernsteuerung bei fehlender Raumtemperatur deaktivieren', // DeepL
  HEATINGOFF: 'Heizkessel starten, wobei die Zwangsheizung ausgeschaltet ist', // DeepL
  MIN_DURATION: 'Wartezeit', // DeepL
  ENABLE_SHOWER_TIMER: 'Dusch-Timer aktivieren', // DeepL
  ENABLE_SHOWER_ALERT: 'Duschalarm aktivieren',
  TRIGGER_TIME: 'Auslösezeit',
  COLD_SHOT_DURATION: 'Dauer der Kaltzufuhr', // DeepL
  FORMATTING_OPTIONS: 'Formatierungsoptionen',
  BOOLEAN_FORMAT_DASHBOARD: 'Web im Booleschen Format', // DeepL
  BOOLEAN_FORMAT_API: 'API/MQTT im booleschen Format', // DeepL
  ENUM_FORMAT: 'Enum-Format API/MQTT', // DeepL
  INDEX: 'Inhaltsverzeichnis', // DeepL
  ENABLE_PARASITE: '1-Wire-Parasitstrom aktivieren', // DeepL
  LOG_HEX: 'Protokollieren Sie EMS-Telegramme im Hexadezimalformat', // DeepL
  ENABLE_SYSLOG: 'Syslog aktivieren',
  LOG_LEVEL: 'Protokollierungsstufe', // DeepL
  MARK_INTERVAL: 'Intervall markieren', // DeepL
  SECONDS: 'Sekunden',
  MINUTES: 'Minuten',
  HOURS: 'Stunden',
  RESTART: 'Neustart', // DeepL
  RESTART_TEXT: 'EMS-ESP muss neu gestartet werden, damit geänderte Systemeinstellungen übernommen werden.', // DeepL
  RESTART_CONFIRM: 'Sind Sie sicher, dass Sie EMS-ESP neu starten möchten?', // DeepL
  COMMAND: 'Befehl',
  CUSTOMIZATIONS_RESTART: 'Alle individuellen Anpassungen wurden entfernt. Neustart…', // DeepL
  CUSTOMIZATIONS_FULL: 'Die Anzahl der ausgewählten Entitäten hat das Limit überschritten. Bitte speichern Sie in mehreren Schritten.', // DeepL
  CUSTOMIZATIONS_SAVED: 'Anpassungen aktualisiert', // DeepL
  CUSTOMIZATIONS_HELP_1: 'Wählen Sie ein Gerät aus und passen Sie die Optionen der Entitäten an', // DeepL
  CUSTOMIZATIONS_HELP_2: 'Als Favorit markieren',
  CUSTOMIZATIONS_HELP_3: 'Schreibaktion deaktivieren',
  CUSTOMIZATIONS_HELP_4: 'Aus MQTT und API ausschließen', // DeepL
  CUSTOMIZATIONS_HELP_5: 'Von „Geräten“ ausblenden', // DeepL
  CUSTOMIZATIONS_HELP_6: 'Aus dem Speicher entfernen',
  SELECT_DEVICE: 'Wählen Sie ein Gerät aus',
  MODIFIED: 'Geändert',
  RESET_SELECTED: 'Ausgewählte zurücksetzen', // DeepL
  OPTIONS: 'Optionen',
  NAME: 'Name',
  CUSTOMIZATIONS_RESET: 'Sind Sie sicher, dass Sie alle individuellen Anpassungen entfernen möchten?', // DeepL
  SYSTEM_INFORMATION: 'Systeminformationen', // DeepL
  HELP_INFORMATION_1: 'Besuchen Sie das Online-Wiki, um Anleitungen zur Konfiguration von EMS-ESP zu erhalten.', // DeepL
  HELP_INFORMATION_2: 'Für einen Live-Chat mit der Community treten Sie bitte unserem Discord-Server bei', // DeepL
  HELP_INFORMATION_3: 'Um eine Funktion anzufragen oder einen Fehler zu melden', // DeepL
  HELP_INFORMATION_4: 'Geben Sie bei der Meldung eines Problems Ihre Systeminformationen an, um eine schnellere Bearbeitung zu gewährleisten', // DeepL
  UPLOAD: 'Hochladen',
  DOWNLOAD: 'Herunterladen',
  INSTALL: 'Installieren',
  REINSTALL: 'Erneut installieren',
  REINSTALL_CONFIRM: 'Möchten Sie Version {0} wirklich erneut installieren?',
  FAILED: 'fehlgeschlagen', // DeepL
  SYSTEM: 'System',
  SYSTEM_LOG: 'Systemprotokoll',
  STATUS: 'Status',
  DOWNLOAD_UPLOAD: 'Herunterladen/Hochladen',
  CLOSE: 'Schließen',
  COMMAND_GUEST: 'Gäste können diesen Befehl ausführen', // DeepL
  TOKENS_RESET: 'Bei diesem Update werden Zugriffstoken mit einem neuen Geräteschlüssel signiert, sodass bestehende API-Zugriffstoken nicht mehr funktionieren. Erstellen Sie für jede Integration, die die API nutzt – beispielsweise Home Assistant –, einen API-Schlüssel und verwenden Sie diesen anstelle des alten Tokens.', // DeepL
  API_KEYS: 'API-Schlüssel',
  API_KEYS_TEXT: 'API-Schlüssel ermöglichen es Integrationen wie Home Assistant und Skripten, die API zu nutzen. Senden Sie den Schlüssel im Header „Authorization: Bearer <SCHLÜSSEL>“. Lese-Schlüssel können Werte auslesen und Befehle ausführen, die auch Gäste ausführen dürfen; Lese- und Schreib-Schlüssel können alles tun, was ein Administrator über die API tun kann.', // DeepL
  API_KEY_ACCESS: 'Zugriff',
  API_KEY_READ: 'Lesen',
  API_KEY_READ_WRITE: 'Lesen und Schreiben',
  API_KEY_SHOW_ONCE: 'Kopieren Sie diesen Schlüssel jetzt und bewahren Sie ihn sicher auf. Er wird nicht erneut angezeigt.', // DeepL
  CREATED: 'Erstellt',
  COPY: 'Text', // DeepL
  COPIED: 'Kopiert',
  USE: 'Verwenden Sie',
  FACTORY_RESET: 'Werksreset', // DeepL
  SYSTEM_FACTORY_TEXT_DIALOG: 'Sind Sie sicher, dass Sie EMS-ESP auf die Werkseinstellungen zurücksetzen möchten?', // DeepL
  STABLE: 'Stabil',
  DEVELOPMENT: 'Entwicklung',
  UPTIME: 'Systemverfügbarkeit', // DeepL
  LAST_RESTART: 'Letzter Neustart',
  POWER_AND_RESET: 'Stromversorgung und Zurücksetzen', // DeepL
  FREE_MEMORY: 'Freier Speicher', // DeepL
  BUFFER_SIZE: 'Maximale Puffergröße', // DeepL
  COMPACT: 'Kompakt', // DeepL
  SUMMARY: 'Zusammenfassung', // DeepL
  SHOW_MORE: '{n} weitere anzeigen',
  SHOW_LESS: 'Weniger anzeigen',
  COREDUMP: 'Absturzbericht', // DeepL
  DOWNLOAD_SYSTEM_BACKUP: 'Systemsicherung',
  ERROR: 'Unerwarteter Fehler, bitte versuchen Sie es erneut', // DeepL
  TIME_SET: 'Eingestellte Zeit', // DeepL
  MANAGE_USERS: 'Benutzer verwalten', // DeepL
  USER_WARNING: 'Sie müssen mindestens einen Administratorbenutzer eingerichtet haben', // DeepL
  ADD: 'Hinzufügen',
  TELNET_ENABLED: 'Die Telnet-Konsole ist aktiviert. Da Telnet nicht verschlüsselt ist, deaktivieren Sie es bitte in den Anwendungseinstellungen, falls Sie es nicht nutzen.', // DeepL
  TOO_MANY_LOGINS: 'Zu viele fehlgeschlagene Anmeldeversuche – versuchen Sie es in einigen Minuten erneut', // DeepL
  PASSWORD_KEEP: 'Lassen Sie das Feld leer, um das aktuelle Passwort beizubehalten', // DeepL
  SETUP_TITLE: 'Legen Sie Ihre Passwörter fest', // DeepL
  SETUP_TEXT: 'Dieses Gerät verwendet noch die werkseitigen Passwörter. Legen Sie Ihre eigenen fest, bevor Sie fortfahren.', // DeepL
  SETUP_ADMIN_PASSWORD: 'Neues Passwort für {name}',
  SETUP_AP_PASSWORD: 'Neues Passwort für den Zugangspunkt', // DeepL
  SETUP_AP_REQUIRED: 'Es ist noch kein WLAN-Netzwerk eingerichtet, daher erfolgt der Zugriff über den Access Point. 8–63 Zeichen; diese werden beim nächsten Start des Access Points verwendet.', // DeepL
  SETUP_AP_OPTIONAL: 'Empfohlen: 8–63 Zeichen; wird beim nächsten Start des Zugangspunkts übernommen. Lassen Sie das Feld leer, um die aktuelle Bezeichnung beizubehalten.', // DeepL
  SETUP_AP_DEFAULT: 'Der Access Point verwendet noch das werkseitige Passwort. Legen Sie in den Einstellungen des Access Points ein eigenes Passwort fest.', // DeepL
  SETUP_VIEW_USER: 'Optional: Fügen Sie einen Gastbenutzer mit Lesezugriff hinzu.', // DeepL
  NETWORK_CORS_ORIGIN_HELP: 'Die Webadresse, über die die API von einem Browser aus aufgerufen werden kann, zum Beispiel http://homeassistant.local:8123', // DeepL
  NETWORK_CORS_WILDCARD: 'Mit * kann jede in einem Browser in Ihrem Netzwerk geöffnete Website die API aufrufen. Geben Sie stattdessen die genaue Adresse an.', // DeepL
  NETWORK_ALLOWED_HOSTS: 'Zulässige Hostnamen', // DeepL
  NETWORK_ALLOWED_HOSTS_HELP: 'Diese Geräte können unter verschiedenen Namen in einem Browser aufgerufen werden, beispielsweise „ems-esp.fritz.box“ oder einem Reverse-Proxy-Namen. Ihre IP-Adresse, ihr Hostname und „hostname.local“ funktionieren stets.', // DeepL
  NOT_ENABLED: 'Nicht aktiviert',
  MQTT_ERRORS: 'MQTT-Fehler',
  DISCONNECT_REASON: 'Grund für die Unterbrechung', // DeepL
  MQTT_BASE_TOPIC: 'Grundlegendes Thema', // DeepL
  LAST_DISCONNECT: 'Letzte Unterbrechung', // DeepL
  CONNECTED_SINCE: 'Verbunden seit',
  ENABLE_MQTT: 'MQTT aktivieren',
  BROKER: 'Broker',
  BASE_TOPIC: 'Grundlagen', // DeepL
  OPTIONAL: 'optional', // DeepL
  MQTT_FORMAT: 'Thema/Payload-Format', // DeepL
  MQTT_NEST_1: 'In einem einzigen Thema verschachtelt', // DeepL
  MQTT_NEST_2: 'Als einzelne Themen', // DeepL
  MQTT_RESPONSE: 'Veröffentlichen Sie die Ausgabe der Befehle in einem `response`-Thema', // DeepL
  MQTT_PUBLISH_TEXT_1: 'Veröffentlichen Sie Einzelwert-Topics bei Änderungen', // DeepL
  MQTT_PUBLISH_TEXT_2: 'Veröffentlichung in Befehlsthemen (ioBroker)', // DeepL
  MQTT_PUBLISH_TEXT_3: 'MQTT-Erkennung aktivieren', // DeepL
  MQTT_PUBLISH_TEXT_4: 'Präfix für die Discovery-Themen', // DeepL
  MQTT_PUBLISH_TEXT_5: 'Art der Erkennung', // DeepL
  MQTT_INPUT_NUMBER_FORMAT: 'Format für Zahleneingaben', // DeepL
  MQTT_PUBLISH_INTERVALS: 'Veröffentlichungsintervalle', // DeepL
  MQTT_INT_BOILER: 'Heizkessel und Wärmepumpen', // DeepL
  MQTT_INT_THERMOSTATS: 'Thermostate',
  MQTT_INT_SOLAR: 'Solarmodule',
  MQTT_INT_MIXER: 'Mischmodule', // DeepL
  MQTT_INT_WATER: 'Wassermodule', // DeepL
  MQTT_QUEUE: 'MQTT-Warteschlange',
  DEFAULT: 'Standard',
  MQTT_ENTITY_FORMAT: 'Format der Entitäts-ID', // DeepL
  MQTT_ENTITY_FORMAT_0: 'Einmalige Verwendung, langer Name (v3.4)', // DeepL
  MQTT_ENTITY_FORMAT_1: 'Einmalige Verwendung, kurzer Name', // DeepL
  MQTT_ENTITY_FORMAT_2: 'Mehrere Instanzen, kurzer Name', // DeepL
  MQTT_CLEAN_SESSION: '„Set clean session“', // DeepL
  MQTT_RETAIN_FLAG: 'Setzen Sie stets das Beibehaltungsflag', // DeepL
  MQTT_COMMANDS: 'Über MQTT empfangene Befehle',
  MQTT_COMMANDS_0: 'Aus, nur Werte lesen',
  MQTT_COMMANDS_1: 'Werte ändern und Befehle ausführen', // DeepL
  MQTT_COMMANDS_2: 'Alles, einschließlich Systembefehle', // DeepL
  INACTIVE: 'Inaktiv',
  ACTIVE: 'Aktiv',
  UNKNOWN: 'Unbekannt',
  SET_TIME: 'Zeit einstellen', // DeepL
  SET_TIME_TEXT: 'Geben Sie unten das lokale Datum und die Uhrzeit ein, um die Uhrzeit einzustellen', // DeepL
  LOCAL_TIME: 'Ortszeit', // DeepL
  UTC_TIME: 'UTC-Zeit',
  ENABLE_NTP: 'NTP aktivieren', // DeepL
  NTP_SERVER: 'NTP-Server',
  TIME_ZONE: 'Zeitzone',
  ACCESS_POINT: 'Zugangspunkt',
  AP_PROVIDE: 'Zugangspunkt aktivieren', // DeepL
  AP_PROVIDE_TEXT_2: 'Wenn keine Netzwerkverbindung gefunden wird',
  AP_PROVIDE_TEXT_3: 'Niemals',
  AP_PREFERRED_CHANNEL: 'Bevorzugter Kanal',
  AP_HIDE_SSID: 'SSID ausblenden', // DeepL
  AP_CLIENTS: 'AP-Clients',
  AP_MAX_CLIENTS: 'Maximale Anzahl an Clients', // DeepL
  AP_LOCAL_IP: 'Lokale IP-Adresse', // DeepL
  IDLE: 'Leerlauf',
  SCAN_AGAIN: 'Erneut scannen', // DeepL
  NETWORK_NO_WIFI: 'Es wurden keine WLAN-Netzwerke gefunden', // DeepL
  NETWORK_BLANK_BSSID: 'Lassen Sie das Feld leer, um nur die SSID zu verwenden', // DeepL
  TX_POWER: 'Sendeleistung', // DeepL
  HOSTNAME: 'Hostname',
  NETWORK_DISABLE_SLEEP: 'Deaktivieren Sie den WLAN-Ruhemodus', // DeepL
  NETWORK_LOW_BAND: 'Verwenden Sie eine geringere WLAN-Bandbreite', // DeepL
  NETWORK_USE_DNS: 'mDNS-Dienst aktivieren', // DeepL
  NETWORK_ENABLE_CORS: 'CORS aktivieren', // DeepL
  NETWORK_CORS_ORIGIN: 'CORS-Ursprung', // DeepL
  NETWORK_FIXED_IP: 'Verwenden Sie eine feste IP-Adresse', // DeepL
  NETWORK_GATEWAY: 'Gateway',
  NETWORK_SUBNET: 'Subnetzmaske', // DeepL
  NETWORK_DNS: 'DNS-Server',
  ADDRESS_OF: '{0}-Adresse',
  ADMINISTRATOR: 'Administrator',
  GUEST: 'Gast',
  NEW: 'Neu', // DeepL
  NEW_ENTITY_NAME: 'Neuer Entitätsname',
  MIN: 'min',
  MAX: 'max',
  BLOCK_NAVIGATE_1: 'Sie haben ungespeicherte Änderungen', // DeepL
  BLOCK_NAVIGATE_2: 'Wenn Sie zu einer anderen Seite wechseln, gehen Ihre nicht gespeicherten Änderungen verloren. Möchten Sie diese Seite wirklich verlassen?', // DeepL
  UPLOAD_ABORTED: 'Upload abgebrochen', // DeepL
  BLOCK_BUSY_1: 'Noch in Arbeit', // DeepL
  BLOCK_BUSY_2: 'Wenn Sie diese Seite nun verlassen, wird der Vorgang abgebrochen. Möchten Sie die Seite wirklich verlassen?', // DeepL
  DISCARD: 'Verwerfen',
  COMMANDS_UPDATED: 'Befehle aktualisiert',
  V_REQUIRED: 'Erforderlich',
  V_BETWEEN: 'Muss zwischen {min} und {max} liegen',
  V_MAX_CHARS: 'Muss maximal {max} Zeichen umfassen', // DeepL
  V_CHARS: 'Muss {min}–{max} Zeichen umfassen', // DeepL
  V_IN_USE: 'Bereits im Einsatz', // DeepL
  V_IP: 'Muss eine IP-Adresse sein',
  V_HOSTNAME: 'Es muss sich um einen gültigen Hostnamen handeln', // DeepL
  V_IP_OR_HOSTNAME: 'Muss eine IP-Adresse oder ein Hostname sein',
  V_NAME: 'Bis zu {max} Zeichen: Buchstaben, Ziffern oder _',
  V_USERNAME: 'Bis zu {max} Zeichen: Buchstaben, Ziffern, _ oder .',
  V_DAY: 'Wählen Sie mindestens einen Tag aus', // DeepL
  V_HEX: 'Muss eine Hexadezimalzahl sein', // DeepL
  V_FINGERPRINT: 'Es muss sich um einen SHA-1-Fingerabdruck (40 Hexadezimalziffern) handeln.', // DeepL
  V_EXAMPLE: 'Muss wie folgt aussehen: {example}', // DeepL
  V_NOT_DEFAULT: 'Wählen Sie ein anderes Passwort',
  ZERO_DISABLES: '0 schaltet die Funktion aus', // DeepL
  MINUS_ONE_DISABLES: '-1 schaltet die Funktion aus', // DeepL
  LED_TYPE: 'LED-Typ',
  PHY_POWER: 'PHY-Stromversorgung', // DeepL
  PHY_CLOCK: 'PHY-Takt',
  AUTO: 'Auto',
  PORT: 'Port',
  TIMEOUT: 'Zeitüberschreitung', // DeepL
  HOST: 'Host',
  SMTP_SERVER: 'SMTP-Server',
  KEEP_ALIVE: 'Keep-Alive',
  MQTT_REFRESH: 'MQTT aktualisieren',
  MQTT_REFRESHED: 'MQTT aktualisiert',
  MQTT_R_USER: 'Vom Benutzer getrennt',
  MQTT_R_TCP: 'TCP-Verbindung geschlossen',
  MQTT_R_PROTOCOL: 'Protokollversion nicht akzeptiert',
  MQTT_R_ID: 'Client-ID abgelehnt',
  MQTT_R_SERVER: 'Server nicht verfügbar',
  MQTT_R_CREDENTIALS: 'Ungültige Anmeldedaten', // DeepL
  MQTT_R_AUTH: 'Nicht autorisiert',
  MQTT_R_FINGERPRINT: 'Der TLS-Fingerabdruck stimmt nicht überein', // DeepL
  GPIO_INVALID: 'Ungültige GPIO-Einstellungen',
  CONNECTED_VIA: 'Verbunden über {0}',
  NETWORK_NOT_FOUND: 'Netzwerk nicht gefunden',
  CONNECTION_FAILED: 'Verbindung fehlgeschlagen',
  CONNECTION_LOST: 'Verbindung unterbrochen', // DeepL
  AP_LINGERING: 'Warten, bis der Systemzustand „Inaktiv“ ist', // DeepL
  NONE: 'Keine',
  LOG_NO_CONNECTION: 'Keine Konnektivität zum Protokollierungsdienst', // DeepL
  CHECK_SYSTEM_LOG: 'Überprüfen Sie das Systemprotokoll auf mögliche Ursachen', // DeepL
  VALUE_TYPE: 'Werttyp',
  DEFAULT_VALUE: 'Standardwert',
  DEFAULT_NAME: 'Standardname',
  ALWAYS_OFF: 'Immer ausgeschaltet', // DeepL
  ALWAYS_ON: 'Immer eingeschaltet', // DeepL
  NO_DATA: 'Noch keine Favoriten. Markieren Sie diese unter {customizations} oder sehen Sie sich alle Entitäten unter {devices} an.', // DeepL
  STAY: 'Bleiben Sie', // DeepL
  LEAVE: 'Überspringen', // DeepL
  MENU: 'Menü',
  MORE: 'Mehr',
  SHOW_PASSWORD: 'Passwort anzeigen',
  STATUS_RUNNING: 'EMS-ESP läuft',
  NUM_ERRORS: '{num} Fehler',
  NUM_WARNINGS: '{num} Warnung{{en}}',
  HEAP: 'Heap',
  METER_USED: '{used} / {total} {unit} verbraucht', // DeepL
  MAX_BLOCK: 'Maximal {num}-KB-Block', // DeepL
  MAX_BLOCK_HELP: '„Max Block“ ist der größte Heap-Block, der auf einmal zugewiesen werden kann. Unterhalb von 40 KB können die Weboberfläche und MQTT ausfallen.', // DeepL
  FLASH_CHIP: 'Flash-Chip',
  FILE_SYSTEM: 'Dateisystem',
  DETAILS: 'Details',
  RECONNECTS: 'Verbindung wird wiederhergestellt', // DeepL
  CONNECTS: 'Verbindet', // DeepL
  ACTIVITY_HELP: 'Einige wenige Rx-Fehler sind normal. Viele Tx-Fehler deuten in der Regel darauf hin, dass der falsche Tx-Modus für Ihren Heizkessel eingestellt ist.', // DeepL
  PAUSE: 'Pause',
  RESUME: 'Zusammenfassung', // DeepL
  LOG_PAUSED: 'Angehalten, Zeilenumbrüche zurückgehalten: {num}', // DeepL
  FILTER_LINES: 'Filterzeilen', // DeepL
  SCHEDULER: 'Planer',
  SCHEDULER_HELP_1: 'Automatisieren Sie Befehle, indem Sie unten geplante Ereignisse hinzufügen', // DeepL
  SCHEDULER_HELP_2: 'Beim Start', // DeepL
  SCHEDULE: 'Zeitplan',
  TIME: 'Zeit',
  TIMER: 'Timer',
  ONCHANGE: 'Bei Änderungen', // DeepL
  CONDITION: 'Bedingung', // DeepL
  SCHEDULE_UPDATED: 'Zeitplan aktualisiert',
  CUSTOM_ENTITIES: 'Benutzerdefinierte Entitäten', // DeepL
  ENTITIES_HELP_1: 'Definieren Sie benutzerdefinierte EMS-Entitäten oder dynamische Benutzervariablen', // DeepL
  ENTITIES_UPDATED: 'Aktualisierte Entitäten', // DeepL
  WRITEABLE: 'Bearbeitbar', // DeepL
  SHOWING: 'Anzeigen', // DeepL
  SEARCH: 'Suche',
  CERT: 'CA-Zertifikat (PEM)',
  ENABLE_TLS: 'TLS aktivieren', // DeepL
  TLS_CHECK: 'Überprüfen Sie den Broker mit', // DeepL
  TLS_CHECK_CA: 'CA-Zertifikat',
  TLS_CHECK_FINGERPRINT: 'Zertifikats-Fingerabdruck (SHA-1)', // DeepL
  TLS_CHECK_NONE: 'Keine Angabe, nicht angekreuzt', // DeepL
  TLS_NOT_CHECKED: 'Die Verbindung ist verschlüsselt, der Broker wird jedoch nicht überprüft, sodass sich ein anderes Gerät im Netzwerk als dieser ausgeben könnte', // DeepL
  ON: 'Auf', // DeepL
  OFF: 'Aus',
  POLARITY: 'Polarität',
  ACTIVEHIGH: 'Aktiv hoch', // DeepL
  ACTIVELOW: 'Aktiv-Low', // DeepL
  UNCHANGED: 'Unverändert',
  SECURITY_1: 'Benutzer hinzufügen oder entfernen',
  DOWNLOAD_UPLOAD_1: 'Einstellungen herunterladen und hochladen', // DeepL
  MODULES: 'Module',
  MODULES_1: 'Externe Module aktivieren oder deaktivieren',
  NETWORK_DESC: 'WLAN, Ethernet und Hostname',
  AP_DESC: 'Eigener Hotspot für Einrichtung und Notfallzugriff',
  MQTT_DESC: 'Broker, Topics und Discovery',
  APPLICATION_DESC: 'EMS-Bus, Board, Sensoren und Formatierung',
  NTP_DESC: 'Zeitserver und Zeitzone',
  FIRMWARE_DESC: 'Version, Updates und Wiederherstellung',
  MODULES_UPDATED: 'Module aktualisiert',
  MODULES_NONE: 'Es wurden keine externen Module erkannt', // DeepL
  RENAME: 'Umbenennen',
  ENABLE_MODBUS: 'Modbus aktivieren',
  VIEW_LOG: 'Protokoll anzeigen, um Probleme zu diagnostizieren', // DeepL
  SERVICES: 'Dienste',
  ALLVALUES: 'Alle Werte',
  WAIT_FIRMWARE: 'Die Firmware wird hochgeladen und installiert', // DeepL
  INSTALL_VERSION: 'Dies installiert Version {0}',
  UPDATE_AVAILABLE: 'Update verfügbar', // DeepL
  LATEST_VERSION: 'Sie verwenden die aktuellste Firmware-Version ({0})',
  PLEASE_WAIT: 'Bitte warten Sie', // DeepL
  RESTARTING_PRE: 'Systemstart', // DeepL
  RESTARTING_POST: 'Vorbereitung',
  AUTO_SCROLL: 'Automatischer Bildlauf', // DeepL
  DASHBOARD: 'Dashboard',
  DEVELOPER_MODE: 'Entwicklermodus',
  DISABLE_RESET: 'Werkseinstellungen zurücksetzen deaktivieren', // DeepL
  AUTO_FW_CHECK: 'Automatisch nach Firmware-Updates suchen',
  BYTES: 'Bytes',
  BITMASK: 'Bitmaske', // DeepL
  DUPLICATE: 'Duplikat', // DeepL
  DASHBOARD_1: 'Alle aktiven EMS-Entitäten, die als Favoriten markiert sind, werden im Folgenden angezeigt, ebenso wie alle benutzerdefinierten Entitäten, Zeitpläne und externen Sensoren.', // DeepL
  NO_GPIO: 'Es wurde kein verfügbarer GPIO gefunden', // DeepL
  PLATFORM: 'Plattform',
  RELEASE_TYPE: 'Release-Typ', // DeepL
  FIRMWARE_VERSION_INFO: 'Firmware-Version', // DeepL
  USER_PROFILE: 'Benutzerprofil',
  ONLINE_HELP: 'Online-Hilfe',
  UPGRADE_IMPORTANT_MESSAGES: 'Wichtige Update-Meldungen', // DeepL
  UPGRADE_IMPORTANT_MESSAGES_2: 'Sie führen ein Upgrade auf eine neue Hauptversion durch. Es wird empfohlen, zunächst eine Systemsicherung durchzuführen. Überprüfen Sie außerdem die Online-Hilfe auf etwaige grundlegende Änderungen.', // DeepL
  UPGRADE_IMPORTANT_MESSAGES_3: 'Version 3.10 enthält grundlegende Sicherheitsänderungen. Erstellen Sie zunächst eine Systemsicherung: Nach dem Upgrade werden die Passwörter gehashte gespeichert, und ältere Versionen können diese nicht mehr lesen; für eine Rückkehr zu einer früheren Version ist daher diese Sicherung erforderlich. Jeder API-Aufruf erfordert fortan einen API-Schlüssel, sodass Integrationen wie Home Assistant aktualisiert werden müssen. Lesen Sie die grundlegenden Änderungen in der Online-Hilfe, bevor Sie fortfahren.', // DeepL
  WARNING_SYSTEM_BACKUP: 'Hiermit erstellen Sie eine Sicherungskopie Ihrer vollständigen Systemkonfiguration und Einstellungen. Passwörter und Schlüssel werden nicht berücksichtigt, es sei denn, Sie geben sie unten an; bei der Wiederherstellung der Sicherungskopie bleiben die bereits auf den Geräten gespeicherten Passwörter erhalten. Befehle sind stets enthalten und können Anmeldedaten enthalten; seien Sie daher vorsichtig beim Weitergeben. Möchten Sie fortfahren?', // DeepL
  BACKUP_INCLUDE_SECRETS: 'Fügen Sie Passwörter und Schlüssel hinzu (die für die Wiederherstellung auf einem neuen Gerät benötigt werden)', // DeepL
  TEST_EMAIL_SUCCESSFUL: 'Test-E-Mail gesendet', // DeepL
  SYSTEM_NAME: 'Systemname',
  COMMANDS: 'Befehle',
  COMMANDS_HELP_1: 'Definieren Sie im Folgenden wiederverwendbare, benannte Befehle. Diese können über die Konsole, die API/MQTT ausgeführt oder vom Planer aufgerufen werden.', // DeepL
  COMMAND_EXECUTED: 'Befehl ausgeführt', // DeepL
  RUN: 'Ausführen', // DeepL
  INTERNET_CONNECTION_CHECK: 'Online nach verfügbaren Versionen suchen', // DeepL
  UPLOAD_BACKUP_TEXT: 'Klicken Sie hier, um eine Sicherungsdatei (.json) auszuwählen, oder ziehen Sie sie hierher', // DeepL
  UPLOAD_FIRMWARE_TEXT: 'Klicken Sie hier, um eine Firmware-Datei im Format .bin/.md5 auszuwählen oder hier abzulegen', // DeepL
  UPLOAD_MD5_RECEIVED: 'Digest empfangen, laden Sie nun die Firmware-Datei im .bin-Format hoch', // DeepL
  UPLOAD_MD5_MATCHED: 'MD5-Übereinstimmung, Firmware-Installation…', // DeepL
  UPLOAD_NOT_OFFICIAL: 'Dies ist keine offizielle EMS-ESP-Version. Installieren Sie nur Firmware, der Sie vertrauen.', // DeepL
  UPLOAD_NOT_OFFICIAL_HELP: 'Informationen zu offiziellen und benutzerdefinierten Builds', // DeepL
  UPLOAD_INSTALL_ANYWAY: 'Trotzdem installieren',
  OFFICIAL_BUILD: 'Offizielle Version', // DeepL
  CUSTOM_BUILD: 'Benutzerdefinierte Version', // DeepL
  USING_CUSTOM_FIRMWARE: 'Sie verwenden eine benutzerdefinierte Firmware', // DeepL
  PASSWORD_CONFIRM_TITLE: 'Geben Sie Ihr Passwort erneut ein', // DeepL
  PASSWORD_CONFIRM_TEXT: 'Zu Ihrer Sicherheit geben Sie bitte vor dieser Änderung Ihr Passwort ein. Sie werden 5 Minuten lang nicht erneut dazu aufgefordert.', // DeepL
  PASSWORD_CONFIRM_BUTTON: 'Bestätigen',
  PASSWORD_WRONG: 'Falsches Passwort',
  PASSWORD_NOT_CONFIRMED: 'Abgebrochen, das Passwort wurde nicht eingegeben',
  RECOVERY_APP: 'Recovery-App', // DeepL
  RECOVERY_NOT_INSTALLED: 'Nicht installiert',
  RECOVERY_INSTALLED: 'Wiederherstellungs-Firmware installiert',
  RESTART_INTO_RECOVERY: 'Neustart in die Recovery-App', // DeepL
  RECOVERY_NOT_FOUND: 'Die Recovery-App wurde online nicht gefunden', // DeepL
  RECOVERY_START_FAILED: 'Die Recovery-App lässt sich nicht starten; möglicherweise ist sie veraltet oder beschädigt. Installieren Sie zunächst die neueste Version.', // DeepL
  PARTITION_START_FAILED: 'Diese Firmware lässt sich nicht starten; sie ist möglicherweise beschädigt', // DeepL
  INSTALLED_VERSIONS: 'Installierte Versionen',
  EMSESP_APP: 'EMS-ESP-App', // DeepL
  NO_UPDATES_AVAILABLE: 'Es sind keine Aktualisierungen verfügbar', // DeepL
  SWITCH_TO_DEV: 'Wechseln Sie zu dev {0}', // DeepL
  SWITCH_TO_STABLE: 'Wechseln Sie zur stabilen Version {0}', // DeepL
  SAVE: 'Speichern',
  NEEDS_RESTART: 'Muss neu gestartet werden', // DeepL
  CHANGED_SETTINGS: '{num} geänderte Einstellung{{en}}',
  SAVED_RESTART: 'Gespeichert. Starten Sie EMS-ESP neu, um die neuen Einstellungen zu übernehmen.', // DeepL
  RESTART_NOW: 'Jetzt neu starten',
  COPY_FAILED: 'Das Kopieren ist nicht möglich; wählen Sie den Text aus und kopieren Sie ihn bitte selbst.', // DeepL
  DOCUMENTATION: 'Dokumentation',
  COMMUNITY_CHAT: 'Community-Chat',
  REPORT_ISSUE: 'Einen Fehler melden oder eine Funktion anfordern', // DeepL
  GET_HELP: 'Hilfe erhalten', // DeepL
  REPORT_PROBLEM: 'Ein Problem melden',
  REPORT_STEP_1: 'Systeminformationen herunterladen',
  REPORT_STEP_1_TEXT: 'Version, Platine, EMS-Bus-Status, angeschlossene Geräte und Einstellungen, ohne Passwörter', // DeepL
  REPORT_STEP_2: 'Das aktuelle Protokoll speichern',
  REPORT_STEP_2_TEXT: 'Stellen Sie zunächst die Protokollierungsstufe auf „DEBUG“ ein, wenn das Problem bei einem Gerät liegt.', // DeepL
  REPORT_STEP_3: 'Eröffnen Sie ein Ticket oder stellen Sie Ihre Frage auf Discord', // DeepL
  REPORT_STEP_3_TEXT: 'Fügen Sie beide Dateien bei oder fügen Sie die kurze Zusammenfassung unten in einen Chat ein', // DeepL
  NEW_ISSUE: 'Neues Problem', // DeepL
  SHORT_SUMMARY: 'Kurze Zusammenfassung', // DeepL
  LICENSE_TEXT: 'Open Source unter der LGPL-3.0-Lizenz',
  ADMIN_ACCESS_TEXT: 'Voller Zugriff auf Einstellungen, Geräte und die API', // DeepL
  GUEST_ACCESS_TEXT: 'können Werte einsehen und die Befehle ausführen, die Gäste ausführen dürfen', // DeepL
  LANGUAGE_BROWSER_ONLY: 'Dies betrifft nur diesen Browser', // DeepL
  LANGUAGE_ENTITIES_HINT: 'Die Sprache für die Entitätsnamen der Geräte in MQTT und der API wird festgelegt in', // DeepL
  ADMINISTRATION: 'Verwaltung',
  API_KEYS_SHORT: 'Schlüssel für Home Assistant und Skripte',
  ADDRESSES: 'Adressen',
  SEARCH_SETTINGS: 'Sucheinstellungen', // DeepL
  NO_SETTINGS_MATCH: 'Es wurden keine Einstellungen gefunden, die Ihrer Suche entsprechen', // DeepL
  GENERAL: 'Allgemeines', // DeepL
  BOARD: 'Board',
  ADVANCED: 'Fortgeschritten', // DeepL
  EMS_BUS_DESC: 'So kommuniziert EMS-ESP mit Ihrer Heizungsanlage.', // DeepL
  BOARD_DESC: 'Die Pins stammen aus dem Board-Profil. Um sie selbst festzulegen, aktivieren Sie den Entwicklermodus und wählen Sie „Benutzerdefiniert“.', // DeepL
  FORMAT_DESC: 'So werden Werte in der Web-Benutzeroberfläche und in MQTT-/API-Nutzdaten dargestellt.', // DeepL
  BOARD_PINS: 'Pins aus dem Profil',
  ENABLE_EMAIL: 'E-Mail-Benachrichtigungen aktivieren',
  SEND_TEST_EMAIL: 'Test-E-Mail senden',
  EMAIL_FROM: 'Aus', // DeepL
  EMAIL_TO: 'Zu', // DeepL
  EMAIL_SUBJECT: 'Betreff',
  WIFI_SCAN: 'WLAN-Scan', // DeepL
  SCAN: 'Scannen', // DeepL
  NETWORKS_NEARBY: 'Netzwerke in der Nähe',
  SIGNAL: 'Signal',
  CHANNEL: 'Kanal',
  WIFI_OPEN: 'öffnen', // DeepL
  WIFI_SAVED: 'gespeichert',
  NETWORK_ETHERNET_INFO: 'Derzeit über Ethernet verbunden ({ip}). WLAN wird nur ohne Kabel verwendet.', // DeepL
  HOSTNAME_ACCESS: 'Hostname und Zugriff',
  IP_ADDRESS: 'IP-Adresse',
  NETWORK_ETH_10MBIT: '10 Mbit Halbduplex erzwingen',
  NETWORK_ETH_10MBIT_HELP: 'Nur bei defekten Schaltern oder langen Kabeln', // DeepL
  NETWORK_DISABLE_SLEEP_HELP: 'zuverlässiger, verbraucht etwas mehr Strom', // DeepL
  NETWORK_MDNS_HELP: 'Erreichbar unter {host}.local über mDNS', // DeepL
  NETWORK_FIXED_IP_HELP: 'Andernfalls wird DHCP verwendet', // DeepL
  NETWORK_CORS_HELP: 'Nur erforderlich, wenn eine andere Webseite die API aufruft', // DeepL
  NTP_TIME_ON_DEVICE: 'Zeit auf EMS-ESP',
  NTP_TIME_SERVER: 'Zeitserver',
  NTP_THERMOSTAT_CLOCK: 'Thermostat-Uhr', // DeepL
  NTP_THERMOSTAT_SYNC: 'Stellen Sie die Uhr des Thermostats ein', // DeepL
  NTP_THERMOSTAT_SYNC_HELP: 'Stellt Datum und Uhrzeit am EMS-Thermostat ein', // DeepL
  NTP_THERMOSTAT_SYNC_1: 'Ein, EMS-ESP-Zeitzone', // DeepL
  NTP_THERMOSTAT_SYNC_2: 'Ein, eigene Zeitzone', // DeepL
  NTP_THERMOSTAT_TZ: 'Zeitzone des Thermostats',
  MQTT_PREVIEW: 'Was veröffentlicht wird',
  MQTT_PREVIEW_CMD: 'wird als Befehl akzeptiert', // DeepL
  MQTT_PREVIEW_RESPONSE: 'Ausgabe von Befehlen',
  MQTT_BEHAVIOUR: 'Verhalten',
  MQTT_TOPICS: 'Themen und Nutzdaten', // DeepL
  MQTT_DISCOVERY: 'Erkundung', // DeepL
  MQTT_INTERVALS_HELP: 'Sekunden zwischen den Veröffentlichungen, 0 = bei jeder Änderung veröffentlichen.', // DeepL
  MQTT_SINGLE_NO_DISCOVERY: 'nicht verfügbar bei der Erkennung von', // DeepL
  MQTT_DISCOVERY_NO_SINGLE: 'Nicht verfügbar bei Themen mit Einzelwerten unter', // DeepL
  MQTT_DISCOVERY_HELP: 'Erstellt die Entitäten in Home Assistant oder Domoticz automatisch', // DeepL
  MQTT_ENTITY_FORMAT_WARN: 'Eine Änderung hier führt dazu, dass alle Entitäten in Home Assistant umbenannt werden', // DeepL
  USERS: 'Benutzer',
  USERS_TEXT: 'Administratoren können alle Einstellungen ändern. Gäste können sich die Inhalte ansehen und für Gäste gekennzeichnete Befehle ausführen.', // DeepL
  ADD_USER: 'Benutzer hinzufügen',
  EDIT_USER: 'Benutzer bearbeiten',
  ROLE: 'Rolle',
  ROLE_ADMIN: 'admin', // DeepL
  ROLE_GUEST: 'Gast',
  ADMIN: 'Admin',
  YOU: 'Sie', // DeepL
  NEW_PASSWORD: 'Neues Passwort',
  UNSAVED_CHANGES: '{num} ungespeicherte Änderung{{en}}',
  CREATE_KEY: 'Schlüssel erstellen',
  NEW_API_KEY: 'Neuer API-Schlüssel',
  NO_API_KEYS: 'Noch keine API-Schlüssel',
  API_KEYS_LIMIT: 'Sie können maximal {num} Tasten verwenden.', // DeepL
  REVOKE: 'Widerrufen',
  REVOKE_KEY: 'API-Schlüssel widerrufen',
  REVOKE_KEY_TEXT: 'Alles, was „{name}“ enthält, funktioniert sofort nicht mehr.', // DeepL
  MODULES_TEXT: 'In diese Firmware integrierte optionale Funktionen. Hier können Sie diese aktivieren oder deaktivieren.', // DeepL
  BY_AUTHOR: 'von {name}',
  LICENSE_KEY: 'Lizenzschlüssel',
  MODULE_ACTIVATED: 'aktiviert',
  MODULE_PENDING: 'Aktivierung ausstehend',
  ENABLED: 'Aktiviert',
  UNDO: 'Rückgängig',
  FW_UPDATES: 'Aktualisierungen', // DeepL
  FW_VERSION_AVAILABLE: '{version} ist verfügbar',
  FW_RELEASE_NOTES: 'Versionshinweise',
  FW_UPDATES_OFFLINE: 'Es konnte nicht online nach neuen Versionen gesucht werden. EMS-ESP benötigt hierfür eine Internetverbindung.', // DeepL
  FW_RESTART_RESET: 'Neustart und Zurücksetzen',
  FW_RECOVERY_TEXT: 'In die Recovery-App neu starten? Diese verbindet sich mit Ihrem Netzwerk und richtet zudem einen eigenen Zugangspunkt ein, über den Sie Firmware hochladen können.', // DeepL
  FW_SIZE: 'Größe',
  FW_INSTALL_DATE: 'Installiert auf', // DeepL
  FW_UPLOAD_FAILED: 'Upload fehlgeschlagen: falscher Dateityp oder Firmware für eine andere Platine', // DeepL
  FILE_TYPE_INVALID: 'Dieser Dateityp wird nicht unterstützt. Wählen Sie bitte eine {types}-Datei aus.', // DeepL
  WHO_CAN_RUN: 'Wer kann das Programm ausführen?', // DeepL
  CMD_NONE: 'Es wurden keine Befehle gefunden', // DeepL
  CMD_NONE_MATCH: 'Es wurden keine Befehle gefunden, die Ihrer Suche entsprechen', // DeepL
  SCHEDULE_NONE: 'Es wurden keine Zeitpläne gefunden', // DeepL
  SCHEDULE_NONE_MATCH: 'Es wurden keine Zeitpläne gefunden, die Ihrer Suche entsprechen', // DeepL
  CE_NONE: 'Es wurden keine Entitäten gefunden', // DeepL
  CE_ALL: 'Alle',
  CE_STORAGE: 'Speicherung', // DeepL
  CE_SOURCE: 'Quelle',
  CE_EMS_SOURCE: 'Gerät {device} · Telegramm {type} · Offset {offset}',
  CE_RAM_TEXT: 'wird bis zum Neustart beibehalten', // DeepL
  CE_NVS_TEXT: 'bleibt auch nach einem Neustart erhalten', // DeepL
  CE_EXCLUDED: 'nicht in API/MQTT',
  SEARCH_SENSORS: 'Suche nach Name, ID oder GPIO', // DeepL
  NO_SENSORS: 'Es wurden keine Sensoren gefunden', // DeepL
  NO_SENSORS_MATCH: 'Es wurden keine Sensoren gefunden, die Ihrer Suche entsprechen', // DeepL
  TEMP_SENSORS_HELP: 'Automatisch erkannt. Klicken Sie auf einen Sensor, um ihn umzubenennen oder einen Offset festzulegen', // DeepL
  FREE_GPIOS: 'Freie GPIOs',
  SYSTEM_SENSOR: 'Systemsensor',
  MASK_FAVORITE: 'Zu den Favoriten hinzufügen', // DeepL
  MASK_READONLY: 'Schreiben deaktiviert', // DeepL
  MASK_NO_API: 'Kein MQTT/API',
  MASK_HIDDEN: 'Verborgen', // DeepL
  MASK_REMOVED: 'Entfernt',
  FILTER_ALL: 'Alle',
  NUM_SELECTED: 'Ausgewählt: {num}',
  ALL_SHOWN: 'Alle anzeigen: {num}', // DeepL
  BULK_HINT: 'Die Schaltflächen ändern die markierten Zeilen oder zeigen alle Zeilen an, wenn keine markiert ist.', // DeepL
  CLEAR_SELECTION: 'Eindeutige Auswahl', // DeepL
  NO_ENTITIES_MATCH: 'Es wurden keine Entitäten gefunden', // DeepL
  LIMIT_HELP: 'Lassen Sie das Feld leer, um den integrierten Grenzwert zu verwenden.', // DeepL
  MIN_ABOVE_MAX: 'Der Mindestwert darf den Höchstwert nicht überschreiten', // DeepL
  RENAME_DEVICE: 'Gerät umbenennen',
  WEEKLY: 'Wöchentlich',
  EVERY_DAY: 'Jeden Tag',
  SCHEDULER_NO_COMMANDS: 'Fügen Sie zunächst einen Befehl auf der Seite „Befehle“ hinzu', // DeepL
  DU_SETTINGS: 'EMS-ESP-Einstellungen',
  DU_SETTINGS_TEXT: 'Sicherung der EMS-ESP-Einstellungen: Netzwerk, MQTT, Anpassungen, Planer, benutzerdefinierte Entitäten und Module. Das Hochladen einer Einstellungs- oder Sicherungsdatei führt zu einem Neustart von EMS-ESP.', // DeepL
  HB_TITLE: 'Heizungseinstellungen',
  HB_TEXT: 'Speichert die aktuellen Werte Ihres Heizkessels, Thermostats und anderer EMS-Geräte, sodass Sie diese später wiederherstellen können, beispielsweise nach dem Austausch eines Geräts. Schreibgeschützte Entitäten und Befehle werden bei einer Wiederherstellung niemals verändert.', // DeepL
  HB_BACKUP: 'Heizungseinstellungen sichern',
  HB_RESTORE: 'Heizungseinstellungen wiederherstellen',
  DATA_EXPORT: 'Datenexport',
  DATA_EXPORT_TEXT: 'Für Support- und Analysezwecke. Diese Dateien können nicht wieder hochgeladen werden.', // DeepL
  NUM_VALUES: '{num} Wert{{e}}',
  HB_NOTHING: 'keine weiteren Angaben erforderlich', // DeepL
  FILE_NAME: 'Dateiname',
  HB_NOTE: 'Hinweis (optional, wird beim Wiederherstellen erneut angezeigt)', // DeepL
  HB_INCOMPLETE: 'Werte, die noch nicht vom Bus ausgelesen wurden, werden nicht angezeigt. Warten Sie nach einem Neustart einige Minuten, bis eine vollständige Datensicherung vorliegt.', // DeepL
  HB_RESTORE_TEXT: 'Wählen Sie eine Sicherungskopie der Heizungseinstellungen aus. Es wird nichts gespeichert, bis Sie die Änderungen überprüft und bestätigt haben.', // DeepL
  HB_DROP: 'Klicken Sie hier, um eine Heizungs-Backup-Datei (.json) auszuwählen, oder ziehen Sie diese hier hinein', // DeepL
  HB_INVALID: 'Dies ist keine Sicherung der Heizungseinstellungen', // DeepL
  HB_OTHER_FILE: 'Bitte wählen Sie eine andere Datei aus', // DeepL
  HB_SAME_DEVICE: 'dasselbe Gerät', // DeepL
  HB_NOT_CONNECTED: 'Nicht verbunden', // DeepL
  HB_OTHER_DEVICE: 'anderes Gerät (Produkt-ID {backup}im Backup, {now}verbunden), übersprungen', // DeepL
  HB_UNIT_MISMATCH: 'Diese Sicherung enthält Temperaturangaben in {unit}. Ändern Sie die Temperatureinheit in den Anwendungseinstellungen, bevor Sie die Sicherung wiederherstellen.', // DeepL
  HB_CHANGED: 'Geändert',
  ALL: 'Alle',
  HB_NOW: 'Nun', // DeepL
  HB_BACKUP_VALUE: 'Sicherung',
  HB_NOT_AVAILABLE: 'nicht verfügbar',
  HB_SENDING: 'Senden', // DeepL
  HB_WRITTEN: 'geschrieben',
  HB_RESTORE_NUM: '{num} Wert{{e}} wiederherstellen',
  RESTORE: 'Wiederherstellen',
  HB_CONFIRM: '{num} Wert{{e}} aus {file} in die EMS-Geräte schreiben?',
  HB_CONFIRM_TEXT: 'Werte werden einzeln gesendet. Wenn ein Gerät einen Wert ablehnt, werden die übrigen dennoch gesendet.', // DeepL
  HB_DONE: '{num} Wert{{e}} geschrieben',
  HB_DONE_FAILED: '{num} ist fehlgeschlagen. Geräte lehnen Werte auf Installateur-Ebene häufig ab; Einzelheiten finden Sie im Systemprotokoll.', // DeepL
  HB_NO_CHANGES: 'Es gibt keine Abweichungen vom Backup', // DeepL
  HB_DIFFERENT_DEVICE: 'anderes Gerät',
  SELECT_ALL: 'Alles auswählen', // DeepL
  DASHBOARD_EMPTY: 'Es gibt noch nichts anzuzeigen', // DeepL
  NO_DEVICES: 'Es wurden noch keine EMS-Geräte gefunden', // DeepL
  ADD_COMMAND: 'Befehl hinzufügen',
  EDIT_COMMAND: 'Befehl „Bearbeiten“', // DeepL
  RUN_COMMAND: 'Befehl ausführen',
  ADD_SCHEDULE: 'Zeitplan hinzufügen',
  EDIT_SCHEDULE: 'Zeitplan bearbeiten',
  ADD_ENTITY: 'Entität hinzufügen',
  EDIT_ENTITY: 'Entität bearbeiten', // DeepL
  ADD_ANALOG_SENSOR: 'Analogen Sensor hinzufügen', // DeepL
  EDIT_ANALOG_SENSOR: 'Analogsensoren bearbeiten', // DeepL
  REMOVE_ANALOG_SENSOR: 'Analogsensor entfernen', // DeepL
  REMOVE_SENSOR_TEXT: 'Der Sensor „{name}“ wird sofort entfernt.', // DeepL
  EDIT_TEMP_SENSOR: 'Temperatursensor bearbeiten',
  INSTALL_FIRMWARE: 'Firmware installieren ({channel})',
  PARTITION: 'Abschnitt', // DeepL
  PULSE: 'Pulse', // DeepL
};

export default de;
