import type { InternalRuleItem, ValidateOption } from 'async-validator';
import type Schema from 'async-validator';

export const validate = <T extends object>(
  validator: Schema,
  source: Partial<T>,
  options?: ValidateOption
): Promise<T> =>
  new Promise((resolve, reject) => {
    void validator.validate(source, options || {}, (errors, fieldErrors) => {
      if (errors) {
        reject(fieldErrors as Error);
      } else {
        resolve(source as T);
      }
    });
  });

//  updated to support both IPv4 and IPv6
const IP_ADDRESS_REGEXP =
  /((^\s*((([0-9]|[1-9][0-9]|1[0-9]{2}|2[0-4][0-9]|25[0-5])\.){3}([0-9]|[1-9][0-9]|1[0-9]{2}|2[0-4][0-9]|25[0-5]))\s*$)|(^\s*((([0-9a-f]{1,4}:){7}([0-9a-f]{1,4}|:))|(([0-9a-f]{1,4}:){6}(:[0-9a-f]{1,4}|((25[0-5]|2[0-4]\d|1\d\d|[1-9]?\d)(\.(25[0-5]|2[0-4]\d|1\d\d|[1-9]?\d)){3})|:))|(([0-9a-f]{1,4}:){5}(((:[0-9a-f]{1,4}){1,2})|:((25[0-5]|2[0-4]\d|1\d\d|[1-9]?\d)(\.(25[0-5]|2[0-4]\d|1\d\d|[1-9]?\d)){3})|:))|(([0-9a-f]{1,4}:){4}(((:[0-9a-f]{1,4}){1,3})|((:[0-9a-f]{1,4})?:((25[0-5]|2[0-4]\d|1\d\d|[1-9]?\d)(\.(25[0-5]|2[0-4]\d|1\d\d|[1-9]?\d)){3}))|:))|(([0-9a-f]{1,4}:){3}(((:[0-9a-f]{1,4}){1,4})|((:[0-9a-f]{1,4}){0,2}:((25[0-5]|2[0-4]\d|1\d\d|[1-9]?\d)(\.(25[0-5]|2[0-4]\d|1\d\d|[1-9]?\d)){3}))|:))|(([0-9a-f]{1,4}:){2}(((:[0-9a-f]{1,4}){1,5})|((:[0-9a-f]{1,4}){0,3}:((25[0-5]|2[0-4]\d|1\d\d|[1-9]?\d)(\.(25[0-5]|2[0-4]\d|1\d\d|[1-9]?\d)){3}))|:))|(([0-9a-f]{1,4}:){1}(((:[0-9a-f]{1,4}){1,6})|((:[0-9a-f]{1,4}){0,4}:((25[0-5]|2[0-4]\d|1\d\d|[1-9]?\d)(\.(25[0-5]|2[0-4]\d|1\d\d|[1-9]?\d)){3}))|:))|(:(((:[0-9a-f]{1,4}){1,7})|((:[0-9a-f]{1,4}){0,5}:((25[0-5]|2[0-4]\d|1\d\d|[1-9]?\d)(\.(25[0-5]|2[0-4]\d|1\d\d|[1-9]?\d)){3}))|:)))(%.+)?\s*$))/;

const isValidIpAddress = (value: string) => IP_ADDRESS_REGEXP.test(value);

export const IP_ADDRESS_VALIDATOR = {
  validator(
    rule: InternalRuleItem,
    value: string,
    callback: (error?: string) => void
  ) {
    if (value && !isValidIpAddress(value)) {
      callback('Must be an IP address');
    } else {
      callback();
    }
  }
};

const HOSTNAME_LENGTH_REGEXP = /^.{0,200}$/;
const HOSTNAME_PATTERN_REGEXP =
  /^(([a-zA-Z0-9]|[a-zA-Z0-9][a-zA-Z0-9-]*[a-zA-Z0-9])\.)*([A-Za-z0-9]|[A-Za-z0-9][A-Za-z0-9-]*[A-Za-z0-9])$/;

const isValidHostname = (value: string) =>
  HOSTNAME_LENGTH_REGEXP.test(value) && HOSTNAME_PATTERN_REGEXP.test(value);

export const HOSTNAME_VALIDATOR = {
  validator(
    rule: InternalRuleItem,
    value: string,
    callback: (error?: string) => void
  ) {
    if (value && !isValidHostname(value)) {
      callback('Must be a valid hostname');
    } else {
      callback();
    }
  }
};

export const IP_OR_HOSTNAME_VALIDATOR = {
  validator(
    rule: InternalRuleItem,
    value: string,
    callback: (error?: string) => void
  ) {
    if (value && !(isValidIpAddress(value) || isValidHostname(value))) {
      callback('Must be a valid IP address or hostname');
    } else {
      callback();
    }
  }
};
