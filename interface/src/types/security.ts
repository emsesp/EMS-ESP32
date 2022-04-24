export interface User {
  id: string; // needed for Table
  username: string;
  password: string;
  admin: boolean;
}

export interface SecuritySettings {
  users: User[];
  jwt_secret: string;
}

export interface Token {
  token: string;
}
