export interface UserType {
  username: string;
  password: string;
  admin: boolean;
}

export interface SecuritySettingsType {
  users: UserType[];
  jwt_secret: string;
}

export interface ApiKeyType {
  id: string;
  label: string;
  write: boolean;
  created: number; // unix time, 0 if unknown
}
