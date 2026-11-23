import { Button, Message } from 'components/ui';
import { resumeDemo, useDemoPaused } from 'utils';

// Hosted demo only: tells the visitor why values stopped updating.
// English only on purpose: it never ships in device firmware.
const DemoPausedBanner = () => {
  const paused = useDemoPaused();
  if (!paused) return null;

  return (
    <Message
      level="info"
      actions={
        <Button icon="refresh" onClick={resumeDemo}>
          Resume
        </Button>
      }
    >
      Live updates are paused while the demo is idle. Move the mouse or click Resume
      to continue.
    </Message>
  );
};

export default DemoPausedBanner;
