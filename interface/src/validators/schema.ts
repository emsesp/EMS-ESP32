// Minimal drop-in replacement for the subset of `async-validator` used by this
// app's form validators. It intentionally mirrors async-validator's runtime
// semantics for the rule features in use (required, type string/number with
// min/max/pattern, and custom `validator(rule, value, callback)` functions) so
// the existing schema definitions and the `ValidateFieldsError` consumers keep
// working unchanged.
//
// Notable async-validator semantics preserved:
//   - A non-required built-in rule is skipped when the value is "empty"
//     (undefined / null / ''); `0` is NOT empty.
//   - Custom `validator` functions are always invoked (they guard empties
//     themselves), matching async-validator's behavior for validator rules.
//   - `min`/`max` mean numeric bounds for `type: 'number'` and length bounds
//     otherwise.
//   - All rule errors for a field are collected (no early-exit per field).

export interface ValidateError {
  message?: string;
  field?: string;
  fieldValue?: unknown;
}

export type ValidateFieldsError = Record<string, ValidateError[]>;

export interface InternalRuleItem {
  field?: string;
  fullField?: string;
  [key: string]: unknown;
}

export type RuleValidator = (
  rule: InternalRuleItem,
  // eslint-disable-next-line @typescript-eslint/no-explicit-any
  value: any,
  callback: (error?: string | Error) => void
) => void | Promise<void>;

export interface RuleItem {
  type?: string;
  required?: boolean;
  pattern?: RegExp | string;
  min?: number;
  max?: number;
  message?: string;
  validator?: RuleValidator;
  [key: string]: unknown;
}

export type Rules = Record<string, RuleItem | RuleItem[]>;

export interface ValidateOption {
  first?: boolean;
  firstFields?: boolean | string[];
  suppressWarning?: boolean;
  [key: string]: unknown;
}

export type ValidateCallback = (
  errors: ValidateError[] | null,
  fields: ValidateFieldsError
) => void;

const isEmpty = (value: unknown): boolean =>
  value === undefined || value === null || value === '';

const runValidator = async (
  rule: RuleItem,
  field: string,
  value: unknown
): Promise<string | undefined> => {
  let captured: string | undefined;
  const callback = (error?: string | Error) => {
    if (error) captured = typeof error === 'string' ? error : error.message;
  };
  try {
    const result = rule.validator!(
      { ...rule, field, fullField: field },
      value,
      callback
    );
    if (result instanceof Promise) {
      await result;
    }
  } catch (error) {
    captured = error instanceof Error ? error.message : String(error);
  }
  return captured;
};

const runRule = async (
  rule: RuleItem,
  field: string,
  value: unknown,
  requiredMessage?: string
): Promise<string | undefined> => {
  // Custom validators own their empty-value handling and run unconditionally.
  if (typeof rule.validator === 'function') {
    return runValidator(rule, field, value);
  }

  const empty = isEmpty(value);

  if (rule.required && empty) {
    return (
      rule.message ?? requiredMessage?.replace('%s', field) ?? `${field} is required`
    );
  }

  // Non-required built-in rules don't validate empty values.
  if (empty) return undefined;

  if (rule.type === 'number') {
    if (typeof value !== 'number' || Number.isNaN(value)) return rule.message;
    if (typeof rule.min === 'number' && value < rule.min) return rule.message;
    if (typeof rule.max === 'number' && value > rule.max) return rule.message;
    return undefined;
  }

  // type 'string' or any rule carrying length/pattern constraints.
  if (
    rule.type === 'string' ||
    typeof rule.min === 'number' ||
    typeof rule.max === 'number' ||
    rule.pattern != null
  ) {
    const str = String(value);
    if (typeof rule.min === 'number' && str.length < rule.min) return rule.message;
    if (typeof rule.max === 'number' && str.length > rule.max) return rule.message;
    if (rule.pattern != null) {
      const re =
        rule.pattern instanceof RegExp ? rule.pattern : new RegExp(rule.pattern);
      if (!re.test(str)) return rule.message;
    }
  }

  return undefined;
};

export default class Schema {
  private readonly rules: Rules;
  private requiredMessage?: string | undefined;

  constructor(descriptor: Rules) {
    this.rules = descriptor;
  }

  // Mirrors async-validator's `messages()`. Only the `required` template (with
  // a `%s` placeholder for the field name) is consumed by this minimal schema.
  messages(messages: { required?: string }): this {
    this.requiredMessage = messages.required;
    return this;
  }

  // Mirrors async-validator's callback form. Always resolves (never rejects);
  // callers (validators/shared.ts) inspect the `errors` argument.
  async validate(
    source: Record<string, unknown>,
    _options?: ValidateOption,
    callback?: ValidateCallback
  ): Promise<void> {
    const fields: ValidateFieldsError = {};
    const errors: ValidateError[] = [];

    for (const field of Object.keys(this.rules)) {
      const ruleDef = this.rules[field];
      if (ruleDef === undefined) continue;
      const ruleList = Array.isArray(ruleDef) ? ruleDef : [ruleDef];
      const value = source[field];

      for (const rule of ruleList) {
        const message = await runRule(rule, field, value, this.requiredMessage);
        if (message !== undefined) {
          const error: ValidateError = { message, field, fieldValue: value };
          (fields[field] ??= []).push(error);
          errors.push(error);
        }
      }
    }

    callback?.(errors.length > 0 ? errors : null, fields);
  }
}
