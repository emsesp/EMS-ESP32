export interface UserType {
  username: string;
  password: string;
  admin: boolean;
}

export interface SecuritySettingsType {
  users: UserType[];
  jwt_secret: string;
}

export interface Token {
  token: string;
}
