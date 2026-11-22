import type { Translation } from '../i18n-types';

const fr: Translation = {
  LANGUAGE: 'Langue',
  RETRY: 'Réessayer',
  IS_REQUIRED: '{0} est obligatoire', // DeepL
  SIGN_IN: 'Se connecter',
  SIGN_IN_SUBTITLE: 'Passerelle pour les systèmes de chauffage EMS', // DeepL
  SIGN_OUT: 'Se déconnecter',
  USERNAME: "Nom d'utilisateur",
  PASSWORD: 'Mot de passe',
  APPLICATION_SETTINGS: 'Paramètres de l\'application',
  AP_SETTINGS: 'Paramètres du point d\'accès',
  NETWORK_SETTINGS: 'Paramètres réseau',
  NTP_SETTINGS: 'Paramètres NTP',
  MQTT_SETTINGS: 'Paramètres MQTT',
  EMS_BUS_SETTINGS: 'Paramètres du bus EMS',
  HELP: 'Aide',
  LOGGED_IN: 'Connecté en tant que {name}',
  PLEASE_SIGNIN: 'Veuillez vous connecter pour continuer',
  UPLOAD_SUCCESSFUL: 'Téléversement réussi',
  DOWNLOAD_SUCCESSFUL: 'Téléchargement réussi', // DeepL
  INVALID_LOGIN: 'Nom d\'utilisateur ou mot de passe incorrect', // DeepL
  NETWORK: 'Réseau',
  SECURITY: 'Sécurité',
  ONOFF_CAP: 'MARCHE/ARRÊT', // DeepL
  ONOFF: 'marche/arrêt', // DeepL
  TYPE: 'Type',
  ENTITIES: 'entités',
  EXPORT: 'Exporter',
  FAVORITES: "Favoris",
  CLIENT_ID: 'ID client',
  DEVICE_ID: 'ID de l\'appareil',
  PRODUCT_ID: 'ID du produit',
  ENTITY_ID: 'ID de l\'entité',
  SENSOR_ID: 'ID du capteur',
  TYPE_ID: 'ID du type',
  DEVICE: 'Appareil',
  BRAND: 'Marque',
  VERSION: 'Version',
  ENTITY_NAME: "Nom de l'entité",
  VALUE: 'Valeur',
  DEVICES: 'Appareils',
  SENSORS: 'Capteurs',
  AUTOMATION: 'Automatisation',
  CONNECTIVITY: 'Connectivité',
  MAINTENANCE: 'Maintenance',
  CHANGE_VALUE: 'Modifier la valeur', // DeepL
  CANCEL: 'Annuler',
  REMOVE_ALL: 'Tout Supprimer', // DeepL
  APPLY_CHANGES: 'Appliquer les modifications ({0})', // DeepL
  UPDATE: 'Mise à jour', // DeepL
  REMOVE: 'Supprimer', // DeepL
  RESET: 'Réinitialiser',
  PROBLEM_UPDATING: 'Problème de mise à jour', // DeepL
  PROBLEM_LOADING: 'Problème de chargement', // DeepL
  ANALOG_SENSORS: 'Capteurs analogiques',
  SETTINGS: 'Paramètres',
  SETTINGS_UPDATED: 'Paramètres mis à jour',
  NAME_UPDATED: 'Nom mis à jour',
  SENSOR_UPDATED: 'Capteur mis à jour',
  ANALOG_SENSOR_UPDATED: 'Capteur analogique mis à jour',
  OFFSET: 'Décalage', // DeepL
  FACTOR: 'Facteur',
  FREQ: 'Fréquence',
  DUTY_CYCLE: 'Cycle de service', // DeepL
  UNIT: 'UoM', // DeepL
  STARTVALUE: 'Valeur initiale', // DeepL
  EDIT: 'Modifier', // DeepL
  TEMP_SENSORS: 'Capteurs de température',
  WRITE_CMD_SENT: 'Commande « Write » envoyée', // DeepL
  EMS_BUS_WARNING: 'Bus EMS déconnecté. Si cet avertissement persiste après quelques secondes, veuillez vérifier les paramètres et le profil de la carte.', // DeepL
  CONNECTED: 'Connecté',
  TX_ISSUES: 'Problèmes de transmission – vérifiez le mode de transmission', // DeepL
  DISCONNECTED: 'Déconnecté',
  DATA_TRAFFIC: 'Trafic de données', // DeepL
  EMS_DEVICE: 'Appareil EMS', // DeepL
  SUCCESS: 'RÉUSSITE', // DeepL
  FAIL: 'ÉCHEC',
  QUALITY: 'QUALITÉ',
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
  APPLICATION: 'Application', // DeepL
  CUSTOMIZATIONS: 'Personnalisation',
  APPLICATION_RESTARTING: 'EMS-ESP est en cours de redémarrage', // DeepL
  BOARD_PROFILE: 'Profil de la carte', // DeepL
  CUSTOM: 'Personnalisé',
  GPIO_OF: '{0} GPIO', // DeepL
  BUTTON: 'Bouton',
  TEMPERATURE: 'Température',
  PHY_TYPE: 'Type de module PHY Eth', // DeepL
  DISABLED: 'désactivé',
  TX_MODE: 'Mode EMS Tx', // DeepL
  EMS_BUS: 'EMS Bus',
  EMS_BUS_ID: 'ID EMS Bus',
  HARDWARE: 'Matériel', // DeepL
  LANGUAGE_ENTITIES: 'Langue (pour les entités d\'appareils)', // DeepL
  HIDE_LED: 'Masquer la LED', // DeepL
  ENABLE_TELNET: 'Activer la console Telnet',
  ENABLE_ANALOG: 'Activer les capteurs analogiques',
  CONVERT_FAHRENHEIT: 'Convertissez les valeurs de température en degrés Fahrenheit', // DeepL
  READONLY: 'Activer le mode lecture seule (bloque toutes les commandes d\'écriture EMS Tx sortantes)', // DeepL
  WRITE_PROTECTION: 'Protégez les appareils EMS contre les écritures répétées et fréquentes', // DeepL
  WRITE_LIMIT: 'Nombre maximal d\'écritures par minute (0 = aucune limite)', // DeepL
  WRITE_LIMIT_REFUSE: 'Refusez toute saisie dépassant la limite (sinon, seul un avertissement est enregistré)', // DeepL
  UNDERCLOCK_CPU: 'Réduisez la fréquence d\'horloge du processeur', // DeepL
  REMOTE_TIMEOUT: 'Délai d\'expiration de la connexion à distance', // DeepL
  REMOTE_TIMEOUT_EN: 'Désactiver la commande à distance en cas d\'absence de température ambiante', // DeepL
  HEATINGOFF: 'Démarrer la chaudière avec le chauffage forcé désactivé', // DeepL
  MIN_DURATION: 'Temps d\'attente', // DeepL
  ENABLE_SHOWER_TIMER: 'Activer le minuteur de douche', // DeepL
  ENABLE_SHOWER_ALERT: 'Activer l\'alerte de douche', // DeepL
  TRIGGER_TIME: 'Heure de déclenchement', // DeepL
  COLD_SHOT_DURATION: 'Durée du « cold shot »', // DeepL
  FORMATTING_OPTIONS: 'Options de mise en forme',
  BOOLEAN_FORMAT_DASHBOARD: 'Web au format booléen', // DeepL
  BOOLEAN_FORMAT_API: 'Format booléen API/MQTT',
  ENUM_FORMAT: 'Format Enum API/MQTT', // DeepL
  INDEX: 'Index',
  ENABLE_PARASITE: 'Activer l\'alimentation par parasite 1-Wire', // DeepL
  LOG_HEX: 'Enregistrez les télégrammes EMS au format hexadécimal', // DeepL
  ENABLE_SYSLOG: 'Activer Syslog', // DeepL
  LOG_LEVEL: 'Niveau de journalisation', // DeepL
  MARK_INTERVAL: 'Marquer l\'intervalle', // DeepL
  SECONDS: 'secondes',
  MINUTES: 'minutes',
  HOURS: 'heures',
  RESTART: 'Redémarrer',
  RESTART_TEXT: 'Il est nécessaire de redémarrer EMS-ESP pour que les modifications apportées aux paramètres système soient prises en compte.', // DeepL
  RESTART_CONFIRM: 'Êtes-vous sûr de vouloir redémarrer EMS-ESP ?', // DeepL
  COMMAND: 'Commandes', // DeepL
  CUSTOMIZATIONS_RESTART: 'Toutes les personnalisations ont été supprimées. Redémarrage en cours…', // DeepL
  CUSTOMIZATIONS_FULL: 'Le nombre d\'entités sélectionnées a dépassé la limite. Veuillez enregistrer par lots.', // DeepL
  CUSTOMIZATIONS_SAVED: 'Mises à jour de la personnalisation', // DeepL
  CUSTOMIZATIONS_HELP_1: 'Sélectionnez un appareil et personnalisez les options des entités', // DeepL
  CUSTOMIZATIONS_HELP_2: 'Ajouter aux Favoris', // DeepL
  CUSTOMIZATIONS_HELP_3: "désactiver l'action d'écriture",
  CUSTOMIZATIONS_HELP_4: 'À exclure de MQTT et de l\'API', // DeepL
  CUSTOMIZATIONS_HELP_5: 'Masquer dans les Appareils', // DeepL
  CUSTOMIZATIONS_HELP_6: 'effacer de la mémoire', // DeepL
  SELECT_DEVICE: 'Sélectionnez un appareil',
  MODIFIED: 'Modifié',
  RESET_SELECTED: 'Réinitialiser la sélection',
  OPTIONS: 'Options',
  NAME: 'Nom',
  CUSTOMIZATIONS_RESET: 'Êtes-vous sûr de vouloir supprimer toutes les personnalisations ?', // DeepL
  SYSTEM_INFORMATION: 'Informations système', // DeepL
  HELP_INFORMATION_1: 'Consultez le wiki en ligne pour obtenir des instructions sur la configuration d\'EMS-ESP', // DeepL
  HELP_INFORMATION_2: 'Pour participer au chat en direct de la communauté, rejoignez notre serveur Discord', // DeepL
  HELP_INFORMATION_3: 'Pour demander une fonctionnalité ou signaler un bug', // DeepL
  HELP_INFORMATION_4: 'Veuillez indiquer les informations relatives à votre système lorsque vous signalez un problème afin d\'obtenir une réponse plus rapide', // DeepL
  UPLOAD: 'Téléverser',
  DOWNLOAD: 'Télécharger',
  INSTALL: 'Installer',
  REINSTALL: 'Réinstaller',
  REINSTALL_CONFIRM: 'Êtes-vous sûr de vouloir réinstaller la version {0} ?',
  FAILED: 'Échec', // DeepL
  SYSTEM: 'Système',
  SYSTEM_LOG: 'Journal système',
  STATUS: 'État',
  DOWNLOAD_UPLOAD: 'Télécharger/Téléverser',
  CLOSE: 'Fermer',
  COMMAND_GUEST: 'Les invités peuvent exécuter cette commande',
  TOKENS_RESET: 'Cette mise à jour signe les jetons d\'accès à l\'aide d\'une nouvelle clé d\'appareil ; par conséquent, les jetons d\'accès à l\'API existants ne fonctionnent plus. Créez une clé API pour chaque intégration utilisant l\'API, telle que Home Assistant, et utilisez-la à la place de l\'ancien jeton.', // DeepL
  API_KEYS: 'Clés API',
  API_KEYS_TEXT: 'Les clés API permettent à des intégrations telles que Home Assistant et à des scripts d\'utiliser l\'API. Envoyez la clé dans l\'en-tête « Authorization: Bearer <CLÉ> ». Les clés en lecture permettent de lire des valeurs et d\'exécuter des Commandes que les invités peuvent exécuter ; les clés en lecture et en écriture permettent d\'effectuer toutes les actions qu\'un administrateur peut effectuer via l\'API.', // DeepL
  API_KEY_ACCESS: 'Accès',
  API_KEY_READ: 'Lire', // DeepL
  API_KEY_READ_WRITE: 'Lecture et Écriture', // DeepL
  API_KEY_SHOW_ONCE: 'Copiez cette clé dès maintenant et conservez-la en lieu sûr. Elle ne s\'affichera plus par la suite.', // DeepL
  CREATED: 'Créé', // DeepL
  COPY: 'Texte', // DeepL
  COPIED: 'Copié',
  USE: 'Utilisez', // DeepL
  FACTORY_RESET: 'Réinitialisation d\'Usine', // DeepL
  SYSTEM_FACTORY_TEXT_DIALOG: 'Êtes-vous sûr de vouloir réinitialiser EMS-ESP à ses paramètres d\'usine ?', // DeepL
  STABLE: 'Stable',
  DEVELOPMENT: 'Développement',
  UPTIME: 'Disponibilité du système', // DeepL
  LAST_RESTART: 'Dernier redémarrage',
  POWER_AND_RESET: 'Mise sous tension et réinitialisation', // DeepL
  FREE_MEMORY: 'Mémoire libre', // DeepL
  BUFFER_SIZE: 'Taille maximale de la mémoire tampon', // DeepL
  COMPACT: 'Concis', // DeepL
  SUMMARY: 'Résumé',
  SHOW_MORE: 'Afficher {n} autres', // DeepL
  SHOW_LESS: 'Afficher moins',
  COREDUMP: 'Fichier de vidage mémoire', // DeepL
  DOWNLOAD_SYSTEM_BACKUP: 'Sauvegarde Système', // DeepL
  ERROR: 'Erreur inattendue, veuillez réessayer',
  TIME_SET: 'Heure programmée', // DeepL
  MANAGE_USERS: 'Gérer les utilisateurs',
  USER_WARNING: 'Vous devez avoir configuré au moins un utilisateur administrateur', // DeepL
  ADD: 'Ajouter',
  TELNET_ENABLED: 'La console Telnet est activée. Telnet n\'étant pas crypté, désactivez-le dans les paramètres de l\'application si vous ne l\'utilisez pas.', // DeepL
  TOO_MANY_LOGINS: 'Trop de tentatives de connexion infructueuses, veuillez réessayer dans quelques minutes', // DeepL
  PASSWORD_KEEP: 'Laissez ce champ vide pour conserver le mot de passe actuel', // DeepL
  SETUP_TITLE: 'Définissez vos mots de passe',
  SETUP_TEXT: 'Cet appareil utilise encore les mots de passe par défaut. Définissez les vôtres avant de continuer.', // DeepL
  SETUP_ADMIN_PASSWORD: 'Nouveau mot de passe pour {name}',
  SETUP_AP_PASSWORD: 'Nouveau mot de passe du point d\'accès',
  SETUP_AP_REQUIRED: 'Aucun réseau Wi-Fi n’est encore configuré ; l’accès se fait donc via le point d’accès. 8 à 63 caractères, qui seront utilisés lors du prochain démarrage du point d’accès.', // DeepL
  SETUP_AP_OPTIONAL: 'Recommandé. 8 à 63 caractères ; cette valeur sera prise en compte au prochain démarrage du point d\'accès. Laissez ce champ vide pour conserver la valeur actuelle.', // DeepL
  SETUP_AP_DEFAULT: 'Le point d\'accès utilise toujours le mot de passe d\'usine. Définissez le vôtre dans les paramètres du point d\'accès.', // DeepL
  SETUP_VIEW_USER: 'Facultatif : ajoutez un utilisateur invité en lecture seule.',
  NETWORK_CORS_ORIGIN_HELP: 'L\'adresse Web permettant d\'appeler l\'API depuis un navigateur, par exemple http://homeassistant.local:8123', // DeepL
  NETWORK_CORS_WILDCARD: 'Avec *, n\'importe quel site web ouvert dans un navigateur sur votre réseau peut appeler l\'API. Indiquez plutôt l\'adresse exacte.', // DeepL
  NETWORK_ALLOWED_HOSTS: 'Noms d\'hôtes autorisés', // DeepL
  NETWORK_ALLOWED_HOSTS_HELP: 'Autres noms : cet appareil peut être ouvert dans un navigateur, par exemple sous le nom « ems-esp.fritz.box » ou sous le nom d’un proxy inverse. Son adresse IP, son nom d’hôte et son nom d’hôte.local fonctionnent toujours.', // DeepL
  NOT_ENABLED: 'Non activé',
  MQTT_ERRORS: 'Erreurs MQTT',
  DISCONNECT_REASON: 'Motif de la déconnexion', // DeepL
  MQTT_BASE_TOPIC: 'Rubrique de base', // DeepL
  LAST_DISCONNECT: 'Dernière déconnexion',
  CONNECTED_SINCE: 'Connecté depuis',
  ENABLE_MQTT: 'Activer MQTT', // DeepL
  BROKER: 'Broker',
  BASE_TOPIC: 'Base',
  OPTIONAL: 'facultatif', // DeepL
  MQTT_FORMAT: 'Format du sujet/de la charge utile', // DeepL
  MQTT_NEST_1: 'Regroupés dans une seule rubrique', // DeepL
  MQTT_NEST_2: 'Par thème', // DeepL
  MQTT_RESPONSE: 'Publiez la sortie de la commande dans un sujet « response »', // DeepL
  MQTT_PUBLISH_TEXT_1: 'Publiez des sujets à valeur unique en cas de modification', // DeepL
  MQTT_PUBLISH_TEXT_2: 'Publier dans les sujets de commande (ioBroker)', // DeepL
  MQTT_PUBLISH_TEXT_3: 'Activer la découverte MQTT',
  MQTT_PUBLISH_TEXT_4: 'Préfixe pour les thèmes de découverte', // DeepL
  MQTT_PUBLISH_TEXT_5: 'Type de découverte',
  MQTT_INPUT_NUMBER_FORMAT: 'Format des nombres saisis', // DeepL
  MQTT_PUBLISH_INTERVALS: 'Fréquence de publication', // DeepL
  MQTT_INT_BOILER: 'Chaudières et pompes à chaleur',
  MQTT_INT_THERMOSTATS: 'Thermostats',
  MQTT_INT_SOLAR: 'Modules solaires',
  MQTT_INT_MIXER: 'Modules de mélange', // DeepL
  MQTT_INT_WATER: 'Modules d\'eau', // DeepL
  MQTT_QUEUE: 'File d\'attente MQTT', // DeepL
  DEFAULT: 'Par défaut', // DeepL
  MQTT_ENTITY_FORMAT: 'Format d\'identification des entités', // DeepL
  MQTT_ENTITY_FORMAT_0: 'Occurrence unique, nom long (v3.4)', // DeepL
  MQTT_ENTITY_FORMAT_1: 'Une seule occurrence, un nom court', // DeepL
  MQTT_ENTITY_FORMAT_2: 'Plusieurs occurrences, nom court', // DeepL
  MQTT_CLEAN_SESSION: 'Lancer une session « propre »', // DeepL
  MQTT_RETAIN_FLAG: 'Activez toujours l\'indicateur de conservation', // DeepL
  MQTT_COMMANDS: 'Commandes reçues via MQTT', // DeepL
  MQTT_COMMANDS_0: 'Désactivé, lecture des valeurs uniquement', // DeepL
  MQTT_COMMANDS_1: 'Modifier des valeurs et exécuter des Commandes', // DeepL
  MQTT_COMMANDS_2: 'Tout, y compris les commandes système',
  INACTIVE: 'Inactif',
  ACTIVE: 'Actif',
  UNKNOWN: 'Inconnu',
  SET_TIME: 'Régler l\'Heure', // DeepL
  SET_TIME_TEXT: 'Saisissez ci-dessous la date et l\'heure locales pour régler l\'heure', // DeepL
  LOCAL_TIME: 'Heure locale',
  UTC_TIME: 'Heure UTC',
  ENABLE_NTP: 'Activer NTP', // DeepL
  NTP_SERVER: 'Serveur NTP',
  TIME_ZONE: 'Fuseau horaire',
  ACCESS_POINT: "Point d'accès",
  AP_PROVIDE: 'Activer le point d\'accès', // DeepL
  AP_PROVIDE_TEXT_2: 'Lorsqu\'aucune connexion réseau n\'est détectée', // DeepL
  AP_PROVIDE_TEXT_3: 'Jamais', // DeepL
  AP_PREFERRED_CHANNEL: 'Canal privilégié', // DeepL
  AP_HIDE_SSID: 'Masquer le SSID', // DeepL
  AP_CLIENTS: 'Clients AP',
  AP_MAX_CLIENTS: 'Nombre maximal de clients', // DeepL
  AP_LOCAL_IP: 'Adresse IP locale', // DeepL
  IDLE: 'Inactif',
  SCAN_AGAIN: 'Relisez-les', // DeepL
  NETWORK_NO_WIFI: 'Aucun réseau Wi-Fi détecté', // DeepL
  NETWORK_BLANK_BSSID: 'Laissez ce champ vide pour n\'utiliser que le SSID', // DeepL
  TX_POWER: 'Puissance d\'émission', // DeepL
  HOSTNAME: "Nom d'hôte",
  NETWORK_DISABLE_SLEEP: 'Désactivez le mode veille du Wi-Fi', // DeepL
  NETWORK_LOW_BAND: 'Utilisez une bande passante Wi-Fi plus faible', // DeepL
  NETWORK_USE_DNS: 'Activer le service mDNS',
  NETWORK_ENABLE_CORS: 'Activer CORS',
  NETWORK_CORS_ORIGIN: 'Origine CORS',
  NETWORK_FIXED_IP: 'Utilisez une adresse IP fixe', // DeepL
  NETWORK_GATEWAY: 'Passerelle',
  NETWORK_SUBNET: 'Masque de sous-réseau',
  NETWORK_DNS: 'Serveurs DNS',
  ADDRESS_OF: 'Adresse {0}', // DeepL
  ADMINISTRATOR: 'Administrateur', // DeepL
  GUEST: 'Invité',
  NEW: 'Nouveau',
  NEW_ENTITY_NAME: 'Nouveau nom de l\'entité',
  MIN: 'min',
  MAX: 'max',
  BLOCK_NAVIGATE_1: 'Vous avez des modifications non enregistrées',
  BLOCK_NAVIGATE_2: 'Si vous accédez à une autre page, vos modifications non enregistrées seront perdues. Êtes-vous sûr de vouloir quitter cette page ?', // DeepL
  UPLOAD_ABORTED: 'Téléversement interrompu',
  BLOCK_BUSY_1: 'En cours de rédaction', // DeepL
  BLOCK_BUSY_2: 'Si vous quittez cette page maintenant, l\'opération sera interrompue. Êtes-vous sûr de vouloir quitter cette page ?', // DeepL
  DISCARD: 'Ignorer', // DeepL
  COMMANDS_UPDATED: 'Commandes mises à jour',
  V_REQUIRED: 'Obligatoire',
  V_BETWEEN: 'La longueur doit être comprise entre {min} et {max}', // DeepL
  V_MAX_CHARS: 'Doit comporter {max} caractères ou moins', // DeepL
  V_CHARS: 'Doit comporter entre {min} et {max} caractères', // DeepL
  V_IN_USE: 'Déjà en service', // DeepL
  V_IP: 'Doit s\'agir d\'une adresse IP', // DeepL
  V_HOSTNAME: 'Le nom d\'hôte doit être valide', // DeepL
  V_IP_OR_HOSTNAME: 'Doit être une adresse IP ou un nom d\'hôte',
  V_NAME: '{max} caractères maximum : lettres, chiffres ou _', // DeepL
  V_USERNAME: '{max} caractères maximum : lettres, chiffres, _ ou .', // DeepL
  V_DAY: 'Choisissez au moins un jour',
  V_HEX: 'Doit être un nombre hexadécimal',
  V_FINGERPRINT: 'Il doit s\'agir d\'une empreinte SHA-1 (40 caractères hexadécimaux)', // DeepL
  V_EXAMPLE: 'Ils doivent se présenter comme suit : {example}', // DeepL
  V_NOT_DEFAULT: 'Choisissez un mot de passe différent', // DeepL
  ZERO_DISABLES: '0 désactive la fonction', // DeepL
  MINUS_ONE_DISABLES: '-1 permet de l\'éteindre', // DeepL
  LED_TYPE: 'Type de LED',
  PHY_POWER: 'Alimentation PHY',
  PHY_CLOCK: 'Horloge PHY',
  AUTO: 'Auto',
  PORT: 'Port',
  TIMEOUT: 'Délai d\'expiration',
  HOST: 'Hôte',
  SMTP_SERVER: 'Serveur SMTP',
  KEEP_ALIVE: 'Restez à l\'écoute', // DeepL
  MQTT_REFRESH: 'Actualiser MQTT',
  MQTT_REFRESHED: 'Mise à jour MQTT', // DeepL
  MQTT_R_USER: 'Déconnecté par l\'utilisateur',
  MQTT_R_TCP: 'Connexion TCP fermée',
  MQTT_R_PROTOCOL: 'Version du protocole non acceptée', // DeepL
  MQTT_R_ID: 'Identifiant client rejeté', // DeepL
  MQTT_R_SERVER: 'Serveur indisponible',
  MQTT_R_CREDENTIALS: 'Identifiants incorrects', // DeepL
  MQTT_R_AUTH: 'Non autorisé',
  MQTT_R_FINGERPRINT: 'L\'empreinte TLS ne correspond pas', // DeepL
  GPIO_INVALID: 'Paramètres GPIO non valides', // DeepL
  CONNECTED_VIA: 'Connecté via {0}',
  NETWORK_NOT_FOUND: 'Réseau introuvable',
  CONNECTION_FAILED: 'Échec de la connexion',
  CONNECTION_LOST: 'Connexion perdue',
  AP_LINGERING: 'En attente jusqu’à l’inactivité', // DeepL
  NONE: 'Aucun', // DeepL
  LOG_NO_CONNECTION: 'Aucune connectivité au service de journalisation', // DeepL
  CHECK_SYSTEM_LOG: 'Vérifiez le journal système pour identifier les causes possibles', // DeepL
  VALUE_TYPE: 'Type de valeur',
  DEFAULT_VALUE: 'Valeur par défaut',
  DEFAULT_NAME: 'Nom par défaut',
  ALWAYS_OFF: 'Toujours désactivé', // DeepL
  ALWAYS_ON: 'Toujours activé', // DeepL
  NO_DATA: 'Aucune entité favorite pour le moment. Marquez-les dans {customizations} ou consultez toutes les entités dans {devices}.', // DeepL
  STAY: 'Restez', // DeepL
  LEAVE: 'Laissez', // DeepL
  MENU: 'Menu',
  MORE: 'En savoir plus', // DeepL
  SHOW_PASSWORD: 'Afficher le mot de passe',
  STATUS_RUNNING: 'EMS-ESP est en cours d\'exécution', // DeepL
  NUM_ERRORS: '{num} erreur{{s}}',
  NUM_WARNINGS: '{num} avertissement{{s}}',
  HEAP: 'Heap',
  METER_USED: '{used} / {total} {unit} utilisée', // DeepL
  MAX_BLOCK: '{num} Ko maximum par bloc', // DeepL
  MAX_BLOCK_HELP: 'Le « Max block » correspond à la plus grande partie de mémoire heap pouvant être allouée en une seule fois. En dessous de 40 Ko, l’interface web et MQTT peuvent présenter des dysfonctionnements.', // DeepL
  FLASH_CHIP: 'Puce Flash', // DeepL
  FILE_SYSTEM: 'Système de fichiers',
  DETAILS: 'Détails',
  RECONNECTS: 'Se reconnecte', // DeepL
  CONNECTS: 'Se connecte', // DeepL
  ACTIVITY_HELP: 'Quelques échecs de réception (Rx) sont normaux. De nombreux échecs d\'émission (Tx) indiquent généralement que le mode d\'émission (Tx) sélectionné n\'est pas adapté à votre chaudière.', // DeepL
  PAUSE: 'Pause',
  RESUME: 'CV', // DeepL
  LOG_PAUSED: 'Mise en pause, sauts de ligne supprimés : {num}', // DeepL
  FILTER_LINES: 'Lignes de filtrage', // DeepL
  SCHEDULER: 'Scheduler',
  SCHEDULER_HELP_1: 'Automatisez les commandes en ajoutant des événements programmés ci-dessous', // DeepL
  SCHEDULER_HELP_2: 'au démarrage',
  SCHEDULE: 'Calendrier', // DeepL
  TIME: 'Heure', // DeepL
  TIMER: 'Minuterie', // DeepL
  ONCHANGE: 'En cas de modification', // DeepL
  CONDITION: 'Condition',
  SCHEDULE_UPDATED: 'Calendrier mis à jour', // DeepL
  CUSTOM_ENTITIES: 'Entités personnalisées',
  ENTITIES_HELP_1: 'Définissez des entités EMS personnalisées ou des variables utilisateur dynamiques', // DeepL
  ENTITIES_UPDATED: 'Entités mises à jour',
  WRITEABLE: 'Modifiable', // DeepL
  SHOWING: 'Affichage',
  SEARCH: 'Recherche', // DeepL
  CERT: 'Certificat CA (PEM)',
  ENABLE_TLS: 'Activer le protocole TLS', // DeepL
  TLS_CHECK: 'Vérifiez le courtier auprès de', // DeepL
  TLS_CHECK_CA: 'Certificat CA',
  TLS_CHECK_FINGERPRINT: 'Empreinte du certificat (SHA-1)',
  TLS_CHECK_NONE: 'Rien, non coché', // DeepL
  TLS_NOT_CHECKED: 'La connexion est cryptée, mais le serveur intermédiaire n’est pas vérifié ; un autre appareil du réseau pourrait donc se faire passer pour celui-ci', // DeepL
  ON: 'Sur', // DeepL
  OFF: 'Désactivé', // DeepL
  POLARITY: 'Polarité', // DeepL
  ACTIVEHIGH: 'Actif haut',
  ACTIVELOW: 'Actif bas',
  UNCHANGED: 'Inchangé',
  SECURITY_1: 'Ajouter ou supprimer des utilisateurs',
  DOWNLOAD_UPLOAD_1: 'Télécharger et téléverser les paramètres',
  MODULES: 'Module',
  MODULES_1: 'Activer ou désactiver des modules externes', // DeepL
  NETWORK_DESC: 'Wi-Fi, Ethernet et nom d\'hôte',
  AP_DESC: 'Point d\'accès intégré pour la configuration et l\'accès de secours',
  MQTT_DESC: 'Broker, topics et découverte',
  APPLICATION_DESC: 'Bus EMS, carte, capteurs et formatage',
  NTP_DESC: 'Serveur de temps et fuseau horaire',
  FIRMWARE_DESC: 'Version, mises à jour et récupération',
  MODULES_UPDATED: 'Modules mis à jour',
  MODULES_NONE: 'Aucun module externe détecté',
  RENAME: 'Renommer',
  ENABLE_MODBUS: 'Activer Modbus',
  VIEW_LOG: 'Consultez le journal pour diagnostiquer les problèmes', // DeepL
  SERVICES: 'Services',
  ALLVALUES: 'Toutes les valeurs',
  WAIT_FIRMWARE: 'Le micrologiciel est en cours de téléversement et d\'installation',
  INSTALL_VERSION: 'Cela installera la version {0}',
  UPDATE_AVAILABLE: 'Mise à jour disponible', // DeepL
  LATEST_VERSION: 'Vous utilisez la dernière version du micrologiciel ({0})',
  PLEASE_WAIT: 'Veuillez patienter',
  RESTARTING_PRE: 'Démarrage',
  RESTARTING_POST: 'Préparation',
  AUTO_SCROLL: 'Défilement automatique',
  DASHBOARD: 'Tableau de bord',
  DEVELOPER_MODE: 'Mode développeur',
  DISABLE_RESET: 'Désactiver la réinitialisation aux paramètres d\'usine', // DeepL
  AUTO_FW_CHECK: 'Vérifier automatiquement la disponibilité des mises à jour du micrologiciel', // DeepL
  BYTES: 'Octets',
  BITMASK: 'Masque de bits',
  DUPLICATE: 'En double', // DeepL
  DASHBOARD_1: 'Toutes les entités EMS actives marquées comme Favoris sont affichées ci-dessous, ainsi que toutes les entités personnalisées, les programmations et les capteurs externes.', // DeepL
  NO_GPIO: 'Aucun GPIO disponible', // DeepL
  PLATFORM: 'Plateforme',
  RELEASE_TYPE: 'Type de Version',
  FIRMWARE_VERSION_INFO: 'Informations sur la Version du Micrologiciel', // DeepL
  USER_PROFILE: 'Profil utilisateur',
  ONLINE_HELP: 'Aide en ligne', // DeepL
  UPGRADE_IMPORTANT_MESSAGES: 'Messages Importants de Mise à Jour', // DeepL
  UPGRADE_IMPORTANT_MESSAGES_2: 'Vous effectuez une mise à niveau vers une nouvelle version majeure. Il est conseillé de commencer par effectuer une sauvegarde du système. Consultez également l’aide en ligne pour prendre connaissance des éventuelles modifications importantes.', // DeepL
  UPGRADE_IMPORTANT_MESSAGES_3: 'La version 3.10 comporte des modifications majeures en matière de sécurité. Effectuez d’abord une sauvegarde du système : après la mise à jour, les mots de passe sont stockés sous forme hachée et les anciennes versions ne peuvent plus les lire ; cette sauvegarde est donc indispensable pour revenir à une version antérieure. Chaque appel à l’API nécessite désormais une clé API ; les intégrations telles que Home Assistant doivent donc être mises à jour. Veuillez consulter les modifications majeures dans l’aide en ligne avant de poursuivre.', // DeepL
  WARNING_SYSTEM_BACKUP: 'Cette opération crée une sauvegarde de l’intégralité de la configuration et des paramètres de votre système. Les mots de passe et les clés ne sont pas inclus, sauf si vous les indiquez ci-dessous ; la restauration de la sauvegarde conserve les mots de passe déjà enregistrés sur l’Appareil. Les Commandes sont toujours incluses et peuvent contenir des identifiants d’authentification ; veillez donc à ne pas les partager à la légère. Souhaitez-vous continuer ?', // DeepL
  BACKUP_INCLUDE_SECRETS: 'N\'oubliez pas d\'inclure les mots de passe et les clés (nécessaires pour effectuer une restauration sur un nouvel appareil)', // DeepL
  TEST_EMAIL_SUCCESSFUL: 'E-mail de test envoyé', // DeepL
  SYSTEM_NAME: 'Nom du système',
  COMMANDS: 'Commandes',
  COMMANDS_HELP_1: 'Définissez ci-dessous des commandes nommées réutilisables. Celles-ci peuvent être exécutées depuis la console, via l’API/MQTT, ou référencées par le Scheduler.', // DeepL
  COMMAND_EXECUTED: 'Commande exécutée', // DeepL
  RUN: 'Exécuter', // DeepL
  INTERNET_CONNECTION_CHECK: 'Vérification en ligne des versions disponibles', // DeepL
  UPLOAD_BACKUP_TEXT: 'Cliquez pour sélectionner, ou déposez un fichier de sauvegarde (.json) ici', // DeepL
  UPLOAD_FIRMWARE_TEXT: 'Cliquez pour sélectionner ou déposez ici un fichier de micrologiciel au format .bin/.md5', // DeepL
  UPLOAD_MD5_RECEIVED: 'Digest reçu, téléversez maintenant le fichier .bin du micrologiciel',
  UPLOAD_MD5_MATCHED: 'Correspondances MD5, installation du micrologiciel…', // DeepL
  UPLOAD_NOT_OFFICIAL: 'Il ne s\'agit pas d\'une version officielle d\'EMS-ESP. N\'installez que des micrologiciels auxquels vous faites confiance', // DeepL
  UPLOAD_NOT_OFFICIAL_HELP: 'À propos des versions officielles et personnalisées',
  UPLOAD_INSTALL_ANYWAY: 'Installez-le quand même', // DeepL
  OFFICIAL_BUILD: 'Version Officielle',
  CUSTOM_BUILD: 'Version Personnalisée', // DeepL
  USING_CUSTOM_FIRMWARE: 'Vous utilisez un micrologiciel personnalisé', // DeepL
  PASSWORD_CONFIRM_TITLE: 'Veuillez saisir à nouveau votre mot de passe', // DeepL
  PASSWORD_CONFIRM_TEXT: 'Pour votre sécurité, veuillez saisir votre mot de passe avant d\'effectuer cette modification. Il ne vous sera plus demandé avant 5 minutes.', // DeepL
  PASSWORD_CONFIRM_BUTTON: 'Confirmer',
  PASSWORD_WRONG: 'Mot de passe incorrect',
  PASSWORD_NOT_CONFIRMED: "Annulé, le mot de passe n'a pas été saisi",
  RECOVERY_APP: 'Application de récupération',
  RECOVERY_NOT_INSTALLED: 'Non installé',
  RECOVERY_INSTALLED: 'Firmware de récupération installé',
  RESTART_INTO_RECOVERY: 'Redémarrage dans l\'application de récupération', // DeepL
  RECOVERY_NOT_FOUND: 'L\'application de récupération est introuvable en ligne', // DeepL
  RECOVERY_START_FAILED: 'L\'application de récupération ne peut pas être lancée ; elle est peut-être obsolète ou endommagée. Veuillez d\'abord installer la dernière version', // DeepL
  PARTITION_START_FAILED: 'Ce micrologiciel ne peut pas être lancé ; il est peut-être endommagé', // DeepL
  INSTALLED_VERSIONS: 'Versions installées',
  EMSESP_APP: 'Application EMS-ESP',
  NO_UPDATES_AVAILABLE: 'Aucune mise à jour disponible', // DeepL
  SWITCH_TO_DEV: 'Passer en mode dev {0}', // DeepL
  SWITCH_TO_STABLE: 'Passer à la version stable {0}',
  SAVE: 'Enregistrer',
  NEEDS_RESTART: 'Un redémarrage est nécessaire', // DeepL
  CHANGED_SETTINGS: '{num} paramètre{{s}} modifié{{s}}',
  SAVED_RESTART: 'Enregistré. Redémarrez EMS-ESP pour que les nouveaux paramètres soient pris en compte.', // DeepL
  RESTART_NOW: 'Redémarrer maintenant',
  COPY_FAILED: 'Impossible de copier ; veuillez sélectionner le texte et le copier vous-même', // DeepL
  DOCUMENTATION: 'Documentation',
  COMMUNITY_CHAT: 'Chat communautaire', // DeepL
  REPORT_ISSUE: 'Signaler un bug ou demander une fonctionnalité', // DeepL
  GET_HELP: 'Obtenir de l\'aide',
  REPORT_PROBLEM: 'Signaler un problème',
  REPORT_STEP_1: 'Télécharger les informations système', // DeepL
  REPORT_STEP_1_TEXT: 'version, carte, état du bus EMS, appareils connectés et paramètres, sans mots de passe', // DeepL
  REPORT_STEP_2: 'Enregistrer le journal récent', // DeepL
  REPORT_STEP_2_TEXT: 'Réglez d\'abord le niveau de journalisation sur DEBUG si le problème concerne un Appareil', // DeepL
  REPORT_STEP_3: 'Créez un ticket ou posez votre question sur Discord', // DeepL
  REPORT_STEP_3_TEXT: 'Joignez les deux fichiers, ou copiez-collez le bref résumé ci-dessous dans une conversation', // DeepL
  NEW_ISSUE: 'Nouveau problème', // DeepL
  SHORT_SUMMARY: 'Bref résumé', // DeepL
  LICENSE_TEXT: 'Logiciel libre sous licence LGPL-3.0', // DeepL
  ADMIN_ACCESS_TEXT: 'Accès complet aux paramètres, aux Appareils et à l\'API', // DeepL
  GUEST_ACCESS_TEXT: 'peut consulter les valeurs et exécuter les Commandes que les invités peuvent exécuter', // DeepL
  LANGUAGE_BROWSER_ONLY: 'Cela ne concerne que ce navigateur', // DeepL
  LANGUAGE_ENTITIES_HINT: 'La langue utilisée pour les noms des entités des appareils dans MQTT et l’API est définie dans', // DeepL
  ADMINISTRATION: 'Administration',
  API_KEYS_SHORT: 'Clés pour Home Assistant et scripts', // DeepL
  ADDRESSES: 'Adresses',
  SEARCH_SETTINGS: 'Paramètres de recherche', // DeepL
  NO_SETTINGS_MATCH: 'Aucun paramètre ne correspond à votre recherche',
  GENERAL: 'Généralités', // DeepL
  BOARD: 'Carte',
  ADVANCED: 'Avancé',
  EMS_BUS_DESC: 'Comment EMS-ESP communique avec votre système de chauffage.',
  BOARD_DESC: 'Les broches proviennent du profil de la carte. Pour les configurer vous-même, activez le mode Développeur et sélectionnez « Personnalisé ».', // DeepL
  FORMAT_DESC: 'Présentation des valeurs dans l\'interface utilisateur Web et dans les charges utiles MQTT/API.', // DeepL
  BOARD_PINS: 'Épingles du profil', // DeepL
  ENABLE_EMAIL: 'Activer les notifications par e-mail',
  SEND_TEST_EMAIL: 'Envoyer un e-mail de test',
  EMAIL_FROM: 'Source :', // DeepL
  EMAIL_TO: 'À',
  EMAIL_SUBJECT: 'Objet',
  WIFI_SCAN: 'Recherche de réseaux Wi-Fi', // DeepL
  SCAN: 'Analyser', // DeepL
  NETWORKS_NEARBY: 'Réseaux à proximité',
  SIGNAL: 'Signal',
  CHANNEL: 'Canal',
  WIFI_OPEN: 'ouvrir', // DeepL
  WIFI_SAVED: 'enregistré',
  NETWORK_ETHERNET_INFO: 'Connexion via Ethernet ({ip}). Le Wi-Fi n\'est utilisé qu\'en l\'absence de câble.', // DeepL
  HOSTNAME_ACCESS: 'Nom d\'hôte et accès',
  IP_ADDRESS: 'Adresse IP',
  NETWORK_ETH_10MBIT: 'Force 10 Mbit semi-duplex', // DeepL
  NETWORK_ETH_10MBIT_HELP: 'uniquement pour les interrupteurs défectueux ou les câbles longs', // DeepL
  NETWORK_DISABLE_SLEEP_HELP: 'plus fiable, consomme un peu plus d\'énergie', // DeepL
  NETWORK_MDNS_HELP: 'Vous pouvez y accéder via {host}.local avec mDNS', // DeepL
  NETWORK_FIXED_IP_HELP: 'Sinon, le protocole DHCP est utilisé', // DeepL
  NETWORK_CORS_HELP: 'Nécessaire uniquement lorsqu\'une autre page Web appelle l\'API', // DeepL
  NTP_TIME_ON_DEVICE: 'Temps passé sur EMS-ESP', // DeepL
  NTP_TIME_SERVER: 'Serveur de temps',
  NTP_THERMOSTAT_CLOCK: 'Horloge du thermostat',
  NTP_THERMOSTAT_SYNC: 'Réglez l\'horloge du thermostat', // DeepL
  NTP_THERMOSTAT_SYNC_HELP: 'règle la date et l\'heure sur le thermostat EMS', // DeepL
  NTP_THERMOSTAT_SYNC_1: 'Activé, fuseau horaire EMS-ESP', // DeepL
  NTP_THERMOSTAT_SYNC_2: 'Activé, fuseau horaire local', // DeepL
  NTP_THERMOSTAT_TZ: 'Fuseau horaire du thermostat',
  MQTT_PREVIEW: 'Ce qui est publié',
  MQTT_PREVIEW_CMD: 'accepté comme commande',
  MQTT_PREVIEW_RESPONSE: 'résultats des commandes', // DeepL
  MQTT_BEHAVIOUR: 'Comportement',
  MQTT_TOPICS: 'Thèmes et contenus', // DeepL
  MQTT_DISCOVERY: 'Découverte',
  MQTT_INTERVALS_HELP: 'Intervalle en secondes entre deux publications ; 0 = publication à chaque modification.', // DeepL
  MQTT_SINGLE_NO_DISCOVERY: 'non disponible avec la fonctionnalité de découverte sur', // DeepL
  MQTT_DISCOVERY_NO_SINGLE: 'non disponible avec les sujets à valeur unique sur', // DeepL
  MQTT_DISCOVERY_HELP: 'crée automatiquement les entités dans Home Assistant ou Domoticz',
  MQTT_ENTITY_FORMAT_WARN: 'La modification de ce paramètre entraîne le changement de nom de toutes les entités dans Home Assistant', // DeepL
  USERS: 'Utilisateurs',
  USERS_TEXT: 'Les administrateurs peuvent tout modifier. Les invités peuvent consulter les informations et exécuter les Commandes qui leur sont réservées.', // DeepL
  ADD_USER: 'Ajouter un utilisateur',
  EDIT_USER: 'Modifier l\'utilisateur',
  ROLE: 'Rôle',
  ROLE_ADMIN: 'admin',
  ROLE_GUEST: 'invité',
  ADMIN: 'Admin', // DeepL
  YOU: 'vous',
  NEW_PASSWORD: 'Nouveau mot de passe',
  UNSAVED_CHANGES: '{num} modification{{s}} non enregistrée{{s}}',
  CREATE_KEY: 'Créer une Clé',
  NEW_API_KEY: 'Nouvelle Clé API',
  NO_API_KEYS: 'Pas encore de clés API', // DeepL
  API_KEYS_LIMIT: 'Vous pouvez disposer d\'un maximum de {num} touches.', // DeepL
  REVOKE: 'Révoquer',
  REVOKE_KEY: 'Révoquer la Clé API',
  REVOKE_KEY_TEXT: 'Tout élément contenant « {name} » cesse immédiatement de fonctionner.', // DeepL
  MODULES_TEXT: 'Fonctionnalités optionnelles intégrées à ce micrologiciel. Vous pouvez les activer ou les désactiver ici.', // DeepL
  BY_AUTHOR: 'par {name}',
  LICENSE_KEY: 'Clé de licence',
  MODULE_ACTIVATED: 'activé',
  MODULE_PENDING: 'en attente d\'activation', // DeepL
  ENABLED: 'Activé',
  UNDO: 'Annuler',
  FW_UPDATES: 'Mises à jour',
  FW_VERSION_AVAILABLE: 'La {version} est disponible', // DeepL
  FW_RELEASE_NOTES: 'Notes de mise à jour', // DeepL
  FW_UPDATES_OFFLINE: 'Impossible de vérifier en ligne la disponibilité de nouvelles versions. EMS-ESP a besoin d\'une connexion Internet pour cela.', // DeepL
  FW_RESTART_RESET: 'Redémarrage et Réinitialisation',
  FW_RECOVERY_TEXT: 'Redémarrer dans l’application de récupération ? Elle se connecte à votre réseau et lance également son propre point d’accès, sur lequel vous pouvez téléverser le micrologiciel.',
  FW_SIZE: 'Taille',
  FW_INSTALL_DATE: 'Installé sur', // DeepL
  FW_UPLOAD_FAILED: 'Échec du téléversement : type de fichier incorrect ou micrologiciel destiné à une autre carte',
  FILE_TYPE_INVALID: 'Ce type de fichier n\'est pas pris en charge, veuillez choisir un fichier {types}', // DeepL
  WHO_CAN_RUN: 'Qui peut exécuter', // DeepL
  CMD_NONE: 'Aucune commande trouvée',
  CMD_NONE_MATCH: 'Aucune commande ne correspond à votre recherche', // DeepL
  SCHEDULE_NONE: 'Aucun programme trouvé', // DeepL
  SCHEDULE_NONE_MATCH: 'Aucun programme ne correspond à votre recherche', // DeepL
  CE_NONE: 'Aucune entité trouvée',
  CE_ALL: 'Tous',
  CE_STORAGE: 'Stockage',
  CE_SOURCE: 'Source',
  CE_EMS_SOURCE: 'appareil {device} · télégramme {type} · décalage {offset}',
  CE_RAM_TEXT: 'conservé jusqu\'au redémarrage',
  CE_NVS_TEXT: 'conservés d\'un redémarrage à l\'autre', // DeepL
  CE_EXCLUDED: 'pas dans l\'API/MQTT', // DeepL
  SEARCH_SENSORS: 'Rechercher un nom, un identifiant ou un GPIO', // DeepL
  NO_SENSORS: 'Aucun capteur détecté', // DeepL
  NO_SENSORS_MATCH: 'Aucun capteur ne correspond à votre recherche', // DeepL
  TEMP_SENSORS_HELP: 'Détecté automatiquement. Cliquez sur un capteur pour le renommer ou définir un décalage', // DeepL
  FREE_GPIOS: 'GPIO libres',
  SYSTEM_SENSOR: 'Capteur du système', // DeepL
  MASK_FAVORITE: 'Favoris', // DeepL
  MASK_READONLY: 'Rédaction pour les personnes en situation de handicap', // DeepL
  MASK_NO_API: 'Pas de MQTT/API', // DeepL
  MASK_HIDDEN: 'Masqué',
  MASK_REMOVED: 'Supprimé',
  FILTER_ALL: 'Tous',
  NUM_SELECTED: 'Sélectionné : {num}', // DeepL
  ALL_SHOWN: 'Tout afficher : {num}', // DeepL
  BULK_HINT: 'Les boutons permettent de modifier les lignes cochées, ou d\'afficher toutes les lignes lorsqu\'aucune n\'est cochée.', // DeepL
  CLEAR_SELECTION: 'Sélection claire', // DeepL
  NO_ENTITIES_MATCH: 'Aucune entité ne correspond',
  LIMIT_HELP: 'Laissez ce champ vide pour la limite intégrée', // DeepL
  MIN_ABOVE_MAX: 'La valeur minimale ne doit pas dépasser la valeur maximale', // DeepL
  RENAME_DEVICE: 'Renommer l\'Appareil', // DeepL
  WEEKLY: 'Hebdomadaire',
  EVERY_DAY: 'Chaque jour', // DeepL
  SCHEDULER_NO_COMMANDS: 'Commencez par ajouter une commande sur la page « Commandes »', // DeepL
  DU_SETTINGS: 'Paramètres d\'EMS-ESP', // DeepL
  DU_SETTINGS_TEXT: 'Sauvegarde des paramètres d’EMS-ESP : réseau, MQTT, personnalisation, Scheduler, entités et modules personnalisés. Le téléversement d’un fichier de paramètres ou de sauvegarde redémarre EMS-ESP.',
  HB_TITLE: 'Paramètres de chauffage',
  HB_TEXT: 'Enregistre les valeurs actuelles de votre chaudière, de votre thermostat et des autres appareils EMS, afin que vous puissiez les rétablir ultérieurement, par exemple après le remplacement d’un appareil. Les entités et Commandes en lecture seule ne sont jamais modifiées par une restauration.', // DeepL
  HB_BACKUP: 'Sauvegarder les Paramètres de Chauffage',
  HB_RESTORE: 'Restaurer les paramètres de chauffage',
  DATA_EXPORT: 'Exportation des données', // DeepL
  DATA_EXPORT_TEXT: 'À des fins d\'assistance et d\'analyse. Ces fichiers ne peuvent pas être réimportés.', // DeepL
  NUM_VALUES: '{num} valeur{{s}}',
  HB_NOTHING: 'rien à sauvegarder',
  FILE_NAME: 'Nom de fichier', // DeepL
  HB_NOTE: 'Remarque (facultative, affichée à nouveau lors de la restauration)', // DeepL
  HB_INCOMPLETE: 'Les valeurs qui n’ont pas encore été lues sur le bus ne sont pas affichées. Veuillez patienter quelques minutes après un redémarrage pour obtenir une sauvegarde complète.', // DeepL
  HB_RESTORE_TEXT: 'Choisissez une sauvegarde des réglages de chauffage. Aucune modification n\'est enregistrée tant que vous n\'avez pas vérifié les changements et validé votre choix.', // DeepL
  HB_DROP: 'Cliquez pour sélectionner, ou déposez ici un fichier de sauvegarde du chauffage (.json)', // DeepL
  HB_INVALID: 'Il ne s\'agit pas d\'une sauvegarde des réglages de chauffage', // DeepL
  HB_OTHER_FILE: 'Choisissez un autre fichier', // DeepL
  HB_SAME_DEVICE: 'même appareil',
  HB_NOT_CONNECTED: 'non connecté',
  HB_OTHER_DEVICE: 'autre appareil (ID produit {backup}en mode secours, {now} connecté), ignoré', // DeepL
  HB_UNIT_MISMATCH: 'Cette sauvegarde contient des températures exprimées en {unit}. Modifiez l\'unité de température dans les paramètres de l\'application avant de la restaurer.', // DeepL
  HB_CHANGED: 'Modifié', // DeepL
  ALL: 'Tous', // DeepL
  HB_NOW: 'À présent', // DeepL
  HB_BACKUP_VALUE: 'Sauvegarde',
  HB_NOT_AVAILABLE: 'non disponible',
  HB_SENDING: 'envoi',
  HB_WRITTEN: 'rédigé', // DeepL
  HB_RESTORE_NUM: 'Restaurer {num} valeur{{s}}',
  RESTORE: 'Restaurer',
  HB_CONFIRM: 'Écrire {num} valeur{{s}} de {file} dans les appareils EMS ?',
  HB_CONFIRM_TEXT: 'Les valeurs sont envoyées une par une. Si un appareil en rejette une, les autres sont tout de même envoyées.', // DeepL
  HB_DONE: '{num} valeur{{s}} écrite{{s}}',
  HB_DONE_FAILED: '{num} a échoué. Les Appareils refusent souvent les valeurs définies au niveau de l\'installateur ; le journal système contient les détails.', // DeepL
  HB_NO_CHANGES: 'Rien ne diffère de la version de sauvegarde', // DeepL
  HB_DIFFERENT_DEVICE: 'Appareil différent', // DeepL
  SELECT_ALL: 'Tout sélectionner',
  DASHBOARD_EMPTY: 'Rien à afficher pour l\'instant',
  NO_DEVICES: 'Aucun appareil EMS n\'a encore été détecté', // DeepL
  ADD_COMMAND: 'Ajouter une commande',
  EDIT_COMMAND: 'Commande « Modifier »', // DeepL
  RUN_COMMAND: 'Exécuter la commande',
  ADD_SCHEDULE: 'Ajouter un programme',
  EDIT_SCHEDULE: 'Modifier le programme',
  ADD_ENTITY: 'Ajouter une entité',
  EDIT_ENTITY: 'Modifier l\'Entité',
  ADD_ANALOG_SENSOR: 'Ajouter un capteur analogique',
  EDIT_ANALOG_SENSOR: 'Modifier un capteur analogique', // DeepL
  REMOVE_ANALOG_SENSOR: 'Retirer le Capteur Analogique', // DeepL
  REMOVE_SENSOR_TEXT: 'Le capteur « {name} » est immédiatement supprimé.', // DeepL
  EDIT_TEMP_SENSOR: 'Modifier le Capteur de Température',
  INSTALL_FIRMWARE: 'Installer le Micrologiciel ({channel})', // DeepL
  PARTITION: 'Partition',
  PULSE: 'Pulse', // DeepL
  SYS_HOME: 'Accueil', // draft, please review
  SYS_HOME_HELP: 'L\'essentiel en un coup d\'œil. Tout le reste se trouve dans le menu Système.', // draft, please review
  SYS_MENU: 'Menu Système', // draft, please review
  SYS_MENU_HELP: 'Toutes les valeurs et tous les réglages de votre installation, classés par thème.', // draft, please review
  SYS_FAV_HELP: 'Vos réglages les plus importants. Utilisez l’étoile à côté d’une valeur pour modifier cette liste.', // draft, please review
  SYS_FAV_NONE: 'Aucun favori pour l’instant. Marquez des valeurs avec l’étoile.', // draft, please review
  SYS_FAV_REMOVED: 'Retiré des favoris', // draft, please review
  SYS_GROUP_HEATING: 'Chauffage', // draft, please review
  SYS_GROUP_PLANT: 'Installation', // draft, please review
  SYS_GROUP_OVERVIEW: 'Aperçu', // draft, please review
  SYS_LEVEL_MOVE_ALL: 'Afficher seulement dans toutes les valeurs', // draft, please review
  SYS_LEVEL_MOVE_NORMAL: 'Afficher dans la vue normale', // draft, please review
  SYS_HC: 'Circuit de chauffage {0}', // draft, please review
  SYS_DHW: 'Eau chaude', // draft, please review
  SYS_AHS: 'Chauffage d’appoint', // draft, please review
  SYS_HS: 'Source de chaleur', // draft, please review
  SYS_SRC: 'Source', // draft, please review
  SYS_OTHER_DEVICES: 'Autres appareils', // draft, please review
  SYS_SERVICE: 'Maintenance et défauts', // draft, please review
  SYS_ENERGY: 'Énergie et durées de fonctionnement', // draft, please review
  SYS_SEC_MODE: 'Fonctionnement', // draft, please review
  SYS_SEC_TEMPS: 'Températures', // draft, please review
  SYS_SEC_STATUS: 'État', // draft, please review
  SYS_SEC_ADVANCED: 'Courbe de chauffe et installation', // draft, please review
  SYS_SEC_MEASURE: 'Valeurs mesurées', // draft, please review
  SYS_SEC_OTHER: 'Autres valeurs', // draft, please review
  SYS_LEVEL_NORMAL: 'Normal', // draft, please review
  SYS_NO_RESULTS: 'Aucun résultat. Essayez toutes les valeurs ou un autre terme.', // draft, please review
  SYS_NOTHING_HERE: 'Rien à afficher ici au niveau Normal.', // draft, please review
  SYS_HIDDEN: 'Valeurs expert masquées : {num}', // draft, please review
  SYS_SEARCH_RESULTS: 'Résultats de la recherche', // draft, please review
  SYS_CHANGED_WAS: 'modifié (avant : {0})', // draft, please review
  SYS_NO_DEVICES: 'Aucun appareil trouvé pour l’instant.', // draft, please review
  SYS_ROOM_TEMP: 'Température ambiante', // draft, please review
  SYS_WISH: 'Consigne', // draft, please review
  SYS_CURRENT: 'Température actuelle', // draft, please review
  SYS_OUTDOOR: 'Température extérieure', // draft, please review
  SYS_FLOW: 'Départ', // draft, please review
  SYS_RETURN: 'Retour', // draft, please review
  SYS_PRESSURE: 'Pression de l’installation', // draft, please review
  SYS_POWER: 'Puissance', // draft, please review
  SYS_BURNER_ON: 'Brûleur en marche', // draft, please review
  SYS_BURNER_OFF: 'Brûleur éteint', // draft, please review
  SYS_DHW_HEATING: 'En cours de chauffe', // draft, please review
  SYS_READY: 'Prêt', // draft, please review
  SYS_ONE_TIME: 'Charge unique', // draft, please review
  SYS_NOTES: 'Remarques', // draft, please review
  SYS_NOTES_SUB: 'Défauts et maintenance', // draft, please review
  SYS_LAST_FAULT: 'Dernier défaut', // draft, please review
  SYS_MAINT_MSG: 'Message de maintenance', // draft, please review
  SYS_NEXT_MAINT: 'Prochaine maintenance', // draft, please review
  SYS_FAULT_LAST: 'Dernier défaut / message', // draft, please review
  SYS_FAULT_CTRL: 'Dernier défaut du régulateur', // draft, please review
  SYS_FAULT_SERVICE: 'Code de service', // draft, please review
  SYS_FAULT_ADD: 'Code supplémentaire {0}', // draft, please review
  SYS_FAULT_RAW_ONLY: 'Aucune explication disponible pour cet appareil.', // draft, please review
  SYS_FAULT_NONE: 'Aucun défaut enregistré.', // draft, please review
  SYS_FAULT_UNTIL_NOW: 'jusqu’à maintenant', // draft, please review
  SYS_MAINTENANCE: 'Maintenance', // draft, please review
};

export default fr;
