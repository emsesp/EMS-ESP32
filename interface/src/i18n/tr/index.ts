import type { Translation } from '../i18n-types';

const tr: Translation = {
  LANGUAGE: 'Dil',
  RETRY: 'Tekrar dene', // DeepL
  IS_REQUIRED: '{0} zorunludur', // DeepL
  SIGN_IN: 'Giriş Yap',
  SIGN_IN_SUBTITLE: 'EMS ısıtma sistemleri için ağ geçidi',
  SIGN_OUT: 'Çıkış', // DeepL
  USERNAME: 'Kullanıcı adı', // DeepL
  PASSWORD: 'Şifre',
  APPLICATION_SETTINGS: 'Uygulama Ayarları',
  AP_SETTINGS: 'Erişim Noktası Ayarları',
  NETWORK_SETTINGS: 'Ağ Ayarları',
  NTP_SETTINGS: 'NTP Ayarları',
  MQTT_SETTINGS: 'MQTT Ayarları',
  EMS_BUS_SETTINGS: 'EMS Hattı Ayarları',
  HELP: 'Yardım',
  LOGGED_IN: '{name} olarak oturum açıldı', // DeepL
  PLEASE_SIGNIN: 'Devam etmek için lütfen oturum açın', // DeepL
  UPLOAD_SUCCESSFUL: 'Yükleme başarıyla tamamlandı', // DeepL
  DOWNLOAD_SUCCESSFUL: 'İndirme işlemi başarılı', // DeepL
  INVALID_LOGIN: 'Yanlış kullanıcı adı veya şifre', // DeepL
  NETWORK: 'Ağ',
  SECURITY: 'Güvenlik',
  ONOFF_CAP: 'AÇIK/KAPALI',
  ONOFF: 'açık/kapalı',
  TYPE: 'Tür',
  ENTITIES: 'varlıklar',
  EXPORT: 'Dışa aktar', // DeepL
  FAVORITES: "Favoriler",
  CLIENT_ID: 'İstemci Kimliği',
  DEVICE_ID: 'Cihaz Kimliği',
  PRODUCT_ID: 'Ürün Kimliği',
  ENTITY_ID: 'Varlık Kimliği',
  SENSOR_ID: 'Sensör Kimliği',
  TYPE_ID: 'Tür Kimliği',
  DEVICE: 'Cihaz',
  BRAND: 'Marka',
  VERSION: 'Sürüm',
  ENTITY_NAME: 'Valık Adı',
  VALUE: 'Değer',
  DEVICES: 'Cihazlar',
  SENSORS: 'Sensörler',
  AUTOMATION: 'Otomasyon',
  CONNECTIVITY: 'Bağlantı',
  MAINTENANCE: 'Bakım',
  CHANGE_VALUE: 'Değeri değiştir',
  CANCEL: 'İptal',
  REMOVE_ALL: 'Tümünü Kaldır', // DeepL
  APPLY_CHANGES: 'Değişiklikleri Uygula ({0})',
  UPDATE: 'Güncelleme', // DeepL
  REMOVE: 'Kaldır',
  RESET: 'Sıfırla',
  PROBLEM_UPDATING: 'Güncelleme sorunu', // DeepL
  PROBLEM_LOADING: 'Yükleme sorunu', // DeepL
  ANALOG_SENSORS: 'Analog Sensörler',
  SETTINGS: 'Ayarlar',
  SETTINGS_UPDATED: 'Ayarlar güncellendi',
  NAME_UPDATED: 'Ad güncellendi',
  SENSOR_UPDATED: 'Sensör güncellendi',
  ANALOG_SENSOR_UPDATED: 'Analog sensör güncellendi',
  OFFSET: 'Ofset', // DeepL
  FACTOR: 'Faktör', // DeepL
  FREQ: 'Sıklık', // DeepL
  DUTY_CYCLE: 'Çalışma döngüsü', // DeepL
  UNIT: 'UoM', // DeepL
  STARTVALUE: 'Başlangıç değeri',
  EDIT: 'Düzenle', // DeepL
  TEMP_SENSORS: 'Sıcaklık Sensörleri',
  WRITE_CMD_SENT: 'Komut gönderildi', // DeepL
  EMS_BUS_WARNING: 'EMS veriyolu bağlantısı kesildi. Bu uyarı birkaç saniye sonra hala devam ediyorsa lütfen ayarları ve kart profilini kontrol edin', // DeepL
  CONNECTED: 'Bağlı', // DeepL
  TX_ISSUES: 'Tx sorunları - Tx modunu kontrol edin', // DeepL
  DISCONNECTED: 'Bağlantı kesildi',
  DATA_TRAFFIC: 'Veri Trafiği', // DeepL
  EMS_DEVICE: 'EMS cihazı', // DeepL
  SUCCESS: 'BAŞARILI',
  FAIL: 'YANLIŞ', // DeepL
  QUALITY: 'KALİTE',
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
  BOARD_PROFILE: 'Kart profili', // DeepL
  CUSTOM: 'Özel',
  GPIO_OF: '{0} GPIO',
  BUTTON: 'Düğme',
  TEMPERATURE: 'Sıcaklık',
  PHY_TYPE: 'Eth PHY türü', // DeepL
  DISABLED: 'devre dışı',
  TX_MODE: 'EMS Tx modu', // DeepL
  EMS_BUS: 'EMS Hattı',
  EMS_BUS_ID: 'EMS Hattı Kimliği',
  HARDWARE: 'Donanım',
  LANGUAGE_ENTITIES: 'Dil (cihaz varlıkları için)',
  HIDE_LED: 'LED\'i gizle', // DeepL
  ENABLE_TELNET: 'Telnet konsolunu etkinleştirin', // DeepL
  ENABLE_ANALOG: 'Analog sensörleri etkinleştirin', // DeepL
  CONVERT_FAHRENHEIT: 'Sıcaklık değerlerini Fahrenheit\'e dönüştürün', // DeepL
  READONLY: 'Salt okunur modunu etkinleştirin (tüm giden EMS Tx yazma komutlarını engeller)', // DeepL
  WRITE_PROTECTION: 'EMS cihazlarını tekrarlanan ve sık yazma işlemlerinden koruyun', // DeepL
  WRITE_LIMIT: 'Dakikada maksimum yazma sayısı (0 = sınır yok)', // DeepL
  WRITE_LIMIT_REFUSE: 'Sınırı aşan yazılar reddedilir (aksi takdirde sadece bir uyarı kaydedilir)', // DeepL
  UNDERCLOCK_CPU: 'CPU hızını düşürün', // DeepL
  REMOTE_TIMEOUT: 'Uzaktan erişim zaman aşımı', // DeepL
  REMOTE_TIMEOUT_EN: 'Oda sıcaklığı eksik olduğunda uzaktan kumandayı devre dışı bırakın', // DeepL
  HEATINGOFF: 'Zorunlu ısıtma kapalıyken kazanı çalıştır', // DeepL
  MIN_DURATION: 'Bekleme süresi', // DeepL
  ENABLE_SHOWER_TIMER: 'Duş zamanlayıcısını etkinleştir', // DeepL
  ENABLE_SHOWER_ALERT: 'Duş uyarısını etkinleştir', // DeepL
  TRIGGER_TIME: 'Tetikleme zamanı', // DeepL
  COLD_SHOT_DURATION: 'Soğuk atış süresi', // DeepL
  FORMATTING_OPTIONS: 'Biçimlendirme Seçenekleri', // DeepL
  BOOLEAN_FORMAT_DASHBOARD: 'Boole biçimli web', // DeepL
  BOOLEAN_FORMAT_API: 'Boole biçimi API/MQTT', // DeepL
  ENUM_FORMAT: 'Enum formatı API/MQTT', // DeepL
  INDEX: 'Dizin', // DeepL
  ENABLE_PARASITE: '1-Wire parazit gücünü etkinleştirin', // DeepL
  LOG_HEX: 'EMS telgraflarını onaltılık olarak kaydedin', // DeepL
  ENABLE_SYSLOG: 'Syslog\'u etkinleştirin', // DeepL
  LOG_LEVEL: 'Günlük seviyesi', // DeepL
  MARK_INTERVAL: 'Aralık işareti', // DeepL
  SECONDS: 'saniye', // DeepL
  MINUTES: 'dakika', // DeepL
  HOURS: 'saat', // DeepL
  RESTART: 'Yeniden başlat', // DeepL
  RESTART_TEXT: 'Değiştirilen sistem ayarlarının uygulanması için EMS-ESP\'nin yeniden başlatılması gerekir', // DeepL
  RESTART_CONFIRM: 'EMS-ESP\'yi yeniden başlatmak istediğinizden emin misiniz?', // DeepL
  COMMAND: 'Komutlar', // DeepL
  CUSTOMIZATIONS_RESTART: 'Tüm özelleştirmeler kaldırıldı. Yeniden başlatılıyor…', // DeepL
  CUSTOMIZATIONS_FULL: 'Seçilen varlıkların sayısı sınırı aştı. Lütfen gruplar halinde kaydedin', // DeepL
  CUSTOMIZATIONS_SAVED: 'Özelleştirmeler güncellendi', // DeepL
  CUSTOMIZATIONS_HELP_1: 'Bir cihaz seçin ve varlıklar için seçenekleri özelleştirin', // DeepL
  CUSTOMIZATIONS_HELP_2: 'Favorilere ekle', // DeepL
  CUSTOMIZATIONS_HELP_3: 'yazma işlemini devre dışı bırak',
  CUSTOMIZATIONS_HELP_4: 'MQTT ve API\'dan hariç tutun', // DeepL
  CUSTOMIZATIONS_HELP_5: 'Cihazlardan gizle',
  CUSTOMIZATIONS_HELP_6: 'bellekten sil', // DeepL
  SELECT_DEVICE: 'Bir cihaz seçin', // DeepL
  MODIFIED: 'Değiştirildi', // DeepL
  RESET_SELECTED: 'Seçileni sıfırla', // DeepL
  OPTIONS: 'Seçenekler',
  NAME: 'Ad', // DeepL
  CUSTOMIZATIONS_RESET: 'Tüm özelleştirmeleri silmek istediğinizden emin misiniz?', // DeepL
  SYSTEM_INFORMATION: 'Sistem Bilgileri',
  HELP_INFORMATION_1: 'EMS-ESP\'yi nasıl yapılandıracağınızla ilgili talimatlar için çevrimiçi wiki sayfasını ziyaret edin', // DeepL
  HELP_INFORMATION_2: 'Canlı topluluk sohbetine katılmak için Discord sunucumuza katılın', // DeepL
  HELP_INFORMATION_3: 'Bir özellik talebinde bulunmak veya bir hata bildirmek için', // DeepL
  HELP_INFORMATION_4: 'Daha hızlı yanıt alabilmek için bir sorun bildirirken sistem bilgilerinizi ekleyin', // DeepL
  UPLOAD: 'Yükle', // DeepL
  DOWNLOAD: 'İndirme',
  INSTALL: 'Yükle', // DeepL
  REINSTALL: 'Yeniden yükle', // DeepL
  REINSTALL_CONFIRM: '{0} sürümünü yeniden yüklemek istediğinizden emin misiniz?', // DeepL
  FAILED: 'başarısız',
  SYSTEM: 'Sistem',
  SYSTEM_LOG: 'Sistem Günlüğü',
  STATUS: 'Durum',
  DOWNLOAD_UPLOAD: 'İndir/Yükle', // DeepL
  CLOSE: 'Kapat',
  COMMAND_GUEST: 'Konuklar bu komutu çalıştırabilir', // DeepL
  TOKENS_RESET: 'Bu güncelleme, erişim jetonlarını yeni bir cihaz anahtarıyla imzalar; bu nedenle mevcut API erişim jetonları artık çalışmaz. Home Assistant gibi API\'yi kullanan her entegrasyon için bir API anahtarı oluşturun ve eski jeton yerine bunu kullanın.', // DeepL
  API_KEYS: 'API Anahtarları',
  API_KEYS_TEXT: 'API anahtarları, Home Assistant gibi entegrasyonların ve komut dosyalarının API\'yi kullanmasını sağlar. Anahtarı "Authorization: Bearer <ANAHTAR>" başlığında gönderin. Okuma anahtarları, değerleri okuyabilir ve misafirlerin çalıştırabileceği komutları çalıştırabilir; Okuma ve Yazma anahtarları ise bir yöneticinin API aracılığıyla yapabileceği her şeyi yapabilir.', // DeepL
  API_KEY_ACCESS: 'Erişim',
  API_KEY_READ: 'Oku', // DeepL
  API_KEY_READ_WRITE: 'Okuma ve Yazma', // DeepL
  API_KEY_SHOW_ONCE: 'Bu anahtarı şimdi kopyalayın ve güvenli bir yerde saklayın. Bir daha gösterilmeyecektir.', // DeepL
  CREATED: 'Oluşturuldu',
  COPY: 'Metin', // DeepL
  COPIED: 'Kopyalandı',
  USE: 'Kullanım', // DeepL
  FACTORY_RESET: 'Fabrika Ayarlarına Sıfırla', // DeepL
  SYSTEM_FACTORY_TEXT_DIALOG: 'EMS-ESP\'yi fabrika ayarlarına sıfırlamak istediğinizden emin misiniz?', // DeepL
  STABLE: 'Kararlı',
  DEVELOPMENT: 'Geliştirme',
  UPTIME: 'Sistem çalışma süresi',
  LAST_RESTART: 'Son yeniden başlatma',
  POWER_AND_RESET: 'Güç ve Sıfırlama', // DeepL
  FREE_MEMORY: 'Boş Bellek', // DeepL
  BUFFER_SIZE: 'Maksimum tampon boyutu', // DeepL
  COMPACT: 'Özet', // DeepL
  SUMMARY: 'Özet',
  SHOW_MORE: '{n} tane daha göster',
  SHOW_LESS: 'Daha az göster',
  COREDUMP: 'Hata Dökümü', // DeepL
  DOWNLOAD_SYSTEM_BACKUP: 'Sistem Yedeklemesi', // DeepL
  ERROR: 'Beklenmedik bir hata oluştu, lütfen tekrar deneyin', // DeepL
  TIME_SET: 'Zaman ayarı',
  MANAGE_USERS: 'Kullanıcıları Yönet', // DeepL
  USER_WARNING: 'En az bir yönetici kullanıcısı yapılandırılmış olmalıdır', // DeepL
  ADD: 'Ekle',
  TELNET_ENABLED: 'Telnet konsolu etkindir. Telnet şifrelenmez; bu nedenle kullanmıyorsanız Uygulama Ayarları\'ndan devre dışı bırakın.', // DeepL
  TOO_MANY_LOGINS: 'Çok fazla başarısız oturum açma denemesi yapıldı, birkaç dakika sonra tekrar deneyin', // DeepL
  PASSWORD_KEEP: 'Mevcut şifreyi korumak için boş bırakın',
  SETUP_TITLE: 'Şifrelerinizi belirleyin',
  SETUP_TEXT: 'Bu cihaz hâlâ fabrika şifrelerini kullanmaktadır. Devam etmeden önce kendi şifrenizi belirleyin.', // DeepL
  SETUP_ADMIN_PASSWORD: '{name} için yeni şifre',
  SETUP_AP_PASSWORD: 'Yeni erişim noktası şifresi',
  SETUP_AP_REQUIRED: 'Henüz bir Wi-Fi ağı kurulmadığından, erişim noktası giriş yoludur. 8-63 karakterden oluşur ve erişim noktası bir sonraki başlatıldığında kullanılır.', // DeepL
  SETUP_AP_OPTIONAL: 'Önerilen: 8-63 karakter; erişim noktası bir sonraki başlatıldığında kullanılır. Mevcut değeri korumak için boş bırakın.', // DeepL
  SETUP_AP_DEFAULT: 'Erişim noktası hâlâ fabrika şifresini kullanıyor. Erişim Noktası ayarlarından kendi şifrenizi belirleyin.',
  SETUP_VIEW_USER: 'İsteğe bağlı: Yalnızca görüntüleme yetkisine sahip bir misafir kullanıcısı ekleyin.', // DeepL
  NETWORK_CORS_ORIGIN_HELP: 'Tarayıcıdan API\'yi çağırmak için izin verilen web adresi, örneğin http://homeassistant.local:8123', // DeepL
  NETWORK_CORS_WILDCARD: '* ile ağınızdaki bir tarayıcıda açık olan herhangi bir web sitesi API\'yi çağırabilir. Bunun yerine tam adresi girin.', // DeepL
  NETWORK_ALLOWED_HOSTS: 'İzin verilen ana bilgisayar adları',
  NETWORK_ALLOWED_HOSTS_HELP: 'Bu cihaz, tarayıcıda EMS-ESP.fritz.box veya bir ters proxy adı gibi farklı adlarla da açılabilir. IP adresi, ana bilgisayar adı ve hostname.local her zaman çalışır.', // DeepL
  NOT_ENABLED: 'Etkin değil', // DeepL
  MQTT_ERRORS: 'MQTT hataları',
  DISCONNECT_REASON: 'Bağlantı kesilme nedeni', // DeepL
  MQTT_BASE_TOPIC: 'Temel konu',
  LAST_DISCONNECT: 'Son bağlantı kesme', // DeepL
  CONNECTED_SINCE: 'Bağlantı tarihi:', // DeepL
  ENABLE_MQTT: 'MQTT\'yi etkinleştirin', // DeepL
  BROKER: 'Broker', // DeepL
  BASE_TOPIC: 'Temel', // DeepL
  OPTIONAL: 'isteğe bağlı', // DeepL
  MQTT_FORMAT: 'Konu/içerik biçimi', // DeepL
  MQTT_NEST_1: 'Tek bir konu altında yer alan', // DeepL
  MQTT_NEST_2: 'Tek tek konular olarak', // DeepL
  MQTT_RESPONSE: 'Komut çıktısını bir `response` konusuna yayınlayın', // DeepL
  MQTT_PUBLISH_TEXT_1: 'Değişiklik olduğunda tek değerli konuları yayınlayın', // DeepL
  MQTT_PUBLISH_TEXT_2: 'Komut konularına (ioBroker) yayınlayın', // DeepL
  MQTT_PUBLISH_TEXT_3: 'MQTT keşfini etkinleştirin', // DeepL
  MQTT_PUBLISH_TEXT_4: 'Keşif konularının öneki', // DeepL
  MQTT_PUBLISH_TEXT_5: 'Keşif türü', // DeepL
  MQTT_INPUT_NUMBER_FORMAT: 'Sayı girişi biçimi', // DeepL
  MQTT_PUBLISH_INTERVALS: 'Yayın Aralıkları', // DeepL
  MQTT_INT_BOILER: 'Kazanlar ve Isı Pompaları',
  MQTT_INT_THERMOSTATS: 'Termostatlar',
  MQTT_INT_SOLAR: 'Güneş Modülleri', // DeepL
  MQTT_INT_MIXER: 'Karıştırıcı Modüller', // DeepL
  MQTT_INT_WATER: 'Su Modülleri',
  MQTT_QUEUE: 'MQTT kuyruğu', // DeepL
  DEFAULT: 'Varsayılan',
  MQTT_ENTITY_FORMAT: 'Varlık Kimliği biçimi', // DeepL
  MQTT_ENTITY_FORMAT_0: 'Tek örnek, uzun ad (v3.4)', // DeepL
  MQTT_ENTITY_FORMAT_1: 'Tek örnek, kısa ad', // DeepL
  MQTT_ENTITY_FORMAT_2: 'Birden fazla örnek, kısa ad', // DeepL
  MQTT_CLEAN_SESSION: 'Temiz oturum oluştur', // DeepL
  MQTT_RETAIN_FLAG: 'Her zaman saklama bayrağını ayarlayın', // DeepL
  MQTT_COMMANDS: 'MQTT üzerinden alınan komutlar',
  MQTT_COMMANDS_0: 'Kapalı, sadece değerleri okur', // DeepL
  MQTT_COMMANDS_1: 'Değerleri değiştirin ve komutları çalıştırın', // DeepL
  MQTT_COMMANDS_2: 'Her şey, sistem komutları da dahil', // DeepL
  INACTIVE: 'Etkin değil', // DeepL
  ACTIVE: 'Etkin', // DeepL
  UNKNOWN: 'Bilinmiyor', // DeepL
  SET_TIME: 'Zamanı Ayarla',
  SET_TIME_TEXT: 'Zamanı ayarlamak için aşağıya yerel tarih ve saati girin', // DeepL
  LOCAL_TIME: 'Yerel saat', // DeepL
  UTC_TIME: 'UTC saati',
  ENABLE_NTP: 'NTP\'yi etkinleştirin', // DeepL
  NTP_SERVER: 'NTP sunucusu', // DeepL
  TIME_ZONE: 'Saat dilimi',
  ACCESS_POINT: 'Erişim Noktası',
  AP_PROVIDE: 'Erişim Noktasını Etkinleştir', // DeepL
  AP_PROVIDE_TEXT_2: 'Ağ bağlantısı bulunamadığında',
  AP_PROVIDE_TEXT_3: 'Asla', // DeepL
  AP_PREFERRED_CHANNEL: 'Tercih edilen kanal',
  AP_HIDE_SSID: 'SSID\'yi gizle', // DeepL
  AP_CLIENTS: 'AP istemcileri', // DeepL
  AP_MAX_CLIENTS: 'Maksimum istemci sayısı', // DeepL
  AP_LOCAL_IP: 'Yerel IP',
  IDLE: 'Bekleme', // DeepL
  SCAN_AGAIN: 'Tekrar tarayın', // DeepL
  NETWORK_NO_WIFI: 'Wi-Fi ağı bulunamadı', // DeepL
  NETWORK_BLANK_BSSID: 'Yalnızca SSID\'yi kullanmak için boş bırakın', // DeepL
  TX_POWER: 'Gönderim gücü', // DeepL
  HOSTNAME: 'Ana bilgisayar adı', // DeepL
  NETWORK_DISABLE_SLEEP: 'Wi-Fi uyku modunu devre dışı bırakın', // DeepL
  NETWORK_LOW_BAND: 'Daha düşük Wi-Fi bant genişliği kullanın', // DeepL
  NETWORK_USE_DNS: 'mDNS hizmetini etkinleştirin', // DeepL
  NETWORK_ENABLE_CORS: 'CORS\'u etkinleştirin', // DeepL
  NETWORK_CORS_ORIGIN: 'CORS kaynağı',
  NETWORK_FIXED_IP: 'Sabit IP adresi kullanın', // DeepL
  NETWORK_GATEWAY: 'Ağ Geçidi', // DeepL
  NETWORK_SUBNET: 'Alt ağ maskesi', // DeepL
  NETWORK_DNS: 'DNS Sunucuları',
  ADDRESS_OF: '{0} adresi', // DeepL
  ADMINISTRATOR: 'Yönetici',
  GUEST: 'Misafir',
  NEW: 'Yeni',
  NEW_ENTITY_NAME: 'Yeni varlık adı',
  MIN: 'min',
  MAX: 'max', // DeepL
  BLOCK_NAVIGATE_1: 'Kaydedilmemiş değişiklikleriniz var', // DeepL
  BLOCK_NAVIGATE_2: 'Başka bir sayfaya geçerseniz, kaydedilmemiş değişiklikleriniz kaybolacaktır. Bu sayfadan ayrılmak istediğinizden emin misiniz?', // DeepL
  UPLOAD_ABORTED: 'Yükleme iptal edildi',
  BLOCK_BUSY_1: 'Halen devam ediyor', // DeepL
  BLOCK_BUSY_2: 'Bu sayfadan şimdi çıkarsanız işlem durdurulur. Çıkmak istediğinizden emin misiniz?', // DeepL
  DISCARD: 'Sil', // DeepL
  COMMANDS_UPDATED: 'Komutlar güncellendi',
  V_REQUIRED: 'Zorunlu', // DeepL
  V_BETWEEN: '{min} ile {max} arasında olmalıdır', // DeepL
  V_MAX_CHARS: '{max} karakter veya daha az olmalıdır', // DeepL
  V_CHARS: '{min}–{max} karakter arasında olmalıdır', // DeepL
  V_IN_USE: 'Halihazırda kullanımda', // DeepL
  V_IP: 'Bir IP adresi olmalıdır', // DeepL
  V_HOSTNAME: 'Geçerli bir ana bilgisayar adı olmalıdır', // DeepL
  V_IP_OR_HOSTNAME: 'Bir IP adresi veya ana bilgisayar adı olmalıdır', // DeepL
  V_NAME: '{max} karakterden fazla olmamalıdır: harfler, rakamlar veya _', // DeepL
  V_USERNAME: '{max} karakterden fazla olmamalıdır: harfler, rakamlar, _ veya .', // DeepL
  V_DAY: 'En az bir gün seçin',
  V_HEX: 'Onaltılık sayı olmalıdır', // DeepL
  V_FINGERPRINT: 'SHA-1 parmak izi (40 onaltılık basamak) olmalıdır', // DeepL
  V_EXAMPLE: '{example} gibi görünmelidir', // DeepL
  V_NOT_DEFAULT: 'Farklı bir şifre seçin',
  ZERO_DISABLES: '0, cihazı kapatır', // DeepL
  MINUS_ONE_DISABLES: '-1, cihazı kapatır', // DeepL
  LED_TYPE: 'LED türü',
  PHY_POWER: 'PHY gücü', // DeepL
  PHY_CLOCK: 'PHY saati',
  AUTO: 'Otomatik',
  PORT: 'Port',
  TIMEOUT: 'Zaman aşımı',
  HOST: 'Ana Bilgisayar', // DeepL
  SMTP_SERVER: 'SMTP sunucusu',
  KEEP_ALIVE: 'Bağlantıyı canlı tutun', // DeepL
  MQTT_REFRESH: 'MQTT\'yi yenile',
  MQTT_REFRESHED: 'MQTT güncellendi', // DeepL
  MQTT_R_USER: 'Kullanıcı tarafından bağlantısı kesildi', // DeepL
  MQTT_R_TCP: 'TCP bağlantısı kapatıldı', // DeepL
  MQTT_R_PROTOCOL: 'Protokol sürümü kabul edilmedi',
  MQTT_R_ID: 'Müşteri Kimliği reddedildi', // DeepL
  MQTT_R_SERVER: 'Sunucu kullanılamıyor',
  MQTT_R_CREDENTIALS: 'Yanlış biçimlendirilmiş kimlik bilgileri', // DeepL
  MQTT_R_AUTH: 'Yetki yok', // DeepL
  MQTT_R_FINGERPRINT: 'TLS parmak izi eşleşmiyor',
  GPIO_INVALID: 'Geçersiz GPIO ayarları',
  CONNECTED_VIA: '{0} üzerinden bağlanıyor', // DeepL
  NETWORK_NOT_FOUND: 'Ağ bulunamadı',
  CONNECTION_FAILED: 'Bağlantı başarısız',
  CONNECTION_LOST: 'Bağlantı kesildi', // DeepL
  AP_LINGERING: 'Boşta kalana kadar bekleme', // DeepL
  NONE: 'Yok',
  LOG_NO_CONNECTION: 'Günlük hizmetiyle bağlantı yok', // DeepL
  CHECK_SYSTEM_LOG: 'Olası nedenleri belirlemek için sistem günlüğünü kontrol edin', // DeepL
  VALUE_TYPE: 'Değer türü',
  DEFAULT_VALUE: 'Varsayılan değer',
  DEFAULT_NAME: 'Varsayılan ad',
  ALWAYS_OFF: 'Her zaman kapalı',
  ALWAYS_ON: 'Her zaman açık',
  NO_DATA: 'Henüz Favorilere eklenmiş varlık yok. Bunları {customizations} içinde işaretleyin veya {devices} içindeki tüm varlıkları görüntüleyin.', // DeepL
  STAY: 'Devam', // DeepL
  LEAVE: 'Bırakın', // DeepL
  MENU: 'Menü',
  MORE: 'Daha fazla',
  SHOW_PASSWORD: 'Şifreyi göster',
  STATUS_RUNNING: 'EMS-ESP çalışıyor',
  NUM_ERRORS: '{num} hata',
  NUM_WARNINGS: '{num} uyarı',
  HEAP: 'Yığın', // DeepL
  METER_USED: '{used} / {total} {unit} kullanıldı', // DeepL
  MAX_BLOCK: '{num} KB\'lik maksimum blok', // DeepL
  MAX_BLOCK_HELP: 'Maksimum blok, tek seferde ayrılabilen en büyük yığın parçasıdır. 40 KB\'nin altında kalınmazsa web arayüzü ve MQTT çalışmayabilir.', // DeepL
  FLASH_CHIP: 'Flash yongası', // DeepL
  FILE_SYSTEM: 'Dosya sistemi',
  DETAILS: 'Ayrıntılar',
  RECONNECTS: 'Yeniden bağlanır', // DeepL
  CONNECTS: 'Bağlantılar',
  ACTIVITY_HELP: 'Birkaç Rx hatası normaldir. Çok sayıda Tx hatası genellikle kazanınız için yanlış Tx modunun kullanıldığı anlamına gelir.', // DeepL
  PAUSE: 'Duraklat',
  RESUME: 'Özet', // DeepL
  LOG_PAUSED: 'Duraklatıldı, yeni satırlar ertelendi: {num}', // DeepL
  FILTER_LINES: 'Satırları filtrele',
  SCHEDULER: 'Zamanlayıcı',
  SCHEDULER_HELP_1: 'Aşağıya zamanlanmış olaylar ekleyerek komutları otomatikleştirin', // DeepL
  SCHEDULER_HELP_2: 'başlangıçta',
  SCHEDULE: 'Zamanlama',
  TIME: 'Zaman',
  TIMER: 'Zamanlayıcı',
  ONCHANGE: 'Değişiklik durumunda', // DeepL
  CONDITION: 'Koşul', // DeepL
  SCHEDULE_UPDATED: 'Zaman çizelgesi güncellendi', // DeepL
  CUSTOM_ENTITIES: 'Özel Varlıklar',
  ENTITIES_HELP_1: 'Özel EMS varlıklarını veya dinamik kullanıcı değişkenlerini tanımlayın', // DeepL
  ENTITIES_UPDATED: 'Güncellenen varlıklar', // DeepL
  WRITEABLE: 'Yazılabilir',
  SHOWING: 'Gösteriliyor', // DeepL
  SEARCH: 'Arama', // DeepL
  CERT: 'CA sertifikası (PEM)',
  ENABLE_TLS: 'TLS\'yi etkinleştirin', // DeepL
  TLS_CHECK: 'Aracıyı şu adresle kontrol edin:', // DeepL
  TLS_CHECK_CA: 'CA sertifikası',
  TLS_CHECK_FINGERPRINT: 'Sertifika parmak izi (SHA-1)',
  TLS_CHECK_NONE: 'Yok, işaretlenmemiş', // DeepL
  TLS_NOT_CHECKED: 'Bağlantı şifrelenmiştir, ancak aracı kontrol edilmez; bu nedenle ağdaki başka bir cihaz, aracı gibi davranabilir.', // DeepL
  ON: 'On',
  OFF: 'Kapalı', // DeepL
  POLARITY: 'Polarite', // DeepL
  ACTIVEHIGH: 'Aktif yüksek', // DeepL
  ACTIVELOW: 'Aktif düşük', // DeepL
  UNCHANGED: 'Değiştirilmedi',
  SECURITY_1: 'Kullanıcı ekleyin veya kaldırın', // DeepL
  DOWNLOAD_UPLOAD_1: 'Ayarları indirme ve yükleme', // DeepL
  MODULES: 'Modül',
  MODULES_1: 'Harici modülleri etkinleştirin veya devre dışı bırakın',
  NETWORK_DESC: 'Wi-Fi, Ethernet ve ana bilgisayar adı',
  AP_DESC: 'Kurulum ve yedek erişim için dahili erişim noktası',
  MQTT_DESC: 'Broker, konular ve keşif',
  APPLICATION_DESC: 'EMS hattı, kart, sensörler ve biçimlendirme',
  NTP_DESC: 'Zaman sunucusu ve saat dilimi',
  FIRMWARE_DESC: 'Sürüm, güncellemeler ve kurtarma',
  MODULES_UPDATED: 'Modüller güncellendi', // DeepL
  MODULES_NONE: 'Harici modül algılanmadı', // DeepL
  RENAME: 'Yeniden adlandırın', // DeepL
  ENABLE_MODBUS: 'Modbus\'u etkinleştirin', // DeepL
  VIEW_LOG: 'Sorunları teşhis etmek için günlüğü görüntüleyin', // DeepL
  SERVICES: 'Hizmetler',
  ALLVALUES: 'Tüm değerler', // DeepL
  WAIT_FIRMWARE: 'Yazılım yükleniyor ve kuruluyor', // DeepL
  INSTALL_VERSION: 'Bu işlem {0} sürümünü yükleyecek',
  UPDATE_AVAILABLE: 'güncelleme mevcut', // DeepL
  LATEST_VERSION: 'En son aygıt yazılımı sürümünü kullanıyorsunuz ({0})',
  PLEASE_WAIT: 'Lütfen bekleyin',
  RESTARTING_PRE: 'Başlatma', // DeepL
  RESTARTING_POST: 'Hazırlık', // DeepL
  AUTO_SCROLL: 'Otomatik Kaydırma', // DeepL
  DASHBOARD: 'Kontrol Paneli',
  DEVELOPER_MODE: 'Geliştirici modu', // DeepL
  DISABLE_RESET: 'Fabrika ayarlarına sıfırlama özelliğini devre dışı bırakın', // DeepL
  AUTO_FW_CHECK: 'Firmware güncellemelerini otomatik olarak kontrol edin', // DeepL
  BYTES: 'Bayt',
  BITMASK: 'Bit maskesi', // DeepL
  DUPLICATE: 'Yinelenen', // DeepL
  DASHBOARD_1: 'Favoriler olarak işaretlenmiş tüm aktif EMS varlıkları aşağıda gösterilir; buna ek olarak tüm özel varlıklar, zamanlamalar ve harici Sensörler de listelenir.', // DeepL
  NO_GPIO: 'Kullanılabilir GPIO bulunamadı',
  PLATFORM: 'Platform', // DeepL
  RELEASE_TYPE: 'Sürüm Türü', // DeepL
  FIRMWARE_VERSION_INFO: 'Firmware Sürümü Bilgileri', // DeepL
  USER_PROFILE: 'Kullanıcı Profili',
  ONLINE_HELP: 'çevrimiçi yardım', // DeepL
  UPGRADE_IMPORTANT_MESSAGES: 'Önemli Güncelleme Mesajları', // DeepL
  UPGRADE_IMPORTANT_MESSAGES_2: 'Yeni bir ana sürüme yükseltme yapıyorsunuz. Öncelikle sistem yedeği almanız tavsiye edilir. Ayrıca, önemli değişiklikler olup olmadığını öğrenmek için çevrimiçi yardıma bakın.', // DeepL
  UPGRADE_IMPORTANT_MESSAGES_3: '3.10 sürümünde önemli güvenlik değişiklikleri bulunmaktadır. Öncelikle sistem yedeği alın: yükseltme işleminden sonra şifreler karma olarak saklanır ve eski sürümler bunları okuyamaz; bu nedenle geri dönmek için bu yedeğe ihtiyaç duyulur. Artık her API çağrısı için bir API anahtarı gereklidir; dolayısıyla Home Assistant gibi entegrasyonların güncellenmesi gerekir. Devam etmeden önce çevrimiçi yardım bölümündeki önemli değişiklikleri okuyun.', // DeepL
  WARNING_SYSTEM_BACKUP: 'Bu işlem, sistem yapılandırmanızın ve ayarlarınızın tam bir yedeğini oluşturur. Aşağıda belirtmediğiniz sürece şifreler ve anahtarlar yedeğe dahil edilmez; yedeği geri yüklediğinizde cihazda zaten bulunan şifreler korunur. Komutlar her zaman yedeğe dahil edilir ve kimlik bilgilerini içerebilir, bu nedenle paylaşırken dikkatli olun. Devam etmek istiyor musunuz?', // DeepL
  BACKUP_INCLUDE_SECRETS: 'Şifreleri ve anahtarları (yeni bir cihaza geri yüklemek için gerekli olan) ekleyin.', // DeepL
  TEST_EMAIL_SUCCESSFUL: 'Test e-postası gönderildi', // DeepL
  SYSTEM_NAME: 'Sistem adı', // DeepL
  COMMANDS: 'Komutlar',
  COMMANDS_HELP_1: 'Aşağıda yeniden kullanılabilir, adlandırılmış komutları tanımlayın. Bunlar konsoldan, API/MQTT üzerinden çalıştırılabilir veya Zamanlayıcı tarafından referans alınabilir.', // DeepL
  COMMAND_EXECUTED: 'Komut yürütüldü', // DeepL
  RUN: 'Çalıştır', // DeepL
  INTERNET_CONNECTION_CHECK: 'Çevrimiçi olarak mevcut sürümleri kontrol etme', // DeepL
  UPLOAD_BACKUP_TEXT: 'Seçmek için tıklayın veya bir yedekleme dosyasını (.json) buraya bırakın', // DeepL
  UPLOAD_FIRMWARE_TEXT: 'Seçmek için tıklayın veya bir firmware .bin/.md5 dosyasını buraya bırakın', // DeepL
  UPLOAD_MD5_RECEIVED: 'Özet alındı, şimdi .bin formatındaki donanım yazılımı dosyasını yükleyin', // DeepL
  UPLOAD_MD5_MATCHED: 'MD5 eşleşmeleri, donanım yazılımı yükleniyor…', // DeepL
  UPLOAD_NOT_OFFICIAL: 'Bu, resmi bir EMS-ESP sürümü değildir. Yalnızca güvendiğiniz donanım yazılımını yükleyin', // DeepL
  UPLOAD_NOT_OFFICIAL_HELP: 'Resmi ve özel sürümler hakkında',
  UPLOAD_INSTALL_ANYWAY: 'Yine de yükleyin', // DeepL
  OFFICIAL_BUILD: 'Resmi Sürüm',
  CUSTOM_BUILD: 'Özel Yapılandırma', // DeepL
  USING_CUSTOM_FIRMWARE: 'Özel yapım bir firmware kullanıyorsunuz', // DeepL
  PASSWORD_CONFIRM_TITLE: 'Şifrenizi tekrar girin',
  PASSWORD_CONFIRM_TEXT: 'Güvenliğiniz için, bu değişikliği yapmadan önce şifrenizi girin. 5 dakika boyunca tekrar şifre sorulmayacaktır.', // DeepL
  PASSWORD_CONFIRM_BUTTON: 'Onayla',
  PASSWORD_WRONG: 'Yanlış şifre',
  PASSWORD_NOT_CONFIRMED: 'İşlem iptal edildi, şifre girilmedi', // DeepL
  RECOVERY_APP: 'Kurtarma Uygulaması',
  RECOVERY_NOT_INSTALLED: 'Yüklenmemiş', // DeepL
  RECOVERY_INSTALLED: 'Kurtarma yazılımı yüklendi', // DeepL
  RESTART_INTO_RECOVERY: 'Kurtarma Modunda Yeniden Başlat Uygulaması', // DeepL
  RECOVERY_NOT_FOUND: 'Recovery App çevrimiçi olarak bulunamadı', // DeepL
  RECOVERY_START_FAILED: 'Recovery Uygulaması başlatılamıyor; uygulama güncel olmayabilir veya bozulmuş olabilir. Önce en son sürümü yükleyin', // DeepL
  PARTITION_START_FAILED: 'Bu donanım yazılımı başlatılamıyor, hasar görmüş olabilir', // DeepL
  INSTALLED_VERSIONS: 'Yüklü Sürümler', // DeepL
  EMSESP_APP: 'EMS-ESP Uygulaması',
  NO_UPDATES_AVAILABLE: 'güncelleme yok',
  SWITCH_TO_DEV: 'dev {0}\'e geçin', // DeepL
  SWITCH_TO_STABLE: '{0} numaralı kararlı sürüme geçin', // DeepL
  SAVE: 'Kaydet',
  NEEDS_RESTART: 'yeniden başlatılması gerekiyor', // DeepL
  CHANGED_SETTINGS: '{num} değiştirilmiş ayar',
  SAVED_RESTART: 'Kaydedildi. Yeni ayarları kullanmak için EMS-ESP\'yi yeniden başlatın.',
  RESTART_NOW: 'Şimdi yeniden başlat',
  COPY_FAILED: 'Kopyalanamadı, metni seçip kendiniz kopyalayın',
  DOCUMENTATION: 'Belgeler',
  COMMUNITY_CHAT: 'Topluluk sohbeti',
  REPORT_ISSUE: 'Bir hata bildirin veya bir özellik talebinde bulunun', // DeepL
  GET_HELP: 'Yardım Alın', // DeepL
  REPORT_PROBLEM: 'Bir Sorun Bildirin', // DeepL
  REPORT_STEP_1: 'Sistem bilgilerini indirin',
  REPORT_STEP_1_TEXT: 'sürüm, anakart, EMS veriyolu durumu, bağlı cihazlar ve ayarlar (şifreler hariç)', // DeepL
  REPORT_STEP_2: 'Son günlüğü kaydet', // DeepL
  REPORT_STEP_2_TEXT: 'Sorun bir cihazla ilgiliyse, önce günlük düzeyini DEBUG olarak ayarlayın', // DeepL
  REPORT_STEP_3: 'Bir sorun bildir veya Discord\'da soru sor', // DeepL
  REPORT_STEP_3_TEXT: 'Her iki dosyayı da ekleyin veya aşağıdaki kısa özeti bir sohbet penceresine yapıştırın', // DeepL
  NEW_ISSUE: 'Yeni sorun', // DeepL
  SHORT_SUMMARY: 'Kısa özet',
  LICENSE_TEXT: 'LGPL-3.0 lisansı altında açık kaynak kodlu', // DeepL
  ADMIN_ACCESS_TEXT: 'ayarlar, cihazlar ve API\'ye tam erişim', // DeepL
  GUEST_ACCESS_TEXT: 'değerleri görüntüleyebilir ve misafirlerin çalıştırabileceği komutları çalıştırabilir', // DeepL
  LANGUAGE_BROWSER_ONLY: 'Bu değişiklik yalnızca bu tarayıcıyı etkiler', // DeepL
  LANGUAGE_ENTITIES_HINT: 'MQTT ve API\'daki cihaz varlık adları için dil, şu dosyada ayarlanır:', // DeepL
  ADMINISTRATION: 'Yönetim',
  API_KEYS_SHORT: 'Home Assistant için tuşlar ve komut dosyaları', // DeepL
  ADDRESSES: 'Adresler',
  SEARCH_SETTINGS: 'Arama ayarları', // DeepL
  NO_SETTINGS_MATCH: 'Aramanızla eşleşen ayar bulunamadı', // DeepL
  GENERAL: 'Genel',
  BOARD: 'Kart',
  ADVANCED: 'Gelişmiş',
  EMS_BUS_DESC: 'EMS-ESP, ısıtma sisteminizle nasıl iletişim kurar?', // DeepL
  BOARD_DESC: 'Pimler, kart profilinden alınır. Bunları kendiniz ayarlamak için Geliştirici modunu etkinleştirin ve "Özel" seçeneğini seçin.', // DeepL
  FORMAT_DESC: 'Değerlerin web kullanıcı arayüzünde ve MQTT/API yüklerinde nasıl göründüğü.', // DeepL
  BOARD_PINS: 'Profilden alınan pimler', // DeepL
  ENABLE_EMAIL: 'E-posta bildirimlerini etkinleştirin', // DeepL
  SEND_TEST_EMAIL: 'Test e-postası gönder',
  EMAIL_FROM: 'Kaynak:', // DeepL
  EMAIL_TO: '-', // DeepL
  EMAIL_SUBJECT: 'Konu',
  WIFI_SCAN: 'Wi-Fi Taraması', // DeepL
  SCAN: 'Tara',
  NETWORKS_NEARBY: 'Yakındaki Ağlar', // DeepL
  SIGNAL: 'Sinyal',
  CHANNEL: 'Kanal',
  WIFI_OPEN: 'açık',
  WIFI_SAVED: 'kaydedildi', // DeepL
  NETWORK_ETHERNET_INFO: 'Şu anda Ethernet üzerinden bağlanıyor ({ip}). Wi-Fi yalnızca kablosuz bağlantı için kullanılır.', // DeepL
  HOSTNAME_ACCESS: 'Ana Bilgisayar Adı ve Erişim', // DeepL
  IP_ADDRESS: 'IP Adresi',
  NETWORK_ETH_10MBIT: '10 Mbit yarı çift yönlü zorla', // DeepL
  NETWORK_ETH_10MBIT_HELP: 'sadece sorunlu anahtarlar veya uzun kablolar için', // DeepL
  NETWORK_DISABLE_SLEEP_HELP: 'daha güvenilirdir, biraz daha fazla enerji tüketir', // DeepL
  NETWORK_MDNS_HELP: 'mDNS ile {host}.local şeklinde erişin', // DeepL
  NETWORK_FIXED_IP_HELP: 'aksi takdirde DHCP kullanılır', // DeepL
  NETWORK_CORS_HELP: 'yalnızca başka bir web sayfası API\'yi çağırdığında gereklidir', // DeepL
  NTP_TIME_ON_DEVICE: 'EMS-ESP\'de geçirilen süre', // DeepL
  NTP_TIME_SERVER: 'Zaman Sunucusu', // DeepL
  NTP_THERMOSTAT_CLOCK: 'Termostat Saati', // DeepL
  NTP_THERMOSTAT_SYNC: 'Termostat saatini ayarlayın', // DeepL
  NTP_THERMOSTAT_SYNC_HELP: 'EMS termostatında tarih ve saati ayarlar', // DeepL
  NTP_THERMOSTAT_SYNC_1: 'Açık, EMS-ESP saat dilimi',
  NTP_THERMOSTAT_SYNC_2: 'Açık, kendi saat dilimi',
  NTP_THERMOSTAT_TZ: 'Termostat saat dilimi',
  MQTT_PREVIEW: 'Neler yayınlanır?', // DeepL
  MQTT_PREVIEW_CMD: 'komut olarak kabul edilir',
  MQTT_PREVIEW_RESPONSE: 'komutların çıktısı',
  MQTT_BEHAVIOUR: 'Davranış',
  MQTT_TOPICS: 'Konular ve Yükler', // DeepL
  MQTT_DISCOVERY: 'Keşif',
  MQTT_INTERVALS_HELP: 'Yayınlar arasındaki saniye aralığı, 0 = her değişiklikte yayınla.', // DeepL
  MQTT_SINGLE_NO_DISCOVERY: 'keşif özelliği ile kullanılamaz', // DeepL
  MQTT_DISCOVERY_NO_SINGLE: 'tek değerli konularla kullanılamaz', // DeepL
  MQTT_DISCOVERY_HELP: 'Home Assistant veya Domoticz\'de varlıklar otomatik olarak oluşturulur', // DeepL
  MQTT_ENTITY_FORMAT_WARN: 'Bunu değiştirmek, Home Assistant\'taki tüm varlıkların adını değiştirir', // DeepL
  USERS: 'Kullanıcılar',
  USERS_TEXT: 'Yöneticiler her şeyi değiştirebilir. Misafirler ise sadece görüntüleyebilir ve misafirler için işaretlenmiş komutları çalıştırabilir.', // DeepL
  ADD_USER: 'Kullanıcı ekle',
  EDIT_USER: 'Kullanıcıyı düzenle',
  ROLE: 'Görev', // DeepL
  ROLE_ADMIN: 'admin', // DeepL
  ROLE_GUEST: 'misafir',
  ADMIN: 'Yönetici',
  YOU: 'siz',
  NEW_PASSWORD: 'Yeni şifre', // DeepL
  UNSAVED_CHANGES: '{num} kaydedilmemiş değişiklik',
  CREATE_KEY: 'Anahtar Oluştur', // DeepL
  NEW_API_KEY: 'Yeni API Anahtarı',
  NO_API_KEYS: 'Henüz API anahtarı yok',
  API_KEYS_LIMIT: 'En fazla {num} tuşunuz olabilir.', // DeepL
  REVOKE: 'İptal et',
  REVOKE_KEY: 'API Anahtarını İptal Et',
  REVOKE_KEY_TEXT: '"{name}" ifadesini içeren her şey anında çalışmaz hale gelir.', // DeepL
  MODULES_TEXT: 'Bu donanım yazılımına entegre edilmiş isteğe bağlı özellikler. Bunları buradan etkinleştirebilir veya devre dışı bırakabilirsiniz.', // DeepL
  BY_AUTHOR: '{name} tarafından', // DeepL
  LICENSE_KEY: 'Lisans anahtarı',
  MODULE_ACTIVATED: 'etkinleştirildi',
  MODULE_PENDING: 'etkinleştirilmeyi bekliyor', // DeepL
  ENABLED: 'Etkin',
  UNDO: 'Geri al',
  FW_UPDATES: 'Güncellemeler',
  FW_VERSION_AVAILABLE: '{version} sürümü mevcuttur', // DeepL
  FW_RELEASE_NOTES: 'Sürüm notları',
  FW_UPDATES_OFFLINE: 'Yeni sürümler için çevrimiçi kontrol yapılamadı. EMS-ESP\'nin bunun için internet bağlantısına ihtiyacı vardır.', // DeepL
  FW_RESTART_RESET: 'Yeniden Başlatma ve Sıfırlama',
  FW_RECOVERY_TEXT: 'Kurtarma Uygulamasına yeniden başlatmak ister misiniz? Uygulama, ağınıza bağlanır ve ayrıca kendi Erişim Noktasını başlatır; buradan yazılım güncellemesini yükleyebilirsiniz.', // DeepL
  FW_SIZE: 'Boyut',
  FW_INSTALL_DATE: 'Yüklendiği yer', // DeepL
  FW_UPLOAD_FAILED: 'Yükleme başarısız: yanlış dosya türü veya başka bir kart için yazılmış donanım yazılımı', // DeepL
  FILE_TYPE_INVALID: 'Bu dosya türü desteklenmiyor, bir {types} dosyası seçin',
  WHO_CAN_RUN: 'Kimler çalıştırabilir', // DeepL
  CMD_NONE: 'Komutlar bulunamadı', // DeepL
  CMD_NONE_MATCH: 'Aramanızla eşleşen komut bulunamadı', // DeepL
  SCHEDULE_NONE: 'Zaman çizelgesi bulunamadı', // DeepL
  SCHEDULE_NONE_MATCH: 'Aramanızla eşleşen hiçbir zamanlayıcı bulunamadı', // DeepL
  CE_NONE: 'Hiçbir varlık bulunamadı', // DeepL
  CE_ALL: 'Tümü',
  CE_STORAGE: 'Depolama',
  CE_SOURCE: 'Kaynak',
  CE_EMS_SOURCE: 'cihaz {device} · telgraf {type} · ofset {offset}', // DeepL
  CE_RAM_TEXT: 'yeniden başlatılana kadar korunur', // DeepL
  CE_NVS_TEXT: 'yeniden başlatmalarda korunur', // DeepL
  CE_EXCLUDED: 'API/MQTT\'de yer almaz', // DeepL
  SEARCH_SENSORS: 'Ad, kimlik numarası veya GPIO\'ya göre arama yapın', // DeepL
  NO_SENSORS: 'Sensör bulunamadı',
  NO_SENSORS_MATCH: 'Aramanızla eşleşen sensör bulunamadı', // DeepL
  TEMP_SENSORS_HELP: 'Otomatik olarak bulunur. Bir sensöre tıklayarak adını değiştirebilir veya ofset ayarlayabilirsiniz', // DeepL
  FREE_GPIOS: 'Boş GPIO\'lar',
  SYSTEM_SENSOR: 'Sistem sensörü',
  MASK_FAVORITE: 'Favoriler', // DeepL
  MASK_READONLY: 'Engelli için yazın', // DeepL
  MASK_NO_API: 'MQTT/API yok',
  MASK_HIDDEN: 'Gizli',
  MASK_REMOVED: 'Kaldırıldı',
  FILTER_ALL: 'Tümü',
  NUM_SELECTED: 'Seçili: {num}',
  ALL_SHOWN: 'Tümünü göster: {num}', // DeepL
  BULK_HINT: 'Düğmeler, işaretli satırları değiştirir; hiçbir satır işaretli değilse ise görüntülenen tüm satırları değiştirir.', // DeepL
  CLEAR_SELECTION: 'Seçimi netleştirin', // DeepL
  NO_ENTITIES_MATCH: 'Eşleşen varlık yok',
  LIMIT_HELP: 'yerleşik sınır için boş bırakın',
  MIN_ABOVE_MAX: 'Minimum değer, maksimum değerden fazla olmamalıdır', // DeepL
  RENAME_DEVICE: 'Cihazın Adını Değiştir', // DeepL
  WEEKLY: 'Haftalık',
  EVERY_DAY: 'Her gün',
  SCHEDULER_NO_COMMANDS: 'Önce Komutlar sayfasına bir komut ekleyin', // DeepL
  DU_SETTINGS: 'EMS-ESP Ayarları', // DeepL
  DU_SETTINGS_TEXT: 'EMS-ESP ayarlarının yedeklenmesi: ağ, MQTT, Özelleştirme, Zamanlayıcı, Özel Varlıklar ve Modüller. Bir ayar veya yedekleme dosyasının yüklenmesi, EMS-ESP\'yi yeniden başlatır.', // DeepL
  HB_TITLE: 'Isıtma Ayarları', // DeepL
  HB_TEXT: 'Kazanın, termostatın ve diğer EMS cihazlarının mevcut değerlerini kaydeder; böylece, örneğin bir cihazı değiştirdikten sonra bu değerleri daha sonra geri yükleyebilirsiniz. Salt okunur varlıklar ve komutlar, geri yükleme işlemiyle asla değiştirilmez.', // DeepL
  HB_BACKUP: 'Isıtma Ayarlarını Yedekle', // DeepL
  HB_RESTORE: 'Isıtma Ayarlarını Geri Yükle', // DeepL
  DATA_EXPORT: 'Veri Dışa Aktarma', // DeepL
  DATA_EXPORT_TEXT: 'Destek ve analiz amaçlıdır. Bu dosyalar geri yüklenemez.', // DeepL
  NUM_VALUES: '{num} değer',
  HB_NOTHING: 'destekleyecek bir şey yok', // DeepL
  FILE_NAME: 'Dosya adı',
  HB_NOTE: 'Not (isteğe bağlı, geri yüklediğinizde tekrar gösterilir)', // DeepL
  HB_INCOMPLETE: 'Henüz veriyolundan okunmamış değerler gösterilmez. Tam bir yedekleme için yeniden başlatmanın ardından birkaç dakika bekleyin.', // DeepL
  HB_RESTORE_TEXT: 'Bir ısıtma ayarları yedeği seçin. Değişiklikleri kontrol edip onaylayana kadar hiçbir şey kaydedilmez.', // DeepL
  HB_DROP: 'Seçmek için tıklayın veya bir ısıtma yedekleme dosyasını (.json) buraya bırakın', // DeepL
  HB_INVALID: 'Bu, ısıtma ayarlarının yedeklenmesi değildir', // DeepL
  HB_OTHER_FILE: 'Başka bir dosya seçin', // DeepL
  HB_SAME_DEVICE: 'aynı cihaz',
  HB_NOT_CONNECTED: 'bağlı değil',
  HB_OTHER_DEVICE: 'farklı cihaz (yedeklemede ürün kimliği {backup}, {now} bağlı), atlandı', // DeepL
  HB_UNIT_MISMATCH: 'Bu yedeklemede sıcaklıklar {unit} biriminde belirtilmiştir. Geri yüklemeden önce Uygulama ayarlarından sıcaklık birimini değiştirin.', // DeepL
  HB_CHANGED: 'Değiştirildi', // DeepL
  ALL: 'Tümü',
  HB_NOW: 'Şimdi',
  HB_BACKUP_VALUE: 'Yedekleme', // DeepL
  HB_NOT_AVAILABLE: 'mevcut değil', // DeepL
  HB_SENDING: 'gönderme', // DeepL
  HB_WRITTEN: 'yazılmış', // DeepL
  HB_RESTORE_NUM: '{num} değeri geri yükle',
  RESTORE: 'Geri yükle',
  HB_CONFIRM: '{file} dosyasındaki {num} değer EMS cihazlarına yazılsın mı?',
  HB_CONFIRM_TEXT: 'Değerler teker teker gönderilir. Bir cihaz bir değeri reddederse, geri kalanlar yine de gönderilir.', // DeepL
  HB_DONE: '{num} değer yazıldı',
  HB_DONE_FAILED: '{num} başarısız oldu. Cihazlar genellikle kurulumcu düzeyindeki değerleri kabul etmez; ayrıntılar Sistem Günlüğü\'nde yer almaktadır.', // DeepL
  HB_NO_CHANGES: 'Yedeklemeden hiçbir farkı yoktur', // DeepL
  HB_DIFFERENT_DEVICE: 'farklı cihaz',
  SELECT_ALL: 'Tümünü seç',
  DASHBOARD_EMPTY: 'Henüz gösterilecek bir şey yok',
  NO_DEVICES: 'Henüz hiçbir EMS cihazı bulunamadı', // DeepL
  ADD_COMMAND: 'Komut ekle',
  EDIT_COMMAND: 'Düzenle komutu', // DeepL
  RUN_COMMAND: 'Komutu çalıştır',
  ADD_SCHEDULE: 'Zamanlama ekle',
  EDIT_SCHEDULE: 'Programı düzenle', // DeepL
  ADD_ENTITY: 'Varlık ekle',
  EDIT_ENTITY: 'Varlığı Düzenle',
  ADD_ANALOG_SENSOR: 'Analog sensör ekleyin', // DeepL
  EDIT_ANALOG_SENSOR: 'Analog sensörü düzenle',
  REMOVE_ANALOG_SENSOR: 'Analog Sensörü Kaldır', // DeepL
  REMOVE_SENSOR_TEXT: '"{name}" sensörü hemen kaldırılır.',
  EDIT_TEMP_SENSOR: 'Sıcaklık Sensörünü Düzenle',
  INSTALL_FIRMWARE: 'Firmware\'i Yükle ({channel})', // DeepL
  PARTITION: 'Bölüm',
  PULSE: 'Pulse', // DeepL
  SYS_HOME: 'Ev', // draft, please review
  SYS_HOME_HELP: 'En önemli şeyler bir bakışta. Geri kalan her şey Sistem menüsünde.', // draft, please review
  SYS_MENU: 'Sistem menüsü', // draft, please review
  SYS_MENU_HELP: 'Sisteminizin tüm değerleri ve ayarları, konulara göre sıralı.', // draft, please review
  SYS_FAV_HELP: 'En önemli ayarlarınız. Bu listeyi değiştirmek için değerin yanındaki yıldızı kullanın.', // draft, please review
  SYS_FAV_NONE: 'Henüz favori yok. Değerleri yıldızla işaretleyin.', // draft, please review
  SYS_GROUP_HEATING: 'Isıtma', // draft, please review
  SYS_GROUP_PLANT: 'Tesisat', // draft, please review
  SYS_GROUP_OVERVIEW: 'Genel bakış', // draft, please review
  SYS_GROUP_TOOLS: 'Sensörler ve otomasyon', // draft, please review
  SYS_LEVEL_MOVE_ALL: 'Yalnızca tüm değerlerde göster', // draft, please review
  SYS_LEVEL_MOVE_NORMAL: 'Normal görünümde göster', // draft, please review
  SYS_HC: 'Isıtma devresi {0}', // draft, please review
  SYS_DHW: 'Sıcak su', // draft, please review
  SYS_AHS: 'Yardımcı ısıtıcı', // draft, please review
  SYS_HS: 'Isı kaynağı', // draft, please review
  SYS_SRC: 'Kaynak', // draft, please review
  SYS_OTHER_DEVICES: 'Diğer cihazlar', // draft, please review
  SYS_SERVICE: 'Bakım ve arızalar', // draft, please review
  SYS_ENERGY: 'Enerji ve çalışma süreleri', // draft, please review
  SYS_SEC_MODE: 'Çalışma', // draft, please review
  SYS_SEC_TEMPS: 'Sıcaklıklar', // draft, please review
  SYS_SEC_STATUS: 'Durum', // draft, please review
  SYS_SEC_ADVANCED: 'Isıtma eğrisi ve kurulum', // draft, please review
  SYS_SEC_MEASURE: 'Ölçülen değerler', // draft, please review
  SYS_SEC_OTHER: 'Diğer değerler', // draft, please review
  SYS_LEVEL_NORMAL: 'Normal', // draft, please review
  SYS_NO_RESULTS: 'Sonuç bulunamadı. Tüm değerleri veya başka bir terimi deneyin.', // draft, please review
  SYS_NOTHING_HERE: 'Normal düzeyinde burada gösterilecek bir şey yok.', // draft, please review
  SYS_HIDDEN: 'Gizli uzman değerleri: {num}', // draft, please review
  SYS_SEARCH_RESULTS: 'Arama sonuçları', // draft, please review
  SYS_CHANGED_WAS: 'değiştirildi (önceki: {0})', // draft, please review
  SYS_NO_DEVICES: 'Henüz cihaz bulunamadı.', // draft, please review
  SYS_ROOM_TEMP: 'Oda sıcaklığı', // draft, please review
  SYS_WISH: 'Hedef', // draft, please review
  SYS_CURRENT: 'Mevcut sıcaklık', // draft, please review
  SYS_OUTDOOR: 'Dış sıcaklık', // draft, please review
  SYS_FLOW: 'Gidiş', // draft, please review
  SYS_RETURN: 'Dönüş', // draft, please review
  SYS_PRESSURE: 'Sistem basıncı', // draft, please review
  SYS_POWER: 'Güç', // draft, please review
  SYS_BURNER_ON: 'Brülör çalışıyor', // draft, please review
  SYS_BURNER_OFF: 'Brülör kapalı', // draft, please review
  SYS_DHW_HEATING: 'Isıtılıyor', // draft, please review
  SYS_READY: 'Hazır', // draft, please review
  SYS_ONE_TIME: 'Tek seferlik ısıtma', // draft, please review
  SYS_NOTES: 'Notlar', // draft, please review
  SYS_NOTES_SUB: 'Arızalar ve bakım', // draft, please review
  SYS_LAST_FAULT: 'Son arıza', // draft, please review
  SYS_MAINT_MSG: 'Bakım mesajı', // draft, please review
  SYS_NEXT_MAINT: 'Sonraki bakım', // draft, please review
  SYS_GOTO_SERVICE: 'Bakım ve arızalara git', // draft, please review
};

export default tr;
