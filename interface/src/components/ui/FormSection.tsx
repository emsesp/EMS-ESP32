import type { FC, ReactNode } from 'react';

import './Field.css';
import Section, { type SectionProps } from './Section';

export interface FormSectionProps extends SectionProps {
  // a line of text under the title
  desc?: ReactNode;
}

// a card with a title and a two-column grid of fields (one column on phones)
const FormSection: FC<FormSectionProps> = ({ desc, children, ...rest }) => (
  <Section {...rest}>
    {desc && <p className="cap form-desc">{desc}</p>}
    <div className="form">{children}</div>
  </Section>
);

export default FormSection;
