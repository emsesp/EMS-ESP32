import type { SetupStateType, SetupType, UserType } from 'types';
import Schema from 'validators/schema';
import type { InternalRuleItem } from 'validators/schema';

const USERNAME_PATTERN = /^[a-zA-Z0-9_\\.]{1,24}$/;
const JWT_SECRET_MAX_LENGTH = 64;
const PASSWORD_MAX_LENGTH = 64;

export const SECURITY_SETTINGS_VALIDATOR = new Schema({
  // empty keeps the current password
  jwt_secret: [
    {
      type: 'string',
      max: JWT_SECRET_MAX_LENGTH,
      message: `Password must be at most ${JWT_SECRET_MAX_LENGTH} characters`
    }
  ]
});

/**
 * Creates a validator to ensure username uniqueness
 * @param users - Array of existing users to check against
 * @returns Validator rule for unique username
 */
export const createUniqueUsernameValidator = (users: UserType[]) => ({
  validator(
    _rule: InternalRuleItem,
    username: string,
    callback: (error?: string) => void
  ) {
    if (username && users.some((u) => u.username === username)) {
      callback('Username already in use');
    } else {
      callback();
    }
  }
});

/**
 * Creates a validator schema for user creation/editing
 * @param users - Array of existing users for uniqueness check
 * @param creating - Whether this is for creating a new user (enables uniqueness check)
 * @returns Schema validator for user data
 */
export const createUserValidator = (users: UserType[], creating: boolean) =>
  new Schema({
    username: [
      { required: true, message: 'Username is required' },
      {
        type: 'string',
        pattern: USERNAME_PATTERN,
        message: "Must be 1-24 characters: alphanumeric, '_' or '.'"
      },
      ...(creating ? [createUniqueUsernameValidator(users)] : [])
    ],
    // an existing user keeps the current password when it's left empty
    password: [
      ...(creating
        ? [{ required: true, message: 'Please provide a password' }]
        : []),
      {
        type: 'string',
        max: PASSWORD_MAX_LENGTH,
        message: `Password must be at most ${PASSWORD_MAX_LENGTH} characters`
      }
    ]
  });

const notFactory = (factory: string) => ({
  validator(
    _rule: InternalRuleItem,
    value: string,
    callback: (error?: string) => void
  ) {
    callback(value === factory ? 'Choose a different password' : undefined);
  }
});

const AP_PASSWORD_RULE = {
  type: 'string' as const,
  min: 8,
  max: 63,
  message: 'Password must be 8-63 characters'
};

export const createSetupValidator = (state: SetupStateType, setup: SetupType) =>
  new Schema({
    ...(state.admin && {
      password: [
        { required: true, message: 'Please provide a password' },
        {
          type: 'string',
          max: PASSWORD_MAX_LENGTH,
          message: `Password must be at most ${PASSWORD_MAX_LENGTH} characters`
        },
        notFactory('admin')
      ]
    }),
    ...(state.su && {
      su_password: [
        { required: true, message: 'Please provide a password' },
        {
          type: 'string',
          max: JWT_SECRET_MAX_LENGTH,
          message: `Password must be at most ${JWT_SECRET_MAX_LENGTH} characters`
        },
        notFactory('ems-esp-neo')
      ]
    }),
    ...(state.ap && {
      ap_password: [
        ...(state.ap_required
          ? [{ required: true, message: 'Please provide an access point password' }]
          : []),
        AP_PASSWORD_RULE,
        notFactory('ems-esp-neo')
      ]
    }),
    view_username: [
      {
        type: 'string',
        pattern: USERNAME_PATTERN,
        message: "Must be 1-24 characters: alphanumeric, '_' or '.'"
      }
    ],
    ...(setup.view_username && {
      view_password: [{ required: true, message: 'Please provide a password' }]
    })
  });
