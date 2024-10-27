export const saveFile = (json: unknown, filename: string, extension: string) => {
  const anchor = document.createElement('a');
  anchor.href = URL.createObjectURL(
    new Blob([JSON.stringify(json, null, 2)], {
      type: 'text/plain'
    })
  );
  anchor.download = 'emsesp_' + filename + extension;
  anchor.click();
  URL.revokeObjectURL(anchor.href);
};
