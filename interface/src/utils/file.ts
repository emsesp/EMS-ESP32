export const saveFile = (
  json: unknown,
  filename: string,
  extension: string
): void => {
  try {
    const blob = new Blob([JSON.stringify(json, null, 2)], {
      type: 'application/json'
    });
    const url = URL.createObjectURL(blob);
    const anchor = document.createElement('a');
    anchor.href = url;
    anchor.download = `emsesp_${filename}${extension}`;

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
    throw new Error(`Unable to save file: ${filename}${extension}`);
  }
};
