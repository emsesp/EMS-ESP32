// This file was auto-generated by 'typesafe-i18n'. Any manual changes will be overwritten.
/* eslint-disable */
import type { BaseTranslation as BaseTranslationType, LocalizedString, RequiredParams } from 'typesafe-i18n'

export type BaseTranslation = BaseTranslationType
export type BaseLocale = 'en'

export type Locales =
	| 'de'
	| 'en'

export type Translation = RootTranslation

export type Translations = RootTranslation

type RootTranslation = {
	/**
	 * Language
	 */
	LANGUAGE: string
	/**
	 * is required
	 */
	IS_REQUIRED: string
	/**
	 * Sign In
	 */
	SIGN_IN: string
	/**
	 * Username
	 */
	USERNAME: string
	/**
	 * Password
	 */
	PASSWORD: string
	/**
	 * Dashboard
	 */
	DASHBOARD: string
	/**
	 * Settings
	 */
	SETTINGS: string
	/**
	 * Help
	 */
	HELP: string
	/**
	 * Logged in as {name}
	 * @param {unknown} name
	 */
	LOGGED_IN: RequiredParams<'name'>
	/**
	 * Please sign in to continue
	 */
	PLEASE_SIGNIN: string
	/**
	 * Upload successful
	 */
	UPLOAD_SUCCESSFUL: string
	/**
	 * Invalid login details
	 */
	INVALID_LOGIN: string
	/**
	 * Network Connection
	 */
	NETWORK_CONNECTION: string
	/**
	 * Security
	 */
	SECURITY: string
	/**
	 * Network Time
	 */
	NETWORK_TIME: string
	/**
	 * ON/OFF
	 */
	ONOFF_CAP: string
	/**
	 * on/off
	 */
	ONOFF: string
	/**
	 * Type
	 */
	TYPE: string
	/**
	 * Description
	 */
	DESCRIPTION: string
	/**
	 * Entities
	 */
	ENTITIES: string
	/**
	 * Refresh
	 */
	REFRESH: string
	/**
	 * Export
	 */
	EXPORT: string
	/**
	 * Entity Name
	 */
	ENTITY_NAME: string
	/**
	 * Value
	 */
	VALUE: string
	/**
	 * only show favorites
	 */
	SHOW_FAV: string
	/**
	 * Device and Sensor Data
	 */
	DEVICE_SENSOR_DATA: string
	/**
	 * Devices & Sensors
	 */
	DEVICES_SENSORS: string
}

export type TranslationFunctions = {
	/**
	 * Language
	 */
	LANGUAGE: () => LocalizedString
	/**
	 * is required
	 */
	IS_REQUIRED: () => LocalizedString
	/**
	 * Sign In
	 */
	SIGN_IN: () => LocalizedString
	/**
	 * Username
	 */
	USERNAME: () => LocalizedString
	/**
	 * Password
	 */
	PASSWORD: () => LocalizedString
	/**
	 * Dashboard
	 */
	DASHBOARD: () => LocalizedString
	/**
	 * Settings
	 */
	SETTINGS: () => LocalizedString
	/**
	 * Help
	 */
	HELP: () => LocalizedString
	/**
	 * Logged in as {name}
	 */
	LOGGED_IN: (arg: { name: unknown }) => LocalizedString
	/**
	 * Please sign in to continue
	 */
	PLEASE_SIGNIN: () => LocalizedString
	/**
	 * Upload successful
	 */
	UPLOAD_SUCCESSFUL: () => LocalizedString
	/**
	 * Invalid login details
	 */
	INVALID_LOGIN: () => LocalizedString
	/**
	 * Network Connection
	 */
	NETWORK_CONNECTION: () => LocalizedString
	/**
	 * Security
	 */
	SECURITY: () => LocalizedString
	/**
	 * Network Time
	 */
	NETWORK_TIME: () => LocalizedString
	/**
	 * ON/OFF
	 */
	ONOFF_CAP: () => LocalizedString
	/**
	 * on/off
	 */
	ONOFF: () => LocalizedString
	/**
	 * Type
	 */
	TYPE: () => LocalizedString
	/**
	 * Description
	 */
	DESCRIPTION: () => LocalizedString
	/**
	 * Entities
	 */
	ENTITIES: () => LocalizedString
	/**
	 * Refresh
	 */
	REFRESH: () => LocalizedString
	/**
	 * Export
	 */
	EXPORT: () => LocalizedString
	/**
	 * Entity Name
	 */
	ENTITY_NAME: () => LocalizedString
	/**
	 * Value
	 */
	VALUE: () => LocalizedString
	/**
	 * only show favorites
	 */
	SHOW_FAV: () => LocalizedString
	/**
	 * Device and Sensor Data
	 */
	DEVICE_SENSOR_DATA: () => LocalizedString
	/**
	 * Devices & Sensors
	 */
	DEVICES_SENSORS: () => LocalizedString
}

export type Formatters = {}
