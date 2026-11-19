import type { FC } from 'react';

import './Field.css';
import Section, { type SectionProps } from './Section';

export type FormSectionProps = SectionProps;

// a card with a title and a two-column grid of fields (one column on phones)
const FormSection: FC<FormSectionProps> = ({ children, ...rest }) => (
  <Section {...rest}>
    <div className="form">{children}</div>
  </Section>
);

export default FormSection;
