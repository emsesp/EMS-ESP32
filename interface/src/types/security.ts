export interface UserType {
  username: string;
  password: string;
  admin: boolean;
}

export interface SecuritySettingsType {
  users: UserType[];
}

export interface ApiKeyType {
  id: string;
  label: string;
  write: boolean;
  created: number; // unix time, 0 if unknown
}

// which factory passwords are still in use, see /rest/setup
export interface SetupStateType {
  admin: boolean;
  ap: boolean;
  ap_required: boolean;
}

export interface SetupType {
  password: string;
  ap_password: string;
  view_username: string;
  view_password: string;
}
