import type { Translation } from '../i18n-types';

const tr: Translation = {
  LANGUAGE: 'Dil',
  RETRY: 'Tekrar Dene',
  LOADING: 'Yükleniyor',
  IS_REQUIRED: '{0} gerekiyor',
  SIGN_IN: 'Giriş Yap',
  SIGN_OUT: 'Çıkış Yap',
  USERNAME: 'Kullanıcı Adı',
  PASSWORD: 'Şifre',
  SU_PASSWORD: 'SK Şifresi',
  SETTINGS_OF: '{0} Ayarlar',
  HELP: 'Yardım',
  LOGGED_IN: '{name} olarak giriş yapıldı',
  PLEASE_SIGNIN: 'Lütfen devam etmek için giriş yapın',
  UPLOAD_SUCCESSFUL: 'Yükleme tamamlandı',
  DOWNLOAD_SUCCESSFUL: 'İndirme tamamlandı',
  INVALID_LOGIN: 'Giriş bilgileri hatalı',
  NETWORK: 'Ağ',
  SECURITY: 'Güvenlik',
  ONOFF_CAP: 'AÇIK/KAPALI',
  ONOFF: 'açık/kapalı',
  TYPE: 'Tür',
  DESCRIPTION: 'Açıklama',
  ENTITIES: 'varlıklar',
  REFRESH: 'Yenile',
  EXPORT: 'Dışarı al',
  FAVORITES: "Favoriler",
  DEVICE_DETAILS: 'Cihaz Ayrıntıları',
  ID_OF: 'Kimlik {0}',
  DEVICE: 'Cihaz',
  PRODUCT: 'Ürün',
  VERSION: 'Sürüm',
  BRAND: 'Marka',
  ENTITY_NAME: 'Valık Adı',
  VALUE: '{{değer|Değer}}',
  DEVICES: 'Cihazlar',
  SENSORS: 'Sensörler',
  RUN_COMMAND: 'Çalıştırma Komutu',
  CHANGE_VALUE: 'Değeri Değiştir',
  CANCEL: 'İptal',
  RESET: 'Reset',
  APPLY_CHANGES: 'Apply Changes ({0})',
  UPDATE: 'Update',
  EXECUTE: 'Execute', // TODO translate
  REMOVE: 'Kaldır',
  PROBLEM_UPDATING: 'Güncelleme Sorunu',
  PROBLEM_LOADING: 'Yükleme Sorunu',
  ANALOG_SENSOR: 'Analog Sensör',
  ANALOG_SENSORS: 'Analog Sensörler',
  SETTINGS: 'Ayarlar',
  UPDATED_OF: '{0} Güncellendi',
  UPDATE_OF: '{0} Güncelleme',
  REMOVED_OF: '{0} Kaldırıldı',
  DELETION_OF: '{0} Silindi',
  OFFSET: 'Fark',
  FACTOR: 'Etmen',
  FREQ: 'Frekans',
  DUTY_CYCLE: 'Görev Çevrimi',
  UNIT: 'ÖB',
  STARTVALUE: 'Başlangıç değeri',
  WARN_GPIO: 'Uyarı: bir GPIO atarken dikkatli olun!',
  EDIT: 'Değiştir',
  SENSOR: 'Sensör',
  TEMP_SENSOR: 'Sıcaklık Sensörü',
  TEMP_SENSORS: 'Sıcaklık Sensörleri',
  WRITE_CMD_SENT: 'Yazma komutu gönderildi',
  EMS_BUS_WARNING: 'EMS hat bağlantısı kesildi. Eğer bu uyarı birkaç saniye sonra devam ediyorsa lütfen ayarları ve kart tipini kontrol edin',
  EMS_BUS_SCANNING: 'EMS cihazları aranıyor...',
  CONNECTED: 'Bağlandı',
  TX_ISSUES: 'Tx sorunu - başka bir Tx Modu deneyin',
  DISCONNECTED: 'Bağlantı kesildi',
  EMS_SCAN: 'EMS Hattında tam bir cihaz taraması başlatmak istediğinizden emin misiniz?',
  DATA_TRAFFIC: 'Data Traffic', // TODO translate
  EMS_DEVICE: 'EMS Cihazı',
  SUCCESS: 'BAŞARILI',
  FAIL: 'HATA',
  QUALITY: 'KALİTE',
  SCAN: 'Tara',
  STATUS_NAMES: [
    'EMS Telegramlar Alındı (Rx)',
    'EMS Okuması (Tx)',
    'EMS Yazması (Tx)',
    'Sıcaklık Sensörü Okuması',
    'Analog Sensör Okuması',
    'MQTT Yayınları',
    'API Aramaları',
    'Sistem Kayıt Mesajları'
  ],
  NUM_DAYS: '{num} gün{{ler}}',
  NUM_SECONDS: '{num} saniye{{ler}}',
  NUM_HOURS: '{num} saat{{ler}}',
  NUM_MINUTES: '{num} dakika{{lar}}',
  APPLICATION: 'Uygulama',
  CUSTOMIZATIONS: 'Özelleştirme',
  APPLICATION_RESTARTING: 'EMS-ESP yeniden başlatılıyor',
  BOARD_PROFILE: 'Kart Profili',
  CUSTOM: 'Özel',
  GPIO_OF: '{0} GPIO',
  BUTTON: 'Düğme',
  TEMPERATURE: 'Sıcaklık',
  PHY_TYPE: 'Eth PHY Tipi',
  DISABLED: 'devre dışı',
  TX_MODE: 'EMS Tx Modu',
  HARDWARE: 'Donanım',
  EMS_BUS: '{{HAT|EMS HATTI}}',
  GENERAL_OPTIONS: 'Genel Seçenekler',
  LANGUAGE_ENTITIES: 'Dil (cihaz varlıkları için)',
  HIDE_LED: 'LEDi kapa',
  ENABLE_TELNET: 'Telnet Konsolunu Aktif Hale Getir',
  ENABLE_ANALOG: 'Analog Sensörleri Aktif Hale Getir',
  CONVERT_FAHRENHEIT: 'Sıcaklık değerlerini Fahrenheit a çevir',
  BYPASS_TOKEN: 'API bağlantılarında Erişim Jeton onaylamasını geç',
  READONLY: 'Salt okunur modu devreye al (bütün giden EMS Tx Yazma komutlarını engeller)',
  UNDERCLOCK_CPU: 'İşlemci hızını düşür',
  REMOTE_TIMEOUT: 'Remote timeout',
  REMOTE_TIMEOUT_EN: 'Disable remote on missing room temperature', // TODO translate
  HEATINGOFF: 'Start boiler with forced heating off', // TODO translate
  MIN_DURATION: 'Wait time',
  ENABLE_SHOWER_TIMER: 'Duş Sayacını Devreye Al',
  ENABLE_SHOWER_ALERT: 'Duş Alarmını Devreye Al',
  TRIGGER_TIME: 'Tetikleme Zamanı',
  COLD_SHOT_DURATION: 'Soğuk Atış Süreci',
  FORMATTING_OPTIONS: 'Formatlama Seçenekleri',
  BOOLEAN_FORMAT_DASHBOARD: 'Boolean Biçimleme Göstergesi',
  BOOLEAN_FORMAT_API: 'Boolean Biçimleme API/MQTT',
  ENUM_FORMAT: 'Enum Biçimleme API/MQTT',
  INDEX: 'İndeks',
  ENABLE_PARASITE: '1-wire parazit gücü devreye al',
  LOGGING: 'Kayıt ediliyor',
  LOG_HEX: 'EMS telegramlarını hexadecimal olarak kayıt et',
  ENABLE_SYSLOG: 'Sistem Kaydını Devreye Al',
  LOG_LEVEL: 'Kayıt Seviyesi',
  MARK_INTERVAL: 'İşaret Aralığı',
  SECONDS: 'saniyeler',
  MINUTES: 'dakikalar',
  HOURS: 'saatler',
  RESTART: 'Yeniden Başlat',
  RESTART_TEXT: 'EMS-ESP Sistem ayarlarının uygulanabilmesi için yeinden başlatılmalı',
  RESTART_CONFIRM: 'EMS-ESP yeniden başlatmak istediğinize emin misiniz?',
  COMMAND: 'Komut',
  CUSTOMIZATIONS_RESTART: 'Bütün özelleştirmeler kaldırıldı. Yeniden başlatılıyor...',
  CUSTOMIZATIONS_FULL: 'Seçilen varlıklar sınırı aşmaktadır. Lütfen parçalar halinde kaydedin',
  CUSTOMIZATIONS_SAVED: 'Özelleştirmeler kaydedildi',
  CUSTOMIZATIONS_HELP_1: 'Bir cihaz seçip varlıkların seçeneklerini özelleştirin veya yeniden adlandırmak için tıklayın',
  CUSTOMIZATIONS_HELP_2: 'favori olarak işaretle',
  CUSTOMIZATIONS_HELP_3: 'yazma işlemini devre dışı bırak',
  CUSTOMIZATIONS_HELP_4: 'MQTT ve APInin dışında bırak',
  CUSTOMIZATIONS_HELP_5: 'Cihazlardan gizle',
  CUSTOMIZATIONS_HELP_6: 'remove from memory',
  SELECT_DEVICE: 'Bir cihaz seç',
  SET_ALL: 'hepsini ayarla',
  OPTIONS: 'Seçenekler',
  NAME: 'İsim',
  CUSTOMIZATIONS_RESET: 'Sıcaklık ve Analog Sensörlerin özelleştirilmiş seçenekleri dahil bütün özelleştirmeleri kaldırmak istediğinizden emin misiniz?',
  SUPPORT_INFORMATION: 'Destek Bilgileri',
  HELP_INFORMATION_1: 'EMS-ESPnin nasıl ayarlanacağı ile ilgili bilgileri edinmek için çevrimiçi WIKI sayfasını ziyaret edin',
  HELP_INFORMATION_2: 'Canlı topluluk sohbeti için Discord sunucumuza katılın',
  HELP_INFORMATION_3: 'Yeni bir özellik talep etmek yada hata bildirmek için',
  HELP_INFORMATION_4: 'Bir sorun bildirirken daha hızlı bir dönüş için sistem bilginizi indirip eklemeyi unutmayın',
  UPLOAD: 'Yükleme',
  DOWNLOAD: '{{İ|i|i}}İndirme',
  INSTALL: 'Düzenlemek',
  ABORTED: 'iptal edildi',
  FAILED: 'başarısız',
  SUCCESSFUL: 'başarılı',
  SYSTEM: 'Sistem',
  LOG_OF: '{0} Kaydı',
  STATUS_OF: '{0} Durumu',
  DOWNLOAD_UPLOAD: 'İndirme/Yükleme',
  CLOSE: 'Kapat',
  USE: 'KUllan',
  FACTORY_RESET: 'Fabrika ayarına dönme',
  SYSTEM_FACTORY_TEXT: 'Cihaz fabrika ayarlarına döndü ve şimdi yendiden başlatılacak',
  SYSTEM_FACTORY_TEXT_DIALOG: 'Cihazı fabrika ayarlarına döndürmek istediğinize emin misiniz?',
  AVAILABLE_VERSION: 'Latest Available Versions', // TODO translate
  STABLE: 'Stable', // TODO translate
  DEVELOPMENT: 'Geliştirme',
  UPTIME: 'Sistem Çalışma Süresi',
  FREE_MEMORY: 'Yığın Memory',
  PSRAM: 'PSRAM (Boyut / Boş)',
  FLASH: 'Flash Çipi (Boyut , Hız)',
  APPSIZE: 'Uygulama (Bölme: Kullanılmış / Boş)',
  FILESYSTEM: 'Dosya Sistemi (Kullanılmış / Boş)',
  BUFFER_SIZE: 'En fazla bellek boyutu',
  COMPACT: 'Sıkışık',
  DOWNLOAD_SETTINGS_TEXT: 'Create a backup of your configuration and settings', // TODO translate
  UPLOAD_TEXT: 'Upload a new firmware file (.bin) or a backup file (.json)', // TODO translate
  UPLOAD_DROP_TEXT: 'Drop a firmware .bin file or click here', // TODO translate
  ERROR: 'Beklenemedik hata, lütfen tekrar deneyin.',
  TIME_SET: 'Zaman ayarı',
  MANAGE_USERS: 'Kullanıcıları yönet',
  IS_ADMIN: 'Yöneticidir',
  USER_WARNING: 'En az bir yönetici kullanıcısı ayarlamanız gerekmektedir',
  ADD: 'Ekle',
  ACCESS_TOKEN_FOR: 'Erişim Jetonunun sahibi',
  ACCESS_TOKEN_TEXT: 'Aşağıdaki Jeton yetki gerektiren REST API çağrıları ile kullanılmaktadır. Taşıyıcı Jeton olarak yetkilendirme başlığında yada erişim jetonu olarak URL sorgu parametresinde kullanılabilir.',
  GENERATING_TOKEN: 'Jeton oluşturuluyor',
  USER: 'Kullanıcı',
  MODIFY: 'Düzenle',
  SU_TEXT: 'SU(Süper kullanıcı şifresi yetkilendirme jetonlarını imzalamaya ve ayrıca konsolda yönetici ayrıcalıklarını etkinleştirmek için kullanılabilir',
  NOT_ENABLED: 'Etkinleştirilmedi',
  ERRORS_OF: '{0} Hata(ları)',
  DISCONNECT_REASON: 'Bağlantının kopma nedeni',
  ENABLE_MQTT: 'MQTTyi devreye al',
  BROKER: 'Aracı',
  CLIENT: 'İstemci',
  BASE_TOPIC: 'Merkez',
  OPTIONAL: 'seçenekli',
  FORMATTING: 'Biçimlendiriliyor',
  MQTT_FORMAT: 'Konu/Mesaj Biçimi',
  MQTT_NEST_1: 'Tek konu üzerine yerleşmiş',
  MQTT_NEST_2: 'Ayrı konular olarak',
  MQTT_RESPONSE: 'Komut çıktısını bir `cevap` konusuna yayınla',
  MQTT_PUBLISH_TEXT_1: 'Değişimde tek değerli konuları yayınla',
  MQTT_PUBLISH_TEXT_2: 'Komut konularına yayınla (ioBroker)',
  MQTT_PUBLISH_TEXT_3: 'MQTT keşfi etkinleştir (Home Assistant, Domoticz)',
  MQTT_PUBLISH_TEXT_4: 'Keşif konuları için ön ek',
  MQTT_PUBLISH_TEXT_5: 'Domoticz Format',
  MQTT_PUBLISH_INTERVALS: 'Yayınlama aralıkları',
  MQTT_INT_BOILER: 'Kazanlar ve Isı Pompaları',
  MQTT_INT_THERMOSTATS: 'Termostatlar',
  MQTT_INT_SOLAR: 'Güneş Enerjisi Modülleri',
  MQTT_INT_MIXER: 'Karışım Modülleri',
  MQTT_INT_WATER: 'Water Modules', // TODO translate
  MQTT_QUEUE: 'MQTT Sırası',
  DEFAULT: 'Varsayılan',
  MQTT_ENTITY_FORMAT: 'Varlık Kimlik biçimi',
  MQTT_ENTITY_FORMAT_0: 'Tekil örnek, uzun ad (v3.4)',
  MQTT_ENTITY_FORMAT_1: 'Tekil örnek, kısa ad',
  MQTT_ENTITY_FORMAT_2: 'Çoklu örnekler, kısa ad',
  MQTT_CLEAN_SESSION: 'Temiz Oturum ayarla',
  MQTT_RETAIN_FLAG: 'Herzaman hazır bayrağı ayarla',
  INACTIVE: 'Faal değil',
  ACTIVE: 'Faal',
  UNKNOWN: 'Bilinmeyen',
  SET_TIME: 'Zamanı ayarla',
  SET_TIME_TEXT: 'Zamanı ayarlamak için yerel tarih ve saati aşağıya giriniz',
  LOCAL_TIME: 'Yerel Saat',
  UTC_TIME: 'UTC Saati',
  ENABLE_NTP: 'NTP yi çalıştır',
  NTP_SERVER: 'NTP Sunucusu',
  TIME_ZONE: 'Saat dilimi',
  ACCESS_POINT: 'Erişim Noktası',
  AP_PROVIDE: 'Erişim noktasını çalıştır',
  AP_PROVIDE_TEXT_1: 'her zaman',
  AP_PROVIDE_TEXT_2: 'Kablosuz bağlantı kesildiğinde',
  AP_PROVIDE_TEXT_3: 'asla',
  AP_PREFERRED_CHANNEL: 'Tercih edilen kanal',
  AP_HIDE_SSID: 'SSID yi gizle',
  AP_CLIENTS: 'AP Kullanıcıları',
  AP_MAX_CLIENTS: 'Maks Kullanıcılar',
  AP_LOCAL_IP: 'Yerel IP',
  NETWORK_SCAN: 'Kablosuz Ağları Tara',
  IDLE: 'Serbest',
  LOST: 'Kayıp',
  SCANNING: 'Tarıyor',
  SCAN_AGAIN: 'Tekrar tara',
  NETWORK_SCANNER: 'Ağ Tarayıcısı',
  NETWORK_NO_WIFI: 'Hiçbir Kablosuz Ağ bulunamadı',
  NETWORK_BLANK_SSID: 'Kablosuz ağı devre dışı bırakmak için boş bırakın',
  NETWORK_BLANK_BSSID: 'leave blank to use only SSID', // TODO translate
  TX_POWER: 'Aktarım gücü',
  HOSTNAME: 'Ana Makine Adı',
  NETWORK_DISABLE_SLEEP: 'Kablosuz uyku modunu devre dışına al',
  NETWORK_LOW_BAND: 'Düşük kablosuz bant genişliği kullan',
  NETWORK_USE_DNS: 'mDNS Servisini deveye al',
  NETWORK_ENABLE_CORS: 'CORS u devreye al',
  NETWORK_CORS_ORIGIN: 'CORS kaynağı',
  NETWORK_FIXED_IP: 'Sabit IP adresi kullan',
  NETWORK_GATEWAY: 'Aü geçidi',
  NETWORK_SUBNET: 'Ağ Alt Maskesi',
  NETWORK_DNS: 'DNS Sunucuları',
  ADDRESS_OF: '{0} Adresi',
  ADMINISTRATOR: 'Yönetici',
  GUEST: 'Misafir',
  NEW: 'Yeni',
  NEW_NAME_OF: 'Yeni {0} adı',
  ENTITY: 'varlık',
  MIN: 'min',
  MAX: 'maks',
  BLOCK_NAVIGATE_1: 'You have unsaved changes', // TODO translate
  BLOCK_NAVIGATE_2: 'If you navigate to a different page, your unsaved changes will be lost. Are you sure you want to leave this page?', // TODO translate
  STAY: 'Stay', // TODO translate
  LEAVE: 'Leave', // TODO translate
  SCHEDULER: 'Scheduler', // TODO translate
  SCHEDULER_HELP_1: 'Automate commands by adding scheduled events below. Set a unique Name to enable/disable activation via API/MQTT', // TODO translate
  SCHEDULER_HELP_2: 'Use 00:00 to trigger once on start-up', // TODO translate
  SCHEDULE: 'Schedule', // TODO translate
  TIME: 'Time', // TODO translate
  TIMER: 'Timer', // TODO translate
  ONCHANGE: 'Değişimde',
  CONDITION: 'Durum', // TODO translate
  IMMEDIATE: 'hemen', // TODO translate
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
  CERT: 'TLS root certificate (leave blank for insecure)',
  ENABLE_TLS: 'TLS deveye al',
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
  MODULES_1: 'Harici modülleri etkinleştirin veya devre dışı bırakın',
  MODULES_UPDATED: 'Modules updated', // TODO translate
  MODULES_DESCRIPTION: 'Click on the Module to activate or de-activate EMS-ESP library modules', // TODO translate
  MODULES_NONE: 'No external modules detected', // TODO translate
  RENAME: 'Rename', // TODO translate 
  ENABLE_MODBUS: 'Enable Modbus', // TODO translate
  VIEW_LOG: 'View log to diagnose issues', // TODO translate
  UPLOAD_DRAG: 'drag and drop a file here or click to select one', // TODO translate
  SERVICES: 'Services', // TODO translate
  ALLVALUES: 'All Values', // TODO translate
  SPECIAL_FUNCTIONS: 'Special Functions', // TODO translate
  WAIT_FIRMWARE: 'Firmware is uploading and installing', // TODO translate
  INSTALL_VERSION: 'This will install version {0}. Are you sure?', // TODO translate
  UPGRADE_AVAILABLE: 'There is a firmware upgrade available!', // TODO translate
  LATEST_VERSION: 'You are using the latest firmware version.', // TODO translate
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
  INTERNET_CONNECTION_REQUIRED: 'Internet connection required for automatic version checking and upgrading', // TODO translate
};

export default tr;
