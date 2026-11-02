// the device answers 428 when the password has to be entered again before a change to the firmware,
// the users or the security settings. PasswordConfirmDialog registers the prompt

export const PASSWORD_REQUIRED = 428;

type Prompt = () => Promise<boolean>;

let prompt: Prompt | undefined;
let cancelledMessage = 'Password not confirmed';
let pending: Promise<boolean> | undefined;

export const setPasswordPrompt = (newPrompt?: Prompt, cancelled?: string) => {
  prompt = newPrompt;
  if (cancelled) {
    cancelledMessage = cancelled;
  }
};

export const passwordCancelledMessage = () => cancelledMessage;

// resolves true once the password was confirmed. Requests that need it at the same time share one dialog
export const askPassword = (): Promise<boolean> => {
  if (!prompt) {
    return Promise.resolve(false);
  }
  pending ??= prompt().finally(() => {
    pending = undefined;
  });
  return pending;
};
