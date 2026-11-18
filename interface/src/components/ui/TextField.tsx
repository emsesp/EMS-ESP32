import {
  type FC,
  type InputHTMLAttributes,
  type ReactNode,
  useId,
  useState
} from 'react';

import { useI18nContext } from 'i18n/i18n-react';

import FieldFlags from './FieldFlags';
import './Field.css';
import IconButton from './IconButton';
import { cx } from './cx';

export interface TextFieldProps extends Omit<
  InputHTMLAttributes<HTMLInputElement>,
  'size'
> {
  label: string;
  name: string;
  error?: string | undefined;
  helperText?: ReactNode;
  fullWidth?: boolean;
  // shown after the input, e.g. "s" or "KB"
  unit?: string | undefined;
  mono?: boolean;
  changed?: boolean | undefined;
  restart?: boolean | undefined;
  // e.g. a button after the input
  action?: ReactNode;
}

// type="password" adds a button that shows the password
const TextField: FC<TextFieldProps> = ({
  label,
  error,
  helperText,
  fullWidth,
  type = 'text',
  unit,
  mono,
  changed,
  restart,
  action,
  className,
  id,
  ...rest
}) => {
  const { LL } = useI18nContext();
  const autoId = useId();
  const inputId = id ?? autoId;
  const [shown, setShown] = useState(false);
  const help = error ?? helperText;

  return (
    <div
      className={cx(
        'field',
        fullWidth && 'full',
        error && 'invalid',
        rest.disabled && 'disabled',
        className
      )}
    >
      <label htmlFor={inputId}>
        {label}
        <FieldFlags restart={restart} changed={changed} />
      </label>
      <div className="input-wrap">
        <input
          id={inputId}
          className={cx('input', type === 'password' && 'has-btn', mono && 'mono')}
          type={type === 'password' && shown ? 'text' : type}
          aria-invalid={error ? true : undefined}
          aria-describedby={help ? `${inputId}-help` : undefined}
          {...rest}
        />
        {type === 'password' && (
          <IconButton
            plain
            icon={shown ? 'visibility' : 'visibility_off'}
            label={LL.SHOW_PASSWORD()}
            aria-pressed={shown}
            disabled={rest.disabled}
            onClick={() => setShown((s) => !s)}
          />
        )}
        {unit && <span className="unit">{unit}</span>}
        {action}
      </div>
      {help && (
        <span id={`${inputId}-help`} className={cx('cap', error && 'err')}>
          {help}
        </span>
      )}
    </div>
  );
};

export default TextField;
