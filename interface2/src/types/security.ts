export interface User {
  username: string;
  password: string;
  admin: boolean;
}

export interface SecuritySettings {
  users: User[];
  jwt_secret: string;
}
