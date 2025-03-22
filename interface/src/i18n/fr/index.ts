import type { Translation } from '../i18n-types';

const fr: Translation = {
  LANGUAGE: 'Langue',
  RETRY: 'Réessayer',
  LOADING: 'Chargement',
  IS_REQUIRED: '{0} est requis',
  SIGN_IN: 'Se connecter',
  SIGN_OUT: 'Se déconnecter',
  USERNAME: "Nom d'utilisateur",
  PASSWORD: 'Mot de passe',
  SU_PASSWORD: 'Mot de passe su',
  SETTINGS_OF: 'Paramètres {0}',
  HELP: 'Aide',
  LOGGED_IN: 'Connecté en tant que {name}',
  PLEASE_SIGNIN: 'Veuillez vous connecter pour continuer',
  UPLOAD_SUCCESSFUL: 'Upload terminée',
  DOWNLOAD_SUCCESSFUL: 'Téléchargement terminé',
  INVALID_LOGIN: 'Informations de connexion invalides',
  NETWORK: 'Réseau',
  SECURITY: 'Sécurité',
  ONOFF_CAP: 'ON/OFF',
  ONOFF: 'on/off',
  TYPE: 'Type',
  DESCRIPTION: 'Description',
  ENTITIES: 'entités',
  REFRESH: 'Rafraîchir',
  EXPORT: 'Exporter',
  FAVORITES: "Favoris",
  DEVICE_DETAILS: "Détails de l'appareil",
  ID_OF: 'ID {0}',
  DEVICE: 'Appareil',
  PRODUCT: 'Produit',
  VERSION: 'Version',
  BRAND: 'Marque',
  ENTITY_NAME: "Nom de l'entité",
  VALUE: 'Valeur',
  DEVICES: 'Appareils',
  SENSORS: 'Capteurs',
  RUN_COMMAND: 'Lancer une commande',
  CHANGE_VALUE: 'Changer la valeur',
  CANCEL: 'Annuler',
  RESET: 'Réinitialiser',
  APPLY_CHANGES: 'Apply Changes ({0})', // TODO translate
  UPDATE: 'Update', // TODO translate
  EXECUTE: 'Execute', // TODO translate
  REMOVE: 'Enlever',
  PROBLEM_UPDATING: 'Problème lors de la mise à jour',
  PROBLEM_LOADING: 'Problème lors du chargement',
  ANALOG_SENSOR: 'Capteur analogique',
  ANALOG_SENSORS: 'Capteurs analogiques',
  SETTINGS: 'Paramètres',
  UPDATED_OF: '{0} mis à jour',
  UPDATE_OF: 'Mise à jour de {0}',
  REMOVED_OF: '{0} enlevé',
  DELETION_OF: '{0} supprimé',
  OFFSET: 'Offset',
  FACTOR: 'Facteur',
  FREQ: 'Fréquence',
  DUTY_CYCLE: 'Cycle de fonctionnement',
  UNIT: 'Unité',
  STARTVALUE: 'Valeur de départ',
  WARN_GPIO: 'Attention: soyez vigilant en choisissant un GPIO!',
  EDIT: 'Éditer',
  SENSOR: 'Capteur',
  TEMP_SENSOR: 'Capteur de température',
  TEMP_SENSORS: 'Capteurs de température',
  WRITE_CMD_SENT: 'Envoyer la commande sent',
  EMS_BUS_WARNING:  'Bus EMS déconnecté. Si ce message persiste après quelques secondes, vérifiez les paramètres et la configuration de la carte.',
  EMS_BUS_SCANNING: 'Scan des appareils EMS...',
  CONNECTED: 'Connecté',
  TX_ISSUES: 'Problèmes de transmission (Tx) - Essayez un autre mode Tx',
  DISCONNECTED: 'Déconnecté',
  EMS_SCAN: 'Etes-vous sûr de vouloir lancer un scan complet du bus EMS ?',
  DATA_TRAFFIC: 'Data Traffic', // TODO translate
  EMS_DEVICE: 'Appareils EMS',
  SUCCESS: 'SUCCÈS',
  FAIL: 'ÉCHEC',
  QUALITY: 'QUALITÉ',
  SCAN: 'Scan',
  STATUS_NAMES: [
    'Télégrammes EMS reçus (Rx)',
    'Lectures EMS (Tx)',
    'Écritures EMS (Tx)',
    'Lectures capteurs de température',
    'Lectures capteurs analogiques',
    'Publications MQTT',
    "Appels à l'API",
    'Messages Syslog'
  ],
  NUM_DAYS: '{num} jour{{s}}',
  NUM_SECONDS: '{num} seconde{{s}}',
  NUM_HOURS: '{num} heure{{s}}',
  NUM_MINUTES: '{num} minute{{s}}',
  APPLICATION: "l'application",
  CUSTOMIZATIONS: 'Personnalisation',
  APPLICATION_RESTARTING: 'EMS-ESP redémarre',
  BOARD_PROFILE: 'Profil de carte',
  CUSTOM: 'Personnalisé',
  GPIO_OF: 'GPIO {0}',
  BUTTON: 'Bouton',
  TEMPERATURE: 'Température',
  PHY_TYPE: 'Eth PHY Type',
  DISABLED: 'désactivé',
  TX_MODE: 'EMS Tx Mode',
  HARDWARE: 'Hardware',
  EMS_BUS: '{{BUS|EMS BUS}}',
  GENERAL_OPTIONS: 'Options générales',
  LANGUAGE_ENTITIES: 'Langue (pour les entités du matériel)',
  HIDE_LED: 'Cacher la LED',
  ENABLE_TELNET: 'Activer la console Telnet',
  ENABLE_ANALOG: 'Activer les capteurs analogiques',
  CONVERT_FAHRENHEIT: 'Convertir les températures en Fahrenheit',
  BYPASS_TOKEN: "Contourner l'autorisation du jeton d'accès sur les appels API",
  READONLY: 'Activer le mode lecture uniquement (bloque toutes les commandes EMS sortantes en écriture Tx)',
  UNDERCLOCK_CPU: 'Underclock du CPU',
  HEATINGOFF: 'Start boiler with forced heating off', // TODO translate
  REMOTE_TIMEOUT: 'Remote timeout',
  REMOTE_TIMEOUT_EN: 'Disable remote on missing room temperature', // TODO translate
  MIN_DURATION: 'Wait time',
  ENABLE_SHOWER_TIMER: 'Activer la minuterie de la douche',
  ENABLE_SHOWER_ALERT: 'Activer les alertes de durée de douche',
  TRIGGER_TIME: 'Durée avant déclenchement',
  COLD_SHOT_DURATION: "Durée du coup d'eau froide",
  FORMATTING_OPTIONS: 'Options de mise en forme',
  BOOLEAN_FORMAT_DASHBOARD: 'Tableau de bord du format booléen',
  BOOLEAN_FORMAT_API: 'Format booléen API/MQTT',
  ENUM_FORMAT: 'Format enum API/MQTT',
  INDEX: 'Index',
  ENABLE_PARASITE: 'Activer la puissance 1-wire parasite',
  LOGGING: 'Journal',
  LOG_HEX: 'Enregistrer les télégrammes EMS en hexadécimal',
  ENABLE_SYSLOG: 'Activer les logs système',
  LOG_LEVEL: 'Niveau de log',
  MARK_INTERVAL: 'Intervalle de marquage',
  SECONDS: 'secondes',
  MINUTES: 'minutes',
  HOURS: 'heures',
  RESTART: 'Redémarrer',
  RESTART_TEXT: 'EMS-ESP a besoin de redémarrer pour appliquer les changements de paramètres du système',
  RESTART_CONFIRM: 'Etes-vous sûr de vouloir redémarrer EMS-ESP ?',
  COMMAND: 'Commande',
  CUSTOMIZATIONS_RESTART: 'Toutes les personnalisations ont été supprimées. Redémarrage...',
  CUSTOMIZATIONS_FULL: 'Les entités sélectionnées ont dépassé la limite. Veuillez sauvegarder par lots',
  CUSTOMIZATIONS_SAVED: 'Personnalisations enregistrées',
  CUSTOMIZATIONS_HELP_1: 'Sélectionnez un appareil et personnalisez les options des entités ou cliquez pour renommer',
  CUSTOMIZATIONS_HELP_2: 'marquer comme favori',
  CUSTOMIZATIONS_HELP_3: "désactiver l'action d'écriture",
  CUSTOMIZATIONS_HELP_4: "exclure de MQTT et de l'API",
  CUSTOMIZATIONS_HELP_5: 'cacher des appareils',
  CUSTOMIZATIONS_HELP_6: 'remove from memory', // TODO translate
  SELECT_DEVICE: 'Sélectionnez un appareil',
  SET_ALL: 'tout régler',
  OPTIONS: 'Options',
  NAME: 'Nom',
  CUSTOMIZATIONS_RESET: 'Êtes-vous sûr de vouloir supprimer toutes les personnalisations, y compris les paramètres personnalisés des capteurs de température et analogiques ?',
  SUPPORT_INFORMATION: 'Information de support',
  HELP_INFORMATION_1: 'Visitez le wiki en ligne pour obtenir des instructions sur la façon de configurer EMS-ESP',
  HELP_INFORMATION_2: 'Pour une discussion en direct avec la communauté, rejoignez notre serveur Discord',
  HELP_INFORMATION_3: 'Pour demander une fonctionnalité ou signaler un problème',
  HELP_INFORMATION_4: "N'oubliez pas de télécharger et de joindre les informations relatives à votre système pour obtenir une réponse plus rapide lorsque vous signalez un problème",
  UPLOAD: 'Upload',
  DOWNLOAD: '{{D|d|d}}ownload',
  INSTALL: 'Installer',
  ABORTED: 'annulé',
  FAILED: 'échoué',
  SUCCESSFUL: 'réussi',
  SYSTEM: 'Système',
  LOG_OF: '{0} Log',
  STATUS_OF: 'Statut {0}',
  DOWNLOAD_UPLOAD: 'Download/Upload', // TODO translate
  CLOSE: 'Fermer',
  USE: 'Utiliser',
  FACTORY_RESET: 'Réinitialisation',
  SYSTEM_FACTORY_TEXT: "L'appareil a été réinitialisé et va maintenant redémarrer",
  SYSTEM_FACTORY_TEXT_DIALOG: "Êtes-vous sûr de vouloir réinitialiser l'appareil à ses paramètres d'usine ?",
  AVAILABLE_VERSION: 'Latest Available Versions', // TODO translate
  STABLE: 'Stable', // TODO translate
  DEVELOPMENT: 'Développement',
  UPTIME: 'Durée de fonctionnement du système',
  FREE_MEMORY: 'Libre Memory',
  PSRAM: 'PSRAM (Taille / Libre)',
  FLASH: 'Flash Chip (Taille , Vitesse)',
  APPSIZE: 'Application (Partition: Utilisée / Libre)',
  FILESYSTEM: 'File System (Utilisée / Libre)',
  BUFFER_SIZE: 'Max taille du buffer',
  COMPACT: 'Compact',
  DOWNLOAD_SETTINGS_TEXT: 'Create a backup of your configuration and settings', // TODO translate
  UPLOAD_TEXT: 'Upload a new firmware file (.bin) or a backup file (.json)', // TODO translate
  UPLOAD_DROP_TEXT: 'Drop a firmware .bin file or click here', // TODO translate
  ERROR: 'Erreur inattendue, veuillez réessayer',
  TIME_SET: 'Time set',
  MANAGE_USERS: 'Gérer les utilisateurs',
  IS_ADMIN: 'admin',
  USER_WARNING: 'Vous devez avoir au moins un utilisateur admin configuré',
  ADD: 'Ajouter',
  ACCESS_TOKEN_FOR: "Jeton d'accès pour",
  ACCESS_TOKEN_TEXT: "Le jeton ci-dessous est utilisé avec les appels d'API REST qui nécessitent une autorisation. Il peut être passé soit en tant que jeton Bearer dans l'en-tête Authorization, soit dans le paramètre de requête URL access_token.",
  GENERATING_TOKEN: 'Génération de jeton',
  USER: 'Utilisateur',
  MODIFY: 'Modifier',
  SU_TEXT: "Le mot de passe su (super utilisateur) est utilisé pour signer les jetons d'authentification et activer les privilèges d'administrateur dans la console.",
  NOT_ENABLED: 'Non activé',
  ERRORS_OF: 'Erreurs {0}',
  DISCONNECT_REASON: 'Raison de la déconnexion',
  ENABLE_MQTT: 'Activer le MQTT',
  BROKER: 'Broker',
  CLIENT: 'Client',
  BASE_TOPIC: 'Base',
  OPTIONAL: 'optionnel',
  FORMATTING: 'Mise en forme',
  MQTT_FORMAT: 'Format du Topic/Payload',
  MQTT_NEST_1: 'Englobé dans un topic unique',
  MQTT_NEST_2: 'En tant que topics individuels',
  MQTT_RESPONSE: 'Publier le résultat des commandes dans un topic `response`',
  MQTT_PUBLISH_TEXT_1: 'Publier des topics à valeur unique sur changement',
  MQTT_PUBLISH_TEXT_2: 'Publier vers des topics de commande (ioBroker)',
  MQTT_PUBLISH_TEXT_3: 'Activer la découverte MQTT',
  MQTT_PUBLISH_TEXT_4: 'Préfixe pour les topics découverte',
  MQTT_PUBLISH_TEXT_5: 'Discovery type', // TODO translate
  MQTT_PUBLISH_INTERVALS: 'Intervalles de publication',
  MQTT_INT_BOILER: 'Chaudières et pompes à chaleur',
  MQTT_INT_THERMOSTATS: 'Thermostats',
  MQTT_INT_SOLAR: 'Modules solaires',
  MQTT_INT_MIXER: 'Modules mélangeurs',
  MQTT_INT_WATER: 'Modules eau',
  MQTT_QUEUE: 'Queue MQTT',
  DEFAULT: 'Défaut',
  MQTT_ENTITY_FORMAT: 'Entity ID format', // TODO translate
  MQTT_ENTITY_FORMAT_0: 'Single instance, long name (v3.4)', // TODO translate
  MQTT_ENTITY_FORMAT_1: 'Single instance, short name', // TODO translate
  MQTT_ENTITY_FORMAT_2: 'Multiple instances, short name', // TODO translate
  MQTT_CLEAN_SESSION: 'Flag Clean Session',
  MQTT_RETAIN_FLAG: 'Toujours activer le Retain Flag',
  INACTIVE: 'Inactif',
  ACTIVE: 'Actif',
  UNKNOWN: 'Inconnu',
  SET_TIME: "Définir l'heure",
  SET_TIME_TEXT: "Entrer la date et l'heure locale ci-dessous pour régler l'heure",
  LOCAL_TIME: 'Heure locale',
  UTC_TIME: 'Heure UTC',
  ENABLE_NTP: 'Activer le NTP',
  NTP_SERVER: 'Serveur NTP',
  TIME_ZONE: 'Fuseau horaire',
  ACCESS_POINT: "Point d'accès",
  AP_PROVIDE: "Activer le Point d'Accès",
  AP_PROVIDE_TEXT_1: 'toujours',
  AP_PROVIDE_TEXT_2: 'quand le WiFi est déconnecté',
  AP_PROVIDE_TEXT_3: 'jamais',
  AP_PREFERRED_CHANNEL: 'Canal préféré',
  AP_HIDE_SSID: 'Cacher le SSID',
  AP_CLIENTS: 'AP Clients',
  AP_MAX_CLIENTS: 'Max Clients',
  AP_LOCAL_IP: 'IP locale',
  NETWORK_SCAN: 'Scanner les réseaux WiFi',
  IDLE: 'Inactif',
  LOST: 'Perdu',
  SCANNING: 'Scan en cours',
  SCAN_AGAIN: 'Rescanner',
  NETWORK_SCANNER: 'Scan réseau',
  NETWORK_NO_WIFI: 'Pas de réseau WiFi trouvé',
  NETWORK_BLANK_SSID: 'laisser vide pour désactiver le WiFi',
  NETWORK_BLANK_BSSID: 'leave blank to use only SSID', // TODO translate
  TX_POWER: 'Puissance Tx',
  HOSTNAME: "Nom d'hôte",
  NETWORK_DISABLE_SLEEP: 'Désactiver le mode veille du WiFi',
  NETWORK_LOW_BAND: 'Utiliser une bande passante WiFi plus faible',
  NETWORK_USE_DNS: 'Activer le service mDNS',
  NETWORK_ENABLE_CORS: 'Activer CORS',
  NETWORK_CORS_ORIGIN: 'Origine CORS',
  NETWORK_FIXED_IP: 'Utiliser une adresse IP fixe',
  NETWORK_GATEWAY: 'Passerelle',
  NETWORK_SUBNET: 'Masque de sous-réseau',
  NETWORK_DNS: 'Serveurs DNS',
  ADDRESS_OF: 'Adresse de {0}',
  ADMINISTRATOR: 'Administrator',
  GUEST: 'Invité',
  NEW: 'Nouveau',
  NEW_NAME_OF: 'Nouveau nom de {0}',
  ENTITY: 'entité',
  MIN: 'min',
  MAX: 'max',
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
  ONCHANGE: 'Sur le changement', // TODO translate
  CONDITION: 'Condition', // TODO translate
  IMMEDIATE: 'Immédiate', // TODO translate
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
  ENABLE_TLS: 'Activer TLS',
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
  MODULES_1: 'Activer ou désactiver les modules externes',
  MODULES_UPDATED: 'Modules updated', // TODO translate
  MODULES_DESCRIPTION: 'Click on the Module to activate or de-activate EMS-ESP library modules', // TODO translate
  MODULES_NONE: 'No external modules detected', // TODO translate
  RENAME: 'Rename', // TODO translate
  ENABLE_MODBUS: 'Activer Modbus',
  VIEW_LOG: 'View log to diagnose issues', // TODO translate
  UPLOAD_DRAG: 'drag and drop a file here or click to select one', // TODO translate
  SERVICES: 'Services', // TODO translate
  ALLVALUES: 'All Values', // TODO translate
  SPECIAL_FUNCTIONS: 'Special Functions',
  WAIT_FIRMWARE: 'Firmware is uploading and installing', // TODO translate
  INSTALL_VERSION: 'This will install version {0}. Are you sure?', // TODO translate
  UPGRADE_AVAILABLE: 'There is a firmware upgrade available!', // TODO translate
  LATEST_VERSION: 'You are using the latest firmware version', // TODO translate
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
  INTERNET_CONNECTION_REQUIRED: 'Internet connection required for automatic version checking and upgrading',
};

export default fr;
