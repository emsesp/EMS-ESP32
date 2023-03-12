import Schema, { InternalRuleItem } from 'async-validator';
import { User } from 'types';

export const SECURITY_SETTINGS_VALIDATOR = new Schema({
  jwt_secret: [
    { required: true, message: 'JWT secret is required' },
    { type: 'string', min: 1, max: 64, message: 'JWT secret must be between 1 and 64 characters' }
  ]
});

export const createUserValidator = (users: User[], creating: boolean) =>
  new Schema({
    username: [
      { required: true, message: 'Username is required' },
      {
        type: 'string',
        pattern: /^[a-zA-Z0-9_\\.]{1,24}$/,
        message: "Must be 1-24 characters: alpha numeric, '_' or '.'"
      },
      ...(creating ? [createUniqueUsernameValidator(users)] : [])
    ],
    password: [
      { required: true, message: 'Please provide a password' },
      { type: 'string', min: 1, max: 64, message: 'Password must be 1-64 characters' }
    ]
  });

export const createUniqueUsernameValidator = (users: User[]) => ({
  validator(rule: InternalRuleItem, username: string, callback: (error?: string) => void) {
    if (username && users.find((u) => u.username === username)) {
      callback('Username already in use');
    } else {
      callback();
    }
  }
});
