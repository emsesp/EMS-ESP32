export const saveBlob = (blob: Blob, filename: string): void => {
  try {
    const url = URL.createObjectURL(blob);
    const anchor = document.createElement('a');
    anchor.href = url;
    anchor.download = filename;

    // Trigger download
    document.body.appendChild(anchor);
    anchor.click();
    document.body.removeChild(anchor);

    // Delay revocation to ensure download starts properly
    setTimeout(() => {
      URL.revokeObjectURL(url);
    }, 100);
  } catch (error) {
    console.error('Failed to save file:', error);
    throw new Error(`Unable to save file: ${filename}`, {
      cause: error
    });
  }
};

export const saveFile = (
  json: unknown,
  filename: string,
  extension: string
): void => {
  const blob = new Blob([JSON.stringify(json, null, 2)], {
    type: 'application/json'
  });
  saveBlob(blob, `emsesp_${filename}${extension}`);
};
