import Schema from 'async-validator';
import type { InternalRuleItem } from 'async-validator';
import type { UserType } from 'types';

const USERNAME_PATTERN = /^[a-zA-Z0-9_\\.]{1,24}$/;
const JWT_SECRET_MAX_LENGTH = 64;
const PASSWORD_MAX_LENGTH = 64;

export const SECURITY_SETTINGS_VALIDATOR = new Schema({
  jwt_secret: [
    { required: true, message: 'JWT secret is required' },
    {
      type: 'string',
      min: 1,
      max: JWT_SECRET_MAX_LENGTH,
      message: `JWT secret must be between 1 and ${JWT_SECRET_MAX_LENGTH} characters`
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
    password: [
      { required: true, message: 'Please provide a password' },
      {
        type: 'string',
        min: 1,
        max: PASSWORD_MAX_LENGTH,
        message: `Password must be 1-${PASSWORD_MAX_LENGTH} characters`
      }
    ]
  });
