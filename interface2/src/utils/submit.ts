export const onEnterCallback = (callback: () => void): (event: React.KeyboardEvent) => void => (
  (event: React.KeyboardEvent) => {
    if (event.key === 'Enter') {
      callback();
      event.preventDefault();
    }
  }
);

