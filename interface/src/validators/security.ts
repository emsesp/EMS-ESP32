import type { SetupStateType, SetupType, UserType } from 'types';
import Schema from 'validators/schema';
import type { InternalRuleItem } from 'validators/schema';

import { vLL } from './messages';
import { required } from './shared';

const USERNAME_PATTERN = /^[a-zA-Z0-9_\\.]{1,24}$/;
const PASSWORD_MAX_LENGTH = 64;

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
      callback(vLL().V_IN_USE());
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
      required(),
      {
        type: 'string',
        pattern: USERNAME_PATTERN,
        message: vLL().V_USERNAME({ max: 24 })
      },
      ...(creating ? [createUniqueUsernameValidator(users)] : [])
    ],
    // an existing user keeps the current password when it's left empty
    password: [
      ...(creating ? [required()] : []),
      {
        type: 'string',
        max: PASSWORD_MAX_LENGTH,
        message: vLL().V_MAX_CHARS({ max: PASSWORD_MAX_LENGTH })
      }
    ]
  });

const notFactory = (factory: string) => ({
  validator(
    _rule: InternalRuleItem,
    value: string,
    callback: (error?: string) => void
  ) {
    callback(value === factory ? vLL().V_NOT_DEFAULT() : undefined);
  }
});

const AP_PASSWORD_RULE = () => ({
  type: 'string' as const,
  min: 8,
  max: 63,
  message: vLL().V_CHARS({ min: 8, max: 63 })
});

export const createSetupValidator = (state: SetupStateType, setup: SetupType) =>
  new Schema({
    ...(state.admin && {
      password: [
        required(),
        {
          type: 'string',
          max: PASSWORD_MAX_LENGTH,
          message: vLL().V_MAX_CHARS({ max: PASSWORD_MAX_LENGTH })
        },
        notFactory('admin')
      ]
    }),
    ...(state.ap && {
      ap_password: [
        ...(state.ap_required ? [required()] : []),
        AP_PASSWORD_RULE(),
        notFactory('ems-esp-neo')
      ]
    }),
    view_username: [
      {
        type: 'string',
        pattern: USERNAME_PATTERN,
        message: vLL().V_USERNAME({ max: 24 })
      }
    ],
    ...(setup.view_username && {
      view_password: [required()]
    })
  });
