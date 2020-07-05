import React from 'react';
import { Features } from './types';

export interface ApplicationContext {
  features: Features;
}

const ApplicationContextDefaultValue = {} as ApplicationContext
export const ApplicationContext = React.createContext(
  ApplicationContextDefaultValue
);

export function withAuthenticatedContexApplicationContext<T extends ApplicationContext>(Component: React.ComponentType<T>) {
  return class extends React.Component<Omit<T, keyof ApplicationContext>> {
    render() {
      return (
        <ApplicationContext.Consumer>
          {authenticatedContext => <Component {...this.props as T} features={authenticatedContext} />}
        </ApplicationContext.Consumer>
      );
    }
  };
}
