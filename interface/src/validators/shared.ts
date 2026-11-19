import type {
  InternalRuleItem,
  ValidateFieldsError,
  ValidateOption
} from 'validators/schema';
import type Schema from 'validators/schema';

import { vLL } from './messages';

export class ValidationError extends Error {
  readonly fieldErrors: ValidateFieldsError;

  constructor(fieldErrors: ValidateFieldsError) {
    super('Validation failed');
    this.name = 'ValidationError';
    this.fieldErrors = fieldErrors;
  }
}

export const validate = <T extends object>(
  validator: Schema,
  source: Partial<T>,
  options?: ValidateOption
): Promise<T> =>
  new Promise((resolve, reject) => {
    void validator.validate(source, options ?? {}, (errors, fieldErrors) => {
      errors ? reject(new ValidationError(fieldErrors)) : resolve(source as T);
    });
  });

// IPv4 pattern: matches 0.0.0.0 to 255.255.255.255
const IPV4_PATTERN =
  /^((25[0-5]|2[0-4]\d|1\d{2}|[1-9]?\d)\.){3}(25[0-5]|2[0-4]\d|1\d{2}|[1-9]?\d)$/;

// IPv6 pattern: matches full and compressed IPv6 addresses (including IPv4-mapped)
const IPV6_PATTERN =
  /^(([0-9a-f]{1,4}:){7}[0-9a-f]{1,4}|([0-9a-f]{1,4}:){1,7}:|([0-9a-f]{1,4}:){1,6}:[0-9a-f]{1,4}|([0-9a-f]{1,4}:){1,5}(:[0-9a-f]{1,4}){1,2}|([0-9a-f]{1,4}:){1,4}(:[0-9a-f]{1,4}){1,3}|([0-9a-f]{1,4}:){1,3}(:[0-9a-f]{1,4}){1,4}|([0-9a-f]{1,4}:){1,2}(:[0-9a-f]{1,4}){1,5}|[0-9a-f]{1,4}:((:[0-9a-f]{1,4}){1,6})|:((:[0-9a-f]{1,4}){1,7}|:)|::)$/i;

// Hostname pattern: RFC 1123 compliant (max 200 chars)
const HOSTNAME_PATTERN =
  /^(?=.{1,200}$)(([a-z0-9]|[a-z0-9][a-z0-9-]*[a-z0-9])\.)*([a-z0-9]|[a-z0-9][a-z0-9-]*[a-z0-9])$/i;

const isValidIpAddress = (value: string): boolean =>
  IPV4_PATTERN.test(value.trim()) || IPV6_PATTERN.test(value.trim());

const isValidHostname = (value: string): boolean =>
  HOSTNAME_PATTERN.test(value.trim());

export const required = () => ({ required: true, message: vLL().V_REQUIRED() });

const createValidator = (
  validatorFn: (value: string) => boolean,
  errorMessage: () => string
) => ({
  validator(
    _rule: InternalRuleItem,
    value: string,
    callback: (error?: string) => void
  ) {
    callback(value && !validatorFn(value) ? errorMessage() : undefined);
  }
});

export const IP_ADDRESS_VALIDATOR = createValidator(isValidIpAddress, () =>
  vLL().V_IP()
);

export const HOSTNAME_VALIDATOR = createValidator(isValidHostname, () =>
  vLL().V_HOSTNAME()
);

export const IP_OR_HOSTNAME_VALIDATOR = createValidator(
  (value) => isValidIpAddress(value) || isValidHostname(value),
  () => vLL().V_IP_OR_HOSTNAME()
);
